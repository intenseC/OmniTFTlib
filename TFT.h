//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
 /*
 *  TFT.h
 *  Adafruit_TFTLCD plain C port for an AVR GCC
 *  Created: 25.6.2017 12:48:21
 *  Authors: Adafruit team, Larry_!
 *   x3merz@gmail.com
 */ 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  #include <util/delay.h>
  #include <stdbool.h>
  #include "registers.h"
  #include <avr/pgmspace.h>

	      #define ILI_AND_MORE

 	      #define  ID_CHK  // AUTO display driver type chip detect
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   #ifndef ID_CHK
				     
				    #define id 0x9325  // set display driver type here manually
     	////////////     #define id 0x9341    //////////// 
                      // #define id 0x7783 
   #define readID()   id   // dummy placeholder
   #define readReg(val)   id    // dummy placeholder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   #else 
   extern uint32_t id; 
   #endif 
	 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
                  #define ILI9341_8BIT    /*  ILI9341 pure SPI mode ON/OFF  */
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  #define DATA_PIN         PINA
  #define DATA_PORT     PORTA
  #define DATA_DIR         DDRA
  #define CTRL_PORT     PORTC
  #define CTRL_DIR        DDRC
 
  // LCD control lines:
  // RD (read), WR (write), CD (command/data), CS (chip select)
  #define RD_PORT PORTC				/*  */
  #define WR_PORT PORTC				/*  */
  #define CD_PORT PORTC				/*  */
  #define CS_PORT PORTC				/*  */
  
  #define RST_MASK    (1 << PC6)
  
  #define RD_MASK     (1 << PC5) 
  #define WR_MASK    (1 << PC4) 
  #define CD_MASK     (1 << PC3)
  #define CS_MASK     (1 << PC2)
 
#define  TOUCHPORT_A  PORTA
#define  TOUCHPIN_A DDRA
#define  TOUCHPORT_D  PORTC
#define  TOUCHPIN_D DDRC

#if 0
unsigned char YP = (1 << PC3);
unsigned char XM = (1 << PC2);
unsigned char XP = (1 << PA1);
unsigned char YM = (1 << PA0);
unsigned char CH0 = 0;
unsigned char CH1 = 1;
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Control signals are ACTIVE LOW (idle is HIGH)
 // Command/Data: LOW = command, HIGH = data
 // These are single-instruction operations and always inline
 #define RD_ACTIVE  RD_PORT &= ~RD_MASK
 #define RD_IDLE    RD_PORT |=  RD_MASK
 #define WR_ACTIVE  WR_PORT &= ~WR_MASK
 #define WR_IDLE    WR_PORT |=  WR_MASK
 #define CD_COMMAND     CD_PORT &= ~CD_MASK
 #define CD_DATA    CD_PORT |=  CD_MASK
 #define CS_ACTIVE  CS_PORT &= ~CS_MASK
 #define CS_IDLE    CS_PORT |=  CS_MASK

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
#define ID_ST7781   7
#define ID_UNKNOWN 0xFF

//	 #define delay(val)    _delay_ms(val)  
     #define delayMicroseconds(val)   _delay_us(val)  


   #define boolean bool
 extern  bool busy;  // DATA lines busy flag

   #ifndef _swap_int16_t
   #define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
   #endif
  
    uint8_t 
	  driver = ID_UNKNOWN;
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  int16_t _width, _height, // Display w/h as modified by current rotation
                 cursor_x, cursor_y;
  
  uint16_t  textcolor, textbgcolor;
  
  uint8_t  textsize,  rotation;
  bool   wrap,   // If set, 'wrap' text at right edge of display
  _cp437; // If set, use correct CP437 charset (default is off)




#if 1

         //  Func Prototypes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void delay(unsigned int d);
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
  void setLR(void); 
  void     setAddrWindow(int x1, int y1, int x2, int y2);
  void     pushColors(uint16_t *data, uint8_t len, boolean first);
  void charBounds(char c, int16_t *x, int16_t *y,
      int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  void flood(uint16_t color, uint32_t len);
 uint16_t readID(void);
 uint32_t readReg(uint8_t r);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

           #ifdef _DEBUG
	       #define  ILI_9325D
           #endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      // Initialization command tables for different LCD controllers


	   #ifdef ILI_9325D
   static const uint16_t ILI932x_regValues[] PROGMEM =   // ILI 9325D
	  {
	0xE5, 0x78F0, // set SRAM internal timing
	0x01, 0x0100, // set Driver Output Control  
	0x02, 0x0200, // set 1 line inversion  
	0x03, 0x1030, // set GRAM write direction and BGR=1.  
	0x04, 0x0000, // Resize register  
	0x08, 0x0207, // set the back porch and front porch  
	0x09, 0x0000, // set non-display area refresh cycle ISC[3:0]  
	0x0A, 0x0000, // FMARK function  
	0x0C, 0x0000, // RGB interface setting  
	0x0D, 0x0000, // Frame marker Position  
	0x0F, 0x0000, // RGB interface polarity  
	//*************Power On sequence ****************//  
	0x10, 0x0000, // SAP, BT[3:0], AP, DSTB, SLP, STB  
	0x11, 0x0007, // DC1[2:0], DC0[2:0], VC[2:0]  
	0x12, 0x0000, // VREG1OUT voltage  
	0x13, 0x0000, // VDV[4:0] for VCOM amplitude  
	0x07, 0x0001,  
	TFTLCD_DELAY, 200, // Dis-charge capacitor power voltage  
	0x10, 0x1690, // SAP, BT[3:0], AP, DSTB, SLP, STB  
	0x11, 0x0227, // Set DC1[2:0], DC0[2:0], VC[2:0]  
	TFTLCD_DELAY, 50, // Delay 50ms  
	0x12, 0x000D, // 0012  
	TFTLCD_DELAY, 50, // Delay 50ms  
	0x13, 0x1200, // VDV[4:0] for VCOM amplitude  
	0x29, 0x000A, // 04  VCM[5:0] for VCOMH  
	0x2B, 0x000D, // Set Frame Rate  
	TFTLCD_DELAY, 50, // Delay 50ms  
	0x20, 0x0000, // GRAM horizontal Address  
	0x21, 0x0000, // GRAM Vertical Address  
	// ----------- Adjust the Gamma Curve ----------//  
	0x30, 0x0000,  
	0x31, 0x0404,  
	0x32, 0x0003,  
	0x35, 0x0405,  
	0x36, 0x0808,  
	0x37, 0x0407,  
	0x38, 0x0303,  
	0x39, 0x0707,  
	0x3C, 0x0504,  
	0x3D, 0x0808,  
	//------------------ Set GRAM area ---------------//  
	0x50, 0x0000, // Horizontal GRAM Start Address  
	0x51, 0x00EF, // Horizontal GRAM End Address  
	0x52, 0x0000, // Vertical GRAM Start Address  
	0x53, 0x013F, // Vertical GRAM Start Address  
	0x60, 0xA700, // Gate Scan Line  
	0x61, 0x0001, // NDL,VLE, REV   
	0x6A, 0x0000, // set scrolling line  
	//-------------- Partial Display Control ---------//  
	0x80, 0x0000,  
	0x81, 0x0000,  
	0x82, 0x0000,  
	0x83, 0x0000,  
	0x84, 0x0000,  
	0x85, 0x0000,  
	//-------------- Panel Control -------------------//  
	0x90, 0x0010, 
	0x92, 0x0000, 
	0x07, 0x0133,  // 262K color and display ON        
	
	// 0xb3, 0x0000,  // CTRL Display  	
	// 0xb5, 0x0000,  // CABC off   	 
		  
		  };
    #endif
 
	  
	  
	  #if defined ILI_9325C && !defined ILI_9325D
   static const uint16_t ILI932x_regValues[] PROGMEM =   // ILI 9325C
	  {
    0xE5, 0x78F0, // set SRAM internal timing
	0x01, 0x0100, // set Driver Output Control  
	0x02, 0x0700, // set 1 line inversion  
	0x03, 0x1030, // set GRAM write direction and BGR=1.  
	0x04, 0x0000, // Resize register  
	0x08, 0x0207, // set the back porch and front porch  
	0x09, 0x0000, // set non-display area refresh cycle ISC[3:0]  
	0x0A, 0x0000, // FMARK function  
	0x0C, 0x0000, // RGB interface setting  
	0x0D, 0x0000, // Frame marker Position  
	0x0F, 0x0000, // RGB interface polarity  
	//*************Power On sequence ****************//  
	0x10, 0x0000, // SAP, BT[3:0], AP, DSTB, SLP, STB  
	0x11, 0x0007, // DC1[2:0], DC0[2:0], VC[2:0]  
	0x12, 0x0000, // VREG1OUT voltage  
	0x13, 0x0000, // VDV[4:0] for VCOM amplitude  
	0x07, 0x0001,  
	TFTLCD_DELAY, 200, // Dis-charge capacitor power voltage  
	0x10, 0x1090, // SAP, BT[3:0], AP, DSTB, SLP, STB  
	0x11, 0x0227, // Set DC1[2:0], DC0[2:0], VC[2:0]  
	TFTLCD_DELAY, 50, // Delay 50ms  
	0x12, 0x001F, // 0012  
	TFTLCD_DELAY, 50, // Delay 50ms  
	0x13, 0x1500, // VDV[4:0] for VCOM amplitude  
	0x29, 0x0027, // 04  VCM[5:0] for VCOMH  
	0x2B, 0x000D, // Set Frame Rate  
	TFTLCD_DELAY, 50, // Delay 50ms  
	0x20, 0x0000, // GRAM horizontal Address  
	0x21, 0x0000, // GRAM Vertical Address  
	// ----------- Adjust the Gamma Curve ----------//  
	0x30, 0x0000,  
	0x31, 0x0707,  
	0x32, 0x0307,  
	0x35, 0x0200,  
	0x36, 0x0008,  
	0x37, 0x0004,  
	0x38, 0x0000,  
	0x39, 0x0707,  
	0x3C, 0x0002,  
	0x3D, 0x1D04,
	//------------------ Set GRAM area ---------------//  
	0x50, 0x0000, // Horizontal GRAM Start Address  
	0x51, 0x00EF, // Horizontal GRAM End Address  
	0x52, 0x0000, // Vertical GRAM Start Address  
	0x53, 0x013F, // Vertical GRAM Start Address  
	0x60, 0xA700, // Gate Scan Line  
	0x61, 0x0001, // NDL,VLE, REV   
	0x6A, 0x0000, // set scrolling line  
	//-------------- Partial Display Control ---------//  
	0x80, 0x0000,  
	0x81, 0x0000,  
	0x82, 0x0000,  
	0x83, 0x0000,  
	0x84, 0x0000,  
	0x85, 0x0000,  
	//-------------- Panel Control -------------------//  
	0x90, 0x0010,  
	0x92, 0x0600,  
	0x07, 0x0133 // 262K color and display ON        
	  };
 #endif


	  	  #if !defined ILI_9325D && !defined ILI_9325C
  static const uint16_t ILI932x_regValues[] PROGMEM = 
	  {
  ILI932X_START_OSC        , 0x0001, // Start oscillator
  TFTLCD_DELAY             , 50,     // 50 millisecond delay
  ILI932X_DRIV_OUT_CTRL    , 0x0100,
  ILI932X_DRIV_WAV_CTRL    , 0x0700,
  ILI932X_ENTRY_MOD        , 0x1030,
  ILI932X_RESIZE_CTRL      , 0x0000,
  ILI932X_DISP_CTRL2       , 0x0202,
  ILI932X_DISP_CTRL3       , 0x0000,
  ILI932X_DISP_CTRL4       , 0x0000,
  ILI932X_RGB_DISP_IF_CTRL1, 0x0,
  ILI932X_FRM_MARKER_POS   , 0x0,
  ILI932X_RGB_DISP_IF_CTRL2, 0x0,
  ILI932X_POW_CTRL1        , 0x0000,
  ILI932X_POW_CTRL2        , 0x0007,
  ILI932X_POW_CTRL3        , 0x0000,
  ILI932X_POW_CTRL4        , 0x0000,
  TFTLCD_DELAY             , 200,
  ILI932X_POW_CTRL1        , 0x1690,
  ILI932X_POW_CTRL2        , 0x0227,
  TFTLCD_DELAY             , 50,
  ILI932X_POW_CTRL3        , 0x001A,
  TFTLCD_DELAY             , 50,
  ILI932X_POW_CTRL4        , 0x1800,
  ILI932X_POW_CTRL7        , 0x002A,
  TFTLCD_DELAY             , 50,
  ILI932X_GAMMA_CTRL1      , 0x0000,
  ILI932X_GAMMA_CTRL2      , 0x0000,
  ILI932X_GAMMA_CTRL3      , 0x0000,
  ILI932X_GAMMA_CTRL4      , 0x0206,
  ILI932X_GAMMA_CTRL5      , 0x0808,
  ILI932X_GAMMA_CTRL6      , 0x0007,
  ILI932X_GAMMA_CTRL7      , 0x0201,
  ILI932X_GAMMA_CTRL8      , 0x0000,
  ILI932X_GAMMA_CTRL9      , 0x0000,
  ILI932X_GAMMA_CTRL10     , 0x0000,
  ILI932X_GRAM_HOR_AD      , 0x0000,
  ILI932X_GRAM_VER_AD      , 0x0000,
  ILI932X_HOR_START_AD     , 0x0000,
  ILI932X_HOR_END_AD       , 0x00EF,
  ILI932X_VER_START_AD     , 0X0000,
  ILI932X_VER_END_AD       , 0x013F,
  ILI932X_GATE_SCAN_CTRL1  , 0xA700, // Driver Output Control (R60h)
  ILI932X_GATE_SCAN_CTRL2  , 0x0003, // Driver Output Control (R61h)
  ILI932X_GATE_SCAN_CTRL3  , 0x0000, // Driver Output Control (R62h)
  ILI932X_PANEL_IF_CTRL1   , 0X0010, // Panel Interface Control 1 (R90h)
  ILI932X_PANEL_IF_CTRL2   , 0X0000,
  ILI932X_PANEL_IF_CTRL3   , 0X0003,
  ILI932X_PANEL_IF_CTRL4   , 0X1100,
  ILI932X_PANEL_IF_CTRL5   , 0X0000,
  ILI932X_PANEL_IF_CTRL6   , 0X0000,
  ILI932X_DISP_CTRL1       , 0x0133, // Main screen turn on
     };
 #endif

    // ILI9341 mcufriend (8bit)
static const uint8_t ILI9341_regValues[] PROGMEM = {
	HX8357_SWRESET, 0,
	0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
	0xCF, 3, 0x00, 0xC1, 0x30,
	0xE8, 3, 0x85, 0x00, 0x78,
	0xEA, 2, 0x00, 0x00,
	0xED, 4, 0x64, 0x03, 0x12, 0x81,
	0xF7, 1, 0x20,
	0xC0, 1, 0x23,
	0xC1, 1, 0x10,
	0xC5, 2, 0x3E, 0x28,
	0xC7, 1, 0x86,
	0x36, 1, 0x48,
	0x3A, 1, 0x55,
	0xB1, 2, 0x00, 0x18,
	0xB6, 3, 0x08, 0x82, 0x27,
	0xF2, 1, 0x00,
	0x26, 1, 0x01,
	0xE0,15, 0x0f,0x31,0x2b,0x0c,0x0e,0x08,0x4e,0xf1,0x37,0x07,0x10,0x03,0x0e,0x09,0x00,
	0xE1,15, 0x00,0x0e,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0f,0x0c,0x31,0x36,0x0f,
	0x11, 0,
	TFTLCD_DELAY, 150,
	0x29, 0,
	0x2C, 0,
};

   #ifdef ILI_AND_MORE
static const uint8_t HX8347G_regValues[] PROGMEM = {
  0x2E           , 0x89,
  0x29           , 0x8F,
  0x2B           , 0x02,
  0xE2           , 0x00,
  0xE4           , 0x01,
  0xE5           , 0x10,
  0xE6           , 0x01,
  0xE7           , 0x10,
  0xE8           , 0x70,
  0xF2           , 0x00,
  0xEA           , 0x00,
  0xEB           , 0x20,
  0xEC           , 0x3C,
  0xED           , 0xC8,
  0xE9           , 0x38,
  0xF1           , 0x01,

  // skip gamma, do later

  0x1B           , 0x1A,
  0x1A           , 0x02,
  0x24           , 0x61,
  0x25           , 0x5C,
  
  0x18           , 0x36,
  0x19           , 0x01,
  0x1F           , 0x88,
  TFTLCD_DELAY   , 5   , // delay 5 ms
  0x1F           , 0x80,
  TFTLCD_DELAY   , 5   ,
  0x1F           , 0x90,
  TFTLCD_DELAY   , 5   ,
  0x1F           , 0xD4,
  TFTLCD_DELAY   , 5   ,
  0x17           , 0x05,

  0x36           , 0x09,
  0x28           , 0x38,
  TFTLCD_DELAY   , 40  ,
  0x28           , 0x3C,

  0x02           , 0x00,
  0x03           , 0x00,
  0x04           , 0x00,
  0x05           , 0xEF,
  0x06           , 0x00,
  0x07           , 0x00,
  0x08           , 0x01,
  0x09           , 0x3F
};

static const uint8_t HX8357D_regValues[] PROGMEM = {
  HX8357_SWRESET, 0,
  HX8357D_SETC, 3, 0xFF, 0x83, 0x57,
  TFTLCD_DELAY, 250,
  HX8357_SETRGB, 4, 0x00, 0x00, 0x06, 0x06,
  HX8357D_SETCOM, 1, 0x25,  // -1.52V
  HX8357_SETOSC, 1, 0x68,  // Normal mode 70Hz, Idle mode 55 Hz
  HX8357_SETPANEL, 1, 0x05,  // BGR, Gate direction swapped
  HX8357_SETPWR1, 6, 0x00, 0x15, 0x1C, 0x1C, 0x83, 0xAA,
  HX8357D_SETSTBA, 6, 0x50, 0x50, 0x01, 0x3C, 0x1E, 0x08,
  // MEME GAMMA HERE
  HX8357D_SETCYC, 7, 0x02, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x78,
  HX8357_COLMOD, 1, 0x55,
  HX8357_MADCTL, 1, 0xC0,
  HX8357_TEON, 1, 0x00,
  HX8357_TEARLINE, 2, 0x00, 0x02,
  HX8357_SLPOUT, 0,
  TFTLCD_DELAY, 150,
  HX8357_DISPON, 0, 
  TFTLCD_DELAY, 50,
};

static const uint8_t ILI9327_regValues[] PROGMEM = {
	HX8357_SWRESET, 0,
	0xE9, 1, 0x20,
	0x11, 0,
	TFTLCD_DELAY, 100,
	0xD1, 3, 0x00, 0x71, 0x19,
	0xD0, 3, 0x07, 0x01, 0x08,
	0x36, 1, 0x48,
	0x3A, 1, 0x05,
	0xC1, 4, 0x10, 0x10, 0x02, 0x02,
	0xC0, 6, 0x00, 0x31, 0x00, 0x00, 0x01, 0x02,
	0xC5, 1, 0x04,
	0xD2, 2, 0x01, 0x44,
	0xC8,15, 0x04, 0x67, 0x35, 0x04, 0x08, 0x06, 0x24, 0x01, 0x37, 0x40, 0x03, 0x10, 0x08, 0x80, 0x00,
	0x2A, 4, 0x00, 0x00, 0x00, 0xEF,
	0x2B, 5, 0x00, 0x00, 0x01, 0x3F, 0x8F,
	0x29, 0,
	0x2C, 0,
	
};

static const uint8_t ILI9488_regValues[] PROGMEM = {
	HX8357_SWRESET, 0,
	0xE0, 15, 0x00, 0x07, 0x10, 0x09, 0x17, 0x0B, 0x40, 0x8A, 0x4B, 0x0A, 0x0D, 0x0F, 0x15, 0x16, 0x0F,
	0xE1, 15, 0x00, 0x1A, 0x1B, 0x02, 0x0D, 0x05, 0x30, 0x35, 0x43, 0x02, 0x0A, 0x09, 0x32, 0x36, 0x0F,
	0xB1,  1, 0xA0,
	0xB4,  1, 0x02,
	0xC0,  2, 0x17, 0x15,
	0xC1,  1, 0x41,
	0xC5,  3, 0x00, 0x0A, 0x80,
	0xB6,  1, 0x02,
	0x36,  1, 0x48,
	0x3A,  1, 0x55,
	0xE9,  1, 0x00,
	0xF7,  4, 0xA9 ,0x51, 0x2C, 0x82,
	0x11,  0,
	TFTLCD_DELAY, 120,
	0x29,  0,
};

static const uint16_t S6D0154_regValues[] PROGMEM = 
	  {
  //0x80, 0x008D, //Testkey
  //0x92, 0x0010,
   0x11, 0x001A,
   0x12, 0x3121,
   0x13, 0x006C,
   0x14, 0x4249,
   0x10, 0x0800,
   TFTLCD_DELAY , 10,
   0x11, 0x011A,
   TFTLCD_DELAY , 10,
   0x11, 0x031A,
   TFTLCD_DELAY , 10,
   0x11, 0x071A,
   TFTLCD_DELAY , 10,
   0x11, 0x0F1A,
   TFTLCD_DELAY , 20,
   0x11, 0x0F3A,
   TFTLCD_DELAY , 30,
   0x01, 0x0128,
   0x02, 0x0100,
   0x03, 0x1030,
   0x07, 0x1012,
   0x08, 0x0303,
   0x0B, 0x1100,
   0x0C, 0x0000,
   0x0F, 0x1801,
   0x15, 0x0020,
   /*
   0x50, 0x0101,
   0x51, 0x0603,
   0x52, 0x0408,
   0x53, 0x0000,
   0x54, 0x0605,
   0x55, 0x0406,
   0x56, 0x0303,
   0x57, 0x0303,
   0x58, 0x0010,
   0x59, 0x1000, 
   */
   0x07, 0x0012,
   TFTLCD_DELAY , 40,
   0x07, 0x0013,      /*  GRAM Address Set */
   0x07, 0x0017       /*  Display Control  DISPLAY ON */
	  };
       
	   
	   #if 0 
       static const uint16_t ST7781_regValues[] PROGMEM = {
            0x00FF, 0x0001,     //can we do 0xFF
            0x00F3, 0x0008,
            //  LCD_Write_COM(0x00F3,

            0x00, 0x0001,
            0x0001, 0x0100,     // Driver Output Control Register (R01h)
            0x0002, 0x0700,     // LCD Driving Waveform Control (R02h)
            0x0003, 0x1030,     // Entry Mode (R03h)
            0x0008, 0x0302,
            0x0009, 0x0000,
            0x0010, 0x0000,     // Power Control 1 (R10h)
            0x0011, 0x0007,     // Power Control 2 (R11h)
            0x0012, 0x0000,     // Power Control 3 (R12h)
            0x0013, 0x0000,     // Power Control 4 (R13h)
            TFTLCD_DELAY, 50,
            0x0010, 0x14B0,     // Power Control 1 (R10h)
            TFTLCD_DELAY, 10,
            0x0011, 0x0007,     // Power Control 2 (R11h)
            TFTLCD_DELAY, 10,
            0x0012, 0x008E,     // Power Control 3 (R12h)
            0x0013, 0x0C00,     // Power Control 4 (R13h)
            0x0029, 0x0015,     // NVM read data 2 (R29h)
            TFTLCD_DELAY, 10,
            0x0030, 0x0000,     // Gamma Control 1
            0x0031, 0x0107,     // Gamma Control 2
            0x0032, 0x0000,     // Gamma Control 3
            0x0035, 0x0203,     // Gamma Control 6
            0x0036, 0x0402,     // Gamma Control 7
            0x0037, 0x0000,     // Gamma Control 8
            0x0038, 0x0207,     // Gamma Control 9
            0x0039, 0x0000,     // Gamma Control 10
            0x003C, 0x0203,     // Gamma Control 13
            0x003D, 0x0403,     // Gamma Control 14
            0x0060, 0xA700,     // Driver Output Control (R60h) .kbv was 0xa700
            0x0061, 0x0001,     // Driver Output Control (R61h)
            0x0090, 0X0029,     // Panel Interface Control 1 (R90h)

            // Display On
            0x0007, 0x0133,     // Display Control (R07h)
            TFTLCD_DELAY, 50,
        };
     #endif       /**/


   #endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

