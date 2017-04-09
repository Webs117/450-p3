#define _BSD_SOURCE
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

sem_t northSem;
sem_t southSem;
sem_t dirLock;
sem_t carsOnBridgeLock;

sem_t southOpen;
sem_t northOpen;

sem_t northWait;
sem_t southWait;

sem_t openBridge;

sem_t switchDir;

//which algorithm to run
int greedy = 0;

int direction = 1;

//number of cars of the bridge 
int carsOnBridge = 0;

//total
int waiting = 0;
int waitingNorth = 0;
int waitingSouth = 0;

double startTime;

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

	if(greedy == 1){
		//Run Greedy Crossing
        
        //*****************************************************************************************
        
        //north and south semaphores start off at 0 (set in main)
        //if cars == 0
            //give 3 semaphores to the semaphore that corresponds with the first car's direction
            
        //else if cars != 0
            //wait in the semaphore that corresonds to your direction
            
        //******************************************************************************************    
            
		sem_wait(&carsOnBridgeLock);
		if(carsOnBridge == 0){//if cars equal to zero
		    
		    //printf("In ArriveBridge (cars == 0), ID: %d, Dir: %d\n", copyCurrentCar->id, copyCurrentCar->dir);
		   
		    sem_wait(&dirLock);
			direction = copyCurrentCar->dir;//change to new direction
			sem_post(&dirLock);
		    if(copyCurrentCar->dir == 1){
		        sem_post(&northSem);
		        sem_post(&northSem);
		        sem_post(&northSem);
		        sem_wait(&northSem);
		    }
		    else if(copyCurrentCar->dir == 2){
		            sem_post(&southSem);
		            sem_post(&southSem);
		            sem_post(&southSem);
		            sem_wait(&southSem);
		        }
		    else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
		    carsOnBridge++;
		    sem_post(&carsOnBridgeLock);
		    
		    
		}
		else {//if cars not equal to zero
		    
			sem_post(&carsOnBridgeLock);
		    if(copyCurrentCar->dir == 1){
	            sem_wait(&northSem);
	        }
	        else if(copyCurrentCar->dir == 2){
	            sem_wait(&southSem);
	        }
	        else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
	        
		    sem_wait(&carsOnBridgeLock);
		    carsOnBridge++;
		    sem_post(&carsOnBridgeLock);

        }
	}
	else{
		//FCFS Crossing

		sem_wait(&carsOnBridgeLock);
		if(carsOnBridge == 0){//if cars equal to zero
		    printf("Arriving to Empty Bridge (cars == 0), ID: %d, Dir: %d\n", copyCurrentCar->id, copyCurrentCar->dir);
		    sem_wait(&dirLock);
			direction = copyCurrentCar->dir;//change to new direction
			sem_post(&dirLock);
		    if(copyCurrentCar->dir == 1){
		        sem_post(&northSem);
		        sem_post(&northSem);
		        sem_post(&northSem);
		        sem_wait(&northSem);
		    }
		    else if(copyCurrentCar->dir == 2){
		            sem_post(&southSem);
		            sem_post(&southSem);
		            sem_post(&southSem);
		            sem_wait(&southSem);
		        }
		    else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
		    carsOnBridge++;
		    sem_post(&carsOnBridgeLock);
		    
		    
		}
		else {//if cars not equal to zero
		    
			sem_post(&carsOnBridgeLock);

			//idea 
			// if 

			sem_wait(&dirLock);
			if(copyCurrentCar->dir != direction){
				//cars on bridge are going in different direction
				
				//release direction lock
				sem_post(&dirLock);


				//request switch direction since FCFS
				sem_wait(&switchDir);
				//increase total waiting cars. Not sure if we need total yet
				waiting++;

				if(copyCurrentCar->dir == 1){
					waitingNorth++;
	        	}
	        	else if(copyCurrentCar->dir == 2){
	        		waitingSouth++;
	        	}				
				sem_post(&switchDir);


	        	//wait until the bridge is open for us to go 
	        	printf("Time: %2.1f: Vehicle  %d  (%d) Bridge direction is opposite. Tell others to wait and wait until open \n", GetTime()-startTime, copyCurrentCar->id, copyCurrentCar->dir);
	        	

				//need to wait until direction is avaliable
				if(copyCurrentCar->dir == 1){
					//direction = 2 (south)
					//we want to tell south cars that we are waiting
	            	sem_wait(&northWait);
	        	}
	        	else if(copyCurrentCar->dir == 2){
	        		//direction = 1 (north)
					//we want to tell north cars that we are waiting
	            	sem_wait(&southWait);
	        	}




				//bridge should be empty now since cars in the same directon should 
				//			not have crossed because we were waiting 


				sem_wait(&dirLock);
    			direction = copyCurrentCar->dir;//change to new direction
    			sem_post(&dirLock);

    			if(copyCurrentCar->dir == 1){
    		        sem_post(&northSem);
    		        sem_post(&northSem);
    		        sem_post(&northSem);
    		        sem_wait(&northSem);
    		    }
    		    else if(copyCurrentCar->dir == 2){
    		            sem_post(&southSem);
    		            sem_post(&southSem);
    		            sem_post(&southSem);
    		            sem_wait(&southSem);
    		     }


			}else{
				//cars on bridge are going in same direction as us


				//idea:
				//check if another car is waiting on other side
				//if not go otherwise wait

				//release direction lock 
				sem_post(&dirLock);

				//check if car on opposite side is waiting
				sem_wait(&switchDir);
				if(waiting > 1){

					//we now have to wait until they go other side goes
					// add ourselves to the openBridge queue 
					printf("Time: %2.1f: Vehicle  %d  (%d) Same direction but car on otherside waiting \n", GetTime()-startTime, copyCurrentCar->id, copyCurrentCar->dir);
				
					//need to wait until direction is avaliable
					if(copyCurrentCar->dir == 1){
						//direction = 2 (south)
						//we want to tell south cars that we are waiting
		            	sem_wait(&northWait);
		        	}
		        	else if(copyCurrentCar->dir == 2){
		        		//direction = 1 (north)
						//we want to tell north cars that we are waiting
		            	sem_wait(&southWait);
		        	}



					if(copyCurrentCar->dir == 1){
		            	sem_wait(&northSem);
		        	}
		        	else if(copyCurrentCar->dir == 2){
		            	sem_wait(&southSem);
		        	}	


				}else{

					//no cars waiting on the other side so cross
					printf("Time: %2.1f: Vehicle  %d  (%d) Same direction and no cars waiting so lets cross \n", GetTime()-startTime, copyCurrentCar->id, copyCurrentCar->dir);
					


					
					if(copyCurrentCar->dir == 1){
		            	sem_wait(&northSem);
		        	}
		        	else if(copyCurrentCar->dir == 2){
		            	sem_wait(&southSem);
		        	}					
				}
				


			}

	        //else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
	        
		    sem_wait(&carsOnBridgeLock);
		    carsOnBridge++;
		    sem_post(&carsOnBridgeLock);

        }

		
	}


}
void* CrossBridge(void* carArgs){


	char pdir;

	vehicle_info *copyCurrentCar = (vehicle_info *) carArgs;

	if(copyCurrentCar->dir == 1){
		pdir = 'N';
	}else{
		pdir = 'S';
	}


	printf("Time: %2.1f: Vehicle  %d  (%c) crossing \n", GetTime()-startTime, copyCurrentCar->id, pdir);

	//takes 5 seconds to cross
	Spin(5);

	
}
void* ExitBridge(void* carArgs){

	vehicle_info *copyCurrentCar = (vehicle_info *) carArgs;

	//give back resource (spot on bridge)
	
	if(greedy == 1){
	
    	sem_wait(&dirLock);
        sem_wait(&carsOnBridgeLock);
    	
        if(copyCurrentCar->dir == 1){
            sem_post(&northSem);
        }
        else if(copyCurrentCar->dir == 2){
            sem_post(&southSem);
        }
        else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
        
        carsOnBridge--;
        sem_post(&carsOnBridgeLock);
        sem_post(&dirLock);
    	
    	sem_wait(&carsOnBridgeLock);
    		if(carsOnBridge == 0){//if cars equal to zero
    		    sem_wait(&dirLock);
    			direction = copyCurrentCar->dir;//change to new direction
    			sem_post(&dirLock);
    		    if(copyCurrentCar->dir == 2){
    		        sem_post(&northSem);
    		        sem_post(&northSem);
    		        sem_post(&northSem);
    		    }
    		    else if(copyCurrentCar->dir == 1){
    		            sem_post(&southSem);
    		            sem_post(&southSem);
    		            sem_post(&southSem);
    		      }
    		    else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
    		    //don't think we need this 
    		    //carsOnBridge++;
    		}
    	    sem_post(&carsOnBridgeLock);
	}
	
	
	else{

		//FCFS 


		sem_wait(&dirLock);
        sem_wait(&carsOnBridgeLock);
 	    sem_wait(&switchDir);
				
	    //wait until the bridge is open for us to go 
	    printf("Time: %2.1f: Vehicle  %d  (%d) in exit lock \n", GetTime()-startTime, copyCurrentCar->id, copyCurrentCar->dir);
	        	
		if(waiting != 0){
			waiting--;
		}
				  	
        if(copyCurrentCar->dir == 1){
        	if(waitingNorth !=0){
        		waitingNorth--;
        	}
            sem_post(&northSem);

        }
        else if(copyCurrentCar->dir == 2){
        	if(waitingSouth !=0){
        		waitingSouth--;
        	}
            sem_post(&southSem);
        }
        else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
        
        carsOnBridge--;
        sem_post(&switchDir); 
        sem_post(&carsOnBridgeLock);
        sem_post(&dirLock);
    	
    	sem_wait(&carsOnBridgeLock);
    		if(carsOnBridge == 0){//if cars equal to zero

    			//set direction of bridge to direction of last car
    		    sem_wait(&dirLock);
    			direction = copyCurrentCar->dir;
    			sem_post(&dirLock);


				sem_wait(&switchDir);
				if(waiting > 1){
					printf("I see some cars waiting\n");
					//need to swap directions to give waiting cars a chance
					if(copyCurrentCar->dir = 1){
						//current direction is north so switch direction to south
						for(int i = 0; i < waitingSouth;i++){
							//give as many south cars waiting slots to go
							printf("initialize south\n");
							sem_post(&southWait);
						}
					}else{
						for(int i = 0; i < waitingSouth;i++){
							printf("initialize south\n");
							sem_post(&southWait);
						}
					}
				}
				sem_post(&switchDir);


    			//bridge is now open 
    			//printf("Time: %2.1f: Vehicle  %d  (%d) Finished crossing and empty bridge. So lets open it up \n", GetTime()-startTime, copyCurrentCar->id, copyCurrentCar->dir);
    			//sem_post(&openBridge);

    			/*
    		    if(copyCurrentCar->dir == 2){
    		        sem_post(&northSem);
    		        sem_post(&northSem);
    		        sem_post(&northSem);
    		    }
    		    else if(copyCurrentCar->dir == 1){
    		            sem_post(&southSem);
    		            sem_post(&southSem);
    		            sem_post(&southSem);
    		     }
    		     */
    		    //else{printf("Incorrect direction, Direction: %d \n", copyCurrentCar->dir);}
    		    //don't think we need this 
    		    //carsOnBridge++;
    		}
    	    sem_post(&carsOnBridgeLock);

	    
	}
}
					//generic pointer argument type
void* VehicleAction(void* carArgs) {
	
									//define generic pointer argument type 
	vehicle_info *copyCurrentCar = (vehicle_info *) carArgs;

	//printf("In vehicleAction, ID: %d, Dir: %d\n", copyCurrentCar->id, copyCurrentCar->dir);
	ArriveBridge(copyCurrentCar);
	CrossBridge(copyCurrentCar);
	ExitBridge(copyCurrentCar);

}




int main(int argc, char *argv[]) {
 

	sem_init(&northSem, 1, 0);
	sem_init(&southSem, 1, 0);
	sem_init(&dirLock, 0, 1);
	sem_init(&carsOnBridgeLock, 0, 1);
	
	sem_init(&northWait, 1, 0);
	sem_init(&southWait, 1, 0);

	sem_init(&openBridge, 1, 0);


	sem_init(&switchDir, 1, 1);

	int numCars = 13;

	//array of structs holding car info
	vehicle_info cars[numCars];

	//each car has its own thread
	pthread_t threads[numCars];

	for (int i=0; i<numCars; i++){
        cars[i].id = i;
        cars[i].dir = 0;
        cars[i].inter_arrival_t = 1.0;
    }

    //set car directions 
    //1 - North
	//2 - South 
    cars[0].dir = 1;
    cars[0].inter_arrival_t = 0.0;
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

    
    startTime = GetTime();

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