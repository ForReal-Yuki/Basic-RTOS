#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// --- TASK 1: MOTOR THRUSTER (Target: 50 Hz) ---
void* task_thruster(void* arg) {
    int hitung = 1;
    // TUGAS 1: Bikin loop abadi buat task ini
    while(1) {
        printf("[MOTOR] Update kecepatan ESC... (Ke-%d)\n", hitung++);
        
        // TUGAS 2: Hitung usleep untuk 50 Hz (Berapa mikrodetik?)
        usleep( 20000 ); 
    }
    return NULL;
}

// --- TASK 2: SENSOR IMU (Target: 10 Hz) ---
void* task_imu(void* arg) {
    int hitung = 1;
    while(1) {
        printf("   [IMU] Baca sudut Roll/Pitch... (Ke-%d)\n", hitung++);
        
        // TUGAS 3: Hitung usleep untuk 10 Hz
        usleep( 100000 ); 
    }
    return NULL;
}

// --- TASK 3: TELEMETRI HEARTBEAT (Target: 1 Hz) ---
void* task_telemetri(void* arg) {
    int hitung = 1;
    while(1) {
        printf("      [TELEMETRI] Bip! Robot masih hidup. (Ke-%d)\n", hitung++);
        
        // TUGAS 4: Hitung usleep untuk 1 Hz
        usleep(1000000); 
    }
    return NULL;
}

// --- MANAGER (MAIN THREAD) ---
int main() {
    printf("=== ROV OS BOOTING UP ===\n\n");

    // Deklarasi 3 pekerja
    pthread_t thread_motor, thread_imu, thread_tele;

    // TUGAS 5: Rekrut / Create ketiga pekerja tersebut biar langsung jalan!
    pthread_create(&thread_motor, NULL, task_thruster, NULL);
    pthread_create(&thread_imu, NULL, task_imu, NULL);
    pthread_create(&thread_tele, NULL, task_telemetri, NULL);

    // Manager cuma ngawasin selama 2 detik aja, abis itu sistem dimatiin.
    // TUGAS 6: Suruh Manager (main thread) nunggu selama 2 detik.
    usleep( 2000000 ); 

    printf("\n=== WAKTU HABIS, SHUTTING DOWN ===\n");

    // Cancel paksa semua pekerja
    pthread_cancel(thread_motor);
    pthread_cancel(thread_imu);
    pthread_cancel(thread_tele);

    return 0;
}