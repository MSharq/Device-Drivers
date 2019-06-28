
#include <stdint.h>
#include<msp430g2253.h>

//#define RXIFG P2IFG_bit.P2IFG_6 // Flag for receiver interrupts
#define RXIFG_set P2IFG|=0x01   // set Flag for receiver interrupts
#define RXIFG_reset P2IFG&=~0x01// reset Flag for receiver interrupts

//#define TXIFG P2IFG_bit.P2IFG_7 // Flag for transmit interrupts
#define TXIFG_set P2IFG|=0x02   // set Flag for transmit interrupts
#define TXIFG_reset P2IFG&=~0x02  // reset Flag for transmit interrupts

#define LED_set P3OUT|=BIT6
#define LED_reset P3OUT&=~BIT6

#define LED_2_set P3OUT|=BIT5
#define LED_2_reset P3OUT&=~BIT5

#define LED_3_set P3OUT|=BIT0
#define LED_3_reset P3OUT&=~BIT0

/*#define RXIE P2IE|=0x01 // Enable receiver interrupts
#define TXIE P2IE|=0x02 // Enable transmit interrupts

void UARTSetUp ( void ); // Function to initialize UART
void WriteTXBUF ( const uint8_t TXData); // Write byte for UART to send
uint8_t ReadRXBUF ( void ); // Read received byte from UART*/


