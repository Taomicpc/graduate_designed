/****************************************************************************
 ** object : ImageIden 
 ** ��ҵ��������˵���ͼ��ʶ�� - ht
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

#define CHANGE_STEP 2//�����ı����ǶȵĲ���
int IshmId;
shmType* shmPtr = NULL;//���ù����ڴ��ȫ�ֱ���ָ��
const QString SAVE_ROOT="/opt/designed/image/save/";

//��ʼ��ȫ�ֱ���shmPtr�����������̵��ڴ����ӣ������г�ʼ�����á�
//���0Ϊ�ɹ���ʼ����1��2��3�ֱ��Ӧ��ȡipc�Ŵ���,�����ڴ�����ʧ�ܣ������ڴ�ӳ��ʧ��
int shm_init(void)
{
//--------------�����ڴ潨������--------------------
    key_t ipcKey;

    ipcKey = ftok("/opt/designed/shm", 'a');
    if(ipcKey == -1)
    {
        perror("ftok error");
        return 1;//����ʧ��
    }

    IshmId = shmget(ipcKey, sizeof(shmType),S_IRUSR | S_IWUSR | IPC_CREAT | 0777);//0666��ubuntu�´˲�����Ҫ��У��

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

//�����ڴ�����ʼ��
    memset(shmPtr, 0, sizeof(shmType));
    shmPtr->tower.hori_angle = 90;
    shmPtr->tower.veri_angle = 90;//�Ƕ��źų�ʼ��
    shmPtr->wtofile.delay = 1;//Ĭ�ϳ�ʼ��Ϊ1s
    //ǰһ��1�����ڽ��̼�ʹ�ã���һ��1����һ���ź�����ʼֵ��1���źſ��У������ź�æµ
    sem_init(&shmPtr->shmSem, 1, 1);

//���ո������������е��ź�������������źţ����̽�����˯��״̬��
    //���ź���æµ�������̼�ʹ������Ҳ�����˯��״̬��
    sem_init(&shmPtr->input.sem_input_wakeup, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_wakeup, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_wakeup, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_wakeup, 1, 0);
    //sem_init(&shmPtr->deal.sem_deal_finish, 1, 0);
    sem_init(&shmPtr->tower.sem_tower_wakeup, 1, 0);

    //�������ź������Ӧ�Ĳ�������ʼ��
    //���ñ�־Ϊfalse��ʹ�����̽�������wakeup�ź�����˯��״̬
    shmPtr->input.b_input_running = true;//����imageͼ��Ĭ��һֱ���� 
    sem_post(&shmPtr->input.sem_input_wakeup);//��ʼvideo����
    
    shmPtr->wtofile.b_wtofile_running = false; 
    shmPtr->wtolcd.b_wtolcd_running = false; 
    shmPtr->deal.b_deal_running = false; 
    //shmPtr->deal.b_need_to_show = false; 
    shmPtr->tower.b_tower_running = false; 

//�ź�����ʼ��Ϊæµ������image.jpg��д���ź�ͬ����
    sem_init(&shmPtr->input.sem_wr_enable, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_standby, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_standby, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_standby, 1, 0);
    //��֮���Ӧ��������ʼ��
    shmPtr->wtofile.b_finish_wtofile = true;//�������˳����trueΪ�˳�
    shmPtr->wtolcd.b_finish_wtolcd = true;
    shmPtr->deal.b_finish_deal = true;

    return 0;//�ɹ����й����ڴ洴������ʼ��
}

//�˵��û�ɾ�������ڴ棬��Ҫ�������ڴ�ӳ�䶼���(���߱�֤����ʹ�ã��������)
int shm_destroy(void)
{
    sem_destroy(&shmPtr->shmSem);//���������ź���
    shmctl(IshmId, IPC_RMID, 0);//ɾ�������ڴ�ӳ����
    return 0;
}

//ֱ�Ӵ������ֽ��̣������̴������������״̬��
int process_create(void)
{
//-------------��������-------------------
    if(fork()==0)//д���ӽ���,��ʼ�������˯��״̬
    {
        execl("/opt/designed/tower", "/opt/designed/tower", NULL);//����������Դд�����
        perror("execl tower error");
    }
    
    if(fork()==0)//д���ӽ���,��ʼ�������˯��״̬
    {
        execl("/opt/designed/input", "/opt/designed/input", NULL);//����������Դд�����
        perror("execl input error");
    }

    if(fork()==0)//������������ʾ��lcd
    {
        execl("/opt/designed/wtofile", "/opt/designed/wtofile", NULL);//����������Դд�����
        perror("execl wtofile error");
    }

    if(fork()==0)//ͼ�������
    {
        execl("/opt/designed/deal", "/opt/designed/deal", NULL);//����������Դд�����
        perror("execl deal error");
    }

    return 0;
}

//�޸Ľ������̵ı�־λ���������ź����ȴ������̽�����
int process_destroy( void )
{
    sem_wait(&shmPtr->shmSem);//�޸Ĺ����ڴ�����ǰ����ź���
    shmPtr->b_endflag = true;//�ù����ڴ洦���˳���־ΪΪ�棬�����̶�ȡ���˳���ǰ���̡�

    //���������ź������������߽���
    sem_post(&shmPtr->input.sem_input_wakeup);//��ʼvideo����
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//��ʼvideo����
//    sem_post(&shmPtr->wtolcd.sem_wtolcd_wakeup);//��ʼvideo����
    sem_post(&shmPtr->deal.sem_deal_wakeup);//��ʼvideo����
    sem_post(&shmPtr->tower.sem_tower_wakeup);//��ʼvideo����
    //���������ź������������߽���
    sem_post(&shmPtr->wtofile.sem_wtofile_standby);//��ʼvideo����
    sem_post(&shmPtr->wtolcd.sem_wtolcd_standby);//��ʼvideo����
    sem_post(&shmPtr->deal.sem_deal_standby);//��ʼvideo����

    sem_post(&shmPtr->shmSem);//�ͷ��ź���

    int status;

    while(waitpid(-1, &status, 0) != -1);//һֱ�ȴ���������ӽ��̶�����,ÿһ�εȴ���������

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
    shm_init();//��ʼ��ȫ�ֹ����ڴ�ָ�롣
    process_create();//��������Ҫ�Ľ��̣���������һ�������ڴ��ʼ������У�
	
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
//�����ı�����ֵ�仯
    connect(ui->horiValueSBox, SIGNAL(editingFinished()), this, SLOT(horiSBoxInput()));//������������
    connect(this, SIGNAL(horiAngleChange(int)), ui->horiValueSBox, SLOT(setValue(int)));//������������
	connect(ui->vertValueSBox, SIGNAL(editingFinished()), this, SLOT(vertSBoxInput()));
    connect(this, SIGNAL(vertAngleChange(int)), ui->vertValueSBox, SLOT(setValue(int)));//������������
    
//������źű仯
    connect(ui->horiCtlDial, SIGNAL(valueChanged(int)), this, SLOT(horiAngleSet(int)));//������������
    connect(this, SIGNAL(horiAngleChange(int)), ui->horiCtlDial, SLOT(setValue(int)));//������������
	connect(ui->vertCtlSlider, SIGNAL(valueChanged(int)), this, SLOT(vertAngleSet(int)));
	connect(this, SIGNAL(vertAngleChange(int)), ui->vertCtlSlider, SLOT(setValue(int)));

	connect(ui->rbRefrashImg, SIGNAL(toggled(bool)), this, SLOT(setRefrashImage(bool)));	
	connect(ui->grayBox, SIGNAL(toggled(bool)), this, SLOT(setGrayImage(bool)));	

	connect(timer1, SIGNAL(timeout()), this, SLOT(doWhenTimeout1()));
//�������Ĳۺ�������
	connect(ui->faceDetectLoadBt, SIGNAL(clicked()), this, SLOT(faceLoadPushed()));	
	connect(ui->faceDetectBt, SIGNAL(clicked()), this, SLOT(faceDetectPushed()));	
//��ʱ����ʼ��    
    timer1->setSingleShot(false); //��δ���
	
	timer2->setSingleShot(false); //��δ���

	//���ñ���ͼ
	QRect screen_size = QApplication::desktop()->screenGeometry(); //get window size
	QPixmap pix("/opt/designed/background_1.jpg", 0, Qt::AutoColor);
	pix = pix.scaled(screen_size.width(), screen_size.height(), Qt::IgnoreAspectRatio); //photo size
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);

//�˳���ť��������
    ui->buttonQuit->setText("");
    ui->buttonQuit->setFixedSize(ui->buttonQuit->size());
    ui->buttonQuit->setIconSize(ui->buttonQuit->size());
    pix.load("/opt/designed/image/shutdown.png", 0, Qt::AutoColor);
    pix = pix.scaled(ui->buttonQuit->size(), Qt::IgnoreAspectRatio);
    ui->buttonQuit->setIcon(QIcon(pix));

//���·���ť����
    ui->btNextPic->setFlat(true);//���ð���͸��
    ui->btPrePic->setFlat(true);

//���ñ���ͼƬ��Ĭ�ϲ���
    ui->leName->setText("hello");
    ui->delaySBox->setValue(1);//Ĭ�ϼ��1s
    ui->numberSBox->setValue(1);//Ĭ�ϲɼ�����
    ui->btSave->setEnabled(false);//Ĭ�ϲ�����
    ui->saveProgressBar->setVisible(false);//Ĭ�Ͻ��Ȳ��ɼ�
//������Ʋ��ֳ�ʼ��
    horiAngleSet(89);//�����Լ��ĳ�Ա�������г�ʼ��
    horiAngleSet(90);//�ȳ�ʼ��Ϊ89�ٳ�ʼ��Ϊ90ȷ�������Ե�ͬ����
    vertAngleSet(89);
    vertAngleSet(90);

    ui->horiValueSBox->setRange(0, 180);//���ýǶ���Чֵ�ڣ���������
    ui->vertValueSBox->setRange(0, 180);//���ýǶ���Чֵ�ڣ���������
    ui->horiCtlDial->setRange(0, 180);//���ýǶ���Чֵ�ڣ���������
    ui->vertCtlSlider->setRange(0, 180);//���ýǶ���Чֵ�ڣ���������

//��ʼ����ʾͼƬ����
    updateRecording();
    ui->displayNameLb->setText("\345\210\235\345\247\213\345\233\276");//ֻ��ʾͼƬ����

//��ʼ������ʾ��    
    loadPicName = "/opt/designed/image/src_image.jpg";
    ui->displayNameLb->setAlignment(Qt::AlignHCenter);//����ˮƽ����

	m_getImg->load(loadPicName);
	*m_getImg = m_getImg->scaled(ui->labelPicture->size(), Qt::IgnoreAspectRatio); //photo size
	ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));

//��ʼ������ʶ����ʾ��
	m_getImg->load("/opt/designed/image/numberDetect.bmp");
	*m_getImg = m_getImg->scaled(ui->numberImage->size(), Qt::IgnoreAspectRatio); //photo size
	ui->numberImage->setPixmap(QPixmap::fromImage(*m_getImg));

    setWindowState(Qt::WindowFullScreen);//�������
    ui->titleLabel->setAlignment(Qt::AlignHCenter);//����ˮƽ����
}

ImageIden::~ImageIden()
{
    process_destroy();//�˳�ǰ����������
    shm_destroy();//�����˳���ɾ�������ڴ��ϵ
    
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

    system(qPrintable(commen));//ִ���б�����

    if((file = fopen("/opt/designed/image/filelist", "rb"))==NULL)
    {
        printf("open filelist error!\n");
        exit(0);
    }

    recordingSave.clear();//���

    while(fgets(name, 80, file))
    {
        temp=name;
        temp=temp.trimmed();//����"\n"
        temp=temp.section('/',-1);//ֻ��������
        recordingSave.append(temp);
    }

    recordingSave.sort();//��������

    showIndex = 0;

    fclose(file);

    system("rm /opt/designed/image/filelist");
}

void ImageIden::loadPicture()
{
	loadPicName = QFileDialog::getOpenFileName(this, trUtf8("ѡ��ͼ��"), "/opt/designed/image/",
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
		
        ui->rbRefrashImg->setChecked(false);//�رո��°�ť��
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
                                "\350\255\246\345\221\212",//�����棢�ı���
                                //��ȷ�����������ı���
                                // "\347\241\256\345\256\232\351\207\215\345\220\257\357\274\237", 
                                "\345\205\263\346\234\272",//�ػ� 
                                "\351\207\215\345\220\257",//ȡ�� 
                                "\345\217\226\346\266\210"))//����
    {
        case 0:
                cout<<"Shutdown"<<endl;
    	        emit returned();
	            close();            
                break;
        case 1:
                cout<<"Reboot"<<endl;
                system("reboot");//shellִ������
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
    ui->rbRefrashImg->setChecked(false);//�رո��°�ť��

    if (timer1->isActive())
      timer1->stop();

    displayImage("/opt/designed/image/save/" + recordingSave.at(showIndex));
}

void ImageIden::btNextPicPushed()
{
    showIndex = showIndex == 0 ? recordingSave.size()-1: showIndex-1;
    
    cout<<recordingSave.at(showIndex).toLocal8Bit().constData()<<endl;
    ui->rbRefrashImg->setChecked(false);//�رո��°�ť��

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

    vertAngleSet(angle);//���ó�Ա�������й����ڴ��д�����ɹ��޸ģ��򷢳������ź�
}

void ImageIden::btDownPushed()
{
	cout << "down" << endl;
    int angle = shmPtr->tower.veri_angle;

    angle = (angle-CHANGE_STEP)<0? 0: angle-CHANGE_STEP;

    vertAngleSet(angle);//���ó�Ա�������й����ڴ��д�����ɹ��޸ģ��򷢳������ź�
}


void ImageIden::btLeftPushed()
{
	cout << "left" << endl;

    int angle = shmPtr->tower.hori_angle;

    angle = (angle-CHANGE_STEP)<0? 0: angle-CHANGE_STEP;

    horiAngleSet(angle);//���ó�Ա�������й����ڴ��д�����ɹ��޸ģ��򷢳������ź�
}

void ImageIden::btRightPushed()
{
  	cout << "right" << endl;

    int angle = shmPtr->tower.hori_angle;

    angle = (angle+CHANGE_STEP)>180? 180: angle+CHANGE_STEP;

    horiAngleSet(angle);//���ó�Ա�������й����ڴ��д�����ɹ��޸ģ��򷢳������ź�
}

void ImageIden::startPushPoll()
{//��ѯ����
  	cout << "long push poll start" << endl;

    connect(timer2, SIGNAL(timeout()), this, SLOT(longPushPoll()));
    timer2->start(100);//������ʱ�����а���״̬��ѯ��ʱ��ԽС���Ӽ�Խ�죮	
}

void ImageIden::stopPushPoll()
{//��ѯ����
  	cout << "long push poll stop" << endl;

    disconnect(timer2, SIGNAL(timeout()), this, SLOT(longPushPoll()));
}

void ImageIden::longPushPoll()
{//��ѯ����
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
        sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
        shmPtr->tower.hori_angle = newvalue;//����Ϊ
    
        //���ѽ���
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);

        emit horiAngleChange(newvalue);//����ˮƽ�Ƕȱ��޸ĵ��źţ����ؼ�������ֵ
    }
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�
}

void ImageIden::vertAngleSet( int newvalue )
{
    if( newvalue >= 0 && newvalue <= 180 && newvalue != (int)shmPtr->tower.veri_angle)
    {
        sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
        shmPtr->tower.veri_angle = newvalue;//����Ϊ
    
        //���ѽ���
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);

        emit vertAngleChange(newvalue);//������ֱ�Ƕȱ��޸ĵ��źţ����ؼ�������ֵ
    }
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�
}

///////////////////////////////Photo Save////////////////////////////////
void ImageIden::enableSaveButton(int value)
{
    ui->btSave->setEnabled(value>1);//ֻ����������1�ż���水ť
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
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//��ʼvideo����
    sem_post(&shmPtr->shmSem);
	
    //ˢ���ڼ��ֹ�޸ģ�ֱ����ɺ��ɶ�ʱ������
    ui->leName->setEnabled(false);//
    ui->numberSBox->setEnabled(false);//
    ui->delaySBox->setEnabled(false);
    ui->btSave->setEnabled(false);//

    connect(timer2, SIGNAL(timeout()), this, SLOT(saveProgressPoll()));
    timer2->start(500);//������ʱ������ˢ�£����½�����	
    ui->saveProgressBar->setRange(0, ui->numberSBox->value());
    ui->saveProgressBar->setVisible(true);//���ý������ɼ�
}

void ImageIden::btPhotoPushed()
{
	cout << "Photo" << endl;

    ui->rbRefrashImg->setChecked(false);//�رո��°�ť��

    if (timer1->isActive())
			timer1->stop();

    if(ui->grayBox->isChecked())
        system("cp -f /opt/designed/image/deal_image.jpg /opt/designed/image/photo.jpg");
    else
        system("cp -f /opt/designed/image/src_image.jpg /opt/designed/image/photo.jpg");

    displayImage("/opt/designed/image/photo.jpg");//��Ա��������

    ui->displayNameLb->setText("\346\213\215\347\205\247");//�����ա�
}

void ImageIden::saveProgressPoll()
{
    ui->saveProgressBar->setValue(shmPtr->wtofile.haveSave);

    if(shmPtr->wtofile.haveSave == shmPtr->wtofile.count)
    {

        updateRecording();//����ͼƬ���б�

	    ui->leName->setEnabled(true);//
	    ui->numberSBox->setEnabled(true);//
        ui->delaySBox->setEnabled(true);
        ui->btSave->setEnabled(true);//

        ui->saveProgressBar->setVisible(false);//Ĭ�Ͻ��Ȳ��ɼ�
        ui->saveProgressBar->hide();//Ĭ�Ͻ��Ȳ��ɼ����������þ���Ч...
        timer2->stop();

	    disconnect(timer2, SIGNAL(timeout()), this, SLOT(saveProgressPoll()));
    }
	//��ʱ����ͼƬ��ʱ������
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
		
        ui->rbRefrashImg->setChecked(false);//�رո��°�ť��
        if (timer1->isActive())
			timer1->stop();

        loadPicName = fileName;
        ui->displayNameLb->setText("\346\234\254\345\234\260:"+fileName.section('/', -1));//ֻ��ʾͼƬ����
        
        *m_getImg = m_getImg->scaled(ui->labelPicture->size(), Qt::IgnoreAspectRatio); //photo size
		ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));
	}
}

void ImageIden::setGrayImage(bool checked)
{
	if (checked)
	{
		cout << "Gray deal" << endl;
        ui->displayNameLb->setText("\345\244\204\347\220\206\345\233\276\345\203\217");//������ͼ��

        sem_wait(&shmPtr->shmSem);
        shmPtr->deal.b_deal_running = true;//����ͼ�����������
        sem_post(&shmPtr->deal.sem_deal_wakeup);
        sem_post(&shmPtr->shmSem);
    }
	else
    {
        cout << "No gray deal" << endl;
        sem_wait(&shmPtr->shmSem);
        shmPtr->deal.b_deal_running = false;//����ͼ�����������
        //sem_post(&shmPtr->deal.sem_deal_wakeup);
        sem_post(&shmPtr->shmSem);
    }
}

void ImageIden::setRefrashImage(bool checked)
{
	if (checked)
	{
		cout << "checked" << endl;
        ui->displayNameLb->setText("\345\256\236\346\227\266\345\233\276\345\203\217");//��ʵʱͼ��

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
	//ˢ��ͼƬ

    if(ui->grayBox->isChecked())
    {
        //shmPtr->deal.b_need_to_show = true; 
        //sem_wait(&shmPtr->deal.sem_deal_finish);
        //æ�ȴ���ʹ������.
        //while(!shmPtr->deal.b_finish_deal);//æ�ȴ�һ��ͼ�������
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

	filename = QFileDialog::getOpenFileName(this, trUtf8("ѡ��ͼ��"), "/opt/designed/image/",
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

//system���÷�������ʵ��Ҳ��fork
    system(commen_ptr);//���е�֡������⣬����������facedetect.jpgͼ����

    free(commen_ptr);
    commen_ptr = NULL;
    
    m_getImg->load("/opt/designed/image/facedetect.jpg");
    *m_getImg = m_getImg->scaled(ui->faceDetectImage->size(), Qt::IgnoreAspectRatio); //photo size
	ui->faceDetectImage->setPixmap(QPixmap::fromImage(*m_getImg));
    
    ui->faceDetectBt->setEnabled(true);
}

/////////////////////////////////////////////END/////////////////////////////////////
