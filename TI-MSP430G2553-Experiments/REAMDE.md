TI MSP430G2553 experiments -

// CAUTION : README needs to be updated and has to be more descriptive, I will be updating it soon.

These programs can be helpful to anyone trying to understand the working
of TI MSP430. Each folder can be individually downloaded. The library files
are present in each of the folders. Also, a header file TemplateEMP.h 
created by my colleagues is also there. This header file is not important for the
code , but is very useful during debugging, as it is helpful in in getting
print statements.

Experiment 1 - Blinking of LEDs

Experiment 2 - Playing around push buttons and polling

Experiment 3 - Playing with Shift registers, which works and takes command very similar to SPI communication protocols.

Experiment 4 - Playing with Analog-to-digital converters and light dependent registers

Experiment 5 - Playing with Pulse width modulation

Experiment 6 - Playing with vibrational sensor and buzzers.

Experiment 7 - Playing with watchdog timers and timer modules.

Experiment 8 - Interfacing with HC-05 bluetooth module using the software UART in the MSP430. Though there are
UART Pins available, but in this code I have software UART configured. i.e. I made the GPIOs work as UART using buffers and 
timer modules. Bluetooth works in slave mode. So a Mobile has to accept its connection. There are various bluetooth terminal
apps available in the play store android, that can be used for sending and receiving the character data. Currently, it so happens that whatever data is sent from Mobile to MSP430 using bluetooth is received, and sent back by the MSP430 back to 
the Mobile.

Experiment 9 - Interfacing with the 16x2 display HD44780, and using various of its commands to make the character move and do all sorts of crazy stuffs. :)
