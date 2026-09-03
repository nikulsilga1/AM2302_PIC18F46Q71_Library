/*
 * Example Usage: AM2302 Sensor with PIC18F46Q71
 * Reads temperature and humidity and displays via UART/serial
 * 
 * Pin Configuration:
 * - RC2 (Pin 35): AM2302 Data Line
 * - RX (Pin 26): UART RX
 * - TX (Pin 25): UART TX
 */

#include <xc.h>
#include <stdio.h>
#include "am2302.h"

// Configuration bits for PIC18F46Q71
#pragma config FEXTOSC = HS     // External Oscillator Frequency Selection bits (HS (Crystal oscillator above 8 MHz))
#pragma config RSTOSC = EXTOSC  // Power-up Default Value for COSC bits (EXTOSC operating per FEXTOSC bits)
#pragma config CLKOEN = OFF     // CLKOUT Output Enable bit (CLKOUT function is disabled)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

#define _XTAL_FREQ 8000000  // 8MHz oscillator

// UART Initialization
void UART_Init(void) {
    // Configure UART for 9600 baud at 8MHz
    BAUD1CONbits.BRG16 = 1;
    SP1BRG = 207;  // For 9600 baud @ 8MHz
    
    TX1STAbits.TXEN = 1;   // Enable transmitter
    RCSTAbits.CREN = 1;    // Enable receiver
    RCSTAbits.SPEN = 1;    // Enable serial port
}

// Send character via UART
void UART_PutChar(char c) {
    while (!PIR3bits.TX1IF);  // Wait for transmit buffer to be empty
    TX1REG = c;
}

// Send string via UART
void UART_PutString(const char *str) {
    while (*str) {
        UART_PutChar(*str++);
    }
}

// Main program
void main(void) {
    AM2302_DATA sensor_data;
    char buffer[50];
    uint8_t result;
    uint16_t attempt = 0;
    
    // Initialize
    UART_Init();
    AM2302_Init();
    
    // Wait for sensor to stabilize
    __delay_ms(2000);
    
    UART_PutString("AM2302 Sensor Test\r\n");
    UART_PutString("================\r\n");
    
    while (1) {
        attempt++;
        
        sprintf(buffer, "\nAttempt #%u:\r\n", attempt);
        UART_PutString(buffer);
        
        // Read sensor
        result = AM2302_Read(&sensor_data);
        
        if (result == 0) {
            // Success
            float humidity = sensor_data.humidity / 10.0;
            float temperature = sensor_data.temperature / 10.0;
            
            sprintf(buffer, "Humidity: %.1f%%\r\n", humidity);
            UART_PutString(buffer);
            
            sprintf(buffer, "Temperature: %.1f°C\r\n", temperature);
            UART_PutString(buffer);
            
            sprintf(buffer, "Checksum: 0x%02X\r\n", sensor_data.checksum);
            UART_PutString(buffer);
        } else {
            // Error
            UART_PutString("Error reading sensor!\r\n");
        }
        
        // Wait 2 seconds before next read
        // (AM2302 requires minimum 2 second interval)
        __delay_ms(2000);
    }
}

/*
 * Redirect printf to UART (optional)
 * Uncomment to use printf for serial output
 */
/*
void putch(char c) {
    UART_PutChar(c);
}
*/
