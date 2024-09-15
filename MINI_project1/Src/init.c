#include <stdint.h>
#include "stm32f405xx.h"
#include "init.h"
#include "lcd.h"

/*******************************************************************************
* Definitions
******************************************************************************/

/*******************************************************************************
* Prototypes
******************************************************************************/

/*******************************************************************************
* Variables
******************************************************************************/

/*******************************************************************************
* Code
******************************************************************************/

void setMODE(GPIO_TypeDef *gpio, uint32_t pin, uint8_t type){
	gpio->MODER &= ~(3<<(pin*2));   // Clear two bit set mode
	gpio->MODER |= (type<<(pin*2)); // Set mode
}

void setPULL(GPIO_TypeDef *gpio, uint32_t pin, uint8_t type){
	gpio->PUPDR &= ~(3<<(pin*2)); // Clear two bit set mode
	gpio->PUPDR |= (type<<(pin*2)); // Set mode
}

void setALT(GPIO_TypeDef *gpio, uint32_t pin, uint8_t AFx){
	if(pin < 8){
		gpio->AFR[0] &= ~(0xF << (pin*4)); // Clear 4 bit config alternate fuction
		gpio->AFR[0] |= (AFx << (pin*4)); // Set AFR
	}
	else {
		gpio->AFR[1] &= ~(0xF << ((pin-8)*4)); // Clear 4 bit config alternate fuction
		gpio->AFR[1] |= (AFx << ((pin-8)*4)); // Set AFR
	}
}

void initTIM3(void){
	RCC->APB1ENR |= (1<<1); 	// Enable LED
	TIM3->PSC = 0; 				// Prescale = 0 -> 16MHz
	TIM3->ARR = 1600-1; 		// Frequency 1600/16x10^6 = 0.1ms
	TIM3->CNT = 0; 				// Clear counter
	TIM3->CCMR1 |= (6<<12); 	// PWM mode 1 channel 2
	TIM3->CCMR2 |= (6<<4); 		// PWM mode 1 channel 3
	TIM3->CCMR2 |= (6<<12); 	// PWM mode 1 channel 4
	TIM3->CR1 = (1<<0);
}

void tim3_dutycycle(uint32_t freq, uint32_t duty_cycle, uint8_t channel){
	TIM3->ARR = ((16000000/freq) - 1);
	switch(channel){
	case 1:
		TIM3->CCR1 = (duty_cycle*(TIM3->ARR+1))/100 - 1;
		break;
	case 2:
		TIM3->CCR2 = (duty_cycle*(TIM3->ARR+1))/100 - 1;
		break;
	case 3:
		TIM3->CCR3 = (duty_cycle*(TIM3->ARR+1))/100 - 1;
		break;
	case 4:
		TIM3->CCR4 = (duty_cycle*(TIM3->ARR+1))/100 - 1;
		break;
	default:
		break;
	}

}

void usart2_config(void){
	RCC->APB1ENR |= (1<<17);		// Enable USART2
	RCC->AHB1ENR |= (1<<0);			// Enable Port A
	GPIOA->MODER |= (1<<5);			// Alt mode pin 2
	GPIOA->MODER |= (1<<7);			// Alt mode pin 3
	GPIOA->AFR[0] |= (7<<8);	 	// AF7
	GPIOA->AFR[0] |= (7<<12); 		// AF7

	USART2->CR1 |= (1<<13); 		// Enable usart
	// Baudrate 9600
	USART2->BRR |= (3<<0); 			// Fraction
	USART2->BRR |= (104<<4);		// Mantissa
	USART2->CR1 |= (3<<2);			// Enable TX RX
}

void writeDataUSART(char buffer[], uint16_t len){
	for(uint8_t i = 0; i < len; i++){
		USART2->DR = buffer[i];
		while((USART2->SR & (1<<6)) == 0){}
	}
}

void switchInit(void){
	RCC->AHB1ENR |= (1<<0); // Enable GPIOA
	RCC->AHB1ENR |= (1<<1); // Enable GPIOB

	setMODE(PORT_SW_1, PIN_SW_1, 0); // Set mode switch 1 input
	setMODE(PORT_SW_2, PIN_SW_2, 0); // Set mode switch 2 input
	setMODE(PORT_SW_3, PIN_SW_3, 0); // Set mode switch 3 input
	setMODE(PORT_SW_4, PIN_SW_4, 0); // Set mode switch 4 input

	setPULL(PORT_SW_1, PIN_SW_1, 2); // Set mode switch 1 pull down
	setPULL(PORT_SW_2, PIN_SW_2, 2); // Set mode switch 2 pull down
	setPULL(PORT_SW_3, PIN_SW_3, 2); // Set mode switch 3 pull down
	setPULL(PORT_SW_4, PIN_SW_4, 2); // Set mode switch 4 pull down
}

void LEDInit(void){
	RCC->AHB1ENR |= (1<<1); // Enable GPIOB
	RCC->AHB1ENR |= (1<<2); // Enable GPIOC
	setMODE(PORT_LED_1, PIN_LED_1, 1); // Set mode LED 1 output

	setMODE(PORT_LED_2, PIN_LED_2, 2); // Set mode LED 2 alternative
	setMODE(PORT_LED_3, PIN_LED_3, 2); // Set mode LED 3 alternative
	setMODE(PORT_LED_4, PIN_LED_4, 2); // Set mode LED 4 alternative

	setALT(PORT_LED_2, PIN_LED_2, 2);  // Set LED 2 AF2 -> TIM3 CH2
	setALT(PORT_LED_3, PIN_LED_3, 2);  // Set LED 3 AF2 -> TIM3 CH3
	setALT(PORT_LED_4, PIN_LED_4, 2);  // Set LED 4 AF2 -> TIM3 CH4
}

void buzzerInit(void){
	RCC->AHB1ENR |= (1<<2); // Enable GPIOC
	setMODE(PORT_BUZZER, PIN_BUZZER, 1);// Set mode BUZZER output
}

void adc_init(void)
{
	RCC->AHB1ENR|=(1<<2); 	// Enable GPIO Clock in AHB1ENR for PortC
	GPIOC->MODER|=(1<<4); 	// Set PC2 to Analog Mode
	GPIOC->MODER|=(1<<5);
	RCC->APB2ENR|=(1<<8);	// Enable clock access to ADC in APB2ENR
	ADC1->SQR3|=(0x0c<<0); 	// ADC channel & sequence selection PC2 at channel 12
	// ADC1->CR1 =(1<<8); 	// Enable Scan Mode
	ADC1->CR2|=(1<<1);  	// Enable Continuous Conversion Mode
	ADC1->CR2|=(1<<0);  	// Enable ADC module
}


uint16_t adc_read(void)
{
    ADC1->CR2|=(1<<30); 		 // Start Conversion
	while(!(ADC1->SR&(1<<1))){}  // Wait till conversion is over
	return(ADC1->DR);
}

void delay_ms(uint32_t t){
	for(uint32_t i = 0; i < t; i++)
		for(uint32_t j = 0; j < 16000; j++);
}
