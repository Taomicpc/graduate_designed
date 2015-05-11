/****************************************************************************
** object : ImageIden 
 ** 毕业设计三级菜单　图像识别 - ht
 ** by luchaodong
 ** class Ui::ImageIden : public Ui_ImageIden {}
 ** ImageIden 
 ****************************************************************************/
#ifndef IMAGEIDE_20150420_DEF
#define IMAGEIDE_20150420_DEF

#include "ui_imageIden.h"
#include "TQInputMethod.h"
#include <QWSInputMethod>
#include <QFileDialog>

extern "C"
{
    #include "public.h"
}

class ImageIden : public QMainWindow
{
	Q_OBJECT
public :
	explicit ImageIden(QWidget *parent = 0);
	~ImageIden();
	
public slots :
	void loadPicture();
	void buttonQuit();
	void buttonReboot();
	
	void btUpPushed();
	void btDownPushed();
	void btLeftPushed();
	void btRightPushed();
    void startPushPoll();//长按的开始和结束槽函数
    void stopPushPoll();

	void btSavePushed();
	void btPhotoPushed();
	void btPrePicPushed();
	void btNextPicPushed();
    void enableSaveButton(int value);

    void horiSBoxInput();
    void vertSBoxInput();

    //用
    void horiAngleSet(int);
    void vertAngleSet(int);

	void displayImage(const QString &);
	void setRefrashImage(bool checked);  
	void setGrayImage(bool checked);  

	void doWhenTimeout1(); //定时刷新图片
	void saveProgressPoll(); //progress poll
	void longPushPoll(); //progress poll

    void faceLoadPushed();
    void faceDetectPushed();

signals :
	void horiAngleChange(int);
    void vertAngleChange(int);
    void returned();

private:
	Ui::ImageIden *ui;
	QWSInputMethod *im;	
	QImage *m_getImg;  //load picture from file
	QTimer *timer1;
	QTimer *timer2;//定时器2动态挂载函数任务，主要用于定时轮询各种状态
	int m_timeoutCount;
    QString loadPicName;//记录labelPicture加载的图片名字
};

#endif 

