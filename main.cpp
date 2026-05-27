#include <iostream> // Incluir la libreria estandar de entrada/salida (cin, cout)
#include <string>   // Incluir la libreria para el manejo de cadenas de texto (string)

using namespace std; // Usar el espacio de nombres estandar para no escribir std:: en cada instruccion

// Estructura PCB (Bloque de Control de Proceso): representa un proceso del sistema operativo
struct PCB {
    int id;              // Identificador unico del proceso
    string nombre;       // Nombre descriptivo del proceso
    string estado;       // Estado actual del proceso (LISTO, EJECUTANDO, etc.)
    int burst_total;     // Tiempo total de ejecucion (rafaga) que necesita el proceso
    int burst_restante;  // Tiempo de ejecucion que aun le queda al proceso
    int prioridad;       // Nivel de prioridad del proceso (1 = mas alta, 5 = mas baja)
    int memoria_mb;      // Cantidad de memoria en megabytes que requiere el proceso
    PCB* siguiente;      // Puntero al siguiente nodo PCB en la lista enlazada
};

// Estructura NodoMemoria: representa un bloque de memoria asignado en la pila
struct NodoMemoria {
    int memoria_mb;          // Tamanio del bloque de memoria en megabytes
    NodoMemoria* siguiente;  // Puntero al siguiente nodo de memoria en la pila
};

// Estructura ListaGlobal: lista enlazada que almacena todos los procesos registrados
struct ListaGlobal {
    PCB* cabeza = NULL;  // Puntero al primer nodo (inicio) de la lista, inicializado en NULL
    PCB* cola = NULL;    // Puntero al ultimo nodo (final) de la lista, inicializado en NULL
    int contador = 0;    // Contador que lleva la cuenta del numero total de procesos en la lista
};

// Estructura ColaPrioridad: cola enlazada ordenada por prioridad para el despacho de CPU
struct ColaPrioridad {
    PCB* cabeza = NULL;  // Puntero al frente de la cola (proceso con mayor prioridad), inicializado en NULL
    PCB* cola = NULL;    // Puntero al final de la cola (proceso con menor prioridad), inicializado en NULL
};

// Estructura PilaMemoria: pila enlazada que simula la asignacion de bloques de memoria
struct PilaMemoria {
    NodoMemoria* tope = NULL;  // Puntero al tope de la pila (ultimo bloque asignado), inicializado en NULL
    int memoria_total = 0;     // Acumulador del total de memoria actualmente en uso (en MB)
};

// --- LISTAS ---

// Funcion para insertar un proceso al final de la lista global
void insertar_al_final(ListaGlobal* lst, PCB* pcb_original) {
    PCB* nuevo_pcb = new PCB(*pcb_original);  // Crear una copia dinamica del PCB original en el heap
    nuevo_pcb->siguiente = NULL;              // El nuevo nodo sera el ultimo, asi que su siguiente es NULL
    if (lst->cabeza == NULL) {                // Si la lista esta vacia (no tiene nodos)
        lst->cabeza = nuevo_pcb;              // El nuevo nodo se convierte en la cabeza de la lista
        lst->cola = nuevo_pcb;                // Tambien se convierte en la cola ya que es el unico nodo
    } else {                                  // Si la lista ya tiene al menos un nodo
        lst->cola->siguiente = nuevo_pcb;     // Enlazar el ultimo nodo actual con el nuevo nodo
        lst->cola = nuevo_pcb;                // Actualizar el puntero de cola para apuntar al nuevo ultimo nodo
    }
    lst->contador++;                          // Incrementar el contador de procesos en la lista
}

// Funcion para eliminar un proceso de la lista global por su ID
void eliminar(ListaGlobal* lst, int id_buscado) {
    if (lst->cabeza == NULL) return;                              // Si la lista esta vacia, no hay nada que eliminar
    PCB* actual = lst->cabeza;                                    // Puntero que recorrera la lista desde la cabeza
    PCB* anterior = NULL;                                         // Puntero al nodo anterior al actual (para re-enlazar)
    while (actual != NULL && actual->id != id_buscado) {          // Recorrer la lista mientras no se encuentre el ID
        anterior = actual;                                        // Guardar el nodo actual como anterior
        actual = actual->siguiente;                               // Avanzar al siguiente nodo
    }
    if (actual == NULL) return;                                   // Si se recorrio toda la lista sin encontrar el ID, salir
    if (anterior == NULL) lst->cabeza = actual->siguiente;        // Si el nodo a eliminar es la cabeza, actualizar la cabeza
    else anterior->siguiente = actual->siguiente;                 // Si no, saltar el nodo actual enlazando anterior con siguiente
    if (actual == lst->cola) lst->cola = anterior;                // Si el nodo eliminado era la cola, actualizar el puntero de cola
    delete actual;                                                // Liberar la memoria del nodo eliminado
    lst->contador--;                                              // Decrementar el contador de procesos
}

// --- COLAS ---

// Funcion para insertar un proceso en la cola de prioridad (mantiene orden por prioridad ascendente)
void enqueue_prioridad(ColaPrioridad* cola, PCB* pcb_original) {
    PCB* nuevo_nodo = new PCB(*pcb_original);  // Crear una copia dinamica del PCB original
    nuevo_nodo->siguiente = NULL;              // Inicializar el puntero siguiente del nuevo nodo en NULL
    
    // Si la cola esta vacia O el nuevo nodo tiene mayor prioridad (numero menor) que la cabeza
    if (cola->cabeza == NULL || nuevo_nodo->prioridad < cola->cabeza->prioridad) {
        nuevo_nodo->siguiente = cola->cabeza;  // El siguiente del nuevo nodo apunta a la cabeza actual
        cola->cabeza = nuevo_nodo;             // El nuevo nodo se convierte en la nueva cabeza de la cola
        return;                                // Salir de la funcion ya que la insercion esta completa
    }
    PCB* actual = cola->cabeza;                // Puntero para recorrer la cola desde la cabeza
    // Avanzar mientras el siguiente nodo tenga prioridad menor o igual (numero mayor o igual) que el nuevo
    while (actual->siguiente != NULL && actual->siguiente->prioridad <= nuevo_nodo->prioridad) {
        actual = actual->siguiente;            // Avanzar al siguiente nodo en la cola
    }
    nuevo_nodo->siguiente = actual->siguiente; // Enlazar el nuevo nodo con el nodo que estaba despues de actual
    actual->siguiente = nuevo_nodo;            // Insertar el nuevo nodo despues del nodo actual
}

// Funcion para extraer (dequeue) el proceso con mayor prioridad del frente de la cola
PCB* dequeue(ColaPrioridad* cola) {
    if (cola->cabeza == NULL) return NULL;          // Si la cola esta vacia, retornar NULL
    PCB* temp = cola->cabeza;                       // Guardar referencia al nodo del frente
    PCB* pcb_extraido = new PCB(*temp);             // Crear una copia del proceso extraido para devolverla
    cola->cabeza = cola->cabeza->siguiente;         // Avanzar la cabeza de la cola al siguiente nodo
    if(cola->cabeza == NULL) cola->cola = NULL;     // Si la cola quedo vacia, tambien poner la cola en NULL
    delete temp;                                    // Liberar la memoria del nodo original del frente
    return pcb_extraido;                            // Retornar la copia del proceso extraido
}

// --- PILAS ---

// Funcion push: apilar un bloque de memoria en la pila
void push(PilaMemoria* p, int bloque_memoria) {
    if (bloque_memoria <= 0) return;                // Si el bloque es 0 o negativo, no hacer nada (validacion)
    NodoMemoria* nuevo_nodo = new NodoMemoria();    // Crear un nuevo nodo de memoria en el heap
    nuevo_nodo->memoria_mb = bloque_memoria;        // Asignar el tamanio del bloque al nuevo nodo
    nuevo_nodo->siguiente = p->tope;                // El siguiente del nuevo nodo apunta al tope actual de la pila
    p->tope = nuevo_nodo;                           // El nuevo nodo se convierte en el nuevo tope de la pila
    p->memoria_total += bloque_memoria;             // Sumar el bloque al acumulador de memoria total en uso
}

// Funcion pop: desapilar y retornar el bloque de memoria del tope de la pila
int pop(PilaMemoria* p) {
    if (p->tope == NULL) return -1;                 // Si la pila esta vacia, retornar -1 como indicador de error
    NodoMemoria* temp = p->tope;                    // Guardar referencia al nodo del tope actual
    int bloque_liberado = temp->memoria_mb;         // Almacenar el tamanio del bloque que se va a liberar
    p->tope = p->tope->siguiente;                   // Mover el tope al siguiente nodo de la pila
    p->memoria_total -= bloque_liberado;            // Restar el bloque liberado del acumulador de memoria total
    delete temp;                                    // Liberar la memoria del nodo desapilado
    return bloque_liberado;                         // Retornar el tamanio del bloque de memoria liberado
}


// 1. Busqueda Lineal en Lista
// Recorre la lista nodo por nodo buscando un proceso por su ID
PCB* busquedaLineal(ListaGlobal* lst, int id_buscado) {
    PCB* actual = lst->cabeza;                      // Iniciar el recorrido desde la cabeza de la lista
    while (actual != NULL) {                        // Mientras haya nodos por recorrer
        if (actual->id == id_buscado) return actual;// Si el ID del nodo actual coincide, retornar el puntero al nodo
        actual = actual->siguiente;                 // Avanzar al siguiente nodo en la lista
    }
    return NULL;                                    // Si se recorrio toda la lista sin encontrar, retornar NULL
}

// 2. Ordenamiento por Burbuja en Lista (Sin librerias complejas)

// Funcion auxiliar para intercambiar los datos de dos nodos PCB (sin mover los punteros)
void intercambiar(PCB* a, PCB* b) {
    // Intercambio manual con variables temporales
    int t_id = a->id;           // Guardar temporalmente el ID del nodo 'a'
    a->id = b->id;              // Copiar el ID de 'b' hacia 'a'
    b->id = t_id;               // Asignar el ID temporal (original de 'a') a 'b'
    
    string t_nombre = a->nombre; // Guardar temporalmente el nombre del nodo 'a'
    a->nombre = b->nombre;       // Copiar el nombre de 'b' hacia 'a'
    b->nombre = t_nombre;        // Asignar el nombre temporal (original de 'a') a 'b'
    
    string t_estado = a->estado; // Guardar temporalmente el estado del nodo 'a'
    a->estado = b->estado;       // Copiar el estado de 'b' hacia 'a'
    b->estado = t_estado;        // Asignar el estado temporal (original de 'a') a 'b'
    
    int t_bt = a->burst_total;       // Guardar temporalmente el burst total del nodo 'a'
    a->burst_total = b->burst_total; // Copiar el burst total de 'b' hacia 'a'
    b->burst_total = t_bt;           // Asignar el burst total temporal (original de 'a') a 'b'
    
    int t_br = a->burst_restante;          // Guardar temporalmente el burst restante del nodo 'a'
    a->burst_restante = b->burst_restante; // Copiar el burst restante de 'b' hacia 'a'
    b->burst_restante = t_br;              // Asignar el burst restante temporal (original de 'a') a 'b'
    
    int t_prio = a->prioridad;     // Guardar temporalmente la prioridad del nodo 'a'
    a->prioridad = b->prioridad;   // Copiar la prioridad de 'b' hacia 'a'
    b->prioridad = t_prio;         // Asignar la prioridad temporal (original de 'a') a 'b'
    
    int t_mem = a->memoria_mb;     // Guardar temporalmente la memoria del nodo 'a'
    a->memoria_mb = b->memoria_mb; // Copiar la memoria de 'b' hacia 'a'
    b->memoria_mb = t_mem;         // Asignar la memoria temporal (original de 'a') a 'b'
}

// Funcion de ordenamiento burbuja: ordena la lista enlazada por ID de menor a mayor
void burbuja_lista(ListaGlobal* lst) {
    if (lst->cabeza == NULL) return;                // Si la lista esta vacia, no hay nada que ordenar
    bool hubo_intercambio;                          // Bandera para saber si se hizo algun intercambio en la pasada
    do {                                            // Inicio del ciclo do-while (se repite hasta que no haya intercambios)
        hubo_intercambio = false;                   // Reiniciar la bandera al inicio de cada pasada
        PCB* actual = lst->cabeza;                  // Comenzar la pasada desde la cabeza de la lista
        while (actual->siguiente != NULL) {         // Recorrer hasta el penultimo nodo
            if (actual->id > actual->siguiente->id) { // Si el ID actual es mayor que el del siguiente (desordenado)
                intercambiar(actual, actual->siguiente); // Intercambiar los datos de ambos nodos
                hubo_intercambio = true;             // Marcar que hubo al menos un intercambio
            }
            actual = actual->siguiente;              // Avanzar al siguiente nodo
        }
    } while (hubo_intercambio);                      // Repetir si hubo intercambios (la lista aun no esta ordenada)
    cout << "-> Lista ordenada por ID utilizando Burbuja.\n"; // Mensaje de confirmacion de ordenamiento completado
}

// 3. Busqueda en Pila usando Pila Auxiliar
// Busca un bloque de memoria especifico en la pila sin perder los elementos
bool buscar_en_pila(PilaMemoria* p, int mem_buscada) {
    PilaMemoria pilaAuxiliar;                       // Crear una pila auxiliar temporal para guardar los elementos
    bool encontrado = false;                        // Bandera para indicar si se encontro el bloque buscado
    
    // Desapilar todos los elementos de la pila original y buscar el valor
    while (p->tope != NULL) {                       // Mientras la pila original no este vacia
        int valor = pop(p);                         // Extraer el elemento del tope de la pila original
        if (valor == mem_buscada) encontrado = true;// Si el valor coincide con el buscado, marcar como encontrado
        push(&pilaAuxiliar, valor);                 // Apilar el elemento extraido en la pila auxiliar
    }
    // Restaurar la pila original desde la pila auxiliar (orden original se mantiene)
    while (pilaAuxiliar.tope != NULL) {             // Mientras la pila auxiliar no este vacia
        push(p, pop(&pilaAuxiliar));                // Desapilar de la auxiliar y apilar en la original
    }
    return encontrado;                              // Retornar si se encontro o no el bloque buscado
}

// 4. Busqueda en Cola usando Cola Auxiliar
// Busca un proceso por ID en la cola de prioridad sin perder los elementos
bool buscar_en_cola(ColaPrioridad* c, int id_buscado) {
    ColaPrioridad colaAuxiliar;                     // Crear una cola auxiliar temporal para guardar los procesos
    bool encontrado = false;                        // Bandera para indicar si se encontro el proceso buscado
    
    // Extraer todos los procesos de la cola original y buscar el ID
    while (c->cabeza != NULL) {                     // Mientras la cola original no este vacia
        PCB* extraido = dequeue(c);                 // Extraer el proceso del frente de la cola original
        if (extraido->id == id_buscado) encontrado = true; // Si el ID coincide, marcar como encontrado
        enqueue_prioridad(&colaAuxiliar, extraido); // Encolar el proceso en la cola auxiliar
        delete extraido;                            // Liberar la copia temporal del proceso extraido
    }
    // Restaurar la cola original desde la cola auxiliar
    while (colaAuxiliar.cabeza != NULL) {           // Mientras la cola auxiliar no este vacia
        PCB* extraido = dequeue(&colaAuxiliar);     // Extraer el proceso del frente de la cola auxiliar
        enqueue_prioridad(c, extraido);             // Encolar el proceso de vuelta en la cola original
        delete extraido;                            // Liberar la copia temporal del proceso extraido
    }
    return encontrado;                              // Retornar si se encontro o no el proceso buscado
}



// Funcion para mostrar todos los procesos registrados en la lista global
void mostrar_todos(ListaGlobal* lst) {
    PCB* actual = lst->cabeza;                      // Iniciar el recorrido desde la cabeza de la lista
    while (actual != NULL) {                        // Mientras haya nodos por mostrar
        // Imprimir los datos del proceso: ID, nombre, estado y prioridad
        cout << "ID: " << actual->id << " | Nombre: " << actual->nombre 
             << " | Estado: " << actual->estado << " | Prio: " << actual->prioridad << "\n";
        actual = actual->siguiente;                 // Avanzar al siguiente nodo en la lista
    }
}

// Funcion para mostrar el contenido de la cola de prioridad
void mostrar_cola(ColaPrioridad* cola) {
    PCB* actual = cola->cabeza;                     // Iniciar el recorrido desde el frente de la cola
    cout << "[FRENTE] -> ";                         // Imprimir indicador del frente de la cola
    while (actual != NULL) {                        // Mientras haya procesos en la cola
        // Imprimir el ID y la prioridad de cada proceso en la cola
        cout << "(ID: " << actual->id << ", Prio: " << actual->prioridad << ") -> ";
        actual = actual->siguiente;                 // Avanzar al siguiente proceso en la cola
    }
    cout << "[FINAL]\n";                            // Imprimir indicador del final de la cola
}

// Funcion para visualizar el estado actual de la pila de memoria
void ver_estado_memoria(PilaMemoria* p) {
    // Imprimir el total de memoria en uso y el encabezado del tope de la pila
    cout << "TOTAL EN USO: " << p->memoria_total << " MB\n[TOPE]\n";
    NodoMemoria* actual = p->tope;                  // Iniciar el recorrido desde el tope de la pila
    while(actual != NULL) {                         // Mientras haya bloques de memoria apilados
        cout << " | " << actual->memoria_mb << " MB |\n"; // Imprimir el tamanio de cada bloque
        actual = actual->siguiente;                 // Avanzar al siguiente bloque en la pila
    }
}

// Funcion principal del programa: muestra el menu y gestiona las opciones del usuario
int main() {
    int opcion;                // Variable para almacenar la opcion seleccionada por el usuario
    ListaGlobal lista;         // Declarar e inicializar la lista global de procesos
    ColaPrioridad cola_cpu;    // Declarar e inicializar la cola de prioridad para despacho de CPU
    PilaMemoria pila;          // Declarar e inicializar la pila de memoria
    
    int tmp_id, tmp_prio, tmp_mem; // Variables temporales para lectura de ID, prioridad y memoria
    PCB pcb_temp;              // Variable temporal para construir un nuevo proceso antes de insertarlo

    do {                       // Inicio del bucle principal del menu (se repite hasta que el usuario elija salir)
        // Imprimir el encabezado del menu principal
        cout << "\n========================================================\n";
        cout << "      SIMULADOR DE PLANIFICACION Y ALGORITMOS (SO)      \n";
        cout << "========================================================\n";
        // Imprimir las opciones del menu
        cout << "1. Agregar Proceso (Lista y Cola)\n";                     // Opcion 1: Crear nuevo proceso
        cout << "2. Buscar Proceso (Busqueda Lineal en Lista)\n";          // Opcion 2: Busqueda lineal
        cout << "3. Buscar en Pila de Memoria (Con Pila Auxiliar)\n";      // Opcion 3: Busqueda en pila
        cout << "4. Buscar en Cola de Prioridad (Con Cola Auxiliar)\n";    // Opcion 4: Busqueda en cola
        cout << "5. Ordenar Lista por ID (Metodo Burbuja)\n";              // Opcion 5: Ordenar con burbuja
        cout << "6. Mostrar Registro Global (Lista)\n";                    // Opcion 6: Ver todos los procesos
        cout << "7. Despachar Proceso (Dequeue CPU y Push a Pila)\n";      // Opcion 7: Despachar de cola a pila
        cout << "8. Ver Cola de Prioridad Actual\n";                       // Opcion 8: Ver cola de CPU
        cout << "9. Ver Estado de Memoria (Pila)\n";                       // Opcion 9: Ver pila de memoria
        cout << "10. Eliminar Proceso del Registro\n";                     // Opcion 10: Eliminar proceso
        cout << "11. Salir\n";                                             // Opcion 11: Terminar programa
        cout << "========================================================\n"; // Linea separadora inferior
        cout << "Ingrese una opcion: ";  // Solicitar al usuario que ingrese una opcion
        cin >> opcion;                   // Leer la opcion seleccionada por el usuario desde teclado
        cout << "\n";                    // Imprimir salto de linea para separar visualmente

        switch (opcion) {                // Evaluar la opcion seleccionada por el usuario
            case 1: // Caso 1: Agregar un nuevo proceso a la lista y a la cola
                cout << "ID del proceso: "; cin >> pcb_temp.id;                     // Solicitar y leer el ID del proceso
                cout << "Nombre: "; cin >> ws; getline(cin, pcb_temp.nombre);       // Leer el nombre completo (con espacios) del proceso
                cout << "Prioridad (1 alta - 5 baja): "; cin >> pcb_temp.prioridad; // Solicitar y leer la prioridad
                cout << "Memoria (MB): "; cin >> pcb_temp.memoria_mb;               // Solicitar y leer la memoria requerida
                pcb_temp.estado = "LISTO";       // Asignar el estado inicial del proceso como "LISTO"
                pcb_temp.burst_total = 10;       // Asignar un valor por defecto de 10 unidades de burst total
                pcb_temp.burst_restante = 10;    // El burst restante inicia igual al burst total
                
                insertar_al_final(&lista, &pcb_temp);       // Insertar el proceso al final de la lista global
                enqueue_prioridad(&cola_cpu, &pcb_temp);    // Encolar el proceso en la cola de prioridad de CPU
                cout << "-> Proceso agregado exitosamente.\n"; // Mensaje de confirmacion de agregado exitoso
                break;                                       // Salir del switch
            case 2: // Caso 2: Buscar un proceso por ID en la lista global
                cout << "Ingrese ID a buscar en Lista: "; cin >> tmp_id;                              // Solicitar y leer el ID a buscar
                if(busquedaLineal(&lista, tmp_id) != NULL) cout << "-> Encontrado en Lista Global.\n"; // Si se encontro, mostrar mensaje
                else cout << "-> No encontrado en Lista Global.\n";                                   // Si no se encontro, mostrar mensaje
                break;                                                                                // Salir del switch
            case 3: // Caso 3: Buscar un bloque de memoria en la pila
                cout << "Ingrese MB de memoria a buscar en la Pila: "; cin >> tmp_mem;                             // Solicitar y leer los MB a buscar
                if(buscar_en_pila(&pila, tmp_mem)) cout << "-> Bloque de " << tmp_mem << " MB encontrado en uso.\n"; // Si se encontro, mostrar mensaje
                else cout << "-> No hay ningun bloque de ese tamano asignado.\n";                                  // Si no se encontro, mostrar mensaje
                break;                                                                                             // Salir del switch
            case 4: // Caso 4: Buscar un proceso por ID en la cola de prioridad
                cout << "Ingrese ID de proceso a buscar en la Cola: "; cin >> tmp_id;                // Solicitar y leer el ID a buscar
                if(buscar_en_cola(&cola_cpu, tmp_id)) cout << "-> El proceso esta esperando en la Cola.\n"; // Si se encontro, mostrar mensaje
                else cout << "-> El proceso NO esta en la Cola.\n";                                  // Si no se encontro, mostrar mensaje
                break;                                                                               // Salir del switch
            case 5: // Caso 5: Ordenar la lista global por ID usando el metodo burbuja
                burbuja_lista(&lista);   // Llamar a la funcion de ordenamiento burbuja sobre la lista
                break;                   // Salir del switch
            case 6: // Caso 6: Mostrar todos los procesos del registro global
                mostrar_todos(&lista);   // Llamar a la funcion que imprime todos los procesos de la lista
                break;                   // Salir del switch
            case 7: { // Caso 7: Despachar el proceso con mayor prioridad de la cola
                PCB* extraido = dequeue(&cola_cpu);            // Extraer el proceso del frente de la cola de CPU
                if (extraido != NULL) {                        // Si la cola no estaba vacia (se extrajo un proceso)
                    cout << "-> Despachando proceso: " << extraido->nombre << "\n"; // Mostrar el nombre del proceso despachado
                    push(&pila, extraido->memoria_mb);         // Apilar la memoria del proceso en la pila de memoria
                    PCB* ref = busquedaLineal(&lista, extraido->id); // Buscar el proceso en la lista global por su ID
                    if(ref) ref->estado = "EJECUTANDO";        // Si se encontro, cambiar su estado a "EJECUTANDO"
                    delete extraido;                           // Liberar la memoria de la copia del proceso extraido
                } else cout << "-> Cola vacia.\n";             // Si la cola estaba vacia, mostrar mensaje
                break;                                         // Salir del switch
            }
            case 8: // Caso 8: Mostrar el contenido actual de la cola de prioridad
                mostrar_cola(&cola_cpu);        // Llamar a la funcion que imprime la cola de prioridad
                break;                          // Salir del switch
            case 9: // Caso 9: Mostrar el estado actual de la pila de memoria
                ver_estado_memoria(&pila);      // Llamar a la funcion que imprime la pila de memoria
                break;                          // Salir del switch
            case 10: // Caso 10: Eliminar un proceso del registro global por su ID
                cout << "Ingrese ID a eliminar: "; cin >> tmp_id;        // Solicitar y leer el ID del proceso a eliminar
                eliminar(&lista, tmp_id);                                // Llamar a la funcion para eliminar el proceso de la lista
                cout << "-> Operacion de eliminacion terminada.\n";      // Mensaje de confirmacion de eliminacion
                break;                                                   // Salir del switch
        }
    } while (opcion != 11);  // Repetir el menu mientras la opcion no sea 11 (Salir)

    return 0;                 // Retornar 0 al sistema operativo indicando que el programa termino correctamente
}
