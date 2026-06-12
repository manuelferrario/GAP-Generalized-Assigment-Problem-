# TP2 - Logística centralizada de primera milla (GAP)

Implementación de heurísticas, búsquedas locales y metaheurística para el Problema de Asignación Generalizada (GAP).

## Compilación

Desde la carpeta `src/`:

```bash
g++ -std=c++17 main.cpp gap.cpp ../implementacion/heuristicas.cpp ../implementacion/busqueda-local.cpp ../implementacion/metaheuristica.cpp -o gap_simulator
```

## Ejecución

```bash
./gap_simulator <input> <output> <modo> [parametro]
```

### Modos disponibles

| Modo | Descripción | Parámetro opcional |
|---|---|---|
| `1` | Heurística 1 (profit con segunda pasada) | — |
| `2` | Heurística 2 (greedy por prioridad, asistida por IA) | — |
| `1 1` | Heurística 1 + Búsqueda local 1 (relocate) | — |
| `2 2` | Heurística 2 + Búsqueda local 2 (swap) | — |
| `meta` | Metaheurística GRASP | tamaño RCL (default: 3) |

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

# Metaheurística con RCL de tamaño 5
./gap_simulator instances/gap/gap_a/a05100 output.txt meta 5
```

## Formato de entrada

Las instancias siguen el formato estándar del OR-Library:
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

Donde `m` = depósitos, `n` = vendedores. Los valores pueden ser enteros o decimales.

## Formato de salida

Una línea por depósito con los índices de los vendedores asignados:
```
0 3 7 12 ...
1 5 9 ...
...
```

## Uso de IA

Según las reglas del TP, se utilizó IA (Claude) para:
- Lectura de instancias y escritura de output (`gap.cpp`, `gap.h`)
- Heurística 2 (`heuristica_2` en `heuristicas.cpp`)
- Correcciones de compilación y debugging en búsquedas locales

La conversación completa está disponible en: https://claude.ai/code
