#ifndef BSP_H_
#define BSP_H_

#include "stm32g0xx.h"

void BSP_system_init(void);

void BSP_IWDG_init(void);
void BSP_IWDG_feed(void);

void BSP_init_timer3(uint32_t psc, uint32_t arr);

// On-Board LED related functions
void BSP_ob_led_init(void);
void BSP_ob_led_set(void);
void BSP_ob_led_toggle(void);
void BSP_ob_led_clear(void);

// Button related functions
void BSP_ob_button_init(void);
int BSP_ob_button_read(void);


#endif


