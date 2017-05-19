#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define delay 800000 // 0.8sec


struct coordinates_t{
		int x;
		int y;
};

struct snake_t{
		char *snake_body;
		int size;
		int current;
		struct coordinates_t crd;		
};

void gotoxy(int, int);
void show_borders(void);
struct snake_t start_position(void); 
void snake_destroy(struct snake_t);
void add_item(struct snake_t*, int);
void go_up(struct snake_t*);

int main(int argc, const char *argv[]){
		system("clear");
		struct snake_t snake;
		show_borders();
		snake = start_position();	
		add_item(&snake, 4);
		int head = snake.crd.x;

		//starting state of movement
		for(;head != 1; snake.crd.x = --head){ 
				gotoxy(snake.crd.x, snake.crd.y);
				for(int i = 0; i < snake.current; i++){			
				    gotoxy(snake.crd.x++, snake.crd.y);
					printf("%c", snake.snake_body[i]);
				}
				    gotoxy(snake.crd.x, snake.crd.y);
					printf(" ");
				      char c = getchar();	
				      fflush(stdout);
				      usleep(delay);
							
		}

		system("clear");
		return 0;
}

void go_up(struct snake_t *snake){
	int current_y = snake->crd.y;
	int current_x = snake->crd.x;
    	for(;current_y !=  0;){
						for(int i = 0; i < snake->current; i++){
								if(i == 0){
										gotoxy(current_x, --current_y);
										printf("%c", snake->snake_body[i]);
								}

								//im not ready yet
						}		
    	}
}

void add_item(struct snake_t *snake, int n){
		for(int i = 0; i < n + 1; i++){
				if(snake->current == snake->size){
						snake->size *= 2;
						snake->snake_body = realloc(snake->snake_body, snake->size * sizeof(char));
				}
			   
				if(snake->current == 0){
						snake->snake_body[snake->current++] = '>';
				}else {
						snake->snake_body[snake->current++] = 'o';
			}
		}
}

void snake_destroy(struct snake_t snake){
   free(snake.snake_body);

}

struct snake_t start_position(){
		struct snake_t snake;
		snake.current = 0;
		snake.size = 1;
		snake.snake_body = malloc(snake.size * sizeof(char));
		
		snake.crd.x = 25;
		snake.crd.y = 12;

		return snake;
}

void show_borders(){

		int width = 50;
		int heigth = 25;
		char borders[heigth][width];
        
		//top 
		for(int i = 0; i < width; i++){
				    borders[0][i] = '-';
					printf("%c", borders[0][i]);
		}
		printf("\n");
		//left and rigth borders
		for(int i = 1; i < heigth; i++){
				    borders[i][0] = '|';
					borders[i][width] = '|';

				    printf("%c", borders[i][0]);
					 //space 
					 for(int k = 0; k < width - 2; k++){
					 		printf(" ");	
					 }
					printf("%c", borders[i][width]);
					printf("\n");
		}
		//bottom		
		for(int i = 0; i < width; i++){
				    borders[heigth][i] = '-';
					printf("%c", borders[heigth][i]);
		}
}

void gotoxy(int x,int y)
{
		    printf("%c[%d;%df",0x1B,y,x);
}

