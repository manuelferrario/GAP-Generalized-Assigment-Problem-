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
    // Busqueda local 2: Swap
    // Intercambia dos vendedores entre depositos si mejora el costo total.
    // First improvement: aplica el primer intercambio que mejore y reinicia.

    int depositos  = inst.m;
    int vendedores = inst.n;
    int timeout_seg = 60;

    auto inicio = chrono::steady_clock::now();
    bool mejoro = true;

    while (mejoro) {
        auto ahora = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(ahora - inicio).count() >= timeout_seg)
            break;

        mejoro = false;

        for (int v1 = 0; v1 < vendedores && !mejoro; v1++) {
            if (sol.assignment[v1] == -1) continue;

            for (int v2 = v1 + 1; v2 < vendedores && !mejoro; v2++) {
                if (sol.assignment[v2] == -1) continue;

                int d1 = sol.assignment[v1];
                int d2 = sol.assignment[v2];

                if (d1 == d2) continue;

                // Factibilidad: residuales tras el intercambio
                int residual_d1 = sol.residual[d1] + inst.demand[d1][v1] - inst.demand[d1][v2];
                int residual_d2 = sol.residual[d2] + inst.demand[d2][v2] - inst.demand[d2][v1];
                //No es factible, porque en alguno de los depositos no da abasto a la demanda.
                if (residual_d1 < 0 || residual_d2 < 0) continue;

                // Si la solucion nueva es factible veo como quedan los costos de las dos soluciones
                int costo_actual = inst.cost[d1][v1] + inst.cost[d2][v2];
                int costo_swap   = inst.cost[d1][v2] + inst.cost[d2][v1];
                // Si es mejor intercambio. Actualizo las variables correspondientes.
                if (costo_swap < costo_actual) {
                    sol.assignment[v1] = d2;
                    sol.assignment[v2] = d1;
                    sol.residual[d1]   = residual_d1;
                    sol.residual[d2]   = residual_d2;
                    mejoro = true; // Si hubo una mejora con algun swap entre los vendedores sigo. Si en ninuga instancia mejoro, no hay swap que mejore. Corto
                }
            }
        }
    }

    sol.cost = computeCost(inst, sol);
    return sol;
}