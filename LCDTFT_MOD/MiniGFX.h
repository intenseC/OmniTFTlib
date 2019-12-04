   /*
*  TFT Display init Lib header
*
*  based on Adafruit team TFT Lib,
*  ported to C for bare AVR MCU
*   Simon  Katznelson
*   x3merz@gmail.com
*
  */

 
 //=========================================================      
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

#define fontbyte(x) pgm_read_byte(&cfont.font[x])

  #define boolean bool

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998
#define disp_x_size _width - 1  //TFTWIDTH //
#define disp_y_size _height - 1 //TFTHEIGHT //

         // Prototypes
//=========================================================
void _setFont(const uint8_t * font);
void setColor(uint16_t color);
void setBackColor(uint32_t color);
void _printChar(char c, int x, int y);
//////////////////////////////////////////////////////////////////////////////////////////////////////
 
//=========================================================
   void startWrite(void);
   void writePixel(int16_t x, int16_t y, uint16_t color);
   void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
   void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
   void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
   void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
   void endWrite(void);

    void
    drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
    drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      uint16_t color),
    fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
    fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t color),
    drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color),
    fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color),
    drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color),
    fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);

    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size),
    setCursor(int16_t x, int16_t y),
    setTextColor(uint16_t c, uint16_t bg),
    setTextSize(uint8_t s),
    setTextWrap(boolean w),
    cp437(boolean x),
    	    #ifdef ADAFONTS
	setFont(const GFXfont *f),
            #endif
	getTextBounds(char *string, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);


  void   write(uint8_t);
  void print(char * st);
//=========================================================

#if 1
  extern int16_t _width, _height, // Display w/h as modified by current rotation
                 cursor_x, cursor_y;
  extern uint16_t  textcolor, textbgcolor;
  extern uint8_t  textsize,  rotation;
  extern bool   wrap,   // If set, 'wrap' text at right edge of display
  _cp437; // If set, use correct CP437 charset (default is off)

         // Prototypes
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
  void     setLR(void);
  void     setAddrWindow(int x1, int y1, int x2, int y2);
  void     pushColors(uint16_t *data, uint8_t len, boolean first);

  void charBounds(char c, int16_t *x, int16_t *y,
  int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  void flood(uint16_t color, uint32_t len);

 uint16_t readID(void);
 uint32_t readReg(uint8_t r);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


