#ifndef XCOLOR_H
#define XCOLOR_H
#pragma once

#include <array>

namespace xcolor
{
    template< typename T >
    struct unit;

    //--------------------------------------------------------------------------------

    struct format
    {
        // (LOW BITS elements first then moving to HIGH BITS)
        enum class type : std::uint8_t
        {   INVALID
        ,   UINT_32_BGRA_8888
        ,   UINT_32_BGRU_8888
        ,   UINT_32_ARGB_8888
        ,   UINT_32_URGB_8888
        ,   UINT_32_RGBU_8888
        ,   UINT_32_RGBA_8888
        ,   UINT_32_ABGR_8888
        ,   UINT_24_RGB_888
        ,   UINT_24_ARGB_8565
        ,   UINT_16_RGBA_4444
        ,   UINT_16_RGB_565
        ,   UINT_16_BGRA_5551
        ,   UINT_16_ABGR_4444
        ,   UINT_16_ARGB_4444
        ,   UINT_16_BGR_565
        ,   UINT_16_ARGB_1555
        ,   UINT_16_RGBA_5551
        ,   UINT_16_URGB_1555
        ,   UINT_16_RGBU_5551
        ,   UINT_16_ABGR_1555
        ,   UINT_16_UBGR_1555
        ,   ENUM_COUNT
        ,   DEFAULT = UINT_32_RGBA_8888
        };

        static constexpr auto count_v = static_cast<int>(type::ENUM_COUNT);

        struct descriptor
        {
            type            m_Format;
            std::uint32_t   m_FormatMask;     // One unique bit out of the 32
            std::int32_t    m_TB;             // Total Bits( 16, 24, 32 )
            std::int32_t    m_NUB;            // Number of Used Bits( 15, 16, 24, 32 )
            std::int32_t    m_AShift;
            std::int32_t    m_RShift;
            std::int32_t    m_GShift;
            std::int32_t    m_BShift;
            std::uint32_t   m_AMask;
            std::uint32_t   m_RMask;
            std::uint32_t   m_GMask;
            std::uint32_t   m_BMask;
        };

        constexpr static   format           FindClosestFormat   ( std::uint32_t FormatMask
                                                                , format        Match
                                                                ) noexcept;
        constexpr static   format           FindFormat          ( std::uint32_t AMask
                                                                , std::uint32_t RMask
                                                                , std::uint32_t GMask
                                                                , std::uint32_t BMask
                                                                ) noexcept;
        // Color space conversions. 
        inline          const descriptor&   getDescriptor       ( void 
                                                                ) const noexcept;

        format::type m_Value;
    };

    //--------------------------------------------------------------------------------

    template<typename T>
    constexpr static unit<T>    getColorCategory(int Index) noexcept;

    //--------------------------------------------------------------------------------

    struct u32_elements
    {
        using element = std::uint8_t;
        union
        {
            struct
            {
                element m_R;
                element m_G;
                element m_B;
                element m_A;
            };

            std::uint32_t         m_Value;
        };
    };

    template< typename T >
    struct elements
    {
        using element = T;
        static_assert( std::is_same_v<T,double> || std::is_same_v<T, float>, "The supported color formats are: uint32, float, double");
        element m_R;
        element m_G;
        element m_B;
        element m_A;
    };

    //--------------------------------------------------------------------------------

    template< typename T >
    struct unit : std::conditional_t< std::is_same_v<T, std::uint32_t>, u32_elements, elements<T> >
    {
        using parent_t = std::conditional_t< std::is_same_v<T, std::uint32_t>, u32_elements, elements<T> >;
        using element  = typename parent_t::element;
        using self     = unit<T>;


        constexpr                               unit                ( void 
                                                                    ) noexcept = default;
        constexpr                               unit                ( const std::uint32_t K 
                                                                    ) noexcept;
        constexpr                               unit                ( const std::array<float,3>& C
                                                                    ) noexcept;
        constexpr                               unit                ( const std::array<float, 4>& C
                                                                    ) noexcept;
        template< typename J >
        constexpr                               unit                ( const unit<J>& C 
                                                                    ) noexcept;
        constexpr                               unit                ( element R
                                                                    , element G
                                                                    , element B
                                                                    , element A
                                                                    ) noexcept;
        constexpr                               unit                ( std::uint32_t RawData
                                                                    , format        DataFormat
                                                                    ) noexcept;
        inline              unit&               setupFromYIQ        ( float   Y
                                                                    , float   I
                                                                    , float   Q
                                                                    ) noexcept;
        inline              unit&               setupFromYUV        ( float   Y
                                                                    , float   U
                                                                    , float   V
                                                                    ) noexcept;
        inline              unit&               setupFromCIE        ( float   C
                                                                    , float   I
                                                                    , float   E
                                                                    ) noexcept;
        inline              unit&               setupFromRGBA       ( const std::array<float, 4>& C
                                                                    ) noexcept;
        inline              unit&               setupFromRGBA       ( float  R
                                                                    , float  G
                                                                    , float  B
                                                                    , float  A
                                                                    ) noexcept;
        inline              unit&               setupFromRGB        ( const std::array<float, 3>& Vector
                                                                    ) noexcept;
        inline              unit&               setupFromRGB        ( float  R
                                                                    , float  G
                                                                    , float  B
                                                                    ) noexcept;

        inline              unit&               setupFromCMY        ( float   C
                                                                    , float   M
                                                                    , float   Y
                                                                    ) noexcept;
                            unit&               setupFromHSV        ( float   H
                                                                    , float   S
                                                                    , float   V
                                                                    ) noexcept;
        inline              unit&               setupFromHSV        ( const std::array<float, 3>& HSV
                                                                    ) noexcept;
        inline              unit&               setupFromLight      ( const std::array<float, 3>& LightDir
                                                                    ) noexcept;
        inline              unit&               setupFromNormal     ( const std::array<float, 3>& Normal
                                                                    ) noexcept;
        inline              unit&               setupFromNormal     ( const std::array<float, 3>&& Normal
                                                                    ) noexcept;
        // Access the color in different forms
        inline              element&            operator []         ( int Index
                                                                    ) noexcept;
        inline              element             operator []         ( int Index
                                                                    ) const noexcept;
        constexpr                           operator std::uint32_t  ( void 
                                                                    ) const noexcept;
        // Math operators
        inline          const unit&             operator +=         ( const unit& C
                                                                    ) noexcept;
        inline          const unit&             operator -=         ( const unit& C
                                                                    ) noexcept;
        inline          const unit&             operator *=         ( const unit& C
                                                                    ) noexcept;
        constexpr       bool                    operator ==         ( const unit& C
                                                                    ) const noexcept;
        constexpr       bool                    operator !=         ( const unit& C
                                                                    ) const noexcept;

        // These are also avariable
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator /  ( const unit& A, const unit& B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator /  ( const unit& A, const T     B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator /  ( const T     B, const unit& A ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator *  ( const unit& A, const unit& B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator *  ( const unit& A, const T     B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator *  ( const T     B, const unit& A ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator +  ( const unit& A, const unit& B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator +  ( const unit& A, const T     B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator +  ( const T     B, const unit& A ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator -  ( const unit& A, const unit& B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator -  ( const unit& A, const T     B ) noexcept;
        //      template< typename T > requires std::is_fundamental_v<T> friend const unit&  operator -  ( const T     B, const unit& A ) noexcept;

        // Color data conversions
        inline          void                    setAlpha            ( float Alpha
                                                                    ) noexcept;
        constexpr       unit                    PremultiplyAlpha    ( void
                                                                    ) const noexcept;
        inline          unit                    MultiplyWithHSV     ( const std::array<float, 3>& V
                                                                    ) noexcept;
        constexpr       unit                    getBlendedColors    ( const unit       Src1
                                                                    , const unit       Src2
                                                                    , float T 
                                                                    ) const noexcept;
        constexpr       std::uint32_t           getDataFromColor    ( format DataFormat
                                                                    ) const noexcept;
        constexpr       void                    getYIQ              ( float& Y
                                                                    , float& I
                                                                    , float& Q
                                                                    ) const noexcept;
        constexpr       void                    getYUV              ( float& Y
                                                                    , float& U
                                                                    , float& V
                                                                    ) const noexcept;
        constexpr       void                    getCIE              ( float& C
                                                                    , float& I
                                                                    , float& E
                                                                    ) const noexcept;
        constexpr       void                    getRGB              ( float& aR
                                                                    , float& aG
                                                                    , float& aB
                                                                    ) const noexcept;
        constexpr       std::array<float, 3>    getRGB              ( void
                                                                    ) const noexcept;
        constexpr       void                    getRGBA             ( float& aR
                                                                    , float& aG
                                                                    , float& aB
                                                                    , float& aA
                                                                    ) const noexcept;
        constexpr       std::array<float, 4>    getRGBA             ( void
                                                                    ) const noexcept;
        constexpr       void                    getCMY              ( float& C
                                                                    , float& M
                                                                    , float& Y
                                                                    ) const noexcept;
        constexpr       void                    getHSV              ( float& H
                                                                    , float& S
                                                                    , float& V
                                                                    ) const noexcept;
        constexpr       std::array<float, 3>    getHSV              ( void
                                                                    ) const noexcept;
        constexpr       std::array<float, 3>    getLight            ( void
                                                                    ) const noexcept;
        constexpr       std::array<float, 3>    getNormal           ( void
                                                                    ) const noexcept;
    };
}

//---------------------------------------------------------------------------------

using xcolori = xcolor::unit<std::uint32_t>;
using xcolorf = xcolor::unit<float>;
using xcolord = xcolor::unit<double>;

//---------------------------------------------------------------------------------

#include "implementation/xcolor_inline.h"

#endif