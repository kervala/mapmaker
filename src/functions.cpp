/*
 *  MapMaker is a map editor
 *  Copyright (C) 2018  Cedric OCHS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common.h"
#include "functions.h"
#include "configfile.h"

#ifdef HAVE_BOOST
#include <ctime>
#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#endif

#ifdef Q_OS_WIN32
	#include <windows.h>
#elif defined(Q_OS_MAC)
	#include <sys/mount.h>
	#include <sys/stat.h>
#else
	#include <sys/vfs.h>
	#include <sys/stat.h>
#endif

#define USE_JPEGCHECKER

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

#ifdef HAVE_BOOST
namespace boost
{
	void throw_exception(std::exception const &e)
	{
		// no exceptions
		qDebug() << "Exception: " << e.what();
	}
}
#endif

int random(unsigned int min, unsigned int max)
{
#ifdef HAVE_BOOST
	static boost::mt19937 engine(static_cast<long unsigned int>(std::time(0)));
	boost::uniform_int<unsigned int> generator(min, max);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<unsigned int> > binded(engine, generator);

	return binded();
#else
/*
	static bool seeded = false;

	if (!seeded)
	{
		qsrand(QDateTime::currentDateTime().toTime_t());
		seeded = true;
	}
*/
	return (int)((double)qrand() / ((double)RAND_MAX + 1.0) * (double)(max - min + 1)) + min;
#endif
}

QString decodeHtmlEntities(const QString &str)
{
	static QMap<QString, QString> s_entities;

	if (s_entities.empty())
	{
		s_entities["nbsp"] = " ";
		s_entities["amp"] = "&";
		s_entities["quot"] = "\"";
		s_entities["gt"] = ">";
		s_entities["lt"] = "<";
	}

	QString res;

	// search for all HTML entities
	QRegExp entities("&([#0-9a-zA-Z]+);");

	int pos = 0, lastPos = 0;

	while((pos = entities.indexIn(str, pos)) > -1)
	{
		int len = entities.matchedLength();

		// copy standard text
		res += str.mid(lastPos, pos-lastPos);

		QString entity = entities.cap(1);

		// an unicode number
		if (entity[0] == '#')
		{
			// append unicode character
			res += QChar(entity.mid(1).toInt());
		}
		else
		{
			if (s_entities.contains(entity))
			{
				// append corresponding unicode character
				res += s_entities.value(entity);
			}
			else
			{
				// append the same entity
				res += "&" + entity + ";";
			}
		}

		pos += len;
		lastPos = pos;
	}

	if (pos == -1)
	{
		res += str.mid(lastPos);
	}

	return res.simplified();
}

QString encodeHtmlEntities(const QString &str)
{
	QString res;

	int len = str.length();
	int i = 0;

	while(i < len)
	{
		QChar c = str[i];
		ushort uni = c.unicode();

		if (uni > 128)
		{
			res += "&#" + QString::number(uni) + ";";
		}
		else
		{
			res += c;
		}

		++i;
	}

	return res;
}

/// Return a readable text according to the error code submited
QString formatErrorMessage(int errorCode)
{
#ifdef _WIN32
	wchar_t *lpMsgBuf = NULL;
	DWORD len = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpMsgBuf, 0, NULL);

	// empty buffer, an error occurred
	if (len == 0) return QString("FormatMessage returned error %1").arg(GetLastError());

	// convert wchar_t* to std::string
	QString ret = QString::fromWCharArray(lpMsgBuf, len);

	// Free the buffer.
	LocalFree(lpMsgBuf);

	return ret;
#else
	return strerror(errorCode);
#endif
}

qint64 getFreeDiskSpace(const QString &path)
{
#ifdef _WIN32
	ULARGE_INTEGER free = {0};
	BOOL bRes = ::GetDiskFreeSpaceExA(QDir::toNativeSeparators(path).toLocal8Bit(), &free, NULL, NULL);
	if (!bRes)
	{
		qDebug() << "GetDiskFreeSpaceExA returned error: " << formatErrorMessage(GetLastError());
		return 0;
	}

	return free.QuadPart;
#else
	struct stat stst;
	struct statfs stfs;

	if (::stat(path.toLocal8Bit(), &stst) == -1) return 0;
	if (::statfs(path.toLocal8Bit(), &stfs) == -1) return 0;

	return stfs.f_bavail * stst.st_blksize;
#endif
}

#ifdef Q_OS_WIN
/** Return the offset in 10th of micro sec between the windows base time (
*	01-01-1601 0:0:0 UTC) and the unix base time (01-01-1970 0:0:0 UTC).
*	This value is used to convert windows system and file time back and
*	forth to unix time (aka epoch)
*/
quint64 getWindowsToUnixBaseTimeOffset()
{
	static bool init = false;

	static quint64 offset = 0;

	if (!init)
	{
		// compute the offset to convert windows base time into unix time (aka epoch)
		// build a WIN32 system time for jan 1, 1970
		SYSTEMTIME baseTime;
		baseTime.wYear = 1970;
		baseTime.wMonth = 1;
		baseTime.wDayOfWeek = 0;
		baseTime.wDay = 1;
		baseTime.wHour = 0;
		baseTime.wMinute = 0;
		baseTime.wSecond = 0;
		baseTime.wMilliseconds = 0;

		FILETIME baseFileTime = { 0,0 };
		// convert it into a FILETIME value
		SystemTimeToFileTime(&baseTime, &baseFileTime);
		offset = baseFileTime.dwLowDateTime | (quint64(baseFileTime.dwHighDateTime) << 32);

		init = true;
	}

	return offset;
}
#endif

bool setFileModificationDate(const QString &filename, const QDateTime &modTime)
{
#if defined (Q_OS_WIN)
	// Use the WIN32 API to set the file times in UTC
	wchar_t wFilename[256];
	int res = filename.toWCharArray(wFilename);

	if (res < filename.size()) return 0;

	wFilename[res] = L'\0';

	// create a file handle (this does not open the file)
	HANDLE h = CreateFileW(wFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (h == INVALID_HANDLE_VALUE)
	{
		qDebug() << QString("Can't set modification date on file '%1' (error accessing file)").arg(filename);
		return false;
	}

	FILETIME creationFileTime;
	FILETIME accessFileTime;
	FILETIME modFileTime;

	// read the current file time
	if (GetFileTime(h, &creationFileTime, &accessFileTime, &modFileTime) == 0)
	{
		qDebug() << QString("Can't set modification date on file '%1'").arg(filename);
		CloseHandle(h);
		return false;
	}

	// win32 file times are in 10th of micro sec (100ns resolution), starting at jan 1, 1601
	// hey Mr Gates, why 1601 ?

	// convert the unix time in ms to a windows file time
	quint64 t = modTime.toMSecsSinceEpoch();
	// convert to 10th of microsec
	t *= 1000;	// microsec
	t *= 10;	// 10th of micro sec (rez of windows file time is 100ns <=> 1/10 us

				// apply the windows to unix base time offset
	t += getWindowsToUnixBaseTimeOffset();

	// update the windows modTime structure
	modFileTime.dwLowDateTime = quint32(t & 0xffffffff);
	modFileTime.dwHighDateTime = quint32(t >> 32);

	// update the file time on disk
	BOOL rez = SetFileTime(h, &creationFileTime, &accessFileTime, &modFileTime);
	if (rez == 0)
	{
		qDebug() << QString("Can't set modification date on file '%1'").arg(filename);

		CloseHandle(h);
		return false;
	}

	// close the handle
	CloseHandle(h);

	return true;

#else
	// first, read the current time of the file
	struct stat buf;
	int result = stat(fn.c_str(), &buf);
	if (result != 0)
		return false;

	// prepare the new time to apply
	utimbuf tb;
	tb.actime = buf.st_atime;
	tb.modtime = modTime;
	// set eh new time
	int res = utime(fn.c_str(), &tb);
	if (res == -1)
	{
		qDebug() << QString("Can't set modification date on file '%1'").arg(filename);
	}

	return res != -1;
#endif
}

QString removeAccents(const QString &str)
{
	static QString s_diacriticLetters;
	static QStringList s_noDiacriticLetters;

	if (s_diacriticLetters.isEmpty())
	{
		s_diacriticLetters = QString::fromLatin1("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
		s_noDiacriticLetters << "S" << "OE" << "Z" << "s" << "oe" << "z" << "Y" << "Y" << "u" << "A" << "A" << "A" << "A" << "A" << "A" << "AE"
			<< "C" << "E" << "E" << "E" << "E" << "I" << "I" << "I" << "I" << "D" << "N" << "O" << "O" << "O" << "O" << "O" << "O" << "U" << "U" << "U" << "U"
			<< "Y" << "s" << "a" << "a" << "a" << "a" << "a" << "a" << "ae" << "c" << "e" << "e" << "e" << "e" << "i" << "i" << "i" << "i"
			<< "o" << "n" << "o" << "o" << "o" << "o" << "o" << "o" << "u" << "u" << "u" << "u" << "y" << "y";
	}

	QString output;

	for (int i = 0, ilen = str.length(); i < ilen; ++i)
	{
		QChar c = str[i];

		int dIndex = s_diacriticLetters.indexOf(c);

		if (dIndex < 0)
		{
			output.append(c);
		}
		else
		{
			output.append(s_noDiacriticLetters[dIndex]);
		}
	}

	return output;
}

QString removeLastPoints(const QString &str)
{
	QString res = str;

	// remove all points at the end, invalid under Windows
	while (!res.isEmpty() && res.right(1) == '.')
	{
		res.resize(res.length() - 1);
	}

	return res;
}
