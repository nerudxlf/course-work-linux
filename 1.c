#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAN 6
#define TIME 5


unsigned short int cacke = 20;


pthread_t tid;


void knife(int id_man){
	printf("work_knife\n");
	cacke--;
}

void eat(int id_man){
	printf("work_eat\n");

}

void think(int in_man){
	printf("work_think\n");
}


void man(void* arg){
	int id_man = (int)arg;
	while(cacke>0){
		knife(id_man);
		eat(id_man);
		think(id_man);
	}
	printf("Hello\n");
}


int main(){
	int rc;
	for(int i = 0; i < MAN; i++){
		rc = pthread_create(&tid, NULL, (void*)man, (void*)i);
	}
}
