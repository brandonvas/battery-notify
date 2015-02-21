#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CRITICAL 10
#define LOW      15
#define BATTERY_STRING_LENGTH 40
#define UPDATE_RATE 10 // In seconds
#define NOTIFICATION_TIMEOUT 5000 // In ms, with a minimum of 1 second

struct Battery {
    int status;             // 0 = discharging, 1 = charging
    float energy_now;         // current energy level
    float energy_full_design; // manufactured energy level
    float energy_full;        // energy level during last charge
};

int get_ith_split(char * string, char * delimiter, int ith, char * str_result) {
    int i;
    char * pch;

    pch = strtok(string, delimiter);
    if(pch == NULL) return -1;

    for (i = ith; i > 0; i--) {
        pch = strtok(NULL, delimiter);
        if(pch == NULL) return -1;
    }
    strncpy(str_result, pch, 100);
    return 0;
}

int get_battery_info(struct Battery * batt) {

    FILE *fp;
    fp = fopen("/sys/class/power_supply/BAT0/uevent", "r");

    char *line;
    size_t nbytes = 0;
    char result[100];

    // Gets the first line which stores name
    getline(&line,&nbytes,fp);

    // Gets the next line which stores the status
    getline(&line,&nbytes,fp);
    get_ith_split(line, "=", 1, result);

    if(strcmp(result, "Charging\n") == 0)
        batt->status = 1;
    else batt->status = 0;

    // Is power supply present
    getline(&line,&nbytes,fp);

    // Power supply technology
    getline(&line,&nbytes,fp);

    // Power supply cycle count
    getline(&line,&nbytes,fp);

    // Power supply min voltage design
    getline(&line,&nbytes,fp);

    // Power supply voltage now
    getline(&line,&nbytes,fp);

    // Power supply power now
    getline(&line,&nbytes,fp);

    // Power supply full design energy
    getline(&line,&nbytes,fp);
    get_ith_split(line, "=", 1, result);

    batt->energy_full_design = (float) atoi(result);

    // Power supply energy when full
    getline(&line,&nbytes,fp);
    get_ith_split(line, "=", 1, result);

    batt->energy_full = (float) atoi(result);

    // Power supply energy now
    getline(&line,&nbytes,fp);
    get_ith_split(line, "=", 1, result);

    batt->energy_now = (float) atoi(result);

    // Power supply capacity
    getline(&line,&nbytes,fp);

    // Power supply capacity level
    getline(&line,&nbytes,fp);

    //Power supply model name
    getline(&line,&nbytes,fp);

    // Power supply manufacturer
    getline(&line,&nbytes,fp);

    // Power supply serial number
    getline(&line,&nbytes,fp);

    free(line);
    return 0;
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

    float battery_percent = 0;
    struct Battery batt;

    while(1) {
        get_battery_info(&batt);
        battery_percent = (batt.energy_now/batt.energy_full_design)*100;

        printf("%f\n", battery_percent);

        if (batt.status == 1);
        else if (battery_percent < CRITICAL) {
            send_notification(CRITICAL, 5000);}
        else if (battery_percent < LOW)
            send_notification(LOW, 5000);

        sleep(UPDATE_RATE);
    }

    return 0;
}
