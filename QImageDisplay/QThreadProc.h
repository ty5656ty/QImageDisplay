#ifndef _QTHREADPROC_H_
#define _QTHREADPROC_H_

#include <qthread.h>

namespace cv
{
	class Mat;
}

class QThreadProc : public QThread
{
	Q_OBJECT
public:
	QThreadProc(QObject *parent = 0);
	~QThreadProc();

	void start(Priority pri = InheritPriority);
	void stop();
	void waitData();
	void setData();
	bool checkData();
	unsigned int getProcIndex();

protected:
	QObject *pMain;
	volatile bool bRunning;
	unsigned int vIndex;

	void MessageShow(QString title, QString msg);

private:
	volatile bool bWaitData;

protected slots:
	virtual void DataSlot(void *dataptr);

signals:
	void DataSignal(void *dataptr);
	void ImageSignal(cv::Mat *image);
	void MessageSignal(QString title, QString msg);
};

#endif