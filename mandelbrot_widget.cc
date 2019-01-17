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

#include<iostream>

CompositionWidget::CompositionWidget(QWidget *parent) : QWidget(parent)
{
	// MandelbrotViewer *viewer = new MandelbrotViewer(this);
	MandelbrotWidget *viewer = new MandelbrotWidget(this);

	QGroupBox *configGroup = new QGroupBox(this);
	configGroup->setTitle("Configuration");

	QGroupBox *inputGroup = new QGroupBox(configGroup);
	inputGroup->setTitle("Input");

	edit_maxiter = new QSpinBox(inputGroup);
	edit_zoom = new QDoubleSpinBox(inputGroup);
	edit_move = new QDoubleSpinBox(inputGroup);
	edit_re_max = new QDoubleSpinBox(inputGroup);
	edit_re_min = new QDoubleSpinBox(inputGroup);
	edit_im_max = new QDoubleSpinBox(inputGroup);
	edit_im_min = new QDoubleSpinBox(inputGroup);
	edit_im_min->setMaximum(10);
	edit_im_min->setMinimum(-2);

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
	std::cout << "BUTTON CLICKED" << std::endl;
}

void CompositionWidget::maxiter(double maxiter)
{
	maxiter_ = maxiter;
}
int CompositionWidget::maxiter() const
{
	return maxiter_;
}
void CompositionWidget::zoom(double zoom)
{
	zoom_ = zoom;
}
double CompositionWidget::zoom() const
{
	return zoom_;
}
void CompositionWidget::move(double move)
{
	move_ = move;
}
double CompositionWidget::move() const
{
	return move_;
}
void CompositionWidget::re_max(double re_max)
{
	re_max_ = re_max;
}
double CompositionWidget::re_max() const
{
	return re_max_;
}

void CompositionWidget::re_min(double re_min)
{
	re_min_ = re_min;
}
double CompositionWidget::re_min() const
{
	return re_min_;
}

void CompositionWidget::im_max(double im_max)
{
	im_max_ = im_max;
}
double CompositionWidget::im_max() const
{
	return im_max_;
}

void CompositionWidget::im_min(double im_min)
{
	im_min_ = im_min;
}
double CompositionWidget::im_min() const
{
	return im_min_;
}


void CompositionWidget::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Escape) QApplication::quit();
}


MandelbrotViewer::MandelbrotViewer(QWidget *parent) : QWidget(parent)
{

}