/*************************************

NAME:usb_camera.c

Author:tao

Date:2015-4-11

Other:毕业设计

*************************************/

#include <errno.h>
#include <sys/types.h>	
#include <sys/stat.h>	
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>    
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>
#include <linux/fb.h>
#include <jpeglib.h>
#include <jerror.h>

typedef struct videoBuf
{
	unsigned char *start;//地址指向映射后的内核帧缓冲区
	size_t offset;//内核帧缓冲区相对文件句柄的偏移
	size_t length;
}videoBufType;

struct lcdDev
{
	int fd;//打开LCD设备的句柄
	void *lcdMem;//frame buffer mmap
	int lcdBufWidth, lcdBufHeight, lcdLineLen, lcdBufSize;
	int lcdBpp;//像素深度:16/24
}lcdDev;

//根据打开的LCD设备句柄，更新结构体的长、宽和位宽，成功则返回0，失败返回－1 
int lcdStat(int lcdFd)
{
	struct fb_fix_screeninfo fb_finfo;
	struct fb_var_screeninfo fb_vinfo;

	if (ioctl(lcdFd, FBIOGET_FSCREENINFO, &fb_finfo))
	{
		perror(__func__);
		return (-1);
	}

	if (ioctl(lcdFd, FBIOGET_VSCREENINFO, &fb_vinfo))
	{
		perror(__func__);
		return (-1);
	}

	lcdDev.lcdBufWidth = fb_vinfo.xres;
	lcdDev.lcdBufHeight = fb_vinfo.yres;
	lcdDev.lcdBpp = fb_vinfo.bits_per_pixel;
	lcdDev.lcdLineLen = fb_finfo.line_length;
	lcdDev.lcdBufSize = fb_finfo.smem_len;

	return (0);
}

//单一个像素点由(jpeg)RGB888转换为RGB565（因为当前LCD是采用的RGB565显示的）
unsigned short RGB888toRGB565(unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned short B = (blue >> 3) & 0x001F;
	unsigned short G = ((green >> 2) << 5) & 0x07E0;
	unsigned short R = ((red >> 3) << 11) & 0xF800;

	return (unsigned short) (R | G | B);
}

//将每一像素点的值(RGB565)写入到LCD的显示内存区中，直接显示
int lcdFillPix(void *lcdMem, int width, int height, int x, int y, unsigned short color)
{
	if ((x > width) || (y > height))
		return (-1);

	unsigned short *dst = ((unsigned short *)lcdMem + y * width + x);

	*dst = color;
	return 0;
}


int main(int argc, char** argv)
{
	int numBufs;//在for循环中计数变量
	printf("USB Camera Test\n");

	int videoFd = open("/dev/video2", O_RDWR, 0);			//打开摄像头设备，使用阻塞方式打开
	if (videoFd<0)
	{
		printf("open error\n");
		return  -1;
	}
//----------------------------------设置USB摄像头相关的v4l2参数---------------------------------------------
	struct v4l2_format v4l2Fmt;						//设置获取视频的格式
	memset( &v4l2Fmt, 0, sizeof(v4l2Fmt) );
	v4l2Fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//视频数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	v4l2Fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV565;			//视频源的格式为JPEG或YUN4:2:2或RGB
	v4l2Fmt.fmt.pix.width = 320;					//设置视频宽度
	v4l2Fmt.fmt.pix.height = 240;					//设置视频高度
	if (ioctl(videoFd, VIDIOC_S_FMT, &v4l2Fmt) < 0)				//使配置生效
	{
		printf("set format failed\n");
		return -1;
	}

	struct v4l2_requestbuffers v4l2Req;					//申请帧缓冲
	memset(&v4l2Req, 0, sizeof (v4l2Req));
	v4l2Req.count = 1;							//缓存数量，即可保存的图片数量
	v4l2Req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	v4l2Req.memory = V4L2_MEMORY_MMAP;					//存储类型：V4L2_MEMORY_MMAP或V4L2_MEMORY_USERPTR
	if (ioctl(videoFd, VIDIOC_REQBUFS, &v4l2Req) == -1)			//申请帧缓冲
	{
		perror("request buffer error \n");
		return -1;
	}

	videoBufType *videoBuf = calloc(v4l2Req.count, sizeof(videoBufType));	//将VIDIOC_REQBUFS获取内存转为物理空间
	struct v4l2_buffer v4l2Buf;//用于与底层驱动以标准结构进行数据通信，读取数据改写到videoBuf
	for (numBufs = 0; numBufs < v4l2Req.count; numBufs++)
	{
		memset( &v4l2Buf, 0, sizeof(v4l2Buf));
		v4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;			//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
		v4l2Buf.memory = V4L2_MEMORY_MMAP;				//存储类型：V4L2_MEMORY_MMAP（内存映射）或V4L2_MEMORY_USERPTR（用户指针）
		v4l2Buf.index = numBufs;//每一帧此信息不同
		if (ioctl(videoFd, VIDIOC_QUERYBUF, &v4l2Buf) < 0)		//根据上面填充信息，获取缓冲帧的地址
		{
			printf("VIDIOC_QUERYBUF error\n");
			return -1;
		}
		videoBuf[numBufs].length = v4l2Buf.length;
		videoBuf[numBufs].offset = (size_t)v4l2Buf.m.offset;
	    videoBuf[numBufs].start = mmap(NULL, videoBuf[numBufs].length, PROT_READ | PROT_WRITE,
			                            MAP_SHARED, videoFd, videoBuf[numBufs].offset);			//使用mmap函数将申请的缓存地址转换应用程序的绝对地址
		
        if (videoBuf[numBufs].start == MAP_FAILED)
		{//判断映射是否成功
			perror("buffers error\n");
			return -1;
		}
		if (ioctl(videoFd, VIDIOC_QBUF, &v4l2Buf) < 0)			//放入缓存队列
		{
			printf("VIDIOC_QBUF error\n");
			return -1;
		}
	}

	enum v4l2_buf_type v4l2Type;					//开始视频显示
	v4l2Type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	if (ioctl(videoFd, VIDIOC_STREAMON, &v4l2Type) < 0)
	{
		printf("VIDIOC_STREAMON error\n");
		return -1;
	}

	v4l2Fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	if (ioctl(videoFd, VIDIOC_G_FMT, &v4l2Fmt) < 0)				//读取视频源格式
	{
		printf("get format failed\n");
		return -1;
	}
	else
	{
		printf("Picture:Width = %d   Height = %d\n", v4l2Fmt.fmt.pix.width, v4l2Fmt.fmt.pix.height);
	}

	FILE* writeImageFd = 0;
	int a=0;
	int k = 0;

	//设置显卡设备framebuffer
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE* imageFd = 0;							//Jpeg文件的句柄
	unsigned char *lcdLineBuf;

	int lcdFd;
	char *fb_device;
	unsigned int x;
	unsigned int y;

	if ((lcdFd = open("/dev/fb0", O_RDWR)) < 0)			//打开显卡设备
	{
		perror(__func__);
		return (-1);
	}

	//获取LCD的信息
	lcdStat(lcdFd);							//获取LCD的长、宽和显示位宽
	printf("frame buffer: %dx%d,  %dbpp, buffersize:0x%xbyte= %d\n", 
		lcdDev.lcdBufWidth, lcdDev.lcdBufHeight, lcdDev.lcdBpp, lcdDev.lcdBufSize, lcdDev.lcdBufSize);

	lcdDev.fd = lcdFd;

	//映射framebuffer的地址
	lcdDev.lcdMem = mmap(NULL, lcdDev.lcdBufSize, PROT_READ|PROT_WRITE,MAP_SHARED, lcdDev.fd,0);
									//映射显存地址,此处为lcd设备

	//预览采集到的图像
	while (1)
	{
		//如果把处理JPEG格式的数据和显示程序分离，把处理JPEG部分的数据作成一个新的线程，预览时会更加流畅。
		for (numBufs = 0; numBufs < v4l2Req.count; numBufs++)
		{
			char s[15] = "tao.jpg";
			//sprintf(s, "%d.jpg", numBufs);
			if ((writeImageFd = fopen(s, "wb")) < 0)
			{
				printf("Unable to create y frame recording file\n");
				return -1;
			}

			v4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;		//取得原始采集数据
			v4l2Buf.memory = V4L2_MEMORY_MMAP;			//存储类型：V4L2_MEMORY_MMAP（内存映射）或V4L2_MEMORY_USERPTR（用户指针）
			if (ioctl(videoFd, VIDIOC_DQBUF, &v4l2Buf) < 0)
			{
				perror("VIDIOC_DQBUF failed.\n");
				return -1;
			}

			unsigned char *ptcur = videoBuf[numBufs].start;		//开始霍夫曼解码
            int i=0;
			
            while(i<v4l2Buf.bytesused)
            {//jpeg图像中，ffd8作为图像的开始，故需要先偏移到图像开始处。
                if((videoBuf[numBufs].start[i] == 0x000000FF)
					&& (videoBuf[numBufs].start[i+1] == 0x000000D8))
				    break;
                ++i;
            }

			int imagesize = v4l2Buf.bytesused - i;

			fwrite(ptcur+i, imagesize, 1, writeImageFd);	//将采集得到的数据写入到jpg文件中。
			fclose(writeImageFd);

			if ((imageFd = fopen(s, "rb")) == NULL)
			{
				fprintf(stderr, "open %s failed\n", s);
				exit(-1);
			}

            //对图像jpeg解压缩需要调用libjpeg库文件。
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);//解码

			//导入要解压的Jpeg文件imageFd
			jpeg_stdio_src(&cinfo, imageFd);

			//读取jpeg文件的文件头
			jpeg_read_header(&cinfo, TRUE);

			//开始解压Jpeg文件，解压后将分配给scanline缓冲区，
			jpeg_start_decompress(&cinfo);

			lcdLineBuf = (unsigned char *) malloc(cinfo.output_width
					* cinfo.output_components);
			y = 0;
			while (cinfo.output_scanline < cinfo.output_height)
			{
				jpeg_read_scanlines(&cinfo, &lcdLineBuf, 1);
				if (lcdDev.lcdBpp == 16)
				{
					unsigned short color;
					for (x = 0; x < cinfo.output_width; x++)
					{
						color = RGB888toRGB565(lcdLineBuf[x * 3],
								lcdLineBuf[x * 3 + 1], lcdLineBuf[x * 3 + 2]);
						lcdFillPix(lcdDev.lcdMem, lcdDev.lcdBufWidth, lcdDev.lcdBufHeight, x+100, y+100, color);
					}
				}
				else if (lcdDev.lcdBpp == 24)
				{
					memcpy((unsigned char *)lcdDev.lcdMem + y * lcdDev.lcdBufWidth * 3, lcdLineBuf,
							cinfo.output_width * cinfo.output_components);
				}
				y++;						//下一个scanline
			}

			//完成Jpeg解码，释放Jpeg文件
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);

			//释放帧缓冲区
			free(lcdLineBuf);

			//关闭Jpeg输入文件
			fclose(imageFd);

			//获取下一帧视频数据
			if (ioctl(videoFd, VIDIOC_QBUF, &v4l2Buf) < 0)
			{
				printf("VIDIOC_QBUF error\n");
				return -1;
			}
		}
	}
	
    munmap(lcdDev.lcdMem, lcdDev.lcdBufSize);//撤销LCD显示的内存映射
    
	for (numBufs = 0; numBufs < v4l2Req.count; numBufs++)//撤销视频帧缓冲区的内存映射
    {
        munmap(videoBuf[numBufs].start, videoBuf[numBufs].length);
    }

    close(lcdFd);							//关闭lcd设备
	close(videoFd);                         //关闭摄像头设备
}

