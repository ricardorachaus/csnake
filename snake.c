#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>

// Game Types

typedef struct SnakePosition {
    int row;
    int column;
} SnakePosition;

typedef enum Input { LEFT, RIGHT, DOWN, UP, QUIT, INVALID } Input;

// Global variables
const int rows = 11;
const int columns = 11;
int isHandlingInput = 0;

/* Prototypes */

void game(void);
void updatePosition(void);

// Initialize game
void initGrid(char [][columns]);
void initSnake(char [][columns], SnakePosition *, int);

// Game logic
void update(char [][columns], SnakePosition *, int *);
int validatePosition(char [][columns], Input, SnakePosition *, int *);
void eatFood(char [][columns], SnakePosition *, int *, SnakePosition);
void move(SnakePosition *, int, SnakePosition);
void spawnFood(char [][columns]);
void updateGrid(char [][columns], SnakePosition *, int);
void clearSnake(char [][columns], SnakePosition *, int);

// Input
Input getInput(void);

// Render
void printGrid(char [][columns]);
void printGameOver(int);
void clearScreen(void);

//
// Main
//

int main(void) {
    static struct termios oldt, newt;
    
    srand(time(NULL));
    
    // Allows input withou have to press enter
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    
    game();
    
    // Returns terminal to old behaviour
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    
    return 0;
}

void game(void) {
    char grid[rows][columns];
    SnakePosition snake[rows * columns];
    int length = 3;

    initGrid(grid);
    initSnake(grid, snake, length);
    spawnFood(grid);
    update(grid, snake, &length);
    printGameOver(length);
}

//
// Initialize
//

void initGrid(char grid[][columns]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            grid[i][j] = '_';
        }
    }
}

void initSnake(char grid[][columns], SnakePosition *snake, int length) {
    const int row = rows / 2;
    const int column = columns / 2;
    for (int i = 0; i < length; i++) {
        grid[row - i][column] = '0';
        snake[i].row = row - i;
        snake[i].column = column; 
    }
}

//
// Game Logic
//

void update(char grid[][columns], SnakePosition *snake, int *snakeSize) {
    Input input;
    int isAlive = 0;
    do {
        printGrid(grid);
        input = getInput();
        isAlive = validatePosition(grid, input, snake, snakeSize);
        if (!isAlive) break;
    } while (input != QUIT);
}

int validatePosition(char grid[][columns], Input input, SnakePosition *snake, int *length) {
    SnakePosition head = snake[0];
    SnakePosition newPosition = head;
    int isValid = 1;
    int hasEatFood = 0;
    switch (input) {
        case INVALID:
            return 1;
        case LEFT:
            isValid = head.column - 1 >= 0;
            if (!isValid) return 0;
            newPosition.column = head.column - 1;
            break;
        case RIGHT:
            isValid = head.column + 1 < columns;
            if (!isValid) return 0;
            newPosition.column = head.column + 1;
            break;
        case DOWN:
            isValid = head.row + 1 < rows;
            if (!isValid) return 0;
            newPosition.row = head.row + 1;
            break;
        case UP:
            isValid = head.row - 1 >= 0;
            if (!isValid) return 0;
            newPosition.row = head.row - 1;
            break;
    }
    isValid = grid[newPosition.row][newPosition.column] != '0';
    hasEatFood = grid[newPosition.row][newPosition.column] == '*';
    if (hasEatFood) {
        eatFood(grid, snake, length, newPosition);
        clearSnake(grid, snake, *length);
        updateGrid(grid, snake, *length);
        spawnFood(grid);
    } else {
        clearSnake(grid, snake, *length);
        move(snake, *length, newPosition);
        updateGrid(grid, snake, *length);
    }
    return isValid;
}

void eatFood(char grid[][columns], SnakePosition *snake, int *length, SnakePosition newPosition) {
    for (int i = *length; i >= 0; i--) {
        snake[i + 1] = snake[i];
    }
    snake[0] = newPosition;
    *length = *length + 1;
}

void move(SnakePosition *snake, int length, SnakePosition newPosition) {
    for (int i = length; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = newPosition;
}

void spawnFood(char grid[][columns]) {
    int row = rand() % rows;
    int column = rand() % columns;
    if (grid[row][column] == '_') {
        grid[row][column] = '*';
    } else {
        spawnFood(grid);
    }
}

void clearSnake(char grid[][columns], SnakePosition *snake, int length) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {            
            grid[i][j] = grid[i][j] == '0' ? '_' : grid[i][j];
        }
    }
}

void updateGrid(char grid[][columns], SnakePosition *snake, int length) {
    for (int i = 0; i < length; i++) {
        grid[snake[i].row][snake[i].column] = '0';
    }
}


//
// Input
//

Input getInput() {
    char input;
    printf(">> ");
    input = getchar();
    //scanf("%c", &input);
    //clearBuffer();
    switch(input) {
        case 'W':
        case 'w':
        case 'I':
        case 'i':
            return UP;
        case 'S':
        case 's':
        case 'K':
        case 'k':
            return DOWN;
        case 'D':
        case 'd':
        case 'L':
        case 'l':
            return RIGHT;
        case 'A':
        case 'a':
        case 'J':
        case 'j':
            return LEFT;
        case 'Q':
        case 'q':
            return QUIT;
        default:
            return INVALID;
    }
}

void printGrid(char grid[][columns]) {
    clearScreen();
    for (int i = 0; i < rows; i++) {
        printf("|");
        for (int j = 0; j < columns; j++) {
            printf(" %c ", grid[i][j]);
        }
        printf("|\n");
    }
}

void printGameOver(int length) {
    printf("\n\nScore: %d\n", length - 3);
    printf("\n");
    printf("--------------------------------------\n");
    printf("-------------Game Over!!!-------------\n");
    printf("--------------------------------------\n");
    
    usleep(1500000);
    clearScreen();
}

void clearScreen(void) {
    system("clear || cls");
}
