#pragma once

namespace xbitmap_details
{
    //------------------------------------------------------------------------------
    // Description:
    //      Computes the Log2 of an integral value. 
    //      It answer the question: how many bits do I need to rshift 'y' to make this expression true: 
    //      (input) x == 1 << 'y'. Assuming x was originally a power of 2.
    //------------------------------------------------------------------------------
    template< typename T> constexpr
    T Log2Int(T x, int p = 0) noexcept
    {
        return (x <= 1) ? static_cast<T>(p) : Log2Int(x >> 1, p + 1);
    }

    //------------------------------------------------------------------------------
    // Description:
    //      Determines the minimum power of two that encapsulates the given number
    // Example:
    //      Log2IntRoundUp(3) == 2 // it takes 2bits to store #3
    //------------------------------------------------------------------------------
    template< typename T> constexpr
    T Log2IntRoundUp( T x ) noexcept
    {
        static_assert( std::is_integral_v<T> ); 
        return x < 1 ? 0 : Log2Int(x) + 1; 
    }

    //------------------------------------------------------------------------------
    template< class T > constexpr
    bool isPowTwo(const T x) noexcept
    {
        static_assert(std::is_integral_v<T>);
        return !((x - 1) & x);
    }
}

//-------------------------------------------------------------------------------

inline
const xbitmap& xbitmap::operator = (xbitmap&& Src) noexcept
{
    // copy all data
    memcpy(this, &Src, sizeof(Src));

    // Make sure to set the ownership of the data to me...
    Src.m_Flags.m_bOwnsMemory = false;
    Src.Kill();

    return *this;
}

//-------------------------------------------------------------------------------

inline
xbitmap::xbitmap(xbitmap&& Src) noexcept
{
    // copy all data
    memcpy(this, &Src, sizeof(Src));

    // Make sure to set the ownership of the data to me...
    Src.m_Flags.m_bOwnsMemory = false;
    Src.Kill();
}

//-------------------------------------------------------------------------------
inline
xbitmap::~xbitmap(void) noexcept
{
    if (m_pData && m_Flags.m_bOwnsMemory) delete m_pData;
}

//-------------------------------------------------------------------------------
inline
void xbitmap::Kill(void) noexcept
{
    if (m_pData && m_Flags.m_bOwnsMemory) delete m_pData;

    m_DataSize      = 0;
    m_FaceSize      = 0;
    m_Height        = 0;
    m_Width         = 0;
    m_Flags.m_Value = 0;
    m_nMips         = 0;
}

//-----------------------------------------------------------------------------------

void xbitmap::setOwnMemory(bool bOwnMemory) noexcept
{
    m_Flags.m_bOwnsMemory = bOwnMemory;
}

//-----------------------------------------------------------------------------------

void xbitmap::setUWrapMode(wrap_mode WrapMode) noexcept
{
    assert(WrapMode != wrap_mode::ENUM_COUNT);
    m_Flags.m_UWrapMode = static_cast<std::uint8_t>(WrapMode);
}

//-----------------------------------------------------------------------------------

void xbitmap::setVWrapMode(wrap_mode WrapMode) noexcept
{
    assert(WrapMode != wrap_mode::ENUM_COUNT);
    m_Flags.m_VWrapMode = static_cast<std::uint8_t>(WrapMode);
}

//-----------------------------------------------------------------------------------

bool xbitmap::isValid(void) const noexcept
{
    return !!m_pData;
}

//-----------------------------------------------------------------------------------

constexpr
bool xbitmap::isLinearSpace(void) const noexcept
{
    return m_Flags.m_bLinearSpace;
}

//-----------------------------------------------------------------------------------
constexpr
std::uint32_t xbitmap::getWidth(void) const noexcept
{
    return m_Width;
}

//-----------------------------------------------------------------------------------

constexpr
std::uint32_t xbitmap::getHeight(void) const noexcept
{
    return m_Height;
}

//-----------------------------------------------------------------------------------

constexpr
xbitmap::format xbitmap::getFormat(void) const noexcept
{
    return m_Flags.m_Format;
}

//-----------------------------------------------------------------------------------

void xbitmap::setFormat(const xbitmap::format Format) noexcept
{
    m_Flags.m_Format = Format;
}

//-----------------------------------------------------------------------------------

void xbitmap::setColorSpace(const xbitmap::color_space ColorSpace) noexcept
{
    m_Flags.m_bLinearSpace = std::uint8_t(ColorSpace);
}

//-----------------------------------------------------------------------------------
constexpr
xbitmap::color_space xbitmap::getColorSpace( void ) const noexcept
{
    return static_cast<xbitmap::color_space>(m_Flags.m_bLinearSpace);
}

//-----------------------------------------------------------------------------------

constexpr
std::uint64_t xbitmap::getFrameSize(void) const noexcept
{
    return m_FaceSize * getFaceCount();
}

//-----------------------------------------------------------------------------------

constexpr
int xbitmap::getFrameCount(void) const noexcept
{
    const auto DataSize = getDataSize() - getMipCount()*sizeof(mip);
    return static_cast<int>(DataSize / getFrameSize());
}

//-----------------------------------------------------------------------------------
constexpr
bool xbitmap::isCubemap( void ) const noexcept
{
    return m_Flags.m_bCubeMap;
}

//-----------------------------------------------------------------------------------

void xbitmap::setCubemap( bool isCubeMap ) noexcept
{
    m_Flags.m_bCubeMap = isCubeMap;
}

//-----------------------------------------------------------------------------------

constexpr
int xbitmap::getFaceCount(void) const noexcept
{
    return m_Flags.m_bCubeMap ? 6 : 1;
}

//-----------------------------------------------------------------------------------
constexpr
std::uint64_t xbitmap::getFaceSize( void ) const noexcept
{
    assert( [&]{ auto fs = ((getFrameSize() / getFaceCount()) * getFaceCount()); return fs == getFrameSize(); }() );
    return getFrameSize() / getFaceCount();
}

//-----------------------------------------------------------------------------------
constexpr   
float xbitmap::getAspectRatio( void ) const noexcept
{
    return m_Width / static_cast<float>(m_Height);
}

//-----------------------------------------------------------------------------------
constexpr
xbitmap::wrap_mode xbitmap::getUWrapMode(void) const noexcept
{
    return static_cast<wrap_mode>(m_Flags.m_UWrapMode);
}

//-----------------------------------------------------------------------------------
constexpr
xbitmap::wrap_mode xbitmap::getVWrapMode(void) const noexcept
{
    return static_cast<wrap_mode>(m_Flags.m_VWrapMode);
}

//-----------------------------------------------------------------------------------

void xbitmap::Copy(const xbitmap& Src) noexcept
{
    CreateFromMips({ &Src, 1u });
}

//-----------------------------------------------------------------------------------

constexpr
std::uint64_t xbitmap::getDataSize(void) const noexcept
{
    return m_DataSize;
}

//-----------------------------------------------------------------------------------

constexpr
int xbitmap::getMipCount(void) const noexcept
{
    return m_nMips;
}

//-----------------------------------------------------------------------------------

xbitmap::xbitmap( std::span<std::byte> Data, std::uint32_t Width, std::uint32_t Height, bool bReleaseWhenDone ) noexcept
: m_pData       { reinterpret_cast<mip*>( Data.data() )         }
, m_DataSize    { Data.size()                                   }
, m_FaceSize    { static_cast<std::uint32_t>( Width * Height * sizeof(xcolori)) }
, m_Height      { static_cast<std::uint16_t>(Height)            }   
, m_Width       { static_cast<std::uint16_t>(Width)             }
, m_Flags       { static_cast<std::uint16_t>( (bReleaseWhenDone ? bit_pack_fields::owns_memory_mask_v : bit_pack_fields::zero_mask_v)
                                              | ( static_cast<std::uint16_t>(xbitmap::format::XCOLOR) << bit_pack_fields::offset_to_format_v )
                                                               )}
, m_nMips       { 1u                                            }
{
    assert( ( static_cast<std::uint64_t>(m_Width) * m_Height * sizeof(xcolori) + sizeof(int) ) == m_DataSize );
}

//-------------------------------------------------------------------------------

std::uint32_t xbitmap::getMipSize( int iMip ) const noexcept
{
    assert( iMip >= 0 );
    assert( iMip < m_nMips );

    const auto NextOffset = ((iMip + 1) == m_nMips ) ? getFaceSize() : static_cast<std::size_t>(m_pData[iMip+1].m_Offset);
    return static_cast<std::uint32_t>(NextOffset - m_pData[iMip].m_Offset);
}

//-------------------------------------------------------------------------------

int xbitmap::getFullMipChainCount( void ) const noexcept
{
    const auto SmallerDimension  = std::min( m_Height, m_Width );
    const auto nMips             = xbitmap_details::Log2IntRoundUp( SmallerDimension ) + 1;
    return nMips;
}

//-------------------------------------------------------------------------------

const void* xbitmap::getMipPtr(const int iMip, const int iFace, const int iFrame ) const noexcept
{
    assert(m_Width > 0);
    assert(m_Height > 0);
    assert(m_pData);
    assert(iMip < m_nMips);
    assert(iMip >= 0);
    assert(iFrame >= 0);
    assert(iFrame < getFrameCount());
    assert(iFace >= 0);
    assert(iFace < getFaceCount());

    auto FinalOffest = m_pData[iMip].m_Offset + iFrame * getFrameSize() + iFace * getFaceSize();
    return &reinterpret_cast<const std::byte*>(&m_pData[m_nMips])[FinalOffest];
}

//-------------------------------------------------------------------------------

void* xbitmap::getMipPtr( const int iMip, const int iFace, const int iFrame ) noexcept
{
    assert( m_Width  > 0 );
    assert( m_Height > 0 );
    assert( m_pData );
    assert( iMip < m_nMips );
    assert( iMip >= 0 );
    assert( iFrame >= 0 );
    assert( iFrame < getFrameCount() );
    assert( iFace >= 0 );
    assert( iFace < getFaceCount() );

    auto FinalOffest = m_pData[iMip].m_Offset + iFrame * getFrameSize() + iFace * getFaceSize();
    return &reinterpret_cast<std::byte*>(&m_pData[m_nMips])[FinalOffest];
}

//-------------------------------------------------------------------------------

template< typename T >
std::span<T> xbitmap::getMip( const int iMip, const int iFace, const int iFrame ) noexcept
{
    return { reinterpret_cast<T*>(getMipPtr(iMip, iFace, iFrame)), getMipSize(iMip) / sizeof(T) };
}

//-------------------------------------------------------------------------------

template< typename T >
std::span< const T> xbitmap::getMip(const int iMip, const int iFace, const int iFrame) const noexcept
{
    return { reinterpret_cast<const T*>(getMipPtr(iMip, iFace, iFrame)), getMipSize(iMip) / sizeof(T) };
}

//-------------------------------------------------------------------------------

constexpr
bool xbitmap::isSquare( void ) const noexcept
{
    assert(m_Width > 0);
    assert(m_Height > 0);
    return m_Width == m_Height;
}

//-------------------------------------------------------------------------------
constexpr
bool xbitmap::isPowerOfTwo( void ) const noexcept
{
    assert( m_Width > 0 );
    assert( m_Height > 0 );
    return xbitmap_details::isPowTwo( m_Width ) && xbitmap_details::isPowTwo( m_Height );
}
