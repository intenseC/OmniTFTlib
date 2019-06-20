/*
* TFT Display Handling app header
*
* written  by V. 'Larry' Beliacoff
*  x3merz@gmail.com
*/
   
          #define  ILI9325          // ILI9341
	      #define  ILI_AND_MORE
 	      #define  ID_CHK  // AUTO display driver type chip detect
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   #ifndef ID_CHK    
		#define id 0x9325  // set display driver type here manually
   #define readID()   id   // dummy placeholder
   #define readReg(val)   id    // dummy placeholder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   #else 
     uint32_t id = 0;
   #endif 
	 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
                  #define ILI9341_8BIT    /*  ILI9341 pure SPI mode ON/OFF  */
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

     bool busy = 0; 

unsigned char YP = (1 << PC3);
unsigned char XM = (1 << PC2);
unsigned char XP = (1 << PA1);
unsigned char YM = (1 << PA0);
unsigned char CH0 = 0;
unsigned char CH1 = 1;
 
#define  TOUCHPORT_A  PORTA
#define  TOUCHPIN_A DDRA
#define  TOUCHPORT_D  PORTC
#define  TOUCHPIN_D DDRC

   #define boolean bool


                 //Func Prototypes
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void delay(unsigned int d);
  void tft_init(void);
  void writeRegister8(uint8_t a, uint8_t d);
  void writeRegister16(uint16_t a, uint16_t d);
  void writeRegisterPair(uint8_t aH, uint8_t aL, uint16_t d);
  void begin(void);
  void reset(void);
  void pushColors(uint16_t *data, uint8_t len, boolean first);
  void setRotation(uint8_t x);
  uint16_t readPixel(int16_t x, int16_t y);
  uint16_t readID(void);
  uint32_t readReg(uint8_t r);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  void writeRegister24(uint8_t r, uint32_t d);
  void writeRegister32(uint8_t r, uint32_t d);
  void     drawPixel(int16_t x, int16_t y, uint16_t color);
  void     drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
  void     drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
  void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
  void     fillScreen(uint16_t color);
  void     setRegisters8(uint8_t *ptr, uint8_t n);
  void     setRegisters16(uint16_t *ptr, uint8_t n);
  void     setRotation(uint8_t x);
  void     setLR(void);
  void     setAddrWindow(int x1, int y1, int x2, int y2);
  void     pushColors(uint16_t *data, uint8_t len, boolean first);
  void     charBounds(char c, int16_t *x, int16_t *y,
      int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  void flood(uint16_t color, uint32_t len);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

