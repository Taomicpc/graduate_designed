/********************************************************************************
** Form generated from reading ui file 'imageIden.ui'
**
** Created: Sun Apr 26 09:55:42 2015
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
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *grayBox;
    QRadioButton *rbRefrashImg;
    QProgressBar *saveProgressBar;
    QPushButton *btPrePic;
    QPushButton *btNextPic;
    QMenuBar *menubar;
    QMenu *menuFile;

    void setupUi(QMainWindow *ImageIden)
    {
        if (ImageIden->objectName().isEmpty())
            ImageIden->setObjectName(QString::fromUtf8("ImageIden"));
        ImageIden->resize(788, 450);
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
        actionFromFile->setFont(font1);
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
        buttonQuit->setGeometry(QRect(640, 360, 81, 32));
        QFont font2;
        font2.setFamily(QString::fromUtf8("WenQuanYi Micro Hei"));
        font2.setBold(false);
        font2.setWeight(50);
        buttonQuit->setFont(font2);
        labelPicture = new QLabel(centralwidget);
        labelPicture->setObjectName(QString::fromUtf8("labelPicture"));
        labelPicture->setGeometry(QRect(30, 30, 250, 301));
        labelPicture->setFont(font2);
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(290, 30, 471, 191));
        groupBox->setFont(font2);
        btUp = new QPushButton(groupBox);
        btUp->setObjectName(QString::fromUtf8("btUp"));
        btUp->setGeometry(QRect(60, 50, 41, 41));
        btUp->setFont(font2);
        btLeft = new QPushButton(groupBox);
        btLeft->setObjectName(QString::fromUtf8("btLeft"));
        btLeft->setGeometry(QRect(10, 100, 41, 41));
        btLeft->setFont(font2);
        btRight = new QPushButton(groupBox);
        btRight->setObjectName(QString::fromUtf8("btRight"));
        btRight->setGeometry(QRect(110, 100, 41, 41));
        btRight->setFont(font2);
        btDown = new QPushButton(groupBox);
        btDown->setObjectName(QString::fromUtf8("btDown"));
        btDown->setGeometry(QRect(60, 100, 41, 41));
        btDown->setFont(font2);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(370, 10, 81, 31));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(220, 10, 81, 31));
        horiCtlDial = new QDial(groupBox);
        horiCtlDial->setObjectName(QString::fromUtf8("horiCtlDial"));
        horiCtlDial->setGeometry(QRect(180, 70, 131, 111));
        horiCtlDial->setMinimumSize(QSize(131, 0));
        horiCtlDial->setSizeIncrement(QSize(0, 0));
        QFont font3;
        font3.setPointSize(15);
        horiCtlDial->setFont(font3);
        horiCtlDial->setCursor(QCursor(Qt::ArrowCursor));
        horiCtlDial->setMouseTracking(false);
        horiCtlDial->setOrientation(Qt::Horizontal);
        horiCtlDial->setInvertedAppearance(false);
        horiCtlDial->setInvertedControls(false);
        horiCtlDial->setWrapping(false);
        horiCtlDial->setNotchesVisible(true);
        vertCtlSlider = new QSlider(groupBox);
        vertCtlSlider->setObjectName(QString::fromUtf8("vertCtlSlider"));
        vertCtlSlider->setGeometry(QRect(380, 70, 21, 111));
        vertCtlSlider->setOrientation(Qt::Vertical);
        horiValueSBox = new QSpinBox(groupBox);
        horiValueSBox->setObjectName(QString::fromUtf8("horiValueSBox"));
        horiValueSBox->setGeometry(QRect(220, 40, 51, 21));
        vertValueSBox = new QSpinBox(groupBox);
        vertValueSBox->setObjectName(QString::fromUtf8("vertValueSBox"));
        vertValueSBox->setGeometry(QRect(370, 40, 51, 21));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(290, 230, 471, 121));
        groupBox_2->setFont(font2);
        leName = new QLineEdit(groupBox_2);
        leName->setObjectName(QString::fromUtf8("leName"));
        leName->setGeometry(QRect(130, 40, 91, 27));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(80, 40, 41, 31));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(290, 40, 61, 31));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(290, 80, 61, 31));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(430, 80, 21, 31));
        btSave = new QPushButton(groupBox_2);
        btSave->setObjectName(QString::fromUtf8("btSave"));
        btSave->setGeometry(QRect(170, 90, 100, 29));
        btSave->setFont(font2);
        btPhoto = new QPushButton(groupBox_2);
        btPhoto->setObjectName(QString::fromUtf8("btPhoto"));
        btPhoto->setGeometry(QRect(50, 90, 98, 27));
        numberSBox = new QSpinBox(groupBox_2);
        numberSBox->setObjectName(QString::fromUtf8("numberSBox"));
        numberSBox->setGeometry(QRect(350, 40, 71, 27));
        delaySBox = new QSpinBox(groupBox_2);
        delaySBox->setObjectName(QString::fromUtf8("delaySBox"));
        delaySBox->setGeometry(QRect(350, 80, 71, 27));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 350, 263, 29));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        grayBox = new QCheckBox(layoutWidget);
        grayBox->setObjectName(QString::fromUtf8("grayBox"));

        horizontalLayout->addWidget(grayBox);

        rbRefrashImg = new QRadioButton(layoutWidget);
        rbRefrashImg->setObjectName(QString::fromUtf8("rbRefrashImg"));
        rbRefrashImg->setCheckable(true);

        horizontalLayout->addWidget(rbRefrashImg);

        saveProgressBar = new QProgressBar(centralwidget);
        saveProgressBar->setObjectName(QString::fromUtf8("saveProgressBar"));
        saveProgressBar->setGeometry(QRect(340, 360, 221, 23));
        saveProgressBar->setValue(24);
        saveProgressBar->setTextVisible(true);
        btPrePic = new QPushButton(centralwidget);
        btPrePic->setObjectName(QString::fromUtf8("btPrePic"));
        btPrePic->setGeometry(QRect(40, 50, 101, 281));
        btNextPic = new QPushButton(centralwidget);
        btNextPic->setObjectName(QString::fromUtf8("btNextPic"));
        btNextPic->setGeometry(QRect(150, 50, 101, 281));
        ImageIden->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ImageIden);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 788, 30));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        ImageIden->setMenuBar(menubar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionFromFile);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);

        retranslateUi(ImageIden);

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
        groupBox->setTitle(QApplication::translate("ImageIden", "\350\210\265\346\234\272\346\216\247\345\210\266", 0, QApplication::UnicodeUTF8));
        btUp->setText(QApplication::translate("ImageIden", "Ahead", 0, QApplication::UnicodeUTF8));
        btLeft->setText(QApplication::translate("ImageIden", "Left", 0, QApplication::UnicodeUTF8));
        btRight->setText(QApplication::translate("ImageIden", "Right", 0, QApplication::UnicodeUTF8));
        btDown->setText(QApplication::translate("ImageIden", "Back", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ImageIden", "\347\253\226\347\233\264\346\216\247\345\210\266", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ImageIden", "\346\260\264\345\271\263\346\216\247\345\210\266", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ImageIden", "\345\233\276\347\211\207\345\255\230\345\202\250", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ImageIden", "\345\220\215\345\255\227", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ImageIden", "\346\225\260\351\207\217", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ImageIden", "\345\273\266\346\227\266", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ImageIden", "(s)", 0, QApplication::UnicodeUTF8));
        btSave->setText(QApplication::translate("ImageIden", "\350\277\236\346\213\215", 0, QApplication::UnicodeUTF8));
        btPhoto->setText(QApplication::translate("ImageIden", "\346\212\223\346\213\215", 0, QApplication::UnicodeUTF8));
        grayBox->setText(QApplication::translate("ImageIden", "\347\201\260\345\272\246\345\233\276\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
        rbRefrashImg->setText(QApplication::translate("ImageIden", "\345\256\232\346\227\266\345\210\267\346\226\260\345\233\276\345\203\217", 0, QApplication::UnicodeUTF8));
        btPrePic->setText(QString());
        btNextPic->setText(QString());
        menuFile->setTitle(QApplication::translate("ImageIden", "\346\226\207\344\273\266", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ImageIden: public Ui_ImageIden {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEIDEN_H
