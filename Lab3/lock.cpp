#include <stdio.h>
#include <pthread.h>

#define NUM_SNACK 1000000

bool g_flag[2];
int g_victim;
int g_sum = 0;

void InitLock() {
    g_flag[0] = false;
    g_flag[1] = false;
    g_victim = 0;
}

void Lock(int thread_id) {
    g_flag[thread_id] = true;
    __sync_synchronize();
    g_victim = thread_id;
    //__sync_synchronize();
    while (g_flag[1-thread_id] && g_victim == thread_id) {
        //empty
    }
}

void Unlock(int thread_id) {
    g_flag[thread_id] = false;
}

void *FeedMyPet(void* arg) {
    int i;
    int id = *((int *) arg);
    int pet_size = 0;
    for (i = 0; i < NUM_SNACK; i++) {
        Lock(id);
        //__sync_synchronize();
        pet_size++;
        g_sum++;
        //__sync_synchronize();
        Unlock(id);
    }

    printf("The size of my pet is %d!\n", pet_size);

    return NULL;
}

int main() {
    pthread_t alice, bob;
    int alice_id = 0;
    int bob_id = 1;

    InitLock();

    pthread_create(&alice, NULL, FeedMyPet, (void*) &alice_id);
    pthread_create(&bob, NULL, FeedMyPet, (void*) &bob_id);

    pthread_join(alice, NULL);
    pthread_join(bob, NULL);

    printf("Finished! %d\n", g_sum);
}
