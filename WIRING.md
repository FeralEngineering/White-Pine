# Wiring

## Power

```text
LiPo Battery
    |
    v
TP4056 Charger Board
(B+ / B-)
    |
    v
OUT+ ---- Rocker Switch ---- MT3608 VIN+
OUT- ----------------------- MT3608 VIN-

MT3608 VOUT+ -------------- D1 Mini 5V
MT3608 VOUT- -------------- D1 Mini GND

LED+
 |
 +------------------------- MT3608 VOUT+

LED-
 |
 +------------------------- GND
```

## Button

```text
Button Pin 1 -------------- D5
Button Pin 2 -------------- GND
```

## OLED Display

```text
OLED VCC ------------------ 3V3
OLED GND ------------------ GND
OLED SDA ------------------ D2
OLED SCL ------------------ D1
```
