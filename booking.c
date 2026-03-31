#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "booking.h"
#include "structures.h"
#include "utils.h"

void book_flight() {
    struct Booking b;
    struct Booking temp_b;
    struct Flight f;
    FILE *fp_flight;
    FILE *fp_book;
    int target_flight_id;
    int flight_found = 0;
    int max_id = 1000;

    clear_screen();
    printf("--- BOOK A FLIGHT ---\n");
    printf("Enter Flight ID you want to book: ");
    scanf("%d", &target_flight_id);

    fp_flight = fopen("flights.dat", "rb+");
    if (fp_flight == NULL) {
        printf("Error: Could not open flights file.\n");
        return;
    }

    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
        if (f.flight_id == target_flight_id && f.is_deleted == 0) {
            flight_found = 1;

            if (f.available_seats <= 0) {
                printf("\nSorry! This flight is completely full.\n");
                fclose(fp_flight);
                printf("Press Enter to continue...\n");
                clear_buffer();
                getchar();
                return;
            }

            f.available_seats = f.available_seats - 1;
            b.seat_number = (f.total_seats - f.available_seats);

            fseek(fp_flight, -sizeof(struct Flight), SEEK_CUR);
            fwrite(&f, sizeof(struct Flight), 1, fp_flight);
            break;
        }
    }
    fclose(fp_flight);

    if (flight_found == 0) {
        printf("\nError: Flight ID not found or removed.\n");
        printf("Press Enter to continue...\n");
        clear_buffer();
        getchar();
        return;
    }

    printf("Enter Passenger Name: ");
    scanf(" %49[^\n]", b.passenger_name);

    b.flight_id = target_flight_id;
    b.is_cancelled = 0;

    fp_book = fopen("bookings.dat", "rb");
    if (fp_book != NULL) {
        while (fread(&temp_b, sizeof(struct Booking), 1, fp_book) == 1) {
            if (temp_b.booking_id > max_id) {
                max_id = temp_b.booking_id;
            }
        }
        fclose(fp_book);
    }
    b.booking_id = max_id + 1;

    fp_book = fopen("bookings.dat", "ab");
    fwrite(&b, sizeof(struct Booking), 1, fp_book);
    fclose(fp_book);

    printf("\nBooking Successful!\n");
    printf("Your Booking ID is: %d\n", b.booking_id);
    printf("Your Seat Number is: %d\n", b.seat_number);

    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

// Automatically loops through all bookings and displays them with flight details
void view_all_booked_flights() {
    struct Booking b;
    struct Flight f;
    FILE *fp_book;
    FILE *fp_flight;
    int count = 0;

    clear_screen();
    printf("--- ALL BOOKED FLIGHTS ---\n");
    printf("-----------------------------------------------------------------------------------------------------\n");
    // Formatted table headers to keep data aligned
    printf("%-10s %-15s %-10s %-15s %-15s %-12s %-5s %-10s\n",
           "BOOK ID", "PASSENGER", "FLIGHT NO", "FROM", "TO", "DATE", "SEAT", "STATUS");
    printf("-----------------------------------------------------------------------------------------------------\n");

    fp_book = fopen("bookings.dat", "rb");
    if (fp_book == NULL) {
        printf("No bookings found in the system.\n");
        printf("Press Enter to continue...\n");
        clear_buffer();
        getchar();
        return;
    }

    while (fread(&b, sizeof(struct Booking), 1, fp_book) == 1) {
        char flight_num[20] = "UNKNOWN";
        char source[50] = "UNKNOWN";
        char dest[50] = "UNKNOWN";
        char date[20] = "UNKNOWN";

        // Read flights file to match the flight data for this specific booking
        fp_flight = fopen("flights.dat", "rb");
        if (fp_flight != NULL) {
            while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
                if (f.flight_id == b.flight_id) {
                    strcpy(flight_num, f.flight_num);
                    strcpy(source, f.source);
                    strcpy(dest, f.dest);
                    strcpy(date, f.date);
                    break;
                }
            }
            fclose(fp_flight);
        }

        char status[15];
        if (b.is_cancelled == 1) {
            strcpy(status, "CANCELLED");
        } else {
            strcpy(status, "CONFIRMED");
        }

        // Using format specifiers to keep everything neatly aligned in terminal columns
        printf("%-10d %-15.15s %-10s %-15.15s %-15.15s %-12s %-5d %-10s\n",
               b.booking_id, b.passenger_name, flight_num, source, dest, date, b.seat_number, status);
        count++;
    }
    fclose(fp_book);

    if (count == 0) {
        printf("\nNo flights have been booked yet.\n");
    }

    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void cancel_booking() {
    struct Booking b;
    struct Flight f;
    FILE *fp_book;
    FILE *fp_flight;
    int target_booking_id;
    int found_booking = 0;
    int flight_to_restore = 0;

    clear_screen();
    printf("--- CANCEL BOOKING ---\n");
    printf("Enter your Booking ID: ");
    scanf("%d", &target_booking_id);

    fp_book = fopen("bookings.dat", "rb+");
    if (fp_book == NULL) {
        printf("Error: Could not open bookings file.\n");
        return;
    }

    while (fread(&b, sizeof(struct Booking), 1, fp_book) == 1) {
        if (b.booking_id == target_booking_id) {
            found_booking = 1;

            if (b.is_cancelled == 1) {
                printf("\nError: This booking is already cancelled.\n");
                fclose(fp_book);
                printf("Press Enter to continue...\n");
                clear_buffer();
                getchar();
                return;
            }

            b.is_cancelled = 1;
            flight_to_restore = b.flight_id;

            fseek(fp_book, -sizeof(struct Booking), SEEK_CUR);
            fwrite(&b, sizeof(struct Booking), 1, fp_book);
            break;
        }
    }
    fclose(fp_book);

    if (found_booking == 0) {
        printf("\nError: Booking ID not found.\n");
        printf("Press Enter to continue...\n");
        clear_buffer();
        getchar();
        return;
    }

    // Restore seat to the flight
    fp_flight = fopen("flights.dat", "rb+");
    if (fp_flight != NULL) {
        while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
            if (f.flight_id == flight_to_restore) {
                f.available_seats = f.available_seats + 1;
                fseek(fp_flight, -sizeof(struct Flight), SEEK_CUR);
                fwrite(&f, sizeof(struct Flight), 1, fp_flight);
                break;
            }
        }
        fclose(fp_flight);
    }

    printf("\nBooking %d cancelled successfully. Your refund will be processed.\n", target_booking_id);
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void reschedule_booking() {
    struct Booking b;
    struct Flight f;
    FILE *fp_book;
    FILE *fp_flight;
    int target_booking_id;
    int new_flight_id;
    int old_flight_id;

    int found_booking = 0;
    int found_new_flight = 0;

    clear_screen();
    printf("--- RESCHEDULE BOOKING ---\n");
    printf("Enter your Booking ID: ");
    scanf("%d", &target_booking_id);

    // Step 1: Find the booking
    fp_book = fopen("bookings.dat", "rb+");
    if (fp_book == NULL) {
        printf("Error: Could not open bookings file.\n");
        return;
    }

    while (fread(&b, sizeof(struct Booking), 1, fp_book) == 1) {
        if (b.booking_id == target_booking_id) {
            found_booking = 1;
            break;
        }
    }

    if (found_booking == 0 || b.is_cancelled == 1) {
        printf("\nError: Booking ID not found or is cancelled.\n");
        fclose(fp_book);
        printf("Press Enter to continue...\n");
        clear_buffer();
        getchar();
        return;
    }

    old_flight_id = b.flight_id;

    printf("Enter the NEW Flight ID you want to change to: ");
    scanf("%d", &new_flight_id);

    if (old_flight_id == new_flight_id) {
        printf("\nError: You are already booked on this flight.\n");
        fclose(fp_book);
        printf("Press Enter to continue...\n");
        clear_buffer();
        getchar();
        return;
    }

    // Step 2: Ensure new flight exists and has seats
    fp_flight = fopen("flights.dat", "rb+");
    if (fp_flight == NULL) {
        fclose(fp_book);
        return;
    }

    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
        if (f.flight_id == new_flight_id && f.is_deleted == 0) {
            found_new_flight = 1;

            if (f.available_seats <= 0) {
                printf("\nSorry, the new flight is completely full.\n");
                fclose(fp_flight);
                fclose(fp_book);
                printf("Press Enter to continue...\n");
                clear_buffer();
                getchar();
                return;
            }
            break;
        }
    }

    if (found_new_flight == 0) {
        printf("\nError: New Flight ID not found.\n");
        fclose(fp_flight);
        fclose(fp_book);
        printf("Press Enter to continue...\n");
        clear_buffer();
        getchar();
        return;
    }

    // Step 3: Revert/Restore the seat on the OLD flight
    rewind(fp_flight);
    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
        if (f.flight_id == old_flight_id) {
            f.available_seats = f.available_seats + 1;
            fseek(fp_flight, -sizeof(struct Flight), SEEK_CUR);
            fwrite(&f, sizeof(struct Flight), 1, fp_flight);
            break;
        }
    }

    // Step 4: Deduct the seat on the NEW flight
    rewind(fp_flight);
    while (fread(&f, sizeof(struct Flight), 1, fp_flight) == 1) {
        if (f.flight_id == new_flight_id) {
            f.available_seats = f.available_seats - 1;
            b.seat_number = (f.total_seats - f.available_seats); // Assign new seat number
            fseek(fp_flight, -sizeof(struct Flight), SEEK_CUR);
            fwrite(&f, sizeof(struct Flight), 1, fp_flight);
            break;
        }
    }
    fclose(fp_flight);

    // Step 5: Update the booking record with new flight info
    b.flight_id = new_flight_id;
    fseek(fp_book, -sizeof(struct Booking), SEEK_CUR);
    fwrite(&b, sizeof(struct Booking), 1, fp_book);
    fclose(fp_book);

    printf("\nReschedule Successful!\n");
    printf("Your new Flight ID is: %d\n", b.flight_id);
    printf("Your new Seat Number is: %d\n", b.seat_number);

    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}

void view_all_bookings() {
    struct Booking b;
    FILE *fp;
    int count = 0;

    clear_screen();
    printf("--- ALL BOOKINGS (ADMIN VIEW) ---\n");
    printf("----------------------------------------------------------------------\n");
    printf("BOOKING ID\tPASSENGER NAME\t\tFLIGHT ID\tSEAT\tSTATUS\n");
    printf("----------------------------------------------------------------------\n");

    fp = fopen("bookings.dat", "rb");
    if (fp == NULL) {
        printf("No bookings found.\n");
        return;
    }

    while (fread(&b, sizeof(struct Booking), 1, fp) == 1) {
        char status[15];
        if (b.is_cancelled == 1) {
            strcpy(status, "CANCELLED");
        } else {
            strcpy(status, "CONFIRMED");
        }

        printf("%d\t\t%s\t\t\t%d\t\t%d\t%s\n",
               b.booking_id, b.passenger_name, b.flight_id, b.seat_number, status);
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo bookings exist in the system.\n");
    }

    printf("----------------------------------------------------------------------\n");
    printf("Press Enter to continue...\n");
    clear_buffer();
    getchar();
}
