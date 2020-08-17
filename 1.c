#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <gpm.h>

#define MAN 6
#define TIME 5


unsigned short int cacke = 20;


pthread_t tid[MAN];
pthread_mutex_t hMutexBlockState, hMutexBlockKnife, hMouse;
const unsigned short int StopX=49, StopY=10;


void printChar(int x, int y, char* stringLine){
	printf("\033[%d;%dH%s\n", y, x, stringLine);
}


int goStep(int numberS, int numberF){
	return (numberF>=numberS)?1:-1;
}


void printCacke(){
	for(int i=0; i<10; i++){
		printChar(50+i, 10, "*");
		printChar(50+i, 21, "*");
	}
	for(int j=0; j<12; j++){
		printChar(50, 10+j, "*");
		printChar(59, 10+j, "*");
	}	
}


void printNumberCacke(){
	if(cacke<10){
		printf("\033[%d;%dH0%d\n", 15, 55, cacke);
	}
	else printf("\033[%d;%dH%d\n", 15, 55, cacke);
}


void goToTableOrBack(int x1, int y1, int x2, int y2, char* name){
	int abs = goStep(y1, y2);
	for(int y = y1; y!=y2; y+=abs){
		if(y!=y1){
			printChar(x1, y-abs, " ");
		}
		printChar(x1, y, name);
		usleep(40000);
	}
	if(y1!=y2){
		printChar(x1, y2-abs,  " ");
	}
	abs = goStep(x1, x2);
	for(int x = x1; x != x2; x+=abs){
		if(x!=x1){
			printChar(x-abs, y2, " ");
		}
		printChar(x, y2, name);
		usleep(40000);
	}
	if(x1!=x2){
		printChar(x2-abs, y2, " ");
	} 
}


void knife(int id_man){
	pthread_mutex_lock(&hMutexBlockKnife);
	goToTableOrBack(20+id_man, id_man+5, StopX, StopY+id_man+3, "M");
	printChar(StopX, StopY+id_man+3, "K");	
	cacke--;	
	usleep(random()%7*500000);
	goToTableOrBack(StopX, StopY+id_man+3, 20+id_man, id_man+5, "M");
	pthread_mutex_unlock(&hMutexBlockKnife);	
}


void eat(int id_man){
	printChar(20+id_man, id_man+5, "E");
	usleep(random()%7*800000);
}


void think(int id_man){
	printChar(20+id_man, id_man+5, "T");
	usleep(random()%7*1500000);
}


void man(void* arg){
	int id_man = (int)arg;
	usleep(100000*id_man);
	printNumberCacke();
	while(cacke>4){
		think(id_man);
		knife(id_man);
		printNumberCacke();
		eat(id_man);
	}
}


void mouse(){
	int x, y, button;
	struct Gpm_Event event;
	struct Gpm_Connect connect;
	connect.eventMask = GPM_DOWN;
	connect.defaultMask = GPM_MOVE;
	connect.minMod = 0;
	connect.maxMod = 0;
	if(Gpm_Open(&connect, 0)==-1){
		printf("Error Open Mouse");
		exit(1);
	}
	while(1){
		Gpm_GetEvent(&event);
		x = event.x;
		y = event.y;
		button = event.button;
		printf("\033[1;60H\033[1;33m")
		printf("row=%3d col=%3d key=%1d\n", x, y, button);	
	}
	Gpm_Close();
}


int main(){
	int rc;
	printf("\033[H\033[2J");
	printChar(1, 1, "M-move, K-knife, T-think, E=eat");
	printCacke();
	pthread_mutex_init(&hMutexBlockState, NULL);
	pthread_mutex_init(&hMutexBlockKnife, NULL);
	rc = pthread_create();
	for(int i = 0; i < MAN; i++){
		rc = pthread_create(&tid[i], NULL, (void*)man, (void*)i+1);
	}
	getchar();
	return 0;
}
