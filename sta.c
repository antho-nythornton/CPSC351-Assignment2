#include <pthread.h>   //Create POSIX threads.
#include <time.h>      //Wait for a random time.
#include <unistd.h>    //Thread calls sleep for specified number of seconds.
#include <semaphore.h> //To create semaphores
#include <stdlib.h>
#include <stdio.h>   //Input Output
pthread_t *Students; // N threads running as Students.
pthread_t TA;        // Separate Thread for TA.
int ChairsCount = 0;
int CurrentIndex = 0;
int Helped = 0;
int number_of_students; // a variable taken from the user to create student threads. Default is 5 student threads.
/*TODO
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
//An array of 3 semaphores to signal and wait chair to wait for the TA.
//A semaphore to signal and wait for TA's next student.
//Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
//hint: use sem_t and pthread_mutex_t
*/

pthread_mutex_t mutex;
sem_t ta_state;
sem_t chairs[3];
sem_t ready_next;

// Declared Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int main(int argc, char *argv[])
{
    int id;
    srand(time(NULL));

    /*TODO
    //Initializing Mutex Lock and Semaphores.
    //hint: use sem_init() and pthread_mutex_init()
    */

    sem_init(&ta_state, 0, 0);
    sem_init(&ready_next, 0, 0);
    for (int i = 0; i < 3; i++)
    {
        sem_init(&chairs[i], 0, 0);
    }
    pthread_mutex_init(&mutex, NULL);

    if (argc < 2)
    {
        printf("Number of Students not specified. Using default (5) students.\n");
        number_of_students = 5;
    }
    else
    {
        printf("Number of Students specified. Creating %d threads.\n",
               number_of_students);
        number_of_students = atoi(argv[1]);
    }
    // Allocate memory for Students
    Students = (pthread_t *)malloc(sizeof(pthread_t) * number_of_students);

    /*TODO
    //Creating one TA thread and N Student threads.
    //hint: use pthread_create
    //Waiting for TA thread and N Student threads.
    //hint: use pthread_join
    */

    pthread_create(&TA, NULL, TA_Activity, NULL);
    for (id = 0; id < number_of_students; id++)
    {
        pthread_create(&Students[id], NULL, Student_Activity, (void *)(long)id);
    }

    pthread_join(TA, NULL);
    for (id = 0; id < number_of_students; id++)
    {
        pthread_join(Students[id], NULL);
    }

    free(Students);

    // have to destroy the semaphores amd mutex.
    pthread_mutex_destroy(&mutex);
    sem_destroy(&ta_state);
    sem_destroy(&ready_next);
    for (int i = 0; i < 3; i++)
    {
        sem_destroy(&chairs[i]);
    }
    // print statement to finish it.
    printf("All students successfully helped by the TA.\n");
    return 0;
}

void *TA_Activity()
{
    /* TODO
    //TA is currently sleeping.
    // lock
    //if chairs are empty, break the loop.
    //TA gets next student on chair.
    //unlock
    //TA is currently helping the student
    //hint: use sem_wait(); sem_post(); pthread_mutex_lock();
    pthread_mutex_unlock()
    */
    while (1)
    {
        printf("TA is currently sleeping\n");
        sem_wait(&ta_state);
        while (1)
        {
            pthread_mutex_lock(&mutex);
            if (ChairsCount == 0)
            {
                pthread_mutex_unlock(&mutex);
                break;
            }
            printf("TA is currently helping a student\n");
            sem_post(&chairs[CurrentIndex]);
            --ChairsCount;
            CurrentIndex = (CurrentIndex + 1) % 3;

            pthread_mutex_unlock(&mutex);
            sleep(3);
            printf("TA finished helping a student.\n");
            sem_post(&ready_next);

            pthread_mutex_lock(&mutex);
            Helped++;
            if (Helped == number_of_students)
            {
                printf("All students have been helped. TA is done for today.\n");
                pthread_mutex_unlock(&mutex);
                return NULL; // Exit TA thread once all students are helped
            }
            pthread_mutex_unlock(&mutex);
        }
    }
}

void *Student_Activity(void *threadID)
{
    int id = (long)threadID;

    /*TODO
    //Student needs help from the TA
    printf("Student %d needs help from the TA.\n", id);
    //Student tried to sit on a chair.
    //wake up the TA.
    // lock
    // unlock
    //Student leaves his/her chair.
    //Student is getting help from the TA
    //Student waits to go next.
    //Student left TA room
    //If student didn't find any chair to sit on.
    //Student will return at another time
    //hint: use sem_wait(); sem_post(); pthread_mutex_lock();
    pthread_mutex_unlock()
    */

    while (1)
    {
        printf("Student %d needs help from the TA.\n", id);
        pthread_mutex_lock(&mutex);
        if (ChairsCount < 3)
        {
            // without this  chair_i index the program can't terminate.
            int chair_i = (CurrentIndex + ChairsCount) % 3;
            ChairsCount++;
            printf("Student %d is sitting on chair %d\n", id, chair_i);

            pthread_mutex_unlock(&mutex);

            sem_post(&ta_state);

            sem_wait(&chairs[chair_i]);

            printf("Student %d is getting help from the TA\n", id);
            sem_wait(&ready_next);
            printf("Student %d left TA room\n", id);

            break;
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            printf("Student %d will return later since there is no chair to sit on.\n", id);
            sleep(rand() % 5 + 1);
        }
    }
    return NULL;
}