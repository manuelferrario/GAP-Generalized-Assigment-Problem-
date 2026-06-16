#include "../src/gap.h"
#include <algorithm>
#include <random>
#include <climits>
#include <chrono>
using namespace std;

GAPSolution metaheuristica(const GAPInstance& inst, int tamaño_rcl) {
    int depositos  = inst.m;
    int vendedores = inst.n;

    int timeout_seg = 60;
    GAPSolution mejor_sol;
    mejor_sol.cost = INT_MAX;

    random_device rd;
    mt19937 gen(rd());

    auto inicio = chrono::steady_clock::now();

    while (true) {
        auto ahora = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(ahora - inicio).count() >= timeout_seg)
            break;

        //  FASE 1: Construccion aleatoria greedy (GRASP) 
        GAPSolution sol;
        sol.assignment.assign(vendedores, -1);
        vector<int> capacity_actual = inst.capacity;

        for (int v = 0; v < vendedores; v++) {    // Recorre todos los vendedores para usar RCL.
            vector<pair<double, int>> candidatos_factibles;

            for (int d = 0; d < depositos; d++) {
                if (inst.cost[d][v] == 0) continue;
                if (capacity_actual[d] >= inst.demand[d][v]) {
                    double profit = (double)inst.demand[d][v] / inst.cost[d][v];
                    candidatos_factibles.push_back({-profit, d});
                }
            }

            if (candidatos_factibles.empty()) continue;

            sort(candidatos_factibles.begin(), candidatos_factibles.end());

            int limite_rcl = min((int)candidatos_factibles.size(), tamaño_rcl);
            uniform_int_distribution<> distribucion(0, limite_rcl - 1);
            int indice_elegido = distribucion(gen);

            int deposito_final = candidatos_factibles[indice_elegido].second;
            sol.assignment[v]  = deposito_final;
            capacity_actual[deposito_final] -= inst.demand[deposito_final][v];
        }

        // Reparacion: vendedores no asignados 
        for (int v = 0; v < vendedores; v++) {
            if (sol.assignment[v] != -1) continue;
            for (int d = 0; d < depositos; d++) {
                if (capacity_actual[d] >= inst.demand[d][v]) {
                    sol.assignment[v] = d;
                    capacity_actual[d] -= inst.demand[d][v];
                    break;
                }
            }
        }

        //  FASE 2: Busqueda local 
        sol.residual = computeResidual(inst, sol);
        sol.cost     = computeCost(inst, sol);
        sol = busqueda_local_2(inst, sol);

        //  FASE 3: Actualizar mejor solucion 
        if (sol.cost < mejor_sol.cost)
            mejor_sol = sol;
    }

    return mejor_sol;
}