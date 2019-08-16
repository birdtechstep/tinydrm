# tinydrm
raspbian buster tinydrm (start kernel 4.19.58-y)

## Out-of-tree tinydrm modules

Not everyone is able or want to spend time on getting a driver included in Linux, but many can write drivers that works and wants to share it.

This repo is dedicated to the maker community and people that make things work.

### rpi kernel
~~~~
sudo apt install git bc bison flex libssl-dev libncurses5-dev
sudo wget https://raw.githubusercontent.com/notro/rpi-source/master/rpi-source -O /usr/bin/rpi-source && sudo chmod +x /usr/bin/rpi-source && /usr/bin/rpi-source -q --tag-update
rpi-source
~~~~

### tinydrm
~~~~
git clone https://github.com/birdtechstep/tinydrm.git
cd tinydrm
make

sudo cp core/tinydrm.ko /lib/modules/`uname -r`/kernel/drivers/gpu/drm/tinydrm/core/
sudo cp mipi-dbi.ko /lib/modules/`uname -r`/kernel/drivers/gpu/drm/tinydrm/
sudo cp ili9488.ko /lib/modules/`uname -r`/kernel/drivers/gpu/drm/tinydrm/
sudo depmod
~~~~

### Dual Display Set
~~~~
sudo nano /usr/share/X11/xorg.conf.d/99-fbturbo.conf
~~~~

~~~~
# This is a minimal sample config file, which can be copied to
# /etc/X11/xorg.conf in order to make the Xorg server pick up
# and load xf86-video-fbturbo driver installed in the system.
#
# When troubleshooting, check /var/log/Xorg.0.log for the debugging
# output and error messages.
#
# Run "man fbturbo" to get additional information about the extra
# configuration options for tuning the driver.

Section "Device"
        Identifier      "Allwinner A10/A13 FBDEV"
        Driver          "fbturbo"
        Option          "fbdev" "/dev/fb0"

        Option          "SwapbuffersWait" "true"
EndSection

Section "Device"
        Identifier      "FBDEV 1"
        Driver          "fbturbo"
        Option          "fbdev" "/dev/fb1"
EndSection

Section "Screen"
        Identifier      "VGA"
        Device          "Allwinner A10/A13 FBDEV"
        Monitor         "Monitor name 0"
EndSection

Section "Screen"
        Identifier      "HDMI"
        Device          "FBDEV 1"
        Monitor         "Monitor name 1"
EndSection

Section "ServerLayout"
        Identifier      "Default Layout"
        Screen          0 "VGA"
        Screen          1 "HDMI" RightOf "VGA"
EndSection
~~~~

### Set Input cell
~~~~
[ 1 0 0 ]
[ 0 1 0 ]
[ 0 0 1 ]
~~~~
Option "TransformationMatrix" "c0 0 c1 0 c2 c3 0 0 1"

•	c0 = touch_area_width / total_width

•	c2 = touch_area_height / total_height

•	c1 = touch_area_x_offset / total_width

•	c3 = touch_area_y_offset / total_height

#### Example

X 480/1366+480 = 480/1846 = 0.2600216684723727

Y 320/768  = 320/768 = 0.4166666666666667

X Offset 1366/1846 = 0.7399783315276273


Option "TransformationMatrix" "0.2600216684723727 0 0.7399783315276273 0 0.4166666666666667 0 0 0 1"

~~~~
sudo nano /usr/share/X11/xorg.conf.d/40-libinput.conf
~~~~
add Option

~~~~
Section "InputClass"
        Identifier "libinput touchscreen catchall"
        MatchIsTouchscreen "on"
        MatchDevicePath "/dev/input/event*"
        Driver "libinput"
        Option "TransformationMatrix" "0.2600216684723727 0 0.7399783315276273 0 0.4166666666666667 0 0 0 1"
EndSection
~~~~

### test video on fb1
~~~~
sudo apt install -y mplayer
wget http://fredrik.hubbe.net/plugger/test.mpg
mplayer -nolirc -vo fbdev2:/dev/fb1 -zoom -x 480 -y 320 test.mpg
~~~~

#### BIRD TECHSTEP
