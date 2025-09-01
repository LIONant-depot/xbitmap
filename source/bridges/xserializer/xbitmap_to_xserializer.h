#ifndef BITMAP_TO_XSERIALIZER_H
#define BITMAP_TO_XSERIALIZER_H
#pragma once
#include "dependencies/xbitmap/source/xbitmap.h"
#include "dependencies/xserializer/source/xserializer.h"

//-------------------------------------------------------------------------------
// Serializer functions
//-------------------------------------------------------------------------------
namespace xserializer::io_functions
{
    template<> inline
    xerr SerializeIO<xbitmap>( xserializer::stream& Stream, const xbitmap& Bitmap ) noexcept
    {
        xerr Err;

        false
        || (Err = Stream.Serialize(reinterpret_cast<const std::byte* const&>(Bitmap.m_pData), Bitmap.m_DataSize, mem_type{ .m_bUnique = true } ))
        || (Err = Stream.Serialize(Bitmap.m_DataSize))
        || (Err = Stream.Serialize(Bitmap.m_FaceSize))
        || (Err = Stream.Serialize(Bitmap.m_Height))
        || (Err = Stream.Serialize(Bitmap.m_Width))
        || (Err = Stream.Serialize(Bitmap.m_Flags.m_Value))
        || (Err = Stream.Serialize(Bitmap.m_nMips))
        || (Err = Stream.Serialize(Bitmap.m_ClampColor.m_R))
        || (Err = Stream.Serialize(Bitmap.m_ClampColor.m_G))
        || (Err = Stream.Serialize(Bitmap.m_ClampColor.m_B))
        || (Err = Stream.Serialize(Bitmap.m_ClampColor.m_A))
        ;

        return Err;
    }
}
#endif