#include "stm32g0xx.h"

#define LEDDELAY 1600000

enum state_enum
{
    amplitude,
    frequency,
    first_input,
    next_input,
    disp_amp,
    disp_freq,
    disp_mode,
};
enum state_enum state; /* State object */
enum state_enum previous_state; /* Previous State object */

enum mode_type_enum /* Creating operation type trackers */
{
    sine,
    square,
    triangle,
    sawtooth,
    noise,
    rand
};
enum mode_type_enum mode_type; /* Operation_type object */

enum button_type_enum /* Creating button type trackers */
{
    number,
    operation
};
enum button_type_enum button_type; /* Button_type object */

// TODO ------------------------- keypad/ssd -------------------------

void keypad_check(void)
{   /* This function checks for the button row and column */
    /* Also informs about the type of button press, is it a number or an operation */

    /* Setting Row 1 (PA0) as 1, others as zero to check */
    clear_rows();
    GPIOA->ODR |= (1U);
    if((GPIOA->IDR >> 8) & (1U)) /* Checking CR1, PA8 */
    { /* C1 and R1 means button "1" */
        column = 1;
        row = 1;
        input = '1';
        newnumber = 1;
        button_type = number;
    }

    if((GPIOA->IDR >> 9) & (1U)) /* Checking CR2, PA9 */
    { /* C2 and R1 means button "2" */
        column = 2;
        row = 1;
        input = '2';
        newnumber = 2;
        button_type = number;
    }

    if((GPIOA->IDR >> 10) & (1U)) /* Checking CR3, PA10 */
    { /* C3 and R1 means button "3" */
        column = 3;
        row = 1;
        input = '3';
        newnumber = 3;
        button_type = number;
    }

    if((GPIOA->IDR >> 11) & (1U)) /* Checking CR4, PA11 */
    { /* C4 and R1 means button "A" */
        column = 4;
        row = 1;
        input = 'A';
        mode_type = amplitude;
        button_type = operation;
    }

    /* Setting Row 2 (PA1) as 1, others as zero to check */
    clear_rows();
    GPIOA->ODR |= (1U << 1);
    if((GPIOA->IDR >> 8) & (1U)) /* Checking CR1, PA8 */
    { /* C1 and R2 means button "4" */
        column = 1;
        row = 2;
        input = '4';
        newnumber = 4;
        button_type = number;
    }

    if((GPIOA->IDR >> 9) & (1U)) /* Checking CR2, PA9 */
    { /* C2 and R2 means button "5" */
        column = 2;
        row = 2;
        input = '5';
        newnumber = 5;
        button_type = number;
    }

    if((GPIOA->IDR >> 10) & (1U)) /* Checking CR3, PA10 */
    { /* C3 and R2 means button "6" */
        column = 3;
        row = 2;
        input = '6';
        newnumber = 6;
        button_type = number;
    }

    if((GPIOA->IDR >> 11) & (1U)) /* Checking CR4, PA11 */
    { /* C4 and R2 means button "B" */
        column = 4;
        row = 2;
        input = 'B';
        mode_type = frequency;
        button_type = operation;
    }

    /* Setting Row 3 (PA12) as 1, others as zero to check */
    clear_rows();
    GPIOA->ODR |= (1U << 12);
    if((GPIOA->IDR >> 8) & (1U)) /* Checking CR1, PA8 */
    { /* C1 and R3 means button "7" */
        column = 1;
        row = 3;
        input = '7';
        newnumber = 7;
        button_type = number;
    }

    if((GPIOA->IDR >> 9) & (1U)) /* Checking CR2, PA9 */
    { /* C2 and R3 means button "8" */
        column = 2;
        row = 3;
        input = '8';
        newnumber = 8;
        button_type = number;
    }

    if((GPIOA->IDR >> 10) & (1U)) /* Checking CR3, PA10 */
    { /* C3 and R3 means button "9" */
        column = 3;
        row = 3;
        input = '9';
        newnumber = 9;
        button_type = number;
    }

    if((GPIOA->IDR >> 11) & (1U)) /* Checking CR4, PA11 */
    { /* C4 and R3 means button "C" */
        // TODO mode cycle?
        column = 4;
        row = 3;
        input = 'C';
        mode_type++;
        if (mode_type == 6){
            mode_type = 0;
        }
        button_type = operation;
    }

    /* Setting Row 4 (PA15) as 1, others as zero to check */
    clear_rows();
    GPIOA->ODR |= (1U << 15);
    if((GPIOA->IDR >> 8) & (1U)) /* Checking CR1, PA8 */
    { /* C1 and R4 means button "*" */
        // TODO figure out how to deal with float
        column = 1;
        row = 4;
        input = '*';
        state = scientific;
        button_type = operation;
    }

    if((GPIOA->IDR >> 9) & (1U)) /* Checking CR2, PA9 */
    { /* C2 and R4 means button "0" */
        column = 2;
        row = 4;
        input = '0';
        newnumber = 0;
        button_type = number;
    }

    if((GPIOA->IDR >> 10) & (1U)) /* Checking CR3, PA10 */
    { /* C3 and R4 means button "#" */
        // TODO figure out enter confirmation
        column = 3;
        row = 4;
        input = '#';

        state = enter;
        button_type = operation;
    }

    if((GPIOA->IDR >> 11) & (1U)) /* Checking CR4, PA11 */
    { /* C4 and R4 means button "D" */
        // TODO display cycle?
        column = 4;
        row = 4;
        input = 'D';
        if (state < 4)
        {
            state = disp_amp;
        }else
        {
            state++;
            if (state == 7)
            {
                state = disp_amp;
            }
        }
        button_type = operation;
    }
    activate_rows(); /* Setting the rows high before leaving the check function*/
}

void EXTI4_15_IRQHandler (void)
{ // TODO fix for our case
    delaycount = 0; /* Setting up for timeout */
    keypad_check(); /* Get button information */
    /* Check for button type */
    if (button_type == number) /* Pressed button is a number */
    {
        switch(state)
        {
            case idle: /* If the last state was idle, we have to reset the number1 value */
                state=first_input; /* Since button type is number, this is first number input state */
                previous_state = idle; /* Setting Previous State */
                number1 = newnumber; /* Carry the input to the number */
                ssd_print(number1); /* Print the current number */
                break;

            case first_input:
                if(previous_state == scientific || previous_state == submenu)
                {
                    state = next_input; /* Setting State */
                    number2 = number1; /* Carry the number to secondary number */
                    number1 = newnumber; /* Carry the input to the number */
                    ssd_print(number1); /* Print the current number */
                }
                else
                {
                    previous_state = first_input; /* Setting Previous State */
                    number1 = (10 * number1) + newnumber; /* Shift and carry the input to the number */
                    ssd_print(number1); /* Print the current number */
                }
                break;

            case next_input:
                if(previous_state == next_input)
                {
                    number1 = (10 * number1) + newnumber; /* Shift and carry the input to the number */
                    ssd_print(number1); /* Print the current number */
                }
                else
                {
                   /* number2 = number1; /* Switching values */
                    state = next_input; /* Setting State */
                    number1 = newnumber; /* Carry the input to the number */
                    previous_state = next_input; /* Setting Previous State */
                    ssd_print(number1); /* Print the current number */
                }
                break;
        }
    }

    if (button_type == operation) /* Pressed button is an operation */
    {
        switch(state)
        {
            case idle:
                number1=0; /* Zeroing number */
                state=next_input; /* Setting State */
                previous_state = idle; /* Setting Previous State */
                ssd_print(number1); /* Print the current number */
                break;

            case first_input:
                state=next_input; /* Setting State */
                previous_state = first_input; /* Setting Previous State */
                number2=number1; /* Switching values */
                number1=0; /* Zeroing number to get new inputs */
                ssd_print(number1); /* Print the current number */
                break;

            case next_input:
                number2=number1; /* Switching values */
                number1=0; /* Zeroing number to get new inputs */
                previous_state = first_input; /* Setting Previous State */
                state = first_input; /* Switching state to get new input */
                ssd_print(number1); /* Print the current number */
                break;

            case scientific:
                previous_state = scientific; /* Setting Previous State */
                if(column == 4 && row == 1)
                    mode_type = logarithmic; /* Setting operation type */
                if(column == 4 && row == 2)
                    mode_type = ln; /* Setting operation type */
                if(column == 4 && row == 3)
                    mode_type = squareroot; /* Setting operation type */
                if(column == 4 && row == 4)
                    mode_type = squared; /* Setting operation type */
                if(column == 1 && row == 4)
                    state = submenu;
                else
                {   /* Something went wrong */
                    number1=0; /* Zero the number */
                    delaycount = 0; /* Set delay */
                    state = error; /* Set Error State */
                    error_function(); /* Go to error */
                }
                break;

            case submenu:
                state = next_input; /* Setting State */
                previous_state = submenu; /* Setting Previous State */
                if(column == 4 && row == 1)
                    mode_type = sinus; /* Setting operation type */
                if(column == 4 && row == 2)
                    mode_type = cosinus; /* Setting operation type */
                if(column == 4 && row == 3)
                    mode_type = tangent; /* Setting operation type */
                if(column == 4 && row == 4)
                    mode_type = cotangent; /* Setting operation type */
                if(column == 1 && row == 4)
                    number1 = 3.14 ; /* Setting number as pi */
                else
                {   /* Something went wrong */
                    number1=0; /* Zero the number */
                    delaycount = 0; /* Set delay */
                    state = error; /* Set Error State */
                    error_function(); /* Go to error */
                }
                break;

            case result:
                state = result_printing; /* Setting State */
                previous_state = result; /* Setting Previous State */
                operation_execute(); /* Execute the operations and print result */
                break;
        }
    }
    delay_ms(500); /* Delay so function does not run continiously */
    EXTI->RPR1 |= (15U << 8); /* Clearing pending PA8 to PA11 */
}

void ssd_print();

void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    // TODO lorem
}

// TODO ------------------------- basic functions -------------------------

void ssd_clear(void) /* Clear the display */
{
    GPIOA->ODR &= ~(15U << 4); /* Clear Digits */
    GPIOB->ODR |= (255U); /* Clear A B C... */
}

/* Prints the values depending on the number */
void ssd_printdigit(volatile uint32_t s)
{
    switch(s)
    {
        case 0: /* Printing 0 */
            GPIOB->ODR &= (0xC0U);
            break;

        case 1: /* Printing 1 */
            GPIOB->ODR &= (0xF9U);
            break;

        case 2: /* Printing 2 */
            GPIOB->ODR &= (0xA4U);
            break;

        case 3: /* Printing 3 */
            GPIOB->ODR &= (0xB0U);
            break;

        case 4: /* Printing 4 */
            GPIOB->ODR &= (0x99U);
            break;

        case 5: /* Printing 5 */
            GPIOB->ODR &= (0x92U);
            break;

        case 6: /* Printing 6 */
            GPIOB->ODR &= (0x82U);
            break;

        case 7: /* Printing 7 */
            GPIOB->ODR &= (0xF8U);
            break;

        case 8: /* Printing 8 */
            GPIOB->ODR &= (0x80U);
            break;

        case 9: /* Printing 9 */
            GPIOB->ODR &= (0x90U);
            break;
    }
}

void ssd_print(volatile uint32_t min_f, volatile uint32_t sec_f)
{
	/* finding digit values to print */
    uint32_t d1=0, d2=0, d3=0, d4=0;

	/* first two digits */
	d1 = min_f/10;
    min_f = min_f-(d1*10);
    d2 = min_f;

	/* last two digits */
	d3 = sec_f/10;
    sec_f = sec_f-(d3*10);
    d4 = sec_f;

    ssd_clear();
    /* Print first digit */
    GPIOA->ODR |= (1U << 4); /* Set D1 High */
    ssd_printdigit(d1); /* Print Value to D1 */
    ssd_clear();

    /* Print second digit */
    GPIOA->ODR |= (1U << 5);
    ssd_printdigit(d2);
    ssd_clear();

    /* Print third digit */
    GPIOA->ODR |= (1U << 6);
    ssd_printdigit(d3);
    ssd_clear();

    /* Print fourth digit */
    GPIOA->ODR |= (1U << 7);
    ssd_printdigit(d4);
    ssd_clear();
}

void delay_ms(volatile uint32_t s) /* Set as volatile to prevent optimization */
{
    delaycount = 0; /* Set counter as zero */
    while(1) /* Continuously checks the counter */
    {
        if(delaycount == s)
        return;
    }
}

void system_initialize(void) /* Setting up the board and subsystems */
{
    /* Enable GPIOA and GPIOB clock */
    RCC->IOPENR |= (3U); /* 3U = 11 */
    /* Setting up the 4x4 Keypad */
    /* Setting up the external button interrupts */
    /* Setup PA8 to PA11 as input to use as columns on the keypad */
    GPIOA->MODER &= ~(255U << 8*2); /* ~(255U) = ~(11111111) */
    GPIOA->PUPDR &= ~(85U << 8*2); /* ~(85U) = ~(01010101) */
    GPIOA->PUPDR |= (170U << 8*2); /* 170U = 10101010 */

    /* Setting up the interrupt operation for columns */
    EXTI->RTSR1 |= (15U << 8); /* Setting the trigger as rising edge */
    EXTI->EXTICR[2] &= ~(1U << 8*0); /* Setting for PA8 */
    EXTI->EXTICR[2] &= ~(1U << 8*1); /* Setting for PA9 */
    EXTI->EXTICR[2] &= ~(1U << 8*2); /* Setting for PA10 */
    EXTI->EXTICR[2] &= ~(1U << 8*3); /* Setting for PA11 */
    EXTI->IMR1 |= (15U << 8); /* Interrupt mask register */
    EXTI->RPR1 |= (15U << 8); /* Rising edge pending register, Clearing pending PA8 to PA11 */
    NVIC_SetPriority(EXTI4_15_IRQn, 1); /* Setting priority */
    NVIC_EnableIRQ(EXTI4_15_IRQn); /* Enabling the interrupt function */

    /* Setting up the pins PA0, PA1, PA12 and PA15 as output to use a rows on the keypad */
    GPIOA->MODER &= ~(3U << 0*2); /* Zero PA0 */
    GPIOA->MODER |= (1U << 0*2); /* Set first bit one on PA0 */
    GPIOA->MODER &= ~(3U << 1*2); /* Zero PA1 */
    GPIOA->MODER |= (1U << 1*2); /* Set first bit one on PA1 */
    GPIOA->MODER &= ~(3U << 12*2); /* Zero PA12 */
    GPIOA->MODER |= (1U << 12*2); /* Set first bit one on PA12 */
    GPIOA->MODER &= ~(3U << 15*2); /* Zero PA15 */
    GPIOA->MODER |= (1U << 15*2); /* Set first bit one on PA15 */
    /* Have to set the rows as high to detect an initial input */
    GPIOA->ODR |= (36867U); /* 36867U = 1001 0000 0000 0011 */

    /* Setting up the Seven Segment Display*/
    /* Set PB0 to PB7 as output to use as A B C D E F G and decimal point pins of the SSD */
    GPIOB->MODER &= ~(65535U); /* Setting first 16 bits as zero */
    GPIOB->MODER |= (21845U); /* Setting odd bits as one */
    /* Set PA4 to PA7 as output to use as D1 D2 D3 D4 pins of the SSD */
    GPIOA->MODER &= ~(65280U); /* Setting bits 8th to 16th as zero */
    GPIOA->MODER |= (21760U); /* Setting odd bits as one from 8th to 16th */

    /* Setting TIM1 */
    RCC->APBENR2 |= (1U << 11); /* Enable TIM1 Clock */
    TIM1->CR1 = 0; /* Clearing the control register */
    TIM1->CR1 |= (1U << 7);  /* Auto Reload Preload Enable */
    TIM1->CNT = 0; /* Zero the counter */
    TIM1->PSC = 15999; /* Setting prescaler as 16000 to achieve a millisecond on my delay function */
    TIM1->ARR = 1;
    TIM1->DIER |= (1U << 0); /* Updating interrupt enabler */
    TIM1->CR1 |= (1U << 0); /* Enable TIM1 */
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0); /* Setting highest priority */
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); /* Enabling interrupt */
}