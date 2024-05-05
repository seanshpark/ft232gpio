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

#ifndef __FT232GPIO_FT232_H__
#define __FT232GPIO_FT232_H__

#include <cstdint>

#include <ftdi.h>

namespace ft232gpio
{

class FT232
{
public:
  FT232();
  virtual ~FT232();

public:
  bool init(void);
  void release(void);

public:
  bool write_data(const uint8_t *buf, int size);

private:
  struct ftdi_context *_ftdi = nullptr;
};

} // namespace ft232gpio

#endif // __FT232GPIO_FT232_H__
