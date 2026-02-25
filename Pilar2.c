#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 1. TUGAS: Deklarasi variabel Gembok I2C
pthread_mutex_t gembok_i2c;

void* task_gyro(void* arg) {
    while(1) {
        // TUGAS 2: Ambil kunci sebelum pake kabel (Lock)
        pthread_mutex_lock(&gembok_i2c); 

        printf("[GYRO] >>> Sedang akses kabel I2C...\n");
        usleep(150000); // Simulasi proses baca sensor yang agak lama
        printf("[GYRO] <<< Selesai, kabel dilepas.\n");

        // TUGAS 3: Balikin kuncinya (Unlock)
        pthread_mutex_unlock(&gembok_i2c);

        usleep(50000); // Istirahat bentar
    }
}

void* task_barometer(void* arg) {
    while(1) {
        // TUGAS 4: Ambil kunci sebelum pake kabel (Lock)
        pthread_mutex_lock(&gembok_i2c);

        printf("   [BARO] >>> Sedang akses kabel I2C...\n");
        usleep(150000); 
        printf("   [BARO] <<< Selesai, kabel dilepas.\n");

        // TUGAS 5: Balikin kuncinya (Unlock)
        pthread_mutex_unlock(&gembok_i2c);

        usleep(50000);
    }
}

int main() {
    printf("=== ROV I2C RESOURCE MANAGEMENT ===\n\n");

    // TUGAS 6: Inisialisasi gembok (init)
    pthread_mutex_init(&gembok_i2c, NULL);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, task_gyro, NULL);
    pthread_create(&t2, NULL, task_barometer, NULL);

    usleep(2000000); // Simulasi robot jalan 2 detik

    pthread_cancel(t1);
    pthread_cancel(t2);

    // TUGAS 7: Hancurkan gembok setelah selesai (destroy)
    pthread_mutex_destroy(&gembok_i2c);

    printf("\nSistem Aman.\n");
    return 0;
}