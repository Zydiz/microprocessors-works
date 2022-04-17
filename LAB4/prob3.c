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

static volatile uint32_t tick = 0, del_len = 0, min = 0, sec = 0;

/* ----------- function prototypes ----------- */

void delay_ms(volatile uint32_t s);
void min_sec(void);
void ssd_clear(void);
void ssd_printdigit(volatile uint32_t s);
void ssd_print(volatile uint32_t min_f, volatile uint32_t sec_f);
void init_func(void);

/* ----------- handlers ----------- */

void EXTI0_1_IRQHandler (void) {
    current_mode = current_mode + 1;
    if (current_mode == 6){
        current_mode = 0;
    }
    EXTI->RPR1 |= (1U << 7); /* pending reset */
}

void TIM1_BRK_UP_TRG_COM_IRQHandler (void) /* Configuring TIM1 */
{
    min_sec();
    if(tick > s){
        //TODO finish func
    }
    tick--;
    TIM1->SR &= ~(1U << 0); /* Clear pending status */
}


/* ----------- main function ----------- */

int main(void){
	//FIXME no delays here
	init_func();
	ssd_clear();

	while(1) {
        switch (current_mode){
            case mode0:
                ssd_print(min, sec);
                break;

            case mode1:
                TIM1->PSC = 15; /* Setting prescaler as 16000 to achieve a millisecond on my delay function */
                ssd_print(min, sec);
                break;

            case mode2:
                TIM1->PSC = 30; /* Setting prescaler as 16000 to achieve a millisecond on my delay function */
                ssd_print(min, sec);
                break;

            case mode3:
                TIM1->PSC = 159; /* 100ms delay */
                ssd_print(min, sec);
                break;

            case mode4:
                TIM1->PSC = 1599; /* 10ms delay */
                ssd_print(min, sec);
                break;

            case mode5:
                TIM1->PSC = 15999; /* millisecond delay */
                ssd_print(min, sec);
                break;

            default:
                /* lorem ipsum dolor */
                break;
        }
    }
    return 0;
}

/* ----------- function definitions ----------- */

void delay_ms(volatile uint32_t s){ /* Volatile for optimization prevention */
	tick = s;
	while(tick);
}

/* Manipulates the minute and second variables */
void min_sec(void){
	if (!(min || sec)){ /* if both are zero */
		min = 23;
		sec = 59;
	}
	else if (sec == 0){ /* if sec is 0 */
		min--;
		sec = 59;
	}
	sec--;
}

void ssd_clear(void){
    GPIOA->ODR &= ~(0xFU << 4); /* Clear digits */
    GPIOB->ODR |= (0xFFU); /* Clear segments */
}

/* Prints the values depending on the number */
void ssd_printdigit(volatile uint32_t s){
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

void ssd_print(volatile uint32_t min_f, volatile uint32_t sec_f){
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

void init_func(void){
	/* Enable GPIOA and GPIOB clock */
    RCC->IOPENR |= (3U); /* 3U = 11 */

    /* Set-up timer TIM1 */
    RCC->APBENR2 |= (1U << 11); /* Enable TIM1 Clock */
    TIM1->CR1 = 0; /* Clearing the control register */
    TIM1->CR1 |= (1U << 7);  /* Auto Reload Preload Enable */
    TIM1->CNT = 0; /* Zero the counter */
    TIM1->PSC = 15999; /* Setting prescaler as 16000 to achieve a millisecond on my delay function */
    TIM1->ARR = 1;
    TIM1->DIER |= (1U << 0); /* Updating interrupt enabler */
    TIM1->CR1 |= (1U << 0); /* Enable TIM1 */
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1); /* Setting priority */
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); /* Enabling interrupt */

	/* Seven Segment Display */
    /* Set PB0 to PB7 as output to use as A B C D E F G and decimal point pins of the SSD */
    GPIOB->MODER &= ~(0xFFFFU); /* Setting first 16 bits as zero */
    GPIOB->MODER |= (0x5555U); /* Setting odd bits as one */
    /* Set PA4 to PA7 as output to use as D1 D2 D3 D4 pins of the SSD */
    GPIOA->MODER &= ~(0xFF00U); /* Setting bits 8th to 16th as zero */
    GPIOA->MODER |= (0x5500U); /* Setting odd bits as one from 8th to 16th */

    /* Mode switch button */
    /* Setup PA0 as input */
    GPIOA->MODER &= ~(0x3U);
    /* Setting up the interrupt operation for PA0 */
    EXTI->RTSR1 |= (1U); /* Setting the trigger as rising edge */
    EXTI->EXTICR[0] &= ~(1U << 8*0); /* EXTICR 0 for 0_1 */
    EXTI->IMR1 |= (1U << 0); /* Interrupt mask register */
    EXTI->RPR1 |= (1U << 0); /* Rising edge pending register, Clearing pending PA0 */
    NVIC_SetPriority(EXTI0_1_IRQn, 1); /* Setting priority */
    NVIC_EnableIRQ(EXTI0_1_IRQn); /* Enabling the interrupt function */

	SystemCoreClockUpdate(); // Update processor clock
	SysTick_Config(SystemCoreClock / 1000); // 1mS systick
}

//problem 3
#include "stm32g0xx.h"
#include "bsp.h"

#define BUTTON_DELAY 500
#define MAX_MINUTE 24
#define MAX_SECOND 60
enum count_timer_values
{
    time1 = 999, //mode1
    time2 = 499, //mode2
    time3 = 99,  //mode3
    time4 = 9,   //mode4
    time5 = 0    //mode5
};

volatile uint32_t SSD_number[] =
{
    0x3F, //0
    0x06, //1
    0x5B, //2
    0x4F, //3
    0x66, //4
    0x6D, //5
    0x7D, //6
    0x07, //7
    0x7F, //8
    0x6F  //9
};

volatile uint32_t digit_display_number[] = {0x3F, 0x3F, 0x3F, 0x3F};
volatile uint32_t SSD_common_pins[] = {0x0010, 0x0020, 0x0040, 0x0080};
volatile uint32_t temp_digit_display_number;
volatile uint32_t seconds;
volatile uint32_t minutes;
volatile uint32_t mode = 0;
volatile uint32_t counter_button = 0;
volatile uint32_t count_digit = 0;

void EXTI4_15_IRQHandler()
{

    if ((counter_button >= BUTTON_DELAY) && ((EXTI->RPR1 >> 8) & 1))
    {
        //Mode select
        seconds = 0;
        minutes = 0;

        if (mode >= 5)
        {
            mode = 0;
        }
        else
        {
            mode++;
        }

        counter_button = 0;
    }
    EXTI->RPR1 |= (1U << 8);
}
void TIM1_BRK_UP_TRG_COM_IRQHandler()
{

    digit_display_number[0] = SSD_number[(MAX_MINUTE - 1 - minutes) / 10];
    digit_display_number[1] = SSD_number[(MAX_MINUTE - 1 - minutes) % 10];
    digit_display_number[2] = SSD_number[(MAX_SECOND - 1 - seconds) / 10];
    digit_display_number[3] = SSD_number[(MAX_SECOND - 1 - seconds) % 10];

    seconds++;
    if (seconds == MAX_SECOND)
    {
        seconds = 0;
        minutes++;
        if (minutes == MAX_MINUTE)
        {
            minutes = 0;
        }
    }
    BSP_IO_pin_write(1, 7, 2);

    TIM1->SR &= ~(1U << 0);
}

void SysTick_Handler()
{
    switch (mode)
    {
    case 0:
        TIM1->CR1 &= ~(1U << 0);
        break;
    case 1:
        TIM1->CR1 |= (1U << 0);
        TIM1->PSC = time1;
        break;
    case 2:
        TIM1->PSC = time2;
        break;
    case 3:
        TIM1->PSC = time3;
        break;
    case 4:
        TIM1->PSC = time4;
        break;
    case 5:
        TIM1->PSC = time5;
        break;
    }
    if (count_digit >= 4)
    {
        count_digit = 0;
    }

    GPIOB->ODR = digit_display_number[count_digit];
    GPIOA->ODR = ~SSD_common_pins[count_digit];

    count_digit++;

    if (counter_button <= BUTTON_DELAY)
    {
        counter_button++;
    }
}

int main(void)
{

    for (uint32_t i = 0; i < 8; i++)
    { //SSD led pins PB0-PB7 output initialization
            BSP_IO_pin_init(1, i, 1, 0);
    }
    for (uint32_t i = 4; i < 8; i++)
    { //SSD common pins PA4-PA7 output
        initial(PA4 - PA7 = > D1 - D4)
            BSP_IO_pin_init(0, i, 1, 0);
    }
    BSP_IO_pin_init(1, 8, 1, 2); //button pin PB8 input pulldown initialization
    BSP_EXTI_init(1, 8, 1, 1); //button pin PB8 interrupt initialization
    NVIC_DisableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 1);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    BSP_TIMx_init(1, 16000, time1, 0); //TIM1 initialization
    BSP_SysTick_init(1);               //SysTick 1ms initialization

    while();

    return 0;
}