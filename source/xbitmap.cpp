#include "xbitmap.h"

#include <stdio.h>
#include <wchar.h>
#include <format>

namespace xbitmap_details
{
    constexpr std::uint32_t s_DefaultBitmapSize = 256u; 
    using t_default_data = std::array<xcolori, s_DefaultBitmapSize*s_DefaultBitmapSize + 1 >;

    static const t_default_data Data = []() constexpr noexcept
    {
        t_default_data Data = {};
        const xcolori C1{ 128, 128, 128, 255 };
        const xcolori C2{ 187, 187, 187, 255 };
        const xcolori CT[] = { xcolori{ 187,  50,  50, 255 }
                                  , xcolori{  50, 187,  50, 255 }
                                  , xcolori{  50,  50, 187, 255 }
                                   };
        const auto          nCheckers    = 16;
        const auto          CheckerSize  = s_DefaultBitmapSize/nCheckers;

        xcolori* pData = &Data[1];

        // Create basic checker pattern
        for( auto y = 0u; y < s_DefaultBitmapSize; y++ )
        {
            for(auto x = 0u; x < s_DefaultBitmapSize; x++)
            {
                // Create the checker pattern
                pData[ x+ s_DefaultBitmapSize*y ] = ((y&CheckerSize)==CheckerSize)^((x&CheckerSize)==CheckerSize)?C1:C2;
            }
        }
        
        // Draw a simple arrows at the top left pointing up...
        const int ArrowSize = CheckerSize*2;
        for( auto k=0; k<3; k++ )
        {
            auto yy = 1u;
            for( auto y=1u; y<(ArrowSize -1); ++y )
            {
                for( auto x = yy; x < (ArrowSize -1)-yy; x++)
                {
                    if( k&1) pData[ k* ArrowSize + x + s_DefaultBitmapSize*(ArrowSize -y - 1) ] = CT[k];
                    else     pData[ k* ArrowSize + x + s_DefaultBitmapSize*(ArrowSize -y - 1) ] = CT[k];
                }
                    
                if (y&1) yy++;
            }
        }
        
        return Data; 
    }();

    static const auto s_DefaultBitmap = []() noexcept
    {
        xbitmap Bitmap{{ const_cast<std::byte*>(reinterpret_cast<const std::byte*>(Data.data())), Data.size()*sizeof(Data[1])}, s_DefaultBitmapSize, s_DefaultBitmapSize, false};
        Bitmap.setUWrapMode(xbitmap::wrap_mode::WRAP);
        Bitmap.setVWrapMode(xbitmap::wrap_mode::WRAP);
        Bitmap.setColorSpace(xbitmap::color_space::SRGB);
        return Bitmap;
    }();
}

//////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////

namespace xbitmap_details
{
    //-------------------------------------------------------------------------------
    static std::string wstring_to_utf8(const std::wstring& wstr)
    {
        std::string result;
        for (std::uint32_t wc : wstr) 
        {
            if (wc <= 0x7F) 
            {
                result += static_cast<char>(wc);
            }
            else if (wc <= 0x7FF) 
            {
                result += static_cast<char>(0xC0 | ((wc >> 6) & 0x1F));
                result += static_cast<char>(0x80 | (wc & 0x3F));
            }
            else if (wc <= 0xFFFF)
            {
                result += static_cast<char>(0xE0 | ((wc >> 12) & 0x0F));
                result += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (wc & 0x3F));
            }
            else if constexpr (sizeof(wchar_t) == 4)
            {
                if (wc <= 0x10FFFF)
                {
                    result += static_cast<char>(0xF0 | ((wc >> 18) & 0x07));
                    result += static_cast<char>(0x80 | ((wc >> 12) & 0x3F));
                    result += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (wc & 0x3F));
                }
                else
                {
                    throw std::runtime_error("Invalid Unicode code point");
                }
            }
            else 
            {
                throw std::runtime_error("Invalid Unicode code point");
            }
        }
        return result;
    }

    //-------------------------------------------------------------------------------

    static void HandleError( std::wstring_view FileName, int Err ) noexcept
    {
        std::array<wchar_t, 256> ErrString;
        _wcserror_s(ErrString.data(), ErrString.size(), Err);
        xerr::LogMessage<xerr::default_states::FAILURE>(wstring_to_utf8(std::format(L"Error Code: {} With Error: {} for file: {}", Err, ErrString.data(), FileName)));
    }
}

//-------------------------------------------------------------------------------

xerr xbitmap::Load( const std::wstring_view FileName ) noexcept
{
    Kill();

    FILE* fp;
    if( auto Err = _wfopen_s(&fp, std::wstring(FileName).c_str(), L"rb"); Err )
    {
        xbitmap_details::HandleError(FileName, Err);
        return xerr::create_f<xerr::default_states, "Fail to open file">();
    }
        

    //
    // Read the signature
    //
    {
        std::uint32_t Signature;
        if( 1 != fread( &Signature, sizeof(Signature), 1, fp ) ) 
        {
            fclose(fp);
            xbitmap_details::HandleError(FileName, errno);
            return xerr::create_f<xerr::default_states, "Fail to read the signature of the file">();
        }

        if( Signature != std::uint32_t('XBMP') )
        {
            fclose(fp);
            return xerr::create_f< xerr::default_states, "Wrong file signature">();
        }
    }

    if ( false
        || (1 != fread(&m_DataSize,         sizeof(m_DataSize),         1, fp))
        || (1 != fread(&m_FaceSize,         sizeof(m_FaceSize),         1, fp))
        || (1 != fread(&m_Height,           sizeof(m_Height),           1, fp))
        || (1 != fread(&m_Width,            sizeof(m_Width),            1, fp))
        || (1 != fread(&m_Flags.m_Value,    sizeof(m_Flags.m_Value),    1, fp))
        || (1 != fread(&m_nMips,            sizeof(m_nMips),            1, fp))
        || (1 != fread(&m_ClampColor.m_R,   sizeof(m_ClampColor.m_R),   1, fp))
        || (1 != fread(&m_ClampColor.m_G,   sizeof(m_ClampColor.m_G),   1, fp))
        || (1 != fread(&m_ClampColor.m_B,   sizeof(m_ClampColor.m_B),   1, fp))
        || (1 != fread(&m_ClampColor.m_A,   sizeof(m_ClampColor.m_A),   1, fp))
        )
    {
        fclose(fp);
        xbitmap_details::HandleError(FileName, errno);
        return xerr::create_f<xerr::default_states, "Fail to read in file">();
    }

    //
    // Read the big data
    //
    m_pData = reinterpret_cast<mip*>(new std::byte[ m_DataSize ] );
    if (1 != fread(m_pData, m_DataSize, 1, fp))
    {
        fclose(fp);
        xbitmap_details::HandleError(FileName, errno);
        return xerr::create_f<xerr::default_states, "Fail to read data in file">();
    }

    fclose(fp);
    return {};
}

//-------------------------------------------------------------------------------

xerr xbitmap::Save( const std::wstring_view FileName ) const noexcept
{
    FILE* fp;
    
    if (auto Err = _wfopen_s(&fp, std::wstring(FileName).c_str(), L"wb"); Err )
    {
        xbitmap_details::HandleError(FileName, Err);
        return xerr::create_f<xerr::default_states, "Fail to open file">();
    }

    static constexpr std::uint32_t Signature('XBMP');

    if( false
        || (1 != fwrite(&Signature,         sizeof(std::uint32_t),      1, fp))
        || (1 != fwrite(&m_DataSize,        sizeof(m_DataSize),         1, fp))
        || (1 != fwrite(&m_FaceSize,        sizeof(m_FaceSize),         1, fp))
        || (1 != fwrite(&m_Height,          sizeof(m_Height),           1, fp))
        || (1 != fwrite(&m_Width,           sizeof(m_Width),            1, fp))
        || (1 != fwrite(&m_Flags.m_Value,   sizeof(m_Flags.m_Value),    1, fp))
        || (1 != fwrite(&m_nMips,           sizeof(m_nMips),            1, fp))
        || (1 != fwrite(&m_ClampColor.m_R,  sizeof(m_ClampColor.m_R),   1, fp))
        || (1 != fwrite(&m_ClampColor.m_G,  sizeof(m_ClampColor.m_G),   1, fp))
        || (1 != fwrite(&m_ClampColor.m_B,  sizeof(m_ClampColor.m_B),   1, fp))
        || (1 != fwrite(&m_ClampColor.m_A,  sizeof(m_ClampColor.m_A),   1, fp))
        || (1 != fwrite(m_pData,            m_DataSize,                 1, fp))
        )
    {
        fclose(fp);
        xbitmap_details::HandleError(FileName, errno);
        return xerr::create_f<xerr::default_states, "Fail to write data to file">();
    }

    fclose(fp);
    return {};
}

//-------------------------------------------------------------------------------

xerr xbitmap::SaveTGA(const std::wstring_view FileName) const noexcept
{
    std::array< std::byte, 18> Header;

    // The format of this picture must be in color format
    assert( getFormat() == xbitmap::format::R8G8B8A8 
         || getFormat() == xbitmap::format::B8G8R8A8
         || getFormat() == xbitmap::format::R8G8B8U8
         || getFormat() == xbitmap::format::B8G8R8U8);

    // Build the header information.
    std::memset(Header.data(), 0, Header.size());
    Header[2]  = std::byte{2u};     // Image type.
    Header[12] = static_cast<std::byte>((getWidth() >> 0) & 0xFF);
    Header[13] = static_cast<std::byte>((getWidth() >> 8) & 0xFF);
    Header[14] = static_cast<std::byte>((getHeight() >> 0) & 0xFF);
    Header[15] = static_cast<std::byte>((getHeight() >> 8) & 0xFF);
    Header[16] = std::byte{ 32u };    // Bit depth.
    Header[17] = std::byte{ 32u };    // NOT flipped vertically.

    // Open the file.
    FILE* fp;
    if (auto Err = _wfopen_s( &fp, std::wstring(FileName).c_str(), L"wb"); Err )
    {
        fclose(fp);
        xbitmap_details::HandleError(FileName, Err);
        return xerr::create_f<xerr::default_states, "Fail to open tga file">();
    }

    // Write out the data.
    if( 1 != fwrite(Header.data(), Header.size(), 1, fp ))
    {
        fclose(fp);
        xbitmap_details::HandleError(FileName, errno);
        return xerr::create_f<xerr::default_states, "Fail to write tga header file">();
    }

    //
    // Convert to what tga expects as a color
    //
    const xcolori* pColor = getMip<xcolori>(0).data();
    const auto     Size   = getWidth() * getHeight();

    if( getFormat() == xbitmap::format::B8G8R8A8 || 
        getFormat() == xbitmap::format::B8G8R8U8 )
    {
        if ( 1 != fwrite(m_pData, getFrameSize(), 1, fp ) )
        {
            fclose(fp);
            xbitmap_details::HandleError(FileName, errno);
            return xerr::create_f<xerr::default_states, "Fail to write tga data to file">();
        }
    }
    else
    {
        auto Convert = std::make_unique<xcolori[]>(Size);
        for( auto i = 0u; i < Size; ++i )
        {
            auto Color = pColor[i];
            std::swap(Color.m_R, Color.m_B);
            Convert[i] = Color;
        }

        if ( 1 != fwrite(Convert.get(), static_cast<std::size_t>(Size), 1, fp) )
        {
            fclose(fp);
            xbitmap_details::HandleError(FileName, errno);
            return xerr::create_f<xerr::default_states, "Fail to write tga data to file">();
        }
    }

    fclose(fp);
    return {};
}

//-------------------------------------------------------------------------------

void xbitmap::setup
( const std::uint32_t       Width                   
, const std::uint32_t       Height
, const xbitmap::format     BitmapFormat            
, const std::uint64_t       FaceSize
, std::span<std::byte>      Data                    
, const bool                bFreeMemoryOnDestruction
, const int                 nMips                   
, const int                 nFrames
, const bool                isCubeMap
) noexcept
{
    assert( Data.size()       >  4 );
    assert( FaceSize          >  0 );
    assert( FaceSize          <  Data.size() );    // In fact this should be equal to: DataSize - ((nMips*sizeof(s32)) * nFrames) which means it should be removed
    assert( FaceSize          <= 0xffffffff );     // Since we are going to pack it into a u32 it can not be larger than that
    assert( nMips             >  0 );
    assert( nFrames           >  0 );
    assert( Width             >  0 );
    assert( Height            >  0 );
    assert((int)BitmapFormat  > (int)xbitmap::format::INVALID );
    assert((int)BitmapFormat  < (int)xbitmap::format::ENUM_COUNT );

    Kill();

    m_Flags.m_bCubeMap    = isCubeMap;
    m_pData               = reinterpret_cast<mip*>(Data.data());
    m_Flags.m_bOwnsMemory = bFreeMemoryOnDestruction;

    m_DataSize          = Data.size();    
    
    m_FaceSize          = static_cast<std::uint32_t>(FaceSize);
    m_Height            = Height;   
    m_Width             = Width;
    
    m_nMips             = nMips;    
    m_Flags.m_Format    = BitmapFormat;

    assert( getFrameSize() == (m_DataSize / nFrames - nMips * sizeof(int)));
    assert( [&]{auto t = getFaceCount() * getFaceSize(); return t == getFrameSize(); }() );
    assert( nFrames == getFrameCount() );
}

//-------------------------------------------------------------------------------

void xbitmap::setupFromColor
( const std::uint32_t       Width  
, const std::uint32_t       Height 
, std::span<xcolori>        Data   
, const bool                bFreeMemoryOnDestruction        
) noexcept
{
    setup
    ( Width
    , Height
    , format::XCOLOR
    , sizeof(xcolori)*( Data.size() - 1 )
    , { reinterpret_cast<std::byte*>(&Data[0]), sizeof(xcolori)*Data.size() }
    , bFreeMemoryOnDestruction
    , 1
    , 1 
    );
}

//-----------------------------------------------------------------------------------

bool xbitmap::isSigned(void) const noexcept
{
    constexpr static auto SignedTable = []() constexpr noexcept
    {
        std::array< bool, static_cast<std::size_t>(format::ENUM_COUNT) > Table = {};

        Table[static_cast<int>(format::BC6H_8RGB_SFLOAT)] = true;

        return Table;
    }();

    return SignedTable[static_cast<int>(m_Flags.m_Format)];
}

//-------------------------------------------------------------------------------

bool xbitmap::hasAlphaChannel( void ) const noexcept
{
    constexpr static auto SupportAlphaTable = []() constexpr noexcept
    {
        std::array< bool, static_cast<std::size_t>(format::ENUM_COUNT) > Table = {};

        Table[ static_cast<int>(format::R4G4B4A4)              ] = true  ;
        Table[ static_cast<int>(format::R5G6B5)                ] = false ;
        Table[ static_cast<int>(format::B5G5R5A1)              ] = true  ;
        Table[ static_cast<int>(format::R8G8B8)                ] = false ;
        Table[ static_cast<int>(format::R8G8B8U8)              ] = false ;
        Table[ static_cast<int>(format::R8G8B8A8)              ] = true  ;
        Table[ static_cast<int>(format::B8G8R8A8)              ] = true  ;
        Table[ static_cast<int>(format::B8G8R8U8)              ] = false ;
        Table[ static_cast<int>(format::A8R8G8B8)              ] = true  ;
        Table[ static_cast<int>(format::U8R8G8B8)              ] = false ;

        Table[ static_cast<int>(format::PAL4_R8G8B8A8)         ] = true  ;
        Table[ static_cast<int>(format::PAL8_R8G8B8A8)         ] = true  ;
                                                                  
        Table[ static_cast<int>(format::ETC2_4RGB)             ] = false ;
        Table[ static_cast<int>(format::ETC2_4RGBA1)           ] = false ;
        Table[ static_cast<int>(format::ETC2_8RGBA)            ] = true  ;
                                                                  
        Table[ static_cast<int>(format::BC1_4RGB)              ] = false ;
        Table[ static_cast<int>(format::BC1_4RGBA1)            ] = true  ;
        Table[ static_cast<int>(format::BC2_8RGBA)             ] = true  ;
        Table[ static_cast<int>(format::BC3_8RGBA)             ] = true  ;
        Table[ static_cast<int>(format::BC3_81Y0X_NORMAL)      ] = false ;
        Table[ static_cast<int>(format::BC4_4R)                ] = false ;
        Table[ static_cast<int>(format::BC5_8RG)               ] = false ;
        Table[ static_cast<int>(format::BC5_8YX_NORMAL)        ] = false ;
        Table[ static_cast<int>(format::BC6H_8RGB_SFLOAT)      ] = false ;
        Table[ static_cast<int>(format::BC6H_8RGB_UFLOAT)      ] = false ;
        Table[ static_cast<int>(format::BC7_8RGBA)             ] = false ;
                                                                  
        Table[ static_cast<int>(format::ASTC_4x4_8RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_5x4_6RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_5x5_5RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_6x5_4RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_6x6_4RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_8x5_3RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_8x6_3RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_8x8_2RGB)         ] = false ;
        Table[ static_cast<int>(format::ASTC_10x5_3RGB)        ] = false ;
        Table[ static_cast<int>(format::ASTC_10x6_2RGB)        ] = false ;
        Table[ static_cast<int>(format::ASTC_10x8_2RGB)        ] = false ;
        Table[ static_cast<int>(format::ASTC_10x10_1RGB)       ] = false ;
        Table[ static_cast<int>(format::ASTC_12x10_1RGB)       ] = false ;
        Table[ static_cast<int>(format::ASTC_12x12_1RGB)       ] = false ;
                                                                  
        Table[ static_cast<int>(format::PVR1_2RGB)             ] = false ;
        Table[ static_cast<int>(format::PVR1_2RGBA)            ] = true  ;
        Table[ static_cast<int>(format::PVR1_4RGB)             ] = false ;
        Table[ static_cast<int>(format::PVR1_4RGBA)            ] = true  ;
        Table[ static_cast<int>(format::PVR2_2RGBA)            ] = true  ;
        Table[ static_cast<int>(format::PVR2_4RGBA)            ] = true  ;
                                                                  
        Table[ static_cast<int>(format::D24S8_FLOAT)           ] = false ;
        Table[ static_cast<int>(format::D24S8)                 ] = false ;
        Table[ static_cast<int>(format::R8)                    ] = false ;
        Table[ static_cast<int>(format::R32)                   ] = false ;
        Table[ static_cast<int>(format::R8G8)                  ] = false ;
        Table[ static_cast<int>(format::R16G16B16A16)          ] = true  ;
        Table[ static_cast<int>(format::R16G16B16A16_FLOAT)    ] = true  ;
        Table[ static_cast<int>(format::A2R10G10B10)           ] = true  ;
        Table[ static_cast<int>(format::B11G11R11_FLOAT)       ] = false ;

        Table[ static_cast<int>(format::R32G32B32A32_FLOAT)    ] = true  ;

        return Table;
    }();

    // Must insert this format into the table and assign avalue to it
    return SupportAlphaTable[static_cast<int>(m_Flags.m_Format)];
}

//-------------------------------------------------------------------------------

bool xbitmap::ComputeHasAlphaInfo( void ) const noexcept
{
    //
    // Check all the pixels to see if they have information other than the default
    //
    
    // We can handle anything with the compress formats and such
    assert( (int)m_Flags.m_Format < (int)format::XCOLOR_END );
    
    const auto  Format = xcolor::format{ static_cast<xcolor::format::type>(m_Flags.m_Format) };
    const auto& Dest   = Format.getDescriptor();

    if( Dest.m_TB == 16 )
    {
        auto            X = 0u;
        xcolori         C1;
        auto            pData = static_cast<const std::uint16_t*>( getMipPtr(0,0,0) );
        for( auto y=0u; y<m_Height; ++y )
        for( auto x=0u; x<m_Width;  ++x )
        {
            auto Data = pData[ x + y*m_Width ];
            C1 = xcolori( Data, Format );
            
            X |= (C1.m_A == 0xff);
            X |= (C1.m_A == 0x00)<<1;
            if( X == 1 || X == 2 )
                continue;
            
            return true;
        }
    }
    else
    {
        auto            X=0u;
        xcolori         C1;
        auto            pData = static_cast<const std::uint32_t*>(getMipPtr(0,0,0));
        for( auto y=0u; y<m_Height; ++y )
        for( auto x=0u; x<m_Width;  ++x )
        {
            auto Data = pData[ x + y*m_Width ];
            C1 = xcolori( Data, Format );
            
            X |= (C1.m_A == 0xff);
            X |= (C1.m_A == 0x00)<<1;
            if( X == 1 || X == 2 )
                continue;
            
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------

void xbitmap::setDefaultTexture( void ) noexcept
{
    Kill();
    std::memcpy( this, &xbitmap_details::s_DefaultBitmap, sizeof(*this) );
}

//-------------------------------------------------------------------------------

const xbitmap& xbitmap::getDefaultBitmap( void ) noexcept
{
    return xbitmap_details::s_DefaultBitmap;
}

//-------------------------------------------------------------------------------

void xbitmap::CreateBitmap( const std::uint32_t Width, const std::uint32_t Height ) noexcept
{
    assert( Width  >= 1 );
    assert( Height >= 1 );
    
    // Allocate the necesary data
    const auto  Size = 1 + Width * Height;
    auto        Data = std::make_unique<xcolori[]>(Size);
    
    // Initialize the offset table
    Data[0].m_Value = 0;
    
    setupFromColor
    ( Width 
    , Height 
    , { Data.release(), Size }
    , true 
    );
}

//-------------------------------------------------------------------------------
/*
bool xbitmap::SaveTGA( const xstring FileName ) const noexcept
{
    xbyte   Header[18];
    
    // The format of this picture must be in color format
    ASSERT( m_Format == FORMAT_XCOLOR || m_Format == FORMAT_R8G8B8U8 );
    
    // Build the header information.
    x_memset( Header, 0, 18 );
    Header[ 2] = 2;     // Image type.
    Header[12] = (getWidth()  >> 0) & 0xFF;
    Header[13] = (getWidth()  >> 8) & 0xFF;
    Header[14] = (getHeight() >> 0) & 0xFF;
    Header[15] = (getHeight() >> 8) & 0xFF;
    Header[16] = 32;    // Bit depth.
    Header[17] = 32;    // NOT flipped vertically.
    
    // Open the file.
    xfile File;
    
    if( !File.Open( FileName, "wb" ) )
    {
        return FALSE;
    }
    
    // Write out the data.
    File.WriteRaw( Header, 1, 18 );
    
    //
    // Convert to what tga expects as a color
    //
    const xcolor* pColor = (xcolor*)getMip(0);
    const s32     Size   = getWidth() * getHeight();
    
    for( s32 i=0; i<Size; ++i )
    {
        xcolor Color = pColor[i];
        x_Swap( Color.m_R, Color.m_B );
        File.WriteRaw( &Color, 4, 1 );
    }
    
    return true;
}
*/

//-------------------------------------------------------------------------------

void xbitmap::CreateFromMips( std::span<const xbitmap> MipList ) noexcept
{
    //
    // Compute the total size
    //
    std::uint64_t TotalSize = 0;
    for( auto i=0u; i<MipList.size(); i++ )
    {
        const auto& Mip = MipList[i];
        
        TotalSize += Mip.m_DataSize;
        
        assert( Mip.m_nMips   == 1 );
    }
    
    //
    // Allocate data
    //
    auto   BaseData     = std::make_unique<std::byte[]>( TotalSize );
    auto   pOffsetTable = reinterpret_cast<mip*>(BaseData.get());
    auto   pData        = reinterpret_cast<std::byte*>(&pOffsetTable[MipList.size()]);
    
    //
    // Copy the actual data
    //
    {
        std::uint64_t TotalOffset=0;
        for( auto i=0u; i<MipList.size(); ++i )
        {
            const auto&     Mip         = MipList[i];
            auto&           Offset      = pOffsetTable[i];
            const auto      MipDataSize = Mip.m_DataSize - sizeof(mip);
            
            Offset = {static_cast<int>( TotalOffset )};
            
            // Copy Data
            std::memcpy( &pData[ Offset.m_Offset ], &Mip.m_pData[1], MipDataSize );
            
            // Get ready for the next entry
            TotalOffset += MipDataSize;
        }
    }
    
    //
    // OK we are ready to setup the bitmap
    //
    const auto& Mip         = MipList[0];
    setup
    ( Mip.m_Width
    , Mip.m_Height
    , static_cast<xbitmap::format>(Mip.getFormat())
    , static_cast<int>( TotalSize - sizeof(mip) * MipList.size() ),
    { BaseData.release(), TotalSize }
    , true
    , static_cast<int>( MipList.size() )
    , 1 
    );
}

//-------------------------------------------------------------------------------

void xbitmap::ComputePremultiplyAlpha( void ) noexcept
{
    if( m_Flags.m_bAlphaPremultiplied )
        return ;
         
    assert( static_cast<int>(m_Flags.m_Format) == static_cast<int>(xcolor::format::type::DEFAULT) );

    if (ComputeHasAlphaInfo() == false) return;

    auto Data = getMip<xcolori>(0);
    for( auto& C : Data ) 
    {
        C = C.PremultiplyAlpha();
    }

    // Remember that we did this
    m_Flags.m_bAlphaPremultiplied = true;
}

//-------------------------------------------------------------------------------

void xbitmap::FlipImageInY( void ) noexcept
{
    assert( isValid() );
    assert( getFormat() == format::XCOLOR );
    assert( getMipCount() == 1 );

    auto pData = getMip<xcolori>(0);
    for( std::uint32_t y = 0u, endy = m_Height / 2u; y < endy;    ++y )
    for( std::uint32_t x = 0u;                       x < m_Width; ++x )
    {
        std::swap( pData[ x + y * m_Width], pData[ x + (m_Height-y-1) * m_Width] );
    }
}

