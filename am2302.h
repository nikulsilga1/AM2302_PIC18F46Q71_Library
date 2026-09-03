/*
 * AM2302 (DHT22) Sensor Driver for PIC18F46Q71
 * Data Pin: RC2 (Pin 35)
 * Compiler: XC8
 * IDE: MPLAB X IDE
 */

#ifndef AM2302_H
#define AM2302_H

#include <xc.h>
#include <stdint.h>

// Pin Configuration - RC2 (Pin 35)
#define AM2302_PORT PORTCbits.RC2
#define AM2302_TRIS TRISCbits.TRISC2
#define AM2302_LAT LATCbits.LATC2

// Sensor Data Structure
typedef struct {
    uint16_t humidity;      // Humidity * 10 (e.g., 550 = 55.0%)
    uint16_t temperature;   // Temperature * 10 (e.g., 250 = 25.0°C)
    uint8_t checksum;       // Checksum value
    uint8_t error;          // Error flag (0 = OK, 1 = ERROR)
} AM2302_DATA;

// Function Prototypes
void AM2302_Init(void);
uint8_t AM2302_Read(AM2302_DATA *sensor_data);
void AM2302_StartSignal(void);
uint8_t AM2302_ReadBit(void);
uint8_t AM2302_ReadByte(void);
void AM2302_Delay_us(uint16_t microseconds);
void AM2302_Delay_ms(uint16_t milliseconds);

#endif // AM2302_H
