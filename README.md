# AM2302 (DHT22) Sensor Interface Library for PIC18F46Q71

A complete XC8 C library for interfacing the AM2302 (DHT22) temperature and humidity sensor with the PIC18F46Q71 microcontroller using MPLAB X IDE.

## Features

✅ Single-wire digital protocol implementation
✅ Accurate timing control for bit detection
✅ Temperature and humidity data extraction
✅ Checksum validation
✅ Error handling and reporting
✅ Example code with UART output

## Hardware Configuration

| Component | Pin | Description |
|-----------|-----|-------------|
| AM2302 Data | RC2 (Pin 35) | Single-wire communication |
| AM2302 Power | VCC | +3.3V to +5.5V |
| AM2302 GND | GND | Ground |
| Pull-up Resistor | RC2 | 4.7kΩ recommended |

**Wiring Diagram:**
```
AM2302:
Pin 1 (VCC)  ──→ +5V
Pin 2 (DATA) ──→ RC2 (Pin 35) + 4.7kΩ pull-up to VCC
Pin 3 (NC)   ──→ Not connected
Pin 4 (GND)  ──→ Ground
```

## File Structure

```
AM2302_PIC18F46Q71_Library/
├── am2302.h          # Header file with function prototypes
├── am2302.c          # Implementation of AM2302 driver
├── main.c            # Example usage with UART output
└── README.md         # This file
```

## Usage

### 1. Include the header file
```c
#include "am2302.h"
```

### 2. Initialize the sensor
```c
AM2302_Init();
```

### 3. Read sensor data
```c
AM2302_DATA sensor_data;
uint8_t result = AM2302_Read(&sensor_data);

if (result == 0) {
    float humidity = sensor_data.humidity / 10.0;
    float temperature = sensor_data.temperature / 10.0;
    printf("Humidity: %.1f%%\n", humidity);
    printf("Temperature: %.1f°C\n", temperature);
} else {
    printf("Error reading sensor!\n");
}
```

## Data Format

The sensor returns a structure with:

```c
typedef struct {
    uint16_t humidity;      // Humidity * 10 (e.g., 550 = 55.0%)
    uint16_t temperature;   // Temperature * 10 (e.g., 250 = 25.0°C)
    uint8_t checksum;       // Checksum value
    uint8_t error;          // Error flag (0 = OK, 1 = ERROR)
} AM2302_DATA;
```

### Temperature Range
- **Range:** -40°C to +80°C
- **Accuracy:** ±0.5°C
- **Resolution:** 0.1°C

### Humidity Range
- **Range:** 0% to 100%
- **Accuracy:** ±2%
- **Resolution:** 0.1%

## Protocol Details

### AM2302 Communication Timing

1. **Start Signal:** MCU pulls DATA line low for 1-10ms, then releases
2. **Sensor Response:** Sensor pulls line low for 80µs, then high for 80µs
3. **Data Transmission:** 40 bits transmitted (5 bytes)
   - Bit "0": Low ~26-28µs, High ~50µs
   - Bit "1": Low ~50µs, High ~76µs
4. **Data Structure:**
   - Byte 0: Humidity MSB
   - Byte 1: Humidity LSB
   - Byte 2: Temperature MSB (MSB = sign bit)
   - Byte 3: Temperature LSB
   - Byte 4: Checksum (Byte0 + Byte1 + Byte2 + Byte3)

### Minimum Reading Interval
⚠️ **Important:** Allow at least 2 seconds between consecutive readings

## Configuration

### Oscillator Frequency

If your PIC18F46Q71 uses a different oscillator frequency, update `_XTAL_FREQ` in `am2302.c`:

```c
#define _XTAL_FREQ 8000000  // Change this value (in Hz)
```

Common values:
- 4MHz: `4000000`
- 8MHz: `8000000`
- 16MHz: `16000000`
- 20MHz: `20000000`

### Pin Configuration

To use a different pin, modify `am2302.h`:

```c
#define AM2302_PORT PORTCbits.RC2
#define AM2302_TRIS TRISCbits.TRISC2
#define AM2302_LAT LATCbits.LATC2
```

For example, to use RA4 instead:
```c
#define AM2302_PORT PORTAbits.RA4
#define AM2302_TRIS TRISAbits.TRISA4
#define AM2302_LAT LATAbits.LATA4
```

## MPLAB X IDE Setup

1. Create a new XC8 project for PIC18F46Q71
2. Copy `am2302.h` and `am2302.c` to your project
3. Add both files to your project:
   - Right-click on "Source Files" → "Add Existing Item"
   - Select `am2302.c`
   - Repeat for `am2302.h` to "Header Files"
4. Configure project settings:
   - Compiler: XC8 (v2.36 or newer recommended)
   - Optimization: -O2 or higher
5. Compile and program the PIC18F46Q71

## Troubleshooting

### Sensor not responding
- Check power supply (3.3V-5.5V)
- Verify wiring and connections
- Ensure 4.7kΩ pull-up resistor is connected to DATA line
- Try increasing START signal duration

### Checksum errors
- Verify timing accuracy for your oscillator frequency
- Check for electrical noise on the DATA line
- Try reading multiple times (sensor may need warm-up)

### Timeout errors
- Increase timeout values in `am2302.c`
- Verify sensor power and connections
- Check PIC18F46Q71 clock frequency setting

## Functions Reference

### `void AM2302_Init(void)`
Initialize the sensor and configure pin as input.

### `uint8_t AM2302_Read(AM2302_DATA *sensor_data)`
Read temperature and humidity from sensor.
- **Returns:** 0 = Success, 1 = Error
- **Parameter:** Pointer to `AM2302_DATA` structure

### `void AM2302_StartSignal(void)`
Send start signal to sensor (called internally by `AM2302_Read`).

### `uint8_t AM2302_ReadBit(void)`
Read single bit from sensor (called internally).

### `uint8_t AM2302_ReadByte(void)`
Read 8 bits from sensor (called internally).

### `void AM2302_Delay_us(uint16_t microseconds)`
Microsecond delay function.

### `void AM2302_Delay_ms(uint16_t milliseconds)`
Millisecond delay function.

## License

This library is provided as-is for educational and commercial use.

## References

- [AM2302 Datasheet](https://cdn-shop.adafruit.com/datasheets/AM2302.pdf)
- [DHT22 Protocol](https://www.adafruit.com/downloads/pdf/DHT.pdf)
- [PIC18F46Q71 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/PIC18F24Q71-PIC18F26Q71-PIC18F44Q71-PIC18F46Q71-Data-Sheet-DS40002251A.pdf)

## Support

For issues or questions, please refer to the example code in `main.c` or consult the datasheets above.
