#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

sem_t bridge;

//number of cars of the bridge 
int cars = 0;



double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

void Spin(int howlong) {
    double t = GetTime();
    while ((GetTime() - t) < (double)howlong)
	; // do nothing in loop
}

typedef struct _vehicle_info {
	int id; // id
	int dir; // heading direction

	//1 - North
	//2 - South 

	double inter_arrival_t; // inter-arrival time between this vehicle and next
} vehicle_info;



void* ArriveBridge(void* carArgs){

vehicle_info *copyCurrentCar = (vehicle_info *) carArgs;




}
void* CrossBridge(void* arg){

	//Print out vehicle information
	vehicle_info *copyCurrentCar = (vehicle_info *) carArgs;

	//see if bridge is full
	sem_wait(&bridge);

	//takes 5 seconds to cross
	Spin(5);
	
}
void* ExitBridge(void* arg){



	
}
					//generic pointer 
void* VehicleAction(void* carArgs) {
	
									//define generic pointer 
	vehicle_info *copyCurrentCar = (vehicle_info *) carArgs;

	printf("In vehicleAction, ID: %d, Dir: %d\n", copyCurrentCar->id, copyCurrentCar->dir);
	ArriveBridge(copyCurrentCar);
	CrossBridge(copyCurrentCar);
	ExitBridge(copyCurrentCar);

}




int main(int argc, char *argv[]) {
 

	sem_init(&bridge, 1,3);


	int numCars = 13;

	//array of structs holding car info
	vehicle_info cars[numCars];

	//each car has its own thread
	pthread_t threads[numCars];

	for (int i=0; i<numCars; i++){
        cars[i].id = i;
        cars[i].dir = 0;
        cars[i].inter_arrival_t = 0.1;
    }

    //set car directions 
    cars[0].dir = 1;
    cars[1].dir = 1;
    cars[2].dir = 2;
    cars[3].dir = 2;
    cars[4].dir = 2;
    cars[5].dir = 1;
    cars[6].dir = 1;
    cars[7].dir = 2;
    cars[8].dir = 2;
    cars[9].dir = 1;
    cars[10].dir = 1;
    cars[11].dir = 1;
    cars[12].dir = 1;

    for (int k=0; k<numCars; k++){
        usleep((int)(cars[k].inter_arrival_t*1000000));
        pthread_create(&threads[k], NULL, VehicleAction, (void*)&cars[k]);
    }

   
	//wait for all the threads to finish
    for (int j=0; j<numCars; j++){
        pthread_join(threads[j], NULL);
    }



	return 0;
}

