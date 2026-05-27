# Gestor-de-procesos-ED

# Simulador de Planificación de Procesos (SO)

Este proyecto es una simulación en C++ de la gestión de procesos y asignación de memoria de un Sistema Operativo. Implementa desde cero estructuras de datos dinámicas y algoritmos clásicos sin depender de la Standard Template Library (STL), gestionando la memoria manualmente mediante punteros.

## Características Principales

El simulador permite recrear el ciclo de vida de un proceso (Nuevo, Listo, Ejecutando), gestionar su prioridad de atención y administrar los bloques de memoria que utiliza. Todo esto se controla a través de una interfaz de consola interactiva.

## Arquitectura y Estructuras de Datos

El sistema se basa en tres estructuras de datos fundamentales implementadas mediante nodos (`struct`):

1. **Lista Enlazada Simple (Registro Global):** Almacena el Bloque de Control de Proceso (PCB) de todos los procesos ingresados al sistema. Permite inserción, eliminación y recorrido dinámico.
2. **Cola de Prioridad (Planificador de CPU):** Organiza los procesos en estado "LISTO". Los procesos se encolan respetando estrictamente su prioridad (donde un valor menor indica mayor urgencia).
3. **Pila / Stack (Gestor de Memoria):** Administra la asignación de memoria (LIFO). Cuando un proceso pasa a "EJECUTANDO", su bloque de memoria se apila (`push`). Cuando termina, se libera (`pop`).

## Algoritmos Implementados

Para la manipulación de datos se han desarrollado los siguientes algoritmos:
* **Búsqueda Lineal:** Rastreo $O(n)$ directo sobre la Lista Enlazada para ubicar procesos por ID.
* **Ordenamiento por Burbuja (Bubble Sort):** Algoritmo de intercambio manual que reorganiza el registro global de procesos de forma ascendente según su ID.
* **Búsquedas Destructivas con Estructuras Auxiliares:** Algoritmos especiales para buscar elementos en la Pila y en la Cola de Prioridad sin romper la regla de acceso (LIFO/FIFO). Se utilizan pilas/colas temporales para retener los datos durante la búsqueda y luego restaurarlos a su estado original.

## Compilación y Ejecución

El código está contenido en un único archivo (`main.cpp`) escrito en C++ estándar, lo que garantiza su portabilidad.

**Usando g++ en terminal:**
```bash
g++ main.cpp -o simulador
./simulador
