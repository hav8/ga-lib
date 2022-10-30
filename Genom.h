/****************************************************************************
\cpptex

    \cppfilename{Genom.h}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#ifndef GENOM_H
#define GENOM_H
//---------------------------------------------------------------------------
#include "Chromosome.h"
//---------------------------------------------------------------------------
#include <values.h>
#include <algorithm>
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    /// Конкретное значение гена
    //---------------------------------------------------------------------------
    union allele
    {
        bool           boolean;
        unsigned short short_int;
        unsigned int   integer;
        unsigned long  long_int;
        double         real;
    };
    //---------------------------------------------------------------------------
    /// Реализация работы с единичным геном в цепочке \cppclassdef{gene}
    //---------------------------------------------------------------------------
    class gene
    {
        friend class genom;
    public:
        enum degree{ boolean = 1, short_int = 8, integer = 16, long_int = 32, real = 48 };
    private:
        chromosome **individual;
        bit_counter  locus;
        degree       size;
        double       low_limit, high_limit;

        gene( void );
        gene( const gene &model );
        gene( const bit_counter &position, const degree &gene_degree,
              chromosome **somebody, double low = 0, double high = 0 );

        const gene &operator=( const gene &model );

        void   select_allele_interface( void );

    private:
        allele (gene::*allele_interface)( void ) const;

        allele get_boolean   ( void ) const;
        allele get_short_int ( void ) const;
        allele get_integer   ( void ) const;
        allele get_long_int  ( void ) const;
        allele get_real      ( void ) const;

    public:
        allele get_allele    ( void ) const;

        std::string   to_string         ( void )              const;
        std::ostream &put_gene_to_stream( std::ostream &out ) const;
    };

    //---------------------------------------------------------------------------
    inline gene::gene( void ) :
        locus     (0),
        size      (short_int),
        individual(NULL),
        low_limit (0),
        high_limit(0)
    {
        select_allele_interface();
    }
    //---------------------------------------------------------------------------
    inline gene::gene( const gene &model ) :
        locus     (model.locus),
        size      (model.size),
        individual(model.individual),
        low_limit (model.low_limit),
        high_limit(model.high_limit)
    {
        select_allele_interface();
    }
    //---------------------------------------------------------------------------
    inline gene::gene( const bit_counter &position, const gene::degree &gene_degree,
                       chromosome **somebody, double low, double high ) :
        locus     (position),
        size      (gene_degree),
        individual(somebody),
        low_limit (min(low, high)),
        high_limit(max(low, high))
    {
        select_allele_interface();
    }
    //---------------------------------------------------------------------------
    inline const gene &gene::operator=( const gene &model )
    {
        if(this != &model)
        {
            locus      = model.locus;
            size       = model.size;
            individual = model.individual;
            low_limit  = model.low_limit,
            high_limit = model.high_limit;
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    inline allele gene::get_boolean( void ) const
    {
    #ifdef GA_DEBUG
        if(size != boolean)
            throw exception(__FUNC__, "Wrong gene size");
    #endif
        allele value;
        value.boolean = (*individual)->get_bit(locus);
        return value;
    }
    //---------------------------------------------------------------------------
    inline allele gene::get_short_int( void ) const
    {
    #ifdef GA_DEBUG
        if(size != short_int)
            throw exception(__FUNC__, "Wrong gene size");
    #endif
        allele value;
        value.short_int = (*individual)->get_byte(locus);
        return value;
    }
    //---------------------------------------------------------------------------
    inline allele gene::get_integer( void ) const
    {
    #ifdef GA_DEBUG
        if(size != integer)
            throw exception(__FUNC__, "Wrong gene size");
    #endif
        allele value;
        value.integer =  (*individual)->get_byte(locus)
                      + ((*individual)->get_byte(locus + 8) << 8);
        return value;
    }
    //---------------------------------------------------------------------------
    inline allele gene::get_long_int( void ) const
    {
    #ifdef GA_DEBUG
        if(size != long_int)
            throw exception( __FUNC__, "Wrong gene size" );
    #endif
        allele value;
        value.long_int =  (unsigned long)( (*individual)->get_byte(locus) )
                      + ( (unsigned long)( (*individual)->get_byte(locus +  8) ) <<  8)
                      + ( (unsigned long)( (*individual)->get_byte(locus + 16) ) << 16)
                      + ( (unsigned long)( (*individual)->get_byte(locus + 24) ) << 24);
        return value;
    }
    //---------------------------------------------------------------------------
    inline allele gene::get_real( void ) const
    {
    #ifdef GA_DEBUG
        if(size != real)
            throw exception( __FUNC__, "Wrong gene size" );
    #endif
        double first_part  = (unsigned long)( (*individual)->get_byte(locus) )
                          + ((unsigned long)( (*individual)->get_byte(locus +  8) ) <<  8)
                          + ((unsigned long)( (*individual)->get_byte(locus + 16) ) << 16);
        double second_part = (unsigned long)( (*individual)->get_byte(locus + 24) )
                          + ((unsigned long)( (*individual)->get_byte(locus + 32) ) << 8)
                          + ((unsigned long)( (*individual)->get_byte(locus + 40) ) << 16);
        double normalized_real = ( first_part + second_part * double(1UL << 24) )
                               / ( double((1UL << 24) - 1) * double((1UL << 24) - 1) );
        allele value;
        value.real = low_limit + ( high_limit - low_limit ) * normalized_real;
        return value;
    }
    //---------------------------------------------------------------------------
    inline allele gene::get_allele( void ) const
    {
        return (this->*allele_interface)();
    }
    //---------------------------------------------------------------------------
    /// Цепочка генов \cppclassdef{genom}
    //---------------------------------------------------------------------------
    class genom
    {
        friend class evolution;
    private:
        long        gene_count;
        const long  max_gene_count;
        gene      **gene_chain;
        chromosome *individual;
        long        chromosome_lenght;
    private:
        genom( long max_count = 1024 );
        genom( const genom &model    );
        virtual ~genom( void );
        const genom &operator=( const genom &model );

    public:
        allele      operator[]( const long index ) const;
        const gene &get_gene  ( const long index ) const;
    private:
        const genom &assign  ( chromosome *somebody );
        long         add_gene( const gene::degree &size, double low_limit = 0, double high_limit = 0 );
        void         clear   ( void );

    public:
        friend double range  ( const genom &one, const genom &two );
    };
    //---------------------------------------------------------------------------
    inline genom::genom( long max_count ) :
        max_gene_count(max_count),
        gene_count(0),
        gene_chain(NULL),
        individual(NULL),
        chromosome_lenght(0)
    {
        gene_chain = new gene*[max_gene_count];
    #ifdef GA_DEBUG
        if(!gene_chain)
             throw exception(__FUNC__, "Unable allocate memory");
    #endif
    }
    //---------------------------------------------------------------------------
    inline genom::~genom( void )
    {
        delete[] gene_chain;
    }
    //---------------------------------------------------------------------------
    inline allele genom::operator[]( const long index ) const
    {
    #ifdef GA_DEBUG
        if(index < 0 || index >= gene_count)
            throw exception(__FUNC__, "Wrong gene index");
    #endif
        return (*(gene_chain + index))->get_allele(); 
    }
    //---------------------------------------------------------------------------
    inline const gene &genom::get_gene( const long index ) const
    {
    #ifdef GA_DEBUG
        if(index < 0 || index >= gene_count)
            throw exception(__FUNC__, "Wrong gene index");
    #endif
        return **(gene_chain + index);
    }
    //---------------------------------------------------------------------------
    inline const genom &genom::assign( chromosome *somebody )
    {
    #ifdef GA_DEBUG
        if(somebody->bit_count() != chromosome_lenght)
            throw exception(__FUNC__, "Unable assign chromosome - invalid size");
    #endif
        individual = somebody;
        return *this;
    }
    //---------------------------------------------------------------------------
} // namespace ga
//---------------------------------------------------------------------------
inline std::ostream &operator<<( std::ostream &out, const ga::gene &some_gene )
{
    return some_gene.put_gene_to_stream(out);
}
//---------------------------------------------------------------------------
#endif // GENOM_H
