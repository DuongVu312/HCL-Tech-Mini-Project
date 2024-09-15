#include <stdint.h>
#include "stm32f405xx.h"
#include "lcd.h"
#include "init.h"

/*******************************************************************************
* Definitions
******************************************************************************/

/*******************************************************************************
* Prototypes
******************************************************************************/

/**
 * @brief: Function to handle Switch1 operations
 * 
 * @param : None
 * 
 * @return: None
 */
static void handleSW1(void);

/**
 * @brief: Function to handle Switch2 operations
 * 
 * @param : None
 * 
 * @return: None
 */
static void handleSW2(void);

/**
 * @brief: Function to handle Switch3 operations
 * 
 * @param : None
 * 
 * @return: None
 */
static void handleSW3(void);

/**
 * @brief: Function to handle Switch4 operations
 * 
 * @param : None
 * 
 * @return: None
 */
static void handleSW4(void);

/*******************************************************************************
* Variables
******************************************************************************/

uint8_t state_sw1 = 0, state_sw2 = 0, state_sw3 = 0, state_sw4 = 0; 			// State of switch
uint8_t prestate_sw1 = 0, prestate_sw2 = 0, prestate_sw3 = 0, prestate_sw4 = 0; // Previous state of switch
uint8_t count_sw1 = 0, count_sw2 = 0, count_sw3 = 0, count_sw4 = 0; 			// Count of switch

/*******************************************************************************
* Code
******************************************************************************/

int main(void)
{
	uint16_t fuelLevel; // Variable to dislay fuel level

	switchInit();
	LEDInit();
	buzzerInit();
	initTIM3();
	usart2_config();
	LcdInit();
	adc_init();
	char buffer[] = {"Start\r\n"};
	writeDataUSART(buffer, sizeof(buffer)/sizeof(char));

	while(1){
		fuelLevel = adc_read()*100/4096;
		if (fuelLevel > 90)
		{
			lprint(0x80, "Fuel Level: Full");
		}
		else if (fuelLevel > 25 && fuelLevel < 90)
		{
			lprint(0x80, "Fuel Level: Good");
		}
		else
		{
			lprint(0x80, "Fuel Level:  Low");
		}
		aprint(fuelLevel, 0xC0);
		lprint(0xC4, "%");
		delay_ms(50);

		state_sw1 = PORT_SW_1->IDR & (1<<PIN_SW_1); // Read input switch 1
		state_sw2 = PORT_SW_2->IDR & (1<<PIN_SW_2); // Read input switch 2
		state_sw3 = PORT_SW_3->IDR & (1<<PIN_SW_3); // Read input switch 3
		state_sw4 = PORT_SW_4->IDR & (1<<PIN_SW_4); // Read input switch 4
		// Pheck if switch change state
		if((state_sw1 != prestate_sw1) || (state_sw2 != prestate_sw2) || (state_sw3 != prestate_sw3) || (state_sw4!=prestate_sw4)){
			delay_ms(50); // Prevent debounce
			state_sw1 = PORT_SW_1->IDR & (1<<PIN_SW_1); //Read input switch 1
			state_sw2 = PORT_SW_2->IDR & (1<<PIN_SW_2); //Read input switch 2
			state_sw3 = PORT_SW_3->IDR & (1<<PIN_SW_3); //Read input switch 3
			state_sw4 = PORT_SW_4->IDR & (1<<PIN_SW_4); //Read input switch 4
			// Check if switch change state again to avoid debounce
			if((state_sw1 != prestate_sw1) || (state_sw2 != prestate_sw2) || (state_sw3 != prestate_sw3) || (state_sw4!=prestate_sw4)){
				// Check if state switch change from LOW to HIGH
				if(state_sw1){
					count_sw1 += 1;
					count_sw1 = count_sw1 % 2; // Count value only 0 and 1 present 1st press and 2nd press
					handleSW1();
				}
				if(state_sw2){
					count_sw2 += 1;
					count_sw2 = count_sw2 % 4; // Count value 0,1,2,3 for represents four presses
					handleSW2();
				}
				if(state_sw3){
					count_sw3 += 1;
					count_sw3 = count_sw3 % 2; // Count value only 0 and 1 present 1st press and 2nd press
					handleSW3();
				}
				if(state_sw4){
					count_sw4 += 1;
					count_sw4 = count_sw4 % 2; // Count value only 0 and 1 present 1st press and 2nd press
					handleSW4();
				}
				// Update previous state of switch
				prestate_sw1 = state_sw1;
				prestate_sw2 = state_sw2;
				prestate_sw3 = state_sw3;
				prestate_sw4 = state_sw4;
			}
		}
	}
}

static void handleSW1(void){
	if(count_sw1) {
		PORT_LED_1->ODR |= (1<<PIN_LED_1); // Turn on LED 1 if count == 1
		char buffer[] = {"Ignition Status ON\r\n"};
		writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
	}
	else {
		PORT_LED_1->ODR &= ~(1<<PIN_LED_1);// Turn of LED 1 if count == 0
		char buffer[] = {"Ignition Status OFF\r\n"};
		writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
		ClrBit(PORT_EN,PIN_EN);
	}
}

static void handleSW2(void){
	if(count_sw1){
		if(count_sw2 == 1){
			char buffer[] = {"Low Beam\r\n"};
			writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
			TIM3->CCER |= (3<<4); // Enable captrue/compare 2 output
			tim3_dutycycle(1, 90, 2); // Low Beam LED 2 -> 10% dutycycle
		}
		else if(count_sw2 == 2){
			char buffer[] = {"High Beam\r\n"};
			writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
			TIM3->CCER |= (3<<4); // Enable captrue/compare 2 output
			tim3_dutycycle(1, 10, 2); // High Beam LED 2 -> 90% dutycycle
		}
		else if(count_sw2 == 3){
			if(!count_sw3 && !count_sw4){
				char buffer[] = {"Parking Light ON\r\n"};
				writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
				TIM3->CCER |= (3<<8); // Enable captrue/compare 3 output
				tim3_dutycycle(1, 50, 3); // Blink LED 3
				TIM3->CCER |= (3<<12); // Enable captrue/compare 4 output
				tim3_dutycycle(1, 50, 4);// Blink LED 4
				PORT_BUZZER->ODR |= (1<<PIN_BUZZER); //turn on buzzer
			}
		}
		else {
			char buffer[] = {"Parking Light OFF\r\n"};
			writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
			tim3_dutycycle(1, 99, 2);
			tim3_dutycycle(1, 99, 3);
			tim3_dutycycle(1, 99, 4);
			TIM3->CCER &= ~(3<<4);    // Disable captrue/compare 2 output
			TIM3->CCER &= ~(3<<8);    // Disable captrue/compare 3 output
			TIM3->CCER &= ~(3<<12);   // Disable captrue/compare 4 output
			PORT_BUZZER->ODR &= ~(1<<PIN_BUZZER); //Turn off buzzer
		}
	}
}

static void handleSW3(void){
	if(count_sw1){
		if(count_sw3){
			if(!count_sw4){
				char buffer[] = {"Right Indication ON\r\n"};
				writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
				TIM3->CCER |= (3<<8); //Enable captrue/compare 3 output
				tim3_dutycycle(1, 50, 3); //blink LED 3
				PORT_BUZZER->ODR |= (1<<PIN_BUZZER); //turn on buzzer
			}
			else{
				char buffer1[] = {"Right Indication OFF\r\n"};
				writeDataUSART(buffer1, sizeof(buffer1)/sizeof(char));
				char buffer2[] = {"Left Indication OFF\r\n"};
				writeDataUSART(buffer2, sizeof(buffer2)/sizeof(char));
				count_sw3 = 0;
				count_sw4 = 0;
				tim3_dutycycle(1, 99, 3);
				tim3_dutycycle(1, 99, 4);
				TIM3->CCER &= ~(3<<8); //disable captrue/compare 3 output
				TIM3->CCER &= ~(3<<12); //disable captrue/compare 4 output
				PORT_BUZZER->ODR &= ~(1<<PIN_BUZZER); //turn off buzzer
			}
		}
		else {
			char buffer[] = {"Right Indication OFF\r\n"};
			writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
			tim3_dutycycle(1, 99, 3);
			TIM3->CCER &= ~(3<<8); //disable captrue/compare 3 output
			PORT_BUZZER->ODR &= ~(1<<PIN_BUZZER); //turn off buzzer
		}
	}
}

static void handleSW4(void){
	if(count_sw1){
		if(count_sw4){
			if(!count_sw3){
				char buffer[] = {"Left Indication ON\r\n"};
				writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
				TIM3->CCER |= (3<<12); //Enable captrue/compare 4 output
				tim3_dutycycle(1, 50, 4);//blink LED 4
				PORT_BUZZER->ODR |= (1<<PIN_BUZZER); //turn on buzzer
			}
			else{
				count_sw3 = 0;
				count_sw4 = 0;
				tim3_dutycycle(1, 99, 3);
				tim3_dutycycle(1, 99, 4);
				TIM3->CCER &= ~(3<<8); //disable captrue/compare 3 output
				TIM3->CCER &= ~(3<<12); //disable captrue/compare 4 output
				PORT_BUZZER->ODR &= ~(1<<PIN_BUZZER); //turn off buzzer
			}
		}
		else {
			char buffer[] = {"Left Indication OFF\r\n"};
			writeDataUSART(buffer, sizeof(buffer)/sizeof(char));
			tim3_dutycycle(1, 99, 4);
			TIM3->CCER &= ~(3<<12); //disable captrue/compare 4 output
			PORT_BUZZER->ODR &= ~(1<<PIN_BUZZER); //turn off buzzer
		}
	}
}
