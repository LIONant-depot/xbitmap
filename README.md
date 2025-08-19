# xbitmap: Advanced Bitmap and Color Handling in C++!

Welcome to **xbitmap**, a powerful and flexible C++ library designed to revolutionize how you handle bitmaps and colors! 
With support for advanced features like cubemaps, multi-frame animations, mipmaps, and a variety of compressed texture formats, 
xbitmap is your go-to solution for high-performance graphics programming. Paired with the `xcolor` library for robust color 
management, xbitmap makes texture manipulation intuitive and efficient.

## Key Features

* **Versatile Bitmap Support**: Manage textures, cubemaps, and multi-frame animations with ease.
* **Comprehensive Format Support**: Includes uncompressed formats (e.g., RGBA8888, RGB565) and compressed formats like PVRTC, ETC, ASTC, S3TC, and more.
* **Mipmap Handling**: Efficiently store and access mipmap chains for optimized rendering.
* **Color Space Flexibility**: Supports sRGB and linear color spaces for accurate rendering.
* **Wrap Modes**: Configurable wrap modes (CLAMP_TO_EDGE, WRAP, MIRROR, etc.) for texture sampling.
* **Alpha Channel Management**: Built-in support for alpha premultiplication and transparency checks.
* **Memory Efficiency**: Optional memory ownership control to optimize resource usage.
* **No External Dependencies**: Lightweight and easy to integrate with just `xbitmap.h`, `xcolor.h`, and their implementation files.
* **MIT License**: Freely use, modify, and distribute.

## Dependencies

* [xerr](): For robust error handling.

## Usage

* Add to your project ```xbitmap.h``` or ```xcolor.h``` Also add ```xbitmap.cpp```

## Memory Layout

The `xbitmap` class organizes texture data efficiently, with support for mipmaps, cubemaps, and animation frames. The memory layout is detailed in `xbitmap.h` and includes:

* **Mip Offset Array**: Stores offsets to mip level data.
* **Face and Frame Support**: Handles cubemaps (6 faces) and multi-frame animations.
* **Palette Support**: Optional palettes for compressed formats like PAL4_R8G8B8A8.

## Code Example

Below is a simple example demonstrating how to create and manipulate a bitmap using the xbitmap library:

```cpp
#include "xbitmap.h"

int main() {
    xbitmap bitmap;
    // Create a 256x256 bitmap with RGBA8888 format
    bitmap.CreateBitmap(256, 256);
    bitmap.setFormat(xbitmap::format::R8G8B8A8);

    // Set a solid color (white) for the bitmap
    xcolori color(255, 255, 255, 255); // RGBA: white, fully opaque
    std::vector<xcolori> data(256 * 256, color);
    bitmap.setupFromColor(256, 256, data);

    // Save the bitmap as a TGA file
    if (auto err = bitmap.SaveTGA(L"output.tga"); err) {
        // Handle error
        return 1;
    }

    // Flip the bitmap vertically
    bitmap.FlipImageInY();

    return 0;
}
```

## Color Management with xcolor

The `xcolor` library provides robust color manipulation, supporting:

* Multiple color formats (e.g., RGBA8888, RGB565, ARGB4444).
* Color space conversions (RGB, HSV, YUV, CMY, etc.).
* Alpha premultiplication and blending operations.
* High-precision floating-point color formats for HDR workflows.

Example usage:

```cpp
#include "xcolor.h"

int main() {
    xcolorf color(1.0f, 0.0f, 0.0f, 1.0f); // Red, fully opaque
    std::array<float, 3> hsv;
    color.getHSV(hsv[0], hsv[1], hsv[2]); // Convert to HSV
    return 0;
}
```

Transform your texture and bitmap handling with xbitmap! Star, fork, and contribute to take your graphics programming to the next level! 🚀