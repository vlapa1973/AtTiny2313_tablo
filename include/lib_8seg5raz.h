#ifndef _LIB_8SEG5RAZ_H_
#define _LIB_8SEG5RAZ_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define BRIGHTNESS 0xFE         //  полная яркость свечения

//================================================================
//	-------------------- ПИНы для сегментов
#define seg_A PB0
#define seg_A_PORT PORTB
#define seg_A_DDR DDRB

#define seg_B PB1
#define seg_B_PORT PORTB
#define seg_B_DDR DDRB

#define seg_C PB2
#define seg_C_PORT PORTB
#define seg_C_DDR DDRB

#define seg_D PB3
#define seg_D_PORT PORTB
#define seg_D_DDR DDRB

#define seg_E PB4
#define seg_E_PORT PORTB
#define seg_E_DDR DDRB

#define seg_F PB5
#define seg_F_PORT PORTB
#define seg_F_DDR DDRB

#define seg_G PB6
#define seg_G_PORT PORTB
#define seg_G_DDR DDRB

#define seg_dot PB7
#define seg_dot_PORT PORTB
#define seg_dot_DDR DDRB

//	-------------------- ПИНы для разрядов
#define raz_0 PD2
#define raz_0_PORT PORTD
#define raz_0_DDR DDRD

#define raz_1 PD3
#define raz_1_PORT PORTD
#define raz_1_DDR DDRD

#define raz_2 PD4
#define raz_2_PORT PORTD
#define raz_2_DDR DDRD

#define raz_3 PD5
#define raz_3_PORT PORTD
#define raz_3_DDR DDRD

#define raz_4 PD6
#define raz_4_PORT PORTD
#define raz_4_DDR DDRD

//================================================================
uint8_t dig[19] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b00000000, // 10 - A - пусто
    0b01000000, // 11 - B - минус
    0b01100011, // 12 - C - градус
    0b01011100, // 13 - o - нижний градус
    0b00001000, // 14 - D - нижнее подчеркивание
    0b10000000, // 15 - . - точка
    0b01111000, // 16 - t
    0b01110011, // 17 - p
    0b01110110, // 18 - h
};

//================================================================
void seg8raz5_ini(void)
{
    seg_A_DDR |= (1 << seg_A);
    seg_B_DDR |= (1 << seg_B);
    seg_C_DDR |= (1 << seg_C);
    seg_D_DDR |= (1 << seg_D);
    seg_E_DDR |= (1 << seg_E);
    seg_F_DDR |= (1 << seg_F);
    seg_G_DDR |= (1 << seg_G);
    seg_dot_DDR |= (1 << seg_dot);

    raz_0_DDR |= (1 << raz_0);
    raz_1_DDR |= (1 << raz_1);
    raz_2_DDR |= (1 << raz_2);
    raz_3_DDR |= (1 << raz_3);
    raz_4_DDR |= (1 << raz_4);
}

void seg8raz5_work(uint8_t dTem, uint8_t lV, uint8_t fP)
{
    if (dTem > 0b10000000 && dTem < 0b10010100)
  {
    fP = 1;
    dTem -= 0b10000000;
  }
  if (lV == 0)
  {
    PORTB = ~dig[dTem];
    if (fP)
      PORTB &= ~(1 << PB7);
  }
  else
  {
    PORTB = dig[dTem];
    if (fP)
      PORTB |= (1 << PB7);
  }
  fP = 0;
}

//================================================================

#endif