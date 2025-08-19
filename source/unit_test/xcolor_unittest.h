#include <array>
#include <cmath>
#include <cassert>
#include <iostream>

namespace xcolor::unit_test
{
    template<typename T>
    bool approx_equal(T a, T b, T epsilon = 0.0001)
    {
        return std::abs(a - b) < epsilon;
    }

    void Test()
    {
        using namespace xcolor;
        // xcolori tests
        {
            std::cout << "\nTesting xcolori\n";
            // Default constructor
            {
                xcolori color{0};
                assert(color.m_R == 0);
                assert(color.m_G == 0);
                assert(color.m_B == 0);
                assert(color.m_A == 0);
            }
            // Constructor from uint32_t
            {
                xcolori color(0x00FF00FF); // RGBA: A=255, R=0, G=255, B=0
                assert(color.m_A == 255);
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 0);
            }
            // Constructor from array<float, 3>
            {
                std::array<float, 3> rgb = { 1.0f, 0.5f, 0.0f };
                xcolori color(rgb);
                assert(color.m_R == 255);
                assert(color.m_G == 127);
                assert(color.m_B == 0);
                assert(color.m_A == 255);
            }
            // Constructor from array<float, 4>
            {
                std::array<float, 4> rgba = { 1.0f, 0.5f, 0.0f, 0.5f };
                xcolori color(rgba);
                assert(color.m_R == 255);
                assert(color.m_G == 127);
                assert(color.m_B == 0);
                assert(color.m_A == 127);
            }
            // Copy constructor
            {
                xcolori color1(255, 128, 64, 32);
                xcolori color2(color1);
                assert(color2.m_R == 255);
                assert(color2.m_G == 128);
                assert(color2.m_B == 64);
                assert(color2.m_A == 32);
            }
            // Cross-type constructor
            {
                xcolorf colorf(1.0f, 0.5f, 0.25f, 0.125f);
                xcolori colori(colorf);
                assert(colori.m_R == 255);
                assert(colori.m_G == 127);
                assert(colori.m_B == 63);
                assert(colori.m_A == 31);
            }
            // Constructor from raw data and format
            {
                xcolori color(0xFF00FF00, format{ format::type::UINT_32_ARGB_8888 });
                assert(color.m_A == 255);
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 0);
                xcolori color16(0xF0F0, format{ format::type::UINT_16_RGBA_4444 });
                assert(color16.m_R == 255);
                assert(color16.m_G == 0);
                assert(color16.m_B == 255);
                assert(color16.m_A == 0);
            }
            // Color space conversions
            {
                xcolori color;
                color.setupFromRGB(1.0f, 0.5f, 0.0f);
                assert(color.m_R == 255);
                assert(color.m_G == 127);
                assert(color.m_B == 0);
                assert(color.m_A == 255);
                color.setupFromRGBA(0.0f, 1.0f, 0.5f, 0.5f);
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 127);
                assert(color.m_A == 127);
                color.setupFromHSV(0.0f, 1.0f, 1.0f);
                assert(color.m_R == 255);
                assert(color.m_G == 0);
                assert(color.m_B == 0);
                color.setupFromYIQ(0.587f, -0.274f, -0.523f); // Approx green (0, 1, 0)
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 0);
                color.setupFromYUV(0.587f, -0.289f, -0.515f); // Approx green
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 0);
                color.setupFromCMY(1.0f, 0.0f, 1.0f); // Green
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 0);
                color.setupFromCIE(0.174f, 0.587f, 0.066f); // Approx green
                assert(color.m_R == 0);
                assert(color.m_G == 255);
                assert(color.m_B == 0);
                color.setupFromNormal({ 0.0f, 1.0f, 0.0f }); // Up normal
                assert(color.m_R == 127);
                assert(color.m_G == 255);
                assert(color.m_B == 127);
                color.setupFromLight({ 0.0f, -1.0f, 0.0f }); // Down light
                assert(color.m_R == 127);
                assert(color.m_G == 255);
                assert(color.m_B == 127);
            }
            // Accessors
            {
                xcolori color(255, 128, 64, 32);
                assert(color[0] == 255);
                assert(color[1] == 128);
                assert(color[2] == 64);
                assert(color[3] == 32);
                std::array<float, 4> rgba = color.getRGBA();
                assert(approx_equal(rgba[0], 1.0f));
                assert(approx_equal(rgba[1], 0.5019608f));
                assert(approx_equal(rgba[2], 0.2509804f));
                assert(approx_equal(rgba[3], 0.1254902f));
                float y, i, q;
                color.getYIQ(y, i, q);
                assert(approx_equal(y, 0.587f, 0.01f));
                assert(approx_equal(i, -0.274f, 0.01f));
                assert(approx_equal(q, -0.523f, 0.01f));
                float c, m, y2;
                color.getCMY(c, m, y2);
                assert(approx_equal(c, 0.0f));
                assert(approx_equal(m, 0.4980392f));
                assert(approx_equal(y2, 0.7490196f));
                std::array<float, 3> normal = color.getNormal();
                assert(approx_equal(normal[0], 0.0f));
                assert(approx_equal(normal[1], 0.0f));
                assert(approx_equal(normal[2], -0.5f, 0.01f));
            }
            // Math operators
            {
                xcolori color1(100, 100, 100, 255);
                xcolori color2(50, 50, 50, 255);
                color1 += color2;
                assert(color1.m_R == 150);
                assert(color1.m_G == 150);
                assert(color1.m_B == 150);
                color1 -= color2;
                assert(color1.m_R == 100);
                assert(color1.m_G == 100);
                assert(color1.m_B == 100);
                color1 *= color2;
                assert(color1.m_R == 19);
                assert(color1.m_G == 19);
                assert(color1.m_B == 19);
                assert(color1 == xcolori(19, 19, 19, 255));
                assert(color1 != color2);
            }
            // Utility methods
            {
                xcolori color(255, 128, 64, 255);
                color.setAlpha(0.5f);
                assert(color.m_A == 127);
                xcolori premul = color.PremultiplyAlpha();
                assert(premul.m_R == 127);
                assert(premul.m_G == 64);
                assert(premul.m_B == 32);
                xcolori blended = color.getBlendedColors(xcolori(0, 0, 0, 255), xcolori(255, 255, 255, 255), 0.5f);
                assert(blended.m_R == 127);
                assert(blended.m_G == 127);
                assert(blended.m_B == 127);
            }
            // Color category
            {
                xcolori color = getColorCategory(0);
                assert(color.m_R == 31);
                assert(color.m_G == 119);
                assert(color.m_B == 180);
                assert(color.m_A == 255);
            }
        }
        // xcolorf tests
        {
            std::cout << "\nTesting xcolorf\n";
            // Default constructor
            {
                xcolorf color{};
                assert(approx_equal(color.m_R, 0.0f));
                assert(approx_equal(color.m_G, 0.0f));
                assert(approx_equal(color.m_B, 0.0f));
                assert(approx_equal(color.m_A, 0.0f));
            }
            // Constructor from array<float, 3>
            {
                std::array<float, 3> rgb = { 1.0f, 0.5f, 0.0f };
                xcolorf color(rgb);
                assert(approx_equal(color.m_R, 1.0f));
                assert(approx_equal(color.m_G, 0.5f));
                assert(approx_equal(color.m_B, 0.0f));
                assert(approx_equal(color.m_A, 1.0f));
            }
            // Constructor from array<float, 4>
            {
                std::array<float, 4> rgba = { 1.0f, 0.5f, 0.0f, 0.5f };
                xcolorf color(rgba);
                assert(approx_equal(color.m_R, 1.0f));
                assert(approx_equal(color.m_G, 0.5f));
                assert(approx_equal(color.m_B, 0.0f));
                assert(approx_equal(color.m_A, 0.5f));
            }
            // Copy constructor
            {
                xcolorf color1(1.0f, 0.5f, 0.25f, 0.125f);
                xcolorf color2(color1);
                assert(approx_equal(color2.m_R, 1.0f));
                assert(approx_equal(color2.m_G, 0.5f));
                assert(approx_equal(color2.m_B, 0.25f));
                assert(approx_equal(color2.m_A, 0.125f));
            }
            // Cross-type constructor
            {
                xcolori colori(255, 128, 64, 32);
                xcolorf colorf(colori);
                assert(approx_equal(colorf.m_R, 1.0f));
                assert(approx_equal(colorf.m_G, 0.5019608f));
                assert(approx_equal(colorf.m_B, 0.2509804f));
                assert(approx_equal(colorf.m_A, 0.1254902f));
            }
            // Color space conversions
            {
                xcolorf color;
                color.setupFromRGB(1.0f, 0.5f, 0.0f);
                assert(approx_equal(color.m_R, 1.0f));
                assert(approx_equal(color.m_G, 0.5f));
                assert(approx_equal(color.m_B, 0.0f));
                assert(approx_equal(color.m_A, 1.0f));
                color.setupFromHSV(0.0f, 1.0f, 1.0f);
                assert(approx_equal(color.m_R, 1.0f));
                assert(approx_equal(color.m_G, 0.0f));
                assert(approx_equal(color.m_B, 0.0f));
                color.setupFromYIQ(0.587f, -0.274f, -0.523f); // Approx green
                assert(approx_equal(color.m_R, 0.0f, 0.01f));
                assert(approx_equal(color.m_G, 1.0f, 0.01f));
                assert(approx_equal(color.m_B, 0.0f, 0.01f));
                color.setupFromYUV(0.587f, -0.289f, -0.515f); // Approx green
                assert(approx_equal(color.m_R, 0.0f, 0.01f));
                assert(approx_equal(color.m_G, 1.0f, 0.01f));
                assert(approx_equal(color.m_B, 0.0f, 0.01f));
                color.setupFromCMY(1.0f, 0.0f, 1.0f); // Green
                assert(approx_equal(color.m_R, 0.0f));
                assert(approx_equal(color.m_G, 1.0f));
                assert(approx_equal(color.m_B, 0.0f));
                color.setupFromCIE(0.174f, 0.587f, 0.066f); // Approx green
                assert(approx_equal(color.m_R, 0.0f, 0.01f));
                assert(approx_equal(color.m_G, 1.0f, 0.01f));
                assert(approx_equal(color.m_B, 0.0f, 0.01f));
                color.setupFromNormal({ 0.0f, 1.0f, 0.0f });
                assert(approx_equal(color.m_R, 0.5f));
                assert(approx_equal(color.m_G, 1.0f));
                assert(approx_equal(color.m_B, 0.5f));
                color.setupFromLight({ 0.0f, -1.0f, 0.0f });
                assert(approx_equal(color.m_R, 0.5f));
                assert(approx_equal(color.m_G, 1.0f));
                assert(approx_equal(color.m_B, 0.5f));
            }
            // Accessors
            {
                xcolorf color(1.0f, 0.5f, 0.25f, 0.125f);
                assert(approx_equal(color[0], 1.0f));
                assert(approx_equal(color[1], 0.5f));
                assert(approx_equal(color[2], 0.25f));
                assert(approx_equal(color[3], 0.125f));
                std::array<float, 4> rgba = color.getRGBA();
                assert(approx_equal(rgba[0], 1.0f));
                assert(approx_equal(rgba[1], 0.5f));
                assert(approx_equal(rgba[2], 0.25f));
                assert(approx_equal(rgba[3], 0.125f));
                float y, u, v;
                color.getYUV(y, u, v);
                assert(approx_equal(y, 0.53725f, 0.01f));
                assert(approx_equal(u, -0.227f, 0.01f));
                assert(approx_equal(v, 0.465f, 0.01f));
            }
            // Math operators
            {
                xcolorf color1(0.4f, 0.4f, 0.4f, 1.0f);
                xcolorf color2(0.2f, 0.2f, 0.2f, 1.0f);
                color1 += color2;
                assert(approx_equal(color1.m_R, 0.6f));
                assert(approx_equal(color1.m_G, 0.6f));
                assert(approx_equal(color1.m_B, 0.6f));
                color1 -= color2;
                assert(approx_equal(color1.m_R, 0.4f));
                assert(approx_equal(color1.m_G, 0.4f));
                assert(approx_equal(color1.m_B, 0.4f));
                color1 *= color2;
                assert(approx_equal(color1.m_R, 0.08f));
                assert(approx_equal(color1.m_G, 0.08f));
                assert(approx_equal(color1.m_B, 0.08f));
            }
            // Utility methods
            {
                xcolorf color(1.0f, 0.5f, 0.25f, 1.0f);
                color.setAlpha(0.5f);
                assert(approx_equal(color.m_A, 0.5f));
                xcolorf premul = color.PremultiplyAlpha();
                assert(approx_equal(premul.m_R, 0.5f));
                assert(approx_equal(premul.m_G, 0.25f));
                assert(approx_equal(premul.m_B, 0.125f));
                xcolorf blended = color.getBlendedColors(xcolorf(0.0f, 0.0f, 0.0f, 1.0f), xcolorf(1.0f, 1.0f, 1.0f, 1.0f), 0.5f);
                assert(approx_equal(blended.m_R, 0.5f));
                assert(approx_equal(blended.m_G, 0.5f));
                assert(approx_equal(blended.m_B, 0.5f));
            }
        }
        // xcolord tests
        {
            std::cout << "\nTesting xcolord\n";
            // Default constructor
            {
                xcolord color{};
                assert(approx_equal(color.m_R, 0.0));
                assert(approx_equal(color.m_G, 0.0));
                assert(approx_equal(color.m_B, 0.0));
                assert(approx_equal(color.m_A, 0.0));
            }
            // Constructor from array<float, 3>
            {
                std::array<float, 3> rgb = { 1.0f, 0.5f, 0.0f };
                xcolord color(rgb);
                assert(approx_equal(color.m_R, 1.0));
                assert(approx_equal(color.m_G, 0.5));
                assert(approx_equal(color.m_B, 0.0));
                assert(approx_equal(color.m_A, 1.0));
            }
            // Constructor from array<float, 4>
            {
                std::array<float, 4> rgba = { 1.0f, 0.5f, 0.0f, 0.5f };
                xcolord color(rgba);
                assert(approx_equal(color.m_R, 1.0));
                assert(approx_equal(color.m_G, 0.5));
                assert(approx_equal(color.m_B, 0.0));
                assert(approx_equal(color.m_A, 0.5));
            }
            // Copy constructor
            {
                xcolord color1(1.0, 0.5, 0.25, 0.125);
                xcolord color2(color1);
                assert(approx_equal(color2.m_R, 1.0));
                assert(approx_equal(color2.m_G, 0.5));
                assert(approx_equal(color2.m_B, 0.25));
                assert(approx_equal(color2.m_A, 0.125));
            }
            // Cross-type constructor
            {
                xcolori colori(255, 128, 64, 32);
                xcolord colord(colori);
                assert(approx_equal(colord.m_R, 1.0));
                assert(approx_equal(colord.m_G, 0.5019608));
                assert(approx_equal(colord.m_B, 0.2509804));
                assert(approx_equal(colord.m_A, 0.1254902));
            }
            // Color space conversions
            {
                xcolord color;
                color.setupFromRGB(1.0f, 0.5f, 0.0f);
                assert(approx_equal(color.m_R, 1.0));
                assert(approx_equal(color.m_G, 0.5));
                assert(approx_equal(color.m_B, 0.0));
                assert(approx_equal(color.m_A, 1.0));
                color.setupFromHSV(0.0f, 1.0f, 1.0f);
                assert(approx_equal(color.m_R, 1.0));
                assert(approx_equal(color.m_G, 0.0));
                assert(approx_equal(color.m_B, 0.0));
                color.setupFromYIQ(0.587f, -0.274f, -0.523f); // Approx green
                assert(approx_equal(color.m_R, 0.0, 0.01));
                assert(approx_equal(color.m_G, 1.0, 0.01));
                assert(approx_equal(color.m_B, 0.0, 0.01));
                color.setupFromYUV(0.587f, -0.289f, -0.515f); // Approx green
                assert(approx_equal(color.m_R, 0.0, 0.01));
                assert(approx_equal(color.m_G, 1.0, 0.01));
                assert(approx_equal(color.m_B, 0.0, 0.01));
                color.setupFromCMY(1.0, 0.0, 1.0); // Green
                assert(approx_equal(color.m_R, 0.0));
                assert(approx_equal(color.m_G, 1.0));
                assert(approx_equal(color.m_B, 0.0));
                color.setupFromCIE(0.174f, 0.587f, 0.066f); // Approx green
                assert(approx_equal(color.m_R, 0.0, 0.01));
                assert(approx_equal(color.m_G, 1.0, 0.01));
                assert(approx_equal(color.m_B, 0.0, 0.01));
                color.setupFromNormal({ 0.0, 1.0, 0.0 });
                assert(approx_equal(color.m_R, 0.5));
                assert(approx_equal(color.m_G, 1.0));
                assert(approx_equal(color.m_B, 0.5));
                color.setupFromLight({ 0.0, -1.0, 0.0 });
                assert(approx_equal(color.m_R, 0.5));
                assert(approx_equal(color.m_G, 1.0));
                assert(approx_equal(color.m_B, 0.5));
            }
            // Accessors
            {
                xcolord color(1.0, 0.5, 0.25, 0.125);
                assert(approx_equal(color[0], 1.0));
                assert(approx_equal(color[1], 0.5));
                assert(approx_equal(color[2], 0.25));
                assert(approx_equal(color[3], 0.125));
                std::array<float, 4> rgba = color.getRGBA();
                assert(approx_equal(rgba[0], 1.0f));
                assert(approx_equal(rgba[1], 0.5f));
                assert(approx_equal(rgba[2], 0.25f));
                assert(approx_equal(rgba[3], 0.125f));
                float c, i, e;
                color.getCIE(c, i, e);
                assert(approx_equal(c, 0.73f, 0.01f));
                assert(approx_equal(i, 0.537f, 0.01f));
                assert(approx_equal(e, 0.045f, 0.01f));
            }
            // Math operators
            {
                xcolord color1(0.4, 0.4, 0.4, 1.0);
                xcolord color2(0.2, 0.2, 0.2, 1.0);
                color1 += color2;
                assert(approx_equal(color1.m_R, 0.6));
                assert(approx_equal(color1.m_G, 0.6));
                assert(approx_equal(color1.m_B, 0.6));
                color1 -= color2;
                assert(approx_equal(color1.m_R, 0.4));
                assert(approx_equal(color1.m_G, 0.4));
                assert(approx_equal(color1.m_B, 0.4));
                color1 *= color2;
                assert(approx_equal(color1.m_R, 0.08));
                assert(approx_equal(color1.m_G, 0.08));
                assert(approx_equal(color1.m_B, 0.08));
            }
            // Utility methods
            {
                xcolord color(1.0, 0.5, 0.25, 1.0);
                color.setAlpha(0.5);
                assert(approx_equal(color.m_A, 0.5));
                xcolord premul = color.PremultiplyAlpha();
                assert(approx_equal(premul.m_R, 0.5));
                assert(approx_equal(premul.m_G, 0.25));
                assert(approx_equal(premul.m_B, 0.125));
                xcolord blended = color.getBlendedColors(xcolord(0.0, 0.0, 0.0, 1.0), xcolord(1.0, 1.0, 1.0, 1.0), 0.5);
                assert(approx_equal(blended.m_R, 0.5));
                assert(approx_equal(blended.m_G, 0.5));
                assert(approx_equal(blended.m_B, 0.5));
            }
        }
        // Format handling tests
        {
            std::cout << "\nTesting format handling\n";
            format fmt = format::FindFormat(0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
            assert(fmt.m_Value == format::type::UINT_32_ARGB_8888);
            format closest = format::FindClosestFormat(0xFF000000, format{ format::type::UINT_32_RGBA_8888 });
            assert(closest.m_Value == format::type::UINT_32_ARGB_8888);
            fmt = format::FindFormat(0xF000, 0x0F00, 0x00F0, 0x000F);
            assert(fmt.m_Value == format::type::UINT_16_RGBA_4444);
        }
    }
}