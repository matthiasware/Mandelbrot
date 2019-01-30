#include "mandelbrot_widget.h"
#include <iostream>
#include <iomanip>

#include<QWidget>
#include<QGroupBox>
#include<QDoubleSpinBox>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QFormLayout>
#include<QLayout>
#include<QKeyEvent>
#include<QApplication>
#include<QPaintEvent>
#include<QPainter>
#include<QImage>
#include<QLabel>
#include "mandelbrot.h"
#include<iostream>
#include <math.h>
#include<QFileDialog>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


MandelbrotViewer::MandelbrotViewer(QWidget *parent) : QWidget(parent)
{
    mapping_[0].setRgb( 66,  30,  15 );
    mapping_[1].setRgb( 25,  7,   26 );
    mapping_[2].setRgb( 9,   1,   47 );
    mapping_[3].setRgb( 4,   4,   73 );
    mapping_[4].setRgb( 0,   7,   100);
    mapping_[5].setRgb( 12,  44,  138);
    mapping_[6].setRgb( 24,  82,  177);
    mapping_[7].setRgb( 57,  125, 209);
    mapping_[8].setRgb( 134, 181, 229);
    mapping_[9].setRgb( 211, 236, 248);
    mapping_[10].setRgb(241, 233, 191);
    mapping_[11].setRgb(248, 201, 95 );
    mapping_[12].setRgb(255, 170, 0  );
    mapping_[13].setRgb(204, 128, 0  );
    mapping_[14].setRgb(153, 87,  0  );
    mapping_[15].setRgb(106, 52,  3  );
}

CompositionWidget::CompositionWidget(QWidget *parent) : QWidget(parent)
{
	// MandelbrotViewer *viewer = new MandelbrotViewer(this);
	viewer = new MandelbrotViewer(this);

	QGroupBox *configGroup = new QGroupBox(this);
	configGroup->setTitle("Configuration");

	QGroupBox *inputGroup = new QGroupBox(configGroup);
	inputGroup->setTitle("Input");

	QGroupBox *saveImgGroup = new QGroupBox(configGroup);
	saveImgGroup->setTitle("Save Image");

	edit_maxiter = new QSpinBox(inputGroup);
	edit_maxiter->setRange(10, 1'000'000);
	edit_maxiter->setValue(500);

	edit_zoom = makeQDoubleSpinBox(-10, 10, 2, 0.7, inputGroup);
	edit_move = makeQDoubleSpinBox(0, 100, 2 ,0.1, inputGroup);
	edit_re_max = makeQDoubleSpinBox(-10, 10, 10, 1, inputGroup);
	edit_re_min = makeQDoubleSpinBox(-10, 10, 10, -2, inputGroup);
	edit_im_max = makeQDoubleSpinBox(-10, 10, 10, 1.2, inputGroup);
	edit_im_min = makeQDoubleSpinBox(-10, 10, 10, -1.2, inputGroup);

	submitButton = new QPushButton(tr("Submit"), configGroup);

	saveImgButton = new QPushButton(tr("Save Image"), saveImgGroup);
	edit_img_width = new QSpinBox(saveImgGroup);
	edit_img_width->setRange(10, 1'000'000);
	edit_img_width->setValue(3000);

	edit_img_height = new QSpinBox(saveImgGroup);
	edit_img_height->setRange(10, 1'000'000);
	edit_img_height->setValue(2400);

	// MAIN LAYOUT
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(viewer);
    mainLayout->addWidget(configGroup);

	// CONFIG LAYOUT
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->addWidget(inputGroup);
    configLayout->addWidget(submitButton);
    configLayout->addWidget(saveImgGroup);
    configLayout->addWidget(saveImgButton);
    configLayout->setSizeConstraint(QLayout::SetFixedSize);

    // INPUT LAYOUT
	QFormLayout *inputLayout = new QFormLayout(inputGroup);
    inputLayout->addRow(new QLabel(tr("Maxiter:")), edit_maxiter);
    inputLayout->addRow(new QLabel(tr("Zoom:")), edit_zoom);
    inputLayout->addRow(new QLabel(tr("Move:")), edit_move);
    inputLayout->addRow(new QLabel(tr("re_max:")), edit_re_max);
    inputLayout->addRow(new QLabel(tr("re_min:")), edit_re_min);
    inputLayout->addRow(new QLabel(tr("im_max:")), edit_im_max);
    inputLayout->addRow(new QLabel(tr("im_min:")), edit_im_min);

	// SAVE IMG LAYOUT
    QFormLayout *imgSaveLayout = new QFormLayout(saveImgGroup);
    imgSaveLayout->addRow(new QLabel(tr("Width:")), edit_img_width);
    imgSaveLayout->addRow(new QLabel(tr("Height:")), edit_img_height);
    
    connect(submitButton,
    		SIGNAL(clicked()), this, SLOT(submitConfiguration()));
    connect(saveImgButton,
    	    SIGNAL(clicked()), this, SLOT(saveImage()));
    submitConfiguration();
}

QDoubleSpinBox* CompositionWidget::makeQDoubleSpinBox(
		double min, double max,
		double dec, double val,
		QWidget *parent)
{
	QDoubleSpinBox* qdsb = new QDoubleSpinBox(parent);
	qdsb->setRange(min, max);
	qdsb->setDecimals(dec);
	qdsb->setValue(val);
	return qdsb;
}

void CompositionWidget::submitConfiguration()
{
	maxiter_ = edit_maxiter->value();
	zoom_ = edit_zoom->value();
	move_ = edit_move->value();
	re_max_ = edit_re_max->value();
	re_min_ = edit_re_min->value();
	im_max_ = edit_im_max->value();
	im_min_ = edit_im_min->value();
	viewer->setFocus();
	updateViewer();
}

void CompositionWidget::saveImage()
{
	int w = edit_img_width->value();
	int h = edit_img_height->value();
	// because otherwise we get segfaults from AVX ;)
	w = w + w % 4; 

	int* map = (int*)aligned_alloc(32, h*w * sizeof(int));
	mandelbrot_avx(w, h, maxiter_, re_min_, re_max_,
		           im_min_, im_max_, map);
	colorMap_omp(w, h, maxiter_, map, RGBFORM::ABGR);
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                           "untitled.png",
                           tr("Images (*.png *.xpm *.jpg)"));
	int r = stbi_write_png(fileName.toLatin1().data(), w, h, 4, (void *) map, 4*w);
}

void MandelbrotViewer::updateValues(int maxiter,
					 				double re_min, double re_max,
					  			    double im_min, double im_max)
{
	maxiter_ = maxiter;
	re_min_ = re_min;
	re_max_ = re_max;
	im_min_ = im_min;
	im_max_ = im_max;
	repaint();
}

void MandelbrotViewer::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	// faster to display
	QPixmap pixmap;

	pixmap = QPixmap::fromImage(mandelbrot());
	painter.drawPixmap(0, 0, pixmap);
	painter.setFont(QFont("times",28));
	painter.setPen(Qt::red);

	painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, QString::number(im_max_));
	painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter, QString::number(im_min_));
	painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, QString::number(re_min_));
	painter.drawText(rect(), Qt::AlignRight | Qt::AlignVCenter, QString::number(re_max_));
	painter.drawText(rect(), Qt::AlignRight | Qt::AlignTop, QString::number(maxiter_));
}

void CompositionWidget::updateViewer()
{
	viewer->updateValues(maxiter_,
						 re_min_, re_max_,
						 im_min_, im_max_);
}
void CompositionWidget::updateConfiguration()
{
	edit_maxiter->setValue(maxiter_);
	edit_zoom->setValue(zoom_);
	edit_move->setValue(move_);
	edit_re_min->setValue(re_min_);
	edit_re_max->setValue(re_max_);
	edit_im_min->setValue(im_min_);
	edit_im_max->setValue(im_max_);
}


void CompositionWidget::applyZoom(double faktor)
{
	applyZoom_helper(re_min_, re_max_, faktor);
	applyZoom_helper(im_min_, im_max_, faktor);
	updateConfiguration();
	updateViewer();
}
void CompositionWidget::applyZoom_helper(double &min, double& max, double faktor)
{
	double diff = max - min;
	double diff_new = diff * faktor;
	double add = (diff - diff_new) * 0.5;
	min += add;
	max -= add;
}
void CompositionWidget::applyMove(CompositionWidget::MoveDirection direction)
{
	double d;
	switch(direction)
	{
		case CompositionWidget::MoveDirection::UP:
			d = move_  * (im_max_ - im_min_);
			im_min_ += d;
			im_max_ += d;
			break;
		case CompositionWidget::MoveDirection::DOWN:
			d = move_  * (im_max_ - im_min_);
			im_min_ -= d;
			im_max_ -= d;
			break;
		case CompositionWidget::MoveDirection::LEFT:
			d = move_ * (re_max_ - re_min_);
			re_min_ -= d;
			re_max_ -= d;
			break;
		case CompositionWidget::MoveDirection::RIGHT:
			d = move_ * (re_max_ - re_min_);
			re_min_ += d;
			re_max_ += d;
			break;
	}
	updateConfiguration();
	updateViewer();
}

QImage MandelbrotViewer::mandelbrot() const
{
	int w = width();
	w = w + w %4;
	int h = height();

	QImage img(w, h, QImage::Format_ARGB32);
	int *map = reinterpret_cast<int*>(img.bits()); // is 32 bit aligned

	mandelbrot_avx_omp(w, h, maxiter_, re_min_, re_max_, im_min_, im_max_, map);
	colorMap_omp(w, h, maxiter_, map, RGBFORM::ARGB);
	return img;
}

QColor MandelbrotViewer::getColor(int iteration) const
{
	if(iteration < maxiter_)
	{
		return mapping_[iteration % 16];
	}
	else
	{
		return Qt::white;
	}
}


void CompositionWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{	
		QApplication::quit();
	}
	else if(event->key() == Qt::Key_Plus)
	{
		applyZoom(zoom_);
	}
	else if(event->key() == Qt::Key_Minus)
	{
		applyZoom(1/zoom_);
	}
	else if(event->key() == Qt::Key_W)
	{
		applyMove(CompositionWidget::MoveDirection::UP);
	}
	else if(event->key() == Qt::Key_A)
	{
		applyMove(CompositionWidget::MoveDirection::LEFT);
	}
	else if(event->key() == Qt::Key_S)
	{
		applyMove(CompositionWidget::MoveDirection::DOWN);
	}
	else if(event->key() == Qt::Key_D)
	{
		applyMove(CompositionWidget::MoveDirection::RIGHT);
	}
	else if(event->key() == Qt::Key_Down)
	{
		maxiter_ -= 10;
		updateConfiguration();
	}
	else if(event->key() == Qt::Key_Up)
	{
		maxiter_ += 10;
		updateConfiguration();
	}
	else if(event->key() == Qt::Key_Return)
	{
		submitConfiguration();
	}
}