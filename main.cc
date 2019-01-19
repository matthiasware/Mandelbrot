#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include "mandelbrot_widget.h"

int main(int argc, char** argv){
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QPalette p;

	QApplication::setStyle("CDE");
	QApplication application(argc,argv);

	p = application.palette();
	p.setColor(QPalette::Window, QColor(53,53,53));
	p.setColor(QPalette::Button, QColor(53,53,53));
	p.setColor(QPalette::Highlight, QColor(142,45,197));
	p.setColor(QPalette::ButtonText, QColor(255,255,255));
	p.setColor(QPalette::WindowText, QColor(255,255,255));
	application.setPalette(p);

	CompositionWidget widget {};
	widget.showMaximized();

	return application.exec();
}