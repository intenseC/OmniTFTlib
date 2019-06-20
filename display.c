/*
* TFT Display Handling app for AVR Atmega32
*  
* written  by V. 'Larry' Beliacoff 
*  x3merz@gmail.com
*/



//***************************************************************************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include "display.h"
#include "LCDTFT_MOD/MiniGFX.h"
#include "LCDTFT_MOD/OCR_A_Extended_M.h"



 //=========
 //  #define UARTSPITID   //transmit TFT ID over UART
 //=========



#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

 // S6D0154 TFT boards and some others employ a different touchpad pin mapping 

 #define  S6D0154  // alternative touchscreen pinout
 
 
 #define TOUCH_INTERVAL 50
 #define ADC_VREF_TYPE 0x40

 #define delay_ms _delay_ms

  
 #define BAUD 10400                
 #define BAUDRATE ((F_CPU) / (BAUD * 16UL) - 1)   // set baud rate value for UBRR
 #define UART_PKTSIZE 16  // in bytes


 // #define  TSTOUCH
//**********************************************************************************************************************
//**********************************************************************************************************************


/*   LCD_D6, LCD_D7, LCD_WR and LCD_RS are also serve as touchscreen pins.   */
#ifdef S6D0154
 bool forceAltTouchPin = 1;
#else
 bool forceAltTouchPin = 0;
#endif
 uint8_t tsPinArr;

 uint32_t coords;
 uint16_t coordX, coordY;
 uint8_t mycntr;
 volatile uint8_t ms_cnt = 0, s_cnt = 0, ns_cnt = 0, fract_count;
 volatile bool reading = 0, flip = 0;
 int mode;
  uint8_t pos = 0;
 uint8_t  push, touchcount, draw_consum, fill_consum = 0;
 bool keypress = 0;
 uint32_t IC_CODE; //Stores Driver IC ID 
 uint8_t j = 1;
 uint8_t cnt = 0;
 
 uint8_t code [4];

#ifdef EEP
 volatile unsigned EEMEM char ID [4];
 unsigned EEMEM long TFTID [4];
#endif
   
   volatile uint8_t uart_byte_count = 0;
   bool uart_busybit = 0;
   volatile uint8_t uart_busy = 50;  //  timeout counter for uart
   bool uart_pkt_rdy = 0;
   uint8_t uart_packet[64];
   volatile uint8_t uart_cache[64];
   
	
uint16_t ttl_overflows;


uint8_t readBtns(uint16_t x, uint16_t y)
{	
 uint8_t row = 0, col = 0, colSz = 3;

if(y > 160 && y < 370) col = 1;
else 
if(y > 370 && y < 600) col = 2;
else 
if(y > 600 && y < 800) col = 3;

if(x > 170 && x < 400) row = 1;
else 
if(x > 400 && x < 640) row = 2;
else 
if(x > 640 && x < 820) row = 3;

return (!row && !col) ? 0 : (row * colSz - (colSz - col));
}





void
timer_init (void)
{
	cli();

    TCCR1B |= (1 << CS11);
    // initialize counter
    TCNT1 = 0;
    // enable overflow interrupt
	 // initialize overflow counter variable
    ttl_overflows = 0;

	TCCR2 |= (1 << WGM21) | (1 << CS21);
    
	// initialize counter
    OCR2   =   8;  // Set CTC compare value to 250 kHz
 // enable global interrupts
  sei();
}

/**/


void init_io(void) 
{
	/* first set all pins to input */
    
	
	PORTA = 0xFF;   													/* activate all pull-ups */
    DDRA = 0xFF;    														/* all pins are output */
	PORTB = 0xFF;   													/* activate all pull-ups */
    DDRB = 0x00;       													/* all pins input */
    PORTC = 0xFF;   													/* activate all pull-ups */
    DDRC = 0xFF;   													/* pins are output */
    PORTD = 0xFF;   													/* activate all pull-ups */
    DDRD = 0;    														/* all pins input */
   
	
      ADMUX |= (1 << REFS0);  
       // ADC Enable and prescaler of 128 
       // 16000000/128 = 125000 
      ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 

	      }


void TSinit(void) 
 {
	  if(id == 0x0154 || id == 0x9328 || forceAltTouchPin)  // driver == ID_S6D0154
      {
       YP = (1 << PC3);     
	   XM = (1 << PC4);  
	   XP =  (1 << PA7);   
	   YM = (1 << PA6);  
	   CH0 = 6;     
	   CH1 = 7;  
      }
 }


void uart_init(void) 
{
 // Set baud rate 
    UBRRH = (BAUDRATE >> 8);
    UBRRL = BAUDRATE;
    
    UCSRB |= (1 << RXEN) | (1 << TXEN);      // Enable receiver and transmitter
    UCSRB |= (1 << RXCIE);                   // Enable the receiver interrupt
    UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);    // Set frame: 8data, 1 stp
}




ISR (USART_RXC_vect)
{
	  uart_busy = 50;
	  uart_cache[uart_byte_count++] = UDR;
      
	  if ( uart_byte_count == UART_PKTSIZE )
	 { uart_byte_count = 0;  uart_pkt_rdy = 1;  }  // the last byte of packet received
}



// Read the AD conversion result
 unsigned int read_adc(unsigned char adc_input) 
	  {
		   #ifdef __TST
		uint16_t v = 0, i = 0;
        while(i < 5)
		  {
           #endif
		  cli();
  ADMUX = adc_input | (ADC_VREF_TYPE & 0xff);
  _delay_us(10);            // Delay needed for the stabilization of the ADC input voltage
  ADCSRA |= 0x40;           // Start the AD conversion
  while ((ADCSRA & 0x10) == 0);    // Wait for the AD conversion to complete
  ADCSRA |= 0x10;

         sei();
          #ifdef __TST
         v += ADCW;
         if(++i == 4) { v /= 5; break; }
		  }
   return v;
          #else
   return ADCW;
          #endif
       }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*  */

// reset touchpad
void touchRestore(void)
{
          TOUCHPIN_D |=  XM | YP;
          TOUCHPORT_D |=  XM |  YP;
          TOUCHPIN_A |=  XP | YM;
          TOUCHPORT_A |=  XP | YM;
}


// touchpad polling
void touchRead(void)
{
   //  This sequence is to be fired each ~1ms
   //   ** if TFT shared pins used for polling, any usual display operations 
   //   ** are to be disabled till global 'busy' flag clears
	   push = 0;  
	   static bool c;   // keeping the state 
       static uint8_t stage;

	   switch(stage)
	{
	    case 1:
	     busy = 1; c = 0; coords = 0; 
	       // Port C PC3 input, PC2  opposite
          TOUCHPIN_D = (TOUCHPIN_D & ~YP) | XM;
           // PC2   - X_minus   -  low   YP high
          TOUCHPORT_D = (TOUCHPORT_D & ~XM) | YP;
	      //   X_plus - set as output, Y_minus input 
          TOUCHPIN_A = (TOUCHPIN_A & ~YM) | XP;
          //  pullup on X_plus is off, on Y_minus on
          TOUCHPORT_A = (TOUCHPORT_A & ~XP) | YM;    
	         break;
	       
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case 2:
	  if(read_adc(CH0) < 100)            // 
	  { 
		  c = 1;  push = 1;
      if (++touchcount > 1) {  keypress = 1; } 
		 // Y_plus  -  set as output  (same  as reset)
	     // Y_plus  -  pullup, so the rest   
	     TOUCHPIN_D =  (TOUCHPIN_D & ~XM) | YP;
	     TOUCHPORT_D |= YP | XM;	

         TOUCHPIN_A = (TOUCHPIN_A & ~XP) | YM;
         TOUCHPORT_A = (TOUCHPORT_A & ~YM) | XP;
	  } else {  busy = 0;  touchRestore();  }
              break;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                                               
		case 3:				  
      if(c)  
		  {
	  coords = (uint32_t )read_adc(CH1) << 16;
                         
  	   //  X_minus   -  and reset (PC6)  - set as output   ;     Y_plus   -  input
	  //   X_minus   -   low   ;     Y_plus   -   high
	  TOUCHPIN_D =  (TOUCHPIN_D & ~YP) | XM;
	  TOUCHPORT_D =  (TOUCHPORT_D & ~XM) | YP;

         TOUCHPIN_A = (TOUCHPIN_A & ~YM) | XP;
         TOUCHPORT_A |= XP | YM; 

	      }
           break;
       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                  

		
		case 4:	
      if(c)
	  coords |= read_adc(CH0);
         else {
             c = 0;     
              keypress = 0;
                touchcount = 0; 
                            }
                busy = 0;   touchRestore();
	      break;
	          }  // end switch
 	   if(++stage > TOUCH_INTERVAL) { stage = 0; } // do nothing for next n iterations
	                                                         
 }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



 void numprint( uint32_t num, uint8_t base) {
 char buf[9] = {}; 
 itoa(num, buf + 1, base);   buf[0] = ' ';      
 print(buf);
 }

  void num2arr( unsigned int num,  char *bufr ) {
  itoa(num, bufr, 16);
  }


   void uart_transmit(unsigned char *data)
{
	unsigned char i;
    	for (i = 0; i < (UART_PKTSIZE); i++)
	{  
	while (!( UCSRA & (1 << UDRE)) );            // wait while register is free
    UDR = *data++;                               // load data to the register
    }
}


     void draw_btns(uint8_t y, uint16_t color_0, uint16_t color_1)
      {
      unsigned char i, x = 4;
     if (!y) y = 4;
	for (i = 0; i < 3; i++)     {
	fillRoundRect(x, y, 102, 75, 7, color_1);
	drawRoundRect(x, y, 102, 75, 7, color_0);
	x += 105;
	}
         i = 78;
	  if (y < i * 2) draw_btns( y + i, color_0, color_1 );
	   }


        //     event forwarding (sorta)
void presets_apply(uint16_t p_keyword) 
      {
	  uint8_t i, x; 
 
	  for (i = 0; i < 16; i++)     
		  {
           //check single bit value in the keyword with a bitmask
		   x = (p_keyword >>  i) & 1;   
		                     if(x == 0) continue; else x = i;
		 // then call an appropriate event
		 switch (x)
	{
		case 0:   setCursor(RIGHT, 130); print("8-bit");   break;
		case 1:   setColor(WHITE); 
                  _setFont(OCR_A_Extended_M);
			      setCursor( 35, 195); print("UTFT library");   break;
		case 2:   fillScreen(BLACK);  break;
		case 3:    /* take action according to preset #3 ... */  break;
		case 4: 
		    setAddrWindow(0, 0, 240, 320);
            flood(WHITE, 9000);
		break; 
		case 5:
		     fillRect(10, 10, 100, 100, BLUE); break;
		case 6: break;
		case 7: break;
		case 8: break;
		case 9: break; 
		case 10: break;
		case 11: break;
		case 12: break;
		case 13: break; 
		case 14: break;
		case 15: break;
	}
		  }  
		        } 


 /* */
int main(void)
	{
       #ifdef UARTSPITID
	char buf[16] = {};
	   #endif
   uint16_t fivesec = 0;
   init_io();
   timer_init();
   uart_init();
    
	 //TFT:
    reset();
	   #ifndef TSTOUCH
	 tft_init();
     begin();
	   #endif
     TSinit();
     setRotation(3); //
    fillScreen(BLACK); fillScreen(WHITE);
    draw_btns(0, VGA_BLACK, VGA_SILVER);
	  presets_apply(1); 

		   #ifdef DRAWTRISQUARE 
		fillRoundRect(10, 10, 120, 100, 7, VGA_LIME);
 /**/	fillTriangle(127, 0, 240, 240, 0, 240, BLUE);  
           #endif

	  #ifdef ADAFONTS
           setTextColor(WHITE, WHITE);
           setFont(&Orbitron_Light_16);
	       setTextSize(1);
	       setCursor(10, 30);
	       write('I'); write('D');
           setCursor(30, 30);
      #endif

  
     setColor(WHITE);
     setBackColor(VGA_TRANSPARENT);
     _setFont(OCR_A_Extended_M);
      setCursor(15, 15);
	 print("ID");   //
      setCursor(35, 15);
     numprint(readID(), 16); //
     setCursor(80, 30); 
	
	 #ifdef ADAFONTS
	 setTextColor(BLACK, WHITE);
	 setCursor(160, 30);   numprint( _height, 10); write(' '); write('X'); numprint( _width, 10);  
     #endif
    
	presets_apply(1); //    3      65535

    fillRoundRect(100, 80, 190, 30, 7, RED); 
	drawRoundRect(100, 80, 190, 30, 7, BLACK);

   setColor(BLACK);
   setBackColor(RED);

//*****************************************************
//*****************************************************
/**/
while(1)
{           
          // main tick routine
	if(TIFR & (1 << OCF2)) {  TIFR = (1 << OCF2);   // 250 kHz
	if( ++fract_count == 25 ) // 0.1 ms / 10 kHz   25
	    {
	   fract_count = 0;  ns_cnt++; if( ms_cnt++ > 10 ) { ms_cnt = 0; s_cnt++;  // 1ms Autoreset
		   touchRead(); 
		 }
	if( ns_cnt > 100 ) { ns_cnt = 0; 
	if( s_cnt > 100 ) { s_cnt = 0;  /* if(!busy) */  {  
		
				setCursor(110, 85); 

		#ifdef DISPLAYPOS
	
		numprint( coords >> 16, 10 ); 
		numprint( coords & 0xFFFF, 10 );
		print("     ");
 
		#else
               print("btn : ");
        numprint (readBtns(coords >> 16, coords & 0xFFFF), 10);
		print("   ");

		// numprint( fivesec, 10 );
		#endif
	 }       // numprint( coords  );   //  coords = 0x12345678; 
		       if( ++fivesec > 1 ) { fivesec = 0; // 50 
					#ifdef TSTOUCH	   
				 if(busy) continue;
					itoa(coords >> 16, buf, 10);
				 itoa(coords & 0xFFFF, buf + 4, 10); 
					 buf[4] = ' '; buf[7] = buf[6] = 0x0d; 
					uart_transmit((unsigned char *)buf);
					uart_transmit((unsigned char *)"\r\n");
					memset(buf, 0, sizeof buf);
			   
			   		#endif

			   #ifdef UARTSPITID  //transmit TFT ID over UART
			   itoa(readID(), buf, 16); uart_transmit((unsigned char *)buf);  
			   itoa(readReg(0x00), buf, 16); uart_transmit((unsigned char *)buf);  
			   #endif
			   }
			   #ifdef UARTSPITID 
			   if (uart_busy-- == 0) uart_byte_count = 0;  //reset
			   if ( uart_pkt_rdy == 1)  { uart_pkt_rdy = 0; // new uart packet was just received
		      }
         	   #endif
	      } 
	   } 
	}
     }
           /*  secondary tick   */
		
	 if (TCNT2 >= 250) 
		       {
		 
   /* do some work here mb */  
		 
                       }
                }
         }
/**/
