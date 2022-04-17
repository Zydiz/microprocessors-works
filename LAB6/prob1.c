#include "stm32g0xx.h"
#include "bsp.h"
#include "stdio.h"

uint8_t adc_data; // 0 - 255
uint32_t pwm_data = 0;
void Setup_ADC(void);
void start_conversion(void);
void Setup_TIM3(void);

int main(void) {

    BSP_system_init();
    init_timer2(); //For PWM
    Setup_TIM3();
    Setup_ADC();
    start_conversion();
    while (1){

    }

    return 0;
}

void Setup_TIM3(void)
{
	/* Setup TIM3 */
	RCC->APBENR1 |= (1U << 1);

	TIM3->CR1 = 0;
	TIM3->CR1 |= (1U << 7);
	TIM3->CNT = 0;

	TIM3->CR2 |= (2U << 4);		//MMS REGISTER UPDATE MODE

	TIM3->PSC = 0;
	TIM3->ARR = 8000;	//8khz

	TIM3->DIER |= (1U << 0);
	TIM3->CR1 |= (1U << 0);
}

void start_conversion(void)
{
    ADC1->CR |= (1U << 2);
    while(!(ADC1->ISR & (1U << 2)));
}

void Setup_ADC(void)
{
	RCC->IOPENR |= (1U << 0);
	GPIOA->MODER &= ~(3U << 2 * 0);		/* Clear the MODER for PA0 */
	GPIOA->MODER |= (3U << 2 * 0);		/* Set PA0 to Analog mode */

	RCC->APBENR2 |= (1U << 20);			/* Enable ADC clock */
	ADC1->CR |= (1U << 28);	 			/* Enable the regulator */
	delay(100);							/* Wait until it is initialized  */

	ADC1->CR |= (1U << 31);				/* Enable ADC Calibration  */
	while (!(ADC1->ISR & (1 << 11)));	/* Wait until it is initialized */

	ADC1->IER |= (1U << 2);				// end of conversion interrupt enable

	ADC1->CFGR1 |= (2U << 3);		// 8-bit resolution

	ADC1->CFGR1 &= ~(1U << 13);		// Single conversion mode
	ADC1->CFGR1 |=  (1U << 16);		// Discontinuous mode

	//TIM3 TRGO
	ADC1->CFGR1 |= (3U << 6);		// TIM3_TRGO is 011 in EXTSEL
	ADC1->CFGR1 |= (1U << 10);		// Hardware trigger detection on the rising edge in EXTEN

	ADC1->CHSELR |= (1U << 0);		// ADC channel selection (PA0)
	ADC1->CFGR1 |= (1U << 23);
	ADC1->CFGR1 |= (0U << 26);

	ADC1->CR |= (1U << 0);				/* Enable ADC */
	while (!(ADC1->ISR & (1 << 0)));	/* Until ADC ready */

	NVIC_SetPriority(ADC1_IRQn, 1);
	NVIC_EnableIRQ(ADC1_IRQn);
}

void ADC_COMP_IRQHandler(void){
	adc_data = (uint8_t) (ADC1->DR);
	pwm_data = (uint32_t) (adc_data)*(4000/255);
	TIM2->CCR2 = (uint32_t) pwm_data;
}
