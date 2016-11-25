#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int timeout;
bool writing;
int readAcquires;
int readReleases;

void writelock()
{
    writing = true;
    __sync_synchronize();
    while (readAcquires != readReleases) {
    }
}

void writeunlock()
{
    writing = false;
    __sync_synchronize();
}

void readlock()
{
    while (writing) {
    }
    __sync_fetch_and_add(&readAcquires, 1);
    
}

void readunlock()
{
    __sync_fetch_and_add(&readReleases, 1);
}

int g_counter[3];
int g_readCounter[3];
int g_writeCounter[3];

void* writer(void* arg)
{
    int i = *((int*) arg);
    int counter = 0; 


    while (!timeout) {
        writelock();

        counter++;
        //usleep(100);
        g_counter[i]++;
        g_writeCounter[i]++;

        usleep(100);
        writeunlock();
        usleep(100);
    }
    printf("writer %d wrote %d times!\n", i, counter);

	return NULL;
}

void* reader(void* arg)
{
    int i = *((int*) arg);
    int counter = 0; 

    while (!timeout) {
        readlock();
        
        if (i <= 2) 
        {
            g_readCounter[i]++;
            g_counter[i]++;
        }

        counter++;
        usleep(100);

        readunlock();
    }
    printf("reader %d read %d times!\n", i, counter);
	
	return NULL;
}

int main()
{
    //init
    int num_writer = 1;
    int num_reader = 10;
    printf("Test rwlock concurrency(%d writer, %d reader) started.\n", num_writer, num_reader);

    //test
    timeout = 0;
    pthread_t writer_thread[num_writer];
    int writer_index[num_writer];
    for (int i = 0; i < num_writer; i++) {
        pthread_create(&writer_thread[i], NULL, writer, (void*) &(writer_index[i]=i+1));
    }
    pthread_t reader_thread[num_reader];
    int reader_index[num_reader];
    for (int i = 0; i < num_reader; i++) {
        pthread_create(&reader_thread[i], NULL, reader, (void*) &(reader_index[i]=i+1));
    }

    usleep(5000000);
    timeout = 1;

    for (int i = 0; i < num_writer; i++) {
        pthread_join(writer_thread[i], NULL);
    }
    for (int i = 0; i < num_reader; i++) {
        pthread_join(reader_thread[i], NULL);
    }

    //assert
    printf("Test rwlock concurrency(%d writer, %d reader) passed.\n", num_writer, num_reader);
    
    printf("g_counter[1] : %d, g_readCounter[1] + g_writeCounter[1] : %d\n"
            , g_counter[1], g_readCounter[1] + g_writeCounter[1]);


    if (num_writer >= 2) {
        printf("g_counter[2] : %d, g_readCounter[2] + g_writeCounter[2] : %d\n"
                , g_counter[2], g_readCounter[2] + g_writeCounter[2]);
    }
}
