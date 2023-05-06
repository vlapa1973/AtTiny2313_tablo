/*
    Табло 8 сегм 5 разр
    20221123 - 20230226
    = vlapa = v.008

    tiny2313, 8MHz, Общий Анод
    A-PB0, B-PB1, C-PB2, D-PB3, E-PB4, F-PB5, G-PB6, dp-PB7
    0-PD2, 1-PD3, 2-PD4, 3-PD5, 4-PD6
*/

#define F_CPU 8000000UL
#define BAUD 9600UL

#include "lib_8seg5raz.h"

uint8_t data[6] = {0, 0, 0, 0, 0};
uint8_t temp[7] = {10, 10, 10, 10, 10, 10};
uint8_t count = 0;
uint8_t numData = 0;
uint8_t s = 0;

uint8_t flagPoint = 0;     //  флаг точки
uint8_t brightness = 0xFE; //  полная яркость

const uint8_t level = 1; //  управление высоким (1) уровнем, или (0) - низким

//================================================================
void usart_ini()
{
  uint16_t ubrr = F_CPU / 16 / BAUD - 1;
  UBRRH = (uint8_t)(ubrr >> 8);         // старшие биты UBRR
  UBRRL = (uint8_t)ubrr;                // младшие биты UBRR
  UCSRA |= (1 << RXC);                  // | (1 << TXC);
  UCSRB |= (1 << RXCIE) | (1 << RXEN);  // | (1 << TXCIE) | (1 << TXEN);
  UCSRC |= (1 << UCSZ1) | (1 << UCSZ0); // асинхронный режим,
                                        // размер посылки 8 бит, проверка чётности отключена, 1 стоп-бит
}

//================================================================
void timer0_ini()
{
  TCCR0A = 0;                          //(1 << WGM11); // (1 << WGM10) | (1 << COM1A1) | (1 << COM1A0); // OC1A Отключен
  TCCR0B |= (1 << CS00) | (1 << CS01); // (1 << WGM12) |
  OCR0A = BRIGHTNESS;
  TIMSK |= (1 << TOIE0) | (1 << OCIE0A);
}

//================================================================
// void digitWork(uint8_t dTem)
// {
//   if (dTem > 0b10000000 && dTem < 0b10010100)
//   {
//     flagPoint = 1;
//     dTem -= 0b10000000;
//   }
//   if (level == 0)
//   {
//     PORTB = ~dig[dTem];
//     if (flagPoint)
//       PORTB &= ~(1 << PB7);
//   }
//   else
//   {
//     PORTB = dig[dTem];
//     if (flagPoint)
//       PORTB |= (1 << PB7);
//   }
//   flagPoint = 0;
// }
//===========================================================
// char USART_receiveChar(void)
// {
//   return ((UCSRA >> RXC) & 1) ? UDR : 0;
//   // возвращаем значение буфера приёма
// }
//================================================================
// Отправка ASCII символа (байта)
// void USART_sendChar(unsigned char character)
// {
//   while (!((UCSRA >> UDRE) & 1))
//   {
//   };
//   // ожидаем очистки буфера передачи
//   UDR = character; // помещаем данные в буфер для отправки
// }

//================================================================
ISR(TIMER0_COMPA_vect)
{
  if (level == 0)
  {
    PORTB = 0xFF;
  }
  else
  {
    PORTB = 0x00;
  }
}

//================================================================
ISR(TIMER0_OVF_vect)
{
  OCR0A = brightness;
  static unsigned char r = 0b01000000;
  PORTB = 0x00;
  PORTD = 0x00;

  switch (s)
  {
  case 0:
    seg8raz5_work(data[0], level, flagPoint);
    break;

  case 1:
    seg8raz5_work(data[1], level, flagPoint);
    // if (!level)
    // {
    //   (data[2] == 0x0D) ? PORTB |= (1 << 0) : PORTB &= ~(1 << 0);
    // }
    // else
    // {
    //   (data[2] == 0x0D) ? PORTB &= ~(1 << 0) : PORTB |= (1 << 0);
    // }
    break;

  case 2:
    seg8raz5_work(data[2], level, flagPoint);
    break;

  case 3:
    seg8raz5_work(data[3], level, flagPoint);
    break;
  case 4:
    seg8raz5_work(data[4], level, flagPoint);
    break;
  }
  PORTD = r;
  r = r == 0b00000100 ? 0b01000000 : r >> 1;
  (s == 4) ? s = 0 : s++; // крутим активный разряд
}

//==========================
ISR(USART_RX_vect)
{
  uint8_t b = UDR;

  if (b == 0x0D || b == 0x0A)
  {
    numData = 0;
  }
  else
  {
    if (b >= 0x4E && b <= 0xFE) //  яркость
    {
      brightness = b;
    }
    else
    {
      if (b == 0x2E)  //  точка
      {
        data[numData - 1] += 0b10000000;
        numData--;
      }
      else if (b > 0x2F && b < 0x3A)  //  цифры
      {
        data[numData] = b - 0x30;
        if (flagPoint)
        {
          b += 0b10000000;
          flagPoint = 0;
        }
      }
      else if (b > 0x40 && b < 0x4A)  //  A, B, C, D, E, F, G, H, I - спецсимволы
      {
        data[numData] = b - 0x37;
        if (flagPoint)
        {
          b += 0b10000000;
          flagPoint = 0;
        }
      }
      else if (b > 0x49)
      {
        data[numData] = 11;
        if (flagPoint)
        {
          b += 0b10000000;
          flagPoint = 0;
        }
      }

      numData >= 5 ? numData = 0 : numData++;
    }
  }
}

//===============================================================
int main(void)
{
  seg8raz5_ini();
  timer0_ini();
  usart_ini();

  // data[0] = 2 + 128;
  // data[1] = 5;
  // data[2] = 11 + 128;
  // data[3] = 5;
  // data[4] = 2;

  // data[0] = 8;
  // data[1] = 15;
  // data[2] = 8;
  // data[3] = 8;
  // data[4] = 8;

  sei();

  while (1)
  {
    // data[4] += 1;
    // if (data[4] >= 19)
    // {
    //   data[4] = 0;
    //   data[3] += 1;
    //   if (data[3] >= 19)
    //   {
    //     data[3] = 0;
    //     data[2] += 1;
    //     if (data[2] >= 19)
    //     {
    //       data[2] = 0;
    //       data[1] += 1;
    //       if (data[1] >= 19)
    //       {
    //         data[1] = 0;
    //         data[0] += 1;
    //         if (data[0] >= 19)
    //         {
    //           for (unsigned char i = 0; i < 5; ++i)
    //           {
    //             data[i] = 0;
    //           }
    //         }
    //       }
    //     }
    //   }
    // }

    // if (count > 6 + 4)
    // {
    //   temp[5] = data[0];
    //   data[0] = data[1];
    //   data[1] = data[2];
    //   data[2] = data[3];
    //   data[3] = data[4];
    //   data[4] = temp[0];

    //   temp[0] = temp[1];
    //   temp[1] = temp[2];
    //   temp[2] = temp[3];
    //   temp[3] = temp[4];
    //   temp[4] = temp[5];
    // }

    // _delay_ms(200);
    // (count <= 6 + 4) ? count++ : (count > 15 + 4) ? count = 0
    //                                               : count++;

    // if (UCSRA & 1 << RXC)
    //   a = UDR;
    // if (UCSRA & 1 << UDRE)
    //   UDR = a;
    // _delay_ms(1); // Опционально
  }
}