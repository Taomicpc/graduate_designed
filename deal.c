//将图片数据处理完以后刷新到lcd屏幕
//做二值化预处理。
#include "public.h"
#include <jpeglib.h>
#include <jerror.h>


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

//单一个像素点由(jpeg)RGB888转换为灰度（因为当前LCD是采用的RGB565显示的）
unsigned char RGB888toASH(unsigned char red, unsigned char green, unsigned char blue)
{
    long long result;

    result = (long long)red*299+(long long)green*587 + (long long)blue*114;

	return (unsigned char)(result/1000) ;
}

//单一个像素点由(jpeg)灰度转换为RGB565（因为当前LCD是采用的RGB565显示的）
unsigned short ASHtoRGB565(unsigned char ash)
{
	return (unsigned short) (((ash>>3)<<11) | ((ash>>2)<<5) | (ash>>3));
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


int main(int argc, char* argv[])
{
    int IshmId;
    key_t ipcKey;
    char filename[50];
    FILE* writeImageFd = 0;
    shmType* shmPtr = NULL;
    int jpgnum;
    int count;

 	//设置显卡设备framebuffer
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE* imageFd = 0;							//Jpeg文件的句柄
	unsigned char *lcdLineBuf;

	int lcdFd;
	char *fb_device;
	unsigned int x;
	unsigned int y;

//连接到共享内存    
    ipcKey = ftok("./shm", 'a');
    if(ipcKey == -1)
        perror("ftok error");

    IshmId = shmget(ipcKey, sizeof(shmType),S_IRUSR | S_IWUSR | IPC_CREAT | 0777);//0666是ubuntu下此操作需要的校验

    if(IshmId == -1)
    {
        perror("shmget error");
        return;
    }

    shmPtr = (shmType *)shmat(IshmId, NULL, 0);
   
    if(shmPtr == (void *)-1)
    {
        perror("main.c shmat error");
        return;
    } 

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
    while(1)
    {
        if(shmPtr->deal.b_deal_running == false)
        {
            sem_wait(&shmPtr->deal.sem_deal_wakeup);//睡眠等待控制台允许
        }
        
        sem_wait(&shmPtr->deal.sem_deal_standby);//等待获得写入lcd的信号量

         //sem_wait(&shmPtr->shmSem);
        if(shmPtr->b_endflag == true)
        {//主程序改变标志位，需要退出
            //sem_post(&shmPtr->shmSem);
            break;
        }
       
        if((imageFd = fopen("image.jpg", "rb")) == NULL)
	 	{
	 		fprintf(stderr, "open %s failed\n", "image.jpg");
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
	 				//color = RGB888toRGB565(lcdLineBuf[x * 3],
	 				//		lcdLineBuf[x * 3 + 1], lcdLineBuf[x * 3 + 2]);
	 				color = ASHtoRGB565( RGB888toASH(lcdLineBuf[x*3], lcdLineBuf[x*3 + 1], lcdLineBuf[x*3 + 2]) ); 
                    lcdFillPix(lcdDev.lcdMem, lcdDev.lcdBufWidth, lcdDev.lcdBufHeight, x+440, y+120, color);
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

        sem_wait(&shmPtr->shmSem);
       // printf("finish deal\n");
        shmPtr->deal.b_finish_deal = true;
        sem_post(&shmPtr->shmSem);
    }

	//关闭Jpeg输入文件
	fclose(imageFd);
    
    shmdt(shmPtr);//解除映射关系;

    return 0;
}

