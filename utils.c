#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

// Clears the terminal screen for Windows and Linux/Mac
void clear_screen() {
    system("clear || cls");
}

// Empties the input buffer to prevent scanf from skipping inputs
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Just consume characters
    }
}

// Creates the binary files if they don't exist yet
void setup_files() {
    FILE *fp1 = fopen("flights.dat", "ab");
    if (fp1 != NULL) {
        fclose(fp1);
    }

    FILE *fp2 = fopen("bookings.dat", "ab");
    if (fp2 != NULL) {
        fclose(fp2);
    }
}

// Returns 1 if strings match regardless of uppercase/lowercase, 0 otherwise
int compare(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
            return 0; // Not equal
        }
        str1++;
        str2++;
    }
    // If both strings ended at the same time, they are equal
    if (*str1 == '\0' && *str2 == '\0') {
        return 1;
    } else {
        return 0;
    }
}
