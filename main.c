#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flight.h"
#include "booking.h"
#include "utils.h"

int main() {
    int main_choice;
    int user_choice;
    int admin_choice;
    char password[20];

    // Guarantee data files exist when the program starts
    setup_files();

    while (1) {
        clear_screen();
        printf("==========================================\n");
        printf("        FLIGHT BOOKING SYSTEM             \n");
        printf("==========================================\n");
        printf("1. User Menu\n");
        printf("2. Admin Menu\n");
        printf("3. Exit Program\n");
        printf("==========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &main_choice);

        switch (main_choice) {
            case 1:
                do {
                    clear_screen();
                    printf("========== USER MENU ==========\n");
                    printf("1. View Available Flights\n");
                    printf("2. Search Flight\n");
                    printf("3. Book a Flight\n");
                    printf("4. View All Booked Flights\n");
                    printf("5. Cancel Booking\n");
                    printf("6. Reschedule Booking\n");
                    printf("7. Go Back to Main Menu\n");
                    printf("Enter choice: ");
                    scanf("%d", &user_choice);

                    switch (user_choice) {
                        case 1: view_available_flights(); break;
                        case 2: search_flight(); break;
                        case 3: book_flight(); break;
                        case 4: view_all_booked_flights(); break;
                        case 5: cancel_booking(); break;
                        case 6: reschedule_booking(); break;
                        case 7: break; // Exits the inner do-while loop
                        default:
                            printf("\nInvalid choice! Please try again.\n");
                            clear_buffer();
                            getchar();
                    }
                } while (user_choice != 7);
                break;

                        case 2:
                            clear_screen();
                            printf("Enter Admin Password: ");
                            scanf(" %19s", password);

                            // Simple hardcoded password
                            if (strcmp(password, "admin") == 0) {
                                do {
                                    clear_screen();
                                    printf("========= ADMIN MENU =========\n");
                                    printf("1. Add New Flight\n");
                                    printf("2. Remove Flight\n");
                                    printf("3. View All Flights\n");
                                    printf("4. View All Bookings\n");
                                    printf("5. Go Back to Main Menu\n");
                                    printf("Enter choice: ");
                                    scanf("%d", &admin_choice);

                                    switch (admin_choice) {
                                        case 1: add_flight(); break;
                                        case 2: remove_flight(); break;
                                        case 3: view_all_flights(); break;
                                        case 4: view_all_bookings(); break;
                                        case 5: break;
                                        default:
                                            printf("\nInvalid choice! Please try again.\n");
                                            clear_buffer();
                                            getchar();
                                    }
                                } while (admin_choice != 5);
                            } else {
                                printf("\nWrong Password! Access Denied.\n");
                                clear_buffer();
                                getchar();
                            }
                            break;

                                        case 3:
                                            printf("\nThank you for using the Flight Booking System! Goodbye.\n");
                                            exit(0);
                                            break;

                                        default:
                                            printf("\nInvalid option! Please enter 1, 2, or 3.\n");
                                            clear_buffer();
                                            getchar();
        }
    }
    return 0;
}
