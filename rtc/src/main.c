/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <debug.h>

/* Standard headers */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Put function prototypes here */
void printText(const char *text, uint8_t x, uint8_t y);
void printTextSmall(const char *text, uint8_t xpos, uint8_t ypos);
char* itoa(uint32_t num, char* str);

uint16_t months_lookup[] = {
    0,      /* 0th month = 0 days */
    0,      /* 0th month = 0 days */
    31,     /* 1st month = 31 days */
    59,     /* 2nd month = 28 days */
    90,     /* 3rd month = 31 days */
    120,    /* 4th month = 30 days */
    151,    /* 5th month = 31 days */
    181,    /* 6th month = 30 days */
    212,    /* 7th month = 31 days */
    243,    /* 8th month = 31 days */
    273,    /* 9th month = 30 days */
    304,    /* 10th month = 31 days */
    334,    /* 11th month = 30 days */
};

/* Put all your code here */
void main(void) {
    /* uint8_t is an unsigned integer that can range from 0-255. */
    /* It performs faster than just an int, so try to use it (or int8_t) when possible */
    uint8_t count;
    uint8_t day, month, seconds, minutes, hours;
    uint16_t year, day_of_year;
    uint32_t second_of_day, days_since_epoch, epoch_time;
    /* set for UTC-5 right now */
    bool timezone_negative = true;
    uint32_t timezone_compensation = 5 * 3600;

    /* Initialize some strings */
    const char *HelloWorld = "Hello World!";
    const char *Welcome = "Check debug console!";
    char buffer[32];

    dbg_sprintf(dbgout, "------------------------------\n");

    /* Clear the homescreen */
    os_ClrHome();

    /* Print a few strings */
    printText(HelloWorld, 0, 0);
    printText(Welcome, 0, 1);

    /* Read in variables */
    boot_GetDate(&day, &month, &year);
    boot_GetTime(&seconds, &minutes, &hours);
    dbg_sprintf(dbgout, "Values returned: %u-%u-%u %u:%u:%u\n", year, month, day, hours, seconds, minutes);

    day_of_year = months_lookup[month] + day - 1;
    /* todo: proper leap years */
    days_since_epoch = (uint32_t)(year - 1970) * (uint32_t)365 + (uint32_t)day_of_year + (uint32_t)12 /* number of leap years between 1970 and 2018, inclusive */ + (uint32_t)((year - 2018)/4) /* number of leap years between 2018 and now, inclusive */;
    second_of_day = (uint32_t)(hours) * (uint32_t)3600 + (uint32_t)(minutes) * (uint32_t)60 + seconds;
    epoch_time = (days_since_epoch) * (uint32_t)86400 + second_of_day + (uint32_t)27 /* leap seconds between 1970 and 2018 inclusive */;

    (void)itoa(second_of_day, buffer);
    dbg_sprintf(dbgout, "Second of day: %s\n", buffer);
    (void)itoa(days_since_epoch, buffer);
    dbg_sprintf(dbgout, "Days since epoch: %s\n", buffer);

    (void)itoa(epoch_time, buffer);
    dbg_sprintf(dbgout, "Epoch time (pre-compensation): %s\n", buffer);

    if (timezone_negative)
        epoch_time += timezone_compensation;
    else
        epoch_time -= timezone_compensation;

    (void)itoa(epoch_time, buffer);
    dbg_sprintf(dbgout, "Epoch time (post-compensation): %s\n", buffer);
    //printText(buffer, 0, 2);

    /* hang for input */
    //while (!os_GetCSC());
}

/* Draw text on the homescreen at the given X/Y location */
void printText(const char *text, uint8_t xpos, uint8_t ypos) {
    os_SetCursorPos(ypos, xpos);
    os_PutStrFull(text);
}

/* Draw small text at the given X/Y location */
void printTextSmall(const char *text, uint8_t xpos, uint8_t ypos) {
    os_FontSelect(0); // sets small font (1 == big, see docs)
    os_FontDrawText(text, xpos, ypos);
}


/* A utility function to reverse a string  */
void reverse(char str[], uint8_t length) {
    uint8_t start = 0;
    uint8_t end = length -1;
    char tmp;

    while (start < end) {
        tmp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = tmp;
        start++;
        end--;
    }
}
 
// Implementation of itoa()
char* itoa(uint32_t num, char* str) {
    uint8_t i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Process individual digits
    while (num != 0) {
        uint8_t rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }
 
    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}
