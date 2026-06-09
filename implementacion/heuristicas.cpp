#include "../src/gap.h"
using namespace std;
#include <limits> 

// Implementación de algoritmos heuristicos.

// Costo: distancia del vendedor al deposito
 
// Idea de la primera heuristica: 
// Priorizar la distancia entre vendedor y deposito.
// Cuando a un deposito no le entra mas bloquearlo
// Si queda un vendedor colgado intentar buscar un deposito posible.

GAPSolution heuristica_1( const GAPInstance& inst , GAPSolution sol ) {
   
    int depositos = inst.m;
    int vendedores = inst.n;

    vector<vector<int>> cost = inst.cost;    
    vector<vector<int>> demand = inst.demand;  
    vector<int> capacity = inst.capacity;

    int v = 0;
    int d = 0;

    for(v; v < vendedores; v++) {
        for(d; d < depositos; d++) {
            if (cost[d][v] == 0) continue;

            double profit = double(demand[d][v]) / double(cost[d][v]);
 
            if ( profit >= 0.05 && capacity[d] >= demand[d][v]) { // Beneficio minimo del 25%. 
 
                sol.assignment[v] = d;
                capacity[d] = capacity[d] - demand[d][v];
                
                break;
                
            }
        }
        
    }

    return sol;
}
