#ifndef __REGVALS_H
#define __REGVALS_H

#ifdef ILI_9325D
extern const uint16_t ILI932x_regValues[] PROGMEM;
#endif

#if defined ILI_9325C && !defined ILI_9325D
extern const uint16_t ILI932x_regValues[] PROGMEM;
#endif

#if !defined ILI_9325D && !defined ILI_9325C
extern const uint16_t ILI932x_regValues[] PROGMEM;
#endif

extern const uint8_t  ILI9341_regValues[] PROGMEM;

#ifdef ILI_AND_MORE
extern  const uint8_t  HX8347G_regValues[] PROGMEM;
extern  const uint8_t  HX8357D_regValues[] PROGMEM;
extern  const uint8_t  ILI9327_regValues[] PROGMEM;
extern  const uint8_t  ILI9488_regValues[] PROGMEM;
extern  const uint16_t S6D0154_regValues[] PROGMEM;
#endif

void begin(void);

#endif

