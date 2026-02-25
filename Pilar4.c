#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

sem_t sinyal;

// --- INI SIMULASI INTERRUPT HARDWARE ---
// Di STM32 ini adalah fungsi: void EXTI0_IRQHandler(void)
void simulasi_emergency(int sig){
    sem_post(&sinyal);
}

void* task_safety_manager(void* arg){
    while(1){
        sem_wait(&sinyal);
        printf("\n[!!!] EMERGENCY DETECTED! Mematikan semua Thruster ROV!\n");
    }
}

int main() {
    printf("=== ROV SAFETY SYSTEM READY ===\n");
    printf("Tekan Ctrl+C di terminal buat simulasiin tombol Emergency!\n\n");

    sem_init(&sinyal, 0, 0);
    signal(SIGINT, simulasi_emergency);

    pthread_t t1;
    pthread_create(&t1, NULL, &task_safety_manager, NULL);

    while(1){
        printf("ROBOT NGERJAIN TUGAS \n");
        usleep(100000);
    }
    return 0;
}