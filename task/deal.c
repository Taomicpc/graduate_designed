//将图片数据处理完以后,压缩成jpeg图像
//做灰度预处理。
#include "public.h"
#include <jpeglib.h>
#include <jerror.h>

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

int main(int argc, char* argv[])
{
    int IshmId;
    key_t ipcKey;
    char filename[50];
    shmType* shmPtr = NULL;
    int jpgnum;
    int count;
    int x;

//jpeg解压变量声明
	struct jpeg_decompress_struct decompress_info;
	struct jpeg_error_mgr jerr;
	FILE* inImageFd = 0;							//Jpeg文件的句柄
	unsigned char *lcdLineBuf;
//jpeg压缩变量声明
	struct jpeg_compress_struct compress_info;
	struct jpeg_error_mgr compress_jerr;
	FILE* outImageFd = 0;							//Jpeg文件的句柄
    JSAMPROW row_pointer[1];//一行位图
    int row_stride;//每行字节数

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
        perror("deal.c shmat error");
        return;
    } 

    printf("deal process standy!\n");

    while(1)
    {
        //sem_wait(&shmPtr->shmSem);
        if(shmPtr->b_endflag == true)
        {//主程序改变标志位，需要退出
            //sem_post(&shmPtr->shmSem);
            break;
        }

        if(shmPtr->deal.b_deal_running == false)
        {
            printf("deal process Sleep!\n");
            sem_wait(&shmPtr->deal.sem_deal_wakeup);//睡眠等待控制台允许
            printf("deal process Wakeup!\n");
        }
        
        sem_wait(&shmPtr->deal.sem_deal_standby);//等待获得写入lcd的信号量

        //每次解压完一张图像后,都要关闭文件,否则直接操作可能会报Not a JPEG file: starts with 0x93
        //0x80
        if((inImageFd = fopen("./image/src_image.jpg", "rb")) == NULL)
        {
    	    fprintf(stderr, "deal.c open %s failed\n", "./image/src_image.jpg");
    	    exit(-1);
        }
    
        if((outImageFd = fopen("./image/deal_image.jpg", "wb")) == NULL)
        {
    	    fprintf(stderr, "deal.c open %s failed\n", "./image/deal_image.jpg");
    	    exit(-1);
        }
      

//压缩对象初始化
         //对图像jpeg解压缩需要调用libjpeg库文件。
	 	decompress_info.err = jpeg_std_error(&jerr);
	 	jpeg_create_decompress(&decompress_info);//解码

	 	//导入要解压的Jpeg文件inImageFd
	 	jpeg_stdio_src(&decompress_info, inImageFd);

	 	//读取jpeg文件的文件头
	 	jpeg_read_header(&decompress_info, TRUE);

	 	//开始解压Jpeg文件，解压后将分配给scanline缓冲区，
	 	jpeg_start_decompress(&decompress_info);

	 	lcdLineBuf = (unsigned char *) malloc(decompress_info.output_width
	 			* decompress_info.output_components);
//解压缩对象初始化
        compress_info.err = jpeg_std_error(&compress_jerr);
        jpeg_create_compress(&compress_info);
        
        jpeg_stdio_dest(&compress_info, outImageFd);

        compress_info.image_width = decompress_info.image_width;//按输入的图像大小设定输出图像大小,单位为像素
        compress_info.image_height = decompress_info.image_height;//按输入的图像大小设定输出图像大小
        //printf("image width:%d,image height:%d\n", decompress_info.image_width, decompress_info.image_height);
        compress_info.input_components = 1;//像素位宽,灰度图输出选1
        compress_info.in_color_space = JCS_GRAYSCALE;//表示灰度图

        jpeg_set_defaults(&compress_info);//设置默认
        jpeg_set_quality(&compress_info, 80, TRUE);//压缩比为80%

        jpeg_start_compress(&compress_info, TRUE);

    
        row_stride = compress_info.image_width * 1;//一行的字节数,灰度图乘以1
	 	while (decompress_info.output_scanline < decompress_info.output_height)
	 	{
	 		jpeg_read_scanlines(&decompress_info, &lcdLineBuf, 1);
            unsigned char gary_color;
            for (x = 0; x < decompress_info.output_width; x++)
	 	    {
	 	    	gary_color = RGB888toASH(lcdLineBuf[x*3], lcdLineBuf[x*3 + 1], lcdLineBuf[x*3 + 2]); 
                lcdLineBuf[x] = gary_color;
	 	    }
            row_pointer[0] = lcdLineBuf;//缓冲区的地址
            jpeg_write_scanlines(&compress_info, row_pointer, 1);
	 	}

	 	//完成Jpeg解码，释放Jpeg文件
	 	jpeg_finish_decompress(&decompress_info);
	 	jpeg_destroy_decompress(&decompress_info);

        //完成jpeg编码,释放文件资源
        jpeg_finish_compress(&compress_info);
        jpeg_destroy_compress(&compress_info);

	 	//释放帧缓冲区
	 	free(lcdLineBuf);

        sem_wait(&shmPtr->shmSem);
       // printf("finish deal\n");
        shmPtr->deal.b_finish_deal = true;
        sem_post(&shmPtr->shmSem);
 
	    //关闭Jpeg输入文件
	    fclose(inImageFd);
        fclose(outImageFd);
    }
    
    shmdt(shmPtr);//解除映射关系;

    return 0;
}

