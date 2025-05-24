#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>



int philosophersNum;
int sleepTime;
bool printForkLogsFLAG;
int maxIterations;
bool infiniteIterationsFLAG;

pthread_mutex_t* forks;
pthread_t* threadIDs;


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


    for (int iter = 0; iter < maxIterations || infiniteIterationsFLAG ; iter++)
    {
        //Think
        printf("Philosopher %d  is thinking       for the %d. time.\n", philosopherID, iter);
        usleep(sleepTime);

        //Try to get forks
        printf("Philosopher %d  is trying to eat  for the %d. time.\n", philosopherID, iter);
        pthread_mutex_lock(&forks[firstFork]);
        if (printForkLogsFLAG) printf("\t Fork %d picked up by Ph.%d.\n", firstFork, philosopherID);
        pthread_mutex_lock(&forks[secondFork]);
        if (printForkLogsFLAG) printf("\t Fork %d picked up by Ph.%d.\n", secondFork, philosopherID);
        
        //Eat
        printf("Philosopher %d  is eating         for the %d. time.\n", philosopherID, iter);
        usleep(2*sleepTime);
        timesEaten++;

        //Give forks back
        if (printForkLogsFLAG) printf("\t Fork %d put down by Ph.%d.\n", firstFork, philosopherID);
        pthread_mutex_unlock(&forks[firstFork]);
        if (printForkLogsFLAG) printf("\t Fork %d put down by Ph.%d.\n", secondFork, philosopherID);
        pthread_mutex_unlock(&forks[secondFork]);
        printf("Philosopher %d  finished eating   for the %d. time.\n", philosopherID, iter);
    }
    
    
    printf("\nEnding thread %llu - philosopher %d has eaten %d times. \t ===== \n", pthread_self(), philosopherID, timesEaten);
    
    int* eatenPtr = new int(timesEaten);
    pthread_exit((void *) eatenPtr);
}


int main(int argc, char* argv[])
{
    sleepTime = 1000;
    infiniteIterationsFLAG = false;
    int rt;

    if (argc < 5){
        printf(
            "\nERROR: Not enough program arguments.\n"
            "# Please provide arguments as follows : /PROGRAM_NAME/ " 
            "<Number of philosophers (as int)> <Number of milisec. that philosopher will think (as int)> <Number of iterations per philosopher (as int, '-1' if infinite)> <Flag for writing fork logs (as 0 or 1)>\n");
        exit(1);
    }

    philosophersNum = atoi(argv[1]);
    sleepTime *= atoi(argv[2]);
    maxIterations = atoi(argv[3]);
    printForkLogsFLAG = (bool)atoi(argv[4]);
    if (maxIterations == -1) infiniteIterationsFLAG = true;
    

    if (philosophersNum < 1){
        printf("\n ERROR : Provided invalid number of philosophers : %s \n", argv[1]);
        exit(2);
    }
    else if (philosophersNum < 2){
        printf("\n ERROR : Number of philosophers must be greater than 1\n");
        exit(2);
    }
    if (sleepTime < 1000){
        printf("\n ERROR : Provided invalid number of miliseconds : %s \n", argv[2]);
        exit(3);
    }
    if (maxIterations == 0 || maxIterations < -1 ){
        printf("\n ERROR : Provided invalid number of iterations : %s \n", argv[3]);
        exit(4);
    }
    



    
    
    forks = new pthread_mutex_t[philosophersNum];
    threadIDs = new pthread_t[philosophersNum];

    for (int i = 0; i < philosophersNum; i++){
        pthread_mutex_init(&forks[i], NULL);
    }
    
    for (int i = 0; i < philosophersNum; i++){
        int* philosopherID = new int(i);
        rt = pthread_create(&threadIDs[i], NULL, philosopherThread, (void*)philosopherID);
        if (rt){
            printf("\n ERROR: Return code from pthread_create : %d - For iteration : %d \n", rt, i);
            exit(10);
        }
        
    }

    

    void* thStatus;
    for (int i = 0; i < philosophersNum; i++)
    {
        rt = pthread_join(threadIDs[i], &thStatus);
        if (rt){
            printf("\n ERROR: Return code from pthread_join : %d \n", rt);
            exit(11);
        }
        printf("Thread %llu joined with status : %d \n", threadIDs[i], *(int*)thStatus);
    }

    

    printf("\nEnding main thread.\n");
    for (int i = 0; i < philosophersNum; i++)
    {
        pthread_mutex_destroy(&forks[i]);
    }
    delete[] forks;
    delete[] threadIDs;
    pthread_exit(NULL);
}