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
    //El while es para que explore todo lo que pueda hasta que se termine el tiempo del timeout.
    while (true) {
        auto ahora = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(ahora - inicio).count() >= timeout_seg)
            break;

        // Construccion aleatoria greedy (GRASP) 
        GAPSolution sol;
        sol.assignment.assign(vendedores, -1);
        vector<int> capacity_actual = inst.capacity;
        //Para todos los vendedores queremos ordenar los depositos de mejor a peor para cada uno.
        for (int v = 0; v < vendedores; v++) {
            vector<pair<double, int>> candidatos_factibles;
            //Cada depostito tiene una demanda y un costo para cada vendedor. Nos basamos en la relacion demanda / costo para determinar su 'valoracion'.
            for (int d = 0; d < depositos; d++) {
                if (inst.cost[d][v] == 0) continue;
                if (capacity_actual[d] >= inst.demand[d][v]) {
                    double profit = (double)inst.demand[d][v] / inst.cost[d][v];
                    candidatos_factibles.push_back({-profit, d});
                }
            }
            //si no entra en ningun deposito, dejamos sin asignar.
            if (candidatos_factibles.empty()) continue;
            //ordenamos de menor a mayor, pero como los guardamos con un '-' los de mejor relacion son los mas chicos. 
            sort(candidatos_factibles.begin(), candidatos_factibles.end());
            //Vemos entre cuantod depositos vamos a sortear el destino del vendedor.
            //Si hay menos depositos que puedan albergar la demanda del vendedor que el rcl elegido, se sortea entre todos los depositos.
            int limite_rcl = min((int)candidatos_factibles.size(), tamaño_rcl);
            uniform_int_distribution<> distribucion(0, limite_rcl - 1);
            int indice_elegido = distribucion(gen);
            // Sorteamos con una ditribucion uniforme y guardamos el valor de la posicion del deposito elegido.
            int deposito_final = candidatos_factibles[indice_elegido].second;
            // Asignamos el deposito para el vendedor. (el .second es porque es un par que tiene el profit y el deposito)
            sol.assignment[v]  = deposito_final;
            capacity_actual[deposito_final] -= inst.demand[deposito_final][v];
            // Actualizamos el deposito para futuras iteraciones.
        }

        // Vemos si podemos asignar alguno de los vendedores no asignados
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

        // Hacemos una busqueda local para ver si podemos encontrar una solucion vecina que mejore.
        // Aclaracion: poner el timeout de busqueda local en 10s, y no en 60s para no explotar tanto la solucion, y que el costo computacional no sea tan grande.
        // Calculamos el coto total y el residual de los depositos.
        sol.residual = computeResidual(inst, sol);
        sol.cost     = computeCost(inst, sol);
        sol = busqueda_local_2(inst, sol);

        // Si la solucion tiene menos costo total, la guardamos como la mejor.
        if (sol.cost < mejor_sol.cost)
            mejor_sol = sol;
    }
 // Devolvemos la mejor solucion que encontramos, hasta que termine el timeout.
    return mejor_sol;
}