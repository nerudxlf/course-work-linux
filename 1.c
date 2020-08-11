#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAN 6
#define TIME 5


unsigned short int cacke = 20;


pthread_t tid[MAN];
pthread_mutex_t hMutexBlockState, hMutexBlockKnife;
const unsigned short int StopX=49, StopY=10;



void printChar(int x, int y, char* stringLine){
	printf("\033[%d;%dH%s\n", y, x, stringLine);
}


int goStep(int numberS, int numberF){
	return (numberF>=numberS)?1:-1;
}


void printCacke(){
	for(int i=0; i<10; i++){
		printChar(50+i, 5, "*");
		printChar(50+i, 16, "*");
	}
	for(int j=0; j<12; j++){
		printChar(50, 5+j, "*");
		printChar(59, 5+j, "*");
	}
	
}


void goToTableOrBack(int x1, int y1, int x2, int y2, char* name){
	int abs = goStep(y1, y2);
	for(int y = y1; y!=y2; y+=abs){
		if(y!=y1){
			printChar(x1, y-abs, " ");
		}
		printChar(x1, y, name);
		usleep(100000);
	}
	if(y1!=y2){
		printChar(x1, y2-abs,  " ");
	}
	abs = goStep(x1, x2);
	for(int x = x1; x != x2; x+=abs){
		if(x!=x1){
			printChar(x-abs, y1, " ");
		}
		printChar(x, y1, name);
		usleep(100000);
	}
	if(x1!=x2){
		printChar(x2-abs, y1, " ");
	} 
}


void knife(int id_man){
	goToTableOrBack(20, id_man+5, StopX, StopY+id_man+3, "K");
	pthread_mutex_lock(&hMutexBlockKnife);
	cacke--;
	usleep(3000000);
	pthread_mutex_unlock(&hMutexBlockKnife);
	goToTableOrBack(StopX, StopY+id_man+3, 20, id_man+5, "K");	
}


void eat(int id_man){
	printChar(20, id_man+5, "E");
	usleep(3000000);
}


void think(int id_man){
	printChar(20, id_man+5, "T");
	usleep(3000000);
}



void man(void* arg){
	int id_man = (int)arg;
	//printf("%d", id_man);
	while(cacke>0){
		knife(id_man);
		eat(id_man);
		think(id_man);
	}
}


int main(){
	int rc;
	printf("\033[H\033[2J");
	printCacke();
	pthread_mutex_init(&hMutexBlockState, NULL);
	pthread_mutex_init(&hMutexBlockKnife, NULL);
	for(int i = 0; i < MAN; i++){
		rc = pthread_create(&tid[i], NULL, (void*)man, (void*)i+1);
	}
	getchar();
	return 0;
}
