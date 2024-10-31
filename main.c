#include <stdio.h>
#include <string.h>
#include <math.h>
#include "interpreter.h"

// Odwołanie do funkcji exit z stdlib.h aby nie importować niepotrzebnie całej biblioteki
extern void exit(int __status);

// Największa możliwa długość nazwy funkcji i argumentu
#define MAX_LENGTH 128

const char COLOR_WHITE[] = "\033[97m";
const char COLOR_YELLOW[] = "\033[93m";
const char CLEAR_SCREEN[] = "\033[2J\033[1;1H";

// Nagłówek
const char HEADER_TEXT[] = 
"  /$$$$$$    /$$      /$$  /$$$$$$  /$$$$$$$  /$$$$$$$  /$$   /$$ /$$$$$$  /$$$$$$  /$$      /$$\n"
" /$$__  $$  | $$$    /$$$ /$$__  $$| $$__  $$| $$__  $$| $$  | $$|_  $$_/ /$$__  $$| $$$    /$$$\n"
"| $$  \\__/  | $$$$  /$$$$| $$  \\ $$| $$  \\ $$| $$  \\ $$| $$  | $$  | $$  | $$  \\__/| $$$$  /$$$$\n"
"| $$ /$$$$$$| $$ $$/$$ $$| $$  | $$| $$$$$$$/| $$$$$$$/| $$$$$$$$  | $$  |  $$$$$$ | $$ $$/$$ $$\n"
"| $$|______/| $$  $$$| $$| $$  | $$| $$__  $$| $$____/ | $$__  $$  | $$   \\____  $$| $$  $$$| $$\n"
"| $$    $$  | $$\\  $ | $$| $$  | $$| $$  \\ $$| $$      | $$  | $$  | $$   /$$  \\ $$| $$\\  $ | $$\n"
"|  $$$$$$/  | $$ \\/  | $$|  $$$$$$/| $$  | $$| $$      | $$  | $$ /$$$$$$|  $$$$$$/| $$ \\/  | $$\n"
" \\______/   |__/     |__/ \\______/ |__/  |__/|__/      |__/  |__/|______/ \\______/ |__/     |__/\n"
"\033[37m"
"                  Implementacja \"polimorfizmu\" czasu rzeczywistego w języku C\n"
"                                    Autor: Michal Pazurek\n\n";

// Funkcja, która narysuje trójkąt równoramienny
void rysujTrojkat(int dlugoscBoku) {
    for (int i = 0; i < dlugoscBoku; i++) {
        for (int j = 0; j <= i; j++) {
            printf("*");
        }
        printf("\n");
    }
}

int poleKwadratu(int bok) {
    return bok * bok;
}

// Dla funkcji z argumentami zmiennoprzecinkowymi musimy użyć wskaznik typu float zamiast zwykłego typu float
// Jest to związane z tym, że floata nie można przekonwertować na void*
float sqrt_m(float *arg) {
    return (int)(sqrt(*arg) * FLOAT_MULTIPLIER);    // Niestety zwracając floata musimy go przekonwertować do inta
}

// Tablica funkcji zawierająca typ funkcji, nazwę funkcji, typ argumentu i wskaźnik na funkcję
FunctionDescriptor functions[] = {
    { VOID, "puts", STRING, (any (*)(any))puts },
    { FLOAT, "sqrt", FLOAT, (any (*)(any))sqrt_m },
    { VOID, "rysujTrojkat", INT, (any (*)(any))rysujTrojkat },
    { INT, "poleKwadratu", INT, (any (*)(any))poleKwadratu },
    { VOID, "exit", INT, (any (*)(any))exit }
};

// Stała przechowująca ilość dostępnych funkcji
const int functionCount = sizeof(functions) / sizeof(FunctionDescriptor);

// Funkcja główna programu
int main() {
    // Czyscimy ekran
    printf("%s%s", CLEAR_SCREEN, COLOR_YELLOW);
    printf("%s%s", HEADER_TEXT, COLOR_WHITE);

    // Wypisujemy wszystkie możliwe do wykonania funkcje
    printf("Dostepne funkcje to:\n");
    displayAvailableFunctions(functions, functionCount);

    // Pobieramy nazwę funkcji oraz argument
    char nazwaFunkcji[MAX_LENGTH] = { 0 };
    char argument[MAX_LENGTH] = { 0 };
    printf("\nPodaj nazwe funkcji do wykonania: ");
    fgets(nazwaFunkcji, sizeof(nazwaFunkcji), stdin);   // fgets pozwala na spacje, scanf nie
    nazwaFunkcji[strlen(nazwaFunkcji) - 1] = '\0';  // fgets dodaje \n na koniec więc trzeba usunąć

    printf("Podaj argument: ");
    fgets(argument, sizeof(argument), stdin);
    argument[strlen(argument) - 1] = '\0';

    // Wykonujemy podaną przez użytkownika funkcję
    printf("\n\033[92mWykonuje %s.....\033[0m\n", nazwaFunkcji);
    executeFunction(nazwaFunkcji, argument, functions, functionCount);  // Wykonujemy podaną funkcję a interpreter zajmuje się resztą
    return 0;
};