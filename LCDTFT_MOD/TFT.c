//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    /*
*  TFT Display init Lib
*  Created: 25.6.2017 12:48:21
*  based on Adafruit team TFT Lib for Arduino
*  C++ classes stripped, functions reorganized
*  and then it all ported to C for an bare AVR MCU
*   Simon  Katznelson
*   x3merz@gmail.com
*
  */
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     #include "TFT.h"

     #define delayMicroseconds(val)  _delay_us(val)  

   void delay(uint16_t d)
     {
     while(d--)
	     {
	 _delay_ms (1);
       }
     }

   

     // Initialization 
    void tft_init(void) 
	{
	CTRL_DIR |=  RD_MASK | WR_MASK | CD_MASK | CS_MASK | RST_MASK;
	CTRL_PORT |= RD_MASK | WR_MASK | CD_MASK | CS_MASK | RST_MASK;
	DATA_DIR      |=    0xFF;  // as output
	DATA_PORT    =   0xFF;   // all pins high / pullups on
  
  setWriteDir(); // Set up LCD data port(s) for WRITE operations

  rotation  = 0;
  cursor_y  = cursor_x = 0;
  textsize  = 1;
  textcolor = 0xFFFF;
  _width    = TFTWIDTH;
  _height   = TFTHEIGHT;

	 }


// Set value of TFT register: 8-bit address, 8-bit value
  void writeRegister8(uint8_t a, uint8_t d) { 
  CD_COMMAND; write8(a); CD_DATA; write8(d); }

// Set value of TFT register: 16-bit address, 16-bit value
// See notes at top about macro expansion, hence hi & lo temp vars
  void writeRegister16(uint16_t a, uint16_t d) { 
  uint8_t hi, lo; 
  hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); 
  hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); }

// Set value of 2 TFT registers: Two 8-bit addresses (hi & lo), 16-bit value
  void writeRegisterPair(uint8_t aH, uint8_t aL, uint16_t d) { 
  uint8_t hi = (d) >> 8, lo = (d); 
  CD_COMMAND; write8(aH); CD_DATA; write8(hi); 
  CD_COMMAND; write8(aL); CD_DATA; write8(lo); }



void begin(void) 
	{
  	 #ifdef ID_CHK
     delay(100); //reset();
 	 id = readID();
  //  id = 0x9341;
	 #endif


  uint8_t i = 0;
  reset();
  delay(200);

   if((id == 0x9325) || (id == 0x9328)) {

    uint16_t a, d;
    driver = ID_932X;
    CS_ACTIVE;
    while(i < sizeof(ILI932x_regValues) / sizeof(uint16_t)) {
      a = pgm_read_word(&ILI932x_regValues[i++]);
      d = pgm_read_word(&ILI932x_regValues[i++]);
      if(a == TFTLCD_DELAY) delay(d);
      else                  writeRegister16(a, d);
    }
    setRotation(rotation);
    setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);

  } else if (id == 0x9341) {
      #ifndef  ILI9341_8BIT
	      //ILI9341 adafruit 
   //  uint16_t a, d;
    driver = ID_9341;
    CS_ACTIVE;
    writeRegister8(ILI9341_SOFTRESET, 0);
    delay(50);
    writeRegister8(ILI9341_DISPLAYOFF, 0);

    writeRegister8(ILI9341_POWERCONTROL1, 0x23);
    writeRegister8(ILI9341_POWERCONTROL2, 0x10);
    writeRegister16(ILI9341_VCOMCONTROL1, 0x2B2B);
    writeRegister8(ILI9341_VCOMCONTROL2, 0xC0);
    writeRegister8(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    writeRegister8(ILI9341_PIXELFORMAT, 0x55);
    writeRegister16(ILI9341_FRAMECONTROL, 0x001B);
    
    writeRegister8(ILI9341_ENTRYMODE, 0x07);
    // writeRegister32(ILI9341_DISPLAYFUNC, 0x0A822700); 

    writeRegister8(ILI9341_SLEEPOUT, 0);
    delay(150);
    writeRegister8(ILI9341_DISPLAYON, 0);
    delay(500);
    setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
    return;
    /* */
	    #else
// ILI9341 mcufriend (8bit)
	driver = ID_9341;
	CS_ACTIVE;
	while(i < sizeof(ILI9341_regValues)) {
		uint8_t r = pgm_read_byte(&ILI9341_regValues[i++]);
		uint8_t len = pgm_read_byte(&ILI9341_regValues[i++]);
		if(r == TFTLCD_DELAY) {
			delay(len);
			} else {
			//Serial.print("Register $"); Serial.print(r, HEX);
			//Serial.print(" datalen "); Serial.println(len);

			CS_ACTIVE;
			CD_COMMAND;
			write8(r);
			CD_DATA;
			for (uint8_t d=0; d<len; d++) {
				uint8_t x = pgm_read_byte(&ILI9341_regValues[i++]);
				write8(x);
			    }
			CS_IDLE;

		    }
	    }
		
                   return;
        #endif
  } 
    #ifdef  ILI_AND_MORE
   else if (id == 0x8357) {
    // HX8357D
    driver = ID_HX8357D;
    CS_ACTIVE;
     while(i < sizeof(HX8357D_regValues)) {
      uint8_t r = pgm_read_byte(&HX8357D_regValues[i++]);
      uint8_t len = pgm_read_byte(&HX8357D_regValues[i++]);
      if(r == TFTLCD_DELAY) {
	delay(len);
      } else {
	//Serial.print("Register $"); Serial.print(r, HEX);
	//Serial.print(" datalen "); Serial.println(len);

	CS_ACTIVE;
	CD_COMMAND;
	write8(r);
	CD_DATA;
	for (uint8_t d=0; d<len; d++) {
	  uint8_t x = pgm_read_byte(&HX8357D_regValues[i++]);
	  write8(x);
	}
	CS_IDLE;

      }
    }
     return;
     
  } else if(id == 0x7575) {

    uint8_t a, d;
    driver = ID_7575;
    CS_ACTIVE;
    while(i < sizeof(HX8347G_regValues)) {
      a = pgm_read_byte(&HX8347G_regValues[i++]);
      d = pgm_read_byte(&HX8347G_regValues[i++]);
      if(a == TFTLCD_DELAY) delay(d);
      else                  writeRegister8(a, d);
    }
    setRotation(rotation);
    setLR(); // Lower-right corner of address window

   }/**/
   else if(id == 0x0154) {
  //S6D0154
   uint16_t a, d;
   driver = ID_S6D0154;
   CS_ACTIVE;
      while(i < sizeof(S6D0154_regValues) / sizeof(uint16_t)) {
      a = pgm_read_word(&S6D0154_regValues[i++]);
      d = pgm_read_word(&S6D0154_regValues[i++]);
      if(a == TFTLCD_DELAY) delay(d);
      else                  writeRegister16(a, d);
    }
   setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
  } 
  /*
  else if(id == 0x7783) {
  //ST7783
   uint16_t a, d;
   driver = ID_ST7781; //  ID_932X;
   CS_ACTIVE;
      while(i < sizeof(ST7781_regValues) / sizeof(uint16_t)) {
      a = pgm_read_word(&ST7781_regValues[i++]);
      d = pgm_read_word(&ST7781_regValues[i++]);
      if(a == TFTLCD_DELAY) delay(d);
      else                  writeRegister16(a, d);
    }
   setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
  } 
*/
  #endif
    else {
    driver = ID_UNKNOWN;
    return;
  }
}



void reset(void) {

  CS_IDLE;
//  CD_DATA;
  WR_IDLE;
  RD_IDLE;

  CTRL_PORT &= ~ RST_MASK;
  delay(2);
  CTRL_PORT |= RST_MASK;

  // Data transfer sync
  CS_ACTIVE;
  CD_COMMAND;
  write8(0x00);
  for(uint8_t i = 0; i < 3; i++) WR_STROBE; // Three extra 0x00s
  CS_IDLE;
 
}



// Sets the LCD address window (and address counter, on 932X).
// Relevant to rect/screen fills and H/V lines.  Input coordinates are
// assumed pre-sorted (e.g. x2 >= x1).
    void setAddrWindow(int x1, int y1, int x2, int y2) {
                         if( busy ) return;
	 CS_ACTIVE;
     if( driver == ID_932X || driver == ID_S6D0154  ) {    //|| driver == ID_ST7781

    // Values passed are in current (possibly rotated) coordinate
    // system.  932X requires hardware-native coords regardless of
    // MADCTL, so rotate inputs as needed.  The address counter is
    // set to the top-left corner -- although fill operations can be
    // done in any direction, the current screen rotation is applied
    // because some users find it disconcerting when a fill does not
    // occur top-to-bottom.

//     if (x1 > x2) _swap_int16_t(x1, x2);
//     if (y1 > y2) _swap_int16_t(y1, y2);
	
	int x, y, t;
    switch(rotation) {
     default:
      x  = x1;
      y  = y1;
      break;
      case 1:
      t  = y1;
      y1 = x1;
      x1 = TFTWIDTH  - 1 - y2;
      y2 = x2;
      x2 = TFTWIDTH  - 1 - t;
      x  = x2;
      y  = y1;
      break;
      case 2:
      t  = x1;
      x1 = TFTWIDTH  - 1 - x2;
      x2 = TFTWIDTH  - 1 - t;
      t  = y1;
      y1 = TFTHEIGHT - 1 - y2;
      y2 = TFTHEIGHT - 1 - t;
      x  = x2;
      y  = y2;
      break;
      case 3:
      t  = x1;
      x1 = y1;
      y1 = TFTHEIGHT - 1 - x2;
      x2 = y2;
      y2 = TFTHEIGHT - 1 - t;
      x  = x1;
      y  = y2;
      break;
    }
    /**/	if(driver == ID_S6D0154) {
	writeRegister16(0x37, x1); //HorizontalStartAddress
	writeRegister16(0x36, x2); //HorizontalEndAddress
	writeRegister16(0x39, y1); //VerticalStartAddress
	writeRegister16(0x38, y2); //VertocalEndAddress
	writeRegister16(0x20, x); //GRAM Address Set
	writeRegister16(0x21, y);
	writeRegister8(0x22, 0);
	} else {
    //if( driver == ID_932X ) 
		writeRegister16(0x0050, x1); // Set address window ,Horizontal and Vertical RAM Address Position (R50h, R51h, R52h, R53h)
    writeRegister16(0x0051, x2);
    //if( driver == ID_932X ) 
		writeRegister16(0x0052, y1);
    writeRegister16(0x0053, y2);
    writeRegister16(0x0020, x );     // GRAM Address Set (Horizontal Address) (R20h)
    writeRegister16(0x0021, y );    // GRAM Address Set (Vertical Address) (R21h)
//	if( driver == ID_ST7781 ) writeRegister8(0x22, 0); // Write Data to GRAM (R22h)
	}

  } else if((driver == ID_9341) || (driver == ID_HX8357D) || (driver == ID_ILI9327) || (driver == ID_ILI9488)){ 
    uint32_t t;

    t = x1;
    t <<= 16;
    t |= x2;
    writeRegister32(ILI9341_COLADDRSET, t);  // HX8357D uses same registers!
    t = y1;
    t <<= 16;
    t |= y2;
    writeRegister32(ILI9341_PAGEADDRSET, t); // HX8357D uses same registers!

  }
     #ifdef ILI_AND_MORE
   else if(driver == ID_7575) {

    writeRegisterPair(HX8347G_COLADDRSTART_HI, HX8347G_COLADDRSTART_LO, x1);
    writeRegisterPair(HX8347G_ROWADDRSTART_HI, HX8347G_ROWADDRSTART_LO, y1);
    writeRegisterPair(HX8347G_COLADDREND_HI  , HX8347G_COLADDREND_LO  , x2);
    writeRegisterPair(HX8347G_ROWADDREND_HI  , HX8347G_ROWADDREND_LO  , y2);

  }/**/
  #endif
  CS_IDLE;
}

// Unlike the 932X drivers that set the address window to the full screen
// by default (using the address counter for drawPixel operations), the
// 7575 needs the address window set on all graphics operations.  In order
// to save a few register writes on each pixel drawn, the lower-right
// corner of the address window is reset after most fill operations, so
// that drawPixel only needs to change the upper left each time.
void setLR(void) {
  if( busy ) return;
  CS_ACTIVE;
  writeRegisterPair(HX8347G_COLADDREND_HI, HX8347G_COLADDREND_LO, _width  - 1);
  writeRegisterPair(HX8347G_ROWADDREND_HI, HX8347G_ROWADDREND_LO, _height - 1);
  CS_IDLE;
}


// Fast block fill operation for fillScreen, fillRect, H/V line, etc.
// Requires setAddrWindow() has previously been called to set the fill
// bounds.  'len' is inclusive, MUST be >= 1.
  void flood(uint16_t color, uint32_t len) {
  if( busy ) return;
  uint16_t blocks;
  uint8_t  i, hi = color >> 8,
                lo = color;

  CS_ACTIVE;
  CD_COMMAND;
  if (driver == ID_9341) {
    write8(0x2C);
  } else if (driver == ID_932X) {
    write8(0x00); // High byte of GRAM register...
    write8(0x22); // Write data to GRAM
  } else if (driver == ID_HX8357D) {
    write8(HX8357_RAMWR);
  } else {
    write8(0x22); // Write data to GRAM
  }

  // Write first pixel normally, decrement counter by 1
  CD_DATA;
  write8(hi);
  write8(lo);
  len--;

  blocks = (uint16_t)(len / 64); // 64 pixels/block
  if(hi == lo) {
    // High and low bytes are identical.  Leave prior data
    // on the port(s) and just toggle the write strobe.
    while(blocks--) {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do {
        WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // 2 bytes/pixel
        WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // x 4 pixels
      } while(--i);
    }
    // Fill any remaining pixels (1 to 64)
    for(i = (uint8_t)len & 63; i--; ) {
      WR_STROBE;
      WR_STROBE;
    }
  } else {
    while(blocks--) {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do {
        write8(hi); write8(lo); write8(hi); write8(lo);
        write8(hi); write8(lo); write8(hi); write8(lo);
      } while(--i);
    }
    for(i = (uint8_t)len & 63; i--; ) {
      write8(hi);
      write8(lo);
    }
  }
  CS_IDLE;
}


void drawFastHLine(int16_t x, int16_t y, int16_t length,  uint16_t color)
{
  int16_t x2;

 //    if (y > length) _swap_int16_t(y, length);
 

  // Initial off-screen clipping
  if((length <= 0     ) ||
     (y      <  0     ) || ( y                  >= _height) ||
     (x      >= _width) || ((x2 = (x+length-1)) <  0      )) return;

  if(x < 0) {        // Clip left
    length += x;
    x       = 0;
  }
  if(x2 >= _width) { // Clip right
    x2      = _width - 1;
    length  = x2 - x + 1;
  }

  setAddrWindow(x, y, x2, y);
  flood(color, length);
  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  setLR();
}
/**/


void drawFastVLine(int16_t x, int16_t y, int16_t length,   uint16_t color)
{
    //    if (x > length) _swap_int16_t(x, length);
  
  int16_t y2;

  // Initial off-screen clipping
  if((length <= 0      ) ||
     (x      <  0      ) || ( x                  >= _width) ||
     (y      >= _height) || ((y2 = (y+length-1)) <  0     )) return;
  if(y < 0) {         // Clip top
    length += y;
    y       = 0;
  }
  if(y2 >= _height) { // Clip bottom
    y2      = _height - 1;
    length  = y2 - y + 1;
  }

  setAddrWindow(x, y, x, y2);
  flood(color, length);
  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  setLR();
}
/**/ /**/


/*
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,  uint16_t color)
{
    //  if (x > h)  _swap_int16_t(x, h);
 	//  if (y > w) _swap_int16_t(y, w);
	
		if(rotation == 1 || rotation == 2)  // landscape?
		{
			for (int i = 0; i < ((h - y) / 2) + 1; i++)
			{
				drawFastHLine(x, y + i, w - x, color);
				drawFastHLine(x, h - i, w - x,  color);
			}
		}
		else
		{
			for (int i = 0; i < ((w - x) / 2) + 1; i++)
			{
				drawFastVLine(x + i, y, h - y, color);
				drawFastVLine(w - i, y, h - y, color);
			}
		}
	}
*/

/*
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
//    if (x > h)  _swap_int16_t(x, h);
//	if (y > w) _swap_int16_t(y, w);
	 
	// Update in subclasses if desired!
    startWrite();
    for (int16_t i = x; i < x+w; i++) {
        writeFastVLine(i, y, h, color);
    }
    endWrite();
}
   */

void fillScreen(uint16_t color) {
    // Update in subclasses if desired!
fillRect(0, 0, _width, _height, color);
//	fillRect(_width, _height, 0, 0,  color);
}



void fillRect(int16_t x1, int16_t y1, int16_t w, int16_t h, 
  uint16_t fillcolor) {
 
  int16_t  x2, y2;

  // Initial off-screen clipping
  if( (w            <= 0     ) ||  (h             <= 0      ) ||
      (x1           >= _width) ||  (y1            >= _height) ||
     ((x2 = x1+w-1) <  0     ) || ((y2  = y1+h-1) <  0      )) return;
  if(x1 < 0) { // Clip left
    w += x1;
    x1 = 0;
  }
  if(y1 < 0) { // Clip top
    h += y1;
    y1 = 0;
  }
  if(x2 >= _width) { // Clip right
    x2 = _width - 1;
    w  = x2 - x1 + 1;
  }
  if(y2 >= _height) { // Clip bottom
    y2 = _height - 1;
    h  = y2 - y1 + 1;
  }

  setAddrWindow(x1, y1, x2, y2);
  flood(fillcolor, (uint32_t)w * (uint32_t)h);
  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                 
	  setLR();
}
/**/



/*
void fillScreen(uint16_t color) {
  
  if(driver == ID_932X) {

    // For the 932X, a full-screen address window is already the default
    // state, just need to set the address pointer to the top-left corner.
    // Although we could fill in any direction, the code uses the current
    // screen rotation because some users find it disconcerting when a
    // fill does not occur top-to-bottom.
    uint16_t x, y;
    switch(rotation) {
      default: x = 0            ; y = 0            ; break;
      case 1 : x = TFTWIDTH  - 1; y = 0            ; break;
      case 2 : x = TFTWIDTH  - 1; y = TFTHEIGHT - 1; break;
      case 3 : x = 0            ; y = TFTHEIGHT - 1; break;
    }
    CS_ACTIVE;
    writeRegister16(0x0020, x);
    writeRegister16(0x0021, y);

  } else if ((driver == ID_9341) || (driver == ID_7575) || (driver == ID_HX8357D)) {
    // For these, there is no settable address pointer, instead the
    // address window must be set for each drawing operation.  However,
    // this display takes rotation into account for the parameters, no
    // need to do extra rotation math here.
    setAddrWindow(0, 0, _width - 1, _height - 1);

  }
  flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}
*/
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  if( busy ) return;
  // Clip
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  CS_ACTIVE;
  if((driver == ID_932X) || (driver == ID_S6D0154)) {
    int16_t t;
    switch(rotation) {
     case 1:
      t = x;
      x = TFTWIDTH  - 1 - y;
      y = t;
      break;
     case 2:
      x = TFTWIDTH  - 1 - x;
      y = TFTHEIGHT - 1 - y;
      break;
     case 3:
      t = x;
      x = y;
      y = TFTHEIGHT - 1 - t;
      break;
    }
    writeRegister16(0x0020, x);
    writeRegister16(0x0021, y);
    writeRegister16(0x0022, color);

  } else if(driver == ID_7575) {

    uint8_t hi, lo;
    switch(rotation) {
     default: lo = 0   ; break;
     case 1 : lo = 0x60; break;
     case 2 : lo = 0xc0; break;
     case 3 : lo = 0xa0; break;
    }
    writeRegister8(   HX8347G_MEMACCESS      , lo);
    // Only upper-left is set -- bottom-right is full screen default
    writeRegisterPair(HX8347G_COLADDRSTART_HI, HX8347G_COLADDRSTART_LO, x);
    writeRegisterPair(HX8347G_ROWADDRSTART_HI, HX8347G_ROWADDRSTART_LO, y);
    hi = color >> 8; lo = color;
    CD_COMMAND; write8(0x22); CD_DATA; write8(hi); write8(lo);

  } else if ((driver == ID_9341) || (driver == ID_HX8357D) || (driver == ID_ILI9327) || (driver == ID_ILI9488)) {
    setAddrWindow(x, y, _width-1, _height-1);
    CS_ACTIVE;
    CD_COMMAND; 
    write8(0x2C);
    CD_DATA; 
    write8(color >> 8); write8(color);
  } //else if (driver == ID_S6D0154) {
	//setAddrWindow(x, y, _width-1, _height-1);
	//CS_ACTIVE;
	//CD_COMMAND;
	//write8(0x22);
	//CD_DATA;
	//write8(color >> 8); write8(color);
 // }

  CS_IDLE;
}

// Issues 'raw' an array of 16-bit color values to the LCD; used
// externally by BMP examples.  Assumes that setWindowAddr() has
// previously been set to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).
void pushColors(uint16_t *data, uint8_t len, boolean first) {
 if( busy ) return;
  uint16_t color;
  uint8_t  hi, lo;
  CS_ACTIVE;
  if(first == true) { // Issue GRAM write command only on first call
    CD_COMMAND;
    if(driver == ID_932X) write8(0x00);
    if ((driver == ID_9341) || (driver == ID_HX8357D)){
       write8(0x2C);
     }  else {
       write8(0x22);
     }
  }
  CD_DATA;
  while(len--) {
    color = *data++;
    hi    = color >> 8; // Don't simplify or merge these
    lo    = color;      // lines, there's macro shenanigans
    write8(hi);         // going on.
    write8(lo);
  }
  CS_IDLE;
}


void setRotation(uint8_t x) {
         if( busy ) return;
 
  // Call parent rotation func first -- sets up rotation flags, etc.
	rotation = (x & 3);
    
    switch(rotation) {
        case 0:
        case 2:
      
		    _width  = TFTWIDTH;
            _height = TFTHEIGHT;
            break;
        case 1:
        case 3:
        
		    _width  = TFTHEIGHT;
            _height = TFTWIDTH;
            break;
    }
	/* */
  
  // Then perform hardware-specific rotation operations...



  CS_ACTIVE;
  if((driver == ID_932X) || (driver == ID_S6D0154)){

    uint16_t t;
    switch(rotation) {
     default: t = 0x1030; break;
     case 1 : t = 0x1028; break;
     case 2 : t = 0x1000; break;
     case 3 : t = 0x1018; break;  
    }
   writeRegister16(0x0003, t ); // MADCTL
    // For 932X, init default full-screen address window:
    setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here

  }

   if (driver == ID_9341) { 
   // MEME, HX8357D uses same registers as 9341 but different values
    uint16_t t= ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;

    switch (rotation) {
     case 2:
     t = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR;
     break;
     case 3:
     t = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
     break;
     case 0:
     t = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
     break;
     case 1:
     t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
     break;
  }
   writeRegister8(ILI9341_MADCTL, t ); // MADCTL
   // For 9341, init default full-screen address window:
   setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
  }

    if(driver == ID_7575) {

    uint8_t t;
    switch(rotation) {
     default: t = 0   ; break;
     case 1 : t = 0x60; break;
     case 2 : t = 0xc0; break;
     case 3 : t = 0xa0; break;
    }
    writeRegister8(HX8347G_MEMACCESS, t);
    // 7575 has to set the address window on most drawing operations.
    // drawPixel() cheats by setting only the top left...by default,
    // the lower right is always reset to the corner.
    setLR(); // CS_IDLE happens here
  }
   if (driver == ID_HX8357D) { 
    // MEME, HX8357D uses same registers as 9341 but different values
    uint16_t t = 0;
    
    switch (rotation) {
      case 2:
        t = HX8357B_MADCTL_RGB;
        break;
      case 3:
        t = HX8357B_MADCTL_MX | HX8357B_MADCTL_MV | HX8357B_MADCTL_RGB;
        break;
      case 0:
        t = HX8357B_MADCTL_MX | HX8357B_MADCTL_MY | HX8357B_MADCTL_RGB;
        break;
      case 1:
        t = HX8357B_MADCTL_MY | HX8357B_MADCTL_MV | HX8357B_MADCTL_RGB;
        break;
    }
    writeRegister8(ILI9341_MADCTL, t ); // MADCTL
    // For 8357, init default full-screen address window:
    setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
  }
  
   if (driver ==ID_ILI9327) {
	   // MEME, HX8357D uses same registers as 9341 but different values
	   uint16_t t = 0;

	   switch (rotation) {
		   case 2:
		   t =  ILI9341_MADCTL_BGR | 0x80;
		   break;
		   case 3:
		   t =  ILI9341_MADCTL_BGR | 0xE0;
		   break;
		   case 0:
		   t =  ILI9341_MADCTL_BGR | 0x02;
		   break;
		   case 1:
		   t =  ILI9341_MADCTL_BGR | 0x20;
		   break;
	   }
	   writeRegister8(ILI9341_MADCTL, t); // MADCTL
	   // For 9341, init default full-screen address window:
	   setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
   }
}


// Because this function is used infrequently, it configures the ports for
// the read operation, reads the data, then restores the ports to the write
// configuration.  Write operations happen a LOT, so it's advantageous to
// leave the ports in that state as a default.
uint16_t readPixel(int16_t x, int16_t y) {
     if( busy ) return 0;
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return 0;

  CS_ACTIVE;
  if((driver == ID_932X) || (driver == ID_S6D0154) || (driver == ID_ILI9327) || (driver == ID_ILI9488)) {

    uint8_t hi, lo;
    int16_t t;
    switch(rotation) {
     case 1:
      t = x;
      x = TFTWIDTH  - 1 - y;
      y = t;
      break;
     case 2:
      x = TFTWIDTH  - 1 - x;
      y = TFTHEIGHT - 1 - y;
      break;
     case 3:
      t = x;
      x = y;
      y = TFTHEIGHT - 1 - t;
      break;
    }
	if (driver == ID_ILI9327) {
		writeRegister16(0x002A, x);
		writeRegister16(0x002B, y);
	} else {
		writeRegister16(0x0020, x);
		writeRegister16(0x0021, y);
	}
    // Inexplicable thing: sometimes pixel read has high/low bytes
    // reversed.  A second read fixes this.  Unsure of reason.  Have
    // tried adjusting timing in read8() etc. to no avail.
                 for(uint8_t pass = 0; pass < 2; pass++) {
	  if (driver == ID_ILI9327){
		CD_COMMAND; write8(0x2E); // Read data from GRAM
	  }else{
		CD_COMMAND; write8(0x00); write8(0x22); // Read data from GRAM
	  }
      CD_DATA;
      setReadDir();  // Set up LCD data port(s) for READ operations
      read8(hi);     // First 2 bytes back are a dummy read
      read8(hi);
      read8(hi);     // Bytes 3, 4 are actual pixel value
      read8(lo);
      setWriteDir(); // Restore LCD data port(s) to WRITE configuration
    }
    CS_IDLE;
    return ((uint16_t)hi << 8) | lo;

  } else if(driver == ID_7575) {

    uint8_t r, g, b;
    writeRegisterPair(HX8347G_COLADDRSTART_HI, HX8347G_COLADDRSTART_LO, x);
    writeRegisterPair(HX8347G_ROWADDRSTART_HI, HX8347G_ROWADDRSTART_LO, y);
    CD_COMMAND; write8(0x22); // Read data from GRAM
    setReadDir();  // Set up LCD data port(s) for READ operations
    CD_DATA;
    read8(r);      // First byte back is a dummy read
    read8(r);
    read8(g);
    read8(b);
    setWriteDir(); // Restore LCD data port(s) to WRITE configuration
    CS_IDLE;
    return (((uint16_t)r & 0b11111000) << 8) |
           (((uint16_t)g & 0b11111100) << 3) |
           (           b              >> 3);
  } else return 0;
}

  #ifdef ILI_AND_MORE
  ;
  #endif

#ifdef ID_CHK

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Ditto with the read/write port directions, as above.
  uint16_t readID(void) {

  uint8_t hi, lo;

  if (readReg(0x04) == 0x8000) { // eh close enough
    // setc!
   
    writeRegister24(HX8357D_SETC, 0xFF8357);
    delay(300);
    //Serial.println(readReg(0xD0), HEX);
    if (readReg(0xD0) == 0x990000) {
      return 0x8357;
    }
  }

  uint16_t id = readReg(0xD3);
  if (id == 0x9341) {
    return 0x9341;
  }
  if (id == 0x9488) {
	return 0x9488;
  }
  
  id = readReg(0xEF);
  if (id == 0x9327) {
	return 0x9327;
  }
  
  CS_ACTIVE;
  CD_COMMAND;
  write8(0x00);
  WR_STROBE;     // Repeat prior byte (0x00)
  setReadDir();  // Set up LCD data port(s) for READ operations
  CD_DATA;
  delayMicroseconds(10);
  read8(hi);
  delayMicroseconds(10);
  read8(lo);
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration
  CS_IDLE;

  id = hi; id <<= 8; id |= lo;
  return id;
  
}




 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  uint32_t readReg(uint8_t r) {
  uint32_t id;
  uint8_t x;

  // try reading register #4
  CS_ACTIVE;
  CD_COMMAND;
  write8(r);
  setReadDir();  // Set up LCD data port(s) for READ operations
  CD_DATA;
 // delayMicroseconds(50);
  read8(x);
  id = x;          // Do not merge or otherwise simplify
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  CS_IDLE;
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration

  return id;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void writeRegister24(uint8_t r, uint32_t d) {
  CS_ACTIVE;
  CD_COMMAND;
  write8(r);
  CD_DATA;
  delayMicroseconds(10);
  write8(d >> 16);
  delayMicroseconds(10);
  write8(d >> 8);
  delayMicroseconds(10);
  write8(d);
  CS_IDLE;

}


void writeRegister32(uint8_t r, uint32_t d) {
  CS_ACTIVE;
  CD_COMMAND;
  write8(r);
  CD_DATA;
  delayMicroseconds(10);
  write8(d >> 24);
  delayMicroseconds(10);
  write8(d >> 16);
  delayMicroseconds(10);
  write8(d >> 8);
  delayMicroseconds(10);
  write8(d);
  CS_IDLE;

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
#endif
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



