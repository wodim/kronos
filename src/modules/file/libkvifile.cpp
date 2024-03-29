//=============================================================================
//
//   File : libkvifile.cpp
//   Creation date : Fri Nov  9 03:27:59 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2012 Elvio Basello (hellvis69 at gmail dot com)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "KviModule.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "KviMemory.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviKvsArrayCast.h"
#include "KviFile.h"

#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <QByteArray>
#include <QDateTime>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <windows.h>
#else
#include <sys/statvfs.h>
#endif


#if defined(COMPILE_SSL_SUPPORT)
	#include <openssl/evp.h>
#else
	// The fallback we can always use, but with very limited set of
	// functionality.
	#include <QCryptographicHash>
#endif

/*
	@doc: file.copy
	@type:
		command
	@title:
		file.copy
	@keyterms:
		copying files
	@short:
		Makes a copy of a file
	@syntax:
		file.copy [-o] <source:string> <destination:string>
	@switches:
		!sw: -o | --overwrite
		Overwrites the file even if it already exists.
	@description:
		Makes a copy of the <source> file as <destination>.[br]
		If the [-o] switch is used, the <destination> file is overwritten, if already exists.[br]
		With no [-o] switch, this command does not overwrite files.[br]
		The destination path must be already existing: if you want to ensure this, use [fnc]$file.mkdir[/fnc] first.[br]
		The paths (<source> and <destination>) are adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed. Just use UNIX style paths for them.[br]
	@seealso:
		[cmd]file.rename[/cmd], [fnc]$file.exists[/fnc]
*/
static bool file_kvs_cmd_copy(KviKvsModuleCommandCall * c)
{
	QString szSrc,szDst;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("source",KVS_PT_NONEMPTYSTRING,0,szSrc)
		KVSM_PARAMETER("destination",KVS_PT_NONEMPTYSTRING,0,szDst)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szSrc);
	KviFileUtils::adjustFilePath(szDst);
	if(KviFileUtils::fileExists(szDst) && !c->switches()->find('o',"overwrite"))
	{
		c->warning(__tr2qs("Destination file exists: no copy made"));
		return true;
	}
	if(!KviFileUtils::copyFile(szSrc,szDst))
	{
		c->warning(__tr2qs("Failed to copy from '%Q' to '%Q'"),&szSrc,&szDst);
		c->warning(__tr2qs("Either the source doesn't exist or the destination can not be created"));
	}
	return true;
}

/*
	@doc: file.addimagepath
	@type:
		command
	@title:
		file.addimagepath
	@short:
		Adds an image search path
	@syntax:
		file.addimagepath <path:string>
	@description:
		Adds <path> to the image search path list.[br]
		KVIrc will look for images files (also) in that path (when a relative image file name is used).[br]
		Yes, this function is a bit misplaced...
	@seealso:
		[cmd]file.delimagepath[/cmd]
*/
static bool file_kvs_cmd_addimagepath(KviKvsModuleCommandCall * c)
{
	QString szDst;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("path",KVS_PT_NONEMPTYSTRING,0,szDst)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szDst);

	//QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).find(szDst);
	int iIndex = KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).indexOf(szDst);
	//if(it == KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).end())
	if(iIndex == -1)
		KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).append(szDst);
	return true;
}

/*
	@doc: file.delimagepath
	@type:
		command
	@title:
		file.delimagepath
	@short:
		Removes an image search path
	@syntax:
		file.delimagepath <path:string>
	@description:
		Removes <path> from the image search path list.[br]
		Yes, this function is a bit misplaced...
	@seealso:
		[cmd]file.addimagepath[/cmd]
*/
static bool file_kvs_cmd_delimagepath(KviKvsModuleCommandCall * c)
{
	QString szDst;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("path",KVS_PT_NONEMPTYSTRING,0,szDst)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szDst);

	//QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).find(szDst);
	int iIndex = KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).indexOf(szDst);
	//if(it == KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).end())
	if(iIndex != -1)
		KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).removeAt(iIndex);
	return true;
}

/*
	@doc: file.write
	@type:
		command
	@title:
		file.write
	@keyterms:
		writing data to files
	@short:
		Writes character data string to a file
	@syntax:
		file.write [-a] [-l] <filename:string> <data:string>
	@switches:
		!sw: -a | --append
		If the file already exists, append the data instead of overwriting the original contents.
		!sw: -l | --local-8-bit
		Causes the file to be written in the local 8 bit character set instead of the
		default utf8.
	@description:
		Writes <data> (which is a string of characters) to the file <filename>.[br]
		It does NOT append a traling LF character: if you want it you must explicitly specify it in the <data> parameter.[br]
		-a causes the command to append the <data> to the file instead of overwriting the entire file.[br]
		The path is adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed. Just use UNIX style paths for them.[br]
		The file is saved in utf8 unless the -l switch is specified (in that case the local 8 bit encoding is used).
		Please note that utf8 is the only character set that preserves ALL the possible characters
		with the exception of the null terminator. If you want to write binary data
		take a look at [fnc]$file.writeBytes[/cmd] instead.
	@seealso:
		[cmd]file.rename[/cmd], [fnc]$file.exists[/fnc]
*/
static bool file_kvs_cmd_write(KviKvsModuleCommandCall * c)
{
	QString szFileName,szData;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szFileName)
		KVSM_PARAMETER("data",KVS_PT_NONEMPTYSTRING,0,szData)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szFileName);

	bool bRet;
	if(c->switches()->find('l',"local-8-bit"))
		bRet = KviFileUtils::writeFileLocal8Bit(szFileName,szData,c->switches()->find('a',"append"));
	else
		bRet = KviFileUtils::writeFile(szFileName,szData,c->switches()->find('a',"append"));

	if(!bRet)
		c->warning(__tr2qs("Failed to write to file '%Q': the destination couldn't be opened"),&szFileName);

	return true;
}

/*
	@doc: file.writeBytes
	@type:
		command
	@title:
		file.writeBytes
	@keyterms:
		writing data to files
	@short:
		Writes binary data string to a file
	@syntax:
		file.writeBytes [-a] <filename:string> <dataArray:array>
	@switches:
		!sw: -a | --append
		If the file already exists, append the data instead of overwriting the original contents.
	@description:
		Writes <dataArray> (which must be an array) to the file <filename>.[br]
		The array should either contain integers between 0 and 255, which will be written as bytes,
		or strings, which will be written in utf8 encoding.
		-a causes the command to append the <data> to the file instead of overwriting the entire file.[br]
		The path is adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed. Just use UNIX style paths for them.[br]
	@examples:
		[example]
			for(%i=0;%i<256;%i++)
				%a[%i] = %i
			file.writeBytes "/home/pragma/test.bin" %a
		[/example]
	@seealso:
		[cmd]file.rename[/cmd], [fnc]$file.exists[/fnc], [fnc]$file.write[/fnc]
*/
static bool file_kvs_cmd_writeBytes(KviKvsModuleCommandCall * c)
{
	QString szFileName;
	KviKvsArray * pArray = NULL;
	
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szFileName)
		KVSM_PARAMETER("dataArray",KVS_PT_ARRAY,0,pArray)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szFileName);

	if(!pArray)
		return c->error(__tr2qs("Missing data array"));

	KviFile f(szFileName);
	if(!f.open(QFile::WriteOnly | (c->switches()->find('a',"append") ? QFile::Append : QFile::Truncate)))
	{
		c->warning(__tr2qs("Can't open file '%1' for writing").arg(szFileName));
		return true;
	}

	QByteArray aBuffer;

	for(kvs_uint_t i=0;i<pArray->size();i++)
	{
		KviKvsVariant * pData = pArray->at(i);
		kvs_int_t iValue;
		if(pData->asInteger(iValue))
		{
			aBuffer.append((char)iValue);
		} else {
			QString szValue;
			pData->asString(szValue);
			aBuffer.append(szValue.toUtf8());
		}
	}

	if(!aBuffer.data())
		return true; // nothing to do
	if(f.write(aBuffer.data(),aBuffer.length()) != ((unsigned int)(aBuffer.length())))
		c->warning(__tr2qs("Error writing bytes to file '%1'").arg(szFileName));

	return true;
}

/*
	@doc: file.rename
	@type:
		command
	@title:
		file.rename
	@keyterms:
		copying files
	@short:
		Makes a copy of a file
	@syntax:
		file.rename <oldname:string> <newname:string>
	@description:
		Renames a file from <oldname> to <newname>.[br]
		This command can also rename directories.[br]
		If the <newname> file already exists, this command fails.[br]
		The paths (<oldname> and <newname>) are adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed.Just use UNIX style paths for them.[br]
	@seealso:
		[cmd]file.copy[/cmd], [fnc]$file.exists[/fnc]
*/
static bool file_kvs_cmd_rename(KviKvsModuleCommandCall * c)
{
	QString szOld,szNew;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("oldname",KVS_PT_NONEMPTYSTRING,0,szOld)
		KVSM_PARAMETER("newname",KVS_PT_NONEMPTYSTRING,0,szNew)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szOld);
	KviFileUtils::adjustFilePath(szNew);

	if(KviFileUtils::fileExists(szNew))
		c->warning(__tr2qs("Destination file exists: file not renamed"));

	if(!KviFileUtils::renameFile(szOld,szNew))
		c->warning(__tr2qs("Failed to rename '%Q' to '%Q'"),&szOld,&szNew);
	return true;
}

/*
	@doc: file.mkdir
	@type:
		command
	@title:
		file.mkdir
	@keyterms:
		creating directories
	@short:
		Creates a directory
	@syntax:
		file.mkdir <directory:string>
	@description:
		Creates the <directory>.[br]
		The path is adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed. Just use an UNIX style path.[br]
	@seealso:
		[fnc]$file.exists[/fnc]
*/
static bool file_kvs_cmd_mkdir(KviKvsModuleCommandCall * c)
{
	QString szDir;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("directory",KVS_PT_NONEMPTYSTRING,0,szDir)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szDir);
	if(!KviFileUtils::makeDir(szDir))
		c->warning(__tr2qs("Failed to make the directory '%Q'"),&szDir);
	return true;
}

/*
	@doc: file.remove
	@type:
		command
	@title:
		file.remove
	@keyterms:
		removing files
	@short:
		Removes a file
	@syntax:
		file.remove [-q] <name:string>
	@switches:
		!sw: -q | --quiet
		Suppresses any warning message
	@description:
		Removes the file <name>.[br]
		-q suppresses any warning message (about non existing file, for example).[br]
		The path is adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed. Just use an UNIX style path.[br]
	@seealso:
		[fnc]$file.exists[/fnc]
*/
static bool file_kvs_cmd_remove(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);
	if(!KviFileUtils::removeFile(szName))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("Failed to remove the file '%Q'"),&szName);
	}
	return true;
}

/*
	@doc: file.rmdir
	@type:
		command
	@title:
		file.rmdir
	@keyterms:
		removing directories
	@short:
		Removes a directory
	@syntax:
		file.rmdir [-q] <name:string>
	@switches:
		!sw: -q | --quiet
		Suppresses any warning message
	@description:
		Removes the directory <name>.[br]
		The directory must be empty for this command to success.[br]
		-q suppresses any warning message (about non existing directory, for example).[br]
		The path is adjusted according to the system that KVIrc
		is running on so you don't have to bother about portability: it *should* be automatically
		guaranteed. Just use an UNIX style path.[br]
	@seealso:
		[fnc]$file.exists[/fnc]
		[cmd]file.remove[/cmd]
*/
static bool file_kvs_cmd_rmdir(KviKvsModuleCommandCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);
	if(!KviFileUtils::removeDir(szName))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("Failed to remove the directory '%Q'"),&szName);
	}
	return true;
}

/*
	@doc: file.exists
	@type:
		function
	@title:
		$file.exists
	@short:
		Check if a file or directory exists
	@syntax:
		<boolean> $file.exists(<file or directory path:string>)
	@description:
		Returns true if the specified file or directory exists.[br]
		The path should be given in UNIX-style and is adjusted according to the system that KVIrc is running on.[br]
	@examples:
		[example]
			# Windows
			echo $file.exists(c:/windows/notepad.exe);
			# Linux, other unixes
			echo $file.exists(/etc/passwd);
		[/example]
*/
static bool file_kvs_fnc_exists(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_STRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);
	QFileInfo f(szName);
	c->returnValue()->setBoolean(f.exists());
	return true;
}

/*
	@doc: file.type
	@type:
		function
	@title:
		$file.type
	@short:
		Checks the type of a path
	@syntax:
		<string> $file.type(<filename:string>)
	@description:
		Returns "f" if the <filename> points to a real file, "d" if <filename>
		is the name of a directory or "l" if it is a symbolic link.[br]
		The <filename> should be an unix-style file path and is adjusted according to the system that KVIrc is running on.[br]
*/
static bool file_kvs_fnc_type(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);
	QFileInfo f(szName);
	if(f.isFile())
		c->returnValue()->setString("f");
	else if(f.isDir())
		c->returnValue()->setString("d");
	else if(f.isSymLink())
		c->returnValue()->setString("l");
	return true;
}

/*
	@doc: file.size
	@type:
		function
	@title:
		$file.size
	@short:
		Returns the size of a file
	@syntax:
		<integer> $file.size(<filename:string>)
	@description:
		Returns the size of the file pointed by <filename>.[br]
		If the file does not exist, this function returns 0.[br]
		The <filename> should be an unix-style file path and is adjusted according to the system that KVIrc is running on.[br]
*/
static bool file_kvs_fnc_size(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);
	QFileInfo f(szName);
	c->returnValue()->setInteger(f.size());
	return true;
}

/*
	@doc: file.allSizes
	@type:
		function
	@title:
		$file.allSizes
	@short:
		Returns the size of each file in a specified directory.
	@syntax:
	<array> $file.allSize(<dirname:string>)
	@description:
	Returns the size of each file in a specified directory as an array.[br]
	This function is not recursive: it won't return the size of files in subdirectories of <dirname>.
*/
static bool file_kvs_fnc_allSizes(KviKvsModuleFunctionCall * c)
{
	QString szDir;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("directory",KVS_PT_NONEMPTYSTRING,0,szDir)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szDir);

	QDir d(szDir);
	if(!d.exists())
	{
		c->warning(__tr2qs("The specified directory does not exist '%Q'"),&szDir);
		return true;
	}

	QStringList sl;
	sl = d.entryList(QDir::Files);

	KviKvsArray * pArray = new KviKvsArray();
	QString szFile;
	if(!sl.isEmpty())
	{
		int iIdx = 0;
		for(QStringList::Iterator it = sl.begin(); it != sl.end(); ++it)
		{
			szFile = szDir + (*it);
			QFileInfo f(szFile);
			pArray->set(iIdx,new KviKvsVariant((kvs_int_t)f.size()));
			iIdx++;
		}
	}
	c->returnValue()->setArray(pArray);

	return true;
}

/*
	@doc: file.fixpath
	@type:
		function
	@title:
		$file.fixpath
	@short:
		Converts file paths
	@syntax:
		<string> $file.fixpath(<filename:string>)
	@description:
		Returns the <filename> adjusted to match the current operating
		system file path conventions.[br] This means that on UNIX, a path like "C:\folder\file.mp3"
		will be returned as "/folder/file.mp3" and vice-versa.[br]
		There is a minor problem with unix paths converted to the windows system: unix
		has no "drive" concept thus the unix paths do not contain a drive. KVIrc will
		always map the paths to the "default" C: drive.[br]
		This is a good reason to avoid using absolute hard-coded paths :).[br]
		Please note that you DON'T NEED to call this function on paths that you
		pass to the other file.* functions: they are adjusted automatically.[br]
	@seealso:
		[fnc]$file.ps[/fnc]
*/
static bool file_kvs_fnc_fixpath(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_STRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);
	c->returnValue()->setString(szName);
	return true;
}

/*
	@doc: file.ps
	@type:
		function
	@title:
		$file.ps
	@short:
		Returns the file path separator
	@syntax:
		<string> $file.ps()
	@description:
		Returns the file path separator for the current operating system.[br]
		On windows, '\' is returned, on UNIX, '/'.[br]
	@seealso:
		[fnc]$file.fixpath[/fnc]
*/
static bool file_kvs_fnc_ps(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setString(QString(QChar(KVI_PATH_SEPARATOR_CHAR)));
	return true;
}

/*
	@doc: file.ls
	@type:
		function
	@title:
		$file.ls
	@short:
		Returns a directory listing
	@syntax:
		<array> $file.ls(<directory:string>,[<flags:string>[,<namefilter:string>]])
	@description:
		Returns the listing of the specified directory as an array.[br]
		The <directory> should be a unix-style file path and is adjusted according to the system that KVIrc is running on.[br]
		<flags> may be a combination of the following characters:[br]
		d: list directories[br]
		f: list files[br]
		l: list symbolic links[br]
		r: list readable files[br]
		w: list writable files[br]
		x: list executable files[br]
		h: list hidden files[br]
		s: list system files[br]
		n: sort files by name[br]
		t: sort files by file time[br]
		b: sort files by file size[br]
		z: put the directories first, then the files[br]
		k: invert sort order[br]
		i: case insensitive sort[br]
		If <flags> is empty then a default of 'dfrwxhs' is set.[br]
		If none of the r,w,x flags are set then KVIrc sets all of them by default.[br]
		If <namefilter> is passed then it is interpreted as a wildcard string
		that must match the entries to be returned.[br]
	@example:
		[example]
			%dir[]=$file.ls(/,"d")
			[cmd]foreach[/cmd](%f,%dir[])echo %f
			%dir[]=$file.ls(/usr/include,"f","t*.h");
			[cmd]foreach[/cmd](%f,%dir[])echo %f
			%dir[]=$file.ls($file.homedir,"dfr");
			[cmd]foreach[/cmd](%f,%dir[])echo %f
		[/example]
*/
static bool file_kvs_fnc_ls(KviKvsModuleFunctionCall * c)
{
	QString szDir,szFlags,szFilter;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("directory",KVS_PT_NONEMPTYSTRING,0,szDir)
		KVSM_PARAMETER("flags",KVS_PT_STRING,KVS_PF_OPTIONAL,szFlags)
		KVSM_PARAMETER("filter",KVS_PT_STRING,KVS_PF_OPTIONAL,szFilter)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szDir);

	QDir d(szDir);
	if(!d.exists())
	{
		c->warning(__tr2qs("The specified directory does not exist '%Q'"),&szDir);
		return true;
	}

	QFlags<QDir::Filter> iFlags = 0;
	if(szFlags.isEmpty())
		iFlags = QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::Writable | QDir::Executable | QDir::Hidden | QDir::System;
	else {
		if(szFlags.indexOf('d',Qt::CaseInsensitive) != -1)iFlags |= QDir::Dirs;
		if(szFlags.indexOf('f',Qt::CaseInsensitive) != -1)iFlags |= QDir::Files;
		if(szFlags.indexOf('l',Qt::CaseInsensitive) == -1)iFlags |= QDir::NoSymLinks;
		if(szFlags.indexOf('r',Qt::CaseInsensitive) != -1)iFlags |= QDir::Readable;
		if(szFlags.indexOf('w',Qt::CaseInsensitive) != -1)iFlags |= QDir::Writable;
		if(szFlags.indexOf('x',Qt::CaseInsensitive) != -1)iFlags |= QDir::Executable;
		if(szFlags.indexOf('h',Qt::CaseInsensitive) != -1)iFlags |= QDir::Hidden;
		if(szFlags.indexOf('s',Qt::CaseInsensitive) != -1)iFlags |= QDir::System;
	}

	QFlags<QDir::SortFlag> iSort = 0;
	if(szFlags.isEmpty())
		iSort = QDir::Unsorted;
	else {
		if(szFlags.indexOf('n',Qt::CaseInsensitive) != -1)iSort |= QDir::Name;
		if(szFlags.indexOf('t',Qt::CaseInsensitive) != -1)iSort |= QDir::Time;
		if(szFlags.indexOf('b',Qt::CaseInsensitive) != -1)iSort |= QDir::Size;
		if(szFlags.indexOf('z',Qt::CaseInsensitive) != -1)iSort |= QDir::DirsFirst;
		if(szFlags.indexOf('k',Qt::CaseInsensitive) != -1)iSort |= QDir::Reversed;
		if(szFlags.indexOf('i',Qt::CaseInsensitive) != -1)iSort |= QDir::IgnoreCase;
	}

	QStringList sl;
	if(!szFilter.isEmpty())
		sl = d.entryList(QStringList(szFilter),iFlags,iSort);
	else
		sl = d.entryList(iFlags,iSort);

	KviKvsArray * pArray = new KviKvsArray();
	if(!sl.isEmpty())
	{
		int iIdx = 0;
		for(QStringList::Iterator it = sl.begin(); it != sl.end(); ++it)
		{
			pArray->set(iIdx,new KviKvsVariant(*it));
			iIdx++;
		}
	}
	c->returnValue()->setArray(pArray);

	return true;
}

/*
	@doc: file.read
	@type:
		function
	@title:
		$file.read
	@short:
		Reads a text file
	@syntax:
		<string> $file.read(<filename:string>[,<size:integer>[,<flags:string>]])
	@description:
		Reads at most <size> bytes of the file pointed by <filename>.[br]
		<size> is an upper limit but may be not reached if the real file is smaller.[br]
		The data read is returned as a string, so if the file contains binary data,
		expect strange results.[br] If <size> is not specified, then KVIrc tries to read
		the whole file up to the 1 MiB limit (so if you want to read a file that is
		bigger than 1 MiB then you MUST specify the <size>).[br]
		If you want read binary data (with null bytes inside) then take a look at [fnc]$file.readBytes[/fnc].
		WARNING: always check the file size before attempting to read a whole file...
		reading a CDROM iso image may sit down your system :) (and will probably crash while
		allocating memory, before attempting to read anything)[br]
		An empty string is returned if a serious error occures.[br]
		The <filename> is adjusted according to the system that KVIrc is running on.[br]
		Flags are actually limited to the single letter 'l'. By default the file
		is decoded from the utf8 characters set. If 'l' is present the the file
		is decoded by using the local 8 bit character set instead.
	@examples:
		[example]
			echo $file.read(/proc/cpuinfo)
		[/example]
	@seealso:
		[fnc]$file.readBytes[/fnc]
*/
static bool file_kvs_fnc_read(KviKvsModuleFunctionCall * c)
{
	QString szNameZ;
	kvs_uint_t uSize;
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szNameZ)
		KVSM_PARAMETER("size",KVS_PT_UINT,KVS_PF_OPTIONAL,uSize)
		KVSM_PARAMETER("flags",KVS_PT_STRING,KVS_PF_OPTIONAL,szFlags)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szNameZ);

	QFile f(szNameZ);
	if(!f.open(QIODevice::ReadOnly))
	{
		c->warning(__tr2qs("Can't open the file '%Q' for reading"),&szNameZ);
		return true;
	}

	if(c->params()->count() < 2)
		uSize = 1024 * 1024; // 1 meg file default

	char * pcBuf = (char *)KviMemory::allocate(sizeof(char) * (uSize + 1));
	unsigned int uReaded = 0;
	unsigned int uRetries = 0;

	while((uReaded < uSize) && (!f.atEnd()))
	{
		if(uRetries > 1000)
		{
			// ops
			KviMemory::free(pcBuf);
			c->warning(__tr2qs("Read error for file '%Q' (have been unable to read the requested size in 1000 retries)"),&szNameZ);
			return true;
		}

		int iReadedNow = f.read(pcBuf + uReaded, uSize - uReaded);

		if(iReadedNow < 0)
		{
			KviMemory::free(pcBuf);
			c->warning(__tr2qs("Read error for file '%Q'"),&szNameZ);
			return true;
		}

		uReaded += iReadedNow;
		uRetries++;
	}

	pcBuf[uReaded] = '\0';

	if(szFlags.indexOf('l',Qt::CaseInsensitive) == -1)
		c->returnValue()->setString(QString::fromUtf8(pcBuf));
	else
		c->returnValue()->setString(QString::fromLocal8Bit(pcBuf));

	KviMemory::free(pcBuf);

	return true;
}

/*
	@doc: file.readBytes
	@type:
		function
	@title:
		$file.readBytes
	@short:
		Reads a binary file
	@syntax:
		<array> $file.read(<filename:string>[,<size:integer>])
	@description:
		Reads at most <size> bytes of the file pointed by <filename>
		and returns it as an array of integers (bytes).
		<size> is an upper limit but may be not reached if the real file is smaller.[br]
		If <size> is not specified, then KVIrc tries to read
		the whole file up to the 1 MiB limit (so if you want to read a file that is
		bigger than 1 MiB then you MUST specify the <size>).[br]
		If you want simple text data then take a look at [fnc]$file.read[/fnc].
		WARNING: always check the file size before attemting to read a whole file...
		reading a CDROM iso image may sit down your system :) (and will probably crash while
		allocating memory, before attempting to read anything)[br]
		An empty array (or just "nothing") is returned if a serious error occures.[br]
		The <filename> is adjusted according to the system that KVIrc is running on.[br]
	@examples:
		[example]
			echo $file.readBytes(/proc/cpuinfo)
		[/example]
	@seealso:
		[fnc]$file.read[/fnc], [cmd]file.writeBytes[/cmd]
*/
static bool file_kvs_fnc_readBytes(KviKvsModuleFunctionCall * c)
{
	QString szNameZ;
	kvs_uint_t uSize;
	QString szFlags;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szNameZ)
		KVSM_PARAMETER("size",KVS_PT_UINT,KVS_PF_OPTIONAL,uSize)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szNameZ);

	QFile f(szNameZ);
	if(!f.open(QIODevice::ReadOnly))
	{
		c->warning(__tr2qs("Can't open the file '%Q' for reading"),&szNameZ);
		return true;
	}

	if(c->params()->count() < 2)
		uSize = 1024 * 1024; // 1 meg file default

	unsigned char * pcBuf = (unsigned char *)KviMemory::allocate(sizeof(char) * (uSize + 1));
	unsigned int uReaded = 0;
	unsigned int uRetries = 0;

	while((uReaded < uSize) && (!f.atEnd()))
	{
		if(uRetries > 1000)
		{
			// ops
			KviMemory::free(pcBuf);
			c->warning(__tr2qs("Read error for file '%Q' (have been unable to read the requested size in 1000 retries)"),&szNameZ);
			return true;
		}

		int iReadedNow = f.read((char *)(pcBuf + uReaded),uSize - uReaded);

		if(iReadedNow < 0)
		{
			KviMemory::free(pcBuf);
			c->warning(__tr2qs("Read error for file '%Q'"),&szNameZ);
			return true;
		}

		uReaded += iReadedNow;
		uRetries++;
	}

	KviKvsArray * pArray = new KviKvsArray();

	kvs_uint_t u = 0;
	unsigned char * p = pcBuf;
	while(u < uReaded)
	{
		pArray->set(u,new KviKvsVariant((kvs_int_t)(*p)));
		u++;
		p++;
	}

	c->returnValue()->setArray(pArray);

	KviMemory::free(pcBuf);

	return true;
}

/*
	@doc: file.readLines
	@type:
		function
	@title:
		$file.readLines
	@short:
		Reads lines of a text file
	@syntax:
		<array> $file.readLines(<filename:string>[,<startline:integer>,[<count:integer>[,<flags:string>]]])
	@description:
		Reads lines from the specified file and returns them as an array of strings.
		The lines are assumed to be separated by linefeed characters (which are NOT returned).
		Eventual terminating carriage return and line feed characters at the end of the line are stripped.
		If <startline> is specified, then all the lines with indexes lower that <startline> are
		discarded. If <count> is specified then at most this number of lines are returned,
		otherwise the entire file is returned.
		The <filename> is adjusted according to the system that KVIrc is running on.[br]
		Flags are actually limited to the single letter 'l'. By default the file
		is decoded from the utf-8 character set, however if 'l' is present then the file
		is decoded by the local 8 bit character set instead.
		WARNING: Always check the size of the file you're going to read - it is not
		a good idea attempting to read a 700 MiB binary file with this function since
		it will probably hang your system and exhaust your virtual memory.
	@examples:
		[example]
			echo $file.readLines(/proc/cpuinfo)
		[/example]
	@seealso:
		[fnc]$file.read[/fnc], [cmd]file.writeLines[/cmd], [fnc]$lf[/fnc]
*/
static bool file_kvs_fnc_readLines(KviKvsModuleFunctionCall * c)
{
	QString szName;
	QString szFlags;
	kvs_int_t iStartLine;
	kvs_int_t iCount;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szName)
		KVSM_PARAMETER("startline",KVS_PT_INT,KVS_PF_OPTIONAL,iStartLine)
		KVSM_PARAMETER("count",KVS_PT_INT,KVS_PF_OPTIONAL,iCount)
		KVSM_PARAMETER("flags",KVS_PT_STRING,KVS_PF_OPTIONAL,szFlags)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::adjustFilePath(szName);

	QFile f(szName);
	if(!f.open(QIODevice::ReadOnly))
	{
		c->warning(__tr2qs("Can't open the file '%Q' for reading"),&szName);
		return true;
	}

	if(c->params()->count() < 2)
		iStartLine = 0;
	if(c->params()->count() < 3)
		iCount = -1;

	bool bLocal8Bit = szFlags.indexOf('l',0,Qt::CaseInsensitive) != -1;

	KviKvsArray * pArray = new KviKvsArray();

	int iIndex = 0;

	QTextStream stream(&f);

	if(!bLocal8Bit)
		stream.setCodec(QTextCodec::codecForMib(106));

	for(int i = 0; i < iStartLine; i++)
		stream.readLine();

	if(iCount > 0)
	{
		for(;(iCount > 0 && !stream.atEnd()); iCount--)
			pArray->set(iIndex,new KviKvsVariant(stream.readLine()));
			iIndex++;
	} else {
		while(!stream.atEnd())
		{
			pArray->set(iIndex,new KviKvsVariant(stream.readLine()));
			iIndex++;
		}
	}

	f.close();

	c->returnValue()->setArray(pArray);

	return true;
}

/*
	@doc: file.writelines
	@type:
		command
	@title:
		file.writeLines
	@short:
		Writes an array of lines to a file
	@syntax:
		file.writeLines [-l] [-a] [-c] <filename:string> <lines:array>
	@switches:
		!sw: -l | --local-8-bit
		Causes the lines to be saved in local 8 bit character set instead
		of the default unicode encoding.
		!sw: -a | --append
		If the file already exists, then the lines are appended to the end
		instead of overwriting the file.
		!sw: -c | --crlf
		The lines are separated by a carriage-return+line-feed character
		combination, compatible with windows text mode files.
		This is the only way to make the file readable in windows notepad, for example.
		Please note that this is broken design: do NOT use it :)
		!sw: -n | --no-separator
		Do not separate the lines at all (either the separators are already
		inside the lines array or no line separation is desired at all).
		-n takes precedence over -c.
		!sw: -q | --quiet
		Don't complain if the file can't be opened: just fail silently
	@description:
		Writes the array of <lines> to the specified file.
		The lines are separated by a single linefeed character (see also [fnc]$lf[/fnc])
		unless the -c or -n switches are used. If the file already exists then it is
		overwritten with the new data unless the -a switch is used.
		The lines array is encoded in the ut8 character set unless the -l switch is used.
	@seealso:
		[fnc]$file.readLines[/fnc]
*/
static bool file_kvs_cmd_writeLines(KviKvsModuleCommandCall * c)
{
	QString szFile,szFlags;
	KviKvsArrayCast a;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_NONEMPTYSTRING,0,szFile)
		KVSM_PARAMETER("lines",KVS_PT_ARRAYCAST,0,a)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szFile);

	KviFile f(szFile);

	bool bAppend = c->switches()->find('a',"append");
	bool bLocal8Bit = c->switches()->find('l',"local-8-bit");
	bool bNoSeparator = c->switches()->find('n',"no-separator");
	bool bAddCR = c->switches()->find('c',"crlf");
	
	if(!f.open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate)))
	{
		if(!c->switches()->find('q',"quiet"))
			c->warning(__tr2qs("Can't open the file '%Q' for writing"),&szFile);
		return true;
	}

	unsigned int u = 0;
	while(u < a.array()->size())
	{
		KviKvsVariant * v = a.array()->at(u);
		QByteArray dat;
		if(v)
		{
			QString szDat;
			v->asString(szDat);
			dat = bLocal8Bit ? szDat.toLocal8Bit() : szDat.toUtf8();

			if(!bNoSeparator)
			{
				if(bAddCR)
					dat += "\r\n";
				else
					dat += '\n';
			}
			f.write(dat.data(),dat.length());
		}
		u++;
	}

	f.close();

	return true;
}

/*
	@doc: file.localdir
	@type:
		function
	@title:
		$file.localdir
	@short:
		Get the KVIrc local directory
	@syntax:
		<string> $file.localdir([relative_path:string])
	@description:
		Returns the path to the KVIrc local data directory.[br]
		The KVIrc local data directory is always writable and contains
		the various subdirectories that KVIrc uses internally: audio,
		config, help, incoming, log, modules, msgcolors and pics.[br]
		If <relative_path> is passed, then it is appended to the directory path to
		form a complete filepath.[br]
		The path is adjusted to contain single separators suitable for the platform
		that KVIrc is atually running on (thus you do not need to care about path
		separators in the <relative_path>, KVIrc will adjust them).[br]
	@examples:
		[example]
			echo KVIrc looks for pictures in $file.localdir(pics)
			echo panic.png would be translated to $file.localdir(pics/panic.png)
		[/example]
*/
static bool file_kvs_fnc_localdir(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("relative_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
	KVSM_PARAMETERS_END(c)
	if(szName.isEmpty())
		szName.append(KVI_PATH_SEPARATOR_CHAR);
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath,KviApplication::None,szName);
	KviFileUtils::adjustFilePath(szPath);
	c->returnValue()->setString(szPath);
	return true;
}

/*
	@doc: file.homedir
	@type:
		function
	@title:
		$file.homedir
	@short:
		Get the user's HOME directory
	@syntax:
		<string> $file.homedir([relative_path:string])
	@description:
		Returns the user's HOME directory path.[br]
		If <relative_path> is passed, then it is appendedto the directory path to
		form a complete filepath.[br]
		The path is adjusted to contain single separators suitable for the platform
		that KVIrc is atually running on (thus you do not need to care about path
		separators in the <relative_path>, KVIrc will adjust them).[br]
*/
static bool file_kvs_fnc_homedir(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("relative_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
	KVSM_PARAMETERS_END(c)
	if(szName.isEmpty())
		szName.append(KVI_PATH_SEPARATOR_CHAR);
	QString szPath = QDir::homePath();
	KviQString::ensureLastCharIs(szPath,KVI_PATH_SEPARATOR_CHAR);
	szPath.append(szName);
	KviFileUtils::adjustFilePath(szPath);
	c->returnValue()->setString(szPath);
	return true;
}

/*
	@doc: file.rootdir
	@type:
		function
	@title:
		$file.rootdir
	@short:
		Get the system root directory
	@syntax:
		$file.rootdir(<relative_path>)
		$file.rootdir
	@description:
		Returns the system's root directory (/ on UNIX and C:/ on Windows).[br]
		If <relative_path> is passed, then it is appended at the end of the directory
		to form a complete filepath.[br]
		The path is adjusted to contain single separators suitable for the platform
		that KVIrc is atually running on (thus you not need to care about path
		separators in the <relative_path>, KVIrc will adjust them).[br]
*/
static bool file_kvs_fnc_rootdir(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("relative_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
	KVSM_PARAMETERS_END(c)
	if(szName.isEmpty())
		szName.append(KVI_PATH_SEPARATOR_CHAR);
	QString szPath = QDir::rootPath();
	KviQString::ensureLastCharIs(szPath,KVI_PATH_SEPARATOR_CHAR);
	szPath.append(szName);
	KviFileUtils::adjustFilePath(szPath);
	c->returnValue()->setString(szPath);
	return true;
}

/*
	@doc: file.cwd
	@type:
		function
	@title:
		$file.currentdir
	@short:
		Get the current directory
	@syntax:
		<string> $file.cwd([relative_path:string])
	@description:
		Returns the current working directory.[br]
		If <relative_path> is passed, then it is appended at the end of the directory
		to form a complete filepath.[br]
		The path is adjusted to contain single separators suitable for the platform
		that KVIrc is actually running on (thus you do not need to care about path
		separators in the <relative_path>, KVIrc will adjust them).[br]
*/
static bool file_kvs_fnc_cwd(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("relative_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
	KVSM_PARAMETERS_END(c)
	if(szName.isEmpty())
		szName.append(KVI_PATH_SEPARATOR_CHAR);
	QString szPath = QDir::currentPath();
	KviQString::ensureLastCharIs(szPath,KVI_PATH_SEPARATOR_CHAR);
	szPath.append(szName);
	KviFileUtils::adjustFilePath(szPath);
	c->returnValue()->setString(szPath);
	return true;
}

/*
	@doc: file.globaldir
	@type:
		function
	@title:
		$file.globaldir
	@short:
		Get the KVIrc global directory
	@syntax:
		$file.globaldir(<relative_path>)
		$file.globaldir
	@description:
		Returns the path to the KVIrc global data directory.[br]
		The KVIrc local data directory is always readable (but usually not writable) and contains
		the various subdirectories that KVIrc uses internally: audio,
		config, help, incoming, log, modules, msgcolors and pics.[br]
		If <relative_path> is passed, then it is appended to the directory path to
		form a complete filepath.[br]
		The path is adjusted to contain single separators suitable for the platform
		that KVIrc is atually running on (thus you do not need to care about path
		separators in the <relative_path>, KVIrc will adjust them).[br]
	@examples:
		[example]
			echo KVIrc looks for pictures in $file.globaldir(pics)
			echo panic.png would be translated to $file.globaldir(pics/panic.png)
		[/example]
*/
static bool file_kvs_fnc_globaldir(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("relative_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
	KVSM_PARAMETERS_END(c)
	if(szName.isEmpty())
		szName.append(KVI_PATH_SEPARATOR_CHAR);
	QString szPath;
	g_pApp->getGlobalKvircDirectory(szPath,KviApplication::None,szName);
	KviFileUtils::adjustFilePath(szPath);
	c->returnValue()->setString(szPath);
	return true;
}

/*
	@doc: file.extractpath
	@type:
		function
	@title:
		$file.extractpath
	@short:
		Extract the path from a filename
	@syntax:
		<string> $file.extractpath(<filepath:string>)
	@description:
		Returns the path part of the <filepath> translated to match the current
		platform filesystem conventions.[br]
		The path will NOT contain a trailing path separator.[br]
		For example, if <filepath> is /usr/arch/mp3/Carisma_SuonoDelSilenzio.mp3 then
		this function will return /usr/arch/mp3 on UNIX and C:\usr\arch\mp3 on Windows.
	@seealso:
		[fnc]$file.extractFileName[/fnc]
*/
static bool file_kvs_fnc_extractpath(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filepath",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setString(QFileInfo(szName).absolutePath());
	return true;
}

/*
	@doc: file.extractfilename
	@type:
		function
	@title:
		$file.extractfilename
	@short:
		Extract the filename from a file path
	@syntax:
		<string> $file.extractfilename(<filepath:string>)
	@description:
		Returns the filename part of the filepath translated to match the current
		platform filesystem conventions.[br]
		For example, if <filepath> is /usr/arch/mp3/Carisma_SuonoDelSilenzio.mp3 then
		this function will return Carisma_SuonoDelSilenzio.mp3
	@seealso:
		[fnc]$file.extractfilename[/fnc]
*/
static bool file_kvs_fnc_extractfilename(KviKvsModuleFunctionCall * c)
{
	QString szName;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filepath",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSM_PARAMETERS_END(c)
	KviFileUtils::extractFileName(szName);
	KviQString::cutToLast(szName,KVI_PATH_SEPARATOR_CHAR);
	c->returnValue()->setString(szName);
	return true;
}

/*
	@doc:	file.diskspace
	@type:
		function
	@title:
		$file.diskspace
	@short:
		Return the available diskspace.
	@syntax:
		<hash> $file.diskspace([dirpath:string])
	@description:
		Returns as hash, with 'freespace' and 'totalspace' as key, the free diskspace in bytes.[br]
		Without optional dirpath parameter will be returned the current device's disk space (i.e. the disk on which KVIrc has been started).[br]
*/
static bool file_kvs_fnc_diskSpace(KviKvsModuleFunctionCall * c)
{
	QString szPath;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("dir_path",KVS_PT_STRING,KVS_PF_OPTIONAL,szPath)
	KVSM_PARAMETERS_END(c)
	if(szPath.isEmpty())
		szPath = ".";
	
	const char * pcPath = szPath.toUtf8().data();
	long long int fTotal;
	long long int fFree;
	// this for win
	#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		ULARGE_INTEGER free,total;
		GetDiskFreeSpaceExA( (LPCTSTR)pcPath , NULL , &total , &free );
		fFree = free.QuadPart;
		fTotal = total.QuadPart;
	#else
		// this one for linux and macos
		struct statvfs stFileSystem;
		statvfs(pcPath,&stFileSystem);
		fFree = stFileSystem.f_bavail*stFileSystem.f_bsize;
		fTotal = stFileSystem.f_blocks*stFileSystem.f_bsize;
	#endif
	KviKvsHash * pHash = new KviKvsHash();
	pHash->set("freespace",new KviKvsVariant((kvs_int_t) fFree));
	pHash->set("totalspace",new KviKvsVariant((kvs_int_t) fTotal));
	c->returnValue()->setHash(pHash);
	return true;
}

/*
	@doc: file.digest
	@type:
		function
	@title:
		$file.digest
	@short:
		Returns the sum of the character codes of the file
	@syntax:
		<string> $file.digest(<file:string>[,<algorithm:string>])
	@description:
		Calculates a digest for the file identified by the given string using the algorithm
		passed as 2nd argument.
		Currently supported: md5 (default), md4, md2, sha1, mdc2, ripemd160 and dss1.
		Requires OpenSSL support, but offers a minimal set of hashes in any case.
*/
static bool file_kvs_fnc_digest(KviKvsModuleFunctionCall * c)
{
	QString szFile,szAlgo,szResult;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("data",KVS_PT_NONEMPTYSTRING,0,szFile)
		KVSM_PARAMETER("algorithm",KVS_PT_STRING,KVS_PF_OPTIONAL,szAlgo)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szFile);

	QFile file(szFile);
	if(!file.open(QIODevice::ReadOnly))
	{
		c->warning(__tr2qs("Can't open the file '%Q' for reading"),&szFile);
		return true;
	}
	
	QByteArray content = file.readAll();
	if(content.isEmpty())
	{
		c->warning(__tr2qs("Can't read data from file"));
		return true;
	}

#if defined(COMPILE_SSL_SUPPORT)
	if(szAlgo.isEmpty())
		szAlgo = "md5";

	EVP_MD_CTX mdctx;
	const EVP_MD * pMD;
	unsigned char ucMDValue[EVP_MAX_MD_SIZE];
	unsigned int uMDLen, u;
	char cBuffer[3];
	OpenSSL_add_all_digests();

	pMD = EVP_get_digestbyname(szAlgo.toUtf8().data());
	if(!pMD)
	{
		c->warning(__tr2qs("'%Q' algorithm is not supported"),&szAlgo);
		return true;
	}

	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, pMD, NULL);
	EVP_DigestUpdate(&mdctx, content.constData(), content.size());
	EVP_DigestFinal_ex(&mdctx, ucMDValue, &uMDLen);
	EVP_MD_CTX_cleanup(&mdctx);

	for(u = 0; u < uMDLen; u++)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		_snprintf(cBuffer,3,"%02x",ucMDValue[u]);
#else
		snprintf(cBuffer,3,"%02x",ucMDValue[u]);
#endif
		szResult.append(cBuffer);
	}
#else // fall back to QCryptographicHash
	QCryptographicHash::Algorithm qAlgo;
	if(szAlgo.toLower() == "sha1")
	{
		qAlgo = QCryptographicHash::Sha1;
	} else if(szAlgo.toLower() == "md4")
	{
		qAlgo = QCryptographicHash::Md4;
	} else if(szAlgo.toLower() == "md5")
	{
		qAlgo = QCryptographicHash::Md5;
	} else {
		c->warning(__tr2qs("KVIrc is compiled without Crypto++ or OpenSSL support. $file.digest supports only MD4, MD5 and SHA1."));
		return true;
	}

	szResult.append(QCryptographicHash::hash(content,qAlgo).toHex());
#endif

	c->returnValue()->setString(szResult);

	return true;
}

/*
	@doc: file.time
	@type:
		function
	@title:
		$file.time
	@short:
		Returns the datetime of a file in unixtime format
	@syntax:
		<integer> $file.time(<file path:string>[,<time type:string>])
	@description:
		Returns the datetime of a file as the number of milliseconds that have passed since 1970-01-01T00:00:00.000, Coordinated Universal Time (UTC).[br]
		The path should be given in UNIX-style and is adjusted according to the system that KVIrc is running on.[br]
		The type have to be:[br]
		  "a" to retrieve the last time the file was accessed;[br]
		  "c" to retrieve the creation time of the file (on most UNIX systems, when the creation time or the last status change time - e.g. changing file permissions - are not available, it will fallback to "m";[br]
		  "m" to retrieve the time of the last modification of the file. This is the default.
	@examples:
		[example]
			# Windows
			echo $file.time(c:/file.txt);
			# Linux, other unixes
			echo $file.time(/etc/passwd,a);
		[/example]
*/
static bool file_kvs_fnc_time(KviKvsModuleFunctionCall * c)
{
	QString szName, szType;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_STRING,0,szName)
		KVSM_PARAMETER("type",KVS_PT_STRING,KVS_PF_OPTIONAL,szType)
	KVSM_PARAMETERS_END(c)

	KviFileUtils::adjustFilePath(szName);
	QFileInfo f(szName);

	QDateTime time;

	// Check type of time
	if(szType.isEmpty())
		szType = "m";

	if(szType.toLower() == "a")
	{
		time = f.lastRead();
	} else if(szType.toLower() == "c")
	{
		time = f.created();
	} else if(szType.toLower() == "m")
	{
		time = f.lastModified();
	} else {
		c->warning(__tr2qs("Unknown option '%1', defaulting to 'm'").arg(szType));
		time = f.lastModified();
	}

	c->returnValue()->setInteger(time.toMSecsSinceEpoch() / 1000);

	return true;
}

static bool file_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"addimagepath",file_kvs_cmd_addimagepath);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"copy",file_kvs_cmd_copy);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"delimagepath",file_kvs_cmd_delimagepath);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"mkdir",file_kvs_cmd_mkdir);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"remove",file_kvs_cmd_remove);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"rename",file_kvs_cmd_rename);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"rmdir",file_kvs_cmd_rmdir);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"write",file_kvs_cmd_write);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"writeBytes",file_kvs_cmd_writeBytes);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"writeLines",file_kvs_cmd_writeLines);

	KVSM_REGISTER_FUNCTION(m,"allsizes",file_kvs_fnc_allSizes);
	KVSM_REGISTER_FUNCTION(m,"cwd",file_kvs_fnc_cwd);
	KVSM_REGISTER_FUNCTION(m,"digest",file_kvs_fnc_digest);
	KVSM_REGISTER_FUNCTION(m,"diskspace",file_kvs_fnc_diskSpace);
	KVSM_REGISTER_FUNCTION(m,"exists",file_kvs_fnc_exists);
	KVSM_REGISTER_FUNCTION(m,"extractfilename",file_kvs_fnc_extractfilename);
	KVSM_REGISTER_FUNCTION(m,"extractpath",file_kvs_fnc_extractpath);
	KVSM_REGISTER_FUNCTION(m,"fixpath",file_kvs_fnc_fixpath);
	KVSM_REGISTER_FUNCTION(m,"globaldir",file_kvs_fnc_globaldir);
	KVSM_REGISTER_FUNCTION(m,"homedir",file_kvs_fnc_homedir);
	KVSM_REGISTER_FUNCTION(m,"localdir",file_kvs_fnc_localdir);
	KVSM_REGISTER_FUNCTION(m,"ls",file_kvs_fnc_ls);
	KVSM_REGISTER_FUNCTION(m,"ps",file_kvs_fnc_ps);
	KVSM_REGISTER_FUNCTION(m,"read",file_kvs_fnc_read);
	KVSM_REGISTER_FUNCTION(m,"readBytes",file_kvs_fnc_readBytes);
	KVSM_REGISTER_FUNCTION(m,"readLines",file_kvs_fnc_readLines);
	KVSM_REGISTER_FUNCTION(m,"rootdir",file_kvs_fnc_rootdir);
	KVSM_REGISTER_FUNCTION(m,"size",file_kvs_fnc_size);
	KVSM_REGISTER_FUNCTION(m,"time",file_kvs_fnc_time);
	KVSM_REGISTER_FUNCTION(m,"type",file_kvs_fnc_type);

	return true;
}

static bool file_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"File",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)" \
	"          (C) 2010 Elvio Basello (hell at hellvis69 dot netsons dot org)" \
	"          (C) 2012 Elvio Basello (hellvis69 at gmail dot com)", // author & (C)
	"Interface to the file system",
	file_module_init,
	0,
	0,
	file_module_cleanup,
	0
)
