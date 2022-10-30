/****************************************************************************
\cpptex

    \cppfilename{Chromosome.cpp}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#include "Chromosome.h"           /// \cppfileref{Chromosome.h}
//---------------------------------------------------------------------------
#include <algorithm>
#pragma hdrstop
//---------------------------------------------------------------------------
namespace ga
{
    /****************************************************************************
        \cpptex  Массивы перевода между обычным двоичным кодированием и кодом Грея

        \begin{tabular}{cccccccccccccccc}
        $0000$&$0001$&$0011$&$0010$&$0110$&$0111$&$0101$&$0100$&$1100$&$1101$&$1111$&$1110$&$1010$&$1011$&$1001$&$1000$\\
        $0$   & $1$  & $3$  & $2$  & $6$  & $7$  & $5$  & $4$  & $C$  & $D$  & $F$  & $E$  & $A$  & $B$  & $9$  & $8$  \\
        $0$   & $1$  & $2$  & $3$  & $4$  & $5$  & $6$  & $7$  & $8$  & $9$  & $A$  & $B$  & $C$  & $D$  & $E$  & $F$  \\
        \end{tabular}

        На мой взгляд обладает существенным недостатком: $F$ отличается от $0$
        на один бит, а это неправельно.
    ****************************************************************************/
    const byte gray_to_bin[16] = { 0x0, 0x1, 0x2, 0x3, 0x7, 0x6, 0x4, 0x5,
                                   0xF, 0xE, 0xC, 0xD, 0x8, 0x9, 0xB, 0xA };
    const byte bin_to_gray[16] = { 0x0, 0x1, 0x3, 0x2, 0x6, 0x7, 0x5, 0x4,
                                   0xC, 0xD, 0xF, 0xE, 0xA, 0xB, 0x9, 0x8 };
    /****************************************************************************
        \cpptex Массивы перевода между обычным двоичным кодированием и моим кодом

        \begin{tabular}{cccccccccccccccc}
        $0000$&$0001$&$0010$&$0100$&$1000$&$1001$&$1010$&$1100$&$0110$&$0101$&$0011$&$0111$&$1011$&$1101$&$1110$&$1111$\\
        $0$   & $1$  & $2$  & $4$  & $8$  & $9$  & $A$  & $C$  & $6$  & $5$  & $3$  & $7$  & $B$  & $D$  & $E$  & $F$  \\
        $0$   & $1$  & $2$  & $3$  & $4$  & $5$  & $6$  & $7$  & $8$  & $9$  & $A$  & $B$  & $C$  & $D$  & $E$  & $F$  \\
        \end{tabular}

        Тут другая проблема --- переход к другому шестнадцатеричному разряду.
    ****************************************************************************/
    const byte bits_to_bin[16] = { 0x0, 0x1, 0x2, 0xA, 0x3, 0x9, 0x8, 0xB,
                                   0x4, 0x5, 0x6, 0xC, 0x7, 0xD, 0xE, 0xF };
    const byte bin_to_bits[16] = { 0x0, 0x1, 0x2, 0x4, 0x8, 0x9, 0xA, 0xC,
                                   0x6, 0x5, 0x3, 0x7, 0xB, 0xD, 0xE, 0xF };
    //---------------------------------------------------------------------------
    const chromosome &chromosome::operator=( const chromosome &model )
    {
        if(this != &model)
        {
            bit_array::operator=(model);
            suitability = model.suitability;
            fitness = model.fitness;
            crossing_technique(model.crossing_tech);
            mutation_technique(model.mutation_tech);
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    void chromosome::crossing_technique( const crossing_method &method )
    {
        switch(crossing_tech = method)
        {
        case ga::one_point_crossing:
            some_crossing = &chromosome::one_point_crossing;
            break;
        case ga::two_point_crossing:
            some_crossing = &chromosome::two_point_crossing;
            break;
        case ga::uniform_crossing:
            some_crossing = &chromosome::uniform_crossing;
            break;
        default:
            throw exception(__FUNC__, "Wrong crossing method");
        }
    }
    //---------------------------------------------------------------------------
    void chromosome::one_point_crossing( const chromosome &mather, const chromosome &father )
    {
        /**********************************************************************
            \cpptex \cppclassmember{chromosome}{one\_point\_crossing}

            Одноточечный кроссовер. Случайным образом выбирается точка
            в хромосоме, после чего от двух родительских особей соответсвующие части.
            \begin{figure}[h]
                \centering
                \small
                \begin{tabular}{lccccc|ccccccccc}
                Родитель A & 1 & 1 & 1 & 0 & 1 & 0 & 0 & 1 & 1 & 0 & 0 & 1 & 0 & 0\\
                           &$\downarrow$&$\downarrow$&$\downarrow$&$\downarrow$&$\downarrow$&&&&&&&&&\\
                Потомок    & 1 & 1 & 1 & 0 & 1 & 0 & 1 & 1 & 1 & 0 & 1 & 1 & 0 & 1\\
                           &&&&&&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$\\
                Родитель B & 1 & 0 & 0 & 1 & 0 & 0 & 1 & 1 & 1 & 0 & 1 & 1 & 0 & 1\\
                \end{tabular}
                \caption{Схема одноточечного коссовера}\label{fig:TwoPointCrossover}
            \end{figure}

        **********************************************************************/
        long position = random_long(mather.bit_count() - 1) + 1;

        copy(mather, 0,        position - 1);
        copy(father, position, father.bit_count() - 1);

    }
    //---------------------------------------------------------------------------
    void chromosome::two_point_crossing( const chromosome &mather, const chromosome &father )
    {
        /**********************************************************************
            \cpptex \cppclassmember{chromosome}{two\_point\_crossing}

            Двухточечный кроссовер. Случайным образом выбираются две
            отличные друг от друга точки в хромосоме, после чего от двух
            родительских особей берутся соответсвующие части при
            создании потомка.
            \begin{figure}[h]
                \centering
                \small
                \begin{tabular}{lccc|cccccc|ccccc}
                Родитель A & 1 & 1 & 1 & 0 & 1 & 0 & 0 & 1 & 1 & 0 & 0 & 1 & 0 & 0\\
                           &$\downarrow$&$\downarrow$&$\downarrow$&&&&&&&$\downarrow$&$\downarrow$&$\downarrow$&$\downarrow$&$\downarrow$\\
                Потомок    & 1 & 1 & 1 & 1 & 0 & 0 & 1 & 1 & 1 & 0 & 0 & 1 & 0 & 0\\
                           &&&&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&$\uparrow$&&&&& \\
                Родитель B & 1 & 0 & 0 & 1 & 0 & 0 & 1 & 1 & 1 & 0 & 1 & 1 & 0 & 1\\
                \end{tabular}
                \caption{Схема двухточечного коссовера}\label{fig:TwoPointCrossover}
            \end{figure}

        **********************************************************************/
        long position_one = random_long(mather.bit_count() - 1) + 1;
        long position_two = random_long(father.bit_count() - 1) + 1;
        while(position_one == position_two)
            position_two  = random_long(father.bit_count() - 1) + 1;
        long min_position = min(position_one, position_two);
        long max_position = max(position_one, position_two);

        copy(mather, 0,            min_position - 1);
        copy(father, min_position, max_position - 1);
        copy(mather, max_position, mather.bit_count() - 1);

    }
    //---------------------------------------------------------------------------
    void chromosome::uniform_crossing( const chromosome &mather, const chromosome &father )
    {
        /**********************************************************************
            \cpptex \cppclassmember{chromosome}{uniform\_crossing}

            Униформный кроссовер. Случайным образом выбираются биты из
            одной или другой родительской особи.
            \begin{figure}[h]
                \centering
                \small
                \begin{tabular}{lcccccccccccccc}
                    Маска      & 0 & 1 & 0 & 0 & 1 & 1 & 0 & 0 & 0 & 1 & 0 & 1 & 1 & 1\\ \hline
                    Родитель A & 1 & 1 & 1 & 0 & 1 & 0 & 0 & 1 & 1 & 0 & 0 & 1 & 0 & 0\\
                               &&$\downarrow$&&&$\downarrow$&$\downarrow$&&&&$\downarrow$&&$\downarrow$&$\downarrow$&$\downarrow$\\
                    Потомок    & 1 & 1 & 0 & 1 & 1 & 0 & 1 & 1 & 1 & 0 & 1 & 1 & 0 & 0\\
                               &$\uparrow$&&$\uparrow$&$\uparrow$&&&$\uparrow$&$\uparrow$&$\uparrow$&&$\uparrow$&&&\\
                    Родитель B & 1 & 0 & 0 & 1 & 0 & 0 & 1 & 1 & 1 & 0 & 1 & 1 & 0 & 1\\
                \end{tabular}
                \caption{Схема униформного коссовера}\label{fig:TwoPointCrossover}
            \end{figure}

        **********************************************************************/
        bit_array mask(mather), mather_part, father_part;
        mather_part.let_and(mather, mask.random_fill());
        father_part.let_and(father, mask.flip());
        let_or(mather_part, father_part);
    }
   //---------------------------------------------------------------------------
    void chromosome::mutation_technique( const mutation_method &method )
    {
        switch(mutation_tech = method)
        {
        case ga::single_point_mutation:
            some_mutation = &chromosome::single_point_mutation;
            break;
        case ga::multi_point_mutation:
            some_mutation = &chromosome::multi_point_mutation;
            break;
        default:
            throw exception(__FUNC__, "Wrong mutation method");
        }
    }
    //---------------------------------------------------------------------------
    void chromosome::single_point_mutation( const double &mutation_probability )
    {
        /// Случайным образом инвертировали один бит \cppclassmember{chromosome}{single\_point\_mutation}
        inverse_bit(random_long(bit_count()));
    }
    //---------------------------------------------------------------------------
    void chromosome::multi_point_mutation( const double &mutation_probability )
    {
        /// С некоторой вероятностью попробовали инвертировать все биты \cppclassmember{chromosome}{multi\_point\_mutation}
        bit_counter bit_index = bit_count();
        while(bit_index-- > 0)
            if(random_real() < mutation_probability)
                inverse_bit(bit_index - 1);
    }
    //---------------------------------------------------------------------------
} // namespace ga
