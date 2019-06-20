   /*
*  TFT Display low level Lib
*
*  largely based on Adafruit team TFT Lib, 
*  I also rewrote the Henning Karlsen's UTFT fonts handler  
*  and ported it all to C for bare AVR MCU
*   V. 'Larry' Beliacoff 
*   x3merz@gmail.com
*
  */
   
   
   
   
   ////////////////////////////////////////////////////////////////////////////////////////////
    #define ADAFONTS  // enable original Adafruit text handler
   ////////////////////////////////////////////////////////////////////////////////////////////
   #include <stdbool.h>
   #include <stdlib.h>
   #include <string.h>

   #ifdef ADAFONTS
   #include "gfxfont.h"  
   #include "glcdfont.h"
   #include "Orbitron_Light_16.h"

   #endif
   #include "MiniGFX.h"
   
    #define boolean bool
   #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
   #ifndef min
   #define min(a, b) (((a) < (b)) ? (a) : (b))
   #endif

   #ifndef _swap_int16_t
   #define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
   #endif
  
    #ifdef ADAFONTS
     GFXfont *gfxFont;
    #endif
 /*     UTFT lib port     */
//////////////////////////////////////////////////////////////////////////////////////////////////////
 extern int mode;
 uint8_t _transparent;
 uint8_t fch, fcl, bch, bcl;

typedef struct _current_font
{
	const uint8_t * font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
} cFont;

cFont cfont; 
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
	 
//=========================================================
void _setFont(const uint8_t * font);
void setColor(uint16_t color);
void setBackColor(uint32_t color);
void _printChar(char c, int x, int y);

#if 0
//////////////////////////////////////////////////////////////////////////////////////////////////////
 /*     UTFT lib port  END    */
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
   // setTextColor(uint16_t c),
    setTextColor(uint16_t c, uint16_t bg),
    setTextSize(uint8_t s),
    setTextWrap(boolean w),
    cp437(boolean x),
    	    #ifdef ADAFONTS
	setFont(const GFXfont *f),
            #endif
        /* getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h), */ 
	getTextBounds(char *string, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);


  void   write(uint8_t);

#endif
//=========================================================


void startWrite(){
    // Overwrite in subclasses if desired!
}

void endWrite(){
    // Overwrite in subclasses if startWrite is defined!
}


void writePixel(int16_t x, int16_t y, uint16_t color){
    // Overwrite in subclasses if startWrite is defined!
    drawPixel(x, y, color);
}


// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void writeFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Can be just writeLine(x, y, x, y+h-1, color);
    // or writeFillRect(x, y, 1, h, color);
    drawFastVLine(x, y, h, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void writeFastHLine(int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Example: writeLine(x, y, x+w-1, y, color);
    // or writeFillRect(x, y, w, 1, color);
    drawFastHLine(x, y, w, color);
}

void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    // Overwrite in subclasses if desired!
    fillRect(x,y,w,h,color);
}


// Bresenham's algorithm - thx wikpedia
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            writePixel(y0, x0, color);
        } else {
            writePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}




void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    // Update in subclasses if desired!
    if(x0 == x1){
        if(y0 > y1) _swap_int16_t(y0, y1);
        drawFastVLine(x0, y0, y1 - y0 + 1, color);
    } else if(y0 == y1){
        if(x0 > x1) _swap_int16_t(x0, x1);
        drawFastHLine(x0, y0, x1 - x0 + 1, color);
    } else {
        startWrite();
        writeLine(x0, y0, x1, y1, color);
        endWrite();
    }
}

/*   */
// Draw a circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    startWrite();
    writePixel(x0  , y0+r, color);
    writePixel(x0  , y0-r, color);
    writePixel(x0+r, y0  , color);
    writePixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 - x, y0 + y, color);
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 - x, y0 - y, color);
        writePixel(x0 + y, y0 + x, color);
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 + y, y0 - x, color);
        writePixel(x0 - y, y0 - x, color);
    }
    endWrite();
}

void drawCircleHelper( int16_t x0, int16_t y0,
        int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            writePixel(x0 + x, y0 + y, color);
            writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            writePixel(x0 + x, y0 - y, color);
            writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            writePixel(x0 - y, y0 + x, color);
            writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            writePixel(x0 - y, y0 - x, color);
            writePixel(x0 - x, y0 - y, color);
        }
    }
}

void fillCircle(int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {
    startWrite();
    writeFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
    endWrite();
}

// Used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
        uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            writeFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            writeFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            writeFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            writeFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

// Draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    startWrite();
    writeFastHLine(x, y, w, color);
    writeFastHLine(x, y+h-1, w, color);
    writeFastVLine(x, y, h, color);
    writeFastVLine(x+w-1, y, h, color);
    endWrite();
}

// Draw a rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    startWrite();
    writeFastHLine(x+r  , y    , w-2*r, color); // Top
    writeFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    writeFastVLine(x    , y+r  , h-2*r, color); // Left
    writeFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
    endWrite();
}

// Fill a rounded rectangle
void fillRoundRect(int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    startWrite();
    writeFillRect(x+r, y, w-2*r, h, color);

    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
    endWrite();
}

// Draw a triangle
void drawTriangle(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void fillTriangle(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

startWrite();
    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        writeFastHLine(a, y0, b-a+1, color);
        endWrite();
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        writeFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        writeFastHLine(a, y, b-a+1, color);
    }
    endWrite();
}




// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------
    #ifdef ADAFONTS
// Draw a character
void drawChar(int16_t x, int16_t y, unsigned char c,
  uint16_t color, uint16_t bg, uint8_t size) {

    if(!gfxFont) { // 'Classic' built-in font

        if((x >= _width)            || // Clip right
           (y >= _height)           || // Clip bottom
           ((x + 6 * size - 1) < 0) || // Clip left
           ((y + 8 * size - 1) < 0))   // Clip top
            return;

        if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

        startWrite();
        for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
            uint8_t line = pgm_read_byte(&font[c * 5 + i]);
            for(int8_t j=0; j<8; j++, line >>= 1) {
                if(line & 1) {
                    if(size == 1)
                        writePixel(x+i, y+j, color);
                    else
                        writeFillRect(x+i*size, y+j*size, size, size, color);
                } else if(bg != color) {
                    if(size == 1)
                        writePixel(x+i, y+j, bg);
                    else
                        writeFillRect(x+i*size, y+j*size, size, size, bg);
                }
            }
        }
        if(bg != color) { // If opaque, draw vertical line for last column
            if(size == 1) writeFastVLine(x+5, y, 8, bg);
            else          writeFillRect(x+5*size, y, size, 8*size, bg);
        }
        endWrite();

    } 
		else { // Custom font

        // Character is assumed previously filtered by write() to eliminate
        // newlines, returns, non-printable characters, etc.  Calling
        // drawChar() directly with 'bad' characters of font may cause mayhem!

        c -= (uint8_t)pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
        uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

        uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
        uint8_t  w  = pgm_read_byte(&glyph->width),
                 h  = pgm_read_byte(&glyph->height);
        int8_t   xo = pgm_read_byte(&glyph->xOffset),
                 yo = pgm_read_byte(&glyph->yOffset);
        uint8_t  xx, yy, bits = 0, bit = 0;
        int16_t  xo16 = 0, yo16 = 0;

        if(size > 1) {
            xo16 = xo;
            yo16 = yo;
        }

        // Todo: Add character clipping here

        // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
        // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
        // has typically been used with the 'classic' font to overwrite old
        // screen contents with new data.  This ONLY works because the
        // characters are a uniform size; it's not a sensible thing to do with
        // proportionally-spaced fonts with glyphs of varying sizes (and that
        // may overlap).  To replace previously-drawn text when using a custom
        // font, use the getTextBounds() function to determine the smallest
        // rectangle encompassing a string, erase the area with fillRect(),
        // then draw new text.  This WILL infortunately 'blink' the text, but
        // is unavoidable.  Drawing 'background' pixels will NOT fix this,
        // only creates a new set of problems.  Have an idea to work around
        // this (a canvas object type for MCUs that can afford the RAM and
        // displays supporting setAddrWindow() and pushColors()), but haven't
        // implemented this yet.

        startWrite();
        for(yy=0; yy<h; yy++) {
            for(xx=0; xx<w; xx++) {
                if(!(bit++ & 7)) {
                    bits = pgm_read_byte(&bitmap[bo++]);
                }
                if(bits & 0x80) {
                    if(size == 1) {
                        writePixel(x+xo+xx, y+yo+yy, color);
                    } else {
                        writeFillRect(x+(xo16+xx)*size, y+(yo16+yy)*size,
                          size, size, color);
                    }
                }
                bits <<= 1;
            }
        }
        endWrite();

    } // End classic vs custom font
	/**/
}


void write(uint8_t c) {

    if(!gfxFont) { // 'Classic' built-in font

        if(c == '\n') {                        // Newline?
            cursor_x  = 0;                     // Reset x to zero,
            cursor_y += textsize * 8;          // advance y one line
        } else if(c != '\r') {                 // Ignore carriage returns
            if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
                cursor_x  = 0;                 // Reset x to zero,
                cursor_y += textsize * 8;      // advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            cursor_x += textsize * 6;          // Advance x one char
        }

    } else { // Custom font

        if(c == '\n') {
            cursor_x  = 0;
            cursor_y += (int16_t)textsize *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        } else if(c != '\r') {
            uint8_t first = pgm_read_byte(&gfxFont->first);
            if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &gfxFont->glyph))[c - first]);
                uint8_t   w     = pgm_read_byte(&glyph->width),
                          h     = pgm_read_byte(&glyph->height);
                if((w > 0) && (h > 0)) { // Is there an associated bitmap?
                    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
                    if(wrap && ((cursor_x + textsize * (xo + w)) > _width)) {
                        cursor_x  = 0;
                        cursor_y += (int16_t)textsize *
                          (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                    }
                    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
                }
                cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
            }
        }

    }

}
#endif

void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}
    #ifdef ADAFONTS
int16_t getCursorX(void)  {
    return cursor_x;
}

int16_t getCursorY(void)  {
    return cursor_y;
}

void setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}
/*
void setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}
*/
void setTextColor(uint16_t c, uint16_t b) {
    textcolor   = c;
    textbgcolor = b;
}

void setTextWrap(boolean w) {
    wrap = w;
}

uint8_t getRotation(void)  {
    return rotation;
}

/* */

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void cp437(boolean x) {
    _cp437 = x;
}


void setFont(const GFXfont *f) {
    if(f) {            // Font struct pointer passed in?
        if(!gfxFont) { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
            cursor_y += 6;
        }
    } else if(gfxFont) { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
        cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
}

// Broke this out as it's used by both the PROGMEM- and RAM-resident
// getTextBounds() functions.
void charBounds(char c, int16_t *x, int16_t *y,
  int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {

    if(gfxFont) {

        if(c == '\n') { // Newline?
            *x  = 0;    // Reset x to zero, advance y by one line
            *y += textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        } else if(c != '\r') { // Not a carriage return; is normal char
            uint8_t first = pgm_read_byte(&gfxFont->first),
                    last  = pgm_read_byte(&gfxFont->last);
            if((c >= first) && (c <= last)) { // Char present in this font?
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &gfxFont->glyph))[c - first]);
                uint8_t gw = pgm_read_byte(&glyph->width),
                        gh = pgm_read_byte(&glyph->height),
                        xa = pgm_read_byte(&glyph->xAdvance);
                int8_t  xo = pgm_read_byte(&glyph->xOffset),
                        yo = pgm_read_byte(&glyph->yOffset);
                if(wrap && ((*x+(((int16_t)xo+gw)*textsize)) > _width)) {
                    *x  = 0; // Reset x to zero, advance y by one line
                    *y += textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                }
                int16_t ts = (int16_t)textsize,
                        x1 = *x + xo * ts,
                        y1 = *y + yo * ts,
                        x2 = x1 + gw * ts - 1,
                        y2 = y1 + gh * ts - 1;
                if(x1 < *minx) *minx = x1;
                if(y1 < *miny) *miny = y1;
                if(x2 > *maxx) *maxx = x2;
                if(y2 > *maxy) *maxy = y2;
                *x += xa * ts;
            }
        }

    } else { // Default font

        if(c == '\n') {                     // Newline?
            *x  = 0;                        // Reset x to zero,
            *y += textsize * 8;             // advance y one line
            // min/max x/y unchaged -- that waits for next 'normal' character
        } else if(c != '\r') {  // Normal char; ignore carriage returns
            if(wrap && ((*x + textsize * 6) > _width)) { // Off right?
                *x  = 0;                    // Reset x to zero,
                *y += textsize * 8;         // advance y one line
            }
            int x2 = *x + textsize * 6 - 1, // Lower-right pixel of char
                y2 = *y + textsize * 8 - 1;
            if(x2 > *maxx) *maxx = x2;      // Track max x, y
            if(y2 > *maxy) *maxy = y2;
            if(*x < *minx) *minx = *x;      // Track min x, y
            if(*y < *miny) *miny = *y;
            *x += textsize * 6;             // Advance x one char
        }
    }
}

// Pass string and a cursor position, returns UL corner and W,H.
void getTextBounds(char *str, int16_t x, int16_t y,
        int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c; // Current character

    *x1 = x;
    *y1 = y;
    *w  = *h = 0;

    int16_t minx = _width, miny = _height, maxx = -1, maxy = -1;

    while((c = *str++))
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if(maxx >= minx) {
        *x1 = minx;
        *w  = maxx - minx + 1;
    }
    if(maxy >= miny) {
        *y1 = miny;
        *h  = maxy - miny + 1;
    }
}


#endif

// Return the size of the display (per current rotation)
int16_t width(void)  {
    return _width;
}

int16_t height(void)  {
    return _height;
}

void invertDisplay(boolean i) {
    // Do nothing, must be subclassed if supported by hardware
}


/*     UTFT lib port     */
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define LEFT 0
#define RIGHT 9999
#define CENTER 9998
#define disp_x_size _width - 1  //TFTWIDTH //
#define disp_y_size _height - 1 //TFTHEIGHT //

void _setFont(const uint8_t *font)
{
	cfont.font = font;
	cfont.x_size = fontbyte(0);
	cfont.y_size = fontbyte(1);
	cfont.offset = fontbyte(2);
	cfont.numchars = fontbyte(3);
}

void setColor(uint16_t color)
{
	fch = (uint8_t)(color >> 8);
	fcl = (uint8_t)(color & 0xFF);
}

void setBackColor(uint32_t color)
{
	if (color == VGA_TRANSPARENT)
		_transparent = true;
	else
	{
		bch = (uint8_t)(color >> 8);
		bcl = (uint8_t)(color & 0xFF);
		_transparent = false;
	}
}


void _printChar(char c, int x, int y)
      {    
	uint8_t i, ch;
	uint16_t j;
	uint16_t temp; 
	
		                                                    // some thoughts on this function:
			temp = ((c - cfont.offset) * ((cfont.x_size / 8) * cfont.y_size)) + 4;  // letter vertical position in the font array
			for(j = 0; j < ((cfont.x_size / 8) * cfont.y_size); j += (cfont.x_size / 8))  // setting the letter window size
			    {
				for (int zz = (cfont.x_size / 8) - 1; zz >= 0; zz--)  // this fills letterbox in passes, one byte per each pass
				        {       // (applies 8-bit horizontal part of a letter per cicle), i. e. 16-bit wide letter will take two passes
				  	ch = pgm_read_byte(&cfont.font[temp + zz]);
					
					for(i = 0; i < 8; i++)
					                    {   
						if((ch&(1 << i)) != 0)   //anding each bit and filling with color accordingly
						{            // If got 1 - Foreground color fill
 							 writePixel(x - (i - zz * 8), y + (j / (cfont.x_size / 8)), (fch << 8) | fcl); 
						     }                     // moving 8 * zz pixels left each time 'zz' changes
						else if (!_transparent)
						           { // If got 0 - Background color fill
 							 writePixel(x - (i - zz * 8), y + (j / (cfont.x_size / 8)), (bch << 8) | bcl);
						     }   
					     } 
				     }
				temp += (cfont.x_size / 8); // switching to the next letter's part given it's larger than 8 bits
			  }
		  
	  }


//void print(char * st, int x, int y)
void print(char * st)
{
    int x = cursor_x;  int y = cursor_y;
	int stl, i;
	stl = strlen(st);
	
	if (x == RIGHT)
		x = (disp_x_size + 1) - (stl * cfont.x_size);
	if (x == CENTER)
		x = ((disp_x_size + 1) - (stl * cfont.x_size)) / 2;
	
	for (i = 0; i < stl; i++)
			_printChar(*st++, x + (i * (cfont.x_size)), y);
     cursor_x = x + (stl * (cfont.x_size));
}

