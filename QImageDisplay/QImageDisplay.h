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

class QIMAGEDISPLAY_EXPORT QImageDisplay : public QLabel //���ؼ��̳���QLabel
{
	Q_OBJECT
public:
	QImageDisplay(QWidget *parent = 0);
	~QImageDisplay();

	void setImageSize(int width, int height, bool axis = false);
	//�ڿؼ�����ͼƬ��ʾǰ����������ͼƬ�ߴ磬�Լ��Ƿ���ʾ������
	//return: void
	//width: ͼƬ���
	//height:ͼƬ�߶�
	//axis:�Ƿ���ʾ�ؼ�������

	void setAxisX(double min, double max, double step = 0);
	//���ÿؼ���X���������
	//return:void
	//min:X�������Сֵ
	//max:X��������ֵ
	//step:�в���ȷ

	void setAxisY(double min, double max, double step = 0);
	//���ÿؼ���Y���������
	//return:void
	//min:Y�������Сֵ
	//max:Y��������ֵ
	//step:�в���ȷ

	unsigned int getFrameIndex();
	//��ȡ�ؼ��ĵ�ǰͼ��֡�����������㣬�ڻ���ʱ����׼ȷ�����ο�
	//return:��ǰͼ��֡�ļ���

	bool startRecord(char *filename, unsigned int length = 0);
	//��ʼ¼�ƿؼ�����������ָ��·��
	//return:��ʾ¼���Ƿ�ɹ�����Ҫ������·���Ƿ���ȷ
	//filename:¼�Ʊ����·������Ҫָ���������ļ��к��ļ������ļ��б����Ѿ����ڣ�������ͬ���ļ���ֱ�Ӹ��ǣ�ע��������
	//length:ָ��¼��֡���ȣ�0��ʾ����¼��

	void stopRecord();
	//�������¼�ƿؼ���������ֹͣ�ؼ���¼��
	//return:void

public slots:
	void showImage(cv::Mat *image);
	//����ͼƬ�Ĳۺ���������ʹ��Qt::DirectConnection��ʽ����
	//return:void
	//image:��Ҫ��ʾͼƬ��Mat����ָ��

signals:
	void line_emit(float value);
	//���߶��꣬���źź������������������Լ��ԭͼ�е�ʵ������ֵ��
	//return:void
	//value:��������Լ��ԭͼ�е�ʵ������ֵ

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