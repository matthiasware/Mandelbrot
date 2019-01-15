#include <QApplication>

#include "mandelbrot_widget.h"

int main(int argc, char** argv){
	QApplication application(argc,argv);

	MandelbrotWidget widget {};
	widget.showMaximized();

	return application.exec();
}