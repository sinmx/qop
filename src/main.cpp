/******************************************************************************
	QOP: Qt Output Parser for tar, zip etc with a compression/extraction progress indicator
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

#include "global.h"

#if CONFIG_EZX
#include <ZApplication.h>
#include <ZLanguage.h>
#endif //CONFIG_EZX

#include <qdir.h>
//#include <qfileinfo.h>
#include <qtranslator.h>
#include <stdlib.h>
#include <stdio.h>

#include "qop.h"
#include "option.h"
#include "version.h"
//#include "msgdef.h"

static const char *appName=(char*)malloc(64);
//static char appName[64]={};
const QString program="qop";

void printHelp()
{
	fprintf(stderr,	"%s %s (%s, %s)\n"
					"Usage: %s [-t parserFor] [-n|s] [-hmc] [-x archieve|-T totalSteps] [files...] [-C cmd]\n"
					"  -t, --parser[=TYPE]  parser(tar,untar,zip,unzip,unrar,lzip.upx)\n"
					"  -n, --number         count number of files as total steps\n"
					"  -s, --size           count size of files as total steps\n"
					"  -T, --steps=STEPS    specify total steps.\n"
					"  -h, --help           help. print me\n"
					"  -m, --multi-thread   multi-thread counting steps while (de)compressing\n"
					"  -c, --auto-close     auto close when finished\n"
					"  -C, --cmd=command    execute command. e.g. -C tar cvvf test.tar test\n"
					"  -x, --extract=FILE   indicates extracting progress\n"
					"  -o, --outdir=dir     set the output dir when using internal extract method\n"

					"\nCopyright (C) 2010 Wangbin(nukin CCMOVE, aka novesky in motorolafans)\n"
					"This program comes with ABSOLUTELY NO WARRANTY, to the extent permitted by low.\n"
					"This is free software, and you are welcome to redistribute it "
					"under the terms of the GNU General Public Licence version 2\n<http://www.gnu.org/licenses/gpl-2.0.html>.\n"
					"\n"
					"Project:\n"
					"    http://sourceforge.net/projects/qop/files\n"
					"    http://qt-apps.org/content/show.php/qop?content=132430\n"
					"Send bugreports to <wbsecg1@gmail.com>\n\n",appName,APP_VERSION_STR,__DATE__,__TIME__,appName);
}

int main(int argc, char *argv[])
{
	appName=getFileName(argv[0]);
	qDebug("%s %s (%s, %s)",APP_NAME,APP_VERSION_STR,__DATE__,__TIME__);

	opts_t options=opts_parse(argc,argv);

	ZApplication a(argc, argv, QApplication::GuiClient);
	printf("Qt %s\n",qVersion());
#if QT_VERSION >= 0x040000
	QString dirname=QCoreApplication::applicationDirPath();
#else
	QString dirname=QFileInfo(argv[0]).dirPath();
	//QString dirname=getFileDir(argv[0]);//
#endif
	//QDir::setCurrent(dirname); //bad in windows cygwin
	ZDEBUG("dir: %s",qPrintable(dirname));


	QTranslator appTranslator(0);
#if CONFIG_EZX
	QString sysLang=ZLanguage::getSystemLanguageCode();
	appTranslator.load(program+"_"+sysLang,dirname+"/i18n");
	//QString(dirname)+"/i18n" will load fail, 乱码
#else
	QString sysLang=QLocale::system().name();
	appTranslator.load(program+"-"+sysLang,dirname+"/i18n");
#endif //CONFIG_EZX
	ZDEBUG("system language: %s",qPrintable(sysLang));
	a.installTranslator(&appTranslator);
	//Need QtTranslator

	Qop qop;
#if !CONFIG_QT4
	a.setMainWidget(qop.progress);
#endif //CONFIG_QT4
	ZDEBUG("Steps from options: %d",options->steps);
	if(!options->hide)
		qop.progress->show();
	//order is important
	//qop.parser_type=options->parser_type;
	qop.setArchive(options->x_file);

	if(options->diy || argc<2) //internal method
		qop.extract(options->x_file,options->out_dir);
	else if(!options->cmd==0)
		qop.execute(QString::fromLocal8Bit(options->cmd));
	else {
		qop.parser_type=options->parser_type;
		ZDEBUG("steps %d",options->steps);
		qop.initParser();
		if(options->unit==0) qop.parser->setCountType(QCounterThread::Size);
		else if(options->unit==1) qop.parser->setCountType(QCounterThread::Num);
		if(options->steps>0) {
			qop.parser->setRecount(false);
			qop.parser->setTotalSize(options->steps);
			//qop.progress->setMaximum(options->steps);
		}
		if(qop.steps<=0) { //compress
			if(options->steps>0) qop.parser->setTotalSize(options->steps);
			QStringList files=QStringList();
		//why is optind?
			for(int i=options->optind;i<argc;++i) files<<argv[i];
			qop.parser->setFiles(files);
			qop.parser->setMultiThread(options->multi_thread);
			qop.parser->startCounterThread();
		}
		qop.parser->start();
	}
#if CONFIG_EZX
	a.processEvents();
#endif
	//progress->exec();
	if(options->auto_close) exit(0);

	return a.exec();
}

