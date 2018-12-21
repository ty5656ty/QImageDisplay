#include "QThreadProc.h"

QThreadProc::QThreadProc(QObject *parent)
{
	vIndex = 0;
	pMain = parent;
	connect(this, SIGNAL(MessageSignal(QString, QString)), pMain, SLOT(MessageShow(QString, QString)));

	bWaitData = true;
}

QThreadProc::~QThreadProc()
{
	disconnect(this, SIGNAL(MessageSignal(QString, QString)), pMain, SLOT(MessageShow(QString, QString)));
}

void QThreadProc::start(Priority pri)
{
	bRunning = true;
	QThread::start(pri);
}

void QThreadProc::stop()
{
	bRunning = false;
	wait();
}

void QThreadProc::waitData()
{
	while (bWaitData)
	{
		if (!bRunning)
		{
			break;
		}
	}
	bWaitData = true;
}

void QThreadProc::setData()
{
	bWaitData = false;
}

bool QThreadProc::checkData()
{
	return bWaitData;
}

unsigned int QThreadProc::getProcIndex()
{
	unsigned int index;
	index = vIndex;
	vIndex = 0;
	return index;
}


void QThreadProc::MessageShow(QString title, QString msg)
{
	emit MessageSignal(title, msg);
}

void QThreadProc::DataSlot(void *dataptr)
{
	dataptr = dataptr;
}