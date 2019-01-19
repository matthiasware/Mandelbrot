#include "mandelbrot_widget.h"
#include <iostream>

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

#include<iostream>


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

	edit_maxiter = new QSpinBox(inputGroup);
	edit_maxiter->setRange(10, 1'000'000);
	edit_maxiter->setValue(50);

	edit_zoom = new QDoubleSpinBox(inputGroup);
	edit_zoom->setRange(-10, 10);
	edit_zoom->setDecimals(2);
	edit_zoom->setValue(0.7);

	edit_move = new QDoubleSpinBox(inputGroup);
	edit_move->setRange(0, 100);
	edit_move->setDecimals(2);
	edit_move->setValue(0.1);

	edit_re_max = new QDoubleSpinBox(inputGroup);
	edit_re_max->setRange(-10, 10);
	edit_re_max->setDecimals(10);
	edit_re_max->setValue(1.0);

	edit_re_min = new QDoubleSpinBox(inputGroup);
	edit_re_min->setRange(-10, 10);
	edit_re_min->setDecimals(10);
	edit_re_min->setValue(-2.0);

	edit_im_max = new QDoubleSpinBox(inputGroup);
	edit_im_max->setRange(-10, 10);
	edit_im_max->setDecimals(10);
	edit_im_max->setValue(1.2);

	edit_im_min = new QDoubleSpinBox(inputGroup);
	edit_im_min->setRange(-10, 10);
	edit_im_min->setDecimals(10);
	edit_im_min->setValue(-1.2);

	submitButton = new QPushButton(tr("Submit"), configGroup);

	// MAIN LAYOUT
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(viewer);
    mainLayout->addWidget(configGroup);

	// CONFIG LAYOUT
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->addWidget(inputGroup);
    configLayout->addWidget(submitButton);
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

    connect(submitButton,
    		SIGNAL(clicked()), this, SLOT(submitConfiguration()));
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
	switch(direction)
	{
		case CompositionWidget::MoveDirection::UP:
			im_min_ += move_ * (im_max_ - im_min_);
			im_max_ += move_ * (im_max_ - im_min_);
			break;
		case CompositionWidget::MoveDirection::DOWN:
			im_min_ -= move_ * (im_max_ - im_min_);
			im_max_ -= move_ * (im_max_ - im_min_);
			break;
		case CompositionWidget::MoveDirection::LEFT:
			re_min_ -= move_ * (re_max_ - re_min_);
			re_max_ -= move_ * (re_max_ - re_min_);
			break;
		case CompositionWidget::MoveDirection::RIGHT:
			re_min_ += move_ * (re_max_ - re_min_);
			re_max_ += move_ * (re_max_ - re_min_);
			break;
	}
	updateConfiguration();
	updateViewer();
}

QImage MandelbrotViewer::mandelbrot() const
{
	int w = width();
	int h = height();

	QImage img(w, h, QImage::Format_RGB888);


	double ref = ((re_max_ - re_min_) / (w - 1));
	double imf =  ((im_max_ - im_min_) / (h - 1));
	for(int x=0; x<w; x++)
	{
		double c_re = re_min_ + x * ref;
		for(int y=0; y<h; y++)
		{
			double c_im = im_max_ - y * imf;
			double Z_re = c_re;
			double Z_im = c_im;
			int i;
			for(i=0; i<maxiter_; i++)
			{
				if(Z_re*Z_re + Z_im*Z_im > 4)
				{
					break;
				}
				double Z_im2 = Z_im * Z_im;
				Z_im = 2 * Z_re * Z_im + c_im;
				Z_re = Z_re * Z_re - Z_im2 + c_re;
			}
			img.setPixelColor(x, y, getColor(i));
		}
	}
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