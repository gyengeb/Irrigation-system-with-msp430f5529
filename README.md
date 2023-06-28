# MSP430F5529 SSD1306 OLED Display Irrigation System

### This project demonstrates an irrigation system using the MSP430F5529 microcontroller and the SSD1306 ### OLED display. The system connects two MSP430 microcontrollers via the I2C bus, with the master 
### transmitting data to the slave.
### Hardware Configuration

## The hardware setup for this project is as follows:

    -MSP430F5529 master microcontroller
    -SSD1306 OLED display
    -10k pull-up resistors for SDA and SCL lines
    -Relay control connected to P6.0 and P6.1

## Software Description

### The main code file for this project is main.c. Here's a brief overview of the code structure and functionality:

    -The code initializes the clock system, GPIO, I2C, SSD1306 OLED display, and watchdog timer.
    -The main loop continuously checks the current state and updates the display accordingly.
    -The state can be one of the following:
        -IRRIGATION: The system is in the irrigation state, and the relays are turned on for a specific duration.
        -COUNTING_DOWN: The system is in the countdown state, displaying the remaining irrigation count.
        -UNDER_CONFIGURATION: The system is in the configuration state, allowing the user to set the irrigation time.
        -INIT: The initial state of the system.
    -The watchdog timer is used to reset the microcontroller periodically to prevent lock-ups.
    -Button interrupts are used to switch between different states and configure the irrigation time.
    -Timer1 A0 interrupt is used to increment the time counters (seconds, minutes, hours) and trigger irrigation if the set time is reached.

## Usage

### To use this irrigation system, follow these steps:

    -Set up the hardware configuration as described in the hardware section.
    -Compile and upload the code to the MSP430F5529 microcontroller.
    -The system will start in the INIT state. Press the buttons to switch between states and configure the irrigation time.
    -The OLED display will show the current status, remaining irrigation count, and other relevant information.

### Please refer to the code comments and the project documentation for more detailed information on the implementation and usage of the irrigation system.

### Feel free to customize and enhance this code to fit your specific requirements.