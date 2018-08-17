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

// int
#define DECLARE_INT_VAR(var) \
public:\
	void set##var(int var);\
	int get##var() const;\
protected:\
	int m_##var;

// bool
#define DECLARE_BOOL_VAR(var) \
public:\
	void set##var(bool var);\
	bool get##var() const;\
protected:\
	bool m_##var;

// generic class
#define DECLARE_CLASS_VAR(c, var) \
public:\
	void set##var(const c &var);\
	c get##var() const;\
protected:\
	c m_##var;

// specialized classes
#define DECLARE_QSTRING_VAR(var) DECLARE_CLASS_VAR(QString, var)
#define DECLARE_QCOLOR_VAR(var) DECLARE_CLASS_VAR(QColor, var)
#define DECLARE_QFONT_VAR(var) DECLARE_CLASS_VAR(QFont, var)

class ConfigFile : public QObject
{
	Q_OBJECT

public:
	ConfigFile(QObject* parent = NULL);
	virtual ~ConfigFile();

	static ConfigFile* getInstance() { return s_instance; }

	// image
	DECLARE_QCOLOR_VAR(OriginForegroundColor);
	DECLARE_QCOLOR_VAR(FinalForegroundColor);

	// number
	DECLARE_QCOLOR_VAR(NumberColor);
	DECLARE_QFONT_VAR(NumberFont);

	// symbol
	DECLARE_INT_VAR(SymbolSize);
	DECLARE_QCOLOR_VAR(SymbolColor);

	// files
	DECLARE_QSTRING_VAR(LastDirectory);

public:
	QSize getWindowSize() const;
	void setWindowSize(const QSize &size);

	QPoint getWindowPosition() const;
	void setWindowPosition(const QPoint &pos);

	bool isMaximized() const;
	void setMaximized(bool maximized);

public slots:
	bool load();
	bool save();

private:
	static ConfigFile* s_instance;

	bool loadVersion1();

	QSettings m_settings;

	QSize m_size;
	QPoint m_position;
	bool m_maximized;
};

#endif
