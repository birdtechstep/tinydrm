# dtoverlay for Raspberry Pi OS
PiTFT350 Display 3.50 Inches. 480x320 16 bits color [st7796s] with Capacitive touchscreen [ft6236] 
PiTFT350 Display 3.92 Inches. 320x320 16 bits color [st7796s] with Capacitive touchscreen [ft6236] 

## Install dtc_install.sh:
~~~~
chmod +x dtc_install.sh
sudo ./dtc_install.sh
~~~~

## Compile dtb and put it directly into it's destination:
~~~~
sudo dtc -@ -I dts -O dtb -o /boot/firmware/overlays/pitft350-capacitive.dtbo pitft350-capacitive-overlay.dts
sudo dtc -@ -I dts -O dtb -o /boot/firmware/overlays/pitft392-capacitive.dtbo pitft392-capacitive-overlay.dts
~~~~
## cmdline.txt
~~~~
fbcon=map:10 fbcon=font:VGA8x16
~~~~
## config.txt
~~~~
dtoverlay=pitft350-capacitive
dtparam=speed=62000000
dtparam=rotation=90
dtparam=touch-swapxy=true
dtparam=touch-invx=true
~~~~

## PIN CONNECT
~~~~
DISPLAY SPI
cs-gpios    = CE0
sclk-gpios  = SCLK
mosi-gpios  = MOSI
miso-gpios  = MISO
dc-gpios    = BCM27 [pin13]
reset-gpios = BCM26 [pin37]
led-gpios   = BCM13 [pin33]
Capacitive Touch I2C
interrupts  = BCM4 [pin36]
~~~~
### BIRD TECHSTEP
