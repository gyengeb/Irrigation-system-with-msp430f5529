//******************************************************************************
//  MSP430F5529 SSD1306 OLED Display
//
//  Description: This demo connects two MSP430's via the I2C bus. The master
//  transmits to the slave. This is the MASTER CODE. It cntinuously
//  transmits an array of data and demonstrates how to implement an I2C
//  master transmitter sending multiple bytes using the USCI_B0 TX interrupt.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
//
//
//                                /|\  /|\
//                MSP430F5529     10k  10k      SSD1306 OLED
//                   master        |    |         Display
//             -----------------   |    |   -----------------
//           -|XIN  P4.1/UCB0SDA|<-|----+->|SDA              |-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |                 |-
//            |     P4.2/UCB0SCL|<-+------>|SCL              |
//            |                 |          |                 |
//            |                 |          |                 |
//            |     P6.0        |----------> relay control
//            |     P6.1        |----------> relay control
//
//******************************************************************************
#include "main.h"

extern unsigned char *PTxData;      // Pointer to TX data, defined in i2c.h
extern unsigned char TXByteCtr;     // number of bytes to transmit, defined in i2c.h

/*---------- global variables ----------*/
uint8_t S1buttonCounter = 0;
uint8_t S2buttonCounter = 0;

volatile bool displayClearRequest = false;

volatile unsigned int seconds = 0;
volatile unsigned int minutes = 0;
volatile unsigned int hours = 0;
volatile unsigned int days = 0;

volatile unsigned int remainingMinutes = 0;
volatile unsigned int setTime = 1;
volatile unsigned int remainingTime = 0;

volatile unsigned int numberOfIrrigation = 0;

EState state = INIT;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // Stop the watchdog timer during initialization

   clock_init();                    // Initialize clock system
   gpio_init();                     // Initialize GPIO
   i2c_init();                      // Initialize UCB1 I2C, port 4 pins 1, 2
   ssd1306_init();                  // Initialize SSD1306 OLED
   watchdog_init();                 // Initialize watchdog

   __enable_interrupt(); // Enable global interrupts

   turn_relays_off();               // Initialize the relays

   ssd1306_clearDisplay();          // Clear garbage data

   __no_operation();
   __bis_SR_register(GIE);          // Enter LPM0, enable interrupts

   while(1)
   {
      // Clear display between state changes
      if (true == displayClearRequest)
      {
         ssd1306_clearDisplay();
      }
#ifdef DEBUG
      ssd1306_printUI32(0, 0, seconds, HCENTERUL_OFF);
      ssd1306_printUI32(0, 2, minutes, HCENTERUL_OFF);
      ssd1306_printUI32(0, 4, setTime, HCENTERUL_OFF);
#endif

      switch(state)
      {
         case IRRIGATION:
         {
            displayClearRequest = false;
            ssd1306_printText(0,6, "Status: Irrigation");
            if (seconds <= IRRIGATION_TIME)
            {
#if defined(DEBUG)
               turn_led_on(RED);
#endif
               turn_relays_on();
            }
            else
            {
#ifdef DEBUG
               turn_led_off(RED);
#endif
               turn_relays_off();
               state = COUNTING_DOWN;
            }
            break;
         }
         case COUNTING_DOWN:
         {
            displayClearRequest = false;
            ssd1306_printText(0, 6, "Status: Counting down");
            ssd1306_printUI32(0, 0, numberOfIrrigation, HCENTERUL_OFF);
            break;
         }
         case UNDER_CONFIGURATION:
         {
            displayClearRequest = false;
            ssd1306_printText(0, 6, "Status: Configuration");
            ssd1306_printUI32(0, 0, seconds, HCENTERUL_OFF);
            ssd1306_printText(14, 0, "seconds");
            ssd1306_printUI32(0, 2, minutes, HCENTERUL_OFF);
            ssd1306_printText(14, 2, "minutes");
            ssd1306_printUI32(0, 4, setTime, HCENTERUL_OFF);
            ssd1306_printText(14, 4, "set time");
            break;
         }
         case INIT:
         {
            displayClearRequest = false;
            ssd1306_printText(0,6, "Status: Init");
            break;
         }
         default:
         {
            ssd1306_printText(0,6, "Status: Error");
            break;
         }
      }

      watchdog_feed(); // Feed the watchdog timer in the main loop
   }

   return 0;
}

/*!
 * \brief       Initialize watchdog.
 * \param[in]:  None
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void watchdog_init()
{
    WDTCTL = WDTPW | WDTCNTCL | WDTSSEL0 | WDTTMSEL | WDTIS1; // Configure watchdog timer

    SFRIE1 |= WDTIE; // Enable watchdog interrupt
}

/*!
 * \brief       Reset the watchdog timer and prevent it from triggering the interrupt.
 * \param[in]:  None
 * \param[out]: None
 * \return      void
 * \retval      None
 */
void watchdog_feed()
{
    WDTCTL = (WDTPW | WDTCNTCL | WDTSSEL0 | WDTTMSEL | WDTIS1); // Clear watchdog timer
}

//------------------------------------------------------------------------------
// Watchdog_interrupt is called when the watchdog timer times out, generating a software reset.
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_interrupt()
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // Code execution reached watchdog interrupt
    // Perform any necessary operations here

    // Reset the microcontroller
    WDTCTL = 0xDEAD; // Invalid value to generate a software reset
}

//------------------------------------------------------------------------------
// The USCIAB1TX_ISR is structured such that it can be used to transmit any
// number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
// points to the next byte to transmit.
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB1IV,12))
  {
   case  0: break;                           // Vector  0: No interrupts
   case  2: break;                           // Vector  2: ALIFG
   case  4: break;                           // Vector  4: NACKIFG
   case  6: break;                           // Vector  6: STTIFG
   case  8: break;                           // Vector  8: STPIFG
   case 10: break;                           // Vector 10: RXIFG
   case 12:                                  // Vector 12: TXIFG  
      if (TXByteCtr)                          // Check TX byte counter
      {
         UCB1TXBUF = *PTxData++;               // Load TX buffer
         TXByteCtr--;                          // Decrement TX byte counter
      }
      else
      {
         UCB1CTL1 |= UCTXSTP;                  // I2C stop condition
         UCB1IFG &= ~UCTXIFG;                  // Clear USCI_B1 TX int flag
         __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
      }
   default: break;
  }
}
//------------------------------------------------------------------------------
// Port 2 interrupt service routine
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT2_VECTOR       // P2.1 push button is used as hardware interrupt
__interrupt void port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
   if (INIT == state)
   {
      state = UNDER_CONFIGURATION;
      displayClearRequest = true;
   }
   else if (UNDER_CONFIGURATION == state)
   {
      state = COUNTING_DOWN;
      displayClearRequest = true;
   }
   else if ((COUNTING_DOWN == state) ||
            (IRRIGATION == state))
   {
      state = UNDER_CONFIGURATION;
      displayClearRequest = true;
   }
   else
   {}

   S1buttonCounter++;
   if (S2buttonCounter >= UINT8_MAX)
   {
      S2buttonCounter = UINT8_MIN;
   }

   __delay_cycles(782000);
   P2IFG &= ~BIT1;                  // Clear P2.1 IFG
}
//------------------------------------------------------------------------------
// Port 1 interrupt service routine
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT1_VECTOR       // P1.1 push button is used as hardware interrupt
__interrupt void port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
   S2buttonCounter++;
   setTime = S2buttonCounter;

   if (S2buttonCounter >= UINT8_MAX)
   {
      S2buttonCounter = UINT8_MIN;
   }

   __delay_cycles(782000);
   P1IFG &= ~BIT1;                  // Clear P1.1 IFG
}
//------------------------------------------------------------------------------
// Timer1 A0 interrupt service routine
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
   seconds++;

   if (seconds >= 60)
   {
#ifdef DEBUG
      remainingMinutes++;
#endif
      minutes++;
      seconds = 0;
   }
   if (minutes >= 60)
   {
      hours++;
      minutes = 0;
   }
   if (hours >= 24)
   {
      days++;
   }

   if ((setTime == days) &&
       (IRRIGATION != state) &&
       (UNDER_CONFIGURATION != state) &&
       (INIT != state))
   {
      state = IRRIGATION;
      days = 0;
      numberOfIrrigation++;
      displayClearRequest = true;
   }

#ifdef DEBUG
   // for debugging
   if ((setTime == remainingMinutes) &&
       (IRRIGATION != state) &&
       (UNDER_CONFIGURATION != state) &&
       (INIT != state))
   {
      state = IRRIGATION;
      remainingMinutes = 0;
      numberOfIrrigation++;
      displayClearRequest = true;
   }
#endif

    TA1CCTL0 &= ~CCIFG;    // Clear interrupt flag
}
