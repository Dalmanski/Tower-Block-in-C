#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX 10
#define FILENAME "leaderboard.txt"
//#define FILENAME "C:\\Users\\Jayrald John\\Documents\\Code\\Tower Block in C\\leaderboard.txt"

void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape sequence for cursor positioning (Position X and Y)
}

int microsecond(double num) {
    return (int)(num * 1000000);
}

void clrScr() {
    printf("\033[H\033[J"); // This is how you clear all the screen
    gotoXY(0,0);
}

void waitAnyKeyPress() {
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

void box(){
    for (int i = 0; i <= 18; i++) {
        for (int j = 0; j <= 17; j++) {
            if (i == 0 || i == 18) {
                printf("==");
            } else if (j == 0 || j == 17) {
                printf("||");
            } else {
                printf("  ");
            }
        }
        printf("\n");
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
    printf("+============================+          \n");
    printf("| _____                      |          \n");
    printf("||_   _|____      _____ _ __ |          \n");
    printf("|  | |/ _ \\ \\ /\\ / / _ \\ '__||      \n");
    printf("|  | | (_) \\ V  V /  __/ |   |         \n");
    printf("| _|_|\\___/ \\_/\\_/ \\___|_|   |      \n");
    printf("|| __ )| | ___   ___| | __   |          \n");
    printf("||  _ \\| |/ _ \\ / __| |/ /   |        \n");
    printf("|| |_) | | (_) | (__|   <    |          \n");
    printf("||____/|_|\\___/ \\___|_|\\_\\   |      \n");
    printf("| / ___| __ _ _ __ ___   ___ |          \n");
    printf("|| |  _ / _` | '_ ` _ \\ / _ \\|        \n");
    printf("|| |_| | (_| | | | | | |  __/|          \n");
    printf("| \\____|\\__,_|_| |_| |_|\\___||       \n");
    printf("+============================+        \n\n");
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

// Perform a block dropping
void prodBlockStack(int arrBlockTop, int blockMiddle, int height, int arrBlock[]){
    for (int i = 0; i <= arrBlockTop; i++) { // Produce a stack block after the block landed
        gotoXY(blockMiddle, height - i - 1);
        changeNumCol(arrBlock[i]);
        printf("%d", arrBlock[i]);
    }
}

struct Player {
    char name[50];
    int totalScore;
    struct Player *next;
};

void clearLeaderboard() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error opening file for clearing leaderboard!\n");
        exit(1);
    }
    fclose(file);
}

// Step 3, save all the sorted ranking player
void saveLeaderboard(struct Player *head) {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }
    while (head != NULL) {
        fprintf(file, "%s %d\n", head->name, head->totalScore);
        head = head->next;
    }
    fclose(file);
}

// Step 2, sort the player on ranking from highest to lowest score
struct Player *sortPlayers(struct Player *head) {
    if (head == NULL || head->next == NULL) {
        return head;  // List is empty or has only one element, nothing to sort
    }
    int swapped;
    struct Player *temp;
    do {
        swapped = 0;
        struct Player *current = head;
        struct Player *prev = NULL;
        while (current != NULL && current->next != NULL) {
            struct Player *nextNode = current->next;
            if (current->totalScore < nextNode->totalScore) {
                // Swap players if they are in the wrong order
                current->next = nextNode->next;
                nextNode->next = current;
                if (prev == NULL) {
                    head = nextNode;
                } else {
                    prev->next = nextNode;
                }
                prev = nextNode;
                swapped = 1;
            } else {
                prev = current;
                current = current->next;
            }
        }
    } while (swapped);
    saveLeaderboard(head);
    return head;
}

// Step 1 to save that player on the leaderboard
struct Player *updateLeaderboard(struct Player *head, const char *name, int score) {
    struct Player *current = head;
    struct Player *prev = NULL;
    // Check if the player already exists
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Update the total score if the player exists
            if (score > current->totalScore) {
                current->totalScore = score;
                return sortPlayers(head);
            }
            return head;  // Player exists, and the score is not higher
        }
        prev = current;
        current = current->next;
    }
    // If the player does not exist, add a new player while maintaining sorted order
    struct Player *newPlayer = malloc(sizeof(struct Player));
    if (newPlayer == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(newPlayer->name, name);
    newPlayer->totalScore = score;
    newPlayer->next = NULL;
    // If the new player should be the new head (highest score)
    if (prev == NULL || score > head->totalScore) {
        newPlayer->next = head;
        return sortPlayers(newPlayer);
    }
    // Insert the new player at the appropriate position to maintain sorted order
    prev->next = sortPlayers(newPlayer);
    return head;
}

void displayLeaderboard(struct Player *head) {
    clrScr();
    colorFont("yellow");
    box();
    gotoXY(7, 4);
    printf("\t    LEADERBOARD:\n\n");
    int rank = 1;
    while (head != NULL) {
        if (rank == 1) {
            colorFont("yellow");
        } else if (rank == 2) {
            colorFont("green");
        } else if (rank == 3) {
            colorFont("cyan");
        } else {
            colorFont("reset");
        }
        printf("\t%d. %s - %d\n", rank, head->name, head->totalScore);
        head = head->next;
        rank++;
    }
    colorFont("violet");
    printf("\n\n      Click anything to start...\n\n");
}

void freeLinkedList(struct Player *head) {
    struct Player *current = head;
    while (current != NULL) {
        struct Player *nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

struct Player *loadLeaderboard(void) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Error opening file for reading!\n");
        return NULL;
    }
    struct Player *head = NULL;
    char name[50];
    int score;
    // Check if the file is empty
    if (fscanf(file, "%s %d", name, &score) != 2) {
        fclose(file);
        return NULL;
    }
    // File is not empty, rewind and read data
    rewind(file);
    while (fscanf(file, "%s %d", name, &score) == 2) {
        head = updateLeaderboard(head, name, score);
    }
    fclose(file);
    return head;
}

char gameOverScr(const char str[]) {
    colorFont("red");
    gotoXY(7, 7);
    printf("\t%s", str);
    gotoXY(6, 9);
    printf("Game Over. Press 1 to try again...");
    gotoXY(12, 11);
    printf("1 - TRY AGAIN?");
    gotoXY(12, 12);
    printf("2 - MAIN MENU");
    char key;
    do { key = _getch(); // Wait until user click
    } while (key != '1' && key != '2');
    return key;
}

// START OF THE GAME //
// ---------------------------------------------------------------------------------------------- //
int main() {
    struct Player *head = loadLeaderboard();
    // Add or update players
    //updateLeaderboard(head, "jayrald", 1231);
    //updateLeaderboard(head, "john", 500);
    //updateLeaderboard(head, "jayrald", 200);
    // freeLinkedList(first);
    char name[100];
    bool play = false;
    char key = clrInpBuffer();
    while (true){
        clrScr();
        intro();
        colorFont("reset");
        printf("            Press:\n");
        printf("          1 - START\n");
        printf("       2 - LEADERBOARD\n");
        do { key = _getch(); // Wait until user click
        } while (key != '1' && key != '2');
        if (key == '2'){
            displayLeaderboard(head);
            waitAnyKeyPress();
        }
        clrScr();
        intro();
        colorFont("reset");
        printf("Enter your name: ");
        scanf("%100[^\n]", name);    
        clrInpBufForScanf();
        play = true;
        while (play) {
            int marginTop = 5, marginLeft = 10;
            int width = marginLeft + 16, height = marginTop + 15;    
            int blockPosX = width, blockPosY = marginTop + 2;
            float speed = 0.1;
            int maxBlockHeight = 10;
            int arrBlock[maxBlockHeight];
            int blockSpawnPosY = height - 1;
            int arrBlockTop = 0, score = 0;
            int blockMiddle = marginLeft + (width - marginLeft) / 2;
            int numStart = 1, numEnd = 3;
            int randNum1 = randomNum(numStart, numEnd);
            int randNum2 = randomNum(numStart, numEnd);
            int alternateNum = 1;
            bool restart = false;
            bool autoG = false;
            bool refresh = false;
            stage(marginLeft, width, marginTop, height, blockMiddle);
            counter(marginLeft, arrBlockTop, name);
            key = clrInpBuffer();    
            while (!restart) {
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
                            score++;
                            arrBlockTop++; // This part is using PUSH STACK
                            arrBlock[arrBlockTop] = randNum2;
                            refresh = true;         
                            randNum2 = randomNum(numStart, numEnd);
                        } else if (blockPosX != blockMiddle) { // This will land on the floor and it will game over
                            for (int i = height - arrBlockTop - 1; i <= blockSpawnPosY; i++) { // Perform block drop
                                blockDrop(blockPosX, i, randNum2, speed);
                            }        
                            key = gameOverScr(" You fell into the spike...");
                            refresh = true;
                            restart = true; 
                            if (key == '2'){
                                play = false; // Back to main menu
                                clrScr();
                            }
                        } else if (arrBlock[arrBlockTop] != randNum2) { // If it's land on different number, it perform a POP STACK
                            erasePrevAnim(blockMiddle, height - arrBlockTop - 1);
                            score--;
                            arrBlockTop--;
                            refresh = true;
                            randNum2 = randomNum(numStart, numEnd);
                            if (arrBlockTop < 0) { // Check if less than 0. if true, game over (This is stack underflow or if it's empty)                          
                                key = gameOverScr(" Stack Underflow.");
                                restart = true; 
                                if (key == '2'){
                                    play = false; // Back to main menu
                                    clrScr();
                                }
                            }
                        }
                    } else if (key == 'c'){ // If user press c
                        char input[50];
                        gotoXY(8, 2);
                        colorFont("reset");
                        printf("Enter your comment: ");
                        scanf("%100[^\n]", input); // This is how you input string to prevent from bug when animating
                        refresh = true;   
                        if (strcmp(input, "autoG") == 0) { // If the user input "autoG"
                            autoG = true;
                        } else if (strcmp(input, "erase") == 0){
                            clearLeaderboard();
                            refresh = false; // The reason is because it didn't clear the txt file due to struct stored
                        }
                        clrInpBufForScanf(); // This will reset the input to prevent from bug when animating
                    } else if (key == 'r'){ // if user press r, restart the game
                        restart = true;
                    } else if (key == 'e'){
                        restart = true; 
                        play = false; // Back to main menu
                    } else if (key == '1' || key == '2' || key == '3'){
                        if (autoG){
                            randNum2 = key - '0'; // That's how you convert char to int
                        }
                    }
                }
                if (arrBlockTop == MAX){
                    clrScr();
                    colorFont("yellow");
                    box();
                    gotoXY(4, 6);
                    printf("\t      You win!");
                    gotoXY(4, 9);
                    printf("\t    1 - CONTINUE");
                    gotoXY(4, 10);
                    printf("\t      2 - END");
                    do { key = _getch(); // Wait until user click
                    } while (key != '1' && key != '2');
                    if (key == '1'){
                        arrBlockTop = 0;
                        if (speed > 0.02){
                            speed -= 0.02; // Increase speed every win
                        }
                        refresh = true;
                        randNum1 = randomNum(numStart, numEnd);
                    } else if (key == '2'){
                        restart = true; 
                        play = false; // Back to main menu
                        clrScr();
                    }
                }
                if (refresh){ // Refresh the stage and score
                    refresh = false;    
                    stage(marginLeft, width, marginTop, height, blockMiddle);
                    counter(marginLeft, score, name); 
                    prodBlockStack(arrBlockTop, blockMiddle, height, arrBlock);
                    head = updateLeaderboard(head, name, score);     
                    if (alternateNum == 1) {
                        alternateNum = 2;
                        blockPosX = marginLeft;
                    } else if (alternateNum == 2) {
                        alternateNum = 1;
                        blockPosX = width;
                    }
                }
                key = clrInpBuffer(); // This will reset the user clicked to prevent from bug when animating
                usleep(microsecond(speed)); // Animation speed      
            }
        }
    }
    colorFont("reset");
    return 0;
}

