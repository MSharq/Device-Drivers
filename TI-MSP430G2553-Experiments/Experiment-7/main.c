/*
 * Author - Mohammad Sharique
 */

#include <templateEMP.h>

void GPIO_Init(void); // GPIO pins
void BCSplus_Init(void); // BCS timer
void clk_high_low(int clk); // for the temperature LEDs
void no_change_mode_led(void); // changing the mode of temp LEDs
void clear_all_Leds(void); // Clearing all the temp LEDs
void right_shift_mode_led(void); // for shifting right the "high" input in temperature LEDs
void Temp_Init(void); // Initializing the registers for the temperature calculation
int Temp_Out(void); // performing the temperature respective operations here
void Timer_Interrupt_Init(void); // Setting timer here

//#define task1 1
#define task2 2
//#define task2e 3 // UNCOMMENT This to run for task2e

int temp;

int main(void)
{
    initMSP(); // Initialize Microcontroller
    int counter = 0;

    Temp_Init();

    WDTCTL = WDTPW + WDTSSEL; //WDTSSEL = 1 selects ACLK

    BCSplus_Init();
    GPIO_Init();
    Timer_Interrupt_Init();

    while(1)
    {
        P1OUT ^= BIT0;  // this is the green led blinking
        __delay_cycles(125000); // this results in the 4 Hz i.e 125ms On/125ms Off the led

        WDTCTL = WDTPW | WDTCNTCL | WDTSSEL; //WDTCNTCL = 1 clears the count value to 0000h (section 10.3.1 in user guide)

        /**************controller logic***********/

#ifndef task2e
        /* heater control */
        if (temp >= 509)
        {
            /* turning heater OFF */
            P3OUT &= ~BIT4;
        }
        else if (temp <= 431)
        {
            /* turning heater ON */
            P3OUT |= BIT4;
        }
#else

        if(temp <= 431)
        {
            while(temp <= 508)
            {
                P3OUT |= BIT5;
                __delay_cycles(500);
                P3OUT &= ~BIT5;
                __delay_cycles(500);
                // the delays help to create 1 Khz frequency

                counter++;
                // 125 selected as , 4Hz is 0.25 secs and 1Khz is 0.001 secs, 0.001 x 250 = 0.25secs
                if (counter == 125)
                {
                    //serialPrint("--blink--");
                    P1OUT ^= BIT0;
                    counter = 0;
                }
            }
         }
        else if (temp >= 509)
        {
            P3OUT &= ~BIT5;
        }

#endif
    }
    return 0;
}

void Temp_Init(void)
{
    // ADC functionality
    ADC10CTL0 = ADC10ON + ADC10SHT_3 ;

    ADC10CTL1 = INCH_5 + ADC10DIV_3; // temp sensor is input to 1.5 and clock/8
}

void GPIO_Init(void)
{
    P1DIR = BIT0; // port 1 direction register, bit0 output

    P1DIR &= ~BIT3; // setting P1.3 as input for PB5
    P1REN |= BIT3;  // Enable pull-up resistor
    P1OUT |= BIT3;  // Set as pull-up
    P1IE |= BIT3 ;  // Enable interrupt for the PB5 push
    P1IES |= BIT3 ; // interrupt recognised on high to low
    P1IFG &= ~ BIT3 ; // Clear interrupt flag

    P1DIR &= ~BIT5; // NTC as input

    P3DIR = BIT2; // port 3 direction register, bit 5, 4 & 2 output, bit 2 for RED led
#ifndef task2e
    P3DIR |= BIT4; //REL_STAT as output
#else
    P3DIR |= BIT5; //REL_DYN as output
#endif

    // modify XTAL pin to be I/O
    P2SEL &= ~(BIT6 | BIT7);

    P3OUT &= ~BIT2; // swichting off the RED LED

    // set pin direction as Output for required pins on port2
    P2DIR |= (BIT0 | BIT1 | BIT4 | BIT5 | BIT6);
    P2OUT = 0x00;
    clk_high_low(1);
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

void no_change_mode_led(void)
{
    // set the shift register 2 mode in NO CHANGE mode, For Leds
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT1;
}

void clear_all_Leds(void)
{
    // clear/reset the shift register, i.e turn all LEDs off
    P2OUT &= ~BIT5;
    // stop the reset mode (/CLR = 1)
    P2OUT |= BIT5;
    // reset the clock signal to 0
    P2OUT &= ~BIT4;
    // switching red led off
    P3OUT &= ~BIT2;
}

void right_shift_mode_led(void)
{
    // set the shift register 2 mode in RIGHT SHIFT mode, For Leds
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
}

void BCSplus_Init(void)
{
#ifdef task1
    BCSCTL1 |= XT2OFF | DIVA_1; //XTS is 0, DIVA_1 division results in ACLK divide by 2
    BCSCTL3 = XT2S_0 | LFXT1S_2; //As XTS is 0, LFXT1S_2 results in VLOCLK as source to ACLK
    //so divide by 2 and further having prescaled with 32768 using WDTISx as 00 in WDTCTL results in 5.5 secs wait in Watchdogtimer
#endif
#ifdef task2
    BCSCTL1 |= XT2OFF | DIVA_3; //XTS is 0, DIVA_1 division results in ACLK divide by 8
    BCSCTL3 = XT2S_0 | LFXT1S_2; //As XTS is 0, LFXT1S_2 results in VLOCLK as source to ACLK
    // WDTCTL results in about 21.9 secs
#endif
}

int Temp_Out(void)
{
    int t=0;
    ADC10CTL0 |= ENC + ADC10SC;      //enable conversion and start conversion
    while(ADC10CTL1 & BUSY);         //wait..i am converting..pum..pum..
    t=ADC10MEM;                       //store val in t
    ADC10CTL0&=~ENC;                     //disable adc conv
    return t;
}

void Timer_Interrupt_Init(void)
{
    TACTL = TASSEL_1 + MC_1 + ID_0;        // TA0 CTL = 1011 01000
    CCTL0 = CCIE;                        // TA0 CCTL0
#ifdef task1
    CCR0 = 12255;                        // TA0 CCR0 value is 12255
#endif
#ifdef task2
    CCR0 = 3053;                        // TA0 CCR0 value is 3053
#endif

}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1()
{
    P1IFG &= ~BIT3; // Clear interrupt flag
    __enable_interrupt();

    if((P1IN & BIT3 ) == 0)
    {
        while(1);
    }
}

// Timer A0 interrupt service routine
# pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A ( void ) {
    int i = 0;
    clear_all_Leds();

    int m1 = Temp_Out();           //read temp
    temp = m1;

    // the division I am taking is 40 steps per division
    // ADC o/p lowest recorded was 350 and highest 550, so 5 subranges of approximately 40 values each
    serialPrint("**");
    serialPrintInt(m1);
    serialPrint("**");
    // so for division 0-390, actually lowest recorded ADC output was 350
    if (m1 <= 390) {
        // switching ON D1
        right_shift_mode_led();
        P2OUT |= BIT6;
        clk_high_low(1);
        no_change_mode_led();
    }
    // division 390-430
    else if (m1 >= 391 && m1 <= 430) {
        // switching ON D1 D2
        right_shift_mode_led();
        i = 2;
        while(i--)
        {
            P2OUT |= BIT6;
            clk_high_low(1);
        }
        no_change_mode_led();
    }
    // division 431-470
    else if (m1 >= 431 && m1 <= 470) {
        // switching ON D1 D2 D3
        right_shift_mode_led();
        i = 3;
        while(i--)
        {
            P2OUT |= BIT6;
            clk_high_low(1);
        }
        no_change_mode_led();
    }
    // division 471-510
    else if (m1 >= 471 && m1 <= 510) {
        // switching ON D1 D2 D3 D4
        right_shift_mode_led();
        i = 4;
        while(i--)
        {
            P2OUT |= BIT6;
            clk_high_low(1);
        }
        no_change_mode_led();
    }
    // division 511- ...-> 550 was the maximum ADC output recorded from NTC temperature observation
    else if (m1 >= 511) {
        // switching ON D1 D2 D3 D4 and the RED led D6
        right_shift_mode_led();
        i = 4;
        while(i--)
        {
            P2OUT |= BIT6;
            clk_high_low(1);
        }
        no_change_mode_led();

        P3OUT |= BIT2;
        clk_high_low(1);
    }
}
