#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>


#define MAN 6//количество людей(потоков)


unsigned short int cacke = 20;//количество кусков пирога


pthread_t tid[MAN];//массив нитей
pthread_mutex_t hMutexBlockKnife;//мьютекс блокировки ножа
const unsigned short int StopX=49, StopY=10;//координаты передвижения


void printChar(int x, int y, char* stringLine){//функция для вывода символа
	printf("\033[%d;%dH%s\n", y, x, stringLine);//в определенных координатах
}


int goStep(int numberS, int numberF){//функция для того чтобы понять в каком направлении двигаться, если в обратном то вернет -1 если нет то 1
	return (numberF>=numberS)?1:-1;
}


void printCacke(){//функция для пирога на экран
	for(int i=0; i<10; i++){
		printChar(50+i, 10, "*");
		printChar(50+i, 21, "*");
	}
	for(int j=0; j<12; j++){
		printChar(50, 10+j, "*");
		printChar(59, 10+j, "*");
	}	
}


void printNumberCacke(){//функция для вывода количества кусков пирога
	if(cacke<10){
		printf("\033[%d;%dH0%d\n", 15, 55, cacke);
	}
	else printf("\033[%d;%dH%d\n", 15, 55, cacke);
}


void goToTableOrBack(int x1, int y1, int x2, int y2, char* name){//функция передвижения символов сначала определям в каком направлении двигаемся обратном или нет
//затем сначала перемещаемся по у координате потом по х
	int abs = goStep(y1, y2);
	for(int y = y1; y!=y2; y+=abs){
		if(y!=y1){
			printChar(x1, y-abs, " ");//стираем символ чтобы не оставалась последовательность
		}
		printChar(x1, y, name);//вывод символа в новой позиции
		usleep(40000);//сон
	}
	if(y1!=y2){
		printChar(x1, y2-abs,  " ");
	}
	abs = goStep(x1, x2);
	//тут все то же самое только для х
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
//поведение ножа
//сначала блокируется мьютекс, чтобы только один поток мог с ним взаимодействовать
//затем условный человек перемещается к пирогу
//затем заменяется его символ на К

	pthread_mutex_lock(&hMutexBlockKnife);
	goToTableOrBack(20+id_man, id_man+5, StopX, StopY+id_man+3, "M");
	printChar(StopX, StopY+id_man+3, "K");	
	cacke--;//уменьшаем кол-во кусков на 1	
	usleep(random()%7*500000);//типа отрезает кусок пирога
	goToTableOrBack(StopX, StopY+id_man+3, 20+id_man, id_man+5, "M");//идет обратно
	pthread_mutex_unlock(&hMutexBlockKnife);// анлочим мьютекс чтобы следующий поток мог отрезать кусок	
}


void eat(int id_man){
//модель поедания пирога 
	printChar(20+id_man, id_man+5, "E");
	usleep(random()%7*800000);
}


void think(int id_man){
//модель размышлений
	printChar(20+id_man, id_man+5, "T");
	usleep(random()%7*1500000);
}


void man(void* arg){
//эта функция запускается в каждом потоке 
//грубо говоря это поведение каждого потока
	int id_man = (int)arg;//преобразуем указатель на объект в число
	usleep(100000*id_man);//небольшое прервывание 
	printNumberCacke();//выводим кол-во пирога осталось
	while(cacke>4){//работает только так можно назвать это костылем но если сравнивать с 0 то будет 24 куска хз как это работает
		think(id_man);//функции поведения ничего интересного
		knife(id_man);
		printNumberCacke();//выводим новое кол-во пирога
		eat(id_man);
	}
}


int main(){
	int rc;
	printf("\033[H\033[2J");//чистим экран
	printChar(1, 1, "M-move, K-knife, T-think, E=eat");
	printCacke();//выводим пирог
	pthread_mutex_init(&hMutexBlockKnife, NULL);//инициализируем мьютекс чтобы дальше с ним взаимодейстовать
	for(int i = 0; i < MAN; i++){//запускаем потоки
	//первый аргумент это ссылка на массив потоков
	//второй хз
	//указатель на объект функции исполнения
	//аргумент который передается в качестве аргумента функции исполнения
		rc = pthread_create(&tid[i], NULL, (void*)man, (void*)i+1);
	}
	getchar();
	pthread_mutex_destroy(&hMutexBlockKnife);//отключаем мьютекс
	return 0;
}
