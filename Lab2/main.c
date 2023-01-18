#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


int buffer = 0;
pthread_mutex_t g_mutex;


void* thread_function();


int main()
{
    pthread_mutex_init(&g_mutex, NULL);

    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;

    pthread_create(&thread_1, NULL, &thread_function, NULL);
    pthread_create(&thread_2, NULL, &thread_function, NULL);
    pthread_create(&thread_3, NULL, &thread_function, NULL);

    int* thread_1_result;
    int* thread_2_result;
    int* thread_3_result;

    pthread_join(thread_1, (void**) &thread_1_result);
    pthread_join(thread_2, (void**) &thread_2_result);
    pthread_join(thread_3, (void**) &thread_3_result);

    printf("TID %ld worked on the buffer %d times\n", (long) thread_1, *thread_1_result);
    printf("TID %ld worked on the buffer %d times\n", (long) thread_2, *thread_2_result);
    printf("TID %ld worked on the buffer %d times\n", (long) thread_3, *thread_3_result);
    printf("Total buffer accesses: %d\n", buffer);

    pthread_mutex_destroy(&g_mutex);

    return 0;
}


void* thread_function()
{
    int* number_of_times_buffer_was_modified = (int*) malloc(sizeof(int));
    *number_of_times_buffer_was_modified = 0;

    pthread_t tid = pthread_self();
    pid_t pid = getpid();

    while(true)
    {
        /*If we dont have the sleep() statement on Ubuntu we might get that each
        thread modified the buffer 15, 0, 0 times respectively. This is not incorrect,
        just that the first thread that is started manages to run 15 times before
        the other threads even gets a chance. This doesn't happen on Mac but it does
        happen on Ubuntu. To ensure that each thread gets a chance to modify the buffer
        you can change the if statement below to if(buffer >= 1500) and remove the sleep
        statement and you will see that everything works correctly even without
        the sleep() statement.*/
        sleep(1);
        pthread_mutex_lock(&g_mutex);
        if(buffer >= 15)
        {
            pthread_mutex_unlock(&g_mutex);
            break;
        }
        else
        {
            printf("TID: %ld, PID: %d, Buffer: %d\n", (long) tid, pid, buffer);
            buffer++;
            (*number_of_times_buffer_was_modified)++;
            pthread_mutex_unlock(&g_mutex);
        }
    }
    
    return (void*) number_of_times_buffer_was_modified;
}