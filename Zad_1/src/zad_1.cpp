#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>



/*
*   > ph0  f0  ph1  f1  ph2  f2  ph3  f3  ph4  f4  ph5  f5 ... >
*   
*/




#define MAX_ITERATIONS 100
#define INFINITE_ITERATIONS false
#define SLEEP_TIME 100

int philosophersNum;
pthread_mutex_t* forks;
pthread_t* philosopherThreads;

void* philosopherThread(void* thdata){
    int timesEaten = 0;
    int philosopherID = *(int*)thdata;
    delete (int*)thdata;
    
    int  firstFork, secondFork;

    firstFork = philosopherID;
    secondFork = (philosopherID+1) % philosophersNum;
    //Hierarchical access to resources to avoid deadlock
    //  Fork with lower index will be picked first.
    if (firstFork > secondFork){
        int temp = secondFork;
        secondFork = firstFork;
        firstFork = temp;
    }
    
    
    printf("Hello. I'm Philosopher %d in a thread nr %llu. I can reach leftFork: %d and rightFork: %d \n", philosopherID, pthread_self(), firstFork, secondFork);


    for (int iter = 0; iter < MAX_ITERATIONS || INFINITE_ITERATIONS ; iter++)
    {
        //Think
        printf("Philosopher %d is thinking for %d. time.\n", philosopherID, iter);
        usleep(SLEEP_TIME);

        //Try to get forks
        printf("Philosopher %d is trying to eat for %d. time.\n", philosopherID, iter);
        pthread_mutex_lock(&forks[firstFork]);
        printf("Fork %d picked up by Ph.%d.\n", firstFork, philosopherID);
        pthread_mutex_lock(&forks[secondFork]);
        printf("Fork %d picked up by Ph.%d.\n", secondFork, philosopherID);
        
        //Eat
        printf("Philosopher %d is eating for %d. time.\n", philosopherID, iter);
        usleep(2*SLEEP_TIME);
        timesEaten++;

        //Give forks back
        pthread_mutex_unlock(&forks[firstFork]);
        printf("Fork %d put down by Ph.%d.\n", firstFork, philosopherID);
        pthread_mutex_unlock(&forks[secondFork]);
        printf("Fork %d put down by Ph.%d.\n", secondFork, philosopherID);
        printf("Philosopher %d finished eating. for %d. time\n", philosopherID, iter);
    }
    
    
    printf("\nEnding thread %llu - philosopher %d has eaten %d times.============================================================\n", pthread_self(), philosopherID, timesEaten);
    printf("\nEnding thread %llu - philosopher %d has eaten %d times.============================================================\n", pthread_self(), philosopherID, timesEaten);
    printf("\nEnding thread %llu - philosopher %d has eaten %d times.============================================================\n", pthread_self(), philosopherID, timesEaten);
    printf("\nEnding thread %llu - philosopher %d has eaten %d times.============================================================\n", pthread_self(), philosopherID, timesEaten);
    printf("\nEnding thread %llu - philosopher %d has eaten %d times.============================================================\n", pthread_self(), philosopherID, timesEaten);

    int* eatenPtr = new int(timesEaten);
    pthread_exit((void *) eatenPtr);
}


int main(int argc, char* argv[])
{
    int rt;
    pthread_t* threadID;
    if (argc < 2){
        printf("Please provide number of philosophers as a program argument.\n");
        exit(1);
    }

    philosophersNum = atoi(argv[1]);
    if (philosophersNum < 1){
        printf("Provided invalid argument : %s \n", argv[1]);
        exit(2);
    }
    else if (philosophersNum < 2){
        printf("Number of philosophers must be greater than 1\n");
        exit(3);
    }
    
    
    forks = new pthread_mutex_t[philosophersNum];
    threadID = new pthread_t[philosophersNum];

    for (int i = 0; i < philosophersNum; i++){
        pthread_mutex_init(&forks[i], NULL);
    }
    
    for (int i = 0; i < philosophersNum; i++){
        int* philosopherID = new int(i);
        rt = pthread_create(&threadID[i], NULL, philosopherThread, (void*)philosopherID);
        if (rt){
            printf("\n ERROR: return code from pthread_create : %d - For iteration : %d \n", rt, i);
            exit(4);
        }
        
    }

    

    void* thStatus;
    for (int i = 0; i < philosophersNum; i++)
    {
        pthread_join(threadID[i], &thStatus);
        printf("Thread %llu joined with status : %d \n", threadID[i], *(int*)thStatus);
    }

    

    printf("\nEnding main.\n");
    for (int i = 0; i < philosophersNum; i++)
    {
        pthread_mutex_destroy(&forks[i]);
    }
    delete[] forks;
    delete[] philosopherThreads;
    pthread_exit(NULL);//---
}