// Implementamos los algoritmos de busqueda local.

#include "../src/gap.h"
#include "heuristicas.cpp"
using namespace std;

GAPSolution busqueda_local_1( const GAPInstance& inst, GAPSolution sol ){
    // Como nuestra heuristica numero 1 funciona mejor para casos grandes como Gap_e.
    // Queremos enfocarnos en mejorar con busqueda local esa solucion parcial encontrada en heu1.

    GAPSolution sol_1 = 
}

