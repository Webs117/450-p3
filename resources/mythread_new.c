#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

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
	r->x = 5;
	r->y = 60;
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
	//pthread_join(p, (void **) &m);
	pthread_join(p, NULL);

	printf("returned %d %d\n", 
			m->x, m->y); 
	return 0;
}

