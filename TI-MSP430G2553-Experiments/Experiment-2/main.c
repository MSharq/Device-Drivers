/**
 * Author - Mohammad Sharique
 */

//#include <msp430.h>
#include <templateEMP.h>

/**
 * main.c
 */

//#define Polling 1

#define LED_set P3OUT|=BIT6
#define LED_reset P3OUT&=~BIT6

#define LED_2_set P3OUT|=BIT5
#define LED_2_reset P3OUT&=~BIT5

#define     RXD            BIT6        // TXD on P2.6

volatile int count;

void PB5_press()
{
    while(((BIT3 & P1IN)== 0) && (P1IN & BIT4) !=0) {
        if(count == 1)
        {
            P1OUT &= 0x18;
            P1OUT |= 0x38;   // 1.5 -> red ON, 1.7 -> D9 yellow OFF
            __delay_cycles(250000);
            P1OUT &= 0x18;
            P1OUT |= 0x98;  // 1.5 -> red OFF, 1.7 -> D9 yellow ON , till the button is kept pressed
            count = count - 1;
        }
        else
        {
            continue;
        }
    }

}

int main(void)
{
    //WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    initMSP();

    // Set P1.3 -> PB5 and P1.4 -> PB6
    P1DIR &= ~0x18;
    P1DIR &= ~0x18;

    // Enable P1.3 and P1.4 for pull-up/pull-down resistors, buttons will not work if these are not set
    P1REN |= 0x18;

    // Set to pull-up
    P1OUT |= 0x18; // pull up, works, so I built my code accordingly.
    //P1OUT &= ~0x18; // pull down, it doesn't work. :/ I do not know why?

    // Enable interrupts
    P1IE |= 0x08;
    P1IE |= 0x08;

    // High/Low-Edges
    P1IES |= 0x08;
    P1IES |= 0x08;

    // Clear interrupt flags
    P1IFG &= ~0x08;
    P1IFG &= ~0x08;

    // Set P1.0 -> Blue LED, P1.5 -> Red LED, P1.6 -> Green LED and P1.7 -> Yellow LED as output
    P1DIR |= 0xE1; // 1110 0001;

    P2DIR &= ~RXD;
    P2REN |= RXD;
    P2OUT |= RXD;
    P2IES |= RXD;     // RXD Hi/Lo edge interrupt
    P2IFG &=~RXD;     // Clear RXD (flag) before enabling interrupt
    P2IE |= RXD;      // Enable RXD interrupt

#ifndef Polling
    // to enable all the interrupts
    _BIS_SR(GIE);
#endif

      while (1)
      {
#ifdef Polling
          // to disable all the interrupts
          _BIC_SR(GIE);
#endif
          // when both button PB5 and PB6 are not pressed
          if (((P1IN & 0x18)) == 0x18)
          {
              P1OUT |= 0x98;    // 1.7 -> D9 yellow
              P1OUT &= 0x98;
          }
          // when button PB5 is released and PB6 is pressed
          else if (((P1IN & 0x08)) == 0x08)
          {
              P1OUT |= 0xD8;    // 1.6 -> D5 green, 1.7 -> D9 yellow
              P1OUT &= 0xD8;
          }
#ifdef Polling
          // when button PB5 is pressed and PB6 is released
          else if (((P1IN & 0x10)) == 0x10)
          {
              count = 1;

              PB5_press();
          }

          // when PB5 and PB6 are pressed
          else
          {
              P1OUT |= 0x99;   // 1.0 -> D7 blue, 1.7 -> D9 yellow
              P1OUT &= 0x99;
          }
#endif
      }

}

// Interrupt Service routine (ISR) for PB5
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    /*
    // when button PB5 is pressed and PB6 is released
    if (((P1IN & 0x10)) == 0x10)
    {
        count = 1;

        PB5_press();
    }
    // when PB5 and PB6 are pressed
    else
    {
        P1OUT |= 0x99;   // 1.0 -> D7 blue, 1.7 -> D9 yellow
        P1OUT &= 0x99;
    }*/

    LED_set;
    __delay_cycles(10000);
    LED_reset;

    P1IFG &= ~BIT6;
    //P1IFG &= ~0x08; // interrupt serviced so clearing the flag
}
