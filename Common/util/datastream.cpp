//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "core/types.h"
#include "util/datastream.h"
#include "util/math.h"

namespace AGS
{
namespace Common
{

DataStream::DataStream(DataEndianess caller_endianess, DataEndianess stream_endianess)
    : _callerEndianess(caller_endianess)
    , _streamEndianess(stream_endianess)
{
}

DataStream::~DataStream()
{
}

String DataStream::ReadString(int max_chars)
{
    if (!CanRead())
    {
        return "";
    }

    String str;
    int chars_read_last = 0;
    int null_t_position = -1;
    // Read a chunk of memory to buffer and seek for null-terminator,
    // if not found, repeat until EOS
    const int single_chunk_length = 3000;
    char char_buffer[single_chunk_length + 1];
    do
    {
        chars_read_last = Read(char_buffer, single_chunk_length);
        char *seek_ptr = char_buffer;
        for (int c = 0; c < chars_read_last && *seek_ptr != '\0'; ++c, ++seek_ptr);

        int append_length = 0;
        int str_len = str.GetLength();
        if (*seek_ptr == '\0')
        {
            null_t_position = seek_ptr - char_buffer;
            if (str_len < max_chars)
            {
                append_length = Math::Min(null_t_position, max_chars - str_len);
            }
        }
        else
        {
            append_length = Math::Min(chars_read_last, max_chars - str_len);
        }

        if (append_length > 0)
        {
            char_buffer[append_length] = '\0';
            str.Append(char_buffer);
        }
    }
    while (!EOS() && null_t_position < 0);

    // If null-terminator was found make sure stream is positioned at the next
    // byte after terminator
    if (null_t_position >= 0)
    {
        // the seek offset should be negative
        Seek(kSeekCurrent, null_t_position - chars_read_last + 1 /* beyond null-terminator */);
    }

    return str;
}

int DataStream::WriteString(const String &str)
{
    if (CanWrite())
    {
        int bytes_written = Write(str.GetCStr(), sizeof(char) * str.GetLength());
        WriteInt8(0);
        return bytes_written;
    }
    return 0;
}

int DataStream::ReadArrayOfIntPtr(intptr_t *buffer, int count)
{
#if defined (AGS_64BIT) || defined (TEST_64BIT)
    return MustSwapBytes() ? ReadAndConvertArrayOfInt64(buffer, count) : ReadArrayOfInt64(buffer, count);
#else
    return MustSwapBytes() ? ReadAndConvertArrayOfInt32((int32_t*)buffer, count) : ReadArrayOfInt32((int32_t*)buffer, count);
#endif
}

int DataStream::ReadArrayOfIntPtr32(intptr_t *buffer, int count)
{
    if (!CanRead())
    {
        return 0;
    }

    // Read 32-bit values to array; this will always be safe,
    // because array is either 32-bit or 64-bit; in the last
    // case only first half of array will be used.
    count = ReadArrayOfInt32((int32_t*)buffer, count);

    if (count < 0)
    {
        return -1;
    }

#if defined (AGS_64BIT) || defined (TEST_64BIT)
    {
        // If we need 64-bit array, then copy 32-bit values to their
        // correct 64-bit slots, starting from the last element and
        // moving towards array head.
        int32_t *buffer32 = (int32_t*)buffer;
        buffer   += count - 1;
        buffer32 += count - 1;
        for (int i = count - 1; i >= 0; --i, --buffer, --buffer32)
        {
            // Ensure correct endianess-dependent positions; note that the
            // value bytes are already properly set by ReadArrayOfInt32
            if (_callerEndianess == kBigEndian)
            {
                *buffer = ((int64_t)*buffer32) << 32;
            }
            else
            {
                *buffer = *buffer32 & 0xFFFFFFFF;
            }
        }
    }
#endif // AGS_64BIT
    return count;
}

int DataStream::WriteArrayOfIntPtr(const intptr_t *buffer, int count)
{
#if defined (AGS_64BIT) || defined (TEST_64BIT)
    return MustSwapBytes() ? WriteAndConvertArrayOfInt64(buffer, count) : WriteArrayOfInt64(buffer, count);
#else
    return MustSwapBytes() ? WriteAndConvertArrayOfInt32((const int32_t*)buffer, count) : WriteArrayOfInt32((const int32_t*)buffer, count);
#endif
}

int DataStream::WriteArrayOfIntPtr32(const intptr_t *buffer, int count)
{
    if (!CanWrite())
    {
        return 0;
    }

    int elem;
    for (elem = 0; elem < count && !EOS(); ++elem, ++buffer)
    {
        int32_t val = (int32_t)*buffer;
        ConvertInt32(val);
        WriteInt32(val);
    }
    return elem;
}

int DataStream::ReadAndConvertArrayOfInt16(int16_t *buffer, int count)
{
    if (!CanRead())
    {
        return 0;
    }

    count = ReadArray(buffer, sizeof(int16_t), count);
    for (int i = 0; i < count; ++i, ++buffer)
    {
        BBOp::SwapBytesInt16(*buffer);
    }
    return count;
}

int DataStream::ReadAndConvertArrayOfInt32(int32_t *buffer, int count)
{
    if (!CanRead())
    {
        return 0;
    }

    count = ReadArray(buffer, sizeof(int32_t), count);
    for (int i = 0; i < count; ++i, ++buffer)
    {
        BBOp::SwapBytesInt32(*buffer);
    }
    return count;
}

int DataStream::ReadAndConvertArrayOfInt64(int64_t *buffer, int count)
{
    if (!CanRead())
    {
        return 0;
    }

    count = ReadArray(buffer, sizeof(int64_t), count);
    for (int i = 0; i < count; ++i, ++buffer)
    {
        BBOp::SwapBytesInt64(*buffer);
    }
    return count;
}

int DataStream::WriteAndConvertArrayOfInt16(const int16_t *buffer, int count)
{
    if (!CanWrite())
    {
        return 0;
    }

    int elem;
    for (elem = 0; elem < count && !EOS(); ++elem, ++buffer)
    {
        int16_t val = *buffer;
        BBOp::SwapBytesInt16(val);
        WriteInt16(val);
    }
    return elem;
}

int DataStream::WriteAndConvertArrayOfInt32(const int32_t *buffer, int count)
{
    if (!CanWrite())
    {
        return 0;
    }

    int elem;
    for (elem = 0; elem < count && !EOS(); ++elem, ++buffer)
    {
        int32_t val = *buffer;
        BBOp::SwapBytesInt32(val);
        WriteInt32(val);
    }
    return elem;
}

int DataStream::WriteAndConvertArrayOfInt64(const int64_t *buffer, int count)
{
    if (!CanWrite())
    {
        return 0;
    }

    int elem;
    for (elem = 0; elem < count && !EOS(); ++elem, ++buffer)
    {
        int64_t val = *buffer;
        BBOp::SwapBytesInt64(val);
        WriteInt64(val);
    }
    return elem;
}

} // namespace Common
} // namespace AGS
