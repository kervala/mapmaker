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

// int
#define IMPLEMENT_INT_VAR(var) \
void ConfigFile::set##var(int var)\
{\
	if (m_##var == var) return;\
	\
	m_##var = var;\
}\
\
int ConfigFile::get##var() const\
{\
	return m_##var;\
}

// bool
#define IMPLEMENT_BOOL_VAR(var) \
void ConfigFile::set##var(bool var)\
{\
	if (m_##var == var) return;\
	\
	m_##var = var;\
}\
\
bool ConfigFile::get##var() const\
{\
	return m_##var;\
}

// generic class
#define IMPLEMENT_CLASS_VAR(c, var) \
void ConfigFile::set##var(const c &var)\
{\
	if (m_##var == var) return;\
	\
	m_##var = var;\
}\
\
c ConfigFile::get##var() const\
{\
	return m_##var;\
}

// specialized classes
#define IMPLEMENT_QSTRING_VAR(var) IMPLEMENT_CLASS_VAR(QString, var)
#define IMPLEMENT_QCOLOR_VAR(var) IMPLEMENT_CLASS_VAR(QColor, var)
#define IMPLEMENT_QFONT_VAR(var) IMPLEMENT_CLASS_VAR(QFont, var)

#define SAVE_INT_VAR(var) m_settings.setValue(#var, m_##var)
#define SAVE_BOOL_VAR(var) m_settings.setValue(#var, m_##var)
#define SAVE_QSTRING_VAR(var) m_settings.setValue(#var, m_##var)
#define SAVE_QCOLOR_VAR(var) m_settings.setValue(#var, m_##var.name())
#define SAVE_QFONT_VAR(var) m_settings.setValue(#var, m_##var.toString())

#define LOAD_INT_VAR(var) m_##var = m_settings.value(#var, m_##var).toInt()
#define LOAD_BOOL_VAR(var) m_##var = m_settings.value(#var, m_##var).toBool()
#define LOAD_QSTRING_VAR(var) m_##var = m_settings.value(#var, m_##var).toString()
#define LOAD_QCOLOR_VAR(var) m_##var.setNamedColor(m_settings.value(#var, m_##var).toString())
#define LOAD_QFONT_VAR(var) m_##var.fromString(m_settings.value(#var, m_##var).toString())

#define INIT_INT_VAR(var) m_##var = 0
#define INIT_BOOL_VAR(var) m_##var = false
#define INIT_QSTRING_VAR(var) m_##var.clear()
#define INIT_QCOLOR_VAR(var) m_##var.setRgb(0, 0, 0)
#define INIT_QFONT_VAR(var) m_##var = QFont()

ConfigFile* ConfigFile::s_instance = NULL;

ConfigFile::ConfigFile(QObject* parent):QObject(parent), m_settings(QSettings::IniFormat, QSettings::UserScope, AUTHOR, PRODUCT), m_size(0, 0), m_position(0, 0)
{
	if (!s_instance) s_instance = this;
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

	// image
	LOAD_QCOLOR_VAR(OriginForegroundColor);
	LOAD_QCOLOR_VAR(FinalForegroundColor);

	// number
	LOAD_QCOLOR_VAR(NumberColor);
	LOAD_QFONT_VAR(NumberFont);

	// symbol
	LOAD_INT_VAR(SymbolSize);
	LOAD_QCOLOR_VAR(SymbolColor);

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

	// image
	SAVE_QCOLOR_VAR(OriginForegroundColor);
	SAVE_QCOLOR_VAR(FinalForegroundColor);

	// number
	SAVE_QCOLOR_VAR(NumberColor);
	SAVE_QFONT_VAR(NumberFont);

	// symbol
	SAVE_INT_VAR(SymbolSize);
	SAVE_QCOLOR_VAR(SymbolColor);

	// window parameters
	m_settings.beginGroup("window");

	m_settings.setValue("width", m_size.width());
	m_settings.setValue("height", m_size.height());
	m_settings.setValue("x", m_position.x());
	m_settings.setValue("y", m_position.y());

	m_settings.endGroup();

	return true;
}

// image
IMPLEMENT_QCOLOR_VAR(OriginForegroundColor);
IMPLEMENT_QCOLOR_VAR(FinalForegroundColor);

// number
IMPLEMENT_QCOLOR_VAR(NumberColor);
IMPLEMENT_QFONT_VAR(NumberFont);

// symbol
IMPLEMENT_INT_VAR(SymbolSize);
IMPLEMENT_QCOLOR_VAR(SymbolColor);

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
