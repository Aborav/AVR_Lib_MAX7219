#ifndef _MAX7219_H_
#define _MAX7219_H_

#include <avr/io.h>
#include <stdint.h>

/*-------------------DEFINES-----------------*/

// HAL LEVEL
///////////////////////////////////////////////
// cs
#define MAX7219_CS_PIN_0() PORTB &= ~(1 << PORTB2)
#define MAX7219_CS_PIN_1() PORTB |= (1 << PORTB2)
#define MAX7219_CS_PIN_TO_OUTPUT() DDRB |= (1 << DDB2)
// mosi
#define MAX7219_MOSI_PIN_TO_OUTPUT() DDRB |= (1 << DDB3)
// clk
#define MAX7219_CLK_PIN_TO_OUTPUT() DDRB |= (1 << DDB5)
// spi
#define MAX7219_MACR_SPI_TX(data) MAX7219_SPI_TX(data)
#define MAX7219_SPI_TIMEOUT 1000000U
// init
#define MAX7219_MACR_SPI_INIT() MAX7219_SPI_Init()

// DRIVER LEVEL
///////////////////////////////////////////////
#define MAX7219_DIGITS_QUANT 6U
#define MAX7219_INTENSITY 1

// errors:
// SPI timeout
#define MAX7219_ERR_SPI_TIMEOUT 0U
// max7219 can handle only 7 digits at maximum
#define MAX7219_ERR_DIGIT_QUANT 1U
// empty pointer in return error functions
#define MAX7219_ERR_EMPTY_PTR 2U

#define MAX7219_REG_DECODE 0x09       // "decode mode" register
#define MAX7219_REG_INTENSITY 0x0A    // "intensity" register
#define MAX7219_REG_SCAN_LIMIT 0x0B   // "scan limit" register
#define MAX7219_REG_SHUTDOWN 0x0C     // "shutdown" register
#define MAX7219_REG_DISPLAY_TEST 0x0F // "display test" register

// macros
#define MAX7219_MACR_WRITE_ERR(dev, bit)                                       \
    do {                                                                       \
        (dev)->err |= (1U << (bit));                                           \
    } while (0)

#define MAX7219_MACR_CHECK_PTR(ptr, act)                                       \
    do {                                                                       \
        if (ptr == 0) {                                                     \
            act;                                                               \
        }                                                                      \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif
/*------------------TYPES---------------------*/
typedef enum {
    MAX7219_OK = 0, ///< all in ok
    MAX7219_TO,     ///< SPI timeout reached
    MAX7219_EP,     ///< empty pointer
} max7219_state_t;

typedef struct max7219_s {
    volatile uint8_t *cs_port;      ///< chip select port pointer
    uint8_t cs_pin;                 ///< chip select pin
    uint8_t buf[MAX7219_DIGITS_QUANT]; ///< diplay buffer
    uint8_t err;                    ///< error holding variable
    uint8_t bar_rate;               ///< display bar rating
} max7219_t;

/*------------------FUNCTIONS------------------*/
// HAL LEVEL
max7219_state_t MAX7219_SPI_TX(uint8_t data);
void MAX7219_SPI_Init(void);
max7219_state_t MAX7219_SPI_TX_Buf(const uint8_t *data_buf, uint8_t size);

// DRIVER LEVEL
void MAX7219_WriteReg(uint8_t reg, uint8_t data, max7219_t *struc);
void MAX7219_Init(max7219_t *struc);
void MAX7219_SetIntensity(uint8_t intensity, max7219_t *struc);
void MAX7219_Flush(max7219_t *struc);
void MAX7219_Clear(max7219_t *struc);
uint8_t MAX7219_GetErr(max7219_t *struc);

// API LEVEL

#ifdef __cplusplus
}
#endif

#endif