# tinydrm
tinydrm for Raspberry Pi OS Kernel rpi-6.1.y

## Out-of-tree tinydrm modules

Not everyone is able or want to spend time on getting a driver included in Linux, but many can write drivers that works and wants to share it.

This repo is dedicated to the maker community and people that make things work.

### First
~~~
sudo apt update
sudo apt upgrade
sudo reboot
~~~

### Install Raspberry Pi Kernel Headers
~~~~
sudo apt install git bc bison flex libssl-dev libncurses5-dev
sudo apt-get install raspberrypi-kernel-headers
~~~~

### tinydrm
~~~~
git clone -b rpi-6.1.y https://github.com/birdtechstep/tinydrm.git
cd tinydrm
make

sudo cp ili9488.ko /lib/modules/`uname -r`/kernel/drivers/gpu/drm/tiny/
sudo depmod
~~~~

### test video on fbX [example fb1]
~~~~
sudo apt install -y mplayer
wget http://fredrik.hubbe.net/plugger/test.mpg
mplayer -nolirc -vo fbdev2:/dev/fb1 -zoom -x 480 -y 320 test.mpg
~~~~

#### BIRD TECHSTEP

##### Donation
If this project help you reduce time to develop, you can give me a cup of coffee :) 

[![paypal](https://www.paypalobjects.com/en_GB/TH/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=CYA3UGY8TNY82)
