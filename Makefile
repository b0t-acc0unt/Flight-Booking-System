CC = gcc
CFLAGS = -Wall

all:
	$(CC) $(CFLAGS) main.c utils.c flight.c booking.c -o flight_booking.out

clean:
	rm -f flight_booking flights.dat bookings.dat
