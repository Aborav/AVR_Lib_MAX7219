#ifndef _MAX7219_H_
#define _MAX7219_H_

#include <avr/io.h>
#include <stdint.h>

#define _A 0b1110111
#define _B 0b1111111
#define _C 0b1001110
#define _D 0b1111110
#define _E 0b1001111
#define _F 0b1000111
#define _G 0b1011110
#define _H 0b0110111
#define _i 0b0110000
#define _J 0b0111100
#define _L 0b0001110
#define _N 0b1110110
#define _O 0b1111110
#define _P 0b1100111
#define _r 0b0000101
#define _S 0b1011011
#define _t 0b0001111
#define _U 0b0111110
#define _Y 0b0111011
#define _DP 0b10000000
#define _DASH 0b0000001
#define _UNDER 0b0001000
#define _EMPTY 0b00000000

#define _0 0b1111110
#define _1 0b0110000
#define _2 0b1101101
#define _3 0b1111001
#define _4 0b0110011
#define _5 0b1011011
#define _6 0b1011111
#define _7 0b1110000
#define _8 0b1111111
#define _9 0b1111011

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
        if (ptr == NULL) {                                                     \
            act;                                                               \
        }                                                                      \
    } while (0)

// API LEVEL
#define MAX7219_RUN_STR_DELAY 100
/*---------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif
/*-----------TYPES, STRUC, VARIABLES-----------*/
typedef enum {
    MAX7219_OK = 0, ///< all in ok
    MAX7219_TO,     ///< SPI timeout reached
    MAX7219_EP,     ///< empty pointer
} max7219_state_t;

typedef struct max7219_s {
    volatile uint8_t *cs_port;      ///< chip select PORT port pointer
    uint8_t cs_pin;                 ///< chip select pin
    char buf[MAX7219_DIGITS_QUANT]; ///< diplay buffer
    uint8_t err;            ///< error holding variable
    uint8_t bar_rate;               ///< display bar rating
} max7219_t;

/*------------------FUNCTIONS------------------*/
// HAL LEVEL
max7219_state_t MAX7219_SPI_TX(uint8_t data);
void MAX7219_SPI_Init(void);
max7219_state_t MAX7219_SPI_TX_Buf(uint8_t *data_buf, uint8_t size);

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