/****************************************************************************
\cpptex

    \cppfilename{Genom.cpp}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#include "Genom.h"
#pragma hdrstop
//---------------------------------------------------------------------------
namespace ga
{
//---------------------------------------------------------------------------
    void gene::select_allele_interface( void )
    {
        switch(size)
        {
        case boolean:   allele_interface = &gene::get_boolean;   break;
        case short_int: allele_interface = &gene::get_short_int; break;
        case integer:   allele_interface = &gene::get_integer;   break;
        case long_int:  allele_interface = &gene::get_long_int;  break;
        case real:      allele_interface = &gene::get_real;      break;
        default:
            throw exception(__FUNC__, "Wrong size");
        }
    }
//---------------------------------------------------------------------------
    std::ostream &gene::put_gene_to_stream( std::ostream &out ) const
    {
        switch(size)
        {
        case boolean:    return out << get_allele().boolean;
        case short_int:  return out << get_allele().short_int;
        case integer:    return out << get_allele().integer;
        case long_int:   return out << get_allele().long_int;
        case real:       return out << get_allele().real;
        default:
            throw exception(__FUNC__, "Wrong size");
        }
    }
//---------------------------------------------------------------------------
    std::string gene::to_string( void ) const
    {
        bit_array this_gene(**individual);
        return this_gene.subarray(this_gene, locus, locus + (size - 1)).to_string();
    }
//---------------------------------------------------------------------------
    genom::genom( const genom &model ) :
        max_gene_count(model.max_gene_count),
        gene_count(model.gene_count),
        gene_chain(NULL),
        individual(model.individual),
        chromosome_lenght(model.chromosome_lenght)
    {
        gene_chain = new gene*[max_gene_count];

        if(!gene_chain)
             throw exception(__FUNC__, "Unable allocate memory");

        gene **model_chain = model.gene_chain;
        gene **current     = gene_chain;
        gene **const end   = gene_chain + (gene_count - 1);
        while(current <= end)
            *current++ = new gene(**model_chain++);
    }
//---------------------------------------------------------------------------
    const genom &genom::operator=( const genom &model )
    {
        if(this != &model)
        {
            if(max_gene_count < model.gene_count)
                    throw exception(__FUNC__, "Wrong model size");

            clear();
            gene_count = model.gene_count;
            gene **model_chain = model.gene_chain;
            gene **current     = gene_chain;
            gene **const end   = gene_chain + (gene_count - 1);
            while(current <= end)
                *current++ = new gene(**model_chain++);
            individual = model.individual;
            chromosome_lenght = model.chromosome_lenght;
        }
        return *this;
    }
//---------------------------------------------------------------------------
    void genom::clear( void )
    {
        gene **current   = gene_chain;
        gene **const end = gene_chain + (gene_count - 1);
        while(current <= end)
            delete *current++;
        chromosome_lenght = 0;
        gene_count = 0;
    }
//---------------------------------------------------------------------------
    long genom::add_gene( const gene::degree &size, double low_limit, double high_limit )
    {
        if(gene_count == max_gene_count)
            throw exception(__FUNC__, "Unable add gene");

        *(gene_chain + gene_count) = new gene(chromosome_lenght, size, &individual, low_limit, high_limit);
        individual->resize(chromosome_lenght += size);
        return gene_count++;
    }
//---------------------------------------------------------------------------
} // namespace ga

