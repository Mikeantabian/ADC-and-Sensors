/*--------------------------INCLUDE HEADER FILES---------------------*/
#include "TM4C123GH6PM.h"
# include <stdbool.h>
# include <stdio.h>

/*--------------------------USER FUNCTION DECLARATIONS---------------*/
/*Function 1: Delay in milliseconds (50 MHz CPU Clock)*/
void delayMs(int ms);
/*Function 2: LCD Nibble Write Function*/
void LCD_nibble_write(char data, unsigned char control);
/*Function 3: LCD Write Command Function */
void LCD_command(unsigned char command);
/*Function 4: LCD Write Character Data Function  */
void LCD_data(char data);
/*Function 5: LCD Initialization and Configuration  */
void LCD_init(void);
/*Function 6: GPIO ports Initialization and Configuration  */
void PORTS_init(void);
/*Function 7: write sensor names  */
void writeSensorName(int caseNum);
/*Function 8: Handler function for GPIOD  */
void GPIOD_Handler(void);

volatile int sensorIndex = 1;
volatile bool format = true;

/*--------------------------MAIN FUNCTION-----------------------------*/
int main(void){
	/*------------------------INITIALIZE /CONFIGURE PERIPHERALS ----------*/
	/*Such as GPIO, UARTs, ADC, Timer, Interrupt, etc*/
	
	/*  */
	int potent_dig, photosensor_dig, temperature_dig;
	
	
	/* Configure ADC */
	SYSCTL->RCGCADC |= 1; /* Enable clock to ADC module 0 */
	SYSCTL->RCGCGPIO |= 0x10; /* Enable clock to GPIOE */
	
	/* Enable alternate functionality  of PE1(light sensor), 
	PE2(potentiometer), PE5(temperature sensor) */
	GPIOE-> AFSEL |= 0x26; 
	GPIOE->DEN &= ~0x26; /* disabling the digital functionality */
	GPIOE->AMSEL |= 0x26; /* enabling analog functionality */
	
	ADC0->ACTSS &= ~1; /* Disable the sample sequencer */
	ADC0->EMUX &= ~0x000F; /* clearing the 4 LSB enables the software trigger */
	ADC0->SSMUX0 &= ~0xFFFFFFFF; /* Clear all the bits to ensure a clean slate */
	ADC0->SSMUX0 |= 0x00000821; /* Choose channel 1 2 and 8 for the samples */
	ADC0->SSCTL0 |= 0x00000600; /* raw interrupt at third samples conversion */
	ADC0->ACTSS |= 1; /* Enable the sample sequencer */
	
	SYSCTL->RCGCGPIO |= 0x08; /* enable clock to GPIOD */
	GPIOD->DIR &= ~0x0F; /* set SW2 as input pin */
	GPIOD->AMSEL &= ~0x0F; /* disable analog functionality */
	GPIOD->DEN |= 0x0F; /* enable digital functionality */
	
	/* Configure GPIO interrupt */
	GPIOD->IS &= ~0x0F; /* make bit 3-0 edge sensitive */
	GPIOD->IBE &= ~0x0F; /* trigger is controlled by IEV */
	GPIOD->IEV &= ~0x0F; /* rising edge trigger */
	GPIOD->ICR |= 0x0F; /* clear any previous interrupts */
	GPIOD->IM |= 0x0F; /* Unmask Interrupt */
	
	NVIC->IP[3] = 1 << 5; /* sets the priority of the interrupt to 1 */
	NVIC->ISER[0] |= 0x08; /* enable IRQ3 */
	
	/*  */

	/*--------------------------Main Tasks----------------------*/
	/*To keep the program running*/
	/*Main logic of the program goes inside the loop */
	
	unsigned int nelms, i;
	double Temp_reading, Potent_reading, Light_reading;
	char reading_str[16] = {};
	LCD_init();
	
	for(;;){
		
		ADC0-> PSSI |= 1;
		while ((ADC0->RIS & 1) == 0); /* Do all three sample conversions */
		/* Save digital values from conversion to int varaiables */
		potent_dig = ADC0 -> SSFIFO0;
		photosensor_dig = ADC0 -> SSFIFO0;
		temperature_dig = ADC0 -> SSFIFO0;
		ADC0->ISC = 1; /* Clear interrupt flag */
		
		nelms = sizeof(reading_str)/sizeof(reading_str[0]); /* Find the number of elements */
		
		switch(sensorIndex) {
			case 0: /* Potentiometer */
				Potent_reading = (potent_dig * 3.3) / 4095; /* Convert value from Digital to Analog */
				sprintf(reading_str, "%.3f", Potent_reading); /* store char reading in array */
				LCD_command(1); /* Clear LCD */
				writeSensorName(0); /* Write Potentiometer */
				LCD_command(0xC0); /* Go to new line */
				
				
				for(i = 0; i< nelms; i++){
					if(reading_str[i] == 0){
						break;
					}
					LCD_data(reading_str[i]);
				}
				LCD_data(' ');
				LCD_data('V');
				break;
			case 1: /* Light sensor */
				Light_reading = (photosensor_dig * 3.3) / 4095; /* Convert value from Digital to Analog */
				sprintf(reading_str, "%.3f", Light_reading); /* store char reading in array */
			
			
				LCD_command(1); /* Clear LCD */
				writeSensorName(1); /* Write Photosensor */
				LCD_command(0xC0); /* Go to new line */
			
				for(i = 0; i< nelms; i++){
					if(reading_str[i] == 0){
						break;
					}
					LCD_data(reading_str[i]);
				}
				LCD_data(' ');
				LCD_data('V');
				break;
			case 2: /* Temperature sensor */
				Temp_reading = (temperature_dig * 3.3) / 4095;
				Temp_reading = Temp_reading * 100;
			
				if(format){
					Temp_reading = (Temp_reading * 9.0 / 5.0) + 32.0;
				}
			
				sprintf(reading_str, "%.1f", Temp_reading); /* store char reading in array */
			
				LCD_command(1); /* Clear LCD */
				writeSensorName(2); /* Write Temperature */
				LCD_command(0xC0); /* Go to new line */
			
				for(i = 0; i< nelms; i++){
					if(reading_str[i] == 0){
						break;
					}
					LCD_data(reading_str[i]);
				}
				LCD_data(' ');
				
				if(format){
					LCD_data('F');
				} else {
					LCD_data('C');
				}
				break;
		}
		delayMs(1000);
	}
											
}


/*-------------------USER FUNCTION DEFINITIONS---------------------------*/
/*Function 1: Delay in milliseconds (50 MHz CPU Clock)*/
void delayMs(int ms) {
	int i, j;
	for(i = 0 ; i < ms; i++)
		for(j = 0; j < 2850; j++)
			{}   /* do nothing for 1 ms */
}

/*Function 2: LCD Nibble Write Function*/
void LCD_nibble_write(char data, unsigned char control){
	GPIOA->DIR |= 0x3C; /* set PA2-PA5 as outputs */
	GPIOA->DATA &= ~0x3C; /* clearing the line */
	GPIOA->DATA |= (data & 0xF0) >> 2;  /* extract the upper nibble */
	
	/* set RS bit */
	if(control & 1) {
		GPIOE->DATA |= 1; /* Check if LCD data is data */
	} else {
		GPIOE->DATA &= ~1; /*Check if LCD data is command  */
	}
	
	/* High to Low transition pulse on LCD enable pin */
	delayMs(0);
	GPIOC->DATA |= 1 << 6;
	delayMs(0);
	GPIOC->DATA &= ~(1 << 6);
	delayMs(0);
	
	GPIOA->DATA &= ~0x3C; /* clearing the line */
	delayMs(1);
}

/*Function 3: LCD Write Command Function */
/* RS=1 since we are sending commmand and not data */
void LCD_command(unsigned char command){
	LCD_nibble_write(command & 0xF0, 0); /* upper nibble */
	LCD_nibble_write(command << 4, 0); /* lower nibble */
	
	if(command < 3) {
		delayMs(2); /* maximum delay of 2 ms for the first 2 instructions */
	} else {
		delayMs(1); /* maximum delay of 1 ms for other */
	}
}

/*Function 4: LCD Write Character Data Function  */
/* RS=1 since we are sending data and not a command */
void LCD_data(char data){
	LCD_nibble_write(data & 0xF0, 1); /* upper nibble */
	LCD_nibble_write(data << 4, 1); /* lower nibble */
	delayMs(1);
}

/*Function 5: LCD Initialization and Configuration  */
void LCD_init(void){
	PORTS_init();
	/* Initialization sequence */
	delayMs(20);
	LCD_nibble_write(0x30, 0);
	delayMs(5);
	LCD_nibble_write(0x30, 0);
	delayMs(1);
	LCD_nibble_write(0x30, 0);
	delayMs(1);
	
	/* Configuration */
	LCD_nibble_write(0x20, 0); /* Use 4-bit data mode */
	delayMs(1);
	LCD_command(0x28); /* Set 4 bit data, 2-line, 5x7 font */
	LCD_command(0x06); /* move cursor right */
	LCD_command(0x01); /* clear screen, move cursor to home */
	LCD_command(0x0C); /* turn on LCD */
}

/*Function 6: GPIO ports Initialization and Configuration  */
void PORTS_init(void){
	SYSCTL->RCGCGPIO |= 0x01; /* Enable clock to Port A */
	SYSCTL->RCGCGPIO |= 0x10; /* Enable clock to Port E*/
	SYSCTL->RCGCGPIO |= 0x04; /* Enable clock to Port C */
	
	GPIOA->AMSEL &= ~0x3C; /* Turn off Analog functionality */
	GPIOA->DATA &= ~0x3C; /* Clear data for clean slate */
	GPIOA->DIR |= 0x3C; /* Set PA5-2 as output pins */
	GPIOA->DEN |= 0x3C; /* Set PA5-2 as digital functionality */
	
	/* PE0 handles the RS pin which decides between data or commands */
	GPIOE->AMSEL &= ~0x01; /* Disable Analog functionaly for PE0 */
	GPIOE->DIR |= 0x01; /* Set PE0 as output */
	GPIOE->DEN |= 0x01; /* Set PE0 digital functionality */
	GPIOE->DATA &= ~0x01; /* Set high to make incoming data into display data */
	
	/* Configure LCD Enable pin using PC6 */
	GPIOC->AMSEL &= ~0x40; /* Disable analog functionality for PC6 */
	GPIOC->DIR |= 0x40; /* Set PC6 as output */
	GPIOC->DEN |= 0x40; /* Set PC6 with digital functionality */
	GPIOC->DATA &= ~0x40; /* Clear PC6, disabling the LCD panel */
}

void GPIOD_Handler(void){
	unsigned int data;
	data = GPIOD->MIS & 0x0F; /* button pressed */
	
	switch(data){
		case 1:  
			/* do nothing */
			delayMs(10);
			GPIOD->ICR |= 0x01; /* clear interrupt flag */
			break;
		case 2:  /* SW4: switch temperature format */
			delayMs(10);
			format = !format;
			GPIOD->ICR |= 0x02; /* clear interrupt flag */
			break; 
		case 4:  /* SW3: Toggle LED: On or Off*/
			/* do nothing */
			delayMs(10);
			GPIOD->ICR |= 0x04; /* clear interrupt flag */
			break;
		case 8:  /* SW2: change */
			delayMs(10);
			sensorIndex = (sensorIndex + 1) % 3; /* loop: 0-1-2->0 */
			GPIOD->ICR |= 0x08; /* clear interrupt flag */
			break;
	}
}



void writeSensorName(int caseNum){
	switch(caseNum){
		case 0: /* Potentiometer */
			LCD_data('P');
			LCD_data('O');
			LCD_data('T');
			LCD_data('E');
			LCD_data('N');
			LCD_data('T');
			LCD_data('I');
			LCD_data('O');
			LCD_data('M');
			LCD_data('E');
			LCD_data('T');
			LCD_data('E');
			LCD_data('R');
			break;
		case 1: /* Light */
			LCD_data('P');
			LCD_data('H');
			LCD_data('O');
			LCD_data('T');
			LCD_data('O');
			LCD_data('S');
			LCD_data('E');
			LCD_data('N');
			LCD_data('S');
			LCD_data('O');
			LCD_data('R');
			break;
		case 2: /* Temperature */
			LCD_data('T');
			LCD_data('E');
			LCD_data('M');
			LCD_data('P');
			LCD_data('E');
			LCD_data('R');
			LCD_data('A');
			LCD_data('T');
			LCD_data('U');
			LCD_data('R');
			LCD_data('E');
			break;
	}
}