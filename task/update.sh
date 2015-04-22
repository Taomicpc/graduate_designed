#! /bin/sh

rm /home/tao/work/linux-3.0-rc6-Old/drivers/char/taoTower.ko
cd /home/tao/work/linux-3.0-rc6-Old/
make SUBDIRS=drivers/char/ modules

cd /home/tao/program_practice/linuxBsp_practice/graduate_designed/
make

sudo cp -f exemain input wtofile wtolcd deal tower /srv/nfs/
sudo cp -f /home/tao/work/linux-3.0-rc6-Old/drivers/char/taoTower.ko /srv/nfs/

