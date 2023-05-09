#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> //included to support power function

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

unsigned int value;
unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;
unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char sharp, distance, adc_reading;
unsigned int value;
float BATT_Voltage, BATT_V;


volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
volatile unsigned int Degrees; //to accept angle in degrees for turning

uint8_t data;
uint8_t column1;
uint8_t row1;
uint8_t column2;
uint8_t row2;
uint8_t goal_column;
uint8_t goal_row;
uint8_t start_column;
uint8_t start_row;

void pin_config (void)
{
	DDRA = DDRA | 0x0F; //set direction of the PORTA 3 to PORTA 0 pins as output
	PORTA = PORTA & 0xF0; // set initial value of the PORTA 3 to PORTA 0 pins to logic 0
	DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
	PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//ADC pin configuration
void adc_pin_config (void)
{
	DDRF = 0x00; //set PORTF direction as input
	PORTF = 0x00; //set PORTF pins floating
	DDRK = 0x00; //set PORTK direction as input
	PORTK = 0x00; //set PORTK pins floating
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo2_pin_config (void)
{
	DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
	PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Configure PORTB 7 pin for servo motor 3 operation
void servo3_pin_config (void)
{
	DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
	PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}

//Function to initialize ports
void port_init()
{
	pin_config();
	left_encoder_pin_config(); //left encoder pin config
	right_encoder_pin_config(); //right encoder pin config
	servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
	servo3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation
	adc_pin_config();
}

//Function to Initialize ADC
void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}



//This Function accepts the Channel Number and returns the corresponding Analog Value
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for ADC conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

// This Function calculates the actual distance in millimeters(mm) from the input
// analog value of Sharp Sensor.
unsigned int Sharp_GP2D12_estimation(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
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

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt
}

//ISR for right position encoder
ISR(INT5_vect)
{
	ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
	ShaftCountLeft++;  //increment left shaft position count
}


//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
	unsigned char PortARestore = 0;

	Direction &= 0x0F; 		// removing upper nibbel for the protection
	PortARestore = PORTA; 		// reading the PORTA original status
	PortARestore &= 0xF0; 		// making lower direction nibbel to 0
	PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
	PORTA = PortARestore; 		// executing the command
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

void stop (void)
{
	motion_set(0x00);
}


//Function used for turning robot by specified degrees
void angle_rotate(unsigned int Degrees)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;

	while (1)
	{
		if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
		break;
	}
	stop(); //Stop robot
}

//Function used for moving robot forward by specified distance

void linear_distance_mm(unsigned int DistanceInMM)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	
	ShaftCountRight = 0;
	while(1)
	{
		if(ShaftCountRight > ReqdShaftCountInt)
		{
			break;
		}
	}
	stop(); //Stop robot
}

void forward_mm(unsigned int DistanceInMM)
{
	forward();
	linear_distance_mm(DistanceInMM);
}

void back_mm(unsigned int DistanceInMM)
{
	back();
	linear_distance_mm(DistanceInMM);
}

void left_degrees(unsigned int Degrees)
{
	// 88 pulses for 360 degrees rotation 4.090 degrees per count
	left(); //Turn left
	angle_rotate(Degrees);
}



void right_degrees(unsigned int Degrees)
{
	// 88 pulses for 360 degrees rotation 4.090 degrees per count
	right(); //Turn right
	angle_rotate(Degrees);
}


void soft_left_degrees(unsigned int Degrees)
{
	// 176 pulses for 360 degrees rotation 2.045 degrees per count
	soft_left(); //Turn soft left
	Degrees=Degrees*2;
	angle_rotate(Degrees);
}

void soft_right_degrees(unsigned int Degrees)
{
	// 176 pulses for 360 degrees rotation 2.045 degrees per count
	soft_right();  //Turn soft right
	Degrees=Degrees*2;
	angle_rotate(Degrees);
}

void soft_left_2_degrees(unsigned int Degrees)
{
	// 176 pulses for 360 degrees rotation 2.045 degrees per count
	soft_left_2(); //Turn reverse soft left
	Degrees=Degrees*2;
	angle_rotate(Degrees);
}

void soft_right_2_degrees(unsigned int Degrees)
{
	// 176 pulses for 360 degrees rotation 2.045 degrees per count
	soft_right_2();  //Turn reverse soft right
	Degrees=Degrees*2;
	angle_rotate(Degrees);
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

//TIMER1 initialization in 10 bit fast PWM mode  
//prescale:256
// WGM: 7) PWM 10bit fast, TOP=0x03FF
// actual value: 42.187Hz 
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
 TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
 OCR1BH = 0x03;	//Output compare Register high value for servo 2
 OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
 OCR1CH = 0x03;	//Output compare Register high value for servo 3
 OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
 ICR1H  = 0x03;	
 ICR1L  = 0xFF;
 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 TCCR1C = 0x00;
 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:64
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:674.988Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

// Function for robot velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

void init_devices (void)
{
	cli(); //Clears the global interrupts
	port_init();
	timer1_init();
	timer5_init();
	adc_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
	sei(); //Enables the global interrupts
}

//Function to rotate Servo 2 by a specified angle in the multiples of 1.86 degrees
void servo_2(unsigned char degrees)
{
	float PositionTiltServo = 0;
	PositionTiltServo = ((float)degrees / 1.86) + 35.0;
	OCR1BH = 0x00;
	OCR1BL = (unsigned char) PositionTiltServo;
}

//Function to rotate Servo 3 by a specified angle in the multiples of 1.86 degrees
void servo_3(unsigned char degrees)
{
	float PositionServo = 0;
	PositionServo = ((float)degrees / 1.86) + 35.0;
	OCR1CH = 0x00;
	OCR1CL = (unsigned char) PositionServo;
}

//servo_free functions unlocks the servo motors from the any angle
//and make them free by giving 100% duty cycle at the PWM. This function can be used to
//reduce the power consumption of the motor if it is holding load against the gravity.

void servo_2_free (void) //makes servo 2 free rotating
{
	OCR1BH = 0x03;
	OCR1BL = 0xFF; //Servo 2 off
}

void servo_3_free (void) //makes servo 3 free rotating
{
	OCR1CH = 0x03;
	OCR1CL = 0xFF; //Servo 3 off
}

void lift (void) // grip the object
{
	servo_2 (1);
	_delay_ms(1000);
}

void down (void) // relase the object
{
	servo_2 (90);
	_delay_ms(1000);

}

void ungrip (void)
{
	servo_3 (40);
	_delay_ms(1000);
}

void grip (void)
{
	servo_3 (80);
	_delay_ms(1000);
}

void check_pin()
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
}
void move_robot()
{
	if ((row1==row2) && (column1<column2))
	{ // if column1 is to the left of column2
		left_degrees(90); //Rotate robot left by 90 degrees
		_delay_ms(1000);
		forward_mm(200); //Moves robot forward 100mm
		_delay_ms(1000);
		right_degrees(90); //Rotate robot right by 90 degrees
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1==row2) && (column1>column2))
	{ // if column1 is to the left of column2
		right_degrees(90); //Rotate robot right by 90 degrees
		_delay_ms(1000);
		forward_mm(200); //Moves robot forward 100mm
		_delay_ms(1000);
		left_degrees(90); //Rotate robot left by 90 degrees
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1<row2) && (column1==column2))
	{ // if column1 is to the left of column2
		forward_mm(200); //Moves robot forward 100mm
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1>row2) && (column1==column2))
	{ // if column1 is to the left of column2
		left_degrees(180); //Rotate robot left by 100 degrees
		_delay_ms(1000);
		forward_mm(200); //Moves robot forward 100mm
		_delay_ms(1000);
		right_degrees(180); //Rotate robot right by 180 degrees
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}

	if (UCSR0A & (1<<RXC0))
	{
		// update column1, row1, column2, and row2 with the next set of column and row addresses received
		
		row1 = row2;
		_delay_ms(1000);
		lcd_print(2,1,row1,3);
		
		column1 = column2;
		_delay_ms(1000);
		lcd_print(2,5,column1,3);
		
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

void check_pin()
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
}
void movenear_robot()
{
	if ((row1==row2) && (column1<column2))
	{ // if column1 is to the left of column2
		left_degrees(90); //Rotate robot left by 90 degrees
		_delay_ms(1000);
		forward_mm(100); //Moves robot forward 100mm
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1==row2) && (column1>column2))
	{ // if column1 is to the left of column2
		right_degrees(90); //Rotate robot right by 90 degrees
		_delay_ms(1000);
		forward_mm(100); //Moves robot forward 100mm
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1<row2) && (column1==column2))
	{ // if column1 is to the left of column2
		forward_mm(100); //Moves robot forward 100mm
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1>row2) && (column1==column2))
	{ // if column1 is to the left of column2
		left_degrees(180); //Rotate robot left by 100 degrees
		_delay_ms(1000);
		forward_mm(100); //Moves robot forward 100mm
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
}

void moveaway_robot()
{
	if ((row1==row2) && (column1<column2))
	{ // if column1 is to the left of column2
		back_mm(100); //Moves robot back 100mm
		_delay_ms(1000);
		right_degrees(90); //Rotate robot left by 90 degrees
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1==row2) && (column1>column2))
	{ // if column1 is to the left of column2
		back_mm(100); //Moves robot back 100mm
		_delay_ms(1000);
		left_degrees(90); //Rotate robot right by 90 degrees
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1<row2) && (column1==column2))
	{ // if column1 is to the left of column2
		back_mm(100); //Moves robot back 100mm
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
	else if ((row1>row2) && (column1==column2))
	{ // if column1 is to the left of column2
		back_mm(100); //Moves robot back 100mm
		_delay_ms(1000);
		left_degrees(180); //Rotate robot left by 100 degrees
		_delay_ms(1000);
		stop();
		_delay_ms(1000);
	}
}

int main(void)
{
	USART_Init(); // initialize UART communication
	unsigned char i = 0;
	init_devices();
	lcd_set_4bit();
	lcd_init();
	
	lift();
	_delay_ms(2000);
	grip();
	_delay_ms(2000);
	
	sharp = ADC_Conversion(11);						//Stores the Analog value of front sharp connected to ADC channel 11 into variable "sharp"
	value = Sharp_GP2D12_estimation(sharp);				//Stores Distance calsulated in a variable "value".
	lcd_print(1,7,value,3);
	
	start_row = USART_Receive(); // receive data from UART
	_delay_ms(1000);
	lcd_print(2,1,start_row,3);
	
	USART_Transmit('0');
	start_column = USART_Receive(); // receive data from UART
	_delay_ms(1000);
	lcd_print(2,5,start_column,3);
	
	USART_Transmit('1');
	goal_row = USART_Receive(); // receive data from UART
	_delay_ms(1000);
	lcd_print(2,10,goal_row,3);
	
	USART_Transmit('0');
	goal_column = USART_Receive(); // receive data from UART
	_delay_ms(1000);
	lcd_print(2,14,goal_column,3);

	while(1)
	{
		//lcd_cursor(1,6);
		//lcd_string("TEAM 6");
		
		if (UCSR0A & (1<<RXC0))
		{
			check_pin();

			while ((goal_column != column2) || (goal_row != row2))
			{
				move_robot ();
			}
			movenear_robot();
			
			down();
			_delay_ms(2000);
			ungrip();
			_delay_ms(2000);
			grip();
			_delay_ms(2000);
			lift();
			_delay_ms(2000);
			
			moveaway_robot();
			
			check_pin();
			
			while ((start_column != column2) || (start_row != row2))
			{
				move_robot ();
			}
			
			down();
			_delay_ms(2000);
			ungrip();
			_delay_ms(2000);
			lift();
			_delay_ms(2000);
			grip();
			_delay_ms(2000);
		}
	}
}