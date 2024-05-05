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
#include <ft232gpio/i2c.h>
#include <ft232gpio/lcd1602.h>

#include <cstdio>

#include <signal.h>
#include <unistd.h>

static bool _do_loop = true;

void signal_handler(int sig)
{
  printf("Ctrl+Break!\r\n");
  _do_loop = false;
}

void show_lcd1602(ft232gpio::LCD1602 &lcd1602)
{
  while (_do_loop)
  {
    lcd1602.move(0, 0);
    lcd1602.puts("Hello!");
    sleep(1);

    lcd1602.move(1, 0);
    lcd1602.puts("World!");
    sleep(1);

    lcd1602.clear();
    sleep(1);
  }
}

int main(int argc, char **argv)
{
  signal(SIGINT, signal_handler);

  ft232gpio::FT232 ft232;
  if (!ft232.init())
    return -1;

  ft232gpio::I2C i2c;
  i2c.init(&ft232, 0x27);

  ft232gpio::LCD1602 lcd1602;
  lcd1602.init(&i2c);
  lcd1602.cursor(true);
  lcd1602.blink(true);

  show_lcd1602(lcd1602);

  lcd1602.release();
  i2c.release();
  ft232.release();

  return 0;
}
