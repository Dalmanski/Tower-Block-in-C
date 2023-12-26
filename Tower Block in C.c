#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape sequence for cursor positioning
}

int microsecond(double num) {
    return (int)(num * 1000000);
}

void clrScr() {
    printf("\033[H\033[J");
}

void waitKeyPress() {
    while (!_kbhit()) {
        // Do nothing while waiting for a key press
    }
}

int clrInpBuffer(){
    // Clear the input buffer at the beginning
    while (_kbhit()) {
        _getch();
    }
    return -1;
}

void clrInpBufForScanf(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void erasePrevAnim(int x, int y) {
    gotoXY(x, y); // Erase before block animate
    printf(" ");
}

void colorFont(char color[]) {
    if (color == "green") {
        printf("\033[1;32m");
    } else if (color == "yellow") {
        printf("\033[1;33m");
    } else if (color == "cyan") {
        printf("\033[1;36m");
    } else if (color == "white") {
        printf("\033[1;37m");
    } else if (color == "red") {
        printf("\033[1;31m");
    } else if (color == "violet") {
        printf("\033[1;35m");
    } else if (color == "reset") {
        printf("\033[0m");
    } else {
        clrScr();
        colorFont("reset");
        printf("Error color %s", color);
        exit(0);
    }
}

void stage(int marginLeft, int width, int marginTop, int height, int blockMiddle){
    clrScr();
    for (int i = marginLeft-1; i <= width+1; i++) { // Form a stage
         for (int j = marginTop; j <= height+1; j++) {     
            if (j == height){  
                colorFont("red");
                gotoXY(i, j);
                printf("▲");        
            }
            if (j == height && i == blockMiddle){
                colorFont("violet");
                gotoXY(i, j);
                printf("☒");
            }
            if (i == marginLeft-1 || i == width+1 || j == height+1 ) {
                colorFont("violet");
                gotoXY(i, j);
                printf("☒");
            }
        }
    }
}

void counter(int marginLeft, int arrBlockTop, char name[100]) {
    gotoXY(marginLeft + 2, 2);
    colorFont("yellow");
    printf("Player: %s", name);
    gotoXY(marginLeft + 5, 3);
    printf("Score: %d", arrBlockTop);
}

void intro(){
    colorFont("yellow");
    printf("+============================+\n");
    printf("| _____                      |\n");
    printf("||_   _|____      _____ _ __ |\n");
    printf("|  | |/ _ \\ \\ /\\ / / _ \\ '__||\n");
    printf("|  | | (_) \\ V  V /  __/ |   |\n");
    printf("| _|_|\\___/ \\_/\\_/ \\___|_|   |\n");
    printf("|| __ )| | ___   ___| | __   |\n");
    printf("||  _ \\| |/ _ \\ / __| |/ /   |\n");
    printf("|| |_) | | (_) | (__|   <    |\n");
    printf("||____/|_|\\___/ \\___|_|\\_\\   |\n");
    printf("| / ___| __ _ _ __ ___   ___ |\n");
    printf("|| |  _ / _` | '_ ` _ \\ / _ \\|\n");
    printf("|| |_| | (_| | | | | | |  __/|\n");
    printf("| \\____|\\__,_|_| |_| |_|\\___||\n");
    printf("+============================+\n\n");
}

bool gameOverScr(const char str[]) {
    gotoXY(5, 7);
    colorFont("red");
    printf("\n\n\t%s", str);
    printf("\n\nGame Over. Press anything to try again...");
    waitKeyPress();
    return true;
}

void changeNumCol(int num) {
    if (num == 1) {
        colorFont("green");
    } else if (num == 2) {
        colorFont("yellow");
    } else if (num == 3) {
        colorFont("cyan");
    }
}

void blockDrop(int blockPosX, int i, int randNum2, float speed) {
    gotoXY(blockPosX, i);
    changeNumCol(randNum2);
    printf("%d", randNum2);
    usleep(microsecond(speed / 2));
    erasePrevAnim(blockPosX, i);
}

int randomNum(int start, int end) {
    return rand() % (end - start + 1) + start;
}

void prodBlockStack(int arrBlockTop, int blockMiddle, int height, int arrBlock[]){
    for (int i = 0; i <= arrBlockTop; i++) { // Produce a stack block after the block landed
        gotoXY(blockMiddle, height - i - 1);
        changeNumCol(arrBlock[i]);
        printf("%d", arrBlock[i]);
    }
}

struct Player {
    char name[50];
    int score;
    struct Player* next;
};

void leaderboard(){
    struct Player* first = NULL;
    struct Player* second = NULL;
    struct Player* third = NULL;
    first = (struct Player*)malloc(sizeof(struct Player));
    second = (struct Player*)malloc(sizeof(struct Player));
    third = (struct Player*)malloc(sizeof(struct Player));
    first->next = second;
    second->next = third;
    third->next = NULL;
}

// START OF THE GAME //
// ------------------------------------------------------------------------------------------------------------------------------ //
int main() {
    clrScr();
    char name[100];
    bool play = false;
    struct Player player[100];
    int slot = 0;
    while (true){
        intro();
        colorFont("reset");
        printf("            Press:\n");
        printf("          1 - START\n");
        printf("       2 - LEADERBOARD\n");
        int key;
        do {
            key = _getch();
        } while (key != '1');
        clrScr();
        intro();
        colorFont("reset");
        printf("Enter your name: ");
        scanf("%s", &name);
        strcpy(player[slot].name, name);
        slot++;
        play = true;
        while (play) {
            int marginTop = 5, marginLeft = 10;
            int width = marginLeft + 16, height = marginTop + 15;    
            int blockPosX = width, blockPosY = marginTop + 2;
            float speed = 0.1;
            int maxBlockHeight = 10;
            int arrBlock[maxBlockHeight];
            int blockSpawnPosY = height - 1;
            int arrBlockTop = 0;
            int blockMiddle = marginLeft + (width - marginLeft) / 2;
            int numStart = 1, numEnd = 3;
            int randNum1 = randomNum(numStart, numEnd);
            int randNum2 = randomNum(numStart, numEnd);
            int alternateNum = 1;
            bool gameOver = false;
            bool autoG = false;
            stage(marginLeft, width, marginTop, height, blockMiddle);
            counter(marginLeft, arrBlockTop, name);
            key = clrInpBuffer();
            
            while (!gameOver) {
                gotoXY(blockMiddle, blockSpawnPosY); // Place 1 block into the middle
                changeNumCol(randNum1);
                printf("%d", randNum1);
                arrBlock[0] = randNum1;
                erasePrevAnim(blockPosX, blockPosY);
                if (alternateNum == 1) { // This alternateNum will looks like animate a block from left to right then right to left and so on
                    blockPosX--; // block start to go from right to left
                    if (blockPosX == marginLeft) { // If block reach at the left
                        randNum2 = randomNum(numStart, numEnd);
                        alternateNum = 2;
                    }
                } else if (alternateNum == 2) {
                    blockPosX++; // block start to go from left to right
                    if (blockPosX == width) { // If block reach at the right
                        randNum2 = randomNum(numStart, numEnd);
                        alternateNum = 1;
                    }
                }
                gotoXY(blockPosX, blockPosY); // Block spawn with animation
                changeNumCol(randNum2);
                printf("%d", randNum2);
                if (_kbhit() || autoG) { // Detect if user press or autoG activate
                    if (!autoG || _kbhit()){ // This will be activate if autoG is not activate
                        key = _getch();
                    }    
                    if (key == '\r' || key == ' ' || autoG && blockPosX == blockMiddle && arrBlock[arrBlockTop] == randNum2) { // '\r' for Enter key and ' ' for space
                        for (int i = blockPosY; i < height - arrBlockTop - 1; i++) { // Perform block drop
                            blockDrop(blockPosX, i, randNum2, speed);
                        }
                        if (arrBlock[arrBlockTop] == randNum2 && blockPosX == blockMiddle) {
                            arrBlockTop++; // This part is using Stack push
                            arrBlock[arrBlockTop] = randNum2;
                            stage(marginLeft, width, marginTop, height, blockMiddle);
                            counter(marginLeft, arrBlockTop, name);
                            prodBlockStack(arrBlockTop, blockMiddle, height, arrBlock);
                            if (alternateNum == 1) {
                                alternateNum = 2;
                                blockPosX = marginLeft;
                            } else if (alternateNum == 2) {
                                alternateNum = 1;
                                blockPosX = width;
                            }
                            randNum2 = randomNum(numStart, numEnd);
                        } else if (blockPosX != blockMiddle) { // This will land on the floor and it will game over
                            for (int i = height - arrBlockTop - 1; i <= blockSpawnPosY; i++) { // Perform block drop
                                blockDrop(blockPosX, i, randNum2, speed);
                            }
                            gameOver = gameOverScr("You fell into the spike...");
                        } else if (arrBlock[arrBlockTop] != randNum2) { // If it's land on different number, it perform a pop stack
                            erasePrevAnim(blockMiddle, height - arrBlockTop - 1);
                            arrBlockTop--;
                            stage(marginLeft, width, marginTop, height, blockMiddle);
                            counter(marginLeft, arrBlockTop, name);
                            prodBlockStack(arrBlockTop, blockMiddle, height, arrBlock);
                            if (alternateNum == 1) {
                                alternateNum = 2;
                                blockPosX = marginLeft;
                            } else if (alternateNum == 2) {
                                alternateNum = 1;
                                blockPosX = width;
                            }
                            randNum2 = randomNum(numStart, numEnd);
                            if (arrBlockTop < 0) { // Check if less than 0. if true, game over (This is stack underflow or if it's empty)
                                gameOver = gameOverScr(" Stack Underflow.");
                            }
                        } else {
                            gameOver = gameOverScr("I dont know this game over");
                        }
                    } else if (key == 'c'){ // If user press c
                        char input[50];
                        gotoXY(8, 2);
                        colorFont("reset");
                        printf("Enter your comment: ");
                        scanf("%49[^\n]", input); // This is how you input string to prevent from bug when animating
                        if (strcmp(input, "autoG") == 0) { // If the user input "autoG"
                            autoG = true;
                        }
                        clrInpBufForScanf(); // This will reset the input to prevent from bug when animating
                        stage(marginLeft, width, marginTop, height, blockMiddle);
                        counter(marginLeft, arrBlockTop, name);
                        prodBlockStack(arrBlockTop, blockMiddle, height, arrBlock);
                    } else if (key == 'r'){ // if user press r, restart the game
                        gameOver = true;
                    } else if (key == 'p'){
                        gotoXY(8, 7);
                        colorFont("white");
                        printf("Pause, press any key to play.\n\n");
                        gotoXY(16, 9);
                        printf("1 - RESUME\n");
                        gotoXY(18, 10);
                        printf("2 - END\n");
                        key = _getch();
                        if (key == 2){

                        }
                        stage(marginLeft, width, marginTop, height, blockMiddle);
                        counter(marginLeft, arrBlockTop, name);
                    }
                }
                if (arrBlockTop == 10){
                    gotoXY(5, 7);
                    colorFont("red");
                    printf("\n\n\tYou win!");
                    leaderboard(arrBlockTop);
                    printf("\n\nYou are in the %d place", arrBlockTop);
                    waitKeyPress();
                }
                key = clrInpBuffer(); // This will reset the user clicked to prevent from bug when animating
                usleep(microsecond(speed)); // Animation speed
            }
        }
        colorFont("reset");
        return 0;
    }
}

