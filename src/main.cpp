#include "gap.h"

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <input> <output> <heuristica 1|2> [busqueda_local 1|2]" << std::endl;
        return 1;
    }

    GAPInstance inst = readInstance(argv[1]);
    std::cout << "Instancia leida: " << inst.m << " depositos, " << inst.n << " vendedores" << std::endl;

    GAPSolution sol;
    sol.assignment.assign(inst.n, -1);

    int heuristica = std::stoi(argv[3]);
    if (heuristica == 1)
        sol = heuristica_1(inst, sol);
    else
        sol = heuristica_2(inst, sol);

    sol.cost = computeCost(inst, sol);
    sol.residual = computeResidual(inst, sol);
    std::cout << "Heuristica " << heuristica << " - Costo: " << sol.cost << std::endl;

    if (argc >= 5) {
        int busqueda = std::stoi(argv[4]);
        if (busqueda == 1)
            sol = busqueda_local_1(inst, sol);
        else
            sol = busqueda_local_2(inst, sol);

        sol.cost = computeCost(inst, sol);
        sol.residual = computeResidual(inst, sol);
        std::cout << "Busqueda local " << busqueda << " - Costo: " << sol.cost << std::endl;
    }

    writeOutput(argv[2], inst, sol);
    std::cout << "Solucion escrita en " << argv[2] << std::endl;

    return 0;
}
