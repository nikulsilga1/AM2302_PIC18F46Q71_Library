/*
 * AM2302 (DHT22) Sensor Driver Implementation for PIC18F46Q71
 * Data Pin: RC2 (Pin 35)
 * Compiler: XC8
 * IDE: MPLAB X IDE
 * 
 * Oscillator: 16MHz HFINTOSC (adjust _XTAL_FREQ if different)
 */

#include "am2302.h"

// Set your oscillator frequency here (16MHz HFINTOSC)
#define _XTAL_FREQ 16000000

/*
 * Initialize AM2302 sensor
 * Sets up the data pin as input (high impedance)
 */
void AM2302_Init(void) {
    AM2302_TRIS = 1;  // Set RC2 as input
    AM2302_LAT = 1;   // Set high initially
}

/*
 * Send start signal to AM2302
 * MCU pulls line low for 1-10ms, then releases
 */
void AM2302_StartSignal(void) {
    AM2302_TRIS = 0;  // Set RC2 as output
    AM2302_LAT = 0;   // Pull line low
    AM2302_Delay_ms(2);  // Hold for 2ms
    AM2302_LAT = 1;   // Release line
    AM2302_TRIS = 1;  // Set RC2 as input
    AM2302_Delay_us(20);  // Wait for sensor response
}

/*
 * Read single bit from AM2302
 * Bit timing: 
 *   0: ~26-28us low, ~50us high
 *   1: ~50us low, ~76us high
 */
uint8_t AM2302_ReadBit(void) {
    uint16_t timeout = 100;
    
    // Wait for line to go low
    while (AM2302_PORT && timeout--);
    if (timeout == 0) return 0;
    
    timeout = 100;
    // Wait for line to go high
    while (!AM2302_PORT && timeout--);
    if (timeout == 0) return 0;
    
    // Delay ~30us and check line state
    AM2302_Delay_us(30);
    
    if (AM2302_PORT) {
        // Wait for line to go low (bit = 1)
        timeout = 100;
        while (AM2302_PORT && timeout--);
        return 1;
    }
    return 0;  // bit = 0
}

/*
 * Read one byte (8 bits) from AM2302
 */
uint8_t AM2302_ReadByte(void) {
    uint8_t byte = 0;
    uint8_t i;
    
    for (i = 0; i < 8; i++) {
        byte <<= 1;
        byte |= AM2302_ReadBit();
    }
    
    return byte;
}

/*
 * Delay function: microseconds
 * Accurate for 16MHz oscillator (HFINTOSC)
 * Each instruction cycle = 4 clock cycles = 0.25us at 16MHz
 * We need microseconds * 4 instruction cycles
 */
void AM2302_Delay_us(uint16_t microseconds) {
    uint16_t i;
    for (i = 0; i < microseconds; i++) {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }
}

/*
 * Delay function: milliseconds
 */
void AM2302_Delay_ms(uint16_t milliseconds) {
    uint16_t i;
    for (i = 0; i < milliseconds; i++) {
        AM2302_Delay_us(1000);
    }
}

/*
 * Read temperature and humidity from AM2302
 * Returns: 0 = Success, 1 = Error
 * 
 * Data format (40 bits):
 * [Humidity MSB] [Humidity LSB] [Temp MSB] [Temp LSB] [Checksum]
 */
uint8_t AM2302_Read(AM2302_DATA *sensor_data) {
    uint8_t data[5] = {0};
    uint8_t i;
    uint16_t timeout = 1000;
    
    if (sensor_data == NULL) {
        return 1;  // Error: NULL pointer
    }
    
    // Send start signal
    AM2302_StartSignal();
    
    // Wait for sensor response (low pulse)
    while (AM2302_PORT && timeout--);
    if (timeout == 0) {
        sensor_data->error = 1;
        return 1;
    }
    
    // Wait for sensor to pull line high
    timeout = 1000;
    while (!AM2302_PORT && timeout--);
    if (timeout == 0) {
        sensor_data->error = 1;
        return 1;
    }
    
    // Read 40 bits (5 bytes)
    for (i = 0; i < 5; i++) {
        data[i] = AM2302_ReadByte();
    }
    
    // Verify checksum
    uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    if (checksum != data[4]) {
        sensor_data->error = 1;
        return 1;  // Checksum error
    }
    
    // Parse humidity (data[0] and data[1])
    sensor_data->humidity = ((uint16_t)data[0] << 8) | data[1];
    
    // Parse temperature (data[2] and data[3])
    // Check if negative (MSB of data[2])
    if (data[2] & 0x80) {
        sensor_data->temperature = 0x8000 | (((uint16_t)data[2] & 0x7F) << 8) | data[3];
    } else {
        sensor_data->temperature = ((uint16_t)data[2] << 8) | data[3];
    }
    
    sensor_data->checksum = data[4];
    sensor_data->error = 0;
    
    return 0;  // Success
}
