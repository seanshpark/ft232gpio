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
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

#include <signal.h>
#include <unistd.h>
#include <string.h>

static bool _do_loop = true;

void signal_handler(int sig)
{
  printf("Ctrl+Break!\r\n");
  _do_loop = false;
}

void msleep(unsigned int msecs)
{
  //
  usleep(msecs * 1000);
}

void make_time(char *buff, int32_t leng)
{
  auto now = std::chrono::system_clock::now();
  std::time_t time_now = std::chrono::system_clock::to_time_t(now);
  std::tm *tl = std::localtime(&time_now);
  snprintf(buff, leng, "%02d:%02d:%02d", tl->tm_hour, tl->tm_min, tl->tm_sec);
}

void make_temp(char *buff, int32_t leng)
{
  const char *path = "/sys/class/thermal/thermal_zone0/temp";
  std::ifstream ft(path, std::ios::in | std::ios::binary);

  std::string line;
  std::getline(ft, line);
  long lv = atol(line.c_str()) / 100;
  float fv = float(lv) / 10.0f;
  snprintf(buff, leng, "%04.1f", fv);
}

void make_freemem(char *buff, int32_t leng)
{
  const char *path = "/proc/meminfo";
  std::ifstream ft(path, std::ios::in | std::ios::binary);

  std::string line;
  std::getline(ft, line); // MemTotal
  std::getline(ft, line); // MemFree
  std::getline(ft, line); // MemAvailable:    4115664 kB

  const char *num = line.c_str() + 13;
  char *dst = buff;
  while (*num)
  {
    if (*num == ' ')
      num++;
    else
      break;
  }
  strncpy(buff, num, leng);
  num = buff;
  while (*num)
  {
    if (*num != ' ')
      num++;
    else
      break;
  }
  long lv = atol(buff) / 1024;
  snprintf(buff, leng, "%ld MB", lv);
}

void show_lcd1602(ft232gpio::LCD1602 &lcd1602)
{
  char buff[260];

  while (_do_loop)
  {
    make_temp(buff, 260);
    lcd1602.move(0, 0);
    lcd1602.puts(buff);
    lcd1602.putch(0xdf);
    lcd1602.putch('C');

    make_time(buff, 260);
    lcd1602.move(0, 8);
    lcd1602.puts(buff);

    make_freemem(buff, 260);
    lcd1602.move(1, 0);
    lcd1602.puts(buff);

    msleep(1000);
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
  lcd1602.cursor(false);
  lcd1602.blink(false);

  show_lcd1602(lcd1602);

  lcd1602.release();
  i2c.release();
  ft232.release();

  return 0;
}
