/********************************************************************************
** Form generated from reading ui file 'imageIden.ui'
**
** Created: Mon May 11 12:58:45 2015
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_IMAGEIDEN_H
#define UI_IMAGEIDEN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDial>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageIden
{
public:
    QAction *actionIden;
    QAction *actionZuoqu;
    QAction *actionXunliang;
    QAction *actionFromFile;
    QAction *actionCatch;
    QAction *actionTrain;
    QAction *actionFace;
    QAction *actionDigit;
    QAction *actionQuit;
    QAction *actionAa;
    QWidget *centralwidget;
    QPushButton *buttonQuit;
    QLabel *labelPicture;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *grayBox;
    QRadioButton *rbRefrashImg;
    QProgressBar *saveProgressBar;
    QPushButton *btPrePic;
    QPushButton *btNextPic;
    QLabel *label_7;
    QTabWidget *ctlTab;
    QWidget *photoInput;
    QGroupBox *groupBox;
    QPushButton *btUp;
    QPushButton *btLeft;
    QPushButton *btRight;
    QPushButton *btDown;
    QLabel *label_2;
    QLabel *label_6;
    QDial *horiCtlDial;
    QSlider *vertCtlSlider;
    QSpinBox *horiValueSBox;
    QSpinBox *vertValueSBox;
    QGroupBox *groupBox_2;
    QLineEdit *leName;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QPushButton *btSave;
    QPushButton *btPhoto;
    QSpinBox *numberSBox;
    QSpinBox *delaySBox;
    QWidget *photoDeal;
    QLabel *faceDetectImage;
    QPushButton *faceDetectLoadBt;
    QPushButton *faceDetectBt;
    QWidget *tab;
    QPushButton *rebootButton;
    QMenuBar *menubar;
    QMenu *menuFile;

    void setupUi(QMainWindow *ImageIden)
    {
        if (ImageIden->objectName().isEmpty())
            ImageIden->setObjectName(QString::fromUtf8("ImageIden"));
        ImageIden->resize(788, 480);
        ImageIden->setMaximumSize(QSize(800, 480));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(75);
        font.setStrikeOut(false);
        ImageIden->setFont(font);
        actionIden = new QAction(ImageIden);
        actionIden->setObjectName(QString::fromUtf8("actionIden"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        actionIden->setFont(font1);
        actionZuoqu = new QAction(ImageIden);
        actionZuoqu->setObjectName(QString::fromUtf8("actionZuoqu"));
        actionXunliang = new QAction(ImageIden);
        actionXunliang->setObjectName(QString::fromUtf8("actionXunliang"));
        actionFromFile = new QAction(ImageIden);
        actionFromFile->setObjectName(QString::fromUtf8("actionFromFile"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font2.setPointSize(11);
        actionFromFile->setFont(font2);
        actionCatch = new QAction(ImageIden);
        actionCatch->setObjectName(QString::fromUtf8("actionCatch"));
        actionCatch->setFont(font1);
        actionTrain = new QAction(ImageIden);
        actionTrain->setObjectName(QString::fromUtf8("actionTrain"));
        actionTrain->setFont(font1);
        actionFace = new QAction(ImageIden);
        actionFace->setObjectName(QString::fromUtf8("actionFace"));
        actionFace->setCheckable(true);
        actionFace->setChecked(true);
        actionFace->setFont(font1);
        actionDigit = new QAction(ImageIden);
        actionDigit->setObjectName(QString::fromUtf8("actionDigit"));
        actionDigit->setCheckable(true);
        actionDigit->setFont(font1);
        actionQuit = new QAction(ImageIden);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionQuit->setFont(font1);
        actionAa = new QAction(ImageIden);
        actionAa->setObjectName(QString::fromUtf8("actionAa"));
        centralwidget = new QWidget(ImageIden);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        buttonQuit = new QPushButton(centralwidget);
        buttonQuit->setObjectName(QString::fromUtf8("buttonQuit"));
        buttonQuit->setGeometry(QRect(710, 400, 71, 31));
        QFont font3;
        font3.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font3.setBold(true);
        font3.setWeight(75);
        buttonQuit->setFont(font3);
        labelPicture = new QLabel(centralwidget);
        labelPicture->setObjectName(QString::fromUtf8("labelPicture"));
        labelPicture->setGeometry(QRect(29, 90, 261, 286));
        QFont font4;
        font4.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font4.setBold(false);
        font4.setWeight(50);
        labelPicture->setFont(font4);
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(45, 390, 211, 29));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        grayBox = new QCheckBox(layoutWidget);
        grayBox->setObjectName(QString::fromUtf8("grayBox"));
        QFont font5;
        font5.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font5.setPointSize(10);
        grayBox->setFont(font5);

        horizontalLayout->addWidget(grayBox);

        rbRefrashImg = new QRadioButton(layoutWidget);
        rbRefrashImg->setObjectName(QString::fromUtf8("rbRefrashImg"));
        rbRefrashImg->setFont(font5);
        rbRefrashImg->setCheckable(true);

        horizontalLayout->addWidget(rbRefrashImg);

        saveProgressBar = new QProgressBar(centralwidget);
        saveProgressBar->setObjectName(QString::fromUtf8("saveProgressBar"));
        saveProgressBar->setGeometry(QRect(330, 395, 221, 31));
        saveProgressBar->setValue(24);
        saveProgressBar->setTextVisible(true);
        btPrePic = new QPushButton(centralwidget);
        btPrePic->setObjectName(QString::fromUtf8("btPrePic"));
        btPrePic->setGeometry(QRect(30, 325, 121, 51));
        btNextPic = new QPushButton(centralwidget);
        btNextPic->setObjectName(QString::fromUtf8("btNextPic"));
        btNextPic->setGeometry(QRect(150, 325, 121, 51));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 5, 431, 41));
        QFont font6;
        font6.setFamily(QString::fromUtf8("WenQuanYi Micro Hei Mono"));
        font6.setPointSize(16);
        font6.setBold(false);
        font6.setWeight(50);
        label_7->setFont(font6);
        ctlTab = new QTabWidget(centralwidget);
        ctlTab->setObjectName(QString::fromUtf8("ctlTab"));
        ctlTab->setGeometry(QRect(300, 50, 481, 341));
        QFont font7;
        font7.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font7.setPointSize(14);
        ctlTab->setFont(font7);
        photoInput = new QWidget();
        photoInput->setObjectName(QString::fromUtf8("photoInput"));
        groupBox = new QGroupBox(photoInput);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 0, 456, 171));
        QFont font8;
        font8.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font8.setPointSize(13);
        font8.setBold(false);
        font8.setWeight(50);
        groupBox->setFont(font8);
        btUp = new QPushButton(groupBox);
        btUp->setObjectName(QString::fromUtf8("btUp"));
        btUp->setGeometry(QRect(60, 50, 51, 51));
        QFont font9;
        font9.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font9.setPointSize(12);
        font9.setBold(false);
        font9.setWeight(50);
        btUp->setFont(font9);
        btLeft = new QPushButton(groupBox);
        btLeft->setObjectName(QString::fromUtf8("btLeft"));
        btLeft->setGeometry(QRect(10, 100, 51, 51));
        btLeft->setFont(font9);
        btRight = new QPushButton(groupBox);
        btRight->setObjectName(QString::fromUtf8("btRight"));
        btRight->setGeometry(QRect(110, 100, 51, 51));
        btRight->setFont(font9);
        btDown = new QPushButton(groupBox);
        btDown->setObjectName(QString::fromUtf8("btDown"));
        btDown->setGeometry(QRect(60, 100, 51, 51));
        btDown->setFont(font9);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(350, 10, 101, 31));
        QFont font10;
        font10.setPointSize(12);
        label_2->setFont(font10);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(200, 10, 111, 31));
        label_6->setFont(font10);
        horiCtlDial = new QDial(groupBox);
        horiCtlDial->setObjectName(QString::fromUtf8("horiCtlDial"));
        horiCtlDial->setGeometry(QRect(180, 70, 131, 101));
        horiCtlDial->setMinimumSize(QSize(131, 0));
        horiCtlDial->setSizeIncrement(QSize(0, 0));
        QFont font11;
        font11.setPointSize(15);
        horiCtlDial->setFont(font11);
        horiCtlDial->setCursor(QCursor(Qt::ArrowCursor));
        horiCtlDial->setMouseTracking(false);
        horiCtlDial->setOrientation(Qt::Horizontal);
        horiCtlDial->setInvertedAppearance(false);
        horiCtlDial->setInvertedControls(false);
        horiCtlDial->setWrapping(false);
        horiCtlDial->setNotchesVisible(true);
        vertCtlSlider = new QSlider(groupBox);
        vertCtlSlider->setObjectName(QString::fromUtf8("vertCtlSlider"));
        vertCtlSlider->setGeometry(QRect(380, 65, 16, 101));
        vertCtlSlider->setOrientation(Qt::Vertical);
        horiValueSBox = new QSpinBox(groupBox);
        horiValueSBox->setObjectName(QString::fromUtf8("horiValueSBox"));
        horiValueSBox->setGeometry(QRect(210, 40, 71, 21));
        horiValueSBox->setFont(font10);
        vertValueSBox = new QSpinBox(groupBox);
        vertValueSBox->setObjectName(QString::fromUtf8("vertValueSBox"));
        vertValueSBox->setGeometry(QRect(360, 40, 71, 21));
        vertValueSBox->setFont(font10);
        groupBox_2 = new QGroupBox(photoInput);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 170, 456, 121));
        groupBox_2->setFont(font8);
        leName = new QLineEdit(groupBox_2);
        leName->setObjectName(QString::fromUtf8("leName"));
        leName->setGeometry(QRect(100, 40, 91, 27));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(60, 40, 41, 31));
        label->setFont(font10);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(280, 30, 61, 31));
        label_3->setFont(font10);
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(280, 80, 61, 31));
        label_4->setFont(font10);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(410, 80, 41, 31));
        btSave = new QPushButton(groupBox_2);
        btSave->setObjectName(QString::fromUtf8("btSave"));
        btSave->setGeometry(QRect(140, 80, 100, 31));
        btSave->setFont(font4);
        btPhoto = new QPushButton(groupBox_2);
        btPhoto->setObjectName(QString::fromUtf8("btPhoto"));
        btPhoto->setGeometry(QRect(20, 80, 98, 31));
        numberSBox = new QSpinBox(groupBox_2);
        numberSBox->setObjectName(QString::fromUtf8("numberSBox"));
        numberSBox->setGeometry(QRect(330, 30, 71, 27));
        delaySBox = new QSpinBox(groupBox_2);
        delaySBox->setObjectName(QString::fromUtf8("delaySBox"));
        delaySBox->setGeometry(QRect(330, 80, 71, 27));
        ctlTab->addTab(photoInput, QString());
        photoDeal = new QWidget();
        photoDeal->setObjectName(QString::fromUtf8("photoDeal"));
        faceDetectImage = new QLabel(photoDeal);
        faceDetectImage->setObjectName(QString::fromUtf8("faceDetectImage"));
        faceDetectImage->setGeometry(QRect(180, 15, 261, 271));
        faceDetectLoadBt = new QPushButton(photoDeal);
        faceDetectLoadBt->setObjectName(QString::fromUtf8("faceDetectLoadBt"));
        faceDetectLoadBt->setGeometry(QRect(25, 70, 111, 41));
        faceDetectBt = new QPushButton(photoDeal);
        faceDetectBt->setObjectName(QString::fromUtf8("faceDetectBt"));
        faceDetectBt->setGeometry(QRect(25, 180, 111, 41));
        ctlTab->addTab(photoDeal, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        ctlTab->addTab(tab, QString());
        rebootButton = new QPushButton(centralwidget);
        rebootButton->setObjectName(QString::fromUtf8("rebootButton"));
        rebootButton->setGeometry(QRect(630, 400, 71, 31));
        rebootButton->setFont(font1);
        ImageIden->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ImageIden);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 788, 27));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(menubar->sizePolicy().hasHeightForWidth());
        menubar->setSizePolicy(sizePolicy);
        menubar->setBaseSize(QSize(0, 0));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        QBrush brush2(QColor(103, 164, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        QBrush brush3(QColor(159, 158, 158, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        menubar->setPalette(palette);
        menubar->setFont(font1);
        menubar->setAutoFillBackground(true);
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuFile->setGeometry(QRect(329, 138, 145, 99));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(menuFile->sizePolicy().hasHeightForWidth());
        menuFile->setSizePolicy(sizePolicy1);
        menuFile->setBaseSize(QSize(13, 16));
        menuFile->setFont(font1);
        ImageIden->setMenuBar(menubar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionFromFile);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);

        retranslateUi(ImageIden);

        ctlTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ImageIden);
    } // setupUi

    void retranslateUi(QMainWindow *ImageIden)
    {
        ImageIden->setWindowTitle(QApplication::translate("ImageIden", "\345\233\276\345\203\217\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        actionIden->setText(QApplication::translate("ImageIden", "\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        actionZuoqu->setText(QApplication::translate("ImageIden", "zhuqu", 0, QApplication::UnicodeUTF8));
        actionXunliang->setText(QApplication::translate("ImageIden", "xunliang", 0, QApplication::UnicodeUTF8));
        actionFromFile->setText(QApplication::translate("ImageIden", "\350\275\275\345\205\245\345\233\276\347\211\207", 0, QApplication::UnicodeUTF8));
        actionCatch->setText(QApplication::translate("ImageIden", "\346\215\225\350\216\267", 0, QApplication::UnicodeUTF8));
        actionTrain->setText(QApplication::translate("ImageIden", "\350\256\255\347\273\203", 0, QApplication::UnicodeUTF8));
        actionFace->setText(QApplication::translate("ImageIden", "\344\272\272\350\204\270\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        actionFace->setIconText(QApplication::translate("ImageIden", "\344\272\272\350\204\270\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        actionDigit->setText(QApplication::translate("ImageIden", "\346\225\260\345\255\227\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("ImageIden", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        actionAa->setText(QApplication::translate("ImageIden", "aa", 0, QApplication::UnicodeUTF8));
        buttonQuit->setText(QApplication::translate("ImageIden", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        labelPicture->setText(QString());
        grayBox->setText(QApplication::translate("ImageIden", "\347\201\260\345\272\246\345\233\276\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
        rbRefrashImg->setText(QApplication::translate("ImageIden", "\345\256\236\346\227\266\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
        btPrePic->setText(QString());
        btNextPic->setText(QString());
        label_7->setText(QApplication::translate("ImageIden", "\346\257\225\344\270\232\350\256\276\350\256\241--\345\233\276\345\203\217\351\207\207\351\233\206\347\263\273\347\273\237", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ImageIden", "\350\210\265\346\234\272\346\216\247\345\210\266", 0, QApplication::UnicodeUTF8));
        btUp->setText(QApplication::translate("ImageIden", "\345\211\215", 0, QApplication::UnicodeUTF8));
        btLeft->setText(QApplication::translate("ImageIden", "\345\267\246", 0, QApplication::UnicodeUTF8));
        btRight->setText(QApplication::translate("ImageIden", "\345\217\263", 0, QApplication::UnicodeUTF8));
        btDown->setText(QApplication::translate("ImageIden", "\345\220\216", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ImageIden", "\347\253\226\347\233\264\346\216\247\345\210\266", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ImageIden", "\346\260\264\345\271\263\346\216\247\345\210\266", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ImageIden", "\345\233\276\347\211\207\345\255\230\345\202\250", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ImageIden", "\345\220\215\345\255\227", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ImageIden", "\346\225\260\351\207\217", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ImageIden", "\345\273\266\346\227\266", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ImageIden", "(s)", 0, QApplication::UnicodeUTF8));
        btSave->setText(QApplication::translate("ImageIden", "\350\277\236\346\213\215", 0, QApplication::UnicodeUTF8));
        btPhoto->setText(QApplication::translate("ImageIden", "\346\212\223\346\213\215", 0, QApplication::UnicodeUTF8));
        ctlTab->setTabText(ctlTab->indexOf(photoInput), QApplication::translate("ImageIden", "\345\233\276\345\203\217\351\207\207\351\233\206", 0, QApplication::UnicodeUTF8));
        faceDetectImage->setText(QString());
        faceDetectLoadBt->setText(QApplication::translate("ImageIden", "\345\233\276\347\211\207\345\212\240\350\275\275", 0, QApplication::UnicodeUTF8));
        faceDetectBt->setText(QApplication::translate("ImageIden", "\344\272\272\350\204\270\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        ctlTab->setTabText(ctlTab->indexOf(photoDeal), QApplication::translate("ImageIden", "\344\272\272\350\204\270\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        ctlTab->setTabText(ctlTab->indexOf(tab), QApplication::translate("ImageIden", "\346\225\260\345\255\227\350\257\206\345\210\253", 0, QApplication::UnicodeUTF8));
        rebootButton->setText(QApplication::translate("ImageIden", "\351\207\215\345\220\257", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("ImageIden", "\346\226\207\344\273\266", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ImageIden: public Ui_ImageIden {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEIDEN_H
