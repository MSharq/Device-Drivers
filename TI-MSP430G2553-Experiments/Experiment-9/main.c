/***************************************************************************//**
 * @file    main.c
 * @author  Mohammad Sharique
 * @date    11/05/2019
 *
 * @brief   Exercise - Display Interface
 *
 * Pin connection description
 * --------------------------
 * CON5 D7 -- CON3 P2.3
 * CON5 D6 -- CON3 P2.2
 * CON5 D5 -- CON3 P2.1
 * CON5 D4 -- CON3 P2.0
 * CON5 E -- CON4 P3.2
 * CON5 R/W -- CON4 P3.1
 * CON5 RS -- CON4 P3.0
 *
 * @note    The project was exported using CCS 8.0.0.
 ******************************************************************************/

#include "libs/templateEMP.h"
#include "libs/LCD.h"

int main(void) {
    initMSP();
    WDTCTL = WDTPW + WDTHOLD; // stop watchdog timer

    //Set MCLK = SMCLK = 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    initTimer_A();
    _enable_interrupt();

    // initializing lcd
    lcd_init();

    while (1) {
        //a funky demo of what I did.

        lcd_clear(); // clear screen
        lcd_cursorSet(1,1); // row 1 column 1
        lcd_putText("mohammad");
        delayMS(2000);

        lcd_cursorSet(2,1); // row 2 column 1
        lcd_putText("sharque");
        delayMS(2000);

        lcd_clear();
        delayMS(1000);

        lcd_cursorShow(0);
        lcd_cursorBlink(0);
        delayMS(2000);

        lcd_cursorShow(1);
        lcd_cursorBlink(1);
        delayMS(2000);

        lcd_cursorSet(1,1); // row 1 column 1
        lcd_putText("Freiburg");
        delayMS(2000);
        lcd_scrollDisplayRight(); // scrolling the Display towards Right
        delayMS(2000);
        lcd_scrollDisplayRight();
        delayMS(2000);
        lcd_scrollDisplayLeft(); // scrolling the Display towards Left
        delayMS(2000);
        lcd_scrollDisplayLeft();
        delayMS(2000);

        lcd_cursorSet(2,10); // row 1 column 1
        lcd_displayRightToLeft(); // changing the display and making it print from right to left
        lcd_putText("grubierF");  // so this prints as "Freiburg"

        delayMS(2000);
        lcd_scrollDisplayRight(); // scrolling the Display towards Right
        delayMS(2000);
        lcd_scrollDisplayRight();
        delayMS(2000);
        lcd_scrollDisplayLeft(); // scrolling the Display towards Left
        delayMS(2000);
        lcd_scrollDisplayLeft();
        delayMS(2000);

        lcd_displayLeftToRight(); // changing the display print to usual from left to right

        lcd_clear();
        delayMS(1000);

        // two custom characters
        custom_character();
        delayMS(3000);

        lcd_clear();
        delayMS(1000);

        // displaying +ve and -ve numbers
        lcd_cursorSet(1,1);
        lcd_putNumber(32);
        delayMS(2000);

        lcd_cursorSet(2,1);
        lcd_putNumber(-23);
        delayMS(2000);
    }
}
