#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_THREADS 2
#define NUM_SWITCHES 1000000

void* thread_func_with_yield(void* arg) {
    for (int i = 0; i < NUM_SWITCHES; i++) {
        sched_yield();  // Erzwingt einen Kontextwechsel
    }
    return NULL;
}

double measure_time_with_yield() {
    pthread_t threads[NUM_THREADS];
    struct timeval start, end;

    gettimeofday(&start, NULL);  // Startzeitpunkt

    // Erstelle Threads, die Kontextwechsel durchführen
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func_with_yield, NULL);
    }

    // Warte auf die Beendigung der Threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);  // Endzeitpunkt

    // Berechne die verstrichene Zeit in Nanosekunden
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long elapsed_ns = seconds * 1000000000L + microseconds * 1000L;

    // Ausgabe der Gesamtzeit in Nanosekunden
    printf("Elapsed time with context switches: %ld nanoseconds\n", elapsed_ns);
    
    // Berechne die durchschnittliche Zeit für einen Kontextwechsel
    double avg_context_switch_time_ns = (double)elapsed_ns / NUM_SWITCHES;
    printf("Average context switch time: %.6f nanoseconds\n", avg_context_switch_time_ns);

    return elapsed_ns;
}

int main() {
    measure_time_with_yield();
    return 0;
}
