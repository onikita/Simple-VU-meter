/****************************************************************************
  * Файл:           calculate.cpp
  * Автор:          Никита О.
  * Дата:           17.04.2017
  * Организация:    PhSound
  * Наш сайт:       PhSound.ru
  * e-mail:         PhSound@mail.ru
****************************************************************************/

#include "function.h"

/***************************************************************************/
/* Вычисление квадратного корня для значений до 2 байт включительно        */
/***************************************************************************/
uint16_t sqrt_u16(uint16_t x)
{
  uint16_t m, y, b;
  m = 0x4000;
  y = 0;
  if(x!=0)
  {
    while (m!=0)
    {
      b = y|m;
      y = y>>1;
      if(x>=b) 
      {
        x = x-b;
        y = y|m;
      }
      m = m>>2;
    }
  }
  return y;
}
/***************************************************************************/
/* Вычисление квадратного корня для значений до 4 байт включительно        */
/***************************************************************************/
uint16_t sqrt_u32(uint32_t arg)
{
  uint8_t count;
  uint32_t res, tmp;
  count=16;
  res=0;
  tmp=0;
  if(arg!=0)
  { 
    if(!(arg&0xFF000000)) { arg<<=8;count-=4; }
    res=1;
    while((tmp<1)&&(count))
    {
      count--;
      if(arg&0x80000000UL)tmp|=2;
      if(arg&0x40000000UL)tmp|=1;
      arg<<=2;
    };//поиск первой 1-ы
    tmp--;
    for(;count;count--)
    {
      tmp<<=2;
      res<<=1;
      if(arg&0x80000000UL)tmp|=2;
      if(arg&0x40000000UL)tmp|=1;
      arg<<=2;
      if( tmp>=((res<<1)|1))
      {
        tmp-=((res<<1)|1);
        res|=1;
      }   
    }
  }
  return (uint16_t)res;
}
/***************************************************************************/

uint32_t  filter_ch1[FILTER_MEAS_NUMBER], // массив квадратов измеренных значений для канала 1
          filter_ch2[FILTER_MEAS_NUMBER]; // массив квадратов измеренных значений для канала 2
            
bool      filter_full_ch1 = false, // флаг заполнения массива для канала 1
          filter_full_ch2 = false; // флаг заполнения массива для канала 2
            
uint8_t   index_ch1 = 0, // индекс заполнения массива для канала 1
          index_ch2 = 0; // индекс заполнения массива для канала 2
            
uint32_t  sum_ch1 = 0, // сумма всех значений массива для канала 1
          sum_ch2 = 0; // сумма всех значений массива для канала 2

/***************************************************************************/
/* Обработка измеренных значений каналов аудиосигнала и накопление         */
/***************************************************************************/
void calculate_value(uint8_t channel_No)
{
  uint32_t  x;
  
  // избавление от постоянной составляющей:
  if(adc_ch > ref_ch) adc_ch = adc_ch-ref_ch;
  else                adc_ch = ref_ch-adc_ch;

  // вычисление квадрата измеренного значения:
  x = adc_ch*adc_ch;
  
  // для канала 1
  // добавление квадрата измеренного значения в массив фильтрации и
  // вычисление суммы всех значений массива с учётом нового значения:
  if(channel_No == ADC_CH1_CASE)
  { // если массив заполнен, то замещаем самое старое значение в сумме
    // иначе просто добавляем новое значение в сумму
    if(filter_full_ch1 == true) sum_ch1 += x - filter_ch1[index_ch1];
    else                        sum_ch1 += x;
    filter_ch1[index_ch1] = x;
    index_ch1++;
    if(index_ch1 >= FILTER_MEAS_NUMBER){ index_ch1=0, filter_full_ch1=true; }
  }
  // аналогично для канала 2
  else if(channel_No == ADC_CH2_CASE)
  { 
    if(filter_full_ch2 == true) sum_ch2 += x - filter_ch2[index_ch2];
    else                        sum_ch2 += x;
    filter_ch2[index_ch2] = x;
    index_ch2++;
    if(index_ch2 >= FILTER_MEAS_NUMBER){ index_ch2=0, filter_full_ch2=true; }   
  }
}
/***************************************************************************/ 
/* Обработка накопленных значений массива (вычисление среднеквадратичного
   значения). Получаем количество светодиодов, которое нужно зажечь
   Передача значений происходит через входные параметры (указатели)        */
/***************************************************************************/
void processing_values(uint16_t* calc_value1, uint16_t* calc_value2)
{
  uint32_t  filter_value_ch1,
            filter_value_ch2;

  if(filter_full_ch1 == true) filter_value_ch1 = sum_ch1/FILTER_MEAS_NUMBER;
  else                        filter_value_ch1 = sum_ch1/(index_ch1+1);
  if(filter_full_ch2 == true) filter_value_ch2 = sum_ch2/FILTER_MEAS_NUMBER;
  else                        filter_value_ch2 = sum_ch2/(index_ch2+1);
  
  if(filter_value_ch1 > 0xFFFF) filter_value_ch1 = sqrt_u32(filter_value_ch1);
  else                          filter_value_ch1 = sqrt_u16(filter_value_ch1);
  if(filter_value_ch2 > 0xFFFF) filter_value_ch2 = sqrt_u32(filter_value_ch2);
  else                          filter_value_ch2 = sqrt_u16(filter_value_ch2);
  
  *calc_value1 = filter_value_ch1;
  *calc_value2 = filter_value_ch2;  
}