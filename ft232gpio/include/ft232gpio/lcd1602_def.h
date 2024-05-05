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

#ifndef __FT232GPIO_LCD1602_DEF_H__
#define __FT232GPIO_LCD1602_DEF_H__

// clang-format off
#define HD44780_LCD_CMD_DDRAMADDR     0b10000000
#define HD44780_LCD_CMD_CGRAMADDR     0b01000000
#define HD44780_LCD_CMD_FUNCSET       0b00100000
#define HD44780_LCD_CMD_CURSOR        0b00010000
#define HD44780_LCD_CMD_DISPLAY       0b00001000
#define HD44780_LCD_CMD_ENTRY         0b00000100
#define HD44780_LCD_CMD_RETHOME       0b00000010
#define HD44780_LCD_CMD_CLEAR         0b00000001

// HD44780_LCD_CMD_FUNCSET
#define HD44780_LCD_FUNCSET_8BIT      0b00010000
#define HD44780_LCD_FUNCSET_4BIT      0b00000000
#define HD44780_LCD_FUNCSET_2LINES    0b00001000
#define HD44780_LCD_FUNCSET_1LINES    0b00000000
#define HD44780_LCD_FUNCSET_5x11      0b00000100
#define HD44780_LCD_FUNCSET_5x8       0b00000000

// HD44780_LCD_CMD_CURSOR
#define HD44780_LCD_CURSOR_SHIFT_DIS  0b00001000 // display shift
#define HD44780_LCD_CURSOR_SHIFT_CUR  0b00000000 // cursor move
#define HD44780_LCD_CURSOR_RIGHT      0b00000100 // shift to the right
#define HD44780_LCD_CURSOR_LEFT       0b00000000 // shift to the left

// HD44780_LCD_CMD_DISPLAY
#define HD44780_LCD_DISPLAY_ON        0b00000100
#define HD44780_LCD_DISPLAY_OFF       0b00000000
#define HD44780_LCD_DISPLAY_CUR       0b00000010
#define HD44780_LCD_DISPLAY_CUR_OFF   0b00000000
#define HD44780_LCD_DISPLAY_BLINK     0b00000001
#define HD44780_LCD_DISPLAY_BLINK_OFF 0b00000000

// HD44780_LCD_CMD_ENTRY
#define HD44780_LCD_ENTRY_INC         0b00000010 // increment vs decrement
#define HD44780_LCD_ENTRY_SHIFT       0b00000001 // entire shift on vs off

#define PCF8574_LCD1604_RS            0b00000001 // LCD160x_RS (Register Select: Inst / Data)
#define PCF8574_LCD1604_RW            0b00000010 // LCD160x_RW (R or /W)
#define PCF8574_LCD1604_EN            0b00000100 // LCD160x_EN (Enable)
#define PCF8574_LCD1604_BL            0b00001000 // LCD160x_BL (BackLignt)

// clang-format on

#endif // __FT232GPIO_LCD1602_DEF_H__
