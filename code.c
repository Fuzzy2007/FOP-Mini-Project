#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_BUSES 10
#define MAX_BOOKINGS 50
#define MAX_USERS 20

// Structure to store bus information
struct Bus {
    int bus_id;
    char from_city[30];
    char to_city[30];
    char dep_time[10];
    char arr_time[10];
    char driver[30];
    int driver_rating;
    int route_safety;
    char current_route[50];
    int total_seats;
    int available_seats;
};

// Structure to store user booking information
struct Booking {
    int booking_id;
    int bus_id;
    char phone[15];
    char email[50];
    char passenger_name[30];
    int seat_number;
    char booking_time[20];
    int status; // 1=confirmed, 0=cancelled
};

// Structure to store registered user
struct User {
    char phone[15];
    char email[50];
    char name[30];
};

// Global arrays
struct Bus buses[MAX_BUSES];
struct Booking bookings[MAX_BOOKINGS];
struct User users[MAX_USERS];
int booking_count = 0;
int user_count = 0;

// Converts any string to lowercase for case-insensitive comparison
void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Compares two strings case-insensitively
int strcasecmp_custom(const char* s1, const char* s2) {
    char temp1[50], temp2[50];
    strcpy(temp1, s1);
    strcpy(temp2, s2);
    toLowerCase(temp1);
    toLowerCase(temp2);
    return strcmp(temp1, temp2);
}

// Extracts hour from time string
int getHourFromTime(char* time_str) {
    int hour;
    sscanf(time_str, "%d", &hour);
    return hour;
}

// Determines if travel is at night
int isNightTravel(char* dep_time, char* arr_time) {
    int dep_hour = getHourFromTime(dep_time);
    int arr_hour = getHourFromTime(arr_time);
    return (dep_hour >= 20 || arr_hour >= 20);
}

// Calculates total safety score
int calculateAdjustedSafety(struct Bus b) {
    int base_safety = b.driver_rating + b.route_safety;
    if (isNightTravel(b.dep_time, b.arr_time)) {
        return base_safety - 2;
    }
    return base_safety;
}

// Simulate sending SMS/Email confirmation
void sendTicketConfirmation(struct Booking* booking, struct Bus* bus) {
    printf("\n📱 SMS SENT TO: %s\n", booking->phone);
    printf("📧 EMAIL SENT TO: %s\n", booking->email);
    printf("========================================\n");
    printf("🎫 TICKET CONFIRMED!\n");
    printf("Booking ID: %d\n", booking->booking_id);
    printf("Bus ID: %d (%s → %s)\n", bus->bus_id, bus->from_city, bus->to_city);
    printf("Passenger: %s\n", booking->passenger_name);
    printf("Seat: %d\n", booking->seat_number);
    printf("Departure: %s\n", bus->dep_time);
    printf("Status: CONFIRMED ✅\n");
    printf("========================================\n");
    printf("Thank you for booking with SmartBus!\n\n");
}

// Register new user
int registerUser(char* phone, char* email, char* name) {
    // Check if user already exists
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].phone, phone) == 0 || strcmp(users[i].email, email) == 0) {
            printf("User already registered!\n");
            return i; // Return existing user index
        }
    }
    
    // Add new user
    strcpy(users[user_count].phone, phone);
    strcpy(users[user_count].email, email);
    strcpy(users[user_count].name, name);
    printf("✅ User registered successfully!\n");
    return user_count++;
}

// Book a bus seat
void bookBus(int bus_index, char* phone, char* email, char* passenger_name) {
    if (buses[bus_index].available_seats <= 0) {
        printf("❌ No seats available!\n");
        return;
    }
    
    // Register user if new
    registerUser(phone, email, passenger_name);
    
    // Create booking
    struct Booking new_booking;
    new_booking.booking_id = 1000 + booking_count;
    new_booking.bus_id = buses[bus_index].bus_id;
    strcpy(new_booking.phone, phone);
    strcpy(new_booking.email, email);
    strcpy(new_booking.passenger_name, passenger_name);
    new_booking.seat_number = buses[bus_index].total_seats - buses[bus_index].available_seats + 1;
    strcpy(new_booking.booking_time, __TIME__);
    new_booking.status = 1;
    
    // Add to bookings
    bookings[booking_count] = new_booking;
    
    // Update bus seats
    buses[bus_index].available_seats--;
    
    // Send confirmation
    sendTicketConfirmation(&new_booking, &buses[bus_index]);
    
    booking_count++;
}

// Cancel booking
void cancelBooking(int booking_id) {
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].booking_id == booking_id && bookings[i].status == 1) {
            // Find corresponding bus
            for (int j = 0; j < MAX_BUSES; j++) {
                if (buses[j].bus_id == bookings[i].bus_id) {
                    buses[j].available_seats++;
                    break;
                }
            }
            bookings[i].status = 0;
            printf("✅ Booking %d cancelled successfully.\n", booking_id);
            printf("💰 Refund processed to %s\n", bookings[i].phone);
            return;
        }
    }
    printf("❌ Booking ID not found or already cancelled!\n");
}

// Initialize buses with seat information
void initBuses() {
    struct Bus bus_data[MAX_BUSES] = {
        {101, "Pune", "Mumbai", "06:30", "10:30", "Raj", 4, 4, "Pune-NH48-Khandala", 40, 35},
        {102, "Pune", "Mumbai", "09:00", "13:30", "Anil", 3, 2, "Pune-NH48-Lonavala", 40, 38},
        {103, "Pune", "Nashik", "07:00", "11:00", "Vijay", 5, 4, "Pune-Sangamner-Nashik", 35, 30},
        {104, "Mumbai", "Pune", "21:00", "01:00", "Suresh", 4, 4, "Mumbai-NH48-Panvel", 40, 32},
        {105, "Pune", "Kolhapur", "08:00", "12:00", "Kiran", 3, 3, "Pune-Satara-Kolhapur", 45, 40},
        {106, "Nashik", "Pune", "22:00", "02:00", "Ramesh", 4, 3, "Nashik-Sangamner-Pune", 35, 28},
        {107, "Pune", "Mumbai", "14:00", "18:00", "Mahesh", 4, 4, "Pune-NH48-Khopoli", 40, 36},
        {108, "Pune", "Nashik", "15:00", "19:00", "Sunil", 3, 3, "Pune-Shirdi-Nashik", 35, 31},
        {109, "Mumbai", "Nashik", "10:00", "15:00", "Ajay", 2, 2, "Mumbai-NashikRd-Igatpuri", 40, 25},
        {110, "Kolhapur", "Pune", "16:00", "20:00", "Prakash", 4, 3, "Kolhapur-Satara-Pune", 45, 42}
    };
    for (int i = 0; i < MAX_BUSES; i++) {
        buses[i] = bus_data[i];
    }
}

// Validates if city exists
int cityExists(char* city) {
    for (int i = 0; i < MAX_BUSES; i++) {
        if (strcasecmp_custom(buses[i].from_city, city) == 0 || 
            strcasecmp_custom(buses[i].to_city, city) == 0) {
            return 1;
        }
    }
    return 0;
}

// Emergency SOS
void emergencySOS(int bus_id, char* current_location) {
    printf("\n 🚨 EMERGENCY SOS ACTIVATED! 🚨 \n");
    printf("========================================\n");
    printf(" 🔴 ALERT SENT TO EMERGENCY CONTACTS\n");
    printf(" 🚌 Bus ID: %d\n", bus_id);
    printf(" 📍 Current Location: %s\n", current_location);
    printf(" ⏰ Time: %s IST\n", __TIME__);
    printf("========================================\n");
    printf(" ✅ SOS Message Sent Successfully!\n");
    printf(" Help is on the way! Stay safe.\n");
}

int main() {
    initBuses();
    int choice, selected_bus = -1;
    
    printf("=== 🚌 SMART BUS FINDER & BOOKING SYSTEM ===\n");
    
    while (1) {
        printf("\n1. 🔍 Find bus BETWEEN cities\n");
        printf("2. 📋 Show ALL routes FROM city\n");
        printf("3. 🎫 BOOK Ticket\n");
        printf("4. ❌ Cancel Booking\n");
        printf("5. 🚨 EMERGENCY SOS\n");
        printf("6. 🚪 Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        if (choice == 6) break;
        
        if (choice == 5) {
            printf("Enter your Bus ID: ");
            scanf("%d", &selected_bus);
            for (int i = 0; i < MAX_BUSES; i++) {
                if (buses[i].bus_id == selected_bus) {
                    emergencySOS(buses[i].bus_id, buses[i].current_route);
                    break;
                }
            }
            continue;
        }
        
        if (choice != 1 && choice != 2 && choice != 3 && choice != 4) {
            printf("❌ Invalid choice!\n");
            continue;
        }
        
        char source[30], destination[30];
        printf("Enter source city: ");
        scanf("%s", source);
        if (!cityExists(source)) {
            printf("❌ No such city '%s' in dataset!\n", source);
            continue;
        }
        
        if (choice == 1 || choice == 3) {
            printf("Enter destination city: ");
            scanf("%s", destination);
            if (!cityExists(destination)) {
                printf("❌ No such city '%s' in dataset!\n", destination);
                continue;
            }
        }
        
        if (choice == 1) {
            // Find buses (existing functionality)
            printf("\n%s → %s:\n", source, destination);
            printf("ID  Seats Driver  Time    Safety\n");
            printf("-----------------------------------\n");
            
            int found_count = 0;
            for (int i = 0; i < MAX_BUSES; i++) {
                if (strcasecmp_custom(buses[i].from_city, source) == 0 && 
                    strcasecmp_custom(buses[i].to_city, destination) == 0) {
                    int safety = calculateAdjustedSafety(buses[i]);
                    printf("%-3d %2d/%-2d %-8s %-8s %2d/10\n",
                           buses[i].bus_id, buses[i].total_seats - buses[i].available_seats,
                           buses[i].total_seats, buses[i].driver, buses[i].dep_time, safety);
                    found_count++;
                }
            }
            if (!found_count) printf("\n❌ No direct buses found.\n");
            
        } else if (choice == 2) {
            // Show all routes from city
            printf("\nAll routes from %s:\n", source);
            printf("ID  To          Driver  Time    Seats\n");
            printf("----------------------------------------\n");
            
            int found_count = 0;
            for (int i = 0; i < MAX_BUSES; i++) {
                if (strcasecmp_custom(buses[i].from_city, source) == 0) {
                    printf("%-3d %-11s %-8s %-8s %2d/%2d\n",
                           buses[i].bus_id, buses[i].to_city, buses[i].driver,
                           buses[i].dep_time, buses[i].available_seats, buses[i].total_seats);
                    found_count++;
                }
            }
            if (!found_count) printf("\n❌ No buses found from %s.\n", source);
            
        } else if (choice == 3) {
            // Book ticket
            printf("\nAvailable buses %s → %s:\n", source, destination);
            for (int i = 0; i < MAX_BUSES; i++) {
                if (strcasecmp_custom(buses[i].from_city, source) == 0 && 
                    strcasecmp_custom(buses[i].to_city, destination) == 0) {
                    printf("%d. Bus %d - %s (%d/%d seats left)\n", i, buses[i].bus_id, 
                           buses[i].driver, buses[i].available_seats, buses[i].total_seats);
                }
            }
            
            printf("Enter bus number (index 0-%d): ", MAX_BUSES-1);
            int bus_num;
            scanf("%d", &bus_num);
            if (bus_num >= 0 && bus_num < MAX_BUSES && buses[bus_num].available_seats > 0 &&
                strcasecmp_custom(buses[bus_num].from_city, source) == 0 &&
                strcasecmp_custom(buses[bus_num].to_city, destination) == 0) {
                
                char phone[15], email[50], name[30];
                printf("Enter phone number: ");
                scanf("%s", phone);
                printf("Enter email: ");
                scanf("%s", email);
                printf("Enter passenger name: ");
                scanf("%s", name);
                
                bookBus(bus_num, phone, email, name);
            } else {
                printf("❌ Invalid bus selection!\n");
            }
            
        } else if (choice == 4) {
            printf("Enter Booking ID to cancel: ");
            int booking_id;
            scanf("%d", &booking_id);
            cancelBooking(booking_id);
        }
    }
    
    printf("\n👋 Thank you for using SmartBus! Drive Safe 🚌\n");
    return 0;
}
