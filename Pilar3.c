#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct __attribute__((packed))
{
    int id;
    float nilai;
} Sensor;

int Queue_Ros[2];

void* task_sensor_hero(void* arg){
    Sensor data__depth = {1, 7.0};
    Sensor data_temp = {2, 31.5};

    while(1){
        write(Queue_Ros[1], &data__depth, sizeof(Sensor));;
        usleep(500000);


        write(Queue_Ros[1], &data_temp, sizeof(Sensor));;
        usleep(500000);
    }
}

void* task_pid_processor(void* arg){
    Sensor Datamasuk;

    while(1){
        if(read(Queue_Ros[0], &Datamasuk, sizeof(Sensor)) > 0){
            if(Datamasuk.id == 1){
                printf("[PID] Depth: %.2f m\n", Datamasuk.nilai);
            } else {
                printf("[PID] Temp: %.2f C\n", Datamasuk.nilai);
            }
        }
    }
}

int main() {
    if (pipe(Queue_Ros) == -1) return 1;
    pthread_t t1, t2;
    pthread_create(&t1, NULL, task_sensor_hero, NULL);;
    pthread_create(&t2, NULL, task_pid_processor, NULL);
    usleep(3000000);
    return 0;
}