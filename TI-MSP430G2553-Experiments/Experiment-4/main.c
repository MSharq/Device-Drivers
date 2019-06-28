/**
 * Author - Mohammad Sharique
 */

#include <msp430.h>
#include <templateEMP.h>

void clear_all()
{
    // clear/reset the shift register, i.e turn all LEDs off
    P2OUT &= ~BIT5;
    // stop the reset mode (/CLR = 1)
    P2OUT |= BIT5;
    // reset the clock signal to 0
    P2OUT &= ~BIT4;
}

void clk_high_low(int clk)
{
    while(clk != 0)
    {
        // clock signal high, send data IN
        P2OUT |= BIT4;
        // reset the clock
        P2OUT &= ~ BIT4;
        --clk;
    }
}

void no_change_mode_led()
{
    // set the shift register 2 mode in NO CHANGE mode, For Leds
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT1;
}

void right_shift_mode_led()
{
    // set the shift register 2 mode in RIGHT SHIFT mode, For Leds
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
}

int color_check(int val1, int val2, int val3)
{
    if ((val1 >= 137 && val1 <= 175) && (val2 >= 80 && val2 <= 100) && (val3 >= 70 && val3 <= 100)) {
        // Red qualified
        return 1;
    } else if ((val1 >= 70 && val1 <= 95) && (val2 >= 120 && val2 <= 140) && (val3 >= 60 && val3 <= 82)) {
        // Green qualified
        return 2;
    } else if ((val1 >= 70 && val1 <= 95) && (val2 >= 100 && val2 <= 115) && (val3 >= 115 && val3 <= 135)) {
        // Blue qualified
        return 3;
    } else if ((val1 >= 60 && val1 <= 90) && (val2 >= 60 && val2 <= 95) && (val3 >= 60 && val3 <= 90)) {
        // Black qualified
        return 5;
    } else if ((val1 >= 190) && (val2 >= 190) && (val3 >= 190)) {
        // Silver qualified (I used an Aluminium-foil wrapped around 1 Euro coin to get the Silver color)
        return 6;
    }
    else {
        if(val1 > val3 && val1 > val2) {
            if((val1 - val3) <= 7 && (val1 - val2) <= 7) {
                return 0;
            } else {
                // White qualified
                return 4;
            }
        } else {
            if((val3 - val1) <= 7 && (val2 - val1) <= 7) {
                return 0;
            } else {
                // White qualified
                return 4;
            }
        }
    }
}

/**
 * main.c
 */
int main(void)
{
    unsigned int val[3], count = 0;
    initMSP ();

    // modify XTAL pin to be I/O
    P2SEL &= ~(BIT6 | BIT7);

    // set pin direction as Output for required pins on port2
    P2DIR |= (BIT0 | BIT1 | BIT4 | BIT5 | BIT6);

    // set pin direction as Output for required pins on port3
    P3DIR |=(BIT0 | BIT1 | BIT2);

    P2OUT = 0x00;
    P3OUT = 0x00;
    clear_all();
    clk_high_low(1);

    // Turn ADC on; use 16 clocks as sample & hold time
    ADC10CTL0 = ADC10ON + ADC10SHT_2;

    // Enable P1.4 & P1 .7 as AD input, 1.7 for the Potentiometer
    ADC10AE0 |= (BIT4 | BIT7);

    while (1)
    {
        // Select channel 7 as input for the Potentiometer value reading
        ADC10CTL1 = INCH_7;

        // Start conversion
        ADC10CTL0 |= ENC + ADC10SC;

        // Wait until result is ready
        while ( ADC10CTL1 & ADC10BUSY );
        clear_all();

        // If result is ready , copy it to m1
        int m1 = ADC10MEM;

        // the division I am taking is 205 steps per division
        // so for division 0-205
        if (m1 <= 205) {
            // nothing to do
        }
        // division 206-410
        else if (m1 >= 206 && m1 <= 410) {
            // switching ON D1
            right_shift_mode_led();
            P2OUT |= BIT6;
            clk_high_low(1);
            no_change_mode_led();
        }
        // division 411-615
        else if (m1 >= 411 && m1 <= 615) {
            // switching ON D1 D2
            right_shift_mode_led();
            P2OUT |= BIT6;
            clk_high_low(1);
            P2OUT |= BIT6;
            clk_high_low(1);
            no_change_mode_led();
        }
        // division 616-820
        else if (m1 >= 616 && m1 <= 824) {
            // switching ON D1 D2 D3
            right_shift_mode_led();
            P2OUT |= BIT6;
            clk_high_low(1);
            P2OUT |= BIT6;
            clk_high_low(1);
            P2OUT |= BIT6;
            clk_high_low(1);
            no_change_mode_led();
        }
        // division 821-1023
        else {
            // switching ON D1 D2 D3 D4
            right_shift_mode_led();
            P2OUT |= BIT6;
            clk_high_low(1);
            P2OUT |= BIT6;
            clk_high_low(1);
            P2OUT |= BIT6;
            clk_high_low(1);
            P2OUT |= BIT6;
            clk_high_low(1);
            //__delay_cycles(2500000);
            no_change_mode_led();
        }

        // Stop conversion
        ADC10CTL0 &= ~ENC;

        // LDR evaluation
        while (count != 3) {

            if (count == 0) {
                // Red led ON
                P3OUT = BIT0;
                __delay_cycles(500000);
            } else if (count == 1) {
                // Green led ON
                P3OUT = BIT1;
                __delay_cycles(500000);
            } else if (count == 2) {
                // Blue led ON
                P3OUT = BIT2;
                __delay_cycles(500000);
            }

            // Select channel 4 as input for the LDR analog value reading
            ADC10CTL1 = INCH_4;

            // Start conversion
            ADC10CTL0 |= ENC + ADC10SC;

            // Wait until result is ready
            while ( ADC10CTL1 & ADC10BUSY );

            P3OUT |= BIT0;

            val[count] = ADC10MEM;

            count++;
            // Stop conversion
            ADC10CTL0 &= ~ENC;
        }

        count = 0;

        m1 = color_check(val[0], val[1], val[2]);

        if (m1 == 1) {
            serialPrint("--Red--");
        } else if (m1 == 2) {
            serialPrint("--Green--");
        } else if (m1 == 3) {
            serialPrint("--Blue--");
        } else if (m1 == 4) {
            serialPrint("--White--");
        } else if (m1 == 5) {
            serialPrint("--Black--");
        } else if (m1 == 6) {
            // Bonus task of recognizing another color
            serialPrint("--Silver--");
        } else {
            serialPrint("--Not Recognized-- OR --No CHIP--");
        }
    }
	
	return 0;
}
