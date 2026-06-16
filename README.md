# TP2 - Logística centralizada de primera milla (GAP)

Implementación de heurísticas, búsquedas locales y metaheurística para el **Problema de Asignación Generalizada (GAP)**. El objetivo es asignar `n` vendedores a `m` depósitos minimizando el costo total, respetando las capacidades de cada depósito. Los vendedores no asignados reciben una penalización de `3 × cmax`.

## Estructura del proyecto

```
tp2/
├── src/
│   ├── gap.h               # Estructuras y declaraciones
│   ├── gap.cpp             # Lectura de instancias, escritura de output, funciones auxiliares
│   └── main.cpp            # Punto de entrada y CLI
├── implementacion/
│   ├── heuristicas.cpp     # Heurística 1 y Heurística 2
│   ├── busqueda-local.cpp  # Búsqueda local 1 (relocate) y Búsqueda local 2 (swap)
│   └── metaheuristica.cpp  # GRASP
└── instances/
    ├── gap/
    │   ├── gap_a/          # Instancias grupo A (capacidades holgadas)
    │   ├── gap_b/          # Instancias grupo B (capacidades ajustadas)
    │   └── gap_e/          # Instancias grupo E (costos heterogéneos)
    └── real/               # Instancia real (310 depósitos, 1100 vendedores)
```

## Compilación

Desde la carpeta `src/`:

```bash
g++ -std=c++17 main.cpp gap.cpp ../implementacion/heuristicas.cpp ../implementacion/busqueda-local.cpp ../implementacion/metaheuristica.cpp -o gap_simulator
```

Requiere C++17 o superior.

## Ejecución

```bash
./gap_simulator <input> <output> <modo> [parametros]
```

| Modo | Descripción | Parámetro extra |
|------|-------------|-----------------|
| `1` | Heurística 1 — threshold de profit con segunda pasada | — |
| `2` | Heurística 2 — greedy por prioridad demanda/costo | — |
| `1 1` | Heurística 1 + Búsqueda local 1 (relocate, first improvement) | — |
| `2 2` | Heurística 2 + Búsqueda local 2 (swap, first improvement) | — |
| `1 2` | Heurística 1 + Búsqueda local 2 (swap, first improvement) | — |
| `meta` | Metaheurística GRASP (construcción aleatoria + BL2, timeout 60s) | tamaño RCL (default: 3) |

### Ejemplos

```bash
# Heurística 1
./gap_simulator instances/gap/gap_a/a05100 output.txt 1

# Heurística 2
./gap_simulator instances/gap/gap_a/a05100 output.txt 2

# Heurística 1 + Búsqueda local 1
./gap_simulator instances/gap/gap_a/a05100 output.txt 1 1

# Heurística 2 + Búsqueda local 2
./gap_simulator instances/gap/gap_a/a05100 output.txt 2 2

# Metaheurística GRASP con RCL de tamaño 5
./gap_simulator instances/gap/gap_a/a05100 output.txt meta 5

# Instancia real
./gap_simulator instances/real/real_instance output.txt meta 5
```

## Algoritmos

### Heurística 1 (diseño propio)
Recorre los vendedores y los asigna al depósito que maximiza el ratio demanda/costo, siempre que supere un umbral mínimo de profit. Realiza una segunda pasada para intentar asignar los vendedores que quedaron sin depósito.

### Heurística 2 (asistida por IA)
Ordena los vendedores por prioridad (demanda promedio / costo mínimo) y los asigna al depósito factible de menor costo. Incluye segunda pasada para no asignados.

### Búsqueda local 1 — Relocate (diseño propio)
Operador de reubicación con estrategia *first improvement*. Para cada vendedor asignado, intenta moverlo a otro depósito con menor costo, siempre que haya capacidad disponible.

### Búsqueda local 2 — Swap (diseño propio)
Operador de intercambio con estrategia *first improvement* y timeout de 60 segundos. Para cada par de vendedores asignados a depósitos distintos, evalúa si intercambiarlos reduce el costo total manteniendo factibilidad.

### Metaheurística — GRASP (diseño de compañero de equipo)
Loop de 60 segundos con construcción greedy aleatoria usando RCL (Restricted Candidate List) + Búsqueda local 2. En cada iteración construye una nueva solución eligiendo aleatoriamente entre los `k` mejores candidatos, aplica una fase de reparación para vendedores no asignados, y guarda la mejor solución encontrada.

> **Nota:** el timeout está definido en `implementacion/metaheuristica.cpp` en la variable `timeout_seg`. Si se quiere reducir el tiempo de ejecución para hacer benchmarks rápidos, se puede bajar ese valor (por ejemplo a `10` o `15` segundos) antes de recompilar.

## Formato de entrada

Formato estándar OR-Library:
```
m n
cost[0][0] cost[0][1] ... cost[0][n-1]
...
cost[m-1][0] ... cost[m-1][n-1]
demand[0][0] ... demand[0][n-1]
...
demand[m-1][0] ... demand[m-1][n-1]
capacity[0] capacity[1] ... capacity[m-1]
```

`m` = depósitos, `n` = vendedores. Los valores pueden ser enteros o decimales.

## Formato de salida

Una línea por depósito con los índices (base 0) de los vendedores asignados:
```
0 3 7 12
1 5 9
2 4 8
...
```

## Resultados por grupo de instancias

| Grupo | H1 asig% | H1 costo | H2 asig% | H2 costo | H1+BL1 asig% | H1+BL1 costo | H2+BL2 asig% | H2+BL2 costo |
|-------|----------|----------|----------|----------|--------------|--------------|--------------|--------------|
| GAP_a | 100% | 26.843 | 100% | 12.653 | 100% | 14.112 | 100% | 12.485 |
| GAP_b | 87.4% | 40.349 | 89.8% | 26.546 | 87.4% | 39.321 | 89.8% | 25.642 |
| GAP_e | 59.1% | 12.573.717 | 39.0% | 17.993.779 | 64.3% | 11.433.783 | 39.0% | 17.815.738 |

GRASP en GAP_e (rcl=3): 58.8% asignados promedio, costo promedio 1.131.713.

## Uso de IA

Según las reglas del TP, se utilizó IA (Claude) para:
- Infraestructura de lectura/escritura de instancias (`gap.cpp`, `gap.h`)
- Heurística 2 (`heuristica_2` en `heuristicas.cpp`) y BL2 (`busqueda_local_2` en `busqueda-local.cpp`)
- Correcciones de compilación y debugging
- Benchmarks de tests con GAP`s pasados por catedra.
- Generación del archivo README.md

