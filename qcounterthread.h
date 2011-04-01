/******************************************************************************
	QCounterThread: counting thread. It's a part of QOP.
    Copyright (C) 2010 Wangbin <wbsecg1@gmail.com>
 	(aka. nukin in ccmove & novesky in http://forum.motorolafans.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/
/*!
	EZX's threading is awful
*/
#ifndef QCOUNTERTHREAD_H
#define QCOUNTERTHREAD_H

#include "global.h"
#include <qobject.h>
//#ifdef QT_THREAD_SUPPORT
#include <qthread.h>
//#endif //QT_THREAD_SUPPORT
enum CountType {
		Size=0x0, Num=0x1, NumNoDir=0x3
};
//if no thread support, inherits qobject
#define EZXT_QT
class QCounterThread :
#if !(QT_VERSION >= 0x040000)
		public QObject,
#endif
		public QThread
{
	Q_OBJECT
public:
	QCounterThread(const QStringList& files=QStringList());
	virtual ~QCounterThread();

	void setFiles(const QStringList&);
	void setCountType(CountType);
#ifndef QT_THREAD_SUPPORT
	void start();
#endif //QT_THREAD_SUPPORT
	virtual void run();
signals:
	void done();
	void counted(uint); //connect to QOutParser

protected:
	void numOfFilesNoDir(const QStringList&);
	void numOfFiles(const QStringList&); //or return uint
	void sizeOfFiles(const QStringList&);

private:
	QStringList files;
	uint size, num;
	CountType ct;

};


#endif // QCOUNTERTHREAD_H
