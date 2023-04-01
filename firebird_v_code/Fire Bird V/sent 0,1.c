#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 9600 // XBee module baud rate
#define UBRR_VALUE ((F_CPU/16/BAUD)-1) // calculate UBRR value

uint8_t data;
uint8_t column1;
uint8_t row1;
uint8_t column2;
uint8_t row2;

void motion_pin_config (void)
{
	DDRA = DDRA | 0x0F; //set direction of the PORTA 3 to PORTA 0 pins as output
	PORTA = PORTA & 0xF0; // set initial value of the PORTA 3 to PORTA 0 pins to logic 0
	DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
	PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM
}

//Function to initialize ports
void port_init()
{
	motion_pin_config();
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
	unsigned char PortARestore = 0;

	Direction &= 0x0F; 			// removing upper nibbel as it is not needed
	PortARestore = PORTA; 			// reading the PORTA's original status
	PortARestore &= 0xF0; 			// setting lower direction nibbel to 0
	PortARestore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTA status
	PORTA = PortARestore; 			// setting the command to the port
}

void forward (void) //both wheels forward
{
	motion_set(0x06);
}

void back (void) //both wheels backward
{
	motion_set(0x09);
}

void left (void) //Left wheel backward, Right wheel forward
{
	motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
	motion_set(0x0A);
}

void soft_left (void) //Left wheel stationary, Right wheel forward
{
	motion_set(0x04);
}

void soft_right (void) //Left wheel forward, Right wheel is stationary
{
	motion_set(0x02);
}

void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
	motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
	motion_set(0x08);
}

void stop (void) //hard stop
{
	motion_set(0x00);
}

void USART_Init(void) {
	// set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)(UBRR_VALUE);

	// enable transmitter and receiver
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);

	// set frame format: 8 data bits, no parity, 1 stop bit
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void USART_Transmit(uint8_t data) {
	// wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)));

	// put data into buffer, sends data
	UDR0 = data;
}

uint8_t USART_Receive(void) {
	// wait for data to be received
	while (!(UCSR0A & (1<<RXC0)));

	// get and return received data from buffer
	return UDR0;
}

void init_devices (void)
{
	cli(); //Clears the global interrupts
	port_init();
	sei(); //Enables the global interrupts
}

int main(void)
{
	USART_Init(); // initialize UART communication
	init_devices();

	while(1)
	{
		if (UCSR0A & (1<<RXC0))
		{
			
			USART_Transmit('0');
			 _delay_ms(2000);
			USART_Transmit('1');
			 _delay_ms(2000);

		}
	}
}
