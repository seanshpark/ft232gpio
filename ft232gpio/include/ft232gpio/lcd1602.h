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

#ifndef __FT232GPIO_LCD1602_H__
#define __FT232GPIO_LCD1602_H__

#include "lcd1602_def.h"
#include "i2c.h"

namespace ft232gpio
{

class LCD1602
{
public:
  LCD1602() = default;
  virtual ~LCD1602() = default;

public:
  bool init(I2C *i2c);
  void release(void);

public:
  bool initialized(void) { return _initalized; }

public:
  void clear();
  void home();
  void display(bool enable); // display on/off
  void cursor(bool enable);  // cursor on/off
  void blink(bool enable);   // blinking on/off
  void puts(const char *str);
  void putch(uint8_t ch);
  void move(uint8_t row, uint8_t col);
  void cgram(uint8_t ch, uint8_t *data, uint32_t leng);

private:
  void function_set(uint8_t data);
  void cursor_set(uint8_t data);
  void display_set(void);
  void entrymode_set(uint8_t data);
  void putc(const char c);

private:
  void init_4bit(void);
  void send_4bits(uint8_t lcddata, uint32_t delay);
  void send_data(uint8_t data);
  void send_ctrl(uint8_t data);

private:
  I2C *_i2c = nullptr;
  bool _initalized = false;

  bool _back_light = false;
  bool _display = false;
  bool _cursor = false;
  bool _blink = false;
};

} // namespace ft232gpio

#endif // __FT232GPIO_LCD1602_H__
