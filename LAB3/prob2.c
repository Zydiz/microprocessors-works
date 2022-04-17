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

int main(void) {
    /* Enable GPIOA clock */
    RCC->IOPENR |= (1U << 0);

    /* Setup PA6 as output */
    GPIOA->MODER &= ~(3U << 2*6);
    GPIOA->MODER |= (1U << 2*6);

    /* Setup PA7 as input */
    GPIOA->MODER &= ~(3U << 2*7);

    /* Turn off the LED */
    GPIOA->ODR &= ~(1U << 6);

    while(1) {
        switch (current_mode){
            case mode0:
                s = 0;
                if ((GPIOA->IDR >> 7) & (1U)){
                    current_mode = mode1;
                    delay(500); /* a small delay for non repeating button presses */
                    break;
                }
                GPIOA->ODR &= ~(1U << 6); /* Turning the LED off */
                break;

            case mode1:
                s = 4800000; /* 2 sec delay */
                if ((GPIOA->IDR >> 7) & (1U)){
                    current_mode = mode2;
                    delay(500);
                    break;
                }
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode2:
                s = 2400000; /* 1 sec delay */
                if ((GPIOA->IDR >> 7) & (1U)){
                    current_mode = mode3;
                    delay(500);
                    break;
                }
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode3:
                s = 1200000; /* 0.5 sec delay */
                if ((GPIOA->IDR >> 7) & (1U)){
                    current_mode = mode4;
                    delay(500);
                    break;
                }
                delay(s);
                GPIOA->ODR ^= (1U << 6);
                break;

            case mode4:
                s = 240000; /* 0.1 sec delay */
                if ((GPIOA->IDR >> 7) & (1U)){
                    current_mode = mode5;
                    delay(500);
                    break;
                }
                delay(s);
                GPIOA->ODR ^= (1U << 6); /* Turning the LED on */
                break;

            case mode5:
                s = 0;
                if ((GPIOA->IDR >> 7) & (1U)){
                    current_mode = mode0;
                    delay(500);
                    break;
                }
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

