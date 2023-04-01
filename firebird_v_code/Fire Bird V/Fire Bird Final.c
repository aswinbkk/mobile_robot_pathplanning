#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h> // for itoa()

#define BAUD 9600 // XBee module baud rate
#define UBRR_VALUE ((F_CPU/16/BAUD)-1) // calculate UBRR value

#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

#define sbit(reg,bit)	reg |= (1<<bit)			// Macro defined for Setting a bit of any register.
#define cbit(reg,bit)	reg &= ~(1<<bit)		// Macro defined for Clearing a bit of any register.

void init_ports();
void lcd_reset();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_line1();
void lcd_line2();
void lcd_string(char*);

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

uint8_t data;
uint8_t column1;
uint8_t row1;
uint8_t column2;
uint8_t row2;
uint8_t goal_column;
uint8_t goal_row;

void pin_config (void)
{
	DDRA = DDRA | 0x0F; //set direction of the PORTA 3 to PORTA 0 pins as output
	PORTA = PORTA & 0xF0; // set initial value of the PORTA 3 to PORTA 0 pins to logic 0
	DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
	PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//Function to initialize ports
void port_init()
{
	pin_config();
}
//Function to Reset LCD
void lcd_set_4bit()
{
	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(1);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

//Function to Initialize LCD
void lcd_init()
{
	_delay_ms(1);

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
	
}


//Function to Write Command on LCD
void lcd_wr_command(unsigned char cmd)
{
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	lcd_port &= 0x0F;
	lcd_port |= temp;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

//Function to Write Data on LCD
void lcd_wr_char(char letter)
{
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	lcd_port &= 0x0F;
	lcd_port |= temp;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}


//Function to bring cursor at home position
void lcd_home()
{
	lcd_wr_command(0x80);
}


//Function to Print String on LCD
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

//Position the LCD cursor at "row", "column".

void lcd_cursor (char row, char column)
{
	switch (row) {
		case 1: lcd_wr_command (0x80 + column - 1); break;
		case 2: lcd_wr_command (0xc0 + column - 1); break;
		case 3: lcd_wr_command (0x94 + column - 1); break;
		case 4: lcd_wr_command (0xd4 + column - 1); break;
		default: break;
	}
}

//Function To Print Any input value upto the desired digit on LCD
void lcd_print (char row, char coloumn, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		lcd_home();
	}
	else
	{
		lcd_cursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcd_wr_char(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_wr_char(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_wr_char(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_wr_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_wr_char(unit);
	}
	if(digits>5)
	{
		lcd_wr_char('E');
	}
	
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
	lcd_set_4bit();
	lcd_init();
	
	goal_row = USART_Receive(); // receive data from UART
	_delay_ms(1000);
	lcd_print(2,1,goal_row,3);
	
	USART_Transmit('0');
	goal_column = USART_Receive(); // receive data from UART
	_delay_ms(1000);
	lcd_print(2,5,goal_column,3);

	while(1)
	{
		//lcd_cursor(1,6);
		//lcd_string("TEAM 6");
		
		if (UCSR0A & (1<<RXC0))
		{
			row1 = USART_Receive(); // receive data from UART
			_delay_ms(1000);
			lcd_print(1,1,row1,3);
			
			USART_Transmit('0');
			column1 = USART_Receive(); // receive data from UART
			_delay_ms(1000);
			lcd_print(1,5,column1,3);

			USART_Transmit('1');
			row2 = USART_Receive(); // receive data from UART
			_delay_ms(1000);
			lcd_print(1,10,row2,3);
			
			USART_Transmit('0');
			column2 = USART_Receive(); // receive data from UART
			_delay_ms(1000);
			lcd_print(1,14,column2,3);
			USART_Transmit('1');


			while ((goal_column != column2) || (goal_row != row2))
			{
				// determine the direction to move in based on the current and target positions
				int deltaColumn = column2 - column1;
				int deltaRow = row2 - row1;
				if (deltaColumn > 0)
				{ // if column1 is to the left of column2
					forward();
					_delay_ms(1000);
					stop();
					column1++;
					_delay_ms(1000);
				}
				else if (deltaColumn < 0)
				{ // if column1 is to the right of column2
					left();
					_delay_ms(1000);
					stop();
					column1--;
					_delay_ms(1000);
				}
				if (deltaRow > 0)
				{ // if row1 is above row2
					right();
					_delay_ms(1000);
					stop();
					row1++;
					_delay_ms(1000);
				}
				else if (deltaRow < 0)
				{ // if row1 is below row2
					forward();
					_delay_ms(1000);
					stop();
					row1--;
					_delay_ms(1000);
				}
				// update column1, row1, column2, and row2 with the next set of column and row addresses received
				
				row1 = row2;
				_delay_ms(1000);
				lcd_print(2,1,row1,3);
				
				column1 = column2;
				_delay_ms(1000);
				lcd_print(2,5,column1,3);

				if (UCSR0A & (1<<RXC0))
				{
					row2 = USART_Receive(); // receive data from UART
					_delay_ms(1000);
					lcd_print(2,10,row2,3);
					
					USART_Transmit('0');
					column2 = USART_Receive(); // receive data from UART
					_delay_ms(1000);
					lcd_print(2,14,column2,3);
					USART_Transmit('1');
				}
			}

		}
	}
}
