#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t lock_reader;  // lock for reader
sem_t lock_writer;  // lock for writer

int reader_count = 0;

double t0, t;

double GetTime()
{
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

typedef struct _thread_info {
    int id;
    int rw;
    double inter_arrival_t;
} thread_info;


void* Reader(void* arg) {
    thread_info *ti = (thread_info *) arg;
    //printf("Time %2.1f: Reader %d arriving\n", GetTime()-t0, ti->id);
    
    sem_wait(&lock_reader);
    reader_count++;
    if (reader_count == 1)
        sem_wait(&lock_writer);
    sem_post(&lock_reader);
    
    printf("Time %2.1f: Reader %d reading\n", GetTime()-t0, ti->id);
    usleep(2000000);

    sem_wait(&lock_reader);
    reader_count--;
    if (reader_count == 0)
        sem_post(&lock_writer);
    sem_post(&lock_reader);
    //printf("Time %2.1f: Reader %d exiting\n", GetTime()-t0, ti->id);
}


void* Writer(void* arg) {
    thread_info *ti = (thread_info *) arg;
    //printf("Time %2.1f: Writer %d arriving\n", GetTime()-t0, ti->id);
    sem_wait(&lock_writer);
    printf("Time %2.1f: Writer %d writing\n", GetTime()-t0, ti->id);
    usleep(2000000);
    sem_post(&lock_writer);
    //printf("Time %2.1f: Writer %d exiting\n", GetTime()-t0, ti->id);
}


int main(int argc, char *argv[])
{
    int i;
    int NN = 8;
    thread_info tis[NN];
    pthread_t threads[NN];

    sem_init(&lock_reader, 0, 1);
    sem_init(&lock_writer, 0, 1);
    
    for (i=0; i<NN; i++){
        tis[i].id = i;
        tis[i].rw = 0;
        tis[i].inter_arrival_t = 0.1;
    }

    tis[2].rw = 1; tis[3].rw = 1; tis[4].rw = 1; tis[5].rw = 1; tis[7].rw = 1;
    // 0 0 1 1 1 1 0 1

    t0 = GetTime();
    for (i=0; i<NN; i++){
        usleep((int)(tis[i].inter_arrival_t*1000000));
        if (tis[i].rw == 0)
            pthread_create(&threads[i], NULL, Writer, (void*)&tis[i]);
        else
            pthread_create(&threads[i], NULL, Reader, (void*)&tis[i]);
    }

    
    for (i=0; i<NN; i++){
        pthread_join(threads[i], NULL);
    }

}
