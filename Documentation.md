#  The Ultimate Beginner's Guide to the C Flight Booking System

Welcome! This document is an **exhaustive, line-by-line, atom-by-atom** breakdown of the Flight Booking System. If you have only been studying C for two months, some concepts here (like file pointers, `fseek`, or buffer clearing) might seem like magic. By the end of this guide, you will understand exactly how every single character in this codebase works.


---

##  Part 1: Core Beginner Concepts to Understand First

Before we look at the files, you must understand the four pillars of this codebase:

1. **Structs (`struct`)**: A struct is a custom container. A standard `int` holds one number. A `char` holds one letter. But a `struct Flight` holds an ID, a name, a date, and a price all inside one neat package. It allows us to group related variables together.
2. **Binary File Handling (`.dat`)**: Instead of saving data as readable text (like a `.txt` file), we save the exact binary memory blueprint of our `structs` directly to the hard drive. This is much faster and easier to read/write using `fread()` and `fwrite()`. 
3. **Soft Deletion**: When an admin "removes" a flight, we do not actually delete the data from the hard drive. That is complicated and risky. Instead, we have a variable called `is_deleted`. We simply change it from `0` to `1`. Our program is coded to ignore any flight where `is_deleted == 1`. 
4. **Buffer Clearing**: When you type a number and press `Enter` in C, `scanf` reads the number but leaves the `Enter` key (the newline character `\n`) stuck in the keyboard buffer. The next time you ask for text, C sees that leftover `Enter` and skips your input! We use a custom `clear_buffer()` function to swallow those leftover `Enters`.

---

##  Part 2: File-by-File Breakdown

### 1. `Makefile`
A Makefile is a script that tells the compiler (`gcc`) how to build your program. Instead of typing a massive command into the terminal every time, you just type `make`.

```makefile
CC = gcc
CFLAGS = -Wall

all:
	$(CC) $(CFLAGS) main.c utils.c flight.c booking.c -o flight_booking

clean:
	rm -f flight_booking flights.dat bookings.dat
```
* **`CC = gcc`**: Sets a variable `CC` to use the GNU C Compiler.
* **`CFLAGS = -Wall`**: Tells the compiler to show **All Warnings**. It catches beginner mistakes.
* **`all:`**: The default target. It runs the command below it.
* **`$(CC) $(CFLAGS) ... -o flight_booking`**: Translates to `gcc -Wall main.c utils.c flight.c booking.c -o flight_booking`. It compiles all `.c` files into one executable named `flight_booking`.
* **`clean:`**: If you type `make clean` in the terminal, it deletes the compiled program and your database files to give you a fresh start.

---

### 2. `structures.h`
This header file defines the shape of our data.

```c
#ifndef STRUCTURES_H
#define STRUCTURES_H
```
* **Include Guards**: Imagine `fileA.c` includes `structures.h`, and `fileB.c` also includes it. The compiler would see the same structs defined twice and crash! `#ifndef` means "If Not Defined". If `STRUCTURES_H` hasn't been read yet, read the file. If it has, skip it.

```c
struct Flight {
    int flight_id;
    char flight_num[20];
    char source[50];
    char dest[50];
    char date[20];
    char time[20];
    int total_seats;
    int available_seats;
    float price;
    int is_deleted; // 0 = active, 1 = removed by admin
};
```
* **`flight_id`**: A unique whole number for the flight (e.g., 1, 2, 3).
* **`char[50]` arrays**: These are "strings" (text). We allocate 50 characters to hold long city names like "Los Angeles".
* **`total_seats` & `available_seats`**: When someone books a ticket, `available_seats` goes down by 1. `total_seats` never changes.
* **`float price`**: Handles decimal numbers for money (e.g., 199.99).
* **`is_deleted`**: Our soft-delete toggle.

```c
struct Booking {
    int booking_id;
    char passenger_name[50];
    int flight_id;
    int seat_number;
    int is_cancelled; // 0 = active, 1 = cancelled
};
```
* **`flight_id`**: This is called a *Foreign Key* in databases. The booking doesn't copy the flight's destination or time; it just saves the `flight_id`. When we need the flight details, we look up this ID in the flights file.

---

### 3. `utils.h` & `utils.c`
This file contains utility helper tools used all over the application.

```c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
```
* **`<ctype.h>`**: Includes character-handling functions like `tolower()`.

```c
void clear_screen() {
    system("clear || cls"); 
}
```
* **`system()`**: Tells the operating system terminal to execute a command. `clear` works on Mac/Linux. `cls` works on Windows. The `||` means "OR". It tries `clear`, and if it fails, it tries `cls`. This makes the code work on any computer!

```c
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Just consume characters
    }
}
```
* **The `scanf` bug fix**: `getchar()` reads exactly one character from the keyboard buffer. This `while` loop continuously eats characters until it eats a newline (`\n`) or reaches the End Of File (`EOF`). It acts as a vacuum cleaner for dirty inputs.

```c
void setup_files() {
    FILE *fp1 = fopen("flights.dat", "ab");
    if (fp1 != NULL) { fclose(fp1); }

    FILE *fp2 = fopen("bookings.dat", "ab");
    if (fp2 != NULL) { fclose(fp2); }
}
```
* **`fopen("ab")`**: Opens a file in "Append Binary" mode. If the file does not exist, **it creates it**. If it already exists, it does absolutely nothing (it just readies the file for appending). We immediately close it. This ensures that the files exist before the rest of our code tries to read them, preventing crashes!

```c
int compare(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
            return 0; // Not equal
        }
        str1++;
        str2++;
    }
    if (*str1 == '\0' && *str2 == '\0') {
        return 1;
    } else {
        return 0;
    }
}
```
* **Pointers `*str1`**: `str1` points to the first letter of the string.
* **`tolower()`**: Converts "A" to "a". So "NeW YoRk" becomes "new york".
* **`str1++`**: Moves the pointer to the next letter in the word.
* **`*str1 == '\0'`**: Checks if we reached the null terminator (the invisible character at the end of every C string). If both strings end exactly at the same time, they are a perfect match. Returns `1` (True).

---

### 4. `flight.c` (Flight Management)

Let's break down the functions inside `flight.c`.

#### `add_flight()`
```c
    fp = fopen("flights.dat", "rb");
    if (fp != NULL) {
        while (fread(&temp, sizeof(struct Flight), 1, fp) == 1) {
            if (temp.flight_id > max_id) {
                max_id = temp.flight_id;
            }
        }
        fclose(fp);
    }
    f.flight_id = max_id + 1;
```
* **Goal**: Give the new flight a unique ID automatically.
* **`fopen("rb")`**: Read Binary mode.
* **`fread(&temp, sizeof(struct Flight), 1, fp)`**: Reads exactly one chunk of data the size of a `Flight` struct and puts it into the variable `temp`. If it successfully reads 1 item, it returns `1`. The `while` loop repeats until the end of the file.
* We keep track of the highest `flight_id` we see. The new flight gets `max_id + 1`.

```c
    printf("Enter Source City: ");
    scanf(" %49[^\n]", f.source);
```
* **The Magic Scanf**: This is a pro-beginner trick!
    * **The space before `%`**: Tells `scanf` to ignore any leftover spaces or newlines before reading.
    * **`49`**: Prevents the user from typing 100 characters and crashing the program (buffer overflow). Leaves 1 space for the `\0`.
    * **`[^\n]`**: Means "Read everything UNTIL the user presses Enter (`\n`)". This allows users to type cities with spaces, like "Los Angeles". Standard `%s` would stop at the space and only read "Los".

```c
    fp = fopen("flights.dat", "ab");
    fwrite(&f, sizeof(struct Flight), 1, fp);
    fclose(fp);
```
* **`fopen("ab")`**: Append Binary. Adds the new struct to the very end of the file without deleting the old ones.
* **`fwrite`**: Takes the memory address of `f` (`&f`) and writes it to the hard drive.

#### `remove_flight()`
```c
    fp = fopen("flights.dat", "rb+");
```
* **`rb+`**: Read and Write Binary mode. We need to read the file to find the flight, and then write over it.

```c
    while (fread(&f, sizeof(struct Flight), 1, fp) == 1) {
        if (f.flight_id == target_id && f.is_deleted == 0) {
            f.is_deleted = 1; 
            
            fseek(fp, -sizeof(struct Flight), SEEK_CUR);
            fwrite(&f, sizeof(struct Flight), 1, fp);
            found = 1;
            break;
        }
    }
```
* **The `fseek` maneuver**: This is the most complex concept.
    * When `fread` successfully reads the struct, it physically moves the file pointer *forward* by the size of one struct, so it is ready to read the next one.
    * But we want to modify the struct we *just* read! If we `fwrite` right now, we will overwrite the *next* flight in the list.
    * **`fseek(fp, -sizeof(struct Flight), SEEK_CUR)`**: This commands the file pointer to move backwards (`-sizeof`) from its Current Position (`SEEK_CUR`). Now the pointer is aiming directly at the original struct.
    * **`fwrite`**: Now we overwrite the old data with our updated data (`is_deleted = 1`).

#### `view_all_flights()` & `view_available_flights()`
```c
    while (fread(&f, sizeof(struct Flight), 1, fp) == 1) {
        char status[20];
        if (f.is_deleted == 1) {
            strcpy(status, "DELETED");
        } else if (f.available_seats == 0) {
            strcpy(status, "FULL");
        } else {
            strcpy(status, "ACTIVE");
        }

        printf("%d\t%s\t\t%s\t\t%s\t\t%d\t%s\n", 
               f.flight_id, f.flight_num, f.source, f.dest, f.available_seats, status);
    }
```
* This loops through the file. We use `if/else` logic to determine a friendly text label for the status.
* **`\t`**: Represents a "Tab" space, keeping columns aligned in the terminal.

#### `search_flight()`
```c
    if (compare(f.source, search_source) == 1 && 
        compare(f.dest, search_dest) == 1 && 
        strcmp(f.date, search_date) == 0) {
```
* Calls our custom utility function. If the source matches AND the destination matches AND the date exactly matches (using standard `strcmp`), it prints the flight.

---

### 5. `booking.c` (The Heart of the System)

This file manages the logic connecting a passenger to a flight.

#### `book_flight()`
```c
    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
        if (f.flight_id == target_flight_id && f.is_deleted == 0) {
            if (f.available_seats <= 0) { ... return; }

            f.available_seats = f.available_seats - 1;
            b.seat_number = (f.total_seats - f.available_seats);

            fseek(fp_flight, -sizeof(struct Flight), SEEK_CUR);
            fwrite(&f, sizeof(struct Flight), 1, fp_flight);
            break;
        }
    }
```
* First, we must ensure the flight exists and has seats.
* **`b.seat_number = (f.total_seats - f.available_seats)`**: A simple mathematical trick to assign sequential seat numbers. If total is 100, and available becomes 99, the seat number assigned is 100 - 99 = 1.
* We deduct the seat and overwrite the flight using the `fseek` trick explained earlier.
* Then, we read `bookings.dat` to find the highest booking ID, add 1, assign the passenger name, and `fwrite` the booking in `ab` mode.

#### `view_all_booked_flights()`
```c
    printf("%-10s %-15s %-10s %-15s %-15s %-12s %-5s %-10s\n", ...);
```
* **Formatting Magic**: `%-15.15s`
    * The `-` means align the text to the LEFT.
    * The first `15` means the column must be exactly 15 spaces wide. If the name is "Bob", it adds 12 blank spaces.
    * The `.15` means if the text is longer than 15 characters, cut it off at 15 so it doesn't break the table formatting.

```c
    while (fread(&b, sizeof(struct Booking), 1, fp_book) == 1) {
        ...
        fp_flight = fopen("flights.dat", "rb");
        while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
            if (f.flight_id == b.flight_id) {
                // copy data to local variables
                break;
            }
        }
        fclose(fp_flight);
        ...
        printf(...);
    }
```
* **Nested Loop Database Join**: We read Booking #1. Inside that loop, we open the flights file and search for the flight matching `b.flight_id`. Once found, we copy the route/date, close the flights file, print the row to the terminal, and move on to Booking #2.

#### `cancel_booking()`
```c
    // 1. Mark booking as cancelled
    while (fread(&b, sizeof(struct Booking), 1, fp_book) == 1) { ... b.is_cancelled = 1; ... }
    
    // 2. Add seat back to flight
    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) { ... f.available_seats += 1; ... }
```
* We find the booking using `rb+`, change `is_cancelled` to 1, and save `flight_to_restore = b.flight_id`.
* We open flights in `rb+`, find that `flight_id`, add 1 back to `available_seats`, and overwrite. 

#### `reschedule_booking()`
This is the hardest function. It requires moving seats between two different flights while updating a booking.

1.  **Find the Booking**: Read `bookings.dat`. Store the `old_flight_id`.
2.  **Verify New Flight**: Check if `new_flight_id` exists and `available_seats > 0`.
3.  **Restore Old Seat**:
    ```c
    rewind(fp_flight); 
    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
        if (f.flight_id == old_flight_id) {
            f.available_seats += 1;
            fseek(...); fwrite(...); break;
        }
    }
    ```
    *   **`rewind(fp_flight)`**: This built-in C function moves the file pointer instantly back to the very beginning of the file. Since we already read the file in Step 2 to check the new flight, the pointer is at the bottom. We must rewind it to search from the top! We find the old flight and restore (+1) the seat.
4.  **Deduct New Seat**: `rewind` again. Find the new flight, deduct (-1) the seat, and assign the new seat number to the passenger.
5.  **Update Booking**: Change `b.flight_id` to the new one, and `fwrite` the booking.

---

### 6. `main.c` (The Entry Point)
This file glues everything together into a menu interface.

```c
    setup_files();
```
* Before anything happens, we run our utility to ensure `flights.dat` and `bookings.dat` physically exist on the hard drive.

```c
    while (1) {
        ...
        scanf("%d", &main_choice);
        switch (main_choice) {
```
* **`while(1)`**: An infinite loop. `1` means True in C. The program will run forever until the user selects option 3, which calls `exit(0);`.
* **`switch (main_choice)`**: Evaluates the user's number.
    * `case 1:` Goes to the User Menu.
    * `case 2:` Asks for a password, then goes to the Admin Menu.
    * `default:` If they type '9', it says Invalid Option and loops back.

```c
    do {
        printf("1. View Available Flights\n");
        // ...
        scanf("%d", &user_choice);
        switch(user_choice) {
            case 1: view_available_flights(); break;
            // ...
        }
    } while (user_choice != 7);
```
* **`do-while` loop**: This loop runs the menu *first*, then checks the condition at the bottom. As long as the user doesn't press 7 (Go Back), the User Menu will keep displaying over and over, allowing them to search, book, and view flights endlessly!

---

##  Summary of Your Execution Flow
1. You run `./flight_booking`.
2. `main()` starts. It calls `setup_files()` to touch `.dat` files.
3. The infinite `while(1)` loop prints the Main Menu.
4. If you press `2`, you enter "admin". You enter the Admin Menu (`do-while` loop).
5. You press `1` to Add Flight. Execution jumps to `add_flight()` in `flight.c`.
6. `add_flight()` asks you questions, puts the answers into a `struct Flight`, opens the file in `"ab"`, writes the struct, and returns to the menu.
7. You exit the app. The variables are destroyed from RAM.
8. You start the app tomorrow. You go to User Menu -> Search Flight.
9. `search_flight()` opens the file in `"rb"`, reads the binary data back into a struct, matches your search, and prints it perfectly to the screen!
