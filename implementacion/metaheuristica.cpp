#include "../src/gap.h"
#include <algorithm>
#include <random>
#include <climits>
using namespace std;
#include <vector>

GAPSolution metaheuristica(const GAPInstance& inst, int tamaño_rcl) {
    GAPSolution sol;
    int depositos = inst.m;
    int vendedores = inst.n;

    sol.assignment.assign(vendedores, -1);

    //Copia local de las capacidades. Aca vamos restando el espacio.
    vector<int> capacity_actual = inst.capacity;

    //Motor de generacion de numeros aleatorios (AYUDA DE IA)
    random_device rd;
    mt19937 gen(rd());

    //Iteramos sobre cada vendedor
    for (int v = 0; v < vendedores; v++) {
        
        //Vector de pares: almacena {beneficio_calculado, id_del_deposito}
        //Se reinicia vacio en cada nuevo vendedor.
        vector<pair<double, int>> candidatos_factibles;
    

        for (int d = 0; d < depositos; d++) {
                //Evitamos divisiones por cero o rutas invalidas
                if (inst.cost[d][v] == 0) continue;

                //Verificamos si el deposito 'd' tiene espacio actual suficiente
                if (capacity_actual[d] >= inst.demand[d][v]) {
                
                    //Calculamos el beneficio. Pasamos a double para no perder decimales.
                    //Mayor beneficio = Mejor opcion.
                    double profit = (double)inst.demand[d][v] / inst.cost[d][v];
                
                    //Guardamos esta opcion viable en nuestro vector temporal
                    // -profit devido a que cuando hagamos el sort, queden los numeros mas altos al principio ya que son negativos
                    candidatos_factibles.push_back({-profit, d});
                }
        }
    
        //Si no hay espacio en toda la red para este vendedor, lo ignoramos (queda colgado)
        if (candidatos_factibles.empty()) {
            continue; 
        }

        // Ejecutas el sort que ordena de forma ascendente
        sort(candidatos_factibles.begin(), candidatos_factibles.end());
        
        /*Definimos el limite de nuestra ruleta para asi no tener una falla de asignación. 
        Usamos min() por si hay menos opciones viables que nuestro tamaño_rcl deseado, 
        ya que cada vendedor va a tener distintos depositos disponibles */
        int limite_rcl = min(static_cast<int>(candidatos_factibles.size()), tamaño_rcl);

        //Generamos un indice aleatorio con mismas probabilidades entre 0 y limite_rcl - 1 
        uniform_int_distribution<> distribucion(0, limite_rcl - 1);
        int indice_elegido = distribucion(gen); 
        
        //Extraemos el ID del deposito ganador
        int deposito_final = candidatos_factibles[indice_elegido].second;

        //Consolidamos la asignacion en nuestra solucion final
        sol.assignment[v] = deposito_final;
        
        //Restamos el espacio ocupado para las siguientes iteraciones
        capacity_actual[deposito_final] -= inst.demand[deposito_final][v];
    }

    // Retornamos la solucion construida
    return sol;
}