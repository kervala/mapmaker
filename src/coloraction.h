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

#ifndef COLORACTION_H
#define COLORACTION_H

class ColorListView;
class RecentColorsView;

class ColorAction : public QAction
{
	Q_OBJECT

public:
	ColorAction(QObject *parent = NULL);
	virtual ~ColorAction();

	virtual QColor currentColor() const;
	virtual QPixmap pixmap() const;

signals:
	void colorChanged(const QColor &color);

public slots:
	virtual void setCurrentColor(const QColor &color);
	virtual void setPixmap(const QPixmap &pixmap);

private:
	void updateIcon();

	QColor m_color;

	QPixmap m_originalPixmap;
	QBitmap m_originalMask;
};

#endif
