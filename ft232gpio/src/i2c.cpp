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

// Reference code from https://en.wikipedia.org/wiki/I%C2%B2C

#include "ft232gpio/i2c.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

#include <unistd.h> // usleep

#define PIN_SCL 0x08 // CTS of FT232
#define PIN_SDA 0x10 // DTR of FT232

#define I2C_DELAY 10
#define I2C_DELAY_WAIT 1
#define I2C_RETRY 1000

namespace ft232gpio
{

I2C::I2C()
{
  //
}

I2C::~I2C()
{
  //
}

bool I2C::init(FT232 *ft232, uint8_t addr)
{
  _ft232 = ft232;
  _addr = addr;

  _lost = false;
  _started = false;

  _set_sda();
  _set_scl();
  _ft232_data = PIN_SCL | PIN_SDA;

  _initalized = true;

  return true;
}

void I2C::release(void)
{
  if (_ft232 == nullptr)
  {
    assert(false);
    return;
  }
  _set_sda();
  _set_scl();
  _ft232_data = PIN_SCL | PIN_SDA;

  _addr = 0;
  _ft232 = nullptr;
  _initalized = false;
}

void I2C::_set_sda(void)
{
  _ft232_data |= PIN_SDA;
  _ft232->write_data(&_ft232_data, 1);
}

void I2C::_set_scl(void)
{
  _ft232_data |= PIN_SCL;
  _ft232->write_data(&_ft232_data, 1);
}

void I2C::_clear_sda(void)
{
  _ft232_data &= ~PIN_SDA;
  _ft232->write_data(&_ft232_data, 1);
}

void I2C::_clear_scl(void)
{
  _ft232_data &= ~PIN_SCL;
  _ft232->write_data(&_ft232_data, 1);
}

uint8_t I2C::_read_scl(void)
{
  uint8_t data;
  if (!_ft232->read_data(&data))
    return 0;
  // printf("Read SCL: 0x%02x\r\n", (uint32_t)data);
  return data & PIN_SCL;
}

uint8_t I2C::_read_sda(void)
{
  uint8_t data;
  if (!_ft232->read_data(&data))
    return 0;
  // printf("Read SDA: 0x%02x\r\n", (uint32_t)data);
  return data & PIN_SDA;
}

void I2C::_dummy_clock(void)
{
  // do dummy clock pulse skip read ack
  _set_scl();
  _delay();
  _clear_scl();
  _delay();
}

void I2C::_delay(void)
{
  //
  usleep(I2C_DELAY);
}

void I2C::_arbitration_lost(void) { std::cerr << "I2C arbitration_lost" << std::endl; }

void I2C::_wait_scl(void)
{
  uint16_t retry = I2C_RETRY;
  while (_read_scl() == 0)
  {
    if (!retry)
    {
      std::cerr << "I2C wait SCL timeout" << std::endl;
      break;
    }
    retry--;
    usleep(I2C_DELAY_WAIT);
  }
}

void I2C::start_cond(void)
{
  if (_started)
  {
    _set_sda();
    _delay();
    _set_scl();

    _wait_scl();
    _delay();
  }

  if (_read_sda() == 0)
    _arbitration_lost();

  _clear_sda();
  _delay();
  _clear_scl();

  _started = true;
}

void I2C::stop_cond(void)
{
  //_clear_sda();
  //_delay();

  _set_scl();
  _wait_scl();
  _delay();

  _set_sda();
  _delay();

  if (_read_sda() == 0)
    _arbitration_lost();

  _started = false;
}

void I2C::write_bit(bool bit)
{
  if (bit)
    _set_sda();
  else
    _clear_sda();

  _delay();
  _set_scl();
  _delay();
  //_wait_scl();
  // if (bit && (_read_sda() == 0))
  //  _arbitration_lost();
  _clear_scl();
}

bool I2C::read_bit(void)
{
  bool bit;

  _set_sda();
  _delay();
  _set_scl();

  _wait_scl();
  _delay();

  bit = _read_sda();

  _clear_scl();

  return bit ? true : false;
}

bool I2C::write_byte(bool send_start, bool send_stop, uint8_t data)
{
  uint32_t bit;
  uint8_t send;
  bool nack;

  if (send_start)
  {
    start_cond();
    _delay();

    // send 7bit addr, from MSB to LSB
    send = _addr;
    send <<= 1;
    for (bit = 0; bit < 7; ++bit)
    {
      bool set = (send & 0x80) != 0;
      // std::cout << set ? "1" : "0";
      write_bit(set);
      send <<= 1;
    }
    // std::cout << "W";
    //  send W (0)
    write_bit(0);

    // read ack
    nack = read_bit() ? true : false;
    // std::cout << "Addres ACK: " << (nack ? 1 : 0) << std::endl;
    // std::cout << " ACK" << (nack ? 1 : 0);
    // std::cout << std::endl;
  }

  // send data
  send = data;
  for (bit = 0; bit < 8; ++bit)
  {
    bool set = (send & 0x80) != 0;
    // std::cout << set ? "1" : "0";
    write_bit(set);
    send <<= 1;
  }

  nack = read_bit() ? true : false;
  // std::cout << " ACK" << (nack ? 1 : 0);
  // std::cout << std::endl;
  // std::cout << "Addres ACK: " << (nack ? 1 : 0) << std::endl;

  if (send_stop)
    stop_cond();

  return nack;
}

// Read a byte from I2C bus
uint8_t I2C::read_byte(bool nack, bool send_stop)
{
  uint8_t byte = 0;
  uint8_t bit;

  for (bit = 0; bit < 8; ++bit)
  {
    byte = (byte << 1) | (read_bit() ? 0x01 : 0x00);
  }

  write_bit(nack);

  if (send_stop)
    stop_cond();

  return byte;
}

} // namespace ft232gpio
