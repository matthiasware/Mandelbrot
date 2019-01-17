#ifndef MANDELBROT_WIDGET_H_
#define MANDELBROT_WIDGET_H_ 

#include <chrono>
#include <iostream>
#include <cmath>

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QTimer>
#include <QApplication>
#include <QKeyEvent>
#include <QFont>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>

// class ControlsWidget : public QWidget
// {
// 	Q_OBJECT

// public:
// 	ControlsWidget(QWidget *parent = nullptr) : QWidget(parent)
// 	{

// 		this->setMaximumWidth(300);
// 		// main layout
// 		QVBoxLayout *layout = new QVBoxLayout(this);
		
// 		// item 1
// 		group_box = new QGroupBox(tr("Settings"));
// 		// group_box->setAlignment(Qt::AlignRight | Qt::AlignTop);
// 		// group_box->setFixedSize(200, 400);

// 		// item 2
// 		button = new QPushButton(tr("Enter"));

// 		layout->addWidget(group_box);
// 		layout->addWidget(button);

// 		edit_maxiter = new QLineEdit("");
// 		edit_zoom = new QLineEdit("");
// 		edit_move = new QLineEdit("");
// 		edit_re_max = new QLineEdit("");
// 		edit_re_min = new QLineEdit("");
// 		edit_im_max = new QLineEdit("");
// 		edit_im_min = new QLineEdit("");
		
// 		QFormLayout *gbl = new QFormLayout;
// 	    gbl->addRow(new QLabel(tr("Maxiter:")), edit_maxiter);
// 	    gbl->addRow(new QLabel(tr("Zoom:")), edit_zoom);
// 	    gbl->addRow(new QLabel(tr("Move:")), edit_move);
// 	    gbl->addRow(new QLabel(tr("re_max:")), edit_re_max);
// 	    gbl->addRow(new QLabel(tr("re_min:")), edit_re_min);
// 	    gbl->addRow(new QLabel(tr("im_max:")), edit_im_max);
// 	    gbl->addRow(new QLabel(tr("im_min:")), edit_im_min);

// 	    group_box->setLayout(gbl);

// 	    connect(button, &QPushButton::clicked, &ControlsWidget::enterClicked);
// 	}
// protected:
// 	QGroupBox *group_box;
// 	QPushButton *button;
// 	QLineEdit *edit_maxiter;
// 	QLineEdit *edit_zoom;
// 	QLineEdit *edit_move;
// 	QLineEdit *edit_re_max;
// 	QLineEdit *edit_re_min;
// 	QLineEdit *edit_im_max;
// 	QLineEdit *edit_im_min;

// 	void enterClicked()
// 	{
// 		std::cout << edit_maxiter->text().toDouble() << std::endl;
// 	}

// 	void keyPressEvent(QKeyEvent* event) override
// 	{
// 		if(event->key() == Qt::Key_Return)
// 		{
// 			enterClicked();
// 		}
// 	}
// };

class CompositionWidget : public QWidget
{
	Q_OBJECT

public:
	CompositionWidget(QWidget *parent=nullptr);
	void maxiter(double maxiter);
	int maxiter() const;
	
	void zoom(double zoom);
	double zoom() const;

	void move(double move);
	double move() const;

	void re_max(double re_max);
	double re_max() const;

	void re_min(double re_min);
	double re_min() const;

	void im_max(double im_max);
	double im_max() const;

	void im_min(double im_min);
	double im_min() const;

public slots:
	void submitConfiguration();

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:

	QSpinBox *edit_maxiter;
	QDoubleSpinBox *edit_zoom;
	QDoubleSpinBox *edit_move;
	QDoubleSpinBox *edit_re_max;
	QDoubleSpinBox *edit_re_min;
	QDoubleSpinBox *edit_im_max;
	QDoubleSpinBox *edit_im_min;
	QPushButton *submitButton;

	int maxiter_;
	double zoom_;
	double move_;
	double re_max_;
	double re_min_;
	double im_max_;
	double im_min_;
};

class MandelbrotViewer : public QWidget
{
	Q_OBJECT

public:
	MandelbrotViewer(QWidget *parent=nullptr);
};

class MandelbrotWidget : public QWidget
{
	Q_OBJECT

public:
	// ControlsWidget *controlsWidget;
	MandelbrotWidget(QWidget *parent = nullptr): QWidget(parent)
	{
		    mapping[0].setRgb( 66,  30,  15 );
		    mapping[1].setRgb( 25,  7,   26 );
		    mapping[2].setRgb( 9,   1,   47 );
		    mapping[3].setRgb( 4,   4,   73 );
		    mapping[4].setRgb( 0,   7,   100);
		    mapping[5].setRgb( 12,  44,  138);
		    mapping[6].setRgb( 24,  82,  177);
		    mapping[7].setRgb( 57,  125, 209);
		    mapping[8].setRgb( 134, 181, 229);
		    mapping[9].setRgb( 211, 236, 248);
		    mapping[10].setRgb(241, 233, 191);
		    mapping[11].setRgb(248, 201, 95 );
		    mapping[12].setRgb(255, 170, 0  );
		    mapping[13].setRgb(204, 128, 0  );
		    mapping[14].setRgb(153, 87,  0  );
		    mapping[15].setRgb(106, 52,  3  );
	}

protected:
	void keyPressEvent(QKeyEvent* event) override {
		if (event->key() == Qt::Key_Escape)
		{	
			QApplication::quit();
		}
		else if(event->key() == Qt::Key_Plus)
		{
			zoomAll(zf);
			repaint();
		}
		else if(event->key() == Qt::Key_Minus)
		{
			zoomAll(1/zf);
			repaint();
		}
		else if(event->key() == Qt::Key_W)
		{

			im_min += 0.1 * (im_max - im_min);
			im_max += 0.1 * (im_max - im_min);
			repaint();			
		}
		else if(event->key() == Qt::Key_A)
		{
			re_min -= 0.1 * (re_max - re_min);
			re_max -= 0.1 * (re_max - re_min);
			repaint();
		}
		else if(event->key() == Qt::Key_S)
		{
			im_min -= 0.1 * (im_max - im_min);
			im_max -= 0.1 * (im_max - im_min);
			repaint();
		}
		else if(event->key() == Qt::Key_D)
		{
			re_min += 0.1 * (re_max - re_min);
			re_max += 0.1 * (re_max - re_min);
			repaint();
		}
		else if(event->key() == Qt::Key_Up)
		{
			maxiter += 10;
		}
		else if(event->key() == Qt::Key_Down)
		{
			maxiter -= 10;
		}
		else if(event->key() == Qt::Key_Return)
		{
			repaint();
		}
	}
	void paintEvent(QPaintEvent * event)
	{
		QPainter painter(this);

		// faster to display
		QPixmap pixmap;

		pixmap = QPixmap::fromImage(mandelbrot());
		painter.drawPixmap(0, 0, pixmap);
		painter.setFont(QFont("times",28));
		painter.setPen(Qt::red);

		painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, QString::number(im_max));
		painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter, QString::number(im_min));
		painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, QString::number(re_min));
		painter.drawText(rect(), Qt::AlignRight | Qt::AlignVCenter, QString::number(re_max));
		painter.drawText(rect(), Qt::AlignRight | Qt::AlignTop, QString::number(maxiter));
	}
private:
	QColor mapping[16];
	double zf = 0.7;
	int maxiter = 100;
	double re_min{-2}, re_max{1.0};
	double im_min{-1.2}, im_max{1.2};

	void zoom(double &minimum, double &maximum, double zf)
	{
		double diff = maximum - minimum;
		double diff_new = diff * zf;
		double add = (diff - diff_new) * 0.5;
		minimum += add;
		maximum -= add;
	}

	void zoomAll(double zf)
	{
		zoom(re_min, re_max, zf);
		zoom(im_min, im_max, zf);
	}

	QColor getColor(int iteration)
	{
		if(iteration < maxiter)
		{
			return mapping[iteration % 16];
		}
		else
		{
			return Qt::white;
		}
	}
	QImage mandelbrot()
	{
		int w = width();
		int h = height();

		QImage img(w, h, QImage::Format_RGB888);


		double ref = ((re_max - re_min) / (w - 1));
		double imf =  ((im_max - im_min) / (h - 1));
		for(int x=0; x<w; x++)
		{
			double c_re = re_min + x * ref;
			for(int y=0; y<h; y++)
			{
				double c_im = im_max - y * imf;
				double Z_re = c_re;
				double Z_im = c_im;
				int i;
				for(i=0; i<maxiter; i++)
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
};

#endif // MANDELBROT_WIDGET_H_