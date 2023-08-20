//Proyecto programado 1 - Lenguajes de Programación
//Gestor de bibliotecas
//Estudiantes:
//Duan Espinoza 
//Aaron Piñar
#include <stdio.h>

// Estructuras para almacenar datos
struct CatalogEntry {
    char name[100];
    char author[100];
    int year;
    char genre[50];
    char summary[200];
    int quantity;
};

struct User {
    int id;
    char name[100];
    char address[200];
};

struct Loan {
    int loanId;
    int userId;
    int bookId;
    // Otros campos necesarios para el préstamo
};

// Funciones para cada opción
void manageCatalog() {
    // Implementación para gestionar el catálogo
}

void manageUsers() {
    // Implementación para gestionar los usuarios
}

void loanHistory() {
    // Implementación para mostrar historial de préstamos
}

void dueLoans() {
    // Implementación para mostrar préstamos vencidos o próximos a vencer
}

void showStatistics() {
    // Implementación para mostrar estadísticas
}

void simpleSearch() {
    // Implementación para búsqueda simple
}

void advancedSearch() {
    // Implementación para búsqueda avanzada
}

void borrowBook() {
    // Implementación para realizar préstamo
}

void returnBook() {
    // Implementación para devolver un ejemplar
}

int main() {
    int mainChoice, subChoice;

    do {
        printf("\n--- Menú Principal ---\n");
        printf("1. Opciones Operativas\n");
        printf("2. Opciones Generales\n");
        printf("3. Salir\n");
        printf("Ingrese su elección: ");
        scanf("%d", &mainChoice);

        switch (mainChoice) {
            case 1:
                do {
                    printf("\n--- Opciones Operativas ---\n");
                    printf("1. Gestión de catálogo\n");
                    printf("2. Gestión de usuarios\n");
                    printf("3. Historial de préstamos\n");
                    printf("4. Vencimiento de préstamos\n");
                    printf("5. Estadísticas\n");
                    printf("6. Volver\n");
                    printf("Ingrese su elección: ");
                    scanf("%d", &subChoice);

                    switch (subChoice) {
                        case 1:
                            manageCatalog();
                            break;
                        case 2:
                            manageUsers();
                            break;
                        case 3:
                            loanHistory();
                            break;
                        case 4:
                            dueLoans();
                            break;
                        case 5:
                            showStatistics();
                            break;
                        case 6:
                            break;
                        default:
                            printf("Opción no válida. Intente de nuevo.\n");
                    }
                } while (subChoice != 6);
                break;
            case 2:
                do {
                    printf("\n--- Opciones Generales ---\n");
                    printf("1. Búsqueda simple\n");
                    printf("2. Búsqueda avanzada\n");
                    printf("3. Préstamo de ejemplar\n");
                    printf("4. Devolución de ejemplar\n");
                    printf("5. Volver\n");
                    printf("Ingrese su elección: ");
                    scanf("%d", &subChoice);

                    switch (subChoice) {
                        case 1:
                            simpleSearch();
                            break;
                        case 2:
                            advancedSearch();
                            break;
                        case 3:
                            borrowBook();
                            break;
                        case 4:
                            returnBook();
                            break;
                        case 5:
                            break;
                        default:
                            printf("Opción no válida. Intente de nuevo.\n");
                    }
                } while (subChoice != 5);
                break;
            case 3:
                printf("¡Hasta luego!\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (mainChoice != 3);

    return 0;
}
