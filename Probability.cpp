/****************************************************************************
\cpptex

    \cppfilename{Probability.cpp}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#include "Probability.h"
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    void random_generator::fill_buffer( void )
    {
        unsigned long *current = buffer;
        unsigned long *const end = buffer + ( buffer_size - 1 );
        while( current <= end )
            *current++ = X.next_element();
    }
    //---------------------------------------------------------------------------
    /// Функция-обертка для статического генератора случайных чисел
    //---------------------------------------------------------------------------
    unsigned long generate_random_number( bool random_number )
    {
        /*
        static random_generator generator( true );
        return random_number ? generator.next_element() : generator.rand_max();
        */
        return random_number ? ::rand() : RAND_MAX;
    }
    //---------------------------------------------------------------------------
} // namespace ga
