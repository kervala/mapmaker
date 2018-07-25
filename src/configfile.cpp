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
#include "configfile.h"
#include "moc_configfile.cpp"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

#define IMPLEMENT_QSTRING_VAR(function, var) \
void ConfigFile::set##function(const QString &var)\
{\
	if (m_##var == var) return;\
	\
	m_##var = var;\
}\
\
QString ConfigFile::get##function() const\
{\
	return m_##var;\
}

#define IMPLEMENT_INT_VAR(function, var) \
void ConfigFile::set##function(int var)\
{\
	if (m_##var == var) return;\
	\
	m_##var = var;\
}\
\
int ConfigFile::get##function() const\
{\
	return m_##var;\
}

#define IMPLEMENT_BOOL_VAR(function, var) \
void ConfigFile::set##function(bool var)\
{\
	if (m_##var == var) return;\
	\
	m_##var = var;\
}\
\
bool ConfigFile::get##function() const\
{\
	return m_##var;\
}

ConfigFile* ConfigFile::s_instance = NULL;

ConfigFile::ConfigFile(QObject* parent):QObject(parent), m_settings(QSettings::IniFormat, QSettings::UserScope, AUTHOR, PRODUCT),
	m_size(0, 0), m_position(0, 0)
{
	if (!s_instance) s_instance = this;

	load();
}

ConfigFile::~ConfigFile()
{
	save();

	s_instance = NULL;
}

bool ConfigFile::load()
{
	int version = m_settings.value("version", 1).toInt();

	loadVersion1();

	return true;
}

bool ConfigFile::loadVersion1()
{
	// general parameters
	m_pathOngoing = m_settings.value("path_ongoing").toString();
	m_pathArchived = m_settings.value("path_archived").toString();

	m_useProxyFiles = m_settings.value("use_proxy_hd").toBool();
	m_useProxyOthers = m_settings.value("use_proxy_others").toBool();

	m_userAgent = m_settings.value("user_agent_hd").toString();

	m_login = m_settings.value("login").toString();
	m_password = m_settings.value("password").toString();

	m_proxyFile = m_settings.value("proxy_file").toString();
	m_proxyMd5 = m_settings.value("proxy_md5").toString();
	m_proxyScript = m_settings.value("proxy_script").toString();
	m_proxyOutput = m_settings.value("proxy_output").toString();
	m_proxyIp = m_settings.value("proxy_ip").toString();
	m_proxy = m_settings.value("proxy").toString();

	m_lastIsbn = m_settings.value("last_ean").toString();
	m_lastFilter = m_settings.value("last_filter").toString();

	// window parameters
	m_settings.beginGroup("window");

	m_size = QSize(m_settings.value("width", 0).toInt(), m_settings.value("height", 0).toInt());
	m_position = QPoint(m_settings.value("x", 0).toInt(), m_settings.value("y", 0).toInt());

	m_settings.endGroup();

	return true;
}

bool ConfigFile::save()
{
	// clear previous entries
	m_settings.clear();

	// general parameters
	m_settings.setValue("version", 1);

	m_settings.setValue("path_ongoing", m_pathOngoing);
	m_settings.setValue("path_archived", m_pathArchived);

	m_settings.setValue("use_proxy_hd", m_useProxyFiles);
	m_settings.setValue("use_proxy_others", m_useProxyOthers);

	m_settings.setValue("user_agent_hd", m_userAgent);

	m_settings.setValue("login", m_login);
	m_settings.setValue("password", m_password);

	m_settings.setValue("proxy_file", m_proxyFile);
	m_settings.setValue("proxy_md5", m_proxyMd5);
	m_settings.setValue("proxy_script", m_proxyScript);
	m_settings.setValue("proxy_output", m_proxyOutput);
	m_settings.setValue("proxy_ip", m_proxyIp);
	m_settings.setValue("proxy", m_proxy);

	m_settings.setValue("last_ean", m_lastIsbn);
	m_settings.setValue("last_filter", m_lastFilter);

	// window parameters
	m_settings.beginGroup("window");

	m_settings.setValue("width", m_size.width());
	m_settings.setValue("height", m_size.height());
	m_settings.setValue("x", m_position.x());
	m_settings.setValue("y", m_position.y());

	m_settings.endGroup();

	return true;
}

IMPLEMENT_QSTRING_VAR(PathOngoing, pathOngoing);
IMPLEMENT_QSTRING_VAR(PathArchived, pathArchived);

IMPLEMENT_BOOL_VAR(UseProxyFiles, useProxyFiles);
IMPLEMENT_BOOL_VAR(UseProxyOthers, useProxyOthers);

IMPLEMENT_QSTRING_VAR(UserAgent, userAgent);

IMPLEMENT_QSTRING_VAR(Login, login);
IMPLEMENT_QSTRING_VAR(Password, password);

IMPLEMENT_QSTRING_VAR(ProxyFile, proxyFile);
IMPLEMENT_QSTRING_VAR(ProxyMd5, proxyMd5);
IMPLEMENT_QSTRING_VAR(ProxyScript, proxyScript);
IMPLEMENT_QSTRING_VAR(ProxyOutput, proxyOutput);
IMPLEMENT_QSTRING_VAR(ProxyIp, proxyIp);
IMPLEMENT_QSTRING_VAR(Proxy, proxy);

IMPLEMENT_QSTRING_VAR(LastIsbn, lastIsbn);
IMPLEMENT_QSTRING_VAR(LastFilter, lastFilter);

QSize ConfigFile::getWindowSize() const
{
	return m_size;
}

void ConfigFile::setWindowSize(const QSize &size)
{
	if (m_size == size || size.width() < 10 || size.height() < 10) return;

	m_size = size;
}

QPoint ConfigFile::getWindowPosition() const
{
	return m_position;
}

void ConfigFile::setWindowPosition(const QPoint &pos)
{
	if (m_position == pos || pos.isNull()) return;

	m_position = pos;
}
