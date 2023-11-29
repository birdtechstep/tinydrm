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
uncomments
~~~~
dtparam=i2c_arm=on
dtparam=spi=on
~~~~
comments
~~~~
dtoverlay=vc4-kms-v3d
max_framebuffers=2
~~~~
add line
~~~~
# PiTFT350 320x480 Capacitive (st7796)
dtoverlay=pitft350-capacitive
dtparam=speed=32000000
dtparam=fps=25
dtparam=rotation=90
dtparam=touch-swapxy=true
dtparam=touch-invx=true

# PiTFT392 320x320 Capacitive (st7796)
#dtoverlay=pitft392-capacitive
#dtparam=speed=32000000
#dtparam=fps=25
~~~~

## PIN CONNECT
~~~~
DISPLAY SPI
cs-gpios    = CE0
sclk-gpios  = SCLK
mosi-gpios  = MOSI
miso-gpios  = MISO
dc-gpios    = BCM27 [pin13]
reset-gpios = BCM16 [pin36]
led-gpios   = BCM12 [pin32]
Capacitive Touch I2C
interrupts  = BCM4 [pin7]
~~~~
### BIRD TECHSTEP
