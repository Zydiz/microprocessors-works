 /*
 * Author: Arda Tan GÖKER
 *
 * The code continuously checks if any of the 3 three buttons
 * are being held more than a second. Can also work simultaneously.
 *
 */

#include "stm32g0xx.h"

#define HOLD_TIM 1000

volatile uint32_t count = 0;

volatile uint32_t but1_time = 0;
volatile uint32_t but2_time = 0;
volatile uint32_t but3_time = 0;

volatile uint8_t but_processed[3] = {0, 0, 0};
volatile uint8_t but_out[3] = {0, 0, 0};

void delay_ms(uint32_t dur);

void EXTI4_15_IRQHandler (void) {
    if ((EXTI->RPR1 << 4) && 1)
    {
        GPIOC->ODR ^= (1U << 6); //TODO remove this, its for testing
        but1_time = count;
        but_processed[0] = 1;
        EXTI->RPR1 |= (1U << 4);
        return;
    }
    if ((EXTI->RPR1 << 5) && 1)
    {
        but2_time = count;
        but_processed[1] = 1;
        EXTI->RPR1 |= (1U << 5);
        return;
    }
    if ((EXTI->RPR1 << 6) && 1 )
    {
        but3_time = count;
        but_processed[2] = 1;
        EXTI->RPR1 |= (1U << 6);
        return;
    }

    //falling edge resets
    if ((EXTI->FPR1 << 4) && 1 )
    {
        but_processed[0] = 0;
    }
    if ((EXTI->FPR1 << 5) && 1 )
    {
        but_processed[1] = 0;
    }
    if ((EXTI->FPR1 << 5) && 1 )
    {
        but_processed[2] = 0;
    }
}

void SysTick_Handler(void)
{
    count++;
}

int main(void)
{
    // setup PA4-5-6 as inputs
    RCC->IOPENR |= (1U << 0);
    GPIOA->MODER &= ~(0x3FU << 2 * 4);

    // FOR TESTING
    /* Enable GPIOC clock */
    RCC->IOPENR |= (1U << 2);

    /* Setup PC6 as output */
    GPIOC->MODER &= ~(3U << 2*6);
    GPIOC->MODER |= (1U << 2*6);
    // FOR TESTING

    // setting up the button interrupt for PA4-5-6
    EXTI->EXTICR[1] |= (0U << 8 * 0);  // setting for A4
    EXTI->EXTICR[1] |= (0U << 8 * 1);  // setting for A5
    EXTI->EXTICR[1] |= (0U << 8 * 2);  // setting for A6
    EXTI->RTSR1 |= (7U << 4);  // rising edge for all the pins
    EXTI->FTSR1 |= (7U << 4);  // falling edge for all the pins

    // resetting all the used pending registers
    EXTI->RPR1 |= (7U << 4);
    EXTI->FPR1 |= (7U << 4);


    EXTI->IMR1 |= (7U << 4);

    NVIC_SetPriority(EXTI4_15_IRQn, 3);  // Setting priority
    NVIC_EnableIRQ(EXTI4_15_IRQn);

    SysTick_Config(SystemCoreClock / 1000); // tick at each 1ms (syscclk/1000)

    while (1)
    {
        if (but_processed[0] && (count - but1_time >= HOLD_TIM))
        {
            but_out[0] ^= 1U;
            but_processed[0] = 0;
            but1_time = 0;
        }
        if (but_processed[1] && (count - but2_time >= HOLD_TIM))
        {
            but_out[1] ^= 1U;
            but_processed[1] = 0;
            but2_time = 0;
        }
        if (but_processed[2] && (count - but3_time >= HOLD_TIM))
        {
            but_out[2] ^= 1U;
            but_processed[2] = 0;
            but3_time = 0;
        }
    }
    return 0;
}

void delay_ms(uint32_t dur)
{
    uint32_t first_in = count;
    while (count != (first_in + dur));
}
