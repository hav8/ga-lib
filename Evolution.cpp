/****************************************************************************
\cpptex

    \cppfilename{Evolution.cpp}

                                                 \cppsignature{Харченко А.В.}
****************************************************************************/
#include "Evolution.h"         /// \cppfileref{Evolution.h}
#pragma hdrstop
//---------------------------------------------------------------------------
namespace ga
{
    //---------------------------------------------------------------------------
    evolution::evolution( selection_method a_selection, crossing_method  a_crossing,
                                                        mutation_method  a_mutation ) :
        epoch(0),
        stability_epoch_count(64),

        minimization(true),

        crossing_probability (0.6),
        mutation_probability (0.2),

        selection_tech(a_selection),

        elitism(true),

        const_population_size(false),
        max_population_size  (256),
        population_size      (50),

        curr_generation(NULL),
        next_generation(NULL),

        best_chromosome(NULL),
        gene_chain()
    {
        alloc_generations  (chromosome(a_crossing, a_mutation));
        selection_technique(selection_tech);
        gene_chain.assign  (best_chromosome);
    }
    //---------------------------------------------------------------------------
    evolution::~evolution( void )
    {
        free_generations();
    }
    //---------------------------------------------------------------------------
    void evolution::alloc_generations( const chromosome &model )
    {
        curr_generation = new chromosome[max_population_size];
        if(!curr_generation)
            throw exception(__FUNC__, "Unable allocate memory");

        next_generation = new chromosome[max_population_size];
        if(!next_generation)
            throw exception(__FUNC__, "Unable allocate memory");

        best_chromosome  = curr_generation;
        *best_chromosome = model;
    }
    //---------------------------------------------------------------------------
    void evolution::free_generations( void )
    {
        delete[] next_generation;
        delete[] curr_generation;
        curr_generation = next_generation = best_chromosome = NULL;
    }
    //---------------------------------------------------------------------------
    void evolution::resize_population( const long &size )
    {
    #ifdef GA_DEBUG
        if(size <= 0 || size > max_population_size)
            throw exception(__FUNC__, "Wrong population size");
    #endif
        chromosome model(*best_chromosome);
        free_generations();
        population_size = size;
        alloc_generations(model);
    }
    //---------------------------------------------------------------------------
    void evolution::initialization( optimizible *optimizible_object )
    {
        epoch = 0;

        chromosome *next_current = next_generation;
        chromosome *curr_current = curr_generation;
        chromosome *const end    = curr_generation + (max_population_size - 1);

        while(curr_current <= end)
        {
            /// Гарнтированное совпадение параметров всех хромомсом
            *next_current = *curr_current = *best_chromosome;
            curr_current->random_fill();
            curr_current++;
            next_current++;
        }
        calculate_suitability(optimizible_object);
    }
    //---------------------------------------------------------------------------
    void evolution::next_epoch( optimizible *optimizible_object )
    {
        /// Следующее поколение \cppclassmember{evolution}{next\_epoch}
        long offspring_count  = 0;
        long mating_pair      = population_size;

        chromosome *current   = next_generation;
        chromosome *const end = next_generation + (max_population_size - 1);

        if(elitism)
        {
            /// При элитизме наилучшая хромосома гарантированно выживает
            *current++ = *best_chromosome;
            offspring_count++;
        }
        while(--mating_pair >= 0 && current <= end)
        {
            const chromosome *mather = &selection();
            if(random_real() < crossing_probability)
            {
                const chromosome *father = &selection();
                while(father == mather)
                    father = &selection();

                /// TODO: Вообще говоря кроссовер должен давать двух потомков
                /// Кроме того, в классике, количество особей в популяции должно
                /// изменяться случайным образом.

                if(random_bool())
                    current->crossing(*mather, *father);
                else
                    current->crossing(*father, *mather);
            }
            else
                *current = *mather;

            if(random_real() < mutation_probability)
                current->mutation(mutation_probability);

            offspring_count++;
            current++;
        }
        population_size = offspring_count;
        std::swap(curr_generation, next_generation);
        calculate_suitability(optimizible_object);

        epoch++;
    }
    //---------------------------------------------------------------------------
    void evolution::selection_technique( const selection_method &method )
    {
        switch(selection_tech = method)
        {
        case ga::roulette_selection:
            natural_selection   = &evolution::roulette_selection;
            natural_suitability = &evolution::linear_ranking_suitability;
            break;
        case ga::truncation_selection:
            natural_selection   = &evolution::truncation_selection;
            natural_suitability = &evolution::linear_ranking_suitability;
            break;
        case ga::tournament_selection:
            natural_selection   = &evolution::tournament_selection;
            natural_suitability = &evolution::proportionate_suitability;
            break;
        default:
            throw exception(__FUNC__, "Wrong selection method");
        }
    }
    //---------------------------------------------------------------------------
    void evolution::calculate_fitness( optimizible *optimizible_object )
    {
        chromosome *current   = curr_generation;
        chromosome *const end = curr_generation + (population_size - 1);

        current->fitness = optimizible_object->fitness_function(gene_chain.assign(current));
        fitness          = current->fitness;
        best_chromosome  = current;
        max_fitness      = fitness;
        min_fitness      = fitness;

        while(++current <= end)
        {
            current->fitness = optimizible_object->fitness_function(gene_chain.assign(current));
            fitness += current->fitness;
            if(current->fitness < min_fitness)
            {
                min_fitness = current->fitness;
                if(minimization)
                    best_chromosome = current;
            }
            if(current->fitness > max_fitness)
            {
                max_fitness = current->fitness;
                if(!minimization)
                    best_chromosome = current;
            }

        }
        fitness /= double(population_size);
    }
    //---------------------------------------------------------------------------
    void evolution::linear_ranking_suitability( void )
    {
        /// Линейная ранжировка: ранг - порядковый номер в отсортированном списке  \cppclassmember{evolution}{linear\_ranking\_suitability}

        int    rank            = 1;
        int    general_rank    = 0;
        int    non_sorted      = population_size;
        double current_fitness = minimization ? max_fitness : min_fitness;

        chromosome *current    = curr_generation;
        chromosome *const end  = curr_generation + (population_size - 1);

        while(non_sorted > 0)
        {
            double next_fitness = minimization ? min_fitness : max_fitness;
            current = curr_generation;
            while(current <= end)
            {
                if(current->fitness == current_fitness)
                {
                    general_rank += current->suitability = rank;
                    non_sorted--;
                }
                if(minimization)
                {
                    if(current->fitness < current_fitness
                    && current->fitness > next_fitness)
                        next_fitness = current->fitness;
                }
                else
                {
                    if(current->fitness > current_fitness
                    && current->fitness < next_fitness)
                        next_fitness = current->fitness;
                }
                current++;
            }
            current_fitness = next_fitness;
            rank++;
        }
        current = curr_generation;
        while(current <= end)
            (current++)->suitability /= double(general_rank);
    }
    //---------------------------------------------------------------------------
    void evolution::proportionate_suitability( void )
    {
        /// Пропорциональная ранжировка: ранг пропорционален целевой функции \cppclassmember{evolution}{proportionate\_suitability}
        chromosome *current   = curr_generation;
        chromosome *const end = curr_generation + (population_size - 1);
        while(current <= end)
        {
            if(minimization)
                 current->suitability = (max_fitness - current->fitness)
                                      / ((max_fitness - fitness) * population_size);
            else
                 current->suitability = (current->fitness - min_fitness)
                                      / ((fitness - min_fitness) * population_size);
            current++;
        }
    }
    //---------------------------------------------------------------------------
    const chromosome &evolution::tournament_selection( void )
    {
        /// Выбираем две хромосомы и смотрим какая лучше  \cppclassmember{evolution}{tournament\_selection}
        /// (Вид ранжировки безразличен)
        chromosome *lancelot = curr_generation + random_long(population_size);
        chromosome *ivengo   = curr_generation + random_long(population_size);
        while(lancelot == ivengo)
            ivengo = curr_generation + random_long(population_size);
        return (lancelot->suitability > ivengo->suitability) ? *lancelot : *ivengo;
    }
    //---------------------------------------------------------------------------
    const chromosome &evolution::roulette_selection( void )
    {
        /// Каждой хромосоме соответсвует сектор на рулетке, пропоциональный ее рангу \cppclassmember{evolution}{roulette\_selection}
        /// (работает с любой ранжировкой, однако с линейной лучше)
        double ball   = random_real();
        double sector = 0;

        chromosome *current   = curr_generation;
        chromosome *const end = curr_generation + (population_size - 1);
        while(current <= end)
        {
            sector += current->suitability;
            if(ball < sector)
                return *current;
            current++;
        }
        return *best_chromosome;
    }
    //---------------------------------------------------------------------------
    const chromosome &evolution::truncation_selection( void )
    {
        /// Выбираем одну хромосому из тех, чей ранг близок к наилучшей \cppclassmember{evolution}{truncation\_selection}
        /// (имеет смысл только с линейной ранжировкой)
        double truncation_threshold = 0.7;
        chromosome *selected;
        do
            selected = curr_generation + random_long(population_size);
        while(selected->suitability < truncation_threshold * best_chromosome->suitability);
        return *selected;
    }
    //---------------------------------------------------------------------------
    double evolution::optimization( optimizible *optimizible_object, long epoch_count, double error )
    {
        initialization(optimizible_object);
        double best_fitness  = best_chromosome->fitness;
        long   establishment = stability_epoch_count;
        while(epoch < epoch_count)
        {
            next_epoch(optimizible_object);
            if(fabs(best_fitness - best_chromosome->fitness) > error)
            {
                establishment = stability_epoch_count;
                best_fitness  = best_chromosome->fitness;
            }
            else if(--establishment == 0)
                break;
        }
        return best_chromosome->fitness;
    }
    //---------------------------------------------------------------------------
} // namespace ga

