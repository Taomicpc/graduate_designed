/****************************************************************************
 ** object : ImageIden 
 ** 毕业设计三级菜单　图像识别 - ht
 ** by luchaodong
 ** class Ui::ImageIden : public Ui_ImageIden {}
 ** ImageIden 
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <QTimer>
#include <iostream>
#include "imageIden.h"

#define CHANGE_STEP 2//按键改变舵机角度的步长
int IshmId;
shmType* shmPtr = NULL;//设置共享内存的全局变量指针
const QString SAVE_ROOT="/opt/designed/image/save/";

//初始化全局变量shmPtr，建立本进程的内存链接，并进行初始化设置。
//输出0为成功初始化，1、2、3分别对应获取ipc号错误,共享内存申请失败，共享内存映射失败
int shm_init(void)
{
//--------------共享内存建立链接--------------------
    key_t ipcKey;

    ipcKey = ftok("/opt/designed/shm", 'a');
    if(ipcKey == -1)
    {
        perror("ftok error");
        return 1;//创建失败
    }

    IshmId = shmget(ipcKey, sizeof(shmType),S_IRUSR | S_IWUSR | IPC_CREAT | 0777);//0666是ubuntu下此操作需要的校验

    if(IshmId == -1)
    {
        perror("shmget error");
        return 2;
    }

    shmPtr = (shmType *)shmat(IshmId, NULL, 0);
   
    if(shmPtr == (void *)-1)
    {
        perror("main.c shmat error");
        return 3;
    }

//共享内存区初始化
    memset(shmPtr, 0, sizeof(shmType));
    shmPtr->tower.hori_angle = 90;
    shmPtr->tower.veri_angle = 90;//角度信号初始化
    shmPtr->wtofile.delay = 1;//默认初始化为1s
    //前一个1表明在进程间使用，后一个1设置一个信号量初始值，1是信号空闲，０是信号忙碌
    sem_init(&shmPtr->shmSem, 1, 1);

//回收各进程启动运行的信号量，如果不给信号，进程将进入睡眠状态．
    //设信号量忙碌，各进程即使启动，也会进入睡眠状态。
    sem_init(&shmPtr->input.sem_input_wakeup, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_wakeup, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_wakeup, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_wakeup, 1, 0);
    //sem_init(&shmPtr->deal.sem_deal_finish, 1, 0);
    sem_init(&shmPtr->tower.sem_tower_wakeup, 1, 0);

    //与上面信号量相对应的布尔量初始化
    //先置标志为false，使各进程进入上面wakeup信号量的睡眠状态
    shmPtr->input.b_input_running = true;//更新image图像默认一直运行 
    sem_post(&shmPtr->input.sem_input_wakeup);//开始video更新
    
    shmPtr->wtofile.b_wtofile_running = false; 
    shmPtr->wtolcd.b_wtolcd_running = false; 
    shmPtr->deal.b_deal_running = false; 
    //shmPtr->deal.b_need_to_show = false; 
    shmPtr->tower.b_tower_running = false; 

//信号量初始化为忙碌，用于image.jpg读写的信号同步。
    sem_init(&shmPtr->input.sem_wr_enable, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_standby, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_standby, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_standby, 1, 0);
    //与之相对应布尔量初始化
    shmPtr->wtofile.b_finish_wtofile = true;//各进程退出命令，true为退出
    shmPtr->wtolcd.b_finish_wtolcd = true;
    shmPtr->deal.b_finish_deal = true;

    return 0;//成功进行共享内存创建并初始化
}

//此调用会删除共享内存，需要各共享内存映射都解除(或者保证不再使用，否则出错)
int shm_destroy(void)
{
    sem_destroy(&shmPtr->shmSem);//销毁无名信号量
    shmctl(IshmId, IPC_RMID, 0);//删除共享内存映射区
    return 0;
}

//直接创建各种进程，各进程创建后进入休眠状态。
int process_create(void)
{
//-------------创建进程-------------------
    if(fork()==0)//写入子进程,初始化后进入睡眠状态
    {
        execl("/opt/designed/tower", "/opt/designed/tower", NULL);//启动共享资源写入进程
        perror("execl tower error");
    }
    
    if(fork()==0)//写入子进程,初始化后进入睡眠状态
    {
        execl("/opt/designed/input", "/opt/designed/input", NULL);//启动共享资源写入进程
        perror("execl input error");
    }

    if(fork()==0)//缓冲区数据显示到lcd
    {
        execl("/opt/designed/wtofile", "/opt/designed/wtofile", NULL);//启动共享资源写入进程
        perror("execl wtofile error");
    }

    if(fork()==0)//图像处理进程
    {
        execl("/opt/designed/deal", "/opt/designed/deal", NULL);//启动共享资源写入进程
        perror("execl deal error");
    }

    return 0;
}

//修改结束进程的标志位，并发出信号量等待各进程结束。
int process_destroy( void )
{
    sem_wait(&shmPtr->shmSem);//修改共享内存内容前获得信号量
    shmPtr->b_endflag = true;//置共享内存处的退出标志为为真，各进程读取后退出当前进程。

    //给各进程信号量，唤醒休眠进程
    sem_post(&shmPtr->input.sem_input_wakeup);//开始video更新
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//开始video更新
//    sem_post(&shmPtr->wtolcd.sem_wtolcd_wakeup);//开始video更新
    sem_post(&shmPtr->deal.sem_deal_wakeup);//开始video更新
    sem_post(&shmPtr->tower.sem_tower_wakeup);//开始video更新
    //给各进程信号量，唤醒休眠进程
    sem_post(&shmPtr->wtofile.sem_wtofile_standby);//开始video更新
    sem_post(&shmPtr->wtolcd.sem_wtolcd_standby);//开始video更新
    sem_post(&shmPtr->deal.sem_deal_standby);//开始video更新

    sem_post(&shmPtr->shmSem);//释放信号量

    int status;

    while(waitpid(-1, &status, 0) != -1);//一直等待其产生的子进程都结束,每一次等待都是休眠

    printf("all process exit\n");

    return 0;
}


using namespace std;

ImageIden::ImageIden(QWidget *parent):
	QMainWindow(parent),
	ui(new Ui::ImageIden), //Ui namespace ,not this 
	im(new TQInputMethod),
	m_getImg(new QImage), timer1(new QTimer), timer2(new QTimer)
{
	ui->setupUi(this);
	
    printf("prepare to fork process\n");
    shm_init();//初始化全局共享内存指针。
    process_create();//创建各需要的进程，必须在上一个共享内存初始化后进行！
	
    //input
	QWSServer::setCurrentInputMethod(im);
	((TQInputMethod*)im)->setVisible(false);

	//signal and slots
	connect(ui->actionFromFile, SIGNAL(triggered()), this, SLOT(loadPicture()));
	connect(ui->buttonQuit, SIGNAL(clicked()), this, SLOT(buttonQuit()));

	connect(ui->btUp, SIGNAL(clicked()), this, SLOT(btUpPushed()));	
	connect(ui->btDown, SIGNAL(clicked()), this, SLOT(btDownPushed()));	
	connect(ui->btLeft, SIGNAL(clicked()), this, SLOT(btLeftPushed()));	
	connect(ui->btRight, SIGNAL(clicked()), this, SLOT(btRightPushed()));	

	connect(ui->btUp, SIGNAL(pressed()), this, SLOT(startPushPoll()));	
	connect(ui->btDown, SIGNAL(pressed()), this, SLOT(startPushPoll()));	
	connect(ui->btLeft, SIGNAL(pressed()), this, SLOT(startPushPoll()));	
	connect(ui->btRight, SIGNAL(pressed()), this, SLOT(startPushPoll()));	

	connect(ui->btUp, SIGNAL(released()), this, SLOT(stopPushPoll()));	
	connect(ui->btDown, SIGNAL(released()), this, SLOT(stopPushPoll()));	
	connect(ui->btLeft, SIGNAL(released()), this, SLOT(stopPushPoll()));	
	connect(ui->btRight, SIGNAL(released()), this, SLOT(stopPushPoll()));	

	connect(ui->btSave, SIGNAL(clicked()), this, SLOT(btSavePushed()));	
	connect(ui->btPhoto, SIGNAL(clicked()), this, SLOT(btPhotoPushed()));	
	connect(ui->btNextPic, SIGNAL(clicked()), this, SLOT(btNextPicPushed()));	
	connect(ui->btPrePic, SIGNAL(clicked()), this, SLOT(btPrePicPushed()));	
	connect(ui->numberSBox, SIGNAL(valueChanged(int)), this, SLOT(enableSaveButton(int)));	
//输入文本框数值变化
    connect(ui->horiValueSBox, SIGNAL(editingFinished()), this, SLOT(horiSBoxInput()));//发出进程命令
    connect(this, SIGNAL(horiAngleChange(int)), ui->horiValueSBox, SLOT(setValue(int)));//发出进程命令
	connect(ui->vertValueSBox, SIGNAL(editingFinished()), this, SLOT(vertSBoxInput()));
    connect(this, SIGNAL(vertAngleChange(int)), ui->vertValueSBox, SLOT(setValue(int)));//发出进程命令
    
//滑块等信号变化
    connect(ui->horiCtlDial, SIGNAL(valueChanged(int)), this, SLOT(horiAngleSet(int)));//发出进程命令
    connect(this, SIGNAL(horiAngleChange(int)), ui->horiCtlDial, SLOT(setValue(int)));//发出进程命令
	connect(ui->vertCtlSlider, SIGNAL(valueChanged(int)), this, SLOT(vertAngleSet(int)));
	connect(this, SIGNAL(vertAngleChange(int)), ui->vertCtlSlider, SLOT(setValue(int)));

	connect(ui->rbRefrashImg, SIGNAL(toggled(bool)), this, SLOT(setRefrashImage(bool)));	
	connect(ui->grayBox, SIGNAL(toggled(bool)), this, SLOT(setGrayImage(bool)));	

	connect(timer1, SIGNAL(timeout()), this, SLOT(doWhenTimeout1()));
//人脸检测的槽函数连接
	connect(ui->faceDetectLoadBt, SIGNAL(clicked()), this, SLOT(faceLoadPushed()));	
	connect(ui->faceDetectBt, SIGNAL(clicked()), this, SLOT(faceDetectPushed()));	
//定时器初始化    
    timer1->setSingleShot(false); //多次触发
	
	timer2->setSingleShot(false); //多次触发

	//设置背景图
	QRect screen_size = QApplication::desktop()->screenGeometry(); //get window size
	QPixmap pix("/opt/designed/background_1.jpg", 0, Qt::AutoColor);
	pix = pix.scaled(screen_size.width(), screen_size.height(), Qt::IgnoreAspectRatio); //photo size
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);

//退出按钮背景设置
    ui->buttonQuit->setText("");
    ui->buttonQuit->setFixedSize(ui->buttonQuit->size());
    ui->buttonQuit->setIconSize(ui->buttonQuit->size());
    pix.load("/opt/designed/image/shutdown.png", 0, Qt::AutoColor);
    pix = pix.scaled(ui->buttonQuit->size(), Qt::IgnoreAspectRatio);
    ui->buttonQuit->setIcon(QIcon(pix));

//上下翻按钮背景
    ui->btNextPic->setFlat(true);//设置按键透明
    ui->btPrePic->setFlat(true);

//设置保存图片的默认参数
    ui->leName->setText("hello");
    ui->delaySBox->setValue(1);//默认间隔1s
    ui->numberSBox->setValue(1);//默认采集个数
    ui->btSave->setEnabled(false);//默认不启动
    ui->saveProgressBar->setVisible(false);//默认进度不可见
//舵机控制部分初始化
    horiAngleSet(89);//调用自己的成员函数进行初始化
    horiAngleSet(90);//先初始化为89再初始化为90确保，绝对的同步．
    vertAngleSet(89);
    vertAngleSet(90);

    ui->horiValueSBox->setRange(0, 180);//设置角度有效值在０～１８０
    ui->vertValueSBox->setRange(0, 180);//设置角度有效值在０～１８０
    ui->horiCtlDial->setRange(0, 180);//设置角度有效值在０～１８０
    ui->vertCtlSlider->setRange(0, 180);//设置角度有效值在０～１８０

//初始化显示图片链表
    updateRecording();
    ui->displayNameLb->setText("\345\210\235\345\247\213\345\233\276");//只显示图片名字

//初始化主显示框    
    loadPicName = "/opt/designed/image/src_image.jpg";
    ui->displayNameLb->setAlignment(Qt::AlignHCenter);//设置水平居中

	m_getImg->load(loadPicName);
	*m_getImg = m_getImg->scaled(ui->labelPicture->size(), Qt::IgnoreAspectRatio); //photo size
	ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));

//初始化数字识别显示框
	m_getImg->load("/opt/designed/image/numberDetect.bmp");
	*m_getImg = m_getImg->scaled(ui->numberImage->size(), Qt::IgnoreAspectRatio); //photo size
	ui->numberImage->setPixmap(QPixmap::fromImage(*m_getImg));

    setWindowState(Qt::WindowFullScreen);//窗口最大化
    ui->titleLabel->setAlignment(Qt::AlignHCenter);//设置水平居中
}

ImageIden::~ImageIden()
{
    process_destroy();//退出前结束各进程
    shm_destroy();//进程退出后删除共享内存关系
    
	delete ui;
	delete im;
	delete m_getImg;
	delete timer1;
	delete timer2;
}

void ImageIden::updateRecording()
{
    FILE * file;
    char name[100];
    QString commen;
    QString temp;

    commen.sprintf("find /opt/designed/image/save/ -name '%s*' > /opt/designed/image/filelist",
                    qPrintable(ui->leName->text()));

    system(qPrintable(commen));//执行列表命令

    if((file = fopen("/opt/designed/image/filelist", "rb"))==NULL)
    {
        printf("open filelist error!\n");
        exit(0);
    }

    recordingSave.clear();//清空

    while(fgets(name, 80, file))
    {
        temp=name;
        temp=temp.trimmed();//处理"\n"
        temp=temp.section('/',-1);//只保存名字
        recordingSave.append(temp);
    }

    recordingSave.sort();//进行排序

    showIndex = 0;

    fclose(file);

    system("rm /opt/designed/image/filelist");
}

void ImageIden::loadPicture()
{
	loadPicName = QFileDialog::getOpenFileName(this, trUtf8("选择图像"), "/opt/designed/image/",
					tr("Images(*.png *.bmp *.jpg *.tif *.GIF)"));
	if (loadPicName.isEmpty())
		return;
	else
	{
		if ( !( m_getImg->load(loadPicName) ) )
		{
			QMessageBox::information(this,
					tr("Open img error"),
					tr("Open img error!"));
			return;
		}
		
        ui->rbRefrashImg->setChecked(false);//关闭更新按钮．
        if (timer1->isActive())
			timer1->stop();

        *m_getImg = m_getImg->scaled(ui->labelPicture->size(), Qt::IgnoreAspectRatio); //photo size
		ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));
	}
}

void ImageIden::buttonQuit()
{
    switch(QMessageBox::warning(this, 
                                "",
                                "\350\255\246\345\221\212",//＂警告＂的编码
                                //＂确定重启？＂的编码
                                // "\347\241\256\345\256\232\351\207\215\345\220\257\357\274\237", 
                                "\345\205\263\346\234\272",//关机 
                                "\351\207\215\345\220\257",//取消 
                                "\345\217\226\346\266\210"))//重启
    {
        case 0:
                cout<<"Shutdown"<<endl;
    	        emit returned();
	            close();            
                break;
        case 1:
                cout<<"Reboot"<<endl;
                system("reboot");//shell执行重启
                break;
        case 2:
                cout<<"Cancel"<<endl;
                break;
    }

    return;
}

void ImageIden::btPrePicPushed()
{
    showIndex = showIndex == (recordingSave.size()-1)? 0: showIndex+1;
    
    cout<<recordingSave.at(showIndex).toLocal8Bit().constData()<<endl;
    ui->rbRefrashImg->setChecked(false);//关闭更新按钮．

    if (timer1->isActive())
      timer1->stop();

    displayImage("/opt/designed/image/save/" + recordingSave.at(showIndex));
}

void ImageIden::btNextPicPushed()
{
    showIndex = showIndex == 0 ? recordingSave.size()-1: showIndex-1;
    
    cout<<recordingSave.at(showIndex).toLocal8Bit().constData()<<endl;
    ui->rbRefrashImg->setChecked(false);//关闭更新按钮．

    if (timer1->isActive())
      timer1->stop();

    displayImage("/opt/designed/image/save/" + recordingSave.at(showIndex));
}

///////////////////////////////Tower Ctrl/////////////////////////////////////////////////
void ImageIden::btUpPushed()
{
	cout << "up" << endl;
    int angle = shmPtr->tower.veri_angle;

    angle = (angle+CHANGE_STEP)>180? 180: angle+CHANGE_STEP;

    vertAngleSet(angle);//调用成员函数进行共享内存读写，若成功修改，则发出更新信号
}

void ImageIden::btDownPushed()
{
	cout << "down" << endl;
    int angle = shmPtr->tower.veri_angle;

    angle = (angle-CHANGE_STEP)<0? 0: angle-CHANGE_STEP;

    vertAngleSet(angle);//调用成员函数进行共享内存读写，若成功修改，则发出更新信号
}


void ImageIden::btLeftPushed()
{
	cout << "left" << endl;

    int angle = shmPtr->tower.hori_angle;

    angle = (angle-CHANGE_STEP)<0? 0: angle-CHANGE_STEP;

    horiAngleSet(angle);//调用成员函数进行共享内存读写，若成功修改，则发出更新信号
}

void ImageIden::btRightPushed()
{
  	cout << "right" << endl;

    int angle = shmPtr->tower.hori_angle;

    angle = (angle+CHANGE_STEP)>180? 180: angle+CHANGE_STEP;

    horiAngleSet(angle);//调用成员函数进行共享内存读写，若成功修改，则发出更新信号
}

void ImageIden::startPushPoll()
{//查询常按
  	cout << "long push poll start" << endl;

    connect(timer2, SIGNAL(timeout()), this, SLOT(longPushPoll()));
    timer2->start(100);//启动定时器进行按键状态轮询，时间越小，加减越快．	
}

void ImageIden::stopPushPoll()
{//查询常按
  	cout << "long push poll stop" << endl;

    disconnect(timer2, SIGNAL(timeout()), this, SLOT(longPushPoll()));
}

void ImageIden::longPushPoll()
{//查询常按
  	cout << "long pushing" << endl;

    if(ui->btUp->isDown())
    {
        btUpPushed();
    }
    else if(ui->btDown->isDown())
    {
        btDownPushed();
    }
    else if(ui->btLeft->isDown())
    {
        btLeftPushed();
    }
    else if(ui->btRight->isDown())
    {
        btRightPushed();
    }
}

void ImageIden::horiSBoxInput()
{
    horiAngleSet(ui->horiValueSBox->text().toInt());
}

void ImageIden::vertSBoxInput()
{
    vertAngleSet(ui->vertValueSBox->text().toInt());
}

void ImageIden::horiAngleSet( int newvalue )
{
    if( newvalue >= 0 && newvalue <= 180 && newvalue != (int)shmPtr->tower.hori_angle)
    {
        sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
        shmPtr->tower.hori_angle = newvalue;//增量为
    
        //唤醒进程
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);

        emit horiAngleChange(newvalue);//发出水平角度被修改的信号，各控件更新数值
    }
    else
        return;//直接返回不作角度修改
}

void ImageIden::vertAngleSet( int newvalue )
{
    if( newvalue >= 0 && newvalue <= 180 && newvalue != (int)shmPtr->tower.veri_angle)
    {
        sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
        shmPtr->tower.veri_angle = newvalue;//增量为
    
        //唤醒进程
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);

        emit vertAngleChange(newvalue);//发出竖直角度被修改的信号，各控件更新数值
    }
    else
        return;//直接返回不作角度修改
}

///////////////////////////////Photo Save////////////////////////////////
void ImageIden::enableSaveButton(int value)
{
    ui->btSave->setEnabled(value>1);//只有数量大于1才激活保存按钮
}

void ImageIden::btSavePushed()
{
	cout << "save" << endl;

    sem_wait(&shmPtr->shmSem);
    if(ui->leName->text().isEmpty())
        strcpy(shmPtr->wtofile.name, "hello");
    else
        strcpy(shmPtr->wtofile.name, qPrintable( ui->leName->text() ) );
    
    shmPtr->wtofile.count = ui->numberSBox->value();
    shmPtr->wtofile.haveSave = 0;
    shmPtr->wtofile.delay = ui->delaySBox->value();
    shmPtr->wtofile.b_wtofile_running = true;
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//开始video更新
    sem_post(&shmPtr->shmSem);
	
    //刷新期间禁止修改，直到完成后，由定时器激活
    ui->leName->setEnabled(false);//
    ui->numberSBox->setEnabled(false);//
    ui->delaySBox->setEnabled(false);
    ui->btSave->setEnabled(false);//

    connect(timer2, SIGNAL(timeout()), this, SLOT(saveProgressPoll()));
    timer2->start(500);//启动定时器进行刷新，更新进度条	
    ui->saveProgressBar->setRange(0, ui->numberSBox->value());
    ui->saveProgressBar->setVisible(true);//设置进度条可见
}

void ImageIden::btPhotoPushed()
{
	cout << "Photo" << endl;

    ui->rbRefrashImg->setChecked(false);//关闭更新按钮．

    if (timer1->isActive())
			timer1->stop();

    if(ui->grayBox->isChecked())
        system("cp -f /opt/designed/image/deal_image.jpg /opt/designed/image/photo.jpg");
    else
        system("cp -f /opt/designed/image/src_image.jpg /opt/designed/image/photo.jpg");

    displayImage("/opt/designed/image/photo.jpg");//成员函数调用

    ui->displayNameLb->setText("\346\213\215\347\205\247");//“拍照”
}

void ImageIden::saveProgressPoll()
{
    ui->saveProgressBar->setValue(shmPtr->wtofile.haveSave);

    if(shmPtr->wtofile.haveSave == shmPtr->wtofile.count)
    {

        updateRecording();//更新图片名列表

	    ui->leName->setEnabled(true);//
	    ui->numberSBox->setEnabled(true);//
        ui->delaySBox->setEnabled(true);
        ui->btSave->setEnabled(true);//

        ui->saveProgressBar->setVisible(false);//默认进度不可见
        ui->saveProgressBar->hide();//默认进度不可见，两种设置均无效...
        timer2->stop();

	    disconnect(timer2, SIGNAL(timeout()), this, SLOT(saveProgressPoll()));
    }
	//延时保存图片定时器触发
}

///////////////////////////////Main Display////////////////////////////////
void ImageIden::displayImage(const QString & fileName)
{
    if (fileName.isEmpty())
		return;
	else
	{
		if ( !( m_getImg->load(fileName) ) )
		{
			QMessageBox::information(this,
					tr("Open img error"),
					tr("Open img error!"));
			return;
		}
		
        ui->rbRefrashImg->setChecked(false);//关闭更新按钮．
        if (timer1->isActive())
			timer1->stop();

        loadPicName = fileName;
        ui->displayNameLb->setText("\346\234\254\345\234\260:"+fileName.section('/', -1));//只显示图片名字
        
        *m_getImg = m_getImg->scaled(ui->labelPicture->size(), Qt::IgnoreAspectRatio); //photo size
		ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));
	}
}

void ImageIden::setGrayImage(bool checked)
{
	if (checked)
	{
		cout << "Gray deal" << endl;
        ui->displayNameLb->setText("\345\244\204\347\220\206\345\233\276\345\203\217");//“处理图像”

        sem_wait(&shmPtr->shmSem);
        shmPtr->deal.b_deal_running = true;//允许图像处理进程运行
        sem_post(&shmPtr->deal.sem_deal_wakeup);
        sem_post(&shmPtr->shmSem);
    }
	else
    {
        cout << "No gray deal" << endl;
        sem_wait(&shmPtr->shmSem);
        shmPtr->deal.b_deal_running = false;//允许图像处理进程运行
        //sem_post(&shmPtr->deal.sem_deal_wakeup);
        sem_post(&shmPtr->shmSem);
    }
}

void ImageIden::setRefrashImage(bool checked)
{
	if (checked)
	{
		cout << "checked" << endl;
        ui->displayNameLb->setText("\345\256\236\346\227\266\345\233\276\345\203\217");//“实时图像”

		if ( !(timer1->isActive()) )
		{
		        timer1->start(50);	
		}
	}
	else
		if (timer1->isActive())
			timer1->stop();
}

void ImageIden::doWhenTimeout1()
{
	cout << "Refrash image" << endl;	
	//刷新图片

    if(ui->grayBox->isChecked())
    {
        //shmPtr->deal.b_need_to_show = true; 
        //sem_wait(&shmPtr->deal.sem_deal_finish);
        //忙等待会使程序卡死.
        //while(!shmPtr->deal.b_finish_deal);//忙等待一张图像处理完毕
        //m_getImg->load("/opt/designed/image/deal_image.jpg");
        loadPicName = "/opt/designed/image/deal_image.jpg";
    
        //shmPtr->deal.b_need_to_show = false; 
    }
    else
	    loadPicName = "/opt/designed/image/src_image.jpg";
        
    m_getImg->load(loadPicName);
    *m_getImg = m_getImg->scaled(ui->labelPicture->size(), Qt::IgnoreAspectRatio); //photo size
	ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));
}

//////////////////////////////////////Face Detect////////////////////////////////
void ImageIden::faceLoadPushed()
{
    QString filename;

	filename = QFileDialog::getOpenFileName(this, trUtf8("选择图像"), "/opt/designed/image/",
					tr("Images(*.png *.bmp *.jpg *.tif *.GIF)"));

    displayImage(filename);
}

void ImageIden::faceDetectPushed()
{
    char *commen_ptr=NULL;
    
    ui->faceDetectBt->setEnabled(false);

    commen_ptr = (char *)malloc(strlen(qPrintable(loadPicName)) + strlen("/opt/designed/facedetect ") + 1);

    strcpy(commen_ptr, "/opt/designed/facedetect ");
    strcat(commen_ptr, qPrintable(loadPicName));
    printf("%s\n", commen_ptr);

//system调用方法，但实际也是fork
    system(commen_ptr);//运行单帧人脸检测，检测结果存放在facedetect.jpg图像中

    free(commen_ptr);
    commen_ptr = NULL;
    
    m_getImg->load("/opt/designed/image/facedetect.jpg");
    *m_getImg = m_getImg->scaled(ui->faceDetectImage->size(), Qt::IgnoreAspectRatio); //photo size
	ui->faceDetectImage->setPixmap(QPixmap::fromImage(*m_getImg));
    
    ui->faceDetectBt->setEnabled(true);
}

/////////////////////////////////////////////END/////////////////////////////////////
