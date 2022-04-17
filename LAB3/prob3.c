#include "stm32g0xx.h"

enum mode_state
{
    mode0,
    mode1,
    mode2,
    mode3,
    mode4,
    mode5,
};
enum mode_state current_mode = mode0;
volatile uint32_t s = 0;

void delay(uint32_t s);

void EXTI4_15_IRQHandler (void) {
    current_mode = current_mode + 1;
    if (current_mode == 6){
        current_mode = 0;
    }
    delay(800); /* small pause */
    EXTI->RPR1 |= (1U << 7); // TODO check if these are correctly written
}

int main(void) {
    /* Enable GPIOA clock */
    RCC->IOPENR |= (1U << 0);

    /* Setup PA6 as output */
    GPIOA->MODER &= ~(3U << 2*6);
    GPIOA->MODER |= (1U << 2*6);

    /* Setup PA7 as input */
    GPIOA->MODER &= ~(3U << 2*7);

    /* Setting up the button interrupt for PA7*/
    EXTI->EXTICR[1] |= (0U << 8*3); /* Setting for PortA */
    EXTI->RTSR1 |= (1U << 7); /* Setting the trigger as rising edge */
    EXTI->IMR1 |= (1U << 7);
    // EXTI->RPR1 |= (1U << 7);
    NVIC_SetPriority(EXTI4_15_IRQn, 3); /* Setting priority */
    NVIC_EnableIRQ(EXTI4_15_IRQn); /* Enabling the interrupt function */

    /* Turn off the LED */
    GPIOA->ODR &= ~(1U << 6);

    while(1) {
        switch (current_mode){
            case mode0:
                s = 0;
                GPIOA->ODR &= ~(1U << 6);
                break;

            case mode1:
                s = 4800000; // TODO check, find real values according to freq
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode2:
                s = 2400000;
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode3:
                s = 1200000;
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode4:
                s = 240000;
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode5:
                s = 0;
                GPIOA->ODR |= (1U << 6);
                break;
            default:
                /* lorem ipsum dolor */
                break;
        }
    }
    return 0;
}

void delay(uint32_t s) {
    for(; s>0; s--);
}
