//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
/*
 *  TFT.h
 *  Adafruit_TFTLCD plain C port for an AVR GCC
 *  Created: 25.6.2017 12:48:21
 *  Authors: Adafruit team, Simon Katznelson
 *   x3merz@intensecircuit.com
*/ 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <util/delay.h>
#include <stdbool.h>
#include "registers.h"
#include <avr/pgmspace.h>
#if 0
#include <stdint.h>
#include <inttypes.h>
#endif
#define ILI_AND_MORE
#define  ID_CHK      // AUTO display driver type chip detect
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ID_CHK   
#define id 0x9325  // set display driver type here manually
////////////     #define id 0x9341    //////////// 
// #define id 0x7783 
#define readID()   id   // dummy placeholder
#define readReg(val)   id    // dummy placeholder
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else 
   extern uint32_t id; 
#endif 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
#define ILI9341_8BIT    /*  ILI9341 pure SPI mode ON/OFF  */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DATA_PIN        PINA
#define DATA_PORT       PORTA
#define DATA_DIR        DDRA
#define CTRL_PORT       PORTC
#define CTRL_DIR        DDRC
 
  // LCD control lines:
  // RD (read), WR (write), CD (command/data), CS (chip select)
#define RD_PORT PORTC				/*  */
#define WR_PORT PORTC				/*  */
#define CD_PORT PORTC				/*  */
#define CS_PORT PORTC				/*  */
  
#define RST_MASK    (1 << PC6)
#define RD_MASK     (1 << PC5) 
#define WR_MASK     (1 << PC4) 
#define CD_MASK     (1 << PC3)
#define CS_MASK     (1 << PC2)
 
#define  TOUCHPORT_A  PORTA
#define  TOUCHPIN_A   DDRA
#define  TOUCHPORT_D  PORTC
#define  TOUCHPIN_D   DDRC


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Control signals are ACTIVE LOW (idle is HIGH)
 // Command/Data: LOW = command, HIGH = data
 // These are single-instruction operations and always inline
#define RD_ACTIVE       RD_PORT &= ~RD_MASK
#define RD_IDLE         RD_PORT |=  RD_MASK
#define WR_ACTIVE       WR_PORT &= ~WR_MASK
#define WR_IDLE         WR_PORT |=  WR_MASK
#define CD_COMMAND      CD_PORT &= ~CD_MASK
#define CD_DATA         CD_PORT |=  CD_MASK
#define CS_ACTIVE       CS_PORT &= ~CS_MASK
#define CS_IDLE         CS_PORT |=  CS_MASK

  // Data write strobe, ~2 instructions and always inline
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

  // port/pin: PA7 PA6 PA5 PA4 PA3 PA2 PA1 PA0
#define write8(d)   { PORTA = (d); WR_STROBE; }
#define read8(result) { \
    if(busy) return 0; \
    RD_ACTIVE;                  \
    DELAY7;                     \
    result = DATA_PIN;              \
    RD_IDLE; \
     }
#define setWriteDir() DATA_DIR  = 0xff
#define setReadDir()  DATA_DIR  = 0
// Pixel read operations require a minimum 400 nS delay from RD_ACTIVE
// to polling the input pins.  At 16 MHz, one machine cycle is 62.5 nS.
// This code burns 7 cycles (437.5 nS) doing nothing; the RJMPs are
// equivalent to two NOPs each, final NOP burns the 7th cycle, and the
// last line is a radioactive mutant emoticon.
#define DELAY7        \
  asm volatile(       \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "nop"      "\n"   \
    ::);


#define TFTLCD_DELAY 0xFF
#define TFTWIDTH    240
#define TFTHEIGHT  320
// LCD controller chip identifiers
#define ID_932X      0
#define ID_7575      1
#define ID_9341      2
#define ID_HX8357D   3
#define ID_S6D0154   4
#define ID_ILI9327   5
#define ID_ILI9488   6
#define ID_ST7781    7
#define ID_UNKNOWN 0xFF

//	 #define delay(val)    _delay_ms(val)  
#define delayMicroseconds(val)   _delay_us(val) 
#define boolean bool
extern  bool busy;  // DATA lines busy flag

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern uint8_t driver;
extern uint8_t textsize,  rotation;

#if 1
         //   Prototypes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void delay(unsigned int d);
  void writeRegister8(uint8_t a, uint8_t d);
  void writeRegister16(uint16_t a, uint16_t d);
  void writeRegister24(uint8_t r, uint32_t d);
  void writeRegister32(uint8_t r, uint32_t d);
  void     begin(void);
  void     drawPixel(int16_t x, int16_t y, uint16_t color);
  void     drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
  void     drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
  void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
  void     fillScreen(uint16_t color);
  void     reset(void);
  void     setRegisters8(uint8_t *ptr, uint8_t n);
  void     setRegisters16(uint16_t *ptr, uint8_t n);
  void     setRotation(uint8_t x);
  void 	   setLR(void);
  void     setAddrWindow(int x1, int y1, int x2, int y2);
  void     pushColors(uint16_t *data, uint8_t len, boolean first);
  void charBounds(char c, int16_t *x, int16_t *y,
      int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  void flood(uint16_t color, uint32_t len);
  uint16_t readID(void);
  uint32_t readReg(uint8_t r);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

#ifdef _DEBUG
#define  ILI_9325D
#endif
