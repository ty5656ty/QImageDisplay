#ifndef _QIMAGEDISPLAY_H_
#define _QIMAGEDISPLAY_H_

#ifdef QIMAGEDISPLAY_LIB
# define QIMAGEDISPLAY_EXPORT Q_DECL_EXPORT
#else
# define QIMAGEDISPLAY_EXPORT Q_DECL_IMPORT
#endif

#include <QtCore\qglobal.h>
#include <qlabel.h>

#include <opencv2\opencv.hpp>

class QGridLayout;
class QLabel;
class QwtScaleWidget;
class QwtScaleWidget;
class QwtScaleEngine;
class QSave;

class QIMAGEDISPLAY_EXPORT QImageDisplay : public QLabel //本控件继承自QLabel
{
	Q_OBJECT
public:
	QImageDisplay(QWidget *parent = 0);
	~QImageDisplay();

	void setImageSize(int width, int height, bool axis = false);
	//在控件进行图片显示前，必须设置图片尺寸，以及是否显示坐标轴
	//return: void
	//width: 图片宽度
	//height:图片高度
	//axis:是否显示控件坐标轴

	void setAxisX(double min, double max, double step = 0);
	//设置控件的X坐标轴参数
	//return:void
	//min:X坐标的最小值
	//max:X坐标的最大值
	//step:尚不明确

	void setAxisY(double min, double max, double step = 0);
	//设置控件的Y坐标轴参数
	//return:void
	//min:Y坐标的最小值
	//max:Y坐标的最大值
	//step:尚不明确

	unsigned int getFrameIndex();
	//获取控件的当前图像帧计数，并置零，在画线时并不准确，供参考
	//return:当前图像帧的计数

	bool startRecord(char *filename, unsigned int length = 0);
	//开始录制控件，并保存至指定路径
	//return:表示录制是否成功，主要是输入路径是否正确
	//filename:录制保存的路径，需要指定到具体文件夹和文件名，文件夹必须已经存在，如已有同名文件，直接覆盖，注意命名。
	//length:指定录制帧长度，0表示持续录制

	void stopRecord();
	//如果正在录制控件，则立即停止控件的录制
	//return:void

public slots:
	void showImage(cv::Mat *image);
	//接收图片的槽函数，建议使用Qt::DirectConnection方式连接
	//return:void
	//image:需要显示图片的Mat数据指针

signals:
	void line_emit(float value);
	//画线定标，此信号函数用于输出所画线条约在原图中的实际像素值。
	//return:void
	//value:所画线条约在原图中的实际像素值

protected:
	int widthPic;
	int heightPic;
	int widthUI;
	int heightUI;
	int widthAxis;
	int heightAxis;
	int widthOffset;
	int heightOffset;
	int widthShow;
	int heightShow;
	unsigned int index;
	bool imageInit;
	QPixmap pixmap;
	QMutex mutex;
	cv::Mat imageShow;
	cv::Mat imageX;
	cv::Mat imageY;
	cv::Mat imageAxis;

	QGridLayout *gridLayout;
	QLabel *label;
	QwtScaleWidget *axisX;
	QwtScaleWidget *axisY;
	QwtScaleEngine *axisEngine;

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};

#endif // QIMAGEDISPLAY_H