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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#define DECLARE_QSTRING_VAR(function, var) \
public:\
	void set##function(const QString &var);\
	QString get##function() const;\
protected:\
	QString m_##var;

#define DECLARE_INT_VAR(function, var) \
public:\
	void set##function(int var);\
	int get##function() const;\
protected:\
	int m_##var;

#define DECLARE_BOOL_VAR(function, var) \
public:\
	void set##function(bool var);\
	bool get##function() const;\
protected:\
	bool m_##var;

class ConfigFile : public QObject
{
	Q_OBJECT

public:
	ConfigFile(QObject* parent);
	virtual ~ConfigFile();

	static ConfigFile* getInstance() { return s_instance; }

DECLARE_QSTRING_VAR(PathOngoing, pathOngoing);
DECLARE_QSTRING_VAR(PathArchived, pathArchived);

DECLARE_BOOL_VAR(UseProxyFiles, useProxyFiles);
DECLARE_BOOL_VAR(UseProxyOthers, useProxyOthers);

DECLARE_QSTRING_VAR(UserAgent, userAgent);

DECLARE_QSTRING_VAR(Login, login);
DECLARE_QSTRING_VAR(Password, password);

DECLARE_QSTRING_VAR(ProxyFile, proxyFile);
DECLARE_QSTRING_VAR(ProxyMd5, proxyMd5);
DECLARE_QSTRING_VAR(ProxyScript, proxyScript);
DECLARE_QSTRING_VAR(ProxyOutput, proxyOutput);
DECLARE_QSTRING_VAR(ProxyIp, proxyIp);
DECLARE_QSTRING_VAR(Proxy, proxy);

DECLARE_QSTRING_VAR(LastIsbn, lastIsbn);
DECLARE_QSTRING_VAR(LastFilter, lastFilter);

	QSize getWindowSize() const;
	void setWindowSize(const QSize &size);

	QPoint getWindowPosition() const;
	void setWindowPosition(const QPoint &pos);

public slots:
	bool load();
	bool save();

private:
	static ConfigFile* s_instance;

	bool loadVersion1();

	QSettings m_settings;

	QSize m_size;
	QPoint m_position;
};

#endif
