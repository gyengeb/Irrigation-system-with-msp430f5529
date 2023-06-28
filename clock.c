#include <msp430.h>
#include "clock.h"

/*!
 * \brief       Clock system init.
                Timer1_A is configured for up mode, thus the timer overflows when TAR counts to CCR0.
                Toggle rate = 32768/(32768) = 1 Hz
 * \param[in]:  None
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void clock_init(void)
{
   TA1CCR0 = 32768-1;         // Set CCR0 value for desired frequency
   TA1CCTL0 = CCIE;           // CCR0 interrupt enabled
   TA1CTL = TASSEL_1 + MC_1;  // ACLK, up mode
   //TA1CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK, upmode, clear TAR

   return;
}
