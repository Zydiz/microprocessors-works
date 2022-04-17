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
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000); // 1ms systick
	delay_ms(DELAY_MS);

    while(1);

    return 0;
}
