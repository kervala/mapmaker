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
#include "coloraction.h"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

ColorAction::ColorAction(QObject *parent):QAction(tr("Select a color..."), parent), m_color(Qt::black)
{
	// set an empty pixmap by default
	QPixmap pixmap(16, 16);
	pixmap.fill(QColor(255, 0, 255));
	setPixmap(pixmap);
}

ColorAction::~ColorAction()
{
}

void ColorAction::setCurrentColor(const QColor &col)
{
	m_color = col;
	updateIcon();
}

QColor ColorAction::currentColor() const
{
	return m_color;
}

void ColorAction::setPixmap(const QPixmap &pixmap)
{
	m_originalPixmap = pixmap;
	m_originalMask = pixmap.createMaskFromColor(QColor(255, 0, 255), Qt::MaskOutColor);
	updateIcon();
}

QPixmap ColorAction::pixmap() const
{
	return m_originalPixmap;
}

void ColorAction::updateIcon()
{
	QPixmap pixmap(m_originalPixmap);
	QPixmap colorMask(pixmap.size());
	colorMask.fill(m_color);
	colorMask.setMask(m_originalMask);

	QPainter p(&pixmap);
	p.drawPixmap(0, 0, colorMask);
	p.end();

	setIcon(QIcon(pixmap));
}
