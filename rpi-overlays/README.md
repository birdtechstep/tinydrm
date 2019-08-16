# rasbian-dtoverlay
MakerLAB35 Display 3.5 Inches. 480x320 18 bits color [ili9488] with Capacitive touchscreen [ft6236] 

## Compile dtb and put it directly into it's destination:
~~~~
sudo dtc -@ -I dts -O dtb -o /boot/overlays/foo.dtbo foo-overlay.dts
~~~~
## cmdline.txt
~~~~
fbcon=map:10 fbcon=font:VGA8x16
~~~~
## config.txt
~~~~
dtoverlay=makerlabtft35-capacitive
dtparam=speed=64000000
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
interrupts  = BCM16 [pin36]
~~~~
### BIRD TECHSTEP
