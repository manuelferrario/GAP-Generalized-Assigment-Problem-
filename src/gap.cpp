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

    double tmp;
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
