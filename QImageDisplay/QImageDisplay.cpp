#include "QImageDisplay.h"
#include <qpainter.h>
#include <qdir.h>
#include <qgridlayout.h>
#include <qwidget.h>
#include <qscreen.h>
#include <qapplication.h>
#include <qevent.h>
#include <Qwt\qwt_scale_widget.h>
#include <Qwt\qwt_scale_engine.h>
#include <Windows.h>
#include <tiff.h>
#include <tiffio.h>
#include "QSave.h"

#pragma comment(lib,"qwtd.lib")
#pragma comment(lib,"opencv_world300d.lib")


bool record_run;
unsigned int record_num;
unsigned int record_length;
QMutex record_mutex;
TIFF *record_file;
QScreen *record_device;
QSave *record_save;

bool line_enable;
QPoint line_start;
QPoint line_stop;
QPen line_pen(Qt::red);

QImageDisplay::QImageDisplay(QWidget *parent) : QLabel(parent)
{
	index = 0;
	widthPic = 0;
	heightPic = 0;
	widthAxis = 0;
	heightAxis = 0;
	widthShow = 0;
	heightShow = 0;
	widthOffset = 0;
	heightOffset = 0;
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setSizePolicy(sizePolicy);
	axisEngine = new QwtLinearScaleEngine();
	gridLayout = new QGridLayout(this);
	label = new QLabel(this);
	label->setCursor(Qt::CrossCursor);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->addWidget(label, 0, 1, 1, 1);
	axisX = new QwtScaleWidget(QwtScaleDraw::BottomScale);
	axisX->setMaximumHeight(27);
	axisX->setMinimumHeight(27);
	axisX->setSpacing(0);
	axisX->setMargin(0);
	axisY = new QwtScaleWidget(QwtScaleDraw::LeftScale);
	axisY->setMaximumWidth(36);
	axisY->setMinimumWidth(36);
	axisY->setSpacing(0);
	axisY->setMargin(0);

	imageInit = false;

	record_run = false;
	record_num = 0;
	record_length = 0;
	record_file = NULL;
	record_save = NULL;
	record_device = QApplication::primaryScreen();

	line_enable = false;
}

QImageDisplay::~QImageDisplay()
{
	stopRecord();

	delete axisX;
	delete axisY;
	delete label;
	delete gridLayout;
	delete (QwtLinearScaleEngine*)axisEngine;

	mutex.lock();
	mutex.unlock();
}

void QImageDisplay::setImageSize(int width, int height, bool axis)
{
	widthPic = width;
	heightPic = height;

	if (axis)
	{
		axisX->setParent(this);
		gridLayout->addWidget(axisX, 1, 1, 1, 1);
		axisY->setParent(this);
		gridLayout->addWidget(axisY, 0, 0, 1, 1);
	}
	else
	{
		gridLayout->removeWidget(axisX);
		axisX->setParent(NULL);
		gridLayout->removeWidget(axisY);
		axisY->setParent(NULL);
	}

	resizeEvent(NULL);

	imageInit = true;
}

void QImageDisplay::setAxisX(double min, double max, double step)
{
	int major, minor;
	QwtScaleDiv div;
	major = 2;
	minor = 4;
	axisEngine->setBase(step);
	div = axisEngine->divideScale(min, max, major, minor, step);
	axisX->setScaleDiv(div);
}

void QImageDisplay::setAxisY(double min, double max, double step)
{
	int major, minor;
	QwtScaleDiv div;
	major = 2;
	minor = 4;
	axisEngine->setBase(step);
	div = axisEngine->divideScale(min, max, major, minor, step);
	axisY->setScaleDiv(div);
}

unsigned int QImageDisplay::getFrameIndex()
{
	unsigned int tmp;

	tmp = index;
	index = 0;

	return tmp;
}

bool QImageDisplay::startRecord(char *filename, unsigned int length)
{
	int i;
	int index;
	int filelength;
	char filedir[_MAX_PATH];
	QDir dir;

	if (!imageInit)
		return false;

	index = 0;
	filelength = 0;
	for (i = 0; i < _MAX_PATH + 1; i++)
	{
		if (filename[i] == '\\')
		{
			index = i;
		}

		if (filename[i] == '\0')
		{
			filelength = i;
			break;
		}
	}

	if (i == _MAX_PATH)
		return false;

	if (index == 0)
		return false;

	memcpy(filedir, filename, sizeof(char)* index);
	filedir[index] = '\0';

	dir = QDir(filedir);
	if (!dir.exists())
		return false;

	record_file = TIFFOpen(filename, "w+");
	if (record_file == NULL)
		return false;

	record_run = true;
	record_num = 0;
	record_length = length;

	record_save = new QSave();
	record_save->start(record_file, widthShow + widthAxis, heightShow + heightAxis, 3);

	setMaximumWidth(widthShow);
	setMaximumHeight(heightShow);
	setMinimumWidth(widthShow);
	setMinimumHeight(heightShow);
	return true;
}

void QImageDisplay::stopRecord()
{
	if (record_run)
	{
		record_save->stop();
		record_run = false;
		record_mutex.lock();
		record_mutex.unlock();

		TIFFClose(record_file);
		record_file = NULL;

		setMaximumWidth(0);
		setMaximumHeight(0);
		setMinimumWidth(0);
		setMinimumHeight(0);
	}
}

void QImageDisplay::paintEvent(QPaintEvent *event)
{
	QPainter *painter;
	painter = new QPainter(this);

	if (imageInit)
	{
		mutex.lock();
		painter->drawPixmap(QRect(widthOffset + widthAxis, heightOffset, widthShow, heightShow), pixmap);
		index++;
		if (record_run)
		{
			record_save->DataSlot((void*)record_device->grabWindow(this->winId(), widthOffset, heightOffset, widthShow + widthAxis, heightShow + heightAxis).toImage().constBits());
			if (record_length != 0)
			{
				record_num++;
				if (record_num > record_length - 1)
				{
					stopRecord();
				}
			}
		}
		mutex.unlock();

		if (line_start != line_stop)
		{
			painter->setPen(line_pen);
			painter->drawLine(line_start, line_stop);
		}
	}

	QLabel::paintEvent(event);
	delete painter;
}

void QImageDisplay::resizeEvent(QResizeEvent *event)
{
	float scaleX;
	float scaleY;

	widthUI = width();
	heightUI = height();

	if (axisX->parent() == NULL)
	{
		heightAxis = 0;
	}
	else
	{
		heightAxis = axisX->height();
	}
	if (axisY->parent() == NULL)
	{
		widthAxis = 0;
	}
	else
	{
		widthAxis = axisY->width();
	}

	gridLayout->setContentsMargins(0, 0, 0, 0);
	if (widthPic == 0 || heightPic == 0)
	{
		widthPic = 0;
		heightPic = 0;
		widthShow = 0;
		heightShow = 0;
		widthOffset = 0;
		heightOffset = 0;
		gridLayout->setContentsMargins(0, 0, 0, 0);
	}
	else
	{
		scaleX = (widthUI - widthAxis)* 1.0 / widthPic;
		scaleY = (heightUI - heightAxis) * 1.0 / heightPic;

		if (scaleX < scaleY)
		{
			widthShow = widthPic * scaleX;
			heightShow = heightPic * scaleX;
		}
		else
		{
			widthShow = widthPic * scaleY;
			heightShow = heightPic * scaleY;
		}

		widthOffset = (widthUI - widthAxis - widthShow) / 2;
		heightOffset = (heightUI - heightAxis - heightShow) / 2;

		gridLayout->setContentsMargins(widthOffset, heightOffset, widthOffset, heightOffset);
	}

	line_start.setX(0);
	line_start.setY(0);
	line_stop.setX(0);
	line_stop.setY(0);
}

void QImageDisplay::mousePressEvent(QMouseEvent* event)
{
	RECT rect;
	QPoint los;
	if (event->button() == Qt::LeftButton)
	{
		if (widthPic != 0 && heightPic != 0)
		{
			line_start.setX(event->x());
			line_start.setY(event->y());
			line_stop.setX(event->x());
			line_stop.setY(event->y());
			if (line_start.x() < widthUI - widthOffset && line_start.x() > widthOffset + widthAxis && line_start.y() < heightUI - heightOffset && line_start.y() > heightOffset)
			{
				los = mapToGlobal(QPoint(0, 0));
				rect.left = widthOffset + widthAxis + los.x();
				rect.top = heightOffset + los.y();
				rect.right = widthOffset + widthAxis + widthShow + los.x();
				rect.bottom = heightOffset + heightShow + los.y();
				ClipCursor(&rect);
				line_enable = true;
			}
		}
	}
	else
	{
		line_start.setX(0);
		line_start.setY(0);
		line_stop.setX(0);
		line_stop.setY(0);
	}
}

void QImageDisplay::mouseMoveEvent(QMouseEvent *event)
{
	if (line_enable)
	{
		line_stop.setX(event->x());
		line_stop.setY(event->y());
		update();
	}
}

void QImageDisplay::mouseReleaseEvent(QMouseEvent *event)
{
	float value;
	float x, y;

	line_enable = false;
	x = (line_stop.x() - line_start.x()) * 1.0 / widthShow * widthPic;
	y = (line_stop.y() - line_start.y()) * 1.0 / heightShow * heightPic;
	value = sqrtf(x  *x + y * y);
	ClipCursor(NULL);

	emit line_emit(value);
}

void QImageDisplay::showImage(cv::Mat *image)
{
	int channel;
	cv::Mat tmp;
	QImage *img;

	if (image->empty())
	{
		return;
	}

	if (mutex.tryLock())
	{
		imageShow = image->clone();
		channel = image->channels();
		cv::resize(imageShow, tmp, cv::Size(widthShow, heightShow), 0, 0, CV_INTER_LINEAR);
		
		if (channel == 1)
		{
			img = new QImage((unsigned char *)tmp.data, widthShow, heightShow, widthShow * channel, QImage::Format_Grayscale8);
			pixmap.convertFromImage(*img);
		}
		else
		{
			cv::cvtColor(tmp, tmp, CV_BGR2RGB);
			img = new QImage((unsigned char *)tmp.data, widthShow, heightShow, widthShow * channel, QImage::Format_RGB888);
			pixmap.convertFromImage(*img);
		}

		delete img;
		update();
		mutex.unlock();
	}
}
