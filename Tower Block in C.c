#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape sequence for cursor positioning
}

int microsecond(double num) {
    return (int)(num * 1000000);
}

void clrScr() {
    printf("\033[H\033[J");
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
    int rank; // Add the rank field
    struct Player* next;
};


// Function to save the leaderboard to a text file
void saveLeaderboard(const char* filename, struct Player* first) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }
    // Write each player's data to the file
    struct Player* current = first;
    while (current != NULL) {
        fprintf(file, "%s %d\n", current->name, current->score);
        current = current->next;
    }
    fclose(file);
}

// Function to load the leaderboard from a text file
struct Player* loadLeaderboard(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return NULL;
    }
    // Read player data from the file and construct the linked list
    struct Player* first = NULL;
    struct Player* current = NULL;
    while (1) {
        struct Player* player = malloc(sizeof(struct Player));
        int bytesRead = fscanf(file, "%s %d", player->name, &player->score);
        if (bytesRead != 2) {
            free(player); // Free the unused memory
            break;
        }
        player->next = NULL;
        if (first == NULL) {
            first = player;
            current = first;
        } else {
            current->next = player;
            current = player;
        }
    }
    fclose(file);
    return first;
}

// Function to free the allocated memory for the linked list
void freeLinkedList(struct Player* first) {
    struct Player* current = first;
    while (current != NULL) {
        struct Player* temp = current;
        current = current->next;
        free(temp);
    }
}

// Function to display the leaderboard
void displayLeaderboard(struct Player* first) {
    clrScr();
    colorFont("yellow");
    gotoXY(7, 7);
    printf("\t    LEADERBOARD:\n\n");
    int rank = 1;
    struct Player* current = first;
    while (current != NULL) {
        if (rank == 1){
            colorFont("yellow");
        } else if (rank == 2){
            colorFont("green");
        } else if (rank == 3){
            colorFont("cyan");
        } else {
            colorFont("reset");
        }
        printf("\t%d. %s - %d\n", rank, current->name, current->score);
        current = current->next;
        rank++;
    }
    if (first == NULL){ // If the leaderboard empty
        printf("\n      Be the one to play now!");
    }
}

// Function to update the leaderboard
struct Player* updateLeaderboard(struct Player* first, char playerName[50], int score) {
    struct Player* newPlayer = (struct Player*)malloc(sizeof(struct Player));
    strncpy(newPlayer->name, playerName, sizeof(newPlayer->name) - 1);
    newPlayer->score = score;
    newPlayer->next = NULL;
    // If the leaderboard is empty or the new player has the highest score
    if (first == NULL || score > first->score) {
        newPlayer->next = first;
        newPlayer->rank = 1; // Set rank as 1 for the new player
        first = newPlayer;
        return first;
    }
    struct Player* prev = NULL;
    struct Player* current = first;
    // Traverse the leaderboard to find the correct position for the new player
    int rank = 1;
    while (current != NULL && score <= current->score) {
        prev = current;
        current = current->next;
        rank++;
    }
    // Insert the new player into the leaderboard
    prev->next = newPlayer;
    newPlayer->next = current;
    newPlayer->rank = rank; // Set the rank for the new player
    // Update the ranking for all players after the new player
    while (current != NULL) {
        current->rank = rank + 1;
        current = current->next;
        rank++;
    }
    saveLeaderboard("leaderboard.txt", first);  // Save the leaderboard to the file
    return first;
}

char gameOverScr(const char str[]) {
    gotoXY(7, 7);
    colorFont("red");
    printf("\t%s", str);
    gotoXY(7, 9);
    printf("   Game Over. Press 1 to try again...");
    gotoXY(7, 11);
    printf("\t 1 - TRY AGAIN?");
    gotoXY(7, 12);
    printf("\t 2 - MAIN MENU");
    char key;
    do { key = _getch(); // Wait until user click
    } while (key != '1' && key != '2');
    return key;
}

// START OF THE GAME //
// ---------------------------------------------------------------------------------------------- //
int main() {
    struct Player* first = NULL;
    struct Player* loadedLeaderboard = loadLeaderboard("leaderboard.txt");
    freeLinkedList(first);
    char name[100];
    bool play = false;
    char key = clrInpBuffer();
    first = updateLeaderboard(first, "Akihiro", 999);
    first = updateLeaderboard(first, "ey", 222);
    first = updateLeaderboard(first, "boi", 696);
    clrScr();
    while (true){
        intro();
        colorFont("reset");
        printf("            Press:\n");
        printf("          1 - START\n");
        printf("       2 - LEADERBOARD\n");
        do { key = _getch(); // Wait until user click
        } while (key != '1' && key != '2');
        if (key == '2'){
            displayLeaderboard(first);
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
                            arrBlockTop++; // This part is using Stack push
                            arrBlock[arrBlockTop] = randNum2;
                            refresh = true;
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
                            first = updateLeaderboard(first, name, score);
                            key = gameOverScr("You fell into the spike...");
                            if (key == '2'){
                                restart = true; 
                                play = false; // Back to main menu
                                clrScr();
                            } else {
                                refresh = true;
                            }
                        } else if (arrBlock[arrBlockTop] != randNum2) { // If it's land on different number, it perform a pop stack
                            erasePrevAnim(blockMiddle, height - arrBlockTop - 1);
                            score--;
                            arrBlockTop--;
                            refresh = true;
                            if (alternateNum == 1) {
                                alternateNum = 2;
                                blockPosX = marginLeft;
                            } else if (alternateNum == 2) {
                                alternateNum = 1;
                                blockPosX = width;
                            }
                            randNum2 = randomNum(numStart, numEnd);
                            if (arrBlockTop < 0) { // Check if less than 0. if true, game over (This is stack underflow or if it's empty)
                                first = updateLeaderboard(first, name, score);
                                play, restart = gameOverScr(" Stack Underflow.");
                                if (key == '2'){
                                    restart = true; 
                                    play = false; // Back to main menu
                                    clrScr();
                                } else {
                                    refresh = true;
                                }
                            }
                        }
                    } else if (key == 'c'){ // If user press c
                        char input[50];
                        gotoXY(8, 2);
                        colorFont("reset");
                        printf("Enter your comment: ");
                        scanf("%100[^\n]", input); // This is how you input string to prevent from bug when animating
                        if (strcmp(input, "autoG") == 0) { // If the user input "autoG"
                            autoG = true;
                        }
                        clrInpBufForScanf(); // This will reset the input to prevent from bug when animating
                        refresh = true;   
                    } else if (key == 'r'){ // if user press r, restart the game
                        restart = true;
                    } else if (key == 'e'){
                        first = updateLeaderboard(first, name, score);
                        restart = true; 
                        play = false; // Back to main menu
                    } else if (autoG && key == '1' || key == '2' || key == '3' ){
                        randNum2 = key - '0'; // That's how you convert char to int
                    }
                }
                if (arrBlockTop == 10){
                    clrScr();
                    gotoXY(4, 6);
                    colorFont("yellow");
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
                        first = updateLeaderboard(first, name, score);
                        restart = true; 
                        play = false; // Back to main menu
                    }
                }
                if (refresh){ // Refresh the stage and score
                    refresh = false;    
                    stage(marginLeft, width, marginTop, height, blockMiddle);
                    counter(marginLeft, score, name); 
                    prodBlockStack(arrBlockTop, blockMiddle, height, arrBlock);
                }
                key = clrInpBuffer(); // This will reset the user clicked to prevent from bug when animating
                usleep(microsecond(speed)); // Animation speed      
            }
        }
    }
    colorFont("reset");
    return 0;
}

