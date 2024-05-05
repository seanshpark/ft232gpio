# FT232 as USB to GPIO

Use FT232 USB to TTL Serial as GPIO for PC.
- Use DTR/CTS/RX/TX as GPIO pins
- Use libftdi1

Pin usage

Port | Serial | I2C | TM1637
--- | --- | --- | ---
D4 | DTR | SDA | DIO
D3 | CTS | SCL | CLK

## Prerequisite

```
sudo apt-get install libusb-dev libconfuse-dev libftdi1-dev
```
