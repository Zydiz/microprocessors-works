/*
 * bsp.c
 *
 *  Created on: 28 Kas 2021
 *      Authors: Gizem Bakan, Arda Tan Göker
 *
 */

#include "bsp.h"

#define BSP_GREEN_LED_PIN (6U)
#define BSP_BUTTON_PIN (2U)

volatile static uint32_t tick = 0;

void BSP_system_init() {
	__disable_irq(); // doğru mu yazılmış?
	SystemCoreClockUpdate(); // Update processor clock
	BSP_ob_led_init();
	BSP_ob_button_init();
	SysTick_Config(SystemCoreClock / 1000); // 1mS systick
	BSP_IWDG_init();
	__enable_irq();
}

// Initialize Timer3
void BSP_init_timer3(uint32_t psc, uint32_t arr) {
    RCC->APBENR1 |= (1U << 1); /* Enable TIM3 Clock */

    TIM3->CCMR1 = 1;
    TIM3->CR1 = 0; /* Clearing the control register */
    TIM3->CR1 |= (1U << 7);  /* Auto Reload Preload Enable */
    TIM3->CNT = 0; /* Zero the counter */

    TIM3->PSC = psc;
    TIM3->ARR = arr;

    TIM3->DIER |= (1U << 0); /* Updating interrupt enabler */
    TIM3->CR1 |= (1U << 0); /* Enable TIM3 */

    NVIC_SetPriority(TIM3_BRK_UP_TRG_COM_IRQn, 3); /* Setting lowest priority */
    NVIC_EnableIRQ(TIM3_BRK_UP_TRG_COM_IRQn); /* Enabling interrupt */
}

void SysTick_Handler(void){
	if (tick > 0) {
		--tick;
	}
}

void BSP_IWDG_init(void) {
	IWDG->KR = 0x5555;
	IWDG->PR = 1;
	//IWDG->RLR =
	IWDG->KR = 0xCCCC;
}

void BSP_IWDG_feed(void) {
	IWDG->KR = 0xAAAA;
}

// Initialize on-board LED connected to C6
void BSP_ob_led_init(void) {

	 /* Enable GPIOC clock */
	 RCC->IOPENR |= (1U << 2);

	 /* Setup PC6 as output */
	 GPIOC->MODER &= ~(3U << 2*BSP_GREEN_LED_PIN);
	 GPIOC->MODER |= (1U << 2*BSP_GREEN_LED_PIN);

	 /* Clear LED */
	 GPIOC->BRR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_ob_led_set(void) {
    /* Turn on LED */
    GPIOC->ODR |= (1U << BSP_GREEN_LED_PIN);
}

void BSP_ob_led_toggle(void) {
	/* Toggle LED */
	GPIOC->ODR ^= (1U << BSP_GREEN_LED_PIN);
}

void BSP_ob_led_clear(void) {
	/* Clear LED */
	GPIOC->BRR |= (1U << BSP_GREEN_LED_PIN);
}

// Initialize on-board button connected to F2
void BSP_ob_button_init() {
	 /* Enable GPIOF clock */
	 RCC->IOPENR |= (1U << 5);

	 /* Setup PF2 as output */
	 GPIOF->MODER &= ~(3U << 2*BSP_BUTTON_PIN);
}

// Returns 1 if button is pressed
int BSP_ob_button_read() {
	int b = ((GPIOF->IDR >> BSP_BUTTON_PIN) & 0x01);

	return (b) ? 0 : 1;
}
