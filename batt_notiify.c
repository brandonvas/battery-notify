#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CRITICAL 10
#define LOW      20
#define BATTERY_STRING_LENGTH 40
#define UPDATE_RATE 10 // In seconds
#define NOTIFICATION_TIMEOUT 5000 // In ms, with a minimum of 1 second

int get_battery_string(char * battery_string) {
    FILE *fp;
    fp = popen("/usr/bin/acpi", "r");
    if (!fp) {
        fprintf(stderr, "Unable to execute acpi command from battery notifier");
        return 1;
    }

    fgets(battery_string, BATTERY_STRING_LENGTH, fp);
    pclose(fp);
    return 0;
}

// Parses the battery string to get the battery percentage
int get_battery_percent(char * battery_string) {

    char * pch;
    pch = strtok(battery_string, " ");

    // get the battery percentage, always printed 4th
    int i = 0;
    for (; i < 3; i++) {
        pch = strtok(NULL, " ");
    }
    pch = strtok(pch, "%");

    return atoi(pch);
}

// Used to create the notification command which is sent via system
void send_notification(int battery_level, int timeout) {
    char command[100];
    char * message;

    switch(battery_level) {
        case LOW:
            message = "'Battery Notification' 'Battery level is LOW, charge soon'";
            break;
        case CRITICAL:
            message = "'Battery Notification' 'Battery level is CRITICAL, charge no!'";
            break;
        default: 
            message = "'Battery Notification' 'Battery is nominal'";
            break;
    }

    snprintf(command, 100, "notify-send -u critical -t %i %s", timeout, message);
    system(command);
}


int main(int argc, char * argv[]) {

    char battery_string[1023];
    int battery_percent = 0;

    while(1) {
        if (get_battery_string(battery_string)) return 1;

        battery_percent = get_battery_percent(battery_string);
        if(battery_string < 0) {
            return 1;
        }

        if (battery_percent < LOW)
            send_notification(LOW, 5000);
        else if (battery_percent < CRITICAL)
            send_notification(CRITICAL, 5000);

        sleep(UPDATE_RATE);
    }

    return 0;


}
