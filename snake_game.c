#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <time.h>
#include "ui.h"

int max_y = 0;
int max_x = 0;
int sn_size = 2;
int end_flag = 0;
int dym_size = 4;

int key_x = -1;
int key_y = 0;

pthread_mutex_t mutex;
pthread_t snake_pth;
pthread_t food_pth;
int food_y;
int food_x;
int food_count = 0;

struct Snake_t{
	int x;
	int y;
	const char *elem;
}*snake;

int main(int argc, char *argv[]) {

 initscr();
 noecho();
 curs_set(FALSE);
 getmaxyx(stdscr, max_y, max_x);
 
 show_boarders();
 pthread_mutex_init(&mutex, NULL);
 init();

	while(!end_flag) {
		int ch = get_input();
		switch(ch) {
			case 'q':
				game_end(food_count);
				break;
			// left arrow
			case 'a':
				pthread_mutex_lock(&mutex);
				key_x = -1;
				key_y = 0;
				pthread_mutex_unlock(&mutex);
				break;
			// up arrow
			case 'w':
				pthread_mutex_lock(&mutex);
				key_x = 0;
				key_y = -1;
				pthread_mutex_unlock(&mutex);
				break;	
			// right arrow
			case 'd':
				pthread_mutex_lock(&mutex);
				key_x = 1;
				key_y = 0;
				pthread_mutex_unlock(&mutex);
				break;
			// down arrow
			case 's':
				pthread_mutex_lock(&mutex);
				key_x = 0;
				key_y = 1;
				pthread_mutex_unlock(&mutex);
				break;
		}
	}
	game_end(food_count);
	pthread_mutex_destroy(&mutex);

 endwin();
}

void* generate_food(){

	sleep(2);
	srand(time(NULL));
	
	while(!end_flag){

		do{	
		   food_x = (rand() % ((max_x - 1) - 1) + 1);
		   food_y = (rand() % ((max_y - 1) - 1) + 1);
		}while(eat_myself(food_x, food_y));
		
		mvprintw(food_y, food_x, "f");	
		refresh();
		sleep(4); 
		mvprintw(food_y, food_x, " ");
	}

	return NULL;
} 

void* update(){
	int last_x, last_y;
	int temp_x, temp_y;
	while(!end_flag){
		pthread_mutex_lock(&mutex);
	 	for (int i = 0; i < sn_size; ++i)
	 	{	

	 		if(i == 0){
	 			last_x = snake[i].x;
	 	    	last_y = snake[i].y;
	 			mvprintw(snake[i].y, snake[i].x, " ");
	 			refresh();
	 			snake[i].x += key_x;
	 			snake[i].y += key_y;
	 		}else{
	 			temp_x = snake[i].x;
	 			temp_y = snake[i].y;
	 		   
	 			snake[i].x = last_x;
	 			snake[i].y = last_y;

	 			last_x = temp_x;
	 			last_y = temp_y;
	 		}

	 		mvprintw(snake[i].y, snake[i].x, snake[i].elem);
	 		mvprintw(last_y, last_x, " ");	
			refresh();
			
			if(snake[i].x <= 0 || snake[i].x >= (max_x - 1) || snake[i].y <= 0 || snake[i].y >= (max_y - 1) || eat_myself(snake[0].x, snake[0].y)){
				end_flag = 1;
			}
	 	}
	 	if(food_x == snake[0].x && food_y == snake[0].y){
				food_count++;
				add_snake_body();
		}
		pthread_mutex_unlock(&mutex);		
	 	usleep(70000);
	 }
	 return NULL;
}

void init(){
 snake =  malloc(dym_size * sizeof(struct Snake_t));
 for (int i = 0; i < sn_size; ++i){

 	if(i == 0 )snake[i].elem = "#";
 	else snake[i].elem = "o";
 	mvprintw(snake[i].y = max_y/2,	snake[i].x = max_x/2 + i, snake[i].elem);	
	refresh();
 }

	pthread_create(&snake_pth, NULL, update, NULL); 
	pthread_create(&food_pth, NULL, generate_food, NULL); 
}

int eat_myself(int x, int y){
	for (int i = 1; i < sn_size; ++i)
	{
		if(snake[i].x == x && snake[i].y == y){
			return 1;
		}	
	}
	return 0;
}

void add_snake_body(){
	 sn_size++;
	 if(sn_size >= dym_size){
	 	dym_size *= 2;
	 	snake =  realloc(snake ,dym_size * sizeof(struct Snake_t));
	 }
	 snake[sn_size - 1].x = snake[sn_size - 2].x + 1;
	 snake[sn_size - 1].y = snake[sn_size - 2].y;
	 snake[sn_size - 1].elem = "o";
} 

void show_boarders(){
		// top border
		for(int x = 0; x < max_x; x++) {
			mvprintw(0, x, "-");	
			refresh();
		}
		//	left and rigth borders
		for(int y = 1; y < max_y; y++){
				    mvprintw(y, 0, "|");
				    mvprintw(y, max_x - 1, "|");
				    refresh();
		}
		//bottom		
		for(int x = 0; x < max_x; x++) {
			mvprintw(max_y - 1, x, "-");	
			refresh();
		}
}

int	get_input(){
	return getch();
}

void game_end(int zombies) {
	pthread_mutex_lock(&mutex);
	endwin();
	pthread_mutex_unlock(&mutex);
	
	pthread_mutex_destroy(&mutex);
	printf("Score: %d\n", food_count);
	exit(0);
}