#include <iostream> // Incluye la biblioteca estándar de entrada/salida de C++
using namespace std; // Permite usar elementos del espacio de nombres estándar sin prefijo "std::"

// 1. Búsqueda Lineal Clásica (Listas)
// Función que busca un proceso (PCB) por su ID dentro de una lista enlazada global
PCB* busquedaLineal(ListaGlobal* lst, int id_buscado) { // Recibe un puntero a la lista y el ID a buscar, retorna un puntero a PCB
    PCB* actual = lst->cabeza; // Inicializa el puntero 'actual' con el primer nodo (cabeza) de la lista
    while (actual != NULL) { // Recorre la lista mientras no se llegue al final (NULL)
        if (actual->id == id_buscado) return actual; // Si el ID del nodo actual coincide con el buscado, retorna ese nodo
        actual = actual->siguiente; // Avanza al siguiente nodo de la lista
    } 
    return NULL; // Si no se encontró el ID en toda la lista, retorna NULL (no encontrado)
} 

// 2. Algoritmo de Burbuja (Ordenamiento de Lista Enlazada con Sintaxis Simple)
// Función auxiliar que intercambia los datos internos de dos nodos PCB (sin mover los punteros de los nodos)
void intercambiar(PCB* a, PCB* b) { // Recibe punteros a los dos nodos cuyos datos se van a intercambiar
    // Variables temporales simples para cada dato interno
    int t_id = a->id; // Guarda temporalmente el ID del nodo 'a'
    a->id = b->id; // Asigna el ID de 'b' al nodo 'a'
    b->id = t_id; // Asigna el ID temporal (original de 'a') al nodo 'b'
    
    string t_nombre = a->nombre; // Guarda temporalmente el nombre del nodo 'a'
    a->nombre = b->nombre; // Asigna el nombre de 'b' al nodo 'a'
    b->nombre = t_nombre; // Asigna el nombre temporal (original de 'a') al nodo 'b'
    
    string t_estado = a->estado; // Guarda temporalmente el estado del nodo 'a'
    a->estado = b->estado; // Asigna el estado de 'b' al nodo 'a'
    b->estado = t_estado; // Asigna el estado temporal (original de 'a') al nodo 'b'
    
    int t_bt = a->burst_total; // Guarda temporalmente el burst total del nodo 'a'
    a->burst_total = b->burst_total; // Asigna el burst total de 'b' al nodo 'a'
    b->burst_total = t_bt; // Asigna el burst total temporal (original de 'a') al nodo 'b'
    
    int t_br = a->burst_restante; // Guarda temporalmente el burst restante del nodo 'a'
    a->burst_restante = b->burst_restante; // Asigna el burst restante de 'b' al nodo 'a'
    b->burst_restante = t_br; // Asigna el burst restante temporal (original de 'a') al nodo 'b'
    
    int t_prio = a->prioridad; // Guarda temporalmente la prioridad del nodo 'a'
    a->prioridad = b->prioridad; // Asigna la prioridad de 'b' al nodo 'a'
    b->prioridad = t_prio; // Asigna la prioridad temporal (original de 'a') al nodo 'b'
    
    int t_mem = a->memoria_mb; // Guarda temporalmente la memoria en MB del nodo 'a'
    a->memoria_mb = b->memoria_mb; // Asigna la memoria de 'b' al nodo 'a'
    b->memoria_mb = t_mem; // Asigna la memoria temporal (original de 'a') al nodo 'b'
} 

// Función que ordena una lista enlazada de procesos por ID usando el algoritmo de burbuja
void burbuja_lista(ListaGlobal* lst) { // Recibe un puntero a la lista global a ordenar
    if (lst->cabeza == NULL) return; // Si la lista está vacía, no hay nada que ordenar, sale de la función
    bool hubo_intercambio; // Variable bandera para controlar si se hizo algún intercambio en la pasada
    do { 
        hubo_intercambio = false; // Resetea la bandera al inicio de cada pasada
        PCB* actual = lst->cabeza; // Comienza desde la cabeza de la lista en cada pasada
        while (actual->siguiente != NULL) { // Recorre la lista mientras exista un nodo siguiente para comparar
            if (actual->id > actual->siguiente->id) { // Compara el ID del nodo actual con el del siguiente
                intercambiar(actual, actual->siguiente); // Si está desordenado, intercambia los datos de ambos nodos
                hubo_intercambio = true; // Marca que hubo al menos un intercambio en esta pasada
            } 
            actual = actual->siguiente; // Avanza al siguiente nodo para continuar la comparación
        } // Fin del bucle while interno (una pasada completa)
    } while (hubo_intercambio); // Repite si hubo al menos un intercambio; si no hubo, la lista ya está ordenada
} 

// 3. Búsqueda en Pila usando Estructura Auxiliar (Regla LIFO)
// Función que busca un valor de memoria dentro de una pila sin destruirla, usando una pila auxiliar
bool buscar_en_pila(PilaMemoria* p, int mem_buscada) { // Recibe puntero a la pila y el valor de memoria a buscar
    PilaMemoria pilaAuxiliar; // Crea una pila auxiliar vacía para almacenar temporalmente los elementos
    bool encontrado = false; // Variable bandera que indica si se encontró el valor buscado
    
    // Desapilar y buscar
    while (p->tope != NULL) { // Mientras la pila original no esté vacía
        int valor = pop(p); // Extrae (desapila) el elemento del tope de la pila original
        if (valor == mem_buscada) encontrado = true; // Si el valor extraído coincide con el buscado, marca como encontrado
        push(&pilaAuxiliar, valor); // Apila el valor extraído en la pila auxiliar para preservarlo
    } 
    // Restaurar a la original
    while (pilaAuxiliar.tope != NULL) { // Mientras la pila auxiliar no esté vacía
        push(p, pop(&pilaAuxiliar)); // Desapila de la auxiliar y apila en la original, restaurando el orden
    } 
    return encontrado; // Retorna true si se encontró el valor, false en caso contrario
} 

// 4. Búsqueda en Cola usando Estructura Auxiliar (Regla FIFO)
// Función que busca un proceso por ID dentro de una cola de prioridad sin destruirla
bool buscar_en_cola(ColaPrioridad* c, int id_buscado) { // Recibe puntero a la cola y el ID del proceso a buscar
    ColaPrioridad colaAuxiliar; // Crea una cola auxiliar vacía para almacenar temporalmente los elementos
    bool encontrado = false; // Variable bandera que indica si se encontró el ID buscado
    
    // Desencolar y buscar
    while (c->cabeza != NULL) { // Mientras la cola original no esté vacía
        PCB* extraido = dequeue(c); // Extrae (desencola) el primer elemento de la cola original
        if (extraido->id == id_buscado) encontrado = true; // Si el ID del elemento extraído coincide con el buscado, marca como encontrado
        enqueue_prioridad(&colaAuxiliar, extraido); // Encola el elemento extraído en la cola auxiliar según su prioridad
        delete extraido; // Libera la memoria del nodo extraído ya que enqueue_prioridad crea una copia
    } 
    // Restaurar a la original
    while (colaAuxiliar.cabeza != NULL) { // Mientras la cola auxiliar no esté vacía
        PCB* extraido = dequeue(&colaAuxiliar); // Extrae el primer elemento de la cola auxiliar
        enqueue_prioridad(c, extraido); // Encola el elemento de vuelta en la cola original según su prioridad
        delete extraido; // Libera la memoria del nodo extraído ya que enqueue_prioridad crea una copia
    } 
    return encontrado; // Retorna true si se encontró el ID, false en caso contrario
} 
