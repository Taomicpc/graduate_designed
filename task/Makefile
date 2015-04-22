CC=arm-linux-
CROSS=arm-linux-

all: exemain input wtofile wtolcd deal tower

exemain:main.c public.c myLib.c
	$(CROSS)gcc -o exemain main.c public.c myLib.c -ljpeg -lpthread

input:input.c public.c
	$(CROSS)gcc -o input input.c public.c -ljpeg -lpthread

wtofile:wtofile.c public.c
	$(CROSS)gcc -o wtofile wtofile.c public.c -ljpeg -lpthread

wtolcd:wtolcd.c public.c
	$(CROSS)gcc -o wtolcd wtolcd.c public.c -ljpeg -lpthread

deal:deal.c public.c
	$(CROSS)gcc -o deal deal.c public.c -ljpeg -lpthread

tower:tower.c public.c
	$(CROSS)gcc -o tower tower.c public.c -ljpeg -lpthread

usb_camera:usb_camera.c
	$(CROSS)gcc -o usb_camera usb_camera.c -ljpeg -lpthread
	$(CROSS)strip usb_camera
	@sudo cp -f usb_camera /srv/nfs/
clean:
	@rm -vf usb_camera *.o *~
	@sudo rm -vf /srv/nfs/usb_camera
