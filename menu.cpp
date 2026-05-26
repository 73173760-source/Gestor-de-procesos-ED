#include <iostream>

using namespace std;

int main() {
    int opcion;

    do {
        
        cout << "\n\n"; 
        cout << "========================================================\n";
        cout << "      SIMULADOR DE PLANIFICACION DE PROCESOS (SO)       \n";
        cout << "========================================================\n";
        cout << "1. Agregar Proceso\n";
        cout << "2. Buscar Proceso\n";
        cout << "3. Modificar Prioridad\n";
        cout << "4. Eliminar Proceso\n";
        cout << "5. Mostrar Todos los Procesos\n";
        cout << "6. Encolar Proceso por Prioridad\n";
        cout << "7. Despachar Proceso\n";
        cout << "8. Visualizar Cola de Prioridad\n";
        cout << "9. Ver Estado de Memoria\n";
        cout << "10. Cambiar Estado\n";
        cout << "11. Salir del Sistema\n";
        cout << "========================================================\n";
        cout << "Ingrese una opcion: ";
        
        // Lectura simple de la opción
        cin >> opcion;
        cout << "\n";

        switch (opcion) {
            case 1:
                cout << "--- AGREGAR NUEVO PROCESO ---\n";
                // Aqui ira el codigo o la llamada a la funcion
                break;
            case 2:
                cout << "--- BUSCAR PROCESO ---\n";
                break;
            case 3:
                cout << "--- MODIFICAR PRIORIDAD ---\n";
                break;
            case 4:
                cout << "--- ELIMINAR PROCESO ---\n";
                break;
            case 5:
                cout << "--- REGISTRO GENERAL DE PROCESOS ---\n";
                break;
            case 6:
                cout << "--- ENCOLAR POR PRIORIDAD ---\n";
                break;
            case 7:
                cout << "--- DESPACHAR PROCESO ---\n";
                break;
            case 8:
                cout << "--- COLA DE PRIORIDAD ACTUAL ---\n";
                break;
            case 9:
                cout << "--- ESTADO DE LA MEMORIA (PILA) ---\n";
                break;
            case 10:
                cout << "--- CAMBIAR ESTADO DE PROCESO ---\n";
                break;
            case 11:
                cout << "Saliendo del simulador...\n";
                break;
            default:
                cout << "Error: Opcion no valida. Intente de nuevo.\n";
                break;
        }

    } while (opcion != 11);

    return 0;
}