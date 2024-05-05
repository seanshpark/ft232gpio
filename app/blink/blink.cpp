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

#include <unistd.h>

void msleep(unsigned int msecs)
{
  usleep(msecs * 1000);
}

int main(int argc, char **argv)
{
  ft232gpio::FT232 ft232;

  if (!ft232.init())
    return -1;

  uint8_t data;

  // turn on DTR, CTS
  data = 0x18;
  ft232.write_data(&data, 1);
  sleep(1);

  // off
  data = 0x00;
  ft232.write_data(&data, 1);
  sleep(1);

  for (int i = 0; i < 10; i++)
  {
    data = 0x10;
    ft232.write_data(&data, 1);
    msleep(200);

    data = 0x08;
    ft232.write_data(&data, 1);
    msleep(200);
  }
  // off
  data = 0x00;
  ft232.write_data(&data, 1);
  sleep(1);

  ft232.release();

  return 0;
}
