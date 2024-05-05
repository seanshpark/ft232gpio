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

#include "ft232gpio/tm1637.h"

#include <iostream>
#include <bitset>
#include <cassert>

#include <unistd.h> // usleep

// NOTE TM1637 CLK/DIO is like I2C but quite different

#define PIN_CLOCK 0x08 // CTS of FT232
#define PIN_DIO 0x10   // DTR of FT232

static const uint32_t CLOCK_DELAY = 20;
static const uint32_t DATA_DELAY = 20;

namespace ft232gpio
{

bool TM1637::init(FT232 *ft232)
{
  _ft232 = ft232;
  _initalized = true;

  // initialize chip
  uint8_t command;
  command = TM1637_CMD_DATA | TM1637_DATA_AUTOINC | TM1637_DATA_NORMAL;
  write(command);

  // clear display
  clear();

  // set default bright to 1
  bright(1);

  return true;
}

void TM1637::release(void)
{
  if (_ft232 == nullptr)
  {
    assert(false);
    return;
  }

  bright(0);

  _initalized = false;

  _ft232 = nullptr;
}

void TM1637::write(uint8_t data)
{
  if (not _initalized)
  {
    assert(false);
    return;
  }

  std::cout << "tm1637 write " << std::bitset<8>(data) << std::endl;

  dio_start();

  write_byte(data);
  skip_ack();

  dio_stop();

  usleep(1000);
}

void TM1637::writes(uint8_t *data, int32_t length)
{
  if (not _initalized)
  {
    assert(false);
    return;
  }

  dio_start();

  for (int b = 0; b < length; b++)
  {
    write_byte(data[b]);
    skip_ack();
  }

  dio_stop();
}

// value 0 for display off
// value 1 ~ 7 for display on and brighness value
void TM1637::bright(uint8_t value)
{
  if (not _initalized)
  {
    assert(false);
    return;
  }

  uint8_t command;

  if (value == 0) // display off
    command = TM1637_CMD_DISPLAY | TM1637_DISPLAY_OFF;
  else
  {
    value = value > 8 ? 8 : value;
    command = TM1637_CMD_DISPLAY | TM1637_DISPLAY_ON;
    command |= (value - 1);
    // command is pulse width for brighness
    // 0 :  1/16
    // 1 :  2/16
    // 2 :  4/16
    // 3 : 10/16
    // 4 : 11/16
    // 5 : 12/16
    // 6 : 13/16
    // 7 : 14/16
  }

  write(command);
}

void TM1637::clear(void)
{
  int32_t dst;
  uint8_t segdata[6];

  dst = 0;
  segdata[dst++] = TM1637_CMD_ADDR | TM1637_ADDR_C0H;
  segdata[dst++] = 0;
  segdata[dst++] = 0;
  segdata[dst++] = 0;
  segdata[dst++] = 0;

  writes(segdata, dst);
}

void TM1637::test(void)
{
  int32_t dst;
  uint8_t segdata[6];

  dst = 0;
  segdata[dst++] = TM1637_CMD_ADDR | TM1637_ADDR_C0H;
  segdata[dst++] = 0b00111111;
  segdata[dst++] = 0b00000110 | TM1637_DBIT_COLON;
  segdata[dst++] = 0b01011011;
  segdata[dst++] = 0b01001111;

  writes(segdata, dst);
}

void TM1637::digits(uint8_t data[4], bool colon)
{
  int32_t dst;
  uint8_t segdata[6];

  dst = 0;
  segdata[dst++] = TM1637_CMD_ADDR | TM1637_ADDR_C0H;
  segdata[dst++] = data[0];
  segdata[dst++] = data[1] | (colon ? TM1637_DBIT_COLON : 0);
  segdata[dst++] = data[2];
  segdata[dst++] = data[3];

  writes(segdata, dst);
}

//
// TM1637 privates
//

void TM1637::dio_start(void)
{
  // CLK 11
  // DIO 10

  // set both high to enter start
  uint8_t data = PIN_CLOCK | PIN_DIO;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);

  data = PIN_CLOCK;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);
}

void TM1637::dio_stop(void)
{
  // CLK 0111
  // DIO 0011

  uint8_t data = 0;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);

  data = PIN_CLOCK;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);
  data = PIN_CLOCK | PIN_DIO;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);
}

void TM1637::write_byte(uint8_t b)
{
  uint8_t data;
  for (int i = 0; i < 8; i++)
  {
    // CLK 001
    // DIO 0bb

    data = 0;
    _ft232->write_data(&data, 1);
    usleep(DATA_DELAY);

    // send LSB to MSB
    data = b & 1 ? PIN_DIO : 0;
    _ft232->write_data(&data, 1);
    usleep(CLOCK_DELAY);

    data |= PIN_CLOCK;
    _ft232->write_data(&data, 1);
    usleep(CLOCK_DELAY);

    b >>= 1; // next LSB
  }
}

void TM1637::skip_ack(void)
{
  // CLK 010
  // DIO 111

  uint8_t data = PIN_DIO;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);

  data |= PIN_CLOCK;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);

  data &= ~PIN_CLOCK;
  _ft232->write_data(&data, 1);
  usleep(CLOCK_DELAY);
}

} // namespace ft232gpio
