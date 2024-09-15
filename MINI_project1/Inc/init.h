#ifndef INIT_H_
#define INIT_H_

/*******************************************************************************
* Definitions
******************************************************************************/

#define PORT_SW_1		GPIOB
#define PIN_SW_1		7

#define PORT_SW_2		GPIOB
#define PIN_SW_2		3

#define PORT_SW_3		GPIOB
#define PIN_SW_3		4

#define PORT_SW_4		GPIOB
#define PIN_SW_4		5

#define PORT_LED_1		GPIOC
#define PIN_LED_1		6

#define PORT_LED_2		GPIOC	//TIM3_CH2
#define PIN_LED_2		7

#define PORT_LED_3		GPIOC	//TIM3_CH3
#define PIN_LED_3		8

#define PORT_LED_4		GPIOC	//TIM3_CH4
#define PIN_LED_4		9

#define PORT_BUZZER		GPIOC
#define PIN_BUZZER		10

/*******************************************************************************
* Prototypes
******************************************************************************/

/**
 * @brief: Function to set mode of GPIO
 * 
 * @param gpio: Pointer to GPIOx address 
 * @param pin: Pin number 
 * @param type: Operation mode 
 * 
 * @return: None
 */
void setMODE(GPIO_TypeDef *gpio, uint32_t pin, uint8_t type);

/**
 * @brief: Function to set pull mode for Input
 * 
 * @param gpio: Pointer to GPIOx address 
 * @param pin: Pin number 
 * @param type: Operation mode 
 * 
 * @return: None
 */
void setPULL(GPIO_TypeDef *gpio, uint32_t pin, uint8_t type);

/**
 * @brief: Function to set alternate function mode for GPIOx
 * 
 * @param gpio: Pointer to GPIOx address 
 * @param pin: Pin number 
 * @param type: Operation mode 
 * 
 * @return: None
 */
void setALT(GPIO_TypeDef *gpio, uint32_t pin, uint8_t AFx);

/**
 * @brief: Function to initialize TIMER3
 * 
 * @param : None
 * 
 * @return: None
 */
void initTIM3(void);

/**
 * @brief: Function to set duty cycle for PWM
 * 
 * @param freq: Frequency
 * @param duty_cycle: Duty cycle 
 * @param channel: Timer channel
 * 
 * @return: None
 */
void tim3_dutycycle(uint32_t freq, uint32_t duty_cycle, uint8_t channel);

/**
 * @brief: Function to initialize TIMER3
 * 
 * @param : None
 * 
 * @return: None
 */
void usart2_config(void);

/**
 * @brief: Function to write data using UART
 * 
 * @param buffer: Pointer to array
 * @param len: Length of data
 * 
 * @return: None
 */
void writeDataUSART(char buffer[], uint16_t len);

/**
 * @brief: Function to initialize Switch
 * 
 * @param : None
 * 
 * @return: None
 */
void switchInit(void);

/**
 * @brief: Function to initialize LED
 * 
 * @param : None
 * 
 * @return: None
 */
void LEDInit(void);

/**
 * @brief: Function to initialize Buzzer
 * 
 * @param : None
 * 
 * @return: None
 */
void buzzerInit(void);

/**
 * @brief: Function to initialize ADC
 * 
 * @param : None
 * 
 * @return: None
 */
void adc_init(void);

/**
 * @brief: Function to read ADC value
 * 
 * @param : None
 * 
 * @return: ADC value
 */
uint16_t adc_read(void);

/**
 * @brief: Function to initialize a delay
 * 
 * @param t: delay time (ms)
 * 
 * @return: None
 */
void delay_ms(uint32_t t);

#endif /* INIT_H_ */
