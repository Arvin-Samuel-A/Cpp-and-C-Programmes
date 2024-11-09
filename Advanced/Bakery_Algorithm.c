#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NUM_THREADS 4

int balance = 1000;  // Initial balance
int entering[NUM_THREADS] = {0};   // To indicate if a thread is entering a number
int number[NUM_THREADS] = {0};     // Bakery numbers for each thread

void lock(int thread_id) {
    entering[thread_id] = 1;
    int max_number = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (number[i] > max_number) {
            max_number = number[i];
        }
    }
    number[thread_id] = max_number + 1;
    entering[thread_id] = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i == thread_id) continue;
        while (entering[i]) { /* Wait until thread i gets its number */ }
        while (number[i] != 0 && 
               (number[i] < number[thread_id] || 
               (number[i] == number[thread_id] && i < thread_id))) {
            /* Wait if thread i has a smaller number or same number but higher priority */
        }
    }
}

void unlock(int thread_id) {
    number[thread_id] = 0;
}

void* credit(void* arg) {
    int thread_id = *(int*)arg;
    for (int i = 0; i < 5; i++) { // perform 5 credit operations
        lock(thread_id);
        balance += 100;  // Credit 100 units
        printf("Thread %d credited 100. Balance: %d\n", thread_id, balance);
        unlock(thread_id);
        //sleep(1);
    }
    return NULL;
}

void* debit(void* arg) {
    int thread_id = *(int*)arg;
    for (int i = 0; i < 5; i++) { // perform 5 debit operations
        lock(thread_id);
        if (balance >= 100) {
            balance -= 100;  // Debit 100 units
            printf("Thread %d debited 100. Balance: %d\n", thread_id, balance);
        } else {
            printf("Thread %d attempted to debit but insufficient balance.\n", thread_id);
        }
        unlock(thread_id);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1, 2, 3};

    // Create 2 credit threads and 2 debit threads
    pthread_create(&threads[0], NULL, credit, &thread_ids[0]);
    pthread_create(&threads[1], NULL, credit, &thread_ids[1]);
    pthread_create(&threads[2], NULL, debit, &thread_ids[2]);
    pthread_create(&threads[3], NULL, debit, &thread_ids[3]);

    // Join all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final Balance: %d\n", balance);
    return 0;
}
