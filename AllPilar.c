#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>

// --- DEFINISI DATA ---
typedef struct __attribute__((packed)) {
    int id;          // 1=Depth, 2=Gyro
    float value;
} SensorData;

// --- GLOBAL VARIABLES (RESOURCE) ---
int MotorThrust = 0;     // Variabel yang diperebutkan PID dan Telemetri
int PipaQueue[2];        // Pilar 3: Queue

// --- ALAT SINKRONISASI ---
pthread_mutex_t GembokMotor; // Pilar 2: Mutex
sem_t SinyalDarurat;         // Pilar 4: Semaphore

// --- PILAR 4: INTERRUPT HANDLER ---
void interrupt_handler(int sig) {
    // TUGAS 1: Beri sinyal ke semaphore (jangan print apa2!)
    sem_post(&SinyalDarurat);
}

// --- TASK 1: SENSOR (PRODUCER) ---
void* task_sensor(void* arg) {
    SensorData data = {1, 0.0};
    while(1) {
        data.value += 0.5; // Simulasi data berubah
        // TUGAS 2: Kirim struct data ke Queue (Pipa Masuk = index 1)
        write(PipaQueue[1], &data, sizeof(SensorData));
        usleep(200000); // 5 Hz
    }
}

// --- TASK 2: PID CONTROL (CONSUMER) ---
void* task_pid(void* arg) {
    SensorData paket;
    while(1) {
        // TUGAS 3: Baca Queue (Pipa Keluar = index 0)
        // Task ini akan TIDUR kalau gak ada data (Blocking Read)
        if (read((PipaQueue[0]), &paket, sizeof(SensorData)) > 0) {
            
            // Hitung PID ceritanya...
            int hasil_hitung = (int)(paket.value * 10); 

            // TUGAS 4: Amankan akses ke variabel global MotorThrust
            pthread_mutex_lock(&GembokMotor);
            MotorThrust = hasil_hitung; 
            pthread_mutex_unlock(&GembokMotor);
        }
    }
}

// --- TASK 3: TELEMETRI (MONITOR) ---
void* task_telemetri(void* arg) {
    int baca_motor;
    while(1) {
        // TUGAS 5: Amankan akses saat membaca MotorThrust
        pthread_mutex_lock(&GembokMotor);
        baca_motor = MotorThrust;
        pthread_mutex_unlock(&GembokMotor);

        printf("[TELEMETRY] Motor Thrust saat ini: %d PWM\n", baca_motor);
        usleep(1000000); // 1 Hz
    }
}

// --- TASK 4: SAFETY GUARD ---
void* task_safety(void* arg) {
    while(1) {
        // TUGAS 6: Tunggu sinyal darurat (Blocking Wait)
        sem_wait(&SinyalDarurat);

        printf("\n[!!!] EMERGENCY STOP! RESET MOTOR KE 0!\n");
        
        // Reset motor dengan aman
        pthread_mutex_lock(&GembokMotor);
        MotorThrust = 0;
        pthread_mutex_unlock(&GembokMotor);

        printf("Sistem dimatikan.\n");
        exit(0); // Matikan program
    }
}

int main() {
    printf("=== ROV FINAL SYSTEM ONLINE ===\n");
    printf("Tekan Ctrl+C untuk Emergency Stop.\n\n");

    // TUGAS 7: Inisialisasi Queue, Mutex, Semaphore
    pipe(PipaQueue);
    pthread_mutex_init(&GembokMotor, NULL);
    sem_init(&SinyalDarurat, 0, 0);

    // Register Interrupt
    signal(SIGINT, interrupt_handler);

    // TUGAS 8: Buat 4 Thread (Sensor, PID, Telemetri, Safety)
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, task_sensor, NULL);
    pthread_create(&t2, NULL, task_pid, NULL);
    pthread_create(&t3, NULL, task_telemetri, NULL);
    pthread_create(&t4, NULL, task_safety, NULL);

    // Biarkan main thread tidur selamanya (Tugas didelegasikan ke Safety Task)
    while(1) {
        sleep(1);
    }
    return 0;
}