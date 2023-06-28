#ifndef ACTUATORS_H_
#define ACTUATORS_H_

#include "main.h"
#include "stdint.h"
#include <msp430.h>

void turn_led_on(uint8_t led);
void turn_led_off(uint8_t led);
void toggle_led(uint8_t led);
void gpio_init(void);
void turn_relays_on();
void turn_relays_off();

#endif /* ACTUATORS_H_ */
