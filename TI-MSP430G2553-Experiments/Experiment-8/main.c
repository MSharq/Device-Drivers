/**
 * Author - Mohammad Sharique
 */

#include <msp430g2553.h>
#include <templateEMP.h>
#include <stdbool.h>
#include "uart.h"

#define     TXD            BIT5        // TXD on P1.5
#define     RXD            BIT6        // TXD on P1.6

unsigned int Bit_time = 105; //9600 Baud, SMCLK=1MHz (1MHz/9600)=104
unsigned int Bit_time_5 = 52;  // Time for half a bit


unsigned char BitCnt;  // Bit count, used when transmitting byte
unsigned int TXByte;  // Value sent over UART when Transmit() is called
unsigned int RXByte;  // Value recieved once hasRecieved is set

bool isReceiving;   // Status for when the device is receiving
bool hasReceived;   // Lets the program know when a byte is received

// Function Definitions
void Transmit(void);

void uart_string(const char *str)
{
    while(*str != 0)
    {
        TXByte = *str++;
        Transmit();
    }
}

void led_config()
{
    P3DIR |= BIT6+BIT5+BIT0;

    P3OUT &= ~(BIT6+BIT5+BIT0);
}

void main(void)
{
  initMSP();
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  int y = 10;

  serialPrint("**Wang-Sharique Welcomes All**");

  led_config();

  unsigned int data_in = 0;      // Transmit value counter

  BCSCTL1 = CALBC1_1MHZ;     // Set range
  DCOCTL = CALDCO_1MHZ;      // SMCLK = DCO = 1MHz

  P1SEL |= TXD;                              //
  P1DIR |= TXD;                              //

  P1DIR &= ~RXD;
  P1IES |= RXD;     // RXD Hi/Lo edge interrupt
  P1IFG &=~RXD;     // Clear RXD (flag) before enabling interrupt
  P1IE |= RXD;      // Enable RXD interrupt

  __bis_SR_register(GIE);        // interrupts enabled\

  // put a \n at the front or the back and send the data, the bluetooth will forward it.

  TXByte = '2';      // Load the received byte into the byte to be transmitted
  Transmit();
  //TXByte = 10;          // send '\n' to transfer the data
  //Transmit();

  uart_string("..sharique-wang..");
  TXByte = 10;          // send '\n' to transfer the data
  Transmit();

  while(1)
  {
      if (hasReceived)      // If the device has received a value
      {
          hasReceived = false;  // Clear the flag
          TXByte = RXByte;      // Load the received byte into the byte to be transmitted
          Transmit();
          TXByte = 10;          // send '\n' to transfer the data
          Transmit();
      }
      if (~hasReceived)     // Loop again if another value has been received
      {
          __bis_SR_register(GIE);
      }
  }
}

// Function Transmits Character from TXByte
void Transmit()
{
  LED_set;
  while(isReceiving);               // Wait for RX completion
  CCTL0 = OUT;                      // TXD Idle as Mark
  TACTL = TASSEL_2 + MC_2;          // SMCLK, continuous mode

  BitCnt = 0xA;                       // Load Bit counter, 8 bits + ST/SP
  CCR0 = TAR;                       // Initialize compare register

  CCR0 += Bit_time;                  // Set time till first bit
  TXByte |= 0x100;                 // Add stop bit to TXByte (which is logical 1)
  TXByte = TXByte << 1;            // Add start bit (which is logical 0)

  CCTL0 =  CCIS0 + OUTMOD0 + CCIE; // Set signal, intial value, enable interrupts
  while ( CCTL0 & CCIE );          // Wait for TX completion
  //TACTL = TASSEL_2;             // SMCLK, timer off (for power consumption)
  LED_reset;
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void port_1(void)
{
    LED_2_set;
    isReceiving = true;

    P1IE &= ~RXD;       // Disable RXD interrupt
    P1IFG &= ~RXD;      // Clear RXD IFG(interrupt flag)

    TACTL = TASSEL_2 + MC_2;    // SMCLK, continuous mode
    CCR0 = TAR;                 // Initialize compare register
    CCR0 += Bit_time_5;         // Set time till first bit
    CCTL0 = OUTMOD1 + CCIE;     // Disable TX and enable interrupts

    RXByte = 0;         // Initialize RXByte
    BitCnt = 0x9;       // Load Bit counter, 8 bits + ST
    LED_2_reset;
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    if(!isReceiving)    // if Transmitting
    {
        CCR0 += Bit_time;     // Add Offset to CCR0
        if ( BitCnt == 0)     // If all bits TXed, disable interrupt
        {
            TACTL = TASSEL_2;   // SMCLK, timer 0ff (for power consumption)
            CCTL0 &= ~ CCIE ;
        }
        else
        {
            CCTL0 |=  OUTMOD2;        // set TX bit to 0
            if (TXByte & 0x01)        // we are looking for the "1"
                CCTL0 &= ~ OUTMOD2;   // TX Mark. i.e if it should be 1, set it to 1
            TXByte = TXByte >> 1;
            BitCnt --;
        }
    }
    else  // if Receiving
    {
        CCR0 += Bit_time;           // Add offset to CCR0
        if ( BitCnt == 0)
        {
            TACTL = TASSEL_2;       // SMCLk, timer off (for power consumption)
            CCTL0 &= ~CCIE;         // Disable interrupt

            isReceiving = false;

            P1IFG &= ~RXD;      // clear RXD IFG (interrupt flag)
            P1IE |= RXD;        // enabled RXD interrupt

            if ((RXByte & 0x201) == 0x200)  // Validate the start and stop bits are correct
            {
                RXByte = RXByte >> 1;       // Remove start bit
                RXByte &= 0xFF;             // Remove stop bit
                hasReceived = true;
            }
            else
            {
                RXByte = RXByte >> 1;       // Remove start bit
                hasReceived = true;
            }
            __bic_SR_register_on_exit(CPUOFF);

        }
        else
        {
            if ((P1IN & RXD) == RXD)    // if bit is set?
                RXByte |= 0x400;        // Set the value in the RXByte
            RXByte = RXByte >> 1;       // Shift the bits down
            BitCnt--;                   // Reduce the bit count
        }
    }
}
