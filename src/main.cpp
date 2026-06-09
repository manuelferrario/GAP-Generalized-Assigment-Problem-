#include "gap.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <input> <output>" << std::endl;
        return 1;
    }

    GAPInstance inst = readInstance(argv[1]);
    std::cout << "Instancia leida: " << inst.m << " depositos, " << inst.n << " vendedores" << std::endl;

    // Inicializar solución vacía (-1 = no asignado)
    GAPSolution sol;
    sol.assignment.assign(inst.n, -1);

    sol = heuristica_2(inst, sol);
    sol.cost = computeCost(inst, sol);

    writeOutput(argv[2], inst, sol);
    std::cout << "Solucion escrita en " << argv[2] << std::endl;
    std::cout << "Costo total: " << sol.cost << std::endl;

    return 0;
}
