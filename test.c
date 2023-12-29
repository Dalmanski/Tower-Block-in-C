#include <stdio.h>
#include <stdlib.h>

int main() {
    // Assuming "leaderboard.txt" is in the current working directory
    const char* filename = "leaderboard.txt";

    // Get the full path of the file
    char fullPath[FILENAME_MAX];
    if (_fullpath(fullPath, filename, FILENAME_MAX) == NULL) {
        perror("Error getting full path");
        return EXIT_FAILURE;
    }

    // Print the full path
    printf("Full path of the file: %s\n", fullPath);

    return EXIT_SUCCESS;
}
// I check this and the file location is C:\mingw64\bin\leaderboard.txt
