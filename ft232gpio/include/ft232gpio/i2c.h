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

#ifndef __FT232GPIO_I2C_H__
#define __FT232GPIO_I2C_H__

#include "ft232.h"

namespace ft232gpio
{

class I2C
{
public:
  I2C();
  virtual ~I2C();

public:
  bool init(FT232 *ft232, uint8_t addr);
  void release(void);

  void start_cond(void);
  void stop_cond(void);
  void write_bit(bool bit);
  bool read_bit(void);
  bool write_byte(bool send_start, bool send_stop, uint8_t data);
  uint8_t read_byte(bool nack, bool send_stop);

  bool is_lost(void) { return _lost; }

private:
  void _set_sda(void);
  void _set_scl(void);
  void _clear_sda(void);
  void _clear_scl(void);
  uint8_t _read_scl(void);
  uint8_t _read_sda(void);
  void _delay(void);
  void _arbitration_lost(void);
  void _wait_scl(void);
  void _dummy_clock(void);

private:
  FT232 *_ft232 = nullptr;
  uint8_t _addr = 0x00;
  bool _initalized = false;
  uint8_t _ft232_data = 0x00;

  bool _started = false;
  bool _lost = false;
};

} // namespace ft232gpio

#endif // __FT232GPIO_I2C_H__
