#ifndef STRUCTURES_H
#define STRUCTURES_H

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

struct Booking {
    int booking_id;
    char passenger_name[50];
    int flight_id;
    int seat_number;
    int is_cancelled; // 0 = active, 1 = cancelled
};

#endif
