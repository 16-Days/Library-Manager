//Proyecto programado 1 - Lenguajes de Programación
//Gestor de bibliotecas
//Estudiantes:
//Duan Espinoza 
//Aaron Piñar

#include <stdio.h>
#include "funciones/manageCatalog.h"
#include "funciones/manageUsers.h"
#include "funciones/prestamo.h"


#include "cJSON.h"

char buscaSimple[100]; //contenido para busqueda simple 

char libro[100];
char autor[100];
char resumen[100];
char genero[100];
char tipoBusqueda[100];


// Funciones para cada opción
void manageCatalog() {
    // Implementación para gestionar el catálogo
    actualizaBaseLibros();
}

//llamada a manageUsers.h
void manageUsers() {
    mainss();
}

void loanHistory() {
    // Implementación para mostrar historial de préstamos
}

void dueLoans() {
    // Implementación para mostrar préstamos vencidos o próximos a vencer
    verificarVencimientoPrestamos();
    
}

void showStatistics() {
    // Implementación para mostrar estadísticas
}

void simpleSearch() {  
    system("cls");
    printf("\n\n----------------------------------------------------------------------------------------------------");
    printf("\n\t\t\tBienvenido a busqueda simple");
    printf("\n\nLa busqueda se realiza en base a: Titulo, Autor, Descripción\n~Indique el contenido relacionado a buscar:");
    getchar(); // Consumir el carácter de nueva línea en el búfer.
    fgets(buscaSimple, sizeof(buscaSimple), stdin);  printf("El resumen es: %s\n", buscaSimple);
    //scanf("%s",buscaSimple);
    buscarLibroPorTexto(buscaSimple);
    printf("\n\n----------------------------------------------------------------------------------------------------");
}

void advancedSearch() {
    //buscarLibroAvanzado("no","no","no","Fantasia");
    
    printf("\n\n----------------------------------------------------------------------------------------------------");
    printf("\n\t\t\tBienvenido a la búsqueda avanzada");

    printf("\n\nDebe rellenar las siguientes etiquetas con la información de búsqueda. Si no desea buscar por esa etiqueta, coloque 'no'. La búsqueda debe ser sin caracteres especiales");

    printf("\n\nIndique el Nombre del libro:");
    getchar(); // Consumir el carácter de nueva línea en el búfer.
    fgets(libro, sizeof(libro), stdin);  printf("El resumen es: %s\n", libro);

    printf("\n\nIndique el autor:");
    fgets(autor, sizeof(autor), stdin);  printf("El resumen es: %s\n", autor);

    printf("\n\nIndique el resumen:");
    fgets(resumen, sizeof(resumen), stdin); printf("El resumen es: %s\n", resumen);

    printf("\n\nIndique el género:");
    fgets(genero, sizeof(genero), stdin);  printf("El resumen es: %s\n", genero);

    printf("\n\nIndique Exacta en el caso de buscar por este medio. Si no, se buscara por contiene:");
    scanf("%s", tipoBusqueda);
    
    if (strcmp(tipoBusqueda, "no") == 0 || strcmp(libro, "no") == 0|| strcmp(autor, "no") == 0|| strcmp(resumen, "no") == 0|| strcmp(genero, "no") == 0) { 
         printf("\n\nNo hay datos suficientes para la busqueda");
    }
    else {
        if (strcmp(tipoBusqueda, "exacta") == 0 || strcmp(tipoBusqueda, "Exacta") == 0) {
            printf("\n\nBúsqueda exacta seleccionada.");
            buscarLibroAvanzado(libro,autor,resumen,genero);
        } 
        else {
            printf("\n\nBúsqueda contiene seleccionada.");

            printf("\n\n Contiene en libros.");
            buscarLibroPorTexto(libro);
            printf("\n\n Contiene en autor.");
            buscarLibroPorTexto(autor);
            printf("\n\n Contiene en resumen.");
            buscarLibroPorTexto(resumen);
            printf("\n\n Contiene en genero.");
            buscarLibroPorTexto(genero);
        }
    }
    
    printf("\n\n----------------------------------------------------------------------------------------------------");
    
}



void borrowBook() {
    // Implementación para realizar préstamo
    prestarEjemplar();
    
}

void returnBook() {
    // Implementación para devolver un ejemplar
    devolverEjemplar();
}

int main() {
    system("cls");
    int mainChoice, subChoice;

    do {
        printf("\n\n\n--- Menu Principal ---\n");
        printf("1. Opciones Operativas\n");
        printf("2. Opciones Generales\n");
        printf("3. Salir\n");
        printf("Ingrese su eleccion: ");
        scanf("%d", &mainChoice);

        switch (mainChoice) {
            case 1:
                do {
                    printf("\n--- Opciones Operativas ---\n");
                    printf("1. Gestion de catalogo\n");
                    printf("2. Gestion de usuarios\n");
                    printf("3. Historial de prestamos\n");
                    printf("4. Vencimiento de prestamos\n");
                    printf("5. Estadisticas\n");
                    printf("6. Volver\n");
                    printf("Ingrese su eleccion: ");
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
                            printf("Opcion no valida. Intente de nuevo.\n");
                    }
                } while (subChoice != 6);
                break;
            case 2:
                do {
                    printf("\n--- Opciones Generales ---\n");
                    printf("1. Busqueda simple\n");
                    printf("2. Busqueda avanzada\n");
                    printf("3. Prestamo de ejemplar\n");
                    printf("4. Devolucion de ejemplar\n");
                    printf("5. Volver\n");
                    printf("Ingrese su eleccion: ");
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
                            printf("Opcion no valida. Intente de nuevo.\n");
                    }
                } while (subChoice != 5);
                break;
            case 3:
                printf("¡Hasta luego!\n");
                break;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (mainChoice != 3);

    return 0;
}
