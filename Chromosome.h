/****************************************************************************
\cpptex

    \cppfilename{Chromosome.h}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#ifndef CHROMOSOME_H
#define CHROMOSOME_H
//---------------------------------------------------------------------------
#include "BitCount.h"             /// \cppfileref{BitCount.h}
#include "BitArray.h"             /// \cppfileref{BitArray.h}
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    extern const byte gray_to_bin[16];
    extern const byte bin_to_gray[16];
    //---------------------------------------------------------------------------
    extern const byte bits_to_bin[16];
    extern const byte bin_to_bits[16];
    //---------------------------------------------------------------------------
    enum crossing_method{ one_point_crossing, two_point_crossing, uniform_crossing };
    enum mutation_method{ single_point_mutation, multi_point_mutation };
    //---------------------------------------------------------------------------
    /// Реализация мутации, скрещивания и  т.п. \cppclassdef{chromosome}
    //---------------------------------------------------------------------------
    class chromosome : public bit_array
    {
        crossing_method crossing_tech;
        mutation_method mutation_tech;

    public:
        double suitability;
        double fitness;

        chromosome( crossing_method a_crossing = ga::two_point_crossing,
                    mutation_method a_mutation = ga::single_point_mutation );
        chromosome( const chromosome &model );

        const chromosome &operator=( const chromosome &model );

    public:
        void crossing_technique( const crossing_method &method );
        void crossing          ( const chromosome &mather, const chromosome &father );

        void mutation_technique( const mutation_method &method      );
        void mutation          ( const double &mutation_probability );

        void clear( );

        virtual const byte  get_byte( const bit_counter &position ) const;
        virtual const byte &set_byte( const bit_counter &position, const byte &value );

    private:
        void (chromosome::*some_crossing)( const chromosome &mather, const chromosome &father );
        void one_point_crossing          ( const chromosome &mather, const chromosome &father );
        void two_point_crossing          ( const chromosome &mather, const chromosome &father );
        void uniform_crossing            ( const chromosome &mather, const chromosome &father );

        void (chromosome::*some_mutation)( const double &mutation_probability );
        void single_point_mutation       ( const double &mutation_probability );
        void multi_point_mutation        ( const double &mutation_probability );

        byte convert( const byte convert_table[16], const byte &value ) const;
    };
    //---------------------------------------------------------------------------
    inline chromosome::chromosome( const chromosome &model ) :
        bit_array  (model),
        suitability(model.suitability),
        fitness    (model.fitness)
    {
        crossing_technique(model.crossing_tech);
        mutation_technique(model.mutation_tech);
    }
    //---------------------------------------------------------------------------
    inline chromosome::chromosome( crossing_method a_crossing, mutation_method a_mutation ) :
        bit_array    (),
        suitability  (0),
        fitness      (0),
        crossing_tech(a_crossing),
        mutation_tech(a_mutation)
    {
        crossing_technique(crossing_tech);
        mutation_technique(mutation_tech);
    }
    //---------------------------------------------------------------------------
    inline void chromosome::clear( )
    {
        resize(0);
    }
    //---------------------------------------------------------------------------
    inline byte chromosome::convert( const byte convert_table[16], const byte &value ) const
    {
        return (byte)((convert_table[value >> 4] << 4) + convert_table[value & 15]);
    }
    //---------------------------------------------------------------------------
    inline const byte chromosome::get_byte( const bit_counter &position ) const
    {
        return convert(bits_to_bin, bit_array::get_byte(position));
    }
    //---------------------------------------------------------------------------
    inline const byte &chromosome::set_byte( const bit_counter &position, const byte &value )
    {
        bit_array::set_byte(position, convert(bin_to_bits, value));
        return value;
    }
    //---------------------------------------------------------------------------
    inline void chromosome::crossing( const chromosome &mather, const chromosome &father )
    {
        (this->*some_crossing)(mather, father);
    }
    //---------------------------------------------------------------------------
    inline void chromosome::mutation( const double &mutation_probability )
    {
        (this->*some_mutation)(mutation_probability);
    }
    //---------------------------------------------------------------------------
} // namespace ga
#endif // CHROMOSOME_H
