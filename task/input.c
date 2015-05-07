//初始化摄像头数据，并实时采集更新数据

#include "public.h"

#include <linux/videodev2.h>

int main(int argc, char* argv[])
{
    int IshmId;
    key_t ipcKey;
	FILE* writeImageFd = 0;
    shmType* shmPtr = NULL;

//连接到共享内存    
    ipcKey = ftok("/opt/designed/shm", 'a');
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
   
	int numBufs;//在for循环中计数变量

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

    videoBufType* videoBuf = calloc(v4l2Req.count, sizeof(videoBufType));	//将VIDIOC_REQBUFS获取内存转为物理空间

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

    printf("Input process standby!\n");

    numBufs = 0;
    while(1)
    {
        //sem_wait(&shmPtr->shmSem);
        if(shmPtr->b_endflag == true)
        {//主程序改变标志位，需要退出
            //sem_post(&shmPtr->shmSem);
            break;
        }
        
        if(shmPtr->input.b_input_running == false)
        {
            printf("Input process Sleep!\n");
            sem_wait(&shmPtr->input.sem_input_wakeup);//睡眠等待控制台允许
            printf("Input process Wakeup!\n");
        }

        if(shmPtr->wtofile.b_finish_wtofile == true 
            && shmPtr->wtolcd.b_finish_wtolcd == true
            && shmPtr->deal.b_finish_deal == true)
        {
            sem_post(&shmPtr->input.sem_wr_enable);//释放信号量,表明可写
        }
        else
        {
            continue;//不断查共享内存区的标志为是否已处理完可写。
        }
        
        v4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;		//取得原始采集数据
    	v4l2Buf.memory = V4L2_MEMORY_MMAP;			//存储类型：V4L2_MEMORY_MMAP（内存映射）或V4L2_MEMORY_USERPTR（用户指针）
    	if (ioctl(videoFd, VIDIOC_DQBUF, &v4l2Buf) < 0)
    	{
    		perror("VIDIOC_DQBUF failed.\n");
    		return -1;
    	}
   
        int i=0;
		
        while(i<v4l2Buf.bytesused)
        {//jpeg图像中，ffd8作为图像的开始，故需要先偏移到图像开始处。
            if((videoBuf[0].start[i] == 0x000000FF)
				&& (videoBuf[0].start[i+1] == 0x000000D8))
			    break;
            ++i;
        }
    
       int imageSize = v4l2Buf.bytesused - i;

       sem_wait(&shmPtr->input.sem_wr_enable);//获得可修改图像信号量
       
       if((writeImageFd = fopen("/opt/designed/image/src_image.jpg", "wb")) < 0)
       {
       	printf("Unable to create y frame recording file\n");
       	return -1;
       }

       //printf("reflashing\n");
       fwrite(videoBuf[0].start+i, imageSize, 1, writeImageFd);	//将采集得到的数据写入到jpg文件中。
       fclose(writeImageFd);

        sem_wait(&shmPtr->shmSem);
        if(shmPtr->wtofile.b_wtofile_running == true)
        {
            shmPtr->wtofile.b_finish_wtofile = false;
            sem_post(&shmPtr->wtofile.sem_wtofile_standby);//释放写到文件的信号量
        }
        if(shmPtr->wtolcd.b_wtolcd_running == true)
        {
            shmPtr->wtolcd.b_finish_wtolcd = false;//false;
            sem_post(&shmPtr->wtolcd.sem_wtolcd_standby);//释放读取jpg刷新到lcd的信号量
        }
        if(shmPtr->deal.b_deal_running == true)
        {
            shmPtr->deal.b_finish_deal = false;//false;
            sem_post(&shmPtr->deal.sem_deal_standby);//释放读取jpg刷新到lcd的信号量
        }
        sem_post(&shmPtr->shmSem);//释放信号量
        
       //pthread_rwlock_unlock(&dataPtr->rwlock);
       
       //获取下一帧视频数据
       if (ioctl(videoFd, VIDIOC_QBUF, &v4l2Buf) < 0)
       {
       	printf("VIDIOC_QBUF error\n");
       	return -1;
       }
    }
    //退出前把申请的记录videobuf空间释放
    for (numBufs = 0; numBufs < v4l2Req.count; numBufs++)//撤销视频帧缓冲区的内存映射
    {
        munmap(videoBuf[numBufs].start, videoBuf[numBufs].length);
    }
    shmdt(shmPtr);//解除映射关系;

	close(videoFd);                         //关闭摄像头设备
    printf("input process exit!\n");

    exit(0);
}
    
