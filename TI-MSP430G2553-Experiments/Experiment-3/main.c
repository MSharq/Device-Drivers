/**
 * Author - Mohammad Sharique
 */

//#include <msp430.h>
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

int button_state(int led_pos)
{
    int Qd[4], count;

    // set the shift register 2 mode in NO CHANGE mode, For Leds
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT1;

    // set the SHIFT register 1 mode to RIGHT SHIFT mode, For Buttons
    P2OUT |= BIT2;
    P2OUT &= ~BIT3;

    for(count=3;count>=0;count--)
    {
        Qd[count] = ((P2IN & BIT7) == BIT7);
        P2OUT &= ~BIT6;
        clk_high_low(1);
    }

    // set the shift register 1 mode to PARALLEL mode, For Buttons
    P2OUT |= BIT2;
    P2OUT |= BIT3;

    // set the shift register 2 mode in RIGHT SHIFT mode, For Leds
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;

    return 1000*Qd[3]+100*Qd[2]+10*Qd[1]+Qd[0];
}

int delay_value(Qd_final)
{
    // rewind or fastforward
    if (Qd_final == 1 || Qd_final == 1000) {
        return 125;
    }
    // play
    else if (Qd_final == 100) {
        return 250;
    }

    // pause
    return 0;
}

void delay_cycles(int delay)
{
    if (delay == 125) {
        __delay_cycles(125000);
    } else if (delay == 250){
        __delay_cycles(250000);
    }

}

int pause()
{
    int bs;
    while(1) {

        // set the shift register 2 mode in NO CHANGE mode, For Leds
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT1;

        clk_high_low(1);
        bs = button_state(bs);

        if ((bs == 100) || (bs == 1000) || (bs == 1))
        {
            // set the shift register 2 mode in RIGHT SHIFT mode, For Leds
            P2OUT |= BIT0;
            P2OUT &= ~BIT1;

            return bs;
        }
    }
}

void send_0_to_LEDs()
{
    P2OUT &= ~BIT6;
}

void send_1_to_LEDs()
{
    P2OUT |= BIT6;
}

short int led_pos = 0, led_bonus = 1;

/*
 * functions to call the Bonus Leds IN REVERSE
 */
void call_D_Bonus_Reverse()
{
    if (led_bonus == 1) {
        P1OUT = 0x20;
        led_bonus++;
    } else if (led_bonus == 2) {
        P1OUT = 0x40;
        led_bonus++;
    } else if (led_bonus == 3) {
        P1OUT = 0x80;
        led_bonus = 1;
    }

    return;
}

/*
 * functions to call the Bonus Leds
 */
void call_D_Bonus()
{
    if (led_bonus == 1) {
        P1OUT = 0x80;
        led_bonus++;
    } else if (led_bonus == 2) {
        P1OUT = 0x40;
        led_bonus++;
    } else if (led_bonus == 3) {
        P1OUT = 0x20;
        led_bonus = 1;
    }

    return;
}

void fastforward()
{
    while(led_pos < 4) {
        // set SR to 1, for switching ON LED D1, 1_to_D1 will reach and stay at D filp flop, then in button_state we move to No change state
        if (led_pos == 0) {
            send_1_to_LEDs();
        } else {
            send_0_to_LEDs();
        }
        led_pos++;
        call_D_Bonus();
        delay_cycles(125);
        clk_high_low(1);
        if (button_state(led_pos) == 0) {
                    return;
        }
    }
    led_pos = 0;

    // recursively calling fastforward
    fastforward();
}

void rewind()
{
    while(led_pos < 4) {
        // shifting right the values on Leds, super quickly!!!
        if (led_pos == 0) {
            send_1_to_LEDs();
        } else {
            send_0_to_LEDs();
        }
        led_pos++;
        clk_high_low(1);
    }

    while(led_pos != 1) {
        // set the shift register 2 mode in left shift mode, For Leds.
        P2OUT &= ~BIT0;
        P2OUT |= BIT1;
        call_D_Bonus_Reverse();
        delay_cycles(125);
        clk_high_low(1);
        led_pos--;
        if (button_state(led_pos) == 0) {
            return;
        }
    }
    delay_cycles(125);

    // recursively calling rewind
    rewind();
}

/**
 * main.c
 */
int main(void)
{
	initMSP();
	int  Qd[4], count, Qd_new, Qd_dummy, delay = 250;
	count = 0;

	// set pin direction, all are o/p on port2 but pin7 is i/p as we need the value of Qd from it
	P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
	P2DIR &= ~BIT7;

	// set pin direction, all are o/p on port 1 for D7(BIT5), D6(BIT6), D5(BIT7)
	P1DIR |= (BIT5 | BIT6 | BIT7);

	// modify XTAL pin to be I/O
	P2SEL &= ~(BIT6 | BIT7);

	// keeping default state as all low
	P2OUT = 0x00;
	P1OUT = 0x00;
	clear_all();

	// set the shift register 2 mode in right shift mode, For Leds
	P2OUT |= BIT0;
	P2OUT &= ~BIT1;

    // set the shift register 1 mode to parallel mode( s1 = 1, s0 = 1 ), For buttons.
    P2OUT |= BIT2;
    P2OUT |= BIT3;

	// Time to Play
	while(1)
	{
	    clk_high_low(1);

	    // set the SHIFT register ONE mode to right shift mode( s1 = 0, s0 = 1 ), For buttons.
	    P2OUT |= BIT2;
	    P2OUT &= ~BIT3;
	    // by sending the clock pulse we got the state of the buttons
	    for(count=3;count>=0;count--)
	    {
	        Qd[count] = ((P2IN & BIT7) == BIT7);
	        P2OUT &= ~BIT6;
	        clk_high_low(1);
	    }

	    // Qd[3] = PB4, Qd[2] = PB3, Qd[1] = PB2, Qd[0] = PB1
	    Qd_dummy = 1000*Qd[3]+100*Qd[2]+10*Qd[1]+Qd[0];

        // set the shift register 1 mode to parallel mode( s1 = 1, s0 = 1 ), For Buttons
        P2OUT |= BIT2;
        P2OUT |= BIT3;

        while(Qd_dummy == 100)
        {
                // set the SHIFT register 1 mode to right shift mode( s1 = 0, s0 = 1 ), to shift towards Qd.
                P2OUT |= BIT2;
                P2OUT &= ~BIT3;

                while(led_pos < 4) {
                    // sending 1s or 0s depending on the Led position
                    if (led_pos == 0) {
                        send_1_to_LEDs();
                    } else {
                        send_0_to_LEDs();
                    }

                    led_pos++;
                    call_D_Bonus();
                    clk_high_low(1);

                    Qd_new = button_state(led_pos);
                    //  Pause Button pressed
                    if (Qd_new == 10) {
                            PAUSE1: Qd_new = pause();
                            // play pressed after pause
                            if (Qd_new == 100) {
                                delay = delay_value(Qd_new);
                            }
                            // fastforward pressed after pause, now we have to return to pause
                            else if (Qd_new == 1000) {
                                fastforward();
                                goto PAUSE1;
                            }
                            // rewind pressed after pause, now we have to return to pause
                            else if (Qd_new == 1) {
                                rewind();
                                goto PAUSE1;
                            }
                    }
                    // fastforward pressed
                    else if (Qd_new == 1000) {
                        fastforward();
                    }
                    // rewind pressed
                    else if (Qd_new == 1) {
                        rewind();
                    }
                    delay = 250;
                    delay_cycles(delay);
                }
                led_pos =0;
        }
	}
}
