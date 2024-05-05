/*
 * Copyright 2024 saehie.park@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ft232gpio/lcd1602.h"

#include <iostream>
#include <cassert>

#include <unistd.h> // usleep

namespace ft232gpio
{

bool LCD1602::init(I2C *i2c)
{
  std::cout << "LCD1602::init" << std::endl;

  _i2c = i2c;

  _display = true;
  _cursor = true;
  _blink = true;

  // turn on back-light
  _back_light = true;

  init_4bit();
  usleep(200);

  function_set(HD44780_LCD_FUNCSET_4BIT | HD44780_LCD_FUNCSET_2LINES | HD44780_LCD_FUNCSET_5x8);
  usleep(200);

  cursor_set(HD44780_LCD_CURSOR_SHIFT_CUR | HD44780_LCD_CURSOR_RIGHT);
  usleep(200);

  display_set();
  usleep(200);

  entrymode_set(HD44780_LCD_ENTRY_INC);
  usleep(200);

  _initalized = true;

  clear();
  usleep(100);

  return true;
}

void LCD1602::release(void)
{
  _back_light = false;
  _display = false;
  _cursor = false;
  _blink = false;
  display_set();
  clear();

  _i2c = nullptr;
  _initalized = false;
}

void LCD1602::clear()
{
  uint8_t cmd = HD44780_LCD_CMD_CLEAR;
  send_ctrl(cmd);
  usleep(5000); // wait 5ms
}

void LCD1602::home()
{
  uint8_t cmd = HD44780_LCD_CMD_RETHOME;
  send_ctrl(cmd);
  usleep(1600); // spec says 1.52ms
}

void LCD1602::display(bool enable)
{
  _display = enable;
  display_set();
  usleep(50);
}

void LCD1602::cursor(bool enable)
{
  _cursor = enable;
  display_set();
  usleep(50);
}

void LCD1602::blink(bool enable)
{
  _blink = enable;
  display_set();
  usleep(50);
}

void LCD1602::putc(const char c)
{
  send_data(c);
  usleep(50);
}

void LCD1602::puts(const char *str)
{
  while (*str != '\x0')
  {
    putc(*str++);
  }
}

void LCD1602::putch(uint8_t ch)
{
  send_data(ch);
  usleep(50);
}

void LCD1602::move(uint8_t row, uint8_t col)
{
  uint8_t cmd = HD44780_LCD_CMD_DDRAMADDR;
  uint8_t ram_offset;

  // row 0 : 0x00 ~ 0x27
  // row 1 : 0x40 ~ 0x67
  if (row > 0x01)
    row = 0x01; // use provide 2 lines
  if (col > 0x27)
    col = 0x27;

  ram_offset = row * 0x40 + col;
  ram_offset &= 0b01111111;

  send_ctrl(cmd + ram_offset);
  usleep(50);
}

void LCD1602::cgram(uint8_t ch, uint8_t *data, uint32_t leng)
{
  uint8_t cmd = HD44780_LCD_CMD_CGRAMADDR;
  // NOTE CGRAM address is mapped as 8 bytes per character
  // << 3 (== *8) to jump to address of ch
  cmd |= (ch << 3) & 0x3f;
  send_ctrl(cmd);
  usleep(50);

  for (size_t p = 0; p < leng; ++p)
  {
    send_data(data[p]);
    usleep(50);
  }
}

void LCD1602::init_4bit(void)
{
  uint8_t lcddata;
  uint8_t data;

  // send RS=0, RW=0, DB7~DB4=0011 as 8bit 3 times
  data = HD44780_LCD_CMD_FUNCSET | HD44780_LCD_FUNCSET_8BIT;

  lcddata = data & 0xf0;
  send_4bits(lcddata, 4500);

  lcddata = data & 0xf0;
  send_4bits(lcddata, 150);

  lcddata = data & 0xf0;
  send_4bits(lcddata, 150);

  // send RS=0, RW=0, DB7~DB4=0010 as 4bit 1 time
  data = HD44780_LCD_CMD_FUNCSET;
  lcddata = data & 0xf0;
  send_4bits(lcddata, 150);
}

void LCD1602::send_4bits(uint8_t lcddata, uint32_t delay)
{
  lcddata &= ~PCF8574_LCD1604_BL;

  // this sends 4bits to DB4~DB7 of HD44780
  // lower 4bits are used for control.
  // to write, send bits + EN high, wait 2usec, drop EN low, wait delay
  // data will be written falling edge
  lcddata |= PCF8574_LCD1604_EN;
  lcddata |= _back_light ? PCF8574_LCD1604_BL : 0;
  _i2c->write_byte(true, false, lcddata);
  usleep(2);

  lcddata &= ~PCF8574_LCD1604_EN;
  lcddata |= _back_light ? PCF8574_LCD1604_BL : 0;
  _i2c->write_byte(false, true, lcddata);
  usleep(delay);
}

void LCD1602::send_data(uint8_t data)
{
  uint8_t lcddata;

  // RS high is to select DATA
  // bit 7~4
  lcddata = (data & 0xf0) | PCF8574_LCD1604_RS;
  send_4bits(lcddata, 10);

  // bit 3~0
  lcddata = ((data & 0x0f) << 4) | PCF8574_LCD1604_RS;
  send_4bits(lcddata, 10);
}

void LCD1602::send_ctrl(uint8_t data)
{
  uint8_t lcddata;

  // TODO support send data with 8bits

  // RS low is to select CONTROL
  // bit 7~4
  lcddata = (data & 0xf0);
  send_4bits(lcddata, 10);

  // bit 3~0
  lcddata = ((data & 0x0f) << 4);
  send_4bits(lcddata, 10);
}

void LCD1602::function_set(uint8_t data)
{
  uint8_t cmd = HD44780_LCD_CMD_FUNCSET;
  cmd |= data;
  send_ctrl(cmd);
}

void LCD1602::cursor_set(uint8_t data)
{
  uint8_t cmd = HD44780_LCD_CMD_CURSOR;
  cmd |= data;
  send_ctrl(cmd);
}

void LCD1602::display_set(void)
{
  uint8_t cmd = HD44780_LCD_CMD_DISPLAY;
  cmd |= _display ? HD44780_LCD_DISPLAY_ON : 0;
  cmd |= _cursor ? HD44780_LCD_DISPLAY_CUR : 0;
  cmd |= _blink ? HD44780_LCD_DISPLAY_BLINK : 0;
  send_ctrl(cmd);
}

void LCD1602::entrymode_set(uint8_t data)
{
  uint8_t cmd = HD44780_LCD_CMD_ENTRY;
  cmd |= data;
  send_ctrl(cmd);
}

} // namespace ft232gpio
