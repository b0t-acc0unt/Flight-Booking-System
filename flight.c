#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flight.h"
#include "structures.h"
#include "utils.h"

void add_flight() {
    struct Flight f;
    struct Flight temp;
    FILE *fp;
    int max_id = 0;

    clear_screen();
    printf("--- ADD NEW FLIGHT ---\n");

    // Automatically generate the next flight ID
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

    printf("Enter Flight Number (e.g. AA101): ");
    scanf(" %19[^\n]", f.flight_num);

    printf("Enter Source City: ");
    scanf(" %49[^\n]", f.source);

    printf("Enter Destination City: ");
    scanf(" %49[^\n]", f.dest);

    printf("Enter Date (DD/MM/YYYY): ");
    scanf(" %19[^\n]", f.date);

    printf("Enter Time (HH:MM): ");
    scanf(" %19[^\n]", f.time);

    printf("Enter Total Seats: ");
    scanf("%d", &f.total_seats);
    f.available_seats = f.total_seats;

    printf("Enter Price: ");
    scanf("%f", &f.price);

    f.is_deleted = 0; // Flight is active

    // Save struct to file
    fp = fopen("flights.dat", "ab");
    if (fp == NULL) {
        printf("Error: Could not open flights file.\n");
        return;
    }
    fwrite(&f, sizeof(struct Flight), 1, fp);
    fclose(fp);

    printf("\nFlight added successfully! New Flight ID is %d\n", f.flight_id);
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void remove_flight() {
    int target_id;
    int found = 0;
    struct Flight f;
    FILE *fp;

    clear_screen();
    printf("--- REMOVE A FLIGHT ---\n");
    printf("Enter Flight ID to remove: ");
    scanf("%d", &target_id);

    fp = fopen("flights.dat", "rb+");
    if (fp == NULL) {
        printf("Error: Could not open flights file.\n");
        return;
    }

    while (fread(&f, sizeof(struct Flight), 1, fp) == 1) {
        if (f.flight_id == target_id && f.is_deleted == 0) {
            f.is_deleted = 1;

            fseek(fp, -sizeof(struct Flight), SEEK_CUR);
            fwrite(&f, sizeof(struct Flight), 1, fp);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (found == 1) {
        printf("\nFlight %d has been removed successfully.\n", target_id);
    } else {
        printf("\nError: Flight ID not found or already removed.\n");
    }

    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void view_available_flights() {
    struct Flight f;
    FILE *fp;
    int count = 0;

    clear_screen();
    printf("--------------------------------------------------------------------------------\n");
    printf("ID\tFLIGHT NO\tSOURCE\t\tDESTINATION\tDATE\t\tSEATS\tPRICE\n");
    printf("--------------------------------------------------------------------------------\n");

    fp = fopen("flights.dat", "rb");
    if (fp == NULL) {
        printf("No flights available.\n");
        return;
    }

    while (fread(&f, sizeof(struct Flight), 1, fp) == 1) {
        if (f.is_deleted == 0 && f.available_seats > 0) {
            printf("%d\t%s\t\t%s\t\t%s\t\t%s\t%d\t%.2f\n",
                   f.flight_id, f.flight_num, f.source, f.dest, f.date, f.available_seats, f.price);
            count++;
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo available flights at the moment.\n");
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void view_all_flights() {
    struct Flight f;
    FILE *fp;
    int count = 0;

    clear_screen();
    printf("--- ALL FLIGHTS (ADMIN VIEW) ---\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("ID\tFLIGHT NO\tSOURCE\t\tDESTINATION\tSEATS\tSTATUS\n");
    printf("--------------------------------------------------------------------------------\n");

    fp = fopen("flights.dat", "rb");
    if (fp == NULL) {
        printf("No flights found.\n");
        return;
    }

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
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo flights exist in the system.\n");
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void search_flight() {
    char search_source[50];
    char search_dest[50];
    char search_date[20];
    struct Flight f;
    FILE *fp;
    int count = 0;

    clear_screen();
    printf("--- SEARCH FOR A FLIGHT ---\n");

    printf("Enter Source City: ");
    scanf(" %49[^\n]", search_source);

    printf("Enter Destination City: ");
    scanf(" %49[^\n]", search_dest);

    printf("Enter Date (DD/MM/YYYY): ");
    scanf(" %19[^\n]", search_date);

    clear_screen();
    printf("--- SEARCH RESULTS ---\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("ID\tFLIGHT NO\tSOURCE\t\tDESTINATION\tDATE\t\tSEATS\tPRICE\n");
    printf("--------------------------------------------------------------------------------\n");

    fp = fopen("flights.dat", "rb");
    if (fp != NULL) {
        while (fread(&f, sizeof(struct Flight), 1, fp) == 1) {
            if (f.is_deleted == 0) {
                // Using the new case-insensitive comparison function
                if (compare(f.source, search_source) == 1 &&
                    compare(f.dest, search_dest) == 1 &&
                    strcmp(f.date, search_date) == 0) {

                    printf("%d\t%s\t\t%s\t\t%s\t\t%s\t%d\t%.2f\n",
                           f.flight_id, f.flight_num, f.source, f.dest, f.date, f.available_seats, f.price);
                    count++;
                    }
            }
        }
        fclose(fp);
    }

    if (count == 0) {
        printf("\nSorry, no flights found matching your search.\n");
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}
