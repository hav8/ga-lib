/****************************************************************************
\cpptex

    \cppfilename{BitArray.h}

    В файле "<BitArray.h"> содержится описание классов {\cppprogram bit}
    и {\cppprogram bit\_array}. Класс {\cppprogram bit\_array} реализует
    битовую последовательность и простейшие опреации над ней: конкатенацию
    двух последовательностей, взятие подпоследовательности, логические операции,
    сдвиги и т.д.

    Класс {\cppprogram bit} предназначен для осуществления
    доступа к отдельным битам в битовой последовательности.

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#ifndef BITARRAY_H
#define BITARRAY_H
//---------------------------------------------------------------------------
#include "BitCount.h"                /// \cppfileref{BitCount.h}
#include "Probability.h"             /// \cppfileref{Probability.h}
//---------------------------------------------------------------------------
#include <string.h>
#include <iostream.h>
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    /// Реализация доступа к единичным битам в массиве \cppclassdef{bit}
    //---------------------------------------------------------------------------
    class bit
    {
        friend class bit_array;

        int         offset;
        byte *const segment;

    private:
        bit(byte *const bytes, const bit_counter &position);

    public:
        const int &operator=(const int &value);
        operator int() const;
    };
    //---------------------------------------------------------------------------
    inline bit::bit(byte *const bytes, const bit_counter &position) :
        segment(bytes + position.get_segment()),
        offset (position.get_offset())
    {
    }
    //---------------------------------------------------------------------------
    inline const int &bit::operator=(const int &value)
    {
        *segment &= byte(~(1 << offset));
        *segment |= byte((value ? 1 : 0) << offset);
        return value;
    }
    //---------------------------------------------------------------------------
    inline bit::operator int() const
    {
        return (*segment >> offset) & 1;
    }
    //---------------------------------------------------------------------------
    /// Битовый массив и операции над ним \cppclassdef{bit\_array}
    //---------------------------------------------------------------------------
    class bit_array
    {
        friend class bit;

        byte        *bytes;
        bit_counter  lenght;

    public:
        bit_array();
        bit_array(const bit_counter &counter);
        bit_array(const bit_array   &array);

        virtual ~bit_array();

                const byte  get_bit (const bit_counter &position) const;
                const byte &set_bit (const bit_counter &position, const byte &value);
        virtual const byte  get_byte(const bit_counter &position) const;
        virtual const byte &set_byte(const bit_counter &position, const byte &value);

        long  bit_count  () const;
        long  byte_count () const;

        const bit_array &operator=   (const bit_array &array);

        const bit_array &resize      (const bit_counter &new_lenght);
        const bit_array &random_fill ();
        const bit_array &fill        (bool value);
        const bit_array &copy        (const bit_array &source, const bit_counter &first,
                                                               const bit_counter &last);
        const bit_array &move        (const bit_array &source, const bit_counter &first,
                                                               const bit_counter &last,
                                                               const bit_counter &dest);
        const bit_array &concatinate (const bit_array &one,    const bit_array   &two);
        const bit_array &subarray    (const bit_array &source, const bit_counter &first,
                                                               const bit_counter &last);
        const bit_array &remainder   (const bit_array &source, const bit_counter &first,
                                                               const bit_counter &last);
        const bit_array &let_and     (const bit_array &one,    const bit_array   &two);
        const bit_array &let_or      (const bit_array &one,    const bit_array   &two);
        const bit_array &let_xor     (const bit_array &one,    const bit_array   &two);
        const bit_array &let_not     (const bit_array &source);
        const bit_array &flip        ();
        const bit_array &rotate_left (const bit_array &source);
        const bit_array &rotate_right(const bit_array &source);

        void             inverse_bit (const bit_counter &position);
        const bit        operator[]  (const bit_counter &position);
        const int        operator[]  (const bit_counter &position) const;

    public:
        const std::string to_string() const;

    private:
        void correct_last_byte();
    };

    //---------------------------------------------------------------------------
    inline bit_array::bit_array() :
        lenght(0),
        bytes (NULL)
    {
    }
    //---------------------------------------------------------------------------
    inline bit_array::bit_array(const bit_counter &counter) :
        lenght(counter),
        bytes (NULL)
    {
        bytes = new byte[lenght.byte_count()];
        if(!bytes)
           throw exception(__FUNC__, "Unable alloc memory");
    }
    //---------------------------------------------------------------------------
    inline bit_array::bit_array(const bit_array &array) :
        lenght(array.lenght),
        bytes (NULL)
    {
        bytes = new byte[lenght.byte_count()];
        if(!bytes)
           throw exception(__FUNC__, "Unable alloc memory");
        memcpy(bytes, array.bytes, lenght.byte_count());
    }
    //---------------------------------------------------------------------------
    inline bit_array::~bit_array()
    {
        delete[] bytes;
    }
    //---------------------------------------------------------------------------
    inline long bit_array::bit_count() const
    {
        return lenght.bit_count();
    }
    //---------------------------------------------------------------------------
    inline long bit_array::byte_count() const
    {
        return lenght.byte_count();
    }
    //---------------------------------------------------------------------------
    inline const byte bit_array::get_bit(const bit_counter &position) const
    {
    #ifdef GA_DEBUG
        if(position >= lenght)
            throw exception(__FUNC__, "Out of bounds");
    #endif
        return byte((*(bytes + position.get_segment()) >> position.get_offset()) & 1);
    }
    //---------------------------------------------------------------------------
    inline const byte bit_array::get_byte(const bit_counter &position) const
    {
    #ifdef GA_DEBUG
        if(position > lenght - 8)
            throw exception(__FUNC__, "Out of bounds");
    #endif
        byte value = byte(*(bytes + position.get_segment()) >> position.get_offset());
        value |= byte((*(bytes + (position + 7).get_segment()) << (8 - position.get_offset())));
        return value;
    }
    //---------------------------------------------------------------------------
    inline const byte &bit_array::set_bit(const bit_counter &position, const byte &value)
    {
    #ifdef GA_DEBUG
        if(position >= lenght)
            throw exception(__FUNC__, "Out of bounds");
    #endif
        byte *segment = bytes + position.get_segment();
        *segment &= byte(~(1L << position.get_offset()));
        *segment |= byte((value? 1L :0L) << position.get_offset());
        return value;
    }
    //---------------------------------------------------------------------------
    inline const byte &bit_array::set_byte(const bit_counter &position, const byte &value)
    {
    #ifdef GA_DEBUG
        if(position > lenght - 8)
            throw exception(__FUNC__, "Out of bounds");
    #endif
        byte *const low_byte  = bytes + position.get_segment();
        byte *const high_byte = bytes + (position + 7).get_segment();
        *low_byte  &= byte(   (1 << position.get_offset()) - 1);
        *high_byte &= byte( ~((1 << position.get_offset()) - 1));
        *low_byte  |= byte(value << position.get_offset());
        *high_byte |= byte(value >> (8 - position.get_offset()));
        return value;
    }
    //---------------------------------------------------------------------------
    inline void bit_array::inverse_bit(const bit_counter &position)
    {
        *(bytes + position.get_segment()) ^= byte(1 << position.get_offset());
    }
    //---------------------------------------------------------------------------
    inline void bit_array::correct_last_byte()
    {
        if(lenght.get_offset())
            *(bytes + lenght.get_segment()) &= byte((1 << lenght.get_offset()) - 1);
    }
    //---------------------------------------------------------------------------
    inline const bit bit_array::operator[](const bit_counter &position)
    {
        return bit(bytes, position);
    }
    //---------------------------------------------------------------------------
    inline const int bit_array::operator[](const bit_counter &position) const
    {
        return int(bit(bytes, position));
    }
    //---------------------------------------------------------------------------
} // namespace ga
//---------------------------------------------------------------------------
inline std::ostream &operator<< (std::ostream &out, const ga::bit_array &array)
{
    return out << array.to_string();
}
//---------------------------------------------------------------------------
#endif // BITARRAY_H
