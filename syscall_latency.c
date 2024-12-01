#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 1000000 // Anzahl der Iterationen für die Messung

/*
 * Hauptprogramm zur Messung der minimalen Latenzzeit eines System-Calls.
 * In diesem Fall wird der System-Call `getpid()` als Beispiel verwendet.
 */
int main() {
    struct timespec start, end; // Strukturen zur Speicherung von Start- und Endzeitpunkten
    long min_time_ns = 1000000000L; // Initialisierung mit einem hohen Wert (1 Sekunde in Nanosekunden)

    // Wiederhole die Messung über eine definierte Anzahl von Iterationen
    for (int i = 0; i < ITERATIONS; i++) {
        // Starte die Zeitmessung vor dem System-Call
        clock_gettime(CLOCK_MONOTONIC, &start); // CLOCK_MONOTONIC garantiert monotone Zeitmessung

        getpid(); // Ausführung des Beispiel-System-Calls

        // Beende die Zeitmessung nach dem System-Call
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Berechnung der verstrichenen Zeit in Nanosekunden
        long time_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

        // Aktualisierung der minimalen gemessenen Zeit, falls die aktuelle Zeit kleiner ist
        if (time_ns < min_time_ns) {
            min_time_ns = time_ns;
        }
    }

    // Ausgabe der minimalen gemessenen System-Call-Latenz
    printf("Minimale System-Call-Latenz: %ld ns\n", min_time_ns);

    return 0; // Erfolgreiche Ausführung des Programms
}
