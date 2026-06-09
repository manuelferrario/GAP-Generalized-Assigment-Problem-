#include "../src/gap.h"
#include <algorithm>
#include <climits>
using namespace std;
#include <limits> 

// Implementación de algoritmos heuristicos.

// Costo: distancia del vendedor al deposito
 
// Idea de la primera heuristica: 
// Priorizar la distancia entre vendedor y deposito.
// Cuando a un deposito no le entra mas bloquearlo
// Si queda un vendedor colgado intentar buscar un deposito posible.

GAPSolution heuristica_1( const GAPInstance& inst , GAPSolution sol ) {
    // Entrada: Instancia del problema de GAP y la solucion para sobreescribir.
    // Salida: La solucion al GAP.

    int depositos = inst.m;
    int vendedores = inst.n;

    vector<vector<int>> cost = inst.cost;    
    vector<vector<int>> demand = inst.demand;  
    vector<int> capacity = inst.capacity;

    int v = 0;
    int d = 0;

    for(v; v < vendedores; v++) {
        for(d; d < depositos; d++) {
            if ( cost[d][v] == 0 ) continue;

            double profit = double(demand[d][v]) / double(cost[d][v]);
 
            if ( profit >= 0.05 && capacity[d] >= demand[d][v] ) { // Beneficio minimo del 5%. 
 
                sol.assignment[v] = d;
                capacity[d] = capacity[d] - demand[d][v];
                
                break; // Si le asignamos un deposito al vendedor no buscamos mas depositos.
                
            }

        }

        d = 0; // Reinicio los depositos para analizar todos los casos.        
    
    }

    v = 0; // Reinicio los vendedores para analizar los casos de los vendedores colgados. (Vendedores en -1)

    for(v; v < vendedores; v++) {
        for(d; d < depositos; d++) {
            if ( cost[d][v] == 0 ) continue;
 
            if ( sol.assignment[v] == -1 && capacity[d] >= demand[d][v] ) { 
 
                sol.assignment[v] = d;
                capacity[d] = capacity[d] - demand[d][v];
                
                break; // Si le asignamos un deposito al vendedor no buscamos mas depositos.
                
            }

        }

        d = 0; // Reinicio los depositos para analizar todos los casos.        
    
    }

    return sol;

}

GAPSolution heuristica_2( const GAPInstance& inst, GAPSolution sol ) {
    // Heuristica 2 (asistida por IA):
    // Ordena vendedores por prioridad = demanda_promedio / costo_minimo (menor primero).
    // Asigna cada vendedor al deposito mas barato con capacidad disponible.

    int depositos = inst.m;
    int vendedores = inst.n;

    vector<int> capacity = inst.capacity;

    // Calcular prioridad de cada vendedor
    vector<pair<double, int>> prioridad(vendedores); // (prioridad, indice vendedor)
    for (int v = 0; v < vendedores; v++) {
        double demanda_prom = 0.0;
        int costo_min = INT_MAX;
        for (int d = 0; d < depositos; d++) {
            demanda_prom += inst.demand[d][v];
            costo_min = min(costo_min, inst.cost[d][v]);
        }
        demanda_prom /= depositos;
        double p = (costo_min > 0) ? demanda_prom / costo_min : demanda_prom;
        prioridad[v] = {p, v};
    }

    // Ordenar de menor a mayor prioridad (los faciles primero)
    sort(prioridad.begin(), prioridad.end());

    // Asignar cada vendedor al deposito mas barato con capacidad disponible
    for (auto& [p, v] : prioridad) {
        // Ordenar depositos por costo para este vendedor
        vector<pair<int, int>> depositos_ordenados(depositos); // (costo, indice deposito)
        for (int d = 0; d < depositos; d++)
            depositos_ordenados[d] = {inst.cost[d][v], d};
        sort(depositos_ordenados.begin(), depositos_ordenados.end());

        for (auto& [costo, d] : depositos_ordenados) {
            if (capacity[d] >= inst.demand[d][v]) {
                sol.assignment[v] = d;
                capacity[d] -= inst.demand[d][v];
                break;
            }
        }
    }

    // Se nos ocurrio que tal vez quedan vendedores colgados entonces usamos el mismo for que en la heu_1 para asignarlos.
    int v = 0; // Reinicio los vendedores para analizar los casos de los vendedores colgados. (Vendedores en -1)
    int d = 0; // Reinicio los depositos para analizar todos los casos.
    
    for(v; v < vendedores; v++) {
        for(d; d < depositos; d++) {
            if ( inst.cost[d][v] == 0 ) continue;
 
            if ( sol.assignment[v] == -1 && capacity[d] >= inst.demand[d][v] ) { 
 
                sol.assignment[v] = d;
                capacity[d] = capacity[d] - inst.demand[d][v];
                
                break; // Si le asignamos un deposito al vendedor no buscamos mas depositos.
                
            }

        }

        d = 0; // Reinicio los depositos para analizar todos los casos.        
    
    }

    return sol;
}