/****************************************************************************
\cpptex

    \cppfilename{BitArray.cpp}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#include "BitArray.h"        ///  \cppfileref{BitArray.h}
#pragma hdrstop
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    const bit_array &bit_array::operator=(const bit_array &array)
    {
        if(this != &array)
        {
            resize(array.lenght.bit_count());
            memcpy(bytes, array.bytes, lenght.byte_count());
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::resize(const bit_counter &new_lenght)
    {
        if(lenght != new_lenght)
        {
            delete[] bytes;
            lenght = new_lenght;
            if(lenght.bit_count())
            {
                bytes = new byte[lenght.byte_count()];
                if(!bytes)
                    throw exception(__FUNC__, "Unable allocate memory");
            }
            else
                bytes = NULL;
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::fill(bool value)
    {
        byte *current   = bytes;
        byte *const end = bytes + (lenght.byte_count() - 1);
        while(current <= end)
            *current++ = value ? ~byte(0) : 0;
        correct_last_byte();
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::random_fill()
    {
        byte *current   = bytes;
        byte *const end = bytes + (lenght.byte_count() - 1);
        while(current <= end)
            *current++ = random_byte();
        correct_last_byte();
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::let_and(const bit_array &one, const bit_array &two)
    {
    #ifdef GA_DEBUG
        if(one.lenght != two.lenght)
            throw exception(__FUNC__, "Lenghts must be same");
    #endif
        resize(one.lenght.bit_count());
        byte *one_current = one.bytes;
        byte *two_current = two.bytes;
        byte *current     = bytes;
        byte *const end   = bytes + (lenght.byte_count() - 1);
        while(current <= end)
            *current++ = (*(one_current++) & *(two_current++));
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::let_or(const bit_array &one, const bit_array &two)
    {
    #ifdef GA_DEBUG
        if(one.lenght != two.lenght)
            throw exception(__FUNC__, "Lenghts must be same");
    #endif
        resize(one.lenght.bit_count());
        byte *one_current = one.bytes;
        byte *two_current = two.bytes;
        byte *current     = bytes;
        byte *const end   = bytes + (lenght.byte_count() - 1);
        while(current <= end)
            *current++ = (*(one_current++) | *(two_current++));
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::let_xor(const bit_array &one, const bit_array &two)
    {
    #ifdef GA_DEBUG
        if(one.lenght != two.lenght)
            throw exception(__FUNC__, "Lenghts must be same");
    #endif
        resize(one.lenght.bit_count());
        byte *one_current = one.bytes;
        byte *two_current = two.bytes;
        byte *current     = bytes;
        byte *const end   = bytes + (lenght.byte_count() - 1);
        while(current <= end)
            *current++ = (*(one_current++) ^ *(two_current++));
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::let_not(const bit_array &source)
    {
        resize(source.lenght.bit_count());
        byte *array_current = source.bytes;
        byte *current       = bytes;
        byte *const end     = bytes + (lenght.byte_count() - 1);
        while(current <= end)
            *current++ = byte(~*(array_current++));
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::flip()
    {
        byte *current   = bytes;
        byte *const end = bytes + (lenght.byte_count() - 1);
        while(current <= end)
        {
            *current = byte(~*(current));
            current++;
        }
        correct_last_byte();
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::concatinate(const bit_array &one, const bit_array &two)
    {
        /// Копирование двух сшитых массива \cppclassmember{bit\_array}{concatinate}
        const bit_array &head = (this == &one) ? bit_array(*this) : one;
        const bit_array &tail = (this == &two) ? bit_array(*this) : two;

        resize(head.lenght.bit_count() + tail.lenght.bit_count());
        memcpy(bytes, head.bytes, head.lenght.byte_count());

        if(head.lenght.get_offset())
        {
            byte *tail_current = tail.bytes;
            byte *current      = bytes + (head.lenght.byte_count() - 1);
            byte *const end    = bytes + (lenght.byte_count() - 1);
            while(current < end)
            {
                *current++ |= byte(*tail_current << (head.lenght.get_offset()));
                *current =  byte(*tail_current++ >> (byte_bits - head.lenght.get_offset()));
            }
            *current |= byte(*tail_current << ( head.lenght.get_offset()));
        }
        else
            memcpy(bytes + head.lenght.byte_count(), tail.bytes, tail.lenght.byte_count());
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::copy(const bit_array &source, const bit_counter &first,
                                                              const bit_counter &last)
    {
        /// Копирование части массива источника \cppclassmember{bit\_array}{copy}
    #ifdef GA_DEBUG
        if(lenght - 1 < last || source.lenght - 1 < last)
            throw exception(__FUNC__, "Last element out of bounds");
        if(first > last)
            throw exception(__FUNC__, "First element position most of last element position");
    #endif
        if(first.get_offset())
        {
            byte mask = byte(1 << first.get_offset()) - 1;
            *(bytes + first.get_segment()) &= mask;
            *(bytes + first.get_segment()) |= ~mask & *(source.bytes + first.get_segment());
        }
        if(last.get_segment() - first.byte_count() > 0)
            memcpy(bytes + first.byte_count(), source.bytes + first.byte_count(),
                                        (last.get_segment() - first.byte_count()));
        if(last.get_offset())
        {
            byte mask = byte(1 << (last.get_offset() + 1)) - 1;   /// +1 потому, что \cppp{last} тоже включается
            *(bytes + last.get_segment()) &= ~mask;
            *(bytes + last.get_segment()) |= mask & *(source.bytes + last.get_segment());
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::move(const bit_array &source, const bit_counter &first,
                                                              const bit_counter &last,
                                                              const bit_counter &dest)
    {
        /// Копирование части источника в другую позицию \cppclassmember{bit\_array}{move}
    #ifdef GA_DEBUG
        if(lenght - 1 < dest + last - first || source.lenght - 1 < last)
            throw exception(__FUNC__, "Last element out of bounds");
        if(first > last)
            throw exception(__FUNC__, "First element position most of last element position");
    #endif
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::subarray(const bit_array &source, const bit_counter &first,
                                                                  const bit_counter &last)
    {
        /// Копирование субмассива источника \cppclassmember{bit\_array}{subarray}
    #ifdef GA_DEBUG
        if(first > last)
            throw exception(__FUNC__, "First element most that last element");
    #endif
        const bit_array &array = (this == &source) ? bit_array(*this) : source;
        resize(last.bit_count() - first.bit_count() + 1);
        if(first.get_offset())
        {
            byte *array_current = array.bytes + first.get_segment();
            byte *current       = bytes;
            byte *const end     = bytes + (lenght.byte_count() - 1);
            while(current <= end)
            {
                *current    = byte(*array_current++ >> first.get_offset());
                *current++ |= byte(*array_current << (byte_bits - first.get_offset()));
            }
        }
        else
            memcpy(bytes, array.bytes + first.get_segment(), lenght.byte_count());
        correct_last_byte();
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::remainder(const bit_array &source, const bit_counter &first,
                                                                   const bit_counter &last)
    {
        /// Копирование источника без вырезанного субмассива \cppclassmember{bit\_array}{remainder}
        const bit_array &array = (this == &source) ? bit_array(*this) : source;
        resize(array.lenght.bit_count() - (last.bit_count() - first.bit_count() + 1));
        memcpy(bytes, array.bytes, first.byte_count());
        *(bytes + first.get_segment()) &= byte((1 << first.get_offset()) - 1);

        if(first.get_offset() - 1 == last.get_offset())
        {
            if(last.get_offset())
                *(bytes + first.get_segment()) |= byte(*(array.bytes + last.get_segment())
                                                       & (~((1 << first.get_offset()) - 1)));
            memcpy(bytes + first.byte_count(),
                   array.bytes + last.byte_count(),
                   array.lenght.byte_count() - last.byte_count());
        }
        else
        {
            byte *array_current   = array.bytes + last.get_segment();
            byte *const array_end = array.bytes + (array.lenght.byte_count() - 1);
            byte *current         = bytes + first.get_segment();
            *current |= (byte)((*array_current >> last.get_offset()) << (first.get_offset() - 1));
            if(first.get_offset() - 1 > last.get_offset())
            {
                while(array_current < array_end)
                {
                    *(++current) = byte(*array_current++
                                 >> (last.get_offset() + byte_bits + 1 - first.get_offset()));
                    *current |= byte(*array_current << (first.get_offset() - 1 - last.get_offset()));
                }
            }
            else
            {
                while(array_current < array_end)
                {
                    *current++ |= byte(*(++array_current)
                               << (first.get_offset() + byte_bits - 1 - last.get_offset()));
                    *current = byte(*array_current >> (last.get_offset() + 1 - first.get_offset()));
                }
            }
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::rotate_left(const bit_array &source)
    {
        const bit_array &array = (this == &source) ? bit_array(*this) : source;
        resize(array.lenght.bit_count());
        byte *array_current = array.bytes;
        byte *current = bytes;
        byte *const end = bytes + (lenght.byte_count() - 1);
        while(current < end)
        {
            *current++ = byte((*array_current >> 1) | (*(array_current + 1)
                       << (byte_bits - 1)));
            array_current++;
        }
        *end = byte((*array_current >> 1) | (*array.bytes << (lenght - 1).get_offset()));
        return *this;
    }
    //---------------------------------------------------------------------------
    const bit_array &bit_array::rotate_right(const bit_array &source)
    {
        const bit_array &array = (this == &source) ? bit_array(*this) : source;
        resize(array.lenght.bit_count());
        *bytes = byte((*array.bytes << 1) | (*(array.bytes + (lenght.byte_count() - 1))
               >> (lenght - 1).get_offset()));
        byte *array_current = array.bytes + 1;
        byte *current = bytes + 1;
        byte *const end = bytes + (lenght.byte_count() - 1);
        while(current <= end)
        {
            *current++ = byte((*array_current << 1) | (*(array_current - 1)
                       >> (byte_bits - 1)));
            array_current++;
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    const std::string bit_array::to_string() const
    {
        std::string bit_string;
        if(bytes)
        {
            int i;
            byte *current = bytes;
            byte *const end = bytes + (lenght.byte_count() - 1);
            while(current < end)
            {
                for(i = 0; i < byte_bits; i++)
                    bit_string += ((*current >> i) & 1) ? "1" : "0";
                current++;
            }
            int offset = lenght.get_offset() ? lenght.get_offset() : byte_bits;
            for(i = 0; i < offset; i++)
                bit_string += ((*end >> i) & 1) ? "1" : "0";
        }
        return bit_string;
    }
    //---------------------------------------------------------------------------
} // namespace ga
