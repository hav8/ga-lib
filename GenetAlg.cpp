//---------------------------------------------------------------------------
#include "GALib.h"
#include <conio.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma argsused

int func_count = 0;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
double fitness_function(const ga::genom &chain)
{
    func_count++;
    double sum = -100;
    double j = 0;
    for( ; j < 16; j++)
    {
        double x = chain[j].real;
        sum += (x - j) * (x - j);
    }
    return sum;
}


//---------------------------------------------------------------------------
int main()
{
    randomize();
    try
    {
        ga::evolution ga(ga::truncation_selection, ga::two_point_crossing, ga::single_point_mutation);
        ga.add_integer();

        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);

        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);
        ga.add_real(-100, 100);

        std::cout << "start" << std::endl;

        std::cout << "result " << ga.minimize(fitness_function, 2000, 1e-15) << std::endl;
        int j = 0;
        for( ; j < 16; j++)
            std::cout << "x" << j << " = " << ga.best_genom().get_gene(j).to_string()
                      << " " << ga.best_genom().get_gene(j) << std::endl;
        std::cout << "func_count = " << func_count << std::endl;


    }
    catch(ga::exception &e)
    {
        std::cerr << e << std::endl;
    }


    getch();
}

