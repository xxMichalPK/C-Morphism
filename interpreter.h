#ifndef MP_INTERPRETER_H
#define MP_INTERPRETER_H

#include <stdint.h>

// Jako, że nie wiem jak można zwracać float z funkcji, definiuję stałą, przez którą każdy float jest pomnożony, 
// a następnie podzielony po przeniesieniu go jako liczbę całkowitą
#define FLOAT_MULTIPLIER 1000000

// Definicja nowego typu danych, który może być wszystkim
typedef void* any;
// Do wskaźnika można wstawić wszystko (z ograniczeniem do architektury systemu - 32/64bit),
// więc definiując ten typ sprawiamy, że każda funkcja w tablicy będzie mogła być poprawnie wykonana
// Wszystko zależy tylko od implementacji "funkcji", która rozróżni te wszystkie typy i wykona

// 4 podstawowe typy danych powinny starczyć
enum DataType {
    VOID = 0,
    INT = 1,
    FLOAT = 2,
    STRING = 3
};

// Słownik typów danych do wyświetlenia dla użytkownika
static char *typeDictionary[] = { "void", "int", "float", "string" };

// Podstawowa struktura opisująca funkcję
typedef struct FunctionDescriptor {
    enum DataType returnType;       // Typ danych zwracanej
    char *name;                     // Nazwa funkcji
    enum DataType argumentType;     // Typ danych argumentu
    any (*function)(any);           // Wskaźnik funkcji do wywołania, przyjmujący i zwracający każdy typ danych
} FunctionDescriptor;

void displayAvailableFunctions(FunctionDescriptor *functions, uint32_t count);                  // Wyświetli wszystkie dostępne funkcje

enum DataType detectDataType(char *argument);                                                   // Zwraca typ danych z podanego argumentu

any executeFunction(char *name, char *argument, FunctionDescriptor *functions, uint32_t count); // Wykonuje funkcję o danej nazwie i z podanym argumentem

#endif