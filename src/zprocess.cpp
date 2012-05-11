/******************************************************************************
	ZProcess: archive process with gui progress
	Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
	
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


#include <qfeatures.h>

#ifndef QT_NO_PROCESS
#include "zprocess_p.h"
#include "zprocess.h"


static const QString key_password("%password%");
static const QString key_overwrite("%overwrite%");
static const QString key_level("%level%");
static const QString key_in("%in%");
static const QString key_out("%out%");

ZProcessPrivate::ZProcessPrivate()
	:options(pack_options)
{
    overwrite = false;
    level = 7;
    progress = 0;
	pack_options.insert(ZProcess::Tar, "tar cvvf %out_pat% %in%");
	pack_options.insert(ZProcess::TGz, "tar zcvvf  %out_pat% %in%");
    pack_options.insert(ZProcess::TBz2, "tar jcvvf  %out_pat% %in%");
    pack_options.insert(ZProcess::T7z, "tar --use=7z -cvvf  %out_pat% %in%");
    pack_options.insert(ZProcess::Txz, "tar --use=xz -cvvf  %out_pat% %in%");
    pack_options.insert(ZProcess::TLzma, "tar --use=lzma -cvvf  %out_pat% %in%");
    pack_options.insert(ZProcess::TLz, "tar --use=lzip -cvvf  %out_pat% %in%");
    pack_options.insert(ZProcess::TLzo, "tar --use=lzop -cvvf  %out_pat% %in%");
    //-P %password%
    pack_options.insert(ZProcess::Zip, "zip -ryv -FS -%level% %password%  %out_pat% %in%");
    pack_options.insert(ZProcess::Upx, "upx -9kvf --ultra-brute  %out_pat% %in%");


    unpack_options.insert(ZProcess::Tar, "tar xvvf %in% -C %out_pat%");
    unpack_options.insert(ZProcess::TGz, "gzip -d <%in% |tar xvvf - -C %out_pat%");
    unpack_options.insert(ZProcess::TBz2, "bzip2 -d <%in% |tar xvvf - -C %out_pat%");
    unpack_options.insert(ZProcess::T7z, "7z -d <%in% |tar xvvf - -C %out_pat%");
    unpack_options.insert(ZProcess::Txz, "xz -d <%in% |tar xvvf - -C %out_pat%");
    unpack_options.insert(ZProcess::TLzma, "lzma -d <%in% |tar xvvf - -C %out_pat%");
    unpack_options.insert(ZProcess::TLz, "lzip -d <%in% |tar xvvf - -C %out_pat%");
    unpack_options.insert(ZProcess::TLzo, "lzop -d <%in% |tar xvvf - -C %out_pat%");
    //-P %password%, overwrite: -o(-n not)
    unpack_options.insert(ZProcess::Zip, "unzip %overwrite% %password% %in% -d %out_pat%");
    unpack_options.insert(ZProcess::Unzip, "unzip %overwrite% %password% %in% -d %out_pat%");
    //overwrite: -o+(-o- not), password: -ppwd
    unpack_options.insert(ZProcess::Unrar, "unrar %overwrite% -y %in% %out_pat%");

}

ZProcess::ZProcess(QObject *parent)
	:QProcess(parent),d_ptr(new ZProcessPrivate)
{
}

bool ZProcess::setArchiveTool(ArchiveTool tool)
{
	if (!toolAvailable(tool))
		return false;
	archive_tool = tool;
	return true;
}

void ZProcess::setFiles(const QStringList &files)
{
	Q_D(ZProcess);
	d->filelist = files;
}

void ZProcess::setOutputPath(const QString &path)
{
	Q_D(ZProcess);
	d->out_path = path;
}

void ZProcess::setPassword(const QString &pwd)
{
	Q_D(ZProcess);
	d->password = pwd;
}

void ZProcess::setLevel(int level)
{
	Q_D(ZProcess);
	d->level = level;
}

ZProcess::ArchiveTool ZProcess::archiveTool() const
{
    return archive_tool;
}
void ZProcess::pack()
{
	//replace %keywords%, split to stringlist, startDetached()
    Q_D(ZProcess);
    QString cmd = d->pack_options[archive_tool];
    cmd.replace(key_in, d->filelist.join(" "));
    cmd.replace(key_out, d->out_path);
    if (archive_tool == ZProcess::Zip) {
        cmd.replace("%password%", "-P " + d->password);
    } else if (archive_tool == ZProcess::SevenZip) {

    }
    qDebug("ZProcess command: %s", qPrintable(cmd));
    //exec and parse output
}

void ZProcess::unpack()
{
    Q_D(ZProcess);
    QString cmd = d->unpack_options[archive_tool];
    cmd.replace(key_in, d->filelist.join(" "));
    cmd.replace(key_out, d->out_path);
    if (archive_tool == ZProcess::Zip) {
        cmd.replace("%password%", "-P " + d->password);
        if (d->overwrite)
            cmd.replace("%overwrite%", "-o");
        else
            cmd.replace("%overwrite%", "-n");
    } else if (archive_tool == ZProcess::Unrar) {
        cmd.replace("%password%", "-p" + d->password);
        if (d->overwrite)
            cmd.replace("%overwrite%", "-o+");
        else
            cmd.replace("%overwrite%", "-o-");
    } else if (archive_tool == ZProcess::SevenZip) {

    }
    qDebug("ZProcess command: %s", qPrintable(cmd));
}

//Process control
void ZProcess::resume()
{

}

void ZProcess::pause()
{

}



bool ZProcess::toolAvailable(ArchiveTool tool) const
{
	//PATH
	return true;
}

#endif //QT_NO_PROCESS
