# RTOS Concepts for Robotics Software Engineer
**Author:** forreal | **Platform:** Linux Simulation (pthread) & STM32 (FreeRTOS)

Dokumen ini merangkum 4 Pilar Utama Real-Time Operating System (RTOS) yang diterapkan pada sistem kendali Robot Bawah Air (ROV).

---

## 🏛️ Pilar 1: Task Management (Manajemen Pekerja)
**Konsep:** Memecah sistem `superloop` tunggal menjadi beberapa tugas (*Task*) independen yang berjalan seolah-olah paralel (Multitasking) dengan prioritas dan frekuensi berbeda.

| Fitur | Fungsi di Linux (Simulasi) | Fungsi di STM32 (FreeRTOS CMSIS v2) |
| :--- | :--- | :--- |
| **Membuat Task** | `pthread_create(...)` | `osThreadNew(...)` |
| **Delay (Yield)** | `usleep(mikrodetik)` | `osDelay(milidetik)` |

**Cara Inisiasi di STM32CubeIDE:**
1. Buka file `.ioc`.
2. Masuk ke **Middleware -> FREERTOS**.
3. Tab **Tasks & Queues** -> Klik **Add**.
4. Tentukan *Priority* (Realtime, High, Normal, Low).

**Poin Penting:**
* Gunakan `osDelay()` agar CPU bisa istirahat dan mengerjakan task lain (Context Switching).
* Jangan gunakan `HAL_Delay()` di dalam Task karena akan memblokir seluruh sistem (CPU Blocking).

---

## 🔒 Pilar 2: Resource Management (Mutex)
**Konsep:** Mencegah *Race Condition* (tabrakan data) ketika dua Task atau lebih mencoba mengakses sumber daya yang sama (variabel global, I2C, UART) secara bersamaan.

| Fitur | Fungsi di Linux (Simulasi) | Fungsi di STM32 (FreeRTOS CMSIS v2) |
| :--- | :--- | :--- |
| **Kunci (Lock)** | `pthread_mutex_lock(&gembok)` | `osMutexAcquire(gembok_id, timeout)` |
| **Buka (Unlock)** | `pthread_mutex_unlock(&gembok)` | `osMutexRelease(gembok_id)` |

**Cara Inisiasi di STM32CubeIDE:**
1. Masuk ke tab **Mutexes** di konfigurasi FreeRTOS.
2. Klik **Add** -> Beri nama (misal: `myI2CMutex`).

**Poin Penting:**
* Selalu pasang `Acquire` sebelum akses dan `Release` sesudah akses.
* Jika lupa `Release`, akan terjadi **Deadlock** (Sistem macet total menunggu kunci).

---

## 📨 Pilar 3: Message Queues (Komunikasi Antar Task)
**Konsep:** Mengirim data antar Task menggunakan metode "Pipa" (FIFO), sehingga tidak perlu menggunakan variabel global dan mengurangi risiko tabrakan data. Sangat disarankan untuk mengirim `struct`.

| Fitur | Fungsi di Linux (Simulasi) | Fungsi di STM32 (FreeRTOS CMSIS v2) |
| :--- | :--- | :--- |
| **Kirim Data** | `write(pipe[1], &data, size)` | `osMessageQueuePut(queue_id, &data, prio, timeout)` |
| **Terima Data** | `read(pipe[0], &data, size)` | `osMessageQueueGet(queue_id, &data, prio, timeout)` |

**Cara Inisiasi di STM32CubeIDE:**
1. Masuk ke tab **Tasks & Queues**.
2. Bagian **Queues** -> Klik **Add**.
3. Tentukan `Item Size` (misal: `sizeof(SensorData)`) dan `Queue Size` (jumlah antrian).

**Poin Penting:**
* Gunakan `__attribute__((packed))` pada struct untuk mencegah *padding* memori yang merusak data.
* Penerima (`Get`) akan otomatis "tidur" jika antrian kosong, menghemat CPU.

---

## ⚡ Pilar 4: Interrupt Handling (Deferred Processing)
**Konsep:** Menangani kejadian hardware mendadak (tombol, sensor data ready) dengan cepat. Interrupt hanya memberi sinyal, dan proses berat dilakukan oleh Task biasa.

| Fitur | Fungsi di Linux (Simulasi) | Fungsi di STM32 (FreeRTOS CMSIS v2) |
| :--- | :--- | :--- |
| **Trigger HW** | `signal(SIGINT, handler)` | `HAL_GPIO_EXTI_Callback(...)` |
| **Kirim Sinyal** | `sem_post(&semafor)` | `osSemaphoreRelease(semafor_id)` |
| **Tunggu Sinyal** | `sem_wait(&semafor)` | `osSemaphoreAcquire(semafor_id, timeout)` |

**Cara Inisiasi di STM32CubeIDE:**
1. Masuk ke tab **Timers and Semaphores**.
2. Bagian **Binary Semaphores** -> Klik **Add**.

**Poin Penting:**
* **HARAM** hukumnya melakukan `printf`, `delay`, atau perhitungan berat di dalam fungsi `Callback/Interrupt`.
* Gunakan Semaphore untuk membangunkan Task prioritas tinggi (Safety Task).

---
*Created during intensive training session with Gemini 3 Pro.*