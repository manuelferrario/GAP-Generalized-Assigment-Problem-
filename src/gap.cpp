#include "gap.h"

// .cpp con las funciones para leer la instancia y escribir la resolucion

GAPInstance readInstance(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("No se pudo abrir el archivo: " + filename);

    GAPInstance inst;
    file >> inst.m >> inst.n;

    inst.cost.assign(inst.m, std::vector<int>(inst.n));
    inst.demand.assign(inst.m, std::vector<int>(inst.n));
    inst.capacity.resize(inst.m);

    double tmp; // Para leer los valores como double y luego convertirlos a int, evitando problemas de formato. (Heuristica 1)

    for (int i = 0; i < inst.m; i++)
        for (int j = 0; j < inst.n; j++) {
            file >> tmp;
            inst.cost[i][j] = (int)tmp;
        }

    for (int i = 0; i < inst.m; i++)
        for (int j = 0; j < inst.n; j++) {
            file >> tmp;
            inst.demand[i][j] = (int)tmp;
        }

    for (int i = 0; i < inst.m; i++) {
        file >> tmp;
        inst.capacity[i] = (int)tmp;
    }

    if (file.fail())
        throw std::runtime_error("Error al leer el archivo: " + filename);

    return inst;
}

void writeOutput(const std::string& filename, const GAPInstance& inst, const GAPSolution& sol) {
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("No se pudo escribir el archivo: " + filename);

    for (int i = 0; i < inst.m; i++) {
        bool first = true;
        for (int j = 0; j < inst.n; j++) {
            if (sol.assignment[j] == i) {
                if (!first) file << " ";
                file << j;
                first = false;
            }
        }
        file << "\n";
    }
}

int computeCost(const GAPInstance& inst, const GAPSolution& sol) {
    // Calcula el costo total de la solucion:
    // suma de cost[d][v] para cada vendedor asignado
    // + penalizacion de 3 * cmax por cada vendedor no asignado (-1)

    // Calcular cmax
    int cmax = 0;
    for (int i = 0; i < inst.m; i++)
        for (int j = 0; j < inst.n; j++)
            if (inst.cost[i][j] > cmax) cmax = inst.cost[i][j];

    int total = 0;
    for (int v = 0; v < inst.n; v++) {
        if (sol.assignment[v] == -1)
            total += 3 * cmax;
        else
            total += inst.cost[sol.assignment[v]][v];
    }
    return total;
}
