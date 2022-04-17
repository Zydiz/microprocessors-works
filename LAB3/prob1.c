#include "stm32g0xx.h"

#define LEDDELAY 2400000

void delay(volatile uint32_t);

int main(void) {

    /* Enable GPIOA clock */
    RCC->IOPENR |= (1U << 0);

    /* Setup PA6 as output */
    GPIOA->MODER &= ~(3U << 2*6);
    GPIOA->MODER |= (1U << 2*6);

    /* Turn on LED */
    GPIOA->ODR |= (1U << 6);

    while(1) {
        delay(LEDDELAY);
        /* Toggle LED */
        GPIOA->ODR ^= (1U << 6);
    }

    return 0;
}

void delay(volatile uint32_t s) {
    for(; s>0; s--);
}
