#ifndef MAIN_H_
#define MAIN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "stdbool.h"
#include <msp430.h>
#include "ssd1306.h"
#include "i2c.h"
#include "clock.h"
#include "actuators.h"

/*-------------- defines ---------------*/

/* Compiler switches */

/* Watchdog related defines */
#define WATCHDOG_TIMEOUT_MS 5000 // 5 seconds

/* LED related defines */
#define GREEN              (0x00U)
#define RED                (0x01U)

/* Time related defines */
#define UINT8_MIN          (0U)
#define IRRIGATION_TIME    (15U) // 15 sec

/* OLED LCD related defines*/
#define MAX_COUNT          (4294967295UL)

/*---------- type definitions ----------*/
typedef enum EState_
{
   INIT,
   COUNTING_DOWN,
   UNDER_CONFIGURATION,
   IRRIGATION
}EState;

void watchdog_init();
void watchdog_feed();

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_ */
