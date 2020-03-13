 /*
 ########## Declaration ###########
Code: Vehicle Access Management in Tunnel using Pthread and Mutex
Create executable: $ gcc theTunnel.c -lpthread -fpermissive -o theTunnel
Run in terminal: $ ./theTunnel < sample_input.txt
Author: Shanto Roy (PSID: 1894941)
Copyright: 2019
Thanks: StackOverflow, GeeksforGeeks, Professor's explanation
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// static int carEq = 0;
static int maxLoad;
static int currentLoad = 0;
static pthread_mutex_t mylock;
static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;

// Vehicle enters the tunnel using this function
void *enterTunnel(int vehicleGroup)
{
    int carEq = 0;
    // We are not considering type 03 here
    if (vehicleGroup == 1)
    {
        carEq = 1;
    }
    else
    {
        carEq = 5;
    }

    pthread_mutex_lock(&mylock);
    while (currentLoad + carEq > maxLoad)
    {
        pthread_cond_wait(&ok, &mylock);
    }
    currentLoad += carEq;
    pthread_mutex_unlock(&mylock);
}

// Vehicle leaves the tunnel using this function
void *leaveTunnel(int vehicleGroup)
{
    int carEq;
    if (vehicleGroup == 1)
    {
        carEq = 1;
    }
    else
    {
        carEq = 5;
    }
    pthread_mutex_lock(&mylock);
    currentLoad -= carEq;
    pthread_cond_signal(&ok);
    pthread_mutex_unlock(&mylock);
}

struct VehicleDetails
{
    char license[8];
    int group;  // 1,2, or 3
    int arrival_time;
    int crossing_time;
};

void *vehicle(void *arg) 
{
    pthread_mutex_init(&mylock, NULL);
    struct VehicleDetails *newVehicle;
    newVehicle = (struct VehicleDetails *) arg;
    // sleep(newVehicle->arrival_time);
    printf("\nVehicle %s of type %d has arrived",newVehicle->license,newVehicle->group);
    
    // If car-equivalent is maximum than the max-load, it can't enter
    if(newVehicle->group == 2 && maxLoad < 5 )
    {
        printf("\nVehicle %s of type %d cannot enter",newVehicle->license,newVehicle->group);
    }
    else
    {
        if(newVehicle->group < 3)
        {
            enterTunnel(newVehicle->group);
        }
        printf("\nVehicle %s of type %d has entered",newVehicle->license,newVehicle->group);
        sleep(newVehicle->crossing_time);
        if(newVehicle->group < 3)
        {
            leaveTunnel(newVehicle->group);
        }
        printf("\nVehicle %s of type %d has left",newVehicle->license,newVehicle->group);
        pthread_exit((void *)0);
    }

}


int main()
{
    struct VehicleDetails thisVehicle[10];
    int num_of_vehicles = 0;
    char line[100];

    // printf("Enter the maximum vehicle load:\n");
    scanf("%d", &maxLoad);
    // printf("%d\n", maxLoad);

    pthread_mutex_init(&mylock, NULL);
    int i = 0;
    while (scanf("%s %d %d %d", thisVehicle[i].license, &thisVehicle[i].group, \
                            &thisVehicle[i].arrival_time, &thisVehicle[i].crossing_time)>0)
    {
        // printf("%s %d %d %d\n", thisVehicle[i].license, thisVehicle[i].group, \
                            thisVehicle[i].arrival_time, thisVehicle[i].crossing_time);
        i++;
    }

    num_of_vehicles = i;
    // printf("%d", num_of_vehicles);

    // printf("\n\n");
    
    pthread_t tid[10];

    // if (pthread_mutex_init(&mylock, NULL) != 0) 
    // { 
    //     printf("\n mutex init has failed\n"); 
    //     return 1; 
    // } 

    for(int i=0; i<num_of_vehicles; i++)
    {
        sleep(thisVehicle[i].arrival_time);
        pthread_create(&(tid[i]), NULL, vehicle, (void *) &thisVehicle[i]);
    }
    for(int i=0; i<num_of_vehicles; i++)
    {
        // sleep(thisVehicle[i].arrival_time);
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mylock); 
    printf("\n");
    return 0;
}
