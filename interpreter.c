#include "interpreter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Funkcja wyświetlająca wszystkie dostępne funkcje
void displayAvailableFunctions(FunctionDescriptor *functions, uint32_t count) {
    for (int i = 0; i < count; i++) {
        printf("- %s %s(%s)\n", typeDictionary[functions[i].returnType], functions[i].name, typeDictionary[functions[i].argumentType]);
    }
}

// Funkcja, która na podstawie podanego argumentu wywnioskuje jakiego jest on typu
enum DataType detectDataType(char *argument) {
    int length = strlen(argument);  // Długość argumentu (tekstu)
    int dotCount = 0;               // Ilość kropek - potrzebne do wykrywania floatów
    int isInt = 1;                  // Zmienna sprawdzająca czy argument jest liczbą całkowitą
    for (int i = 0; i < length; i++) {
        // Sprawdzamy czy potencjalna liczba jest ujemna
        if (i == 0 && argument[i] == '-') continue;

        // Sprawdzamy czy argument zawiera kropkę
        if (argument[i] == '.') {
            dotCount++;
            isInt = 0;  // Skoro mamy kropkę to "liczba" nie jest liczbą całkowitą
            
            if (dotCount > 1) {
                return STRING;  // Jeżeli mamy więcej niż jedną kropkę mamy do czynienia z ciągiem snaków (STRING)
            }
        }
        // Inaczej sprawdzamy czy znaki wykraczają poza przedział od 0 do 9 w kodzie ASCII 
        else if (argument[i] < '0' || argument[i] > '9') {
            return STRING;  // Jeżeli tak mamy do czynienia z STRINGiem
        }
    }
    
    // Jeśli doszliśmy dotąd mamy doczynienia z liczbą
    if (isInt) return INT;
    return FLOAT;
}

// Funkcja zajmująca się wywołaniem odpowiedniej funkcji i zwróceniem wyniku
any executeFunction(char *name, char *argument, FunctionDescriptor *functions, uint32_t count) {
    // Sprawdzamy typ argumentu
    enum DataType argumentType = detectDataType(argument);
    
    // Zmienne przechowujące "przekonwertowany" argument na odpowiedni typ
    any convertedArgument = 0;
    float *floatArg = 0;    // Osobna zmienna na floaty ponieważ typu float nie można castować do void*
    
    // Sprawdzamy typ argumentu
    switch (argumentType) {
        case INT:
            convertedArgument = (any)atoi(argument);    // Integer? - konwertujemy na inta
            break;
        case FLOAT: {
            // Jeżeli mamy floata musimy użyć wskaźnika do przekazania danych
            floatArg = (float*)malloc(sizeof(float));   // Alokujemy pamięć dla zmiennej typu float
            *floatArg = atof(argument);                 // Teraz możemy przypisać wartość
            convertedArgument = (any)floatArg;          // I na końcu zamiast wartości przekazujemy wskaźnik na wartość
            break;
        }
        case STRING:
            convertedArgument = (any)argument;      // Jeżeli mamy stringa po prostu castujemy z char* do any [void*]
            break;
    }

    // Teraz szukamy odpowiedniej funkcji do wywołania na podstawie argumentu
    // Deklarujemy indeksy do różnych typów funkcji ponieważ może istnieć
    // parę funkcji o tych samych nazwach ale innych typach argumentów
    int floatFunctionIdx = -1, intFunctionIdx = -1, stringFunctionIdx = -1;

    // Pętla szukająca odpowiedniej funkcji
    for (int i = 0; i < count; i++) {
        // Używamy strcmp z libc
        if (strcmp(functions[i].name, name) == 0) {
            // Mamy funkcję o poprawnej nazwie, teraz argument...
            switch (functions[i].argumentType) {
                case INT:
                    intFunctionIdx = i;
                    break;
                case FLOAT:
                    floatFunctionIdx = i;
                    break;
                case STRING:
                    stringFunctionIdx = i;
                    break;
            }
        }
    }

    // Jeżeli wszystkie indeksy wynoszą -1, nie ma funkcji o takiej nazwie i takim argumencie
    if (floatFunctionIdx == -1 && intFunctionIdx == -1 && stringFunctionIdx == -1) {
        printf("\033[91merr: Nie znaleziono funkcji \033[93m%s(%s)\033[0m\n", name, typeDictionary[argumentType]);
        return 0;
    }

    // Wykonujemy odpowiednią funkcję na podstawie uzyskanego indeksu funkcji
    if (intFunctionIdx >= 0 && argumentType == INT) {   // Przypadek 1 - Funkcja przyjmuje wartość int, argument jest również int
        any ret = functions[intFunctionIdx].function(convertedArgument);    // Wykonujemy funkcję
        
        // Mamy już zwróconą wartość, teraz sprawdzamy jaki mamy typ tej watrtości i wypisujemy wynik działania funkcji
        switch (functions[intFunctionIdx].returnType) {
            case INT:
                printf("Wynik: %d\n", (int)ret);
                break;
            case FLOAT:
                float tmp = (float)(int)ret;
                printf("Wynik: %f\n", tmp / FLOAT_MULTIPLIER);
                break;
            case STRING:
                printf("Wynik: %s\n", (char*)ret);
                break;
            default:
                break;
        }
    }
    // Przypadek 2 - Funkcja przyjmuje wartość float, a argumentem jest float lub int
    else if (floatFunctionIdx >= 0 && (argumentType == INT || argumentType == FLOAT)) {
        // Jeżeli mamy argument typu int musimy przekonwertować go na wskaźnik typu float
        // z powodu wyżej już wymienionych ograniczeń - nie można przechowywać wartości float w void*
        if (argumentType == INT) {
            floatArg = (float*)malloc(sizeof(float));
            *floatArg = atof(argument);
            convertedArgument = (any)floatArg;
        }
        
        any ret = functions[floatFunctionIdx].function(convertedArgument);  // Wykonujemy funkcję
        // Sprawdzamy i wypisujemy wynik
        switch (functions[floatFunctionIdx].returnType) {
            case INT:
                printf("Wynik: %d\n", (int)ret);
                break;
            case FLOAT:
                float tmp = (float)(int)ret;
                printf("Wynik: %f\n", tmp / FLOAT_MULTIPLIER);
                break;
            case STRING:
                printf("Wynik: %s\n", (char*)ret);
                break;
            default:
                break;
        }        
    }
    // Przypadek 3 - Mamy funkcję, która przyjmuje argument typu string
    else if (stringFunctionIdx >= 0) {
        // Wykonujemy funkcję z ORYGINALNYM* argumentem
        // *zmienna convertedArgument może tak na prawdę być w tym momencie intem lub floatem a potrzebujemy char*
        any ret = functions[stringFunctionIdx].function((any)argument);
        
        // Sprawdzamy wynik działania funkcji
        switch (functions[stringFunctionIdx].returnType) {
            case INT:
                printf("Wynik: %d\n", (int)ret);
                break;
            case FLOAT:
                float tmp = (float)(int)ret;
                printf("Wynik: %f\n", tmp / FLOAT_MULTIPLIER);
                break;
            case STRING:
                printf("Wynik: %s\n", (char*)ret);
                break;
            default:
                break;
        }    
    }
    // Przypadek 4 - Żadna funkcja nie spełnia potrzebnych warunków (nazwa i typ argumentu)
    else {
        // Wyświetlamy błąd
        printf("\033[91merr: Nie znaleziono funkcji \033[93m%s(%s)\033[0m\n", name, typeDictionary[argumentType]);
    }

    // W razie gdy floatArg był użyty, zwalniamy jego pamięć
    if (floatArg) free(floatArg);
    return 0;   // Zwracamy zawsze 0
}