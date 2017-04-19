/****************************************************************************
  * Файл:           adc_mega328.cpp
  * Автор:          Никита О.
  * Дата:           17.04.2017
  * Организация:    PhSound
  * Наш сайт:       PhSound.ru
  * e-mail:         PhSound@mail.ru
****************************************************************************/

#include "function.h"

uint16_t adc_ch, ref_ch;
/***************************************************************************/
/* Прочитать измеренное значение АЦП с одного из 3-х каналов: 
   опорное напряжение, аудио 1, аудио 2                                    */
/***************************************************************************/
uint8_t adc_read_value()
{
  static uint8_t channel_adc;
  if(ADCSRA & (1<<ADIF))
  {
    switch(channel_adc)
    {
      case ADC_REF_CASE:
        ref_ch = ADCL;
        ref_ch |= (ADCH<<8);
        ADMUX  |=  ADC_CH1_CASE;
        ADCSRA |= (1<<ADSC);
        channel_adc = ADC_CH1_CASE;
        return ADC_REF_CASE;
        
      case ADC_CH1_CASE:
        adc_ch = ADCL;
        adc_ch |= (ADCH<<8);
        ADMUX  &= ~ADC_CH1_CASE;
        ADMUX  |=  ADC_CH2_CASE;
        ADCSRA |= (1<<ADSC);
        channel_adc = ADC_CH2_CASE;
        return ADC_CH1_CASE;
        
      case ADC_CH2_CASE:
        adc_ch = ADCL;
        adc_ch |= (ADCH<<8);
        ADMUX  &= ~ADC_CH2_CASE;
        ADCSRA |= (1<<ADSC);
        channel_adc = ADC_REF_CASE;
        return ADC_CH2_CASE;
        
      default:
        ADMUX  &= ~(ADC_CH1_CASE|ADC_CH2_CASE);
        ADCSRA |= (1<<ADSC);
        channel_adc = ADC_REF_CASE;
    }
  }
  return ADC_REF_CASE;
}