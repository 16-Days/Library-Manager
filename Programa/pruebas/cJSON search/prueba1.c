#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

//Prueba de búsqueda por etiquetas en cJSON

// Función para buscar personas por ciudad en un JSON dado
void buscarPersonasPorCiudad(cJSON *root, const char *ciudadBuscada) {
    cJSON *personasArray = cJSON_GetObjectItem(root, "personas");
    if (!personasArray || !cJSON_IsArray(personasArray)) {
        printf("No se encontró el array 'personas'.\n");
        return;
    }

    cJSON *persona;
    int personasEncontradas = 0;

    cJSON_ArrayForEach(persona, personasArray) {
        cJSON *ciudad = cJSON_GetObjectItem(persona, "ciudad");
        if (ciudad && cJSON_IsString(ciudad) && strcmp(ciudad->valuestring, ciudadBuscada) == 0) {
            cJSON *nombre = cJSON_GetObjectItem(persona, "nombre");
            if (nombre && cJSON_IsString(nombre)) {
                printf("Se encontró a %s en %s.\n", nombre->valuestring, ciudadBuscada);
                personasEncontradas++;
            }
        }
    }

    if (personasEncontradas == 0) {
        printf("No se encontraron personas en %s.\n", ciudadBuscada);
    }
}

int main() {
    FILE *file = fopen("data.json", "r");
    if (!file) {
        printf("No se pudo abrir el archivo JSON.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *jsonStr = (char *)malloc(fileSize + 1);
    if (!jsonStr) {
        printf("Error al asignar memoria.\n");
        fclose(file);
        return 1;
    }

    fread(jsonStr, 1, fileSize, file);
    fclose(file);

    jsonStr[fileSize] = '\0';

    cJSON *root = cJSON_Parse(jsonStr);
    if (!root) {
        printf("Error al analizar JSON.\n");
        free(jsonStr);
        return 1;
    }

    const char *ciudadBuscada = "México";
    buscarPersonasPorCiudad(root, ciudadBuscada);

    cJSON_Delete(root);
    free(jsonStr);

    printf("Presiona Enter para salir...\n");
    getchar();  // Espera a que se presione Enter
    return 0;
}
