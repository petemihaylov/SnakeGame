#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <time.h>

int max_y = 0;
int max_x = 0;
int sn_size = 4;
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

struct Snake_t {
    int x;
    int y;
    const char *elem;
} *snake;

void* update();
void* food_regeneration();
void init();
int eat_myself(int x, int y);
void add_snake_body();
void show_boarders();
int get_input();
void game_end(int score);
void generate_food();

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE); // Enable arrow key input
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Snake color
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Food color
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // Border color
    getmaxyx(stdscr, max_y, max_x);

    show_boarders();
    pthread_mutex_init(&mutex, NULL);
    init();

    while (!end_flag) {
        int ch = get_input();
        pthread_mutex_lock(&mutex);
        switch (ch) {
            case 'q':
                game_end(food_count);
                break;
            case KEY_LEFT:
                if (key_x != 1) { // Prevent reverse direction
                    key_x = -1;
                    key_y = 0;
                }
                break;
            case KEY_UP:
                if (key_y != 1) {
                    key_x = 0;
                    key_y = -1;
                }
                break;
            case KEY_RIGHT:
                if (key_x != -1) {
                    key_x = 1;
                    key_y = 0;
                }
                break;
            case KEY_DOWN:
                if (key_y != -1) {
                    key_x = 0;
                    key_y = 1;
                }
                break;
        }
        pthread_mutex_unlock(&mutex);
    }

    game_end(food_count);
    pthread_mutex_destroy(&mutex);
    endwin();
}

void generate_food() {
    // Randomly place the food on the screen, avoiding the snake's body
    do {
        food_x = rand() % (max_x - 2) + 1;
        food_y = rand() % (max_y - 2) + 1;
    } while (eat_myself(food_x, food_y));

    // Display the food at the new location
    attron(COLOR_PAIR(2));
    mvprintw(food_y, food_x, "F");
    attroff(COLOR_PAIR(2));
    refresh();
}

void* food_regeneration() {
    while (!end_flag) {
        sleep(7); // Wait for 7 seconds

        pthread_mutex_lock(&mutex);
        
        // Clear the current food position if it hasn't been eaten
        mvprintw(food_y, food_x, " ");
        refresh();

        // Generate new food position
        generate_food();

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* update() {
    int last_x, last_y;
    generate_food(); // Initial food generation

    while (!end_flag) {
        pthread_mutex_lock(&mutex);

        last_x = snake[sn_size - 1].x;
        last_y = snake[sn_size - 1].y;

        // Move the snake head
        for (int i = sn_size - 1; i > 0; i--) {
            snake[i].x = snake[i - 1].x;
            snake[i].y = snake[i - 1].y;
        }
        
        // Update head position with wrapping behavior
        snake[0].x += key_x;
        snake[0].y += key_y;

        if (snake[0].x <= 0) {
            snake[0].x = max_x - 2; // Wrap around to the right side
        } else if (snake[0].x >= max_x - 1) {
            snake[0].x = 1; // Wrap around to the left side
        }

        if (snake[0].y <= 0) {
            snake[0].y = max_y - 2; // Wrap around to the bottom
        } else if (snake[0].y >= max_y - 1) {
            snake[0].y = 1; // Wrap around to the top
        }

        // Clear last tail segment
        mvprintw(last_y, last_x, " ");

        // Display snake with color
        for (int i = 0; i < sn_size; i++) {
            attron(COLOR_PAIR(1));
            mvprintw(snake[i].y, snake[i].x, snake[i].elem);
            attroff(COLOR_PAIR(1));
        }
        refresh();

        // Check for self-collision
        if (eat_myself(snake[0].x, snake[0].y)) {
            end_flag = 1;
        }

        // Check if food is eaten
        if (food_x == snake[0].x && food_y == snake[0].y) {
            // Clear the food from the screen
            mvprintw(food_y, food_x, " ");
            refresh();

            food_count++;
            add_snake_body();

            // Generate a new food position
            generate_food();
        }
        pthread_mutex_unlock(&mutex);       
        usleep(70000);
    }
    return NULL;
}

void init() {
    srand(time(NULL));
    snake = malloc(dym_size * sizeof(struct Snake_t));
    for (int i = 0; i < sn_size; ++i) {
        snake[i].elem = (i == 0) ? "#" : "o";
        snake[i].x = max_x / 2 + i;
        snake[i].y = max_y / 2;
        attron(COLOR_PAIR(1));
        mvprintw(snake[i].y, snake[i].x, snake[i].elem);
        attroff(COLOR_PAIR(1));
        refresh();
    }
    pthread_create(&snake_pth, NULL, update, NULL);
    pthread_create(&food_pth, NULL, food_regeneration, NULL); // Start food regeneration thread
}

int eat_myself(int x, int y) {
    for (int i = 1; i < sn_size; ++i) {
        if (snake[i].x == x && snake[i].y == y) {
            return 1;
        }
    }
    return 0;
}

void add_snake_body() {
    sn_size++;
    if (sn_size >= dym_size) {
        dym_size *= 2;
        snake = realloc(snake, dym_size * sizeof(struct Snake_t));
    }
    snake[sn_size - 1].x = snake[sn_size - 2].x;
    snake[sn_size - 1].y = snake[sn_size - 2].y;
    snake[sn_size - 1].elem = "o";
}

void show_boarders() {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Snake color
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Food color
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // Border color

    attron(COLOR_PAIR(3));
    for (int x = 0; x < max_x; x++) {
        mvprintw(0, x, "-");             // Top border
        mvprintw(max_y - 1, x, "-");     // Bottom border
    }
    for (int y = 1; y < max_y - 1; y++) {
        mvprintw(y, 0, "|");             // Left border
        mvprintw(y, max_x - 1, "|");     // Right border
    }
    attroff(COLOR_PAIR(3));
    refresh();
}

int get_input() {
    return getch();
}

void game_end(int score) {
    pthread_mutex_lock(&mutex);
    endwin();
    pthread_mutex_unlock(&mutex);

    printf("Game Over! Score: %d\n", score);
    exit(0);
}
