// Implementamos los algoritmos de busqueda local.

#include "../src/gap.h"
#include <chrono>
#include <algorithm>
using namespace std;

GAPSolution busqueda_local_1( const GAPInstance& inst, GAPSolution sol ){
    // Como nuestra heuristica numero 1 funciona mejor para casos grandes como Gap_e.
    // Queremos enfocarnos en mejorar con busqueda local esa solucion parcial encontrada en heu1.
    // Implementación sin ayuda de la IA.

    GAPSolution sol_1 = heuristica_1(inst, sol);
    sol_1.cost = computeCost(inst, sol_1);
    sol_1.residual = computeResidual(inst, sol_1);

    int v1 = 0; // iteradores de vendedores.
    int v2;
    
    int depositos = inst.m;
    int v_total = inst.n;

    int cost_inicial = sol_1.cost;

    for(v1; v1 < v_total; v1++) {
        
        for(v2 = v1 + 1; v2 < v_total; v2++) {    // El vecindario de mi busqueda es swappear los dos vecinos que son proximos. 

            if( sol_1.assignment[v1] == -1 && sol_1.assignment[v2] == -1 ) {
                break; // Si ninguno esta asignado no hay nada que cambiar.
            } 
            
            if( sol_1.assignment[v1] == -1 && sol_1.assignment[v2] != -1 ) { // caso v1 sin asignar y v2 asignado.
                int depo2 = sol_1.assignment[v2];
                int residuo2 = sol_1.residual[depo2];
                
                if( residuo2 - inst.demand[depo2][v1] >= 0 ) { // Si entra en el depo de v2 lo incluyo.
                    sol_1.assignment[v1] = depo2;
                    sol_1.residual[depo2] -= inst.demand[depo2][v1];
                    break;
                }
    
            }

            if( sol_1.assignment[v2] == -1 && sol_1.assignment[v1] != -1 ) { // Caso v2 sin asignar y v1 asignado.
                int depo1 = sol_1.assignment[v1];
                int residuo1 = sol_1.residual[depo1];
                
                if( residuo1 - inst.demand[depo1][v2] >= 0 ) { // Si v2 entra en el depo de v1 lo incluyo.
                    sol_1.assignment[v2] = depo1;
                    sol_1.residual[depo1] -= inst.demand[depo1][v2];
                    break;
                }
            } 

            if( sol_1.assignment[v1] != -1 && sol_1.assignment[v2] != -1 ) {
                int depo1 = sol_1.assignment[v1];
                int depo2 = sol_1.assignment[v2];

                int residuo1 = sol_1.residual[depo1];
                int residuo2 = sol_1.residual[depo2];

                // Primer caso mover v1 a depo2:
                if( (residuo2 -  inst.demand[depo2][v1] >= 0) && (cost_inicial > cost_inicial - inst.cost[depo1][v1] + inst.cost[depo2][v1]) ) { // Buscamos minimizar costo. Calculamos si conviene mover a v1 o v2.
                    sol_1.assignment[v1] = depo2;
                    sol_1.residual[depo2] -= inst.demand[depo2][v1];
                    sol_1.residual[depo1] += inst.demand[depo1][v1];
                    continue; // Veo si v2 se puede reubicar tambien

                }

                // Segundo caso mover v2 a depo1:
                if( (residuo1 -  inst.demand[depo1][v2] >= 0) && (cost_inicial > cost_inicial - inst.cost[depo2][v2] + inst.cost[depo1][v2]) ) {
                    sol_1.assignment[v2] = depo1;
                    sol_1.residual[depo1] -= inst.demand[depo1][v2];
                    sol_1.residual[depo2] += inst.demand[depo2][v2];
                    continue;

                }
            
            }


        }
    }
    return sol_1;
}

GAPSolution busqueda_local_2(const GAPInstance& inst, GAPSolution sol) {
    // Busqueda local: Relocate
    // PARTE 1: Mejora vendedores asignados (first improvement, orden por costo descendente)
    //          Repite hasta no encontrar mejoras o hasta timeout.
    // PARTE 2: Intenta colocar vendedores no asignados en el primer deposito que entre.

    // (Asistencia de la IA para escribir este codigo, se le dio la idea general para que la implemente)

    int depositos  = inst.m;
    int vendedores = inst.n;

    int timeout_seg = 60; // Tiempo maximo para la busqueda local (arbitrario)

    // ── PARTE 1: mejorar asignados ────────────────────────────────────────
    auto inicio = chrono::steady_clock::now();
    bool mejoro = true;

    while (mejoro) {

        auto ahora = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(ahora - inicio).count() >= timeout_seg)
            break;

        mejoro = false;

        // Armar y ordenar asignados por costo actual descendente
        vector<int> asignados;
        for (int v = 0; v < vendedores; v++)
            if (sol.assignment[v] != -1)
                asignados.push_back(v);

        sort(asignados.begin(), asignados.end(), [&](int v1, int v2) {
            return inst.cost[sol.assignment[v1]][v1] > inst.cost[sol.assignment[v2]][v2];
        });

        for (int v : asignados) {
            int d_actual     = sol.assignment[v];
            int costo_actual = inst.cost[d_actual][v];

            for (int d = 0; d < depositos; d++) {
                if (d == d_actual) continue;

                if (sol.residual[d] >= inst.demand[d][v] && inst.cost[d][v] < costo_actual) {
                    sol.residual[d_actual] += inst.demand[d_actual][v];  // libero d_actual
                    sol.residual[d]        -= inst.demand[d][v];         // ocupo d
                    sol.assignment[v]       = d;
                    mejoro = true;
                    break;  // first improvement
                }
            }
        }
    }

    // ── PARTE 2: meter no asignados en el primer deposito que entre ───────
    for (int v = 0; v < vendedores; v++) {
        if (sol.assignment[v] != -1) continue;

        for (int d = 0; d < depositos; d++) {
            if (sol.residual[d] >= inst.demand[d][v]) {
                sol.assignment[v]  = d;
                sol.residual[d]   -= inst.demand[d][v];
                break;
            }
        }
    }

    sol.cost = computeCost(inst, sol);
    return sol;
}