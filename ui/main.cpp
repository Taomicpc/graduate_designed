/******************************************************************************

********************************************************************************/

#include <QApplication>
#include <QTextCodec>
#include "imageIden.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QTextCodec *codec = QTextCodec::codecForName("utf8");
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);	

	app.setFont(QFont("wqy-microhei"));
	ImageIden ii;
	ii.show();
	return app.exec();
}
