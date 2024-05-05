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

#include <ft232gpio/ft232.h>
#include <ft232gpio/tm1637.h>

#include <random>

#include <unistd.h>

void msleep(unsigned int msecs)
{
  //
  usleep(msecs * 1000);
}

int main(int argc, char **argv)
{
  ft232gpio::FT232 ft232;
  if (!ft232.init())
    return -1;

  ft232gpio::TM1637 tm1637;
  tm1637.init(&ft232);

  tm1637.test();
  sleep(1);

  int i, j;
  uint8_t data[4];
  for (i = 0; i < 4; ++i)
    data[i] = 0xff;
  tm1637.digits(data, true);

  for (j = 0; j < 3; ++j)
    for (i = 1; i <= 8; ++i)
    {
      tm1637.bright(i);
      msleep(100);
    }

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<uint8_t> dist(0, 0x7f);

  for (j = 0; j < 25; ++j)
  {
    for (i = 0; i < 4; ++i)
      data[i] = dist(mt);
    tm1637.digits(data, false);
    msleep(100);
  }

  tm1637.release();
  ft232.release();

  return 0;
}
