#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

struct GAPInstance {    // Estructura de datos que representa la instancia de un problema GAP
    int m;  // depósitos
    int n;  // vendedores
    std::vector<std::vector<int>> cost;    // cost[D][V]: costo de asignar vendedor V al depósito D (distancia del vendedor al deposito)
    std::vector<std::vector<int>> demand;  // demand[D][V]: demanda de vendedor V en depósito D (lo que puede llevar el vendedor a ese deposito)
    std::vector<int> capacity;             // capacity[D]: capacidad máxima del depósito D
};

// Una solución es, para cada vendedor j, a qué depósito está asignado (-1 = no asignado)
struct GAPSolution {    // Estructura de datos que representa una solucion a GAP.
    std::vector<int> assignment;  // assignment[j] = i: vendedor j asignado al depósito i
    int cost = 0;
    std::vector<int> residual;    // residual[i]: capacidad restante del depósito i
};

GAPInstance readInstance(const std::string& filename);
int computeCost(const GAPInstance& inst, const GAPSolution& sol);
std::vector<int> computeResidual(const GAPInstance& inst, const GAPSolution& sol);
void writeOutput(const std::string& filename, const GAPInstance& inst, const GAPSolution& sol);

GAPSolution heuristica_1(const GAPInstance& inst, GAPSolution sol);
GAPSolution heuristica_2(const GAPInstance& inst, GAPSolution sol);

GAPSolution busqueda_local_1(const GAPInstance& inst, GAPSolution sol);
GAPSolution busqueda_local_2(const GAPInstance& inst, GAPSolution sol);