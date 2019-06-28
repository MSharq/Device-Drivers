/**
 * Author - Mohammad Sharique
 */

#include <msp430.h>
#include <templateEMP.h>
/**
 * main.c
 */
int main(void)
{
    initMSP();
	// sets the direction register for pin 1.4 & 1.5 to output
    P1DIR |= 0x30;
    P1OUT = 0x00;
    // i a variable for the loop
    volatile unsigned int i;

    while(1)
    {
        // Red led: f = 2 hz T = 0.5 sec
        // Red led: duty cycle = 50%, so 0.25 secs ON, 0.25 secs OFF
        // Blue led: half the Red led frequency, f = 1 hz, T = 1 sec, 50% duty cycle
        __delay_cycles(250000);
        P1OUT ^= 0x10;          // toggle P1.4 Red led

        __delay_cycles(250000);
        P1OUT ^= 0x30;          // toggle P1.4 & P1.5, Red & Blue led
        /* the pin for D7 blue led can also control HEATER.
         * To make it work, we need to short at JP4 pin 1 and pin 2.
         */
        serialPrint(" Led status ");

        // checking user input from the Hterm
        if(serialAvailable())
        {
            serialPrint(" Led status ");
            if(P1OUT == 0x00)
            {
                serialPrint("OFF! ");
            }
            else
            {
                serialPrint("ON! ");
            }
            // flushing the ring buffer
            serialFlush();
        }
    }

	return 0;
}
