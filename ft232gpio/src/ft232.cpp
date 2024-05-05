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

#include "ft232gpio/ft232.h"

#include <iostream>

#include <unistd.h> // usleep

namespace ft232gpio
{

FT232::FT232()
{
  //
}

FT232::~FT232()
{
  //
}

bool FT232::init(void)
{
  if ((_ftdi = ::ftdi_new()) == 0)
  {
    std::cerr << "ftdi_new failed" << std::endl;
    return false;
  }

  int fd_usb = ::ftdi_usb_open(_ftdi, 0x0403, 0x6001);
  if (fd_usb < 0 && fd_usb != -5)
  {
    auto msg = ::ftdi_get_error_string(_ftdi);
    std::cerr << "Unable to open ftdi device: " << fd_usb << " (" << msg << ")" << std::endl;
    ::ftdi_free(_ftdi);
    _ftdi = nullptr;
    return false;
  }
  if (::ftdi_set_bitmode(_ftdi, 0xFF, BITMODE_BITBANG))
  {
    std::cerr << "Failed to set bitbang mode" << std::endl;
    ::ftdi_free(_ftdi);
    _ftdi = nullptr;
    return false;
  }
  return true;
}

void FT232::release(void)
{
  if (_ftdi == nullptr)
    return;

  ::ftdi_disable_bitbang(_ftdi);
  ::ftdi_usb_close(_ftdi);
  ::ftdi_free(_ftdi);
  _ftdi = nullptr;
}

bool FT232::write_data(const uint8_t *buf, int size)
{
  auto f = ::ftdi_write_data(_ftdi, buf, size);
  if (f < 0)
  {
    std::cerr << "write_data failed: " << ::ftdi_get_error_string(_ftdi) << std::endl;
    return false;
  }
  return true;
}

bool FT232::read_data(uint8_t *buf)
{
  // bits = which bits to read
  ::ftdi_set_bitmode(_ftdi, 0x00, BITMODE_BITBANG);
  usleep(10);
  auto f = ::ftdi_read_pins(_ftdi, buf);
  usleep(10);
  ::ftdi_set_bitmode(_ftdi, 0xff, BITMODE_BITBANG);
  usleep(10);
  if (f < 0)
  {
    std::cerr << "read_data failed: " << ::ftdi_get_error_string(_ftdi) << std::endl;
    return false;
  }
  return true;
}

} // namespace ft232gpio
