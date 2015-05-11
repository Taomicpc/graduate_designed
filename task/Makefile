CC   =arm-linux-
CROSS=arm-linux-

INCPATH =-I/home/tao/program_practice/graduate_designed/public_lib
SRC     =-L/home/tao/program_practice/graduate_designed/public_src

all: exemain input wtofile wtolcd deal tower facedetect


exemain:main.c public.c myLib.c
	$(CROSS)gcc -o exemain main.c public.c myLib.c -ljpeg -lpthread $(INCPATH) $(SRC)

input:input.c public.c
	$(CROSS)gcc -o input input.c public.c -ljpeg -lpthread  $(INCPATH) $(SRC)

wtofile:wtofile.c public.c
	$(CROSS)gcc -o wtofile wtofile.c public.c -ljpeg -lpthread  $(INCPATH) $(SRC)

wtolcd:wtolcd.c public.c
	$(CROSS)gcc -o wtolcd wtolcd.c public.c -ljpeg -lpthread  $(INCPATH) $(SRC)

deal:deal.cpp 
	$(CROSS)g++ `pkg-config --cflags opencv` -o deal deal.cpp -lpthread -lrt -ldl `pkg-config --libs opencv` $(INCPATH) $(SRC)

facedetect:facedetect.cpp 
	$(CROSS)g++ `pkg-config --cflags opencv` -o facedetect facedetect.cpp -lpthread -lrt -ldl `pkg-config --libs opencv`

tower:tower.c public.c
	$(CROSS)gcc -o tower tower.c public.c -ljpeg -lpthread  $(INCPATH) $(SRC)

usb_camera:usb_camera.c
	$(CROSS)gcc -o usb_camera usb_camera.c -ljpeg -lpthread
	$(CROSS)strip usb_camera
	@sudo cp -f usb_camera /srv/nfs/

clean:
	@rm -vf usb_camera *.o *~
	@sudo rm -vf /srv/nfs/usb_camera
