#! /bin/sh

#使用当前目录下的底层驱动源码，放到内核源码下编译出模块．
cmp ./taoTower.c /home/tao/work/linux-3.0-rc6-Old/drivers/char/taoTower.c
if [ $? ]
then
    rm /home/tao/work/linux-3.0-rc6-Old/drivers/char/taoTower.ko
    cp ./taoTower.c /home/tao/work/linux-3.0-rc6-Old/drivers/char/ 
    cd /home/tao/work/linux-3.0-rc6-Old/
    make SUBDIRS=drivers/char/ modules
fi

#应用层编译
cd /home/tao/program_practice/graduate_designed/task/
make

sudo cp -f exemain input wtofile wtolcd deal tower /srv/nfs/
sudo cp -f /home/tao/work/linux-3.0-rc6-Old/drivers/char/taoTower.ko /srv/nfs/

