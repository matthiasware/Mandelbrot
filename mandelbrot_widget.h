#ifndef MANDELBROT_WIDGET_H_
#define MANDELBROT_WIDGET_H_ 

#include <chrono>
#include <iostream>
#include <cmath>

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QKeyEvent>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>

class MandelbrotViewer : public QWidget
{
	Q_OBJECT

public:
	MandelbrotViewer(QWidget *parent=nullptr);

	void updateValues(int maxiter,
					  double re_min, double re_max,
					  double im_min, double im_max);
protected:
	void paintEvent(QPaintEvent * event) override;
private:
	QColor mapping_[16];

	int maxiter_;
	double re_max_;
	double re_min_;
	double im_max_;
	double im_min_;

	QImage mandelbrot() const;
	QColor getColor(int iteration) const;
};


class CompositionWidget : public QWidget
{
	Q_OBJECT

public:
	CompositionWidget(QWidget *parent=nullptr);

	void updateViewer();
	void updateConfiguration();

public slots:
	void submitConfiguration();

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	enum MoveDirection {UP, DOWN, LEFT, RIGHT};

	MandelbrotViewer *viewer;

	QSpinBox *edit_maxiter;
	QDoubleSpinBox *edit_zoom;
	QDoubleSpinBox *edit_move;
	QDoubleSpinBox *edit_re_max;
	QDoubleSpinBox *edit_re_min;
	QDoubleSpinBox *edit_im_max;
	QDoubleSpinBox *edit_im_min;
	QPushButton *submitButton;

	QSpinBox *edit_img_width;
	QSpinBox *edit_img_height;
	QPushButton *saveImgButton;

	int maxiter_;
	double zoom_;
	double move_;
	double re_max_;
	double re_min_;
	double im_max_;
	double im_min_;

	void applyZoom(double faktor);
	void applyZoom_helper(double &min, double& max, double faktor);
	void applyMove(MoveDirection direction);

	QDoubleSpinBox* makeQDoubleSpinBox(double min, double max, double dec, double val, QWidget *parent);
};

#endif // MANDELBROT_WIDGET_H_