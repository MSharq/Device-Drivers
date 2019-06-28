/**
 * Author - Mohammad Sharique
 */

#include <msp430.h>
#include <templateEMP.h>
#include <stdint.h>

#define interrupt_enable 1;

int count;
int PB6_press = 1;
int counter;

unsigned int data1[12] = {5000 , 5000 , 1000 , 2000 , 1000 , 2000 , 1000 , 2000 , 1000 , 2000 , 1000 , 2000};
unsigned int data2[12] = {9990 , 100 , 1500 , 9000 , 6000 , 9000 , 1500 , 9000 , 6000 , 9000, 100, 100};

// enable PWM
void PWM()
{
    P3DIR |= BIT6;
    P3SEL |= BIT6;
    P3REN &= ~BIT6;
    P3OUT &= ~BIT6;
}
// Function to enable the Piezo sensor, and disable PWM mode
void knockSensor()
{
    P3DIR &= ~BIT6;
    P3SEL &= ~BIT6;
    P3REN |= BIT6;
    P3OUT |= BIT6;
}

void playNote(unsigned int *data)
{
    if (count%2 == 0)
    {
        PB6_press = 1;
        for(counter = 11;counter >= 0;counter--)
        {
            TA0CCTL2 = OUTMOD_3; // CCR2 set / reset
            TA0CCR0 = data[counter];
            TA0CCR2 = data[counter]/2; // CCR2 PWM 50% duty cycle
            while (PB6_press%2 == 0);
            TA0CTL = TASSEL_2 + MC_1; // TASSEL_2 means SMCLK, MC_1 -> up - mode
            __delay_cycles(100000);
        }
    }
    else
    {
        PB6_press = 1;
        for(counter = 11;counter >= 0;counter--)
        {
            TA0CCTL2 = OUTMOD_3; // CCR2 set / reset
            TA0CCR0 = data[counter];
            TA0CCR2 = data[counter]/2; // CCR2 PWM 50% duty cycle
            while (PB6_press%2 == 0);
            TA0CTL = TASSEL_2 + MC_3; // TASSEL_2 means SMCLK, MC_3 -> up - down - mode
            __delay_cycles(100000);
        }
    }

#ifndef interrupt_enable
    count++;
#endif
}

void knockSensor_knockCheck()
{
    if ((P3IN & BIT6) == 0)
    {
        serialPrint("--low--");
    }
}

/**
 * main.c
 */
int main(void)
{
    int i;

    initMSP();

    // P3 .6 output
    P3DIR = 0x00;
    P3DIR |= BIT6;
    P3SEL |= BIT6;

    // P3 .6 TA0 .2 option
    P3SEL |= BIT6;


    // Set P1.3 -> PB5 and P1.4 -> PB6
    P1DIR &= ~0x18;

    // Enable P1.3 and P1.4 for pull-up/pull-down resistors, buttons will not work if these are not set
    P1REN |= 0x18;

    // Set to pull-up
    P1OUT |= 0x18;

    // Enable interrupts
    P1IE |= 0x18;

    // High/Low-Edges
    P1IES |= 0x18;

    // Clear interrupt flags
    P1IFG &= ~0x18;

    //P2IFG &= ~0x18;

#ifdef interrupt_enable
    // to enable all the interrupts
    _BIS_SR(GIE);
#endif

#ifndef interrupt_enable
    // to disable all the interrupts
    _BIC_SR(GIE);
#endif

    for(;;) {
#ifdef interrupt_enable
        // enable the sensor
        knockSensor();
        // resetting the count to 1
        count = 1;
        // a loop of 1000 x 1000 delay cycles = 1 sec delay
        for(i=0;i<1000;i++)
        {
            __delay_cycles(1000);
            if ((P3IN & BIT6) == 0)
            {
                count++;
            }
        }

        if (count == 2)
        {
            PWM();
            playNote(data1);
        }
        else if (count  == 3)
        {
            PWM();
            playNote(data2);
        }
        TA0CCR0 = 0;

#endif
#ifndef interrupt_enable
            playNote(data1);
            TA0CCR0 = 0;
            __delay_cycles(500000);
            __delay_cycles(500000);
            playNote(data2);
            TA0CCR0 = 0;
            TA0CTL = TASSEL_2 + MC_0;
            __delay_cycles(500000);
            __delay_cycles(500000);
#endif
    }
}

// Interrupt Service routine (ISR) for PB5 and PB6
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    // when button PB5 is pressed for the first time->play data1, second time->play data2

    switch (P1IFG)
        {
            case BIT3:
                count++;
                // this "if" will be true if after PB6 is pressed then PB5 is pressed
                if (PB6_press%2 == 0)
                {
                    // this increment will break the infinite while loop in playNote
                    PB6_press++;
                    // changing counter value to 12, so the playNote loop starts from the beginning
                    counter = 12;
                }
                P1IFG &= ~BIT3;         // clear the interrupt flag
                break;
            case BIT4:
                // this is to halt the timer
                TA0CTL = TASSEL_2 + MC_0;
                // this increment will break the infinite while loop in playNote
                PB6_press++;
                count = 1;
                P1IFG &= ~BIT4;         // clear the interrupt flag
                break;
            default:
                P1IFG = 0x00;
                break;
        }
}
