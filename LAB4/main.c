#include "stm32g0xx.h"
#define DELAY_MS 500

static volatile uint32_t tick = 0;

void delay_ms(volatile uint32_t s){
	tick = s;
	while(tick);
}

void SysTick_Handler(void){
	if (tick > 0) {
		--tick;
	}
}

int main(void){
    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*6);
    GPIOC->MODER |= (1U << 2*6);

    /* Turn on LED */
    GPIOC->ODR |= (1U << 6);

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000); // 1ms systick
	while(1) {
	        delay_ms(500);
	        /* Toggle LED */
	        GPIOC->ODR ^= (1U << 6);
	    }

    return 0;
}
