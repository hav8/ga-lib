/****************************************************************************
\cpptex

    \cppfilename{BitCount.h}

    ¬ файле "<BitCount.h"> содержитс€ описание класса {\cppprogram bit\_counter}.
     ласс реализует работу со счетчиками битов, образующих
    битовую последовательность, основанием которой €вл€етс€ тип {\cppprogram byte}.

                                                 \cppsignature{’арченко ј.¬.}
****************************************************************************/
#ifndef BITCOUNT_H
#define BITCOUNT_H
//---------------------------------------------------------------------------
#include "GAException.h"             /// \cppfileref{GAException.h}
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    typedef unsigned char byte;            /// пакет битов (TODO: любое беззнаковое целое)

    const long byte_max  = UCHAR_MAX;      /// максимальное значение байта
    const long byte_bits = CHAR_BIT;       /// количество бит в байте
    const long byte_exp  = 3;              /// дл€ замены делени€ битовым сдвигом (TODO: нужно вычисл€ть)
    const long byte_mask = byte_bits - 1;  /// дл€ замены вз€ти€ остатка логическим "<и">
    const long byte_var  = 1 << byte_bits; /// количество различных значений

    //---------------------------------------------------------------------------
    /// ”казатель на бит в массиве \cppclassdef{bit\_counter}
    //---------------------------------------------------------------------------
    class bit_counter
    {
        long segment;
        int  offset;

    public:
        bit_counter( void );
        bit_counter( const long &bit_full_count );
        bit_counter( const bit_counter &counter );
        bit_counter( const long &byte_count, const int &bit_count );

        long bit_count  ( void ) const;
        long byte_count ( void ) const;
        long get_offset ( void ) const;
        long get_segment( void ) const;

        const  bit_counter &add_byte( const long &byte_value );
        const  bit_counter &sub_byte( const long &byte_value );

        const  bit_counter &operator= ( const bit_counter &counter );
        const  bit_counter &operator+=( const bit_counter &counter );
        const  bit_counter &operator-=( const bit_counter &counter );
        const  bit_counter &operator++( void );
        const  bit_counter &operator++( int );
        const  bit_counter &operator--( void );
        const  bit_counter &operator--( int );
        friend bit_counter  operator+ ( const bit_counter &first, const bit_counter &second );
        friend bit_counter  operator- ( const bit_counter &first, const bit_counter &second );
        friend bool         operator!=( const bit_counter &first, const bit_counter &second );
        friend bool         operator==( const bit_counter &first, const bit_counter &second );
        friend bool         operator>=( const bit_counter &first, const bit_counter &second );
        friend bool         operator<=( const bit_counter &first, const bit_counter &second );
        friend bool         operator< ( const bit_counter &first, const bit_counter &second );
        friend bool         operator> ( const bit_counter &first, const bit_counter &second );
    };
    //-----------------------------------------------------------------------------
    inline bit_counter::bit_counter( void ) :
        segment( 0 ),
        offset ( 0 )
    {
    }
    //-----------------------------------------------------------------------------
    inline bit_counter::bit_counter( const long &bit_full_count ) :
        segment( bit_full_count >> byte_exp ), // аналог (bit_full_count / (sizeof(byte) * 8))
        offset ( bit_full_count & byte_mask )  // аналог (bit_full_count % (sizeof(byte) * 8))
    {
        #ifdef GA_DEBUG
        if( bit_full_count < 0 )
            throw exception( __FUNC__, "Negative full bit count" );
        #endif
    }
    //-----------------------------------------------------------------------------
    inline bit_counter::bit_counter( const bit_counter &counter ) :
        segment( counter.segment ),
        offset ( counter.offset )
    {
    }
    //-----------------------------------------------------------------------------
    inline bit_counter::bit_counter( const long &byte_count, const int &bit_count ) :
        segment( byte_count + ( bit_count >> byte_exp ) ),
        offset ( bit_count & byte_mask )
    {
        #ifdef GA_DEBUG
        if( bit_count < 0 )
            throw exception( __FUNC__, "Negative bit count" );
        if( byte_count < 0 )
            throw exception( __FUNC__, "Negative segment count" );
        #endif
    }
    //-----------------------------------------------------------------------------
    inline long bit_counter::bit_count( void ) const
    {
        return ( segment << byte_exp ) | offset;
    }
    //-----------------------------------------------------------------------------
    inline long bit_counter::byte_count( void ) const
    {
        return segment + ( offset ? 1 : 0 );
    }
    //-----------------------------------------------------------------------------
    inline long bit_counter::get_segment( void ) const
    {
        return segment;
    }
    //-----------------------------------------------------------------------------
    inline long bit_counter::get_offset( void ) const
    {
        return offset;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::add_byte( const long &byte_value )
    {
        segment += byte_value;
        #ifdef GA_DEBUG
        if( segment < 0 )
            throw exception( __FUNC__, "Negative segment count" );
        #endif
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::sub_byte( const long &byte_value )
    {
        segment -= byte_value;
        #ifdef GA_DEBUG
        if( segment < 0 )
            throw exception( __FUNC__, "Negative segment count" );
        #endif
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator= ( const bit_counter &counter )
    {
        offset  = counter.offset;
        segment = counter.segment;
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator+=( const bit_counter &counter )
    {
        offset  += counter.offset;
        segment += counter.segment + ( offset >> byte_exp );
        offset  &= byte_mask;
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator-=( const bit_counter &counter )
    {
        offset  += byte_bits - counter.offset;
        segment += ( offset >> byte_exp ) - 1 - counter.segment;
        offset  &= byte_mask;
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator++( void )
    {
        offset++;
        segment += ( offset >> byte_exp );
        offset  &= byte_mask;
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator++( int )
    {
        offset++;
        segment += ( offset >> byte_exp );
        offset  &= byte_mask;
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator--( void )
    {
        offset  += byte_bits - 1;
        segment += ( offset >> byte_exp ) - 1;
        offset  &= byte_mask;
        #ifdef GA_DEBUG
        if( segment < 0 )
            throw exception( __FUNC__, "Negative segment count" );
        #endif
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline const bit_counter &bit_counter::operator--( int )
    {
        offset  += byte_bits - 1;
        segment += ( offset >> byte_exp ) - 1;
        offset  &= byte_mask;
        #ifdef GA_DEBUG
        if( segment < 0 )
            throw exception( __FUNC__, "Negative segment count" );
        #endif
        return *this;
    }
    //-----------------------------------------------------------------------------
    inline bit_counter operator+ ( const bit_counter &first, const bit_counter &second )
    {
        return bit_counter( first.bit_count() + second.bit_count() );
    }
    //-----------------------------------------------------------------------------
    inline bit_counter operator- ( const bit_counter &first, const bit_counter &second )
    {
        return bit_counter( first.bit_count() - second.bit_count() );
    }
    //-----------------------------------------------------------------------------
    inline bool operator!=( const bit_counter &first, const bit_counter &second )
    {
        return ( first.segment != second.segment ) || ( first.offset != second.offset );
    }
    //-----------------------------------------------------------------------------
    inline bool operator==( const bit_counter &first, const bit_counter &second )
    {
        return ( first.segment == second.segment ) && ( first.offset == second.offset );
    }
    //-----------------------------------------------------------------------------
    inline bool operator>=( const bit_counter &first, const bit_counter &second )
    {
        return ( first.segment > second.segment )
            || ( first.segment == second.segment && first.offset >= second.offset );
    }
    //-----------------------------------------------------------------------------
    inline bool operator<=( const bit_counter &first, const bit_counter &second )
    {
        return ( first.segment < second.segment )
            || ( first.segment == second.segment && first.offset <= second.offset );
    }
    //-----------------------------------------------------------------------------
    inline bool operator< ( const bit_counter &first, const bit_counter &second )
    {
        return ( first.segment < second.segment )
            || ( first.segment == second.segment && first.offset < second.offset );
    }
    //-----------------------------------------------------------------------------
    inline bool operator> ( const bit_counter &first, const bit_counter &second )
    {
        return ( first.segment > second.segment )
            || ( first.segment == second.segment && first.offset > second.offset );
    }
    //-----------------------------------------------------------------------------
}   // namespace ga
//-----------------------------------------------------------------------------
#endif // BITCOUNT_H



