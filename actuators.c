#include "actuators.h"

/*!
 * \brief       GPIO init.
 * \param[in]:  None
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void gpio_init(void)
{
   // LED init
   P1DIR |= BIT0;          // P1.0 set as output - Red LED
   P4DIR |= BIT7;          // P4.7 set as output - Green LED

   // Relay pin init
   P6DIR |= BIT0;          // P6.0 set as output
   P6DIR |= BIT1;          // P6.1 set as output

   // OLED related init
   P1DIR |= BIT5;          // P1.5 output
   P1OUT |= BIT5;          // P1.5 high to turn on power to display

   // S1 button init
   P2DIR = 0x00;          // Port 2 is made as input port
   P2REN = BIT1;          // Pull up resistor for P2.1
   P2OUT = BIT1;
   P2IE |= BIT1;          // Port 2 interrupt
   P2IES |= BIT1;
   P2IFG &= ~BIT1;

   // S2 button init
   P1REN |= BIT1;          // Pull up resistor for P1.1
   P1OUT |= BIT1;
   P1IE  |= BIT1;          // Port 1 interrupt
   P1IES |= BIT1;
   P1IFG &= ~BIT1;

   return;
}

/*!
 * \brief       Turn LEDs on.
 * \param[in]:  led
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void turn_led_on(uint8_t led)
{
    switch(led)
    {
        case GREEN:
        {
            P4OUT |= BIT7;
            break;
        }
        case RED:
        {
            P1OUT |= BIT0;
            break;
        }
        default:
        {}
    }

    return;
}

/*!
 * \brief       Turn LEDs off.
 * \param[in]:  led
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void turn_led_off(uint8_t led)
{
    switch(led)
    {
        case GREEN:
        {
            P4OUT &= (~BIT7);
            break;
        }
        case RED:
        {
            P1OUT &= (~BIT0);
            break;
        }
        default:
        {}
    }

    return;
}

/*!
 * \brief       Toggle LEDs.
 * \param[in]:  led
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void toggle_led(uint8_t led)
{
    switch(led)
    {
        case GREEN:
        {
            /* XOR P4.7 */
            P4OUT ^= BIT7;
            /* Delay */
            __delay_cycles(10000000);
            break;
        }
        case RED:
        {
            /* XOR P1.0 */
            P1OUT ^= BIT0;
            /* Delay */
            __delay_cycles(10000000);
            break;
        }
        default:
        {}
    }

    return;
}

/*!
 * \brief       Turn relays on.
                Control voltage is NOT on the pin, due to the behaviour of the relay.
 * \param[in]:  None
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void turn_relays_on()
{
    P6OUT &= (~BIT0);          // P6.0 set low
    P6OUT &= (~BIT1);          // P6.1 set low

    return;
}

/*!
 * \brief       Turn relays off.
                Control voltage is on the pin, due to the behaviour of the relay.
 * \param[in]:  None
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void turn_relays_off()
{
    P6OUT |= BIT0;              // P6.0 set high
    P6OUT |= BIT1;              // P6.1 set high

    return;
}
