#ifndef _QSAVE_H_
#define _QSAVE_H_

#include "QThreadProc.h"

class QSave : public QThreadProc
{
public:
	QSave();
	~QSave();
	void start(void *file, int width, int height, int bytestep = 1, Priority pri = InheritPriority);
	void DataSlot(void *dataptr);

private:
	int bufferwidth;
	int bufferheight;
	int bufferlen;
	void *buffer;
	void *tiff;
	void *ui;
	void run();

};

#endif