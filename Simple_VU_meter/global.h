/****************************************************************************
  * Файл:           global.h
  * Автор:          Никита О.
  * Дата:           17.04.2017
  * Организация:    PhSound
  * Наш сайт:       PhSound.ru
  * e-mail:         PhSound@mail.ru
****************************************************************************/

#pragma once

// Чтобы компилятор не выдавал предупреждение:
// warning: only initialized variables can be placed into program memory area
#ifdef PROGMEM
  #undef PROGMEM
  #define PROGMEM __attribute__((section(".progmem.data")))
#endif

//#define DEBUG_UART
#define  QUARZ                        (16000000UL)
#define  OPERATION_TIME               (1E9L/QUARZ+1) // 63ns

#define  __disable_interrupt()     __asm__ __volatile__ ("cli")//(SREG &= ~0x80)
#define  __enable_interrupt()      __asm__ __volatile__ ("sei")//(SREG |=  0x80)

#define  BIT(_BIT)                (1 << (_BIT))
#define  SETBIT(_ADDRESS,_BIT)    (_ADDRESS) |=  BIT(_BIT)
#define  CLRBIT(_ADDRESS,_BIT)    (_ADDRESS) &= ~BIT(_BIT)
#define  CHECKBIT(_ADDRESS,_BIT)  ( ((_ADDRESS) & BIT(_BIT)) )

#define  setbit    SETBIT
#define  clrbit    CLRBIT
#define  checkbit  CHECKBIT

#define  io_port(_p,_b)  (PORT##_p)
#define  io_pin(_p,_b)   (PIN##_p)
#define  io_ddr(_p,_b)   (DDR##_p)
#define  io_bit(_p,_b)   (_b)

#define  io_out(port_bit)    setbit(io_ddr(port_bit),   io_bit(port_bit))
#define  io_set(port_bit)    setbit(io_port(port_bit),  io_bit(port_bit))
#define  io_clr(port_bit)    clrbit(io_port(port_bit),  io_bit(port_bit))

#define  io_inp(port_bit)    clrbit(io_ddr(port_bit),   io_bit(port_bit))
#define  io_inp_up(port_bit) clrbit(io_ddr(port_bit),   io_bit(port_bit)); setbit(io_port(port_bit), io_bit(port_bit))
#define  io_hiz(port_bit)    clrbit(io_ddr(port_bit),   io_bit(port_bit)); clrbit(io_port(port_bit), io_bit(port_bit))
#define  io_check(port_bit)  checkbit(io_pin(port_bit), io_bit(port_bit))

extern uint16_t adc_ch, ref_ch;

enum ADC_channels
{
  ADC_REF_CASE = 0x00,
  ADC_CH1_CASE = 0x01,
  ADC_CH2_CASE = 0x02,
};

#define BASE_COLOR_NUMBER            3                                  // кол-во базовых цветов у светодиода
#define LEDS_NUMBER                  (LEDS_CH_NUMBER*CHANNEL_NUMBER)    // кол-во светодиодов
#define ELEMENTS_CH_NUMBER           (BASE_COLOR_NUMBER*LEDS_CH_NUMBER) // кол-во байт цветов на канал
#define ELEMENTS_NUMBER              (BASE_COLOR_NUMBER*LEDS_NUMBER)    // кол-во байт цветов

