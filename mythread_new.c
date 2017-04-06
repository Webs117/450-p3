#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

sem_t bridge;

//number of cars of the bridge 
int cars = 0;

void* VehicleAction(void* arg) {
	
	vehicle_info *ti = (vehicle_info *) arg;
	ArriveBridge(ti);
	CrossBridge(ti);
	ExitBridge(ti);

}

void* ArriveBridge(void* arg){



}
void* CrossBridge(void* arg){

	//Print out vehicle information
	
}
void* ExitBridge(void* arg){


	
}
typedef struct _vehicle_info {
	int id; // id
	int dir; // heading direction

	//1 - South
	//2 - North 

	double inter_arrival_t; // inter-arrival time between this vehicle and next
} vehicle_info;


typedef struct __myarg_t {
	int a;
	int b;
} myarg_t;

typedef struct __myret_t {
	int x;
	int y;
} myret_t;

void *mythread(void *arg) { 
	myarg_t *m = (myarg_t *) arg; 
	printf("%d %d\n", m->a, m->b); 
	myret_t *r = malloc(sizeof(myret_t)); 
	r->x = 1;
	r->y = 2;
	return (void *) r;
}


int main(int argc, char *argv[]) {
	int rc;

	pthread_t p;
	myarg_t args;
	myret_t *m;

	args.a = 10;
	args.b = 20;
	rc = pthread_create(&p, NULL, mythread, &args);
	pthread_join(p, (void **) &m);

	printf("returned %d %d\n", 
			m->x, m->y); 

	sem_init(&bridge, 1,3);

	return 0;
}

