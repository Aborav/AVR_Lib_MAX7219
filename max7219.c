#include "max7219.h"
#include <time.h>

/*-------------------HAL LEVEL-----------------*/
/**
 * @brief function to send 1 byte via SPI
 * @param data -> data byte
 * @retval MAX7219_OK -> ok\
 * MAX7219_TO -> spi timeout\
 */
max7219_state_t MAX7219_SPI_TX(uint8_t data) {
    uint32_t cnt = 0;
    SPDR = data;
    while (!(SPSR & (1 << SPIF))) {
        // emergency exit
        if (cnt++ > MAX7219_SPI_TIMEOUT)
            return MAX7219_TO;
    };
    return MAX7219_OK;
}

/**
 * @brief function to send buffer via SPI
 * @param data_buf[in] -> data buffer pointer
 * @param size -> size of data buffer
 * @retval MAX7219_OK -> ok\
 * MAX7219_TO -> spi timeout\
 * MAX7219_EP -> empty pointer
 */
max7219_state_t MAX7219_SPI_TX_Buf(uint8_t *data_buf, uint8_t size) {
    MAX7219_MACR_CHECK_PTR(data_buf, return MAX7219_EP);
    uint32_t cnt;
    for (uint8_t i = size; i > 0; i++) {
        cnt = 0;
        SPDR = *data_buf;
        while (!(SPSR & (1 << SPIF))) {
            // emergency exit
            if (cnt++ > MAX7219_SPI_TIMEOUT)
                return MAX7219_TO;
        };
    }
    return MAX7219_OK;
}

void MAX7219_SPI_Init(void) {
    // GPIO setting
    MAX7219_MOSI_PIN_TO_OUTPUT(); // pin data to output
    MAX7219_CS_PIN_TO_OUTPUT();   // pin load to output
    MAX7219_CLK_PIN_TO_OUTPUT();  // pin clock to output
    MAX7219_CS_PIN_1();           // cs high - switch spi to master mode

    // SPI setting
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    SPSR |= (1 << SPI2X);
}

/*-----------------DRIVER LEVEL----------------*/
/**
 * @brief function to write MAX7219 register data
 * @param reg -> MAX7219 register
 * @param data -> data to write
 * @param struc [in/out] -> max7219 configure structure
 */
void MAX7219_WriteReg(uint8_t reg, uint8_t data, max7219_t *struc) {
    MAX7219_MACR_CHECK_PTR(struc, return);
    max7219_state_t err = 0;

    *struc->cs_port &= ~(1 << struc->cs_pin); // cs low
    err |= MAX7219_MACR_SPI_TX(reg);
    err |= MAX7219_MACR_SPI_TX(data);
    *struc->cs_port |= (1 << struc->cs_pin); // cs high
    if (err != MAX7219_OK) {
        MAX7219_MACR_WRITE_ERR(struc, MAX7219_ERR_SPI_TIMEOUT);
    }
}

/**
 * @brief function to change dislpay brightness
 * @param intensity from 0 to 8
 * @param struc [in/out] -> max7219 configure structure
 */
void MAX7219_SetIntensity(uint8_t intensity, max7219_t *struc) {
    MAX7219_MACR_CHECK_PTR(struc, return);
    if (intensity > 8)
        intensity = 8;
    MAX7219_WriteReg(MAX7219_REG_INTENSITY, intensity, struc);
}

/**
 * @brief function to initialize AVR and MAX7219 periphery
 * @param struc [in/out] -> max7219 configure structure
 */
void MAX7219_Init(max7219_t *struc) {
    MAX7219_MACR_CHECK_PTR(struc, return);

    MAX7219_MACR_SPI_INIT();
    if (MAX7219_DIGITS_QUANT - 1 > 7) {
        MAX7219_MACR_WRITE_ERR(struc, MAX7219_ERR_DIGIT_QUANT);
        return;
    }
    // scan limit ->number of digits 6 digits
    MAX7219_WriteReg(MAX7219_REG_SCAN_LIMIT, (MAX7219_DIGITS_QUANT - 1), struc);
    MAX7219_WriteReg(MAX7219_REG_DECODE, 0, struc); // no decode, use own font
    MAX7219_WriteReg(MAX7219_REG_SHUTDOWN, 1,
                     struc); // normal mode, Shut down mode OFF
    MAX7219_WriteReg(MAX7219_REG_DISPLAY_TEST, 0,
                     struc); // normal  mode, test mode OFF
    MAX7219_SetIntensity(MAX7219_INTENSITY, struc);
    MAX7219_Clear(struc);
    //__CLEARDISPLAY
}

/**
 * @brief function flush the buffer to the display
 * @param struc [in/out] -> max7219 configure structure
 */
void MAX7219_Flush(max7219_t *struc) {
    MAX7219_MACR_CHECK_PTR(struc, return);
    uint8_t bar = 0;

    MAX7219_WriteReg(6, (uint8_t)struc->buf[4], struc);
    MAX7219_WriteReg(5, (uint8_t)struc->buf[3], struc);
    MAX7219_WriteReg(4, (uint8_t)struc->buf[2], struc);
    MAX7219_WriteReg(3, (uint8_t)struc->buf[1], struc);
    MAX7219_WriteReg(2, (uint8_t)struc->buf[0], struc);
    for (uint8_t i = 0; i < struc->bar_rate; i++) {
        bar |= 2 | 2 << i;
    }
    MAX7219_WriteReg(1, bar, struc);
}

void MAX7219_Clear(max7219_t *struc) {
    MAX7219_MACR_CHECK_PTR(struc, return);
    uint8_t bar = 0;

    MAX7219_WriteReg(6, 0, struc);
    MAX7219_WriteReg(5, 0, struc);
    MAX7219_WriteReg(4, 0, struc);
    MAX7219_WriteReg(3, 0, struc);
    MAX7219_WriteReg(2, 0, struc);
    for (uint8_t i = 0; i < struc->bar_rate; i++) {
        bar |= 2 | 2 << i;
    }
    MAX7219_WriteReg(1, bar, struc);
}

uint8_t MAX7219_GetErr(max7219_t *struc) {
    if (struc == NULL) {
        return 1 << MAX7219_ERR_EMPTY_PTR;
    }
    return struc->err;
}

/* void MAX7219_ClearDisplay(void) {
  for (char i = 0; i < 8; i++)
    MAX7219_WriteReg(i, 0x00);
} */

/* void MAX7219_RunString(uint8_t *str, uint8_t size, int8_t ms) {
  char str_buf[size + 10]; // temporary buffer for running string
  uint8_t str_ptr = 0;

  for (char i = 0; i < 5; i++) { // add empty cells
    str_buf[i] = _EMPTY;
  }
  for (char i = 0; i < size; i++) { // add string body
    str_buf[i + 5] = str[i];
  }
  for (char i = size + 5; i < size + 10; i++) { // add empty cells
    str_buf[i] = _EMPTY;
  }

  while (str_ptr < size + 6) { // blocking function
    display_buf[4] = str_buf[str_ptr];
    display_buf[3] = str_buf[str_ptr + 1];
    display_buf[2] = str_buf[str_ptr + 2];
    display_buf[1] = str_buf[str_ptr + 3];
    display_buf[0] = str_buf[str_ptr + 4];
    MAX7219_Show();
    _delay_ms(MAX7219_RUN_STR_DELAY);
    str_ptr++;
  }
} */

/* uint16_t fastPow10(uint8_t exp) { // fast 10^x (for MAX7219_Float)
  switch (exp) {
  case 1:
    return 10;
  case 2:
    return 100;
  case 3:
    return 1000;
  }
  return 1;
} */

/* void MAX7219_Int(int num) {
  bool sign_flag = 0;
  if (num < 0) {
    sign_flag = 1;
    num *= (-1); // change to positive and rise negative flag
  }
  if (num > (sign_flag ? 9999 : 99999)) {
    MAX7219_Text(_EMPTY, _EMPTY, _O, _U, _F);
    return;
  } else {
    display_buf[0] = digits_array[(num % 10)];
    if (sign_flag)
      display_buf[1] = _DASH;
    if (num > 9) {
      display_buf[1] = digits_array[(num % 100) / 10];
      if (sign_flag)
        display_buf[2] = _DASH;
    }
    if (num > 99) {
      display_buf[2] = digits_array[(num % 1000) / 100];
      if (sign_flag)
        display_buf[3] = _DASH;
    }
    if (num > 999) {
      display_buf[3] = digits_array[(num % 10000) / 1000];
      if (sign_flag)
        display_buf[4] = _DASH;
    }
    if (num > 9999)
      display_buf[4] = digits_array[(num % 100000) / 10000];
  }
}
*/
/*
void MAX7219_Uint(int16_t num) {
  if (num > 9999) {
    MAX7219_Text(_EMPTY, _EMPTY, _O, _U, _F);
    return;
  } else {
    display_buf[0] = digits_array[(num % 10)];
    if (num > 9)
      display_buf[1] = digits_array[(num % 100) / 10];
    if (num > 99)
      display_buf[2] = digits_array[(num % 1000) / 100];
    if (num > 999)
      display_buf[3] = digits_array[(num % 10000) / 1000];
    if (num > 9999)
      display_buf[4] = digits_array[(num % 100000) / 10000];
  }
} */
/*
void MAX7219_Text(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5) {
  if (d1 > 0)
    display_buf[4] = d1;
  if (d2 > 0)
    display_buf[3] = d2;
  if (d3 > 0)
    display_buf[2] = d3;
  if (d4 > 0)
    display_buf[1] = d4;
  if (d5 > 0)
    display_buf[0] = d5;
} */
