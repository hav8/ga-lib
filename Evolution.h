/****************************************************************************
\cpptex

    \cppfilename{Evolution.h}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#ifndef EVOLUTION_H
#define EVOLUTION_H
//---------------------------------------------------------------------------
#include "Chromosome.h"          /// \cppfileref{Chromosome.h}
#include "Genom.h"               /// \cppfileref{Genom.h}
#include <algorithm>

//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    /// Подмешивание предоставляющее возможность применения ГА. \cppclassdef{optimizible}
    //---------------------------------------------------------------------------
    class optimizible
    {
    public:
        virtual double fitness_function( const genom &chain ) = 0;
    };
    //---------------------------------------------------------------------------
    /// Класс для представления глобальных функций \cppclassdef{global\_fitness\_function}
    //---------------------------------------------------------------------------
    class global_fitness_function : public optimizible
    {
        double (*global_function)( const genom & );
    public:
        global_fitness_function( double (*function)(const genom &) );
        virtual double fitness_function( const genom &chain );
    };
    //---------------------------------------------------------------------------
    inline global_fitness_function::global_fitness_function( double (*function)(const genom &) ) :
        global_function(function)
    {
    }
    //---------------------------------------------------------------------------
    inline double global_fitness_function::fitness_function( const genom &chain )
    {
        return global_function(chain);
    }
    //---------------------------------------------------------------------------
    enum selection_method{ roulette_selection, tournament_selection, truncation_selection };
    //---------------------------------------------------------------------------
    /// Собственно генетический алгоритм. \cppclassdef{evolution}
    //---------------------------------------------------------------------------
    class evolution
    {
        selection_method selection_tech;

        chromosome      *curr_generation;
        chromosome      *next_generation;
        chromosome      *best_chromosome;
        genom            gene_chain;

        long             population_size;
        long             max_population_size;

        long             epoch;
        long             stability_epoch_count;

        bool             elitism;
        bool             minimization;
        bool             const_population_size;

        double           crossing_probability;
        double           mutation_probability;

        double           fitness;
        double           min_fitness;
        double           max_fitness;

    public:
        evolution( selection_method a_selection = ga::truncation_selection,
                   crossing_method  a_crossing  = ga::two_point_crossing,
                   mutation_method  a_mutation  = ga::single_point_mutation );

        virtual ~evolution( void );

        double minimize( optimizible *optimizible_object,   long epoch_count, double error = 0 );
        double maximize( optimizible *optimizible_object,   long epoch_count, double error = 0 );

        double minimize( double (*function)(const genom &), long epoch_count, double error = 0 );
        double maximize( double (*function)(const genom &), long epoch_count, double error = 0 );

        const genom &best_genom     ( void );
        allele       best_genom_gene( const long &index );

        void clear_genom  ( void );
        long add_boolean  ( void );
        long add_short_int( void );
        long add_integer  ( void );
        long add_long_int ( void );
        long add_real     ( double low_limit, double high_limit );

        void selection_technique     ( const selection_method &method );
        void resize_population       ( const long &size );

        void set_crossing_probability( double probability );
        void set_mutation_probability( double probability );

    private:
        double optimization( optimizible *optimizible_object, long epoch_count, double error );

        void initialization       ( optimizible *optimizible_object );
        void next_epoch           ( optimizible *optimizible_object );

        void calculate_fitness    ( optimizible *optimizible_object );
        void calculate_suitability( optimizible *optimizible_object );

        void (evolution::*natural_suitability)( void );
        void exponential_ranking_suitability  ( void );
        void linear_ranking_suitability       ( void );
        void proportionate_suitability        ( void );

        const chromosome &selection           ( void );
        const chromosome &(evolution::*natural_selection)( void );
        const chromosome &roulette_selection  ( void );
        const chromosome &tournament_selection( void );
        const chromosome &truncation_selection( void );

        void alloc_generations( const chromosome &model );
        void free_generations ( void );

    };
    //---------------------------------------------------------------------------
    inline long evolution::add_boolean()
    {
        return gene_chain.add_gene(gene::boolean);
    }
    //---------------------------------------------------------------------------
    inline long evolution::add_short_int()
    {
        return gene_chain.add_gene(gene::short_int);
    }
    //---------------------------------------------------------------------------
    inline long evolution::add_integer()
    {
        return gene_chain.add_gene(gene::integer);
    }
    //---------------------------------------------------------------------------
    inline long evolution::add_long_int()
    {
        return gene_chain.add_gene(gene::long_int);
    }
    //---------------------------------------------------------------------------
    inline long evolution::add_real(double low_limit, double high_limit)
    {
        return gene_chain.add_gene(gene::real, low_limit, high_limit);
    }
    //---------------------------------------------------------------------------
    inline void evolution::clear_genom()
    {
        gene_chain.clear();
    }
    //---------------------------------------------------------------------------
    inline const genom &evolution::best_genom()
    {
        return gene_chain.assign(best_chromosome);
    }
    //---------------------------------------------------------------------------
    inline void evolution::calculate_suitability(optimizible *optimizible_object)
    {
        calculate_fitness(optimizible_object);
        (this->*natural_suitability)();
    }
    //---------------------------------------------------------------------------
    inline const chromosome &evolution::selection()
    {
        return (this->*natural_selection)();
    }
    //---------------------------------------------------------------------------
    inline double evolution::minimize(double (*function)(const genom &), long epoch_count, double error)
    {
        return minimize(&global_fitness_function(function), epoch_count, error);
    }
    //---------------------------------------------------------------------------
    inline double evolution::maximize(double (*function)(const genom &), long epoch_count, double error)
    {
        return maximize(&global_fitness_function(function), epoch_count, error);
    }
    //---------------------------------------------------------------------------
    inline double evolution::minimize(optimizible *optimizible_object, long epoch_count, double error)
    {
        minimization = true;
        return optimization(optimizible_object, epoch_count, error);
    }
    //---------------------------------------------------------------------------
    inline double evolution::maximize(optimizible *optimizible_object, long epoch_count, double error)
    {
        minimization = false;
        return optimization(optimizible_object, epoch_count, error);
    }
    //---------------------------------------------------------------------------
    inline allele evolution::best_genom_gene(const long &index)
    {
        return best_genom()[index];
    }
    //---------------------------------------------------------------------------
    inline void evolution::set_crossing_probability(double probability)
    {
        crossing_probability = probability;
    }
    //---------------------------------------------------------------------------
    inline void evolution::set_mutation_probability(double probability)
    {
        mutation_probability = probability;
    }
    //---------------------------------------------------------------------------
} // namespace ga
#endif  //  EVOLUTION_H
