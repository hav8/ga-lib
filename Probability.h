/****************************************************************************
\cpptex

    \cppfilename{Probability.h}

    В файле "<Probability.h"> содержится описание функций для генерации
    случайных чисел.

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#ifndef PROBABILITY_H
#define PROBABILITY_H
//---------------------------------------------------------------------------
#include "BitCount.h"
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <time.h>
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    /// Линейная конгруентная последовательность
    //---------------------------------------------------------------------------
    class linear_congruent_secuence
    {
        const unsigned long A, B, M;
        unsigned long       X;
    public:
       linear_congruent_secuence( const unsigned long &a, const unsigned long &b, const unsigned long &m, unsigned long x = 0 );
       unsigned long next_element( void );
       unsigned long module      ( void ) const;
       void          seed        ( const unsigned long &x );
       void          randomize   ( void );
    };
    //---------------------------------------------------------------------------
    inline linear_congruent_secuence::linear_congruent_secuence( const unsigned long &a, const unsigned long &b,
                                                          const unsigned long &m, unsigned long x ) :
        X( x ),
        A( a ),
        B( b ),
        M( m )
    {
        if( !X )
            randomize();
    }
    //---------------------------------------------------------------------------
    inline unsigned long linear_congruent_secuence::next_element( void )
    {
        return X = ( A * X + B ) % M;
    }
    //---------------------------------------------------------------------------
    inline unsigned long linear_congruent_secuence::module( void ) const
    {
        return M;
    }
    //---------------------------------------------------------------------------
    inline void linear_congruent_secuence::seed( const unsigned long &x )
    {
        X = x;
    }
    //---------------------------------------------------------------------------
    inline void linear_congruent_secuence::randomize( void )
    {
        seed( time( NULL ) );
    }
    //---------------------------------------------------------------------------
    /// Вполне случайная последовательность
    //---------------------------------------------------------------------------
    class random_generator
    {
        linear_congruent_secuence X, Y;
        enum                      { buffer_size = 256 };
        unsigned long             buffer[buffer_size];
    public:
        random_generator   ( const bool &randomized );

        unsigned long   rand_max    ( void ) const;
        unsigned long   next_element( void );
        void            randomize   ( void );
    private:
        void            fill_buffer ( void );
    };
    //---------------------------------------------------------------------------
    inline random_generator::random_generator( const bool &randomized ) :
        X( 3141592653UL, 2718281829UL, 1UL << 30, 5772156649UL ),
        Y( 2718281829UL, 3141592653UL, 1UL << 30, 1781072418UL )
    {
        if( randomized )
            randomize();
        fill_buffer();
    }
    //---------------------------------------------------------------------------
    inline void random_generator::randomize( void )
    {
        X.randomize();
        Y.randomize();
        fill_buffer();
    }
    //---------------------------------------------------------------------------
    inline unsigned long random_generator::rand_max( void ) const
    {
        return X.module() - 1;
    }
    //---------------------------------------------------------------------------
    inline unsigned long random_generator::next_element( void )
    {
        long j = Y.next_element() * buffer_size / Y.module();
        long random_number = *(buffer + j);
        *(buffer + j) = X.next_element();
        return random_number;
    }
    //---------------------------------------------------------------------------
    /// Функция-обертка для статического генератора случайных чисел
    //---------------------------------------------------------------------------
    unsigned long generate_random_number( bool );
    //---------------------------------------------------------------------------
    inline unsigned long rand( void )
    {
        return generate_random_number( true );
    }
    //---------------------------------------------------------------------------
    inline unsigned long rand_max( void )
    {
        return generate_random_number( false );
    }
    //---------------------------------------------------------------------------
    /// Случайное число из интервала $[0;1]$      \cppglobalfunc{random\_real}
    //---------------------------------------------------------------------------
    inline double random_real( void )
    {
        return double( rand() ) / double( rand_max() );
    }
    //---------------------------------------------------------------------------
    /// Вещественное случайное число из интервала $[\text{\cppp{low}};\text{\cppp{high}}]$
    //---------------------------------------------------------------------------
    inline double random_real( const double &low, const double &high )
    {
        return low + ( high - low ) * random_real();
    }
    //---------------------------------------------------------------------------
    /// Целое случайное число из нтервала $[0;\text{\cppp{limit}})$ \cppglobalfunc{random\_long}
    //---------------------------------------------------------------------------
    inline long random_long( const long &limit )
    {                                     
        return limit * double( rand() ) / double( rand_max() + 1 );
    }
    //---------------------------------------------------------------------------
    /// Случайное распределение битов в пакете определяемом типом \cppp{byte} \cppglobalfunc{random\_byte}
    //---------------------------------------------------------------------------
    inline byte random_byte( void )
    {
        return ( double( byte_max + 1 ) * double( rand() ) ) / double( rand_max() + 1);
    }
    //---------------------------------------------------------------------------
    /// Случайное булево значение \cppk{true} или \cppk{false}  \cppglobalfunc{random\_bool}
    //---------------------------------------------------------------------------
    inline bool random_bool( void )
    {
        return random_real() > 0.5;
    }
    //---------------------------------------------------------------------------
} // namespace ga
//---------------------------------------------------------------------------
#endif // PROBABILITY_H
