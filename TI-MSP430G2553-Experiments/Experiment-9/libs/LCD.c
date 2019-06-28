/***************************************************************************//**
 * @file    LCD.c
 * @author  Mohammad Sharique
 * @date    11/Mai/2019
 *
 * @brief   implementation of all the required LCD functions

 ******************************************************************************/

#include "./LCD.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/
static unsigned char var_lcd_enable =  0x08;
static unsigned char var_lcd_mode_set;
static unsigned char var_lcd_cursorSet;
static unsigned char var_lcd_cursorShow;
static unsigned char var_lcd_cursorBlink;
/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/
void data_write(void);
void data_read(void);
void check_busy(void);
void send_command(unsigned char cmd);
void send_data(unsigned char data);
int get_int_len_with_negative_sign (int value);
/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/


void data_write(void)
{
    // Enable is -ve edge triggered for read
    ENABLE_HIGH;
    delayMS(2);
    ENABLE_LOW;
}

void data_read(void)
{
    // Enable is +ve edge triggered for read
    ENABLE_LOW;
    delayMS(2);
    ENABLE_HIGH;
}

void check_busy(void)
{
    P2DIR &= ~(BIT3); // make P2.3 as input
    while((P2IN&BIT3)==1) // while BUSY FLAG is set
    {
        data_read();
    }
    P2DIR |= BIT3; // make P2.3 as output
}

void send_command(unsigned char cmd)
{
    check_busy();

    WRITE;
    RS_LOW;

    P2OUT = (P2OUT & 0xF0)|((cmd>>4) & 0x0F); // send higher nibble
    data_write(); // give enable trigger
    P2OUT = (P2OUT & 0xF0)|(cmd & 0x0F); // send lower nibble
    data_write(); // give enable trigger
}

void lcd_cursorShift(char cur, char direction)
{
    static char state = 0x04;

    if (direction == 0x00) // decrement select
    {
        state = state + 0x02;
    }
    else if (direction == 0x01) // increment select
    {
        state = state + 0x00;
    }

    state = state + cur;
    send_command(state);
}

int get_int_len_with_negative_sign (int value)
{
    int l=!value;
    while(value){ l++; value/=10; }
    return l;
}
/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/
void lcd_init()
{
    P3DIR |= 0xFF; // output direction for RS/E/R&W pins

    P2DIR |= 0xFF; // output direction for cmd/data
    P2OUT &= 0x00; // output is reset

    send_command(0x33); // for initialization, page 46 fig 24 datasheet HD44780 LCD
    send_command(0x32);

    send_command(0x28); // 4 bit mode selection
    // display on cursor on blink on
    lcd_enable(1);
    lcd_cursorShow(1);
    lcd_cursorBlink(1);

    send_command(0x01); // clear the screen
    var_lcd_mode_set = 0x06;
    send_command(var_lcd_mode_set); // increment cursor

}

void lcd_enable (unsigned char on)
{
    if (on == 0x01)
    {
        var_lcd_enable = 0x0C; // display on
    }
    else if (on == 0x00)
    {
        var_lcd_enable = 0x08; // display off
    }

    send_command(var_lcd_enable + var_lcd_cursorShow + var_lcd_cursorBlink);
}

void lcd_cursorSet (unsigned char x, unsigned char y)
{
    if (x == 1)  // row 1
    {
        x = 0x80; // row 1 column 1
    }
    else if (x == 2) // row 2
    {
        x = 0xC0; // row 2 column 1
    }

    var_lcd_cursorSet = x+(--y);
    send_command(var_lcd_cursorSet);
}

void lcd_cursorShow (unsigned char on)
{
    if (on == 0x01)
    {
        var_lcd_cursorShow = 0x02; // Cursor on
    }
    else if (on == 0x00)
    {
        var_lcd_cursorShow = 0x00; // Cursor off
    }

    send_command(var_lcd_enable + var_lcd_cursorShow + var_lcd_cursorBlink);
}

void lcd_cursorBlink (unsigned char on)
{
    if (on == 0x01)
    {
        var_lcd_cursorBlink = 0x01; // Blink on
    }
    else if (on == 0x00)
    {
        var_lcd_cursorBlink = 0x00; // Blink off
    }

    send_command(var_lcd_enable + var_lcd_cursorShow + var_lcd_cursorBlink);
}

void lcd_clear (void)
{
    send_command(0x01); // clear the screen
}

void lcd_putChar (char character)
{
    check_busy();

    WRITE;
    RS_HIGH;

    P2OUT = (P2OUT & 0xF0)|((character>>4) & 0x0F); // send higher nibble
    data_write(); // give enable trigger
    P2OUT = (P2OUT & 0xF0)|(character & 0x0F); // send lower nibble
    data_write(); // give enable trigger
}

void lcd_putText (char *text)
{
    while(*text)
    {
        lcd_putChar(*text);
        text++;
    }
}

void lcd_putNumber (int number) // note "number" is a signed variable
{
    //max int 32767 and min int -32767

    int y,i,limit=0;
    char str[7];
    // could not use functions like memset, so manually provided NULL value.
    // For using memset , i needed to use stdlib.h. I did not know whether
    // I am allowed or not to use stdlib.h header file
    str[0] = '\0';
    str[1] = '\0';
    str[2] = '\0';
    str[3] = '\0';
    str[4] = '\0';
    str[5] = '\0';
    str[6] = '\0';

    // getting the size of the number, whether it is a two digit or three digit etc
    int size = get_int_len_with_negative_sign(number);

    // following code converts the number to string
    if (number < 0)
    {
        str[0] = '-';
        size++; // increasing size value for accomodating -ve sign
        limit++;
        number = number*(-1);
    }

    for(i=size-1;i>=limit;i--)
    {
        y = number%10;
        number = number/10;
        str[i] = y + '0';
    }

    lcd_putText(str);
}

void lcd_displayLeftToRight()
{
    var_lcd_mode_set |= 0x02;
    send_command(var_lcd_mode_set);
}

void lcd_displayRightToLeft()
{
    var_lcd_mode_set &= ~0x02;
    send_command(var_lcd_mode_set);
}

void lcd_scrollDisplayLeft()
{
    // 0x08 selecting Display shift
    // 0x00 towards left
    send_command(0x10 + 0x08 + 0x00);
}

void lcd_scrollDisplayRight()
{
    // 0x08 selecting Display shift
    // 0x04 towards right
    send_command(0x10 + 0x08 + 0x04);
}

void custom_character()
{
    send_command(0x40); // access CGRAM first location

    // building the first custom character
    lcd_putChar(0xFF);
    lcd_putChar(0x00);
    lcd_putChar(0xFF);
    lcd_putChar(0x00);
    lcd_putChar(0xFF);
    lcd_putChar(0x00);
    lcd_putChar(0xFF);
    lcd_putChar(0x00);

    send_command(0x80); // access DDRAM first location
    lcd_putChar(0x00);  // the first custom character

    send_command(0x48); // access CGRAM second location

    // building second custom character
    lcd_putChar(0xFF);
    lcd_putChar(0xFF);
    lcd_putChar(0x00);
    lcd_putChar(0x00);
    lcd_putChar(0x00);
    lcd_putChar(0x00);
    lcd_putChar(0xFF);
    lcd_putChar(0xFF);

    send_command(0x81); // access DDRAM second location
    lcd_putChar(0x01);  // the second custom character
}

void initTimer_A(void)
{
    //Timer0_A3 Configuration
    TACCR0 = 0; //Initially, Stop the Timer
    TACCTL0 |= CCIE; //Enable interrupt for CCR0.
    TACTL = TASSEL_2 + ID_0 + MC_1; //Select SMCLK, SMCLK/1, Up Mode
}

void delayMS(unsigned int msecs)
{
    OFCount = 0; //Reset Over-Flow counter
    TACCR0 = 1000-1; //Start Timer, Compare value for Up Mode to get 1ms delay per loop
    //Total count = TACCR0 + 1. Hence we need to subtract 1.

    while(OFCount<=msecs);

    TACCR0 = 0; //Stop Timer
}

//Timer ISR
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    OFCount++; //Increment Over-Flow Counter
}
