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
#include "mapitem.h"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

MapItem::MapItem(QGraphicsItem *parent):QGraphicsItem(parent), m_id(0)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsSelectable);
}

MapItem::~MapItem()
{
}

int MapItem::getId() const
{
	return m_id;
}

void MapItem::setId(int id)
{
	m_id = id;
}

QRectF MapItem::boundingRect() const
{
	return m_selectionRect;
}

QPainterPath MapItem::shape() const
{
	return m_path;
}

QSizeF MapItem::getSize() const
{
	return m_rect.size();
}

MapItem::Details MapItem::getDetails() const
{
	MapItem::Details details;
	details.type = MapItem::None;
	details.position = scenePos().toPoint();
	return details;
}

void MapItem::drawSelection(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
	const qreal penWidth = 0; // cosmetic pen
	const QColor fgcolor = option->palette.windowText().color();

	// ensure good contrast against fgcolor
	const QColor bgcolor(
		fgcolor.red()   > 127 ? 0 : 255,
		fgcolor.green() > 127 ? 0 : 255,
		fgcolor.blue()  > 127 ? 0 : 255);

	painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawPath(m_selectionPath);

	painter->setPen(QPen(option->palette.windowText(), penWidth, Qt::DashLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawPath(m_selectionPath);
}

void MapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	if (option->state & QStyle::State_Selected)
	{
		drawSelection(painter, option);
	}
}

void MapItem::createShape(QPainterPath &path, const QRectF &rect)
{
	path = QPainterPath();
	path.addRect(rect);
}

void MapItem::serialize(QDataStream &stream) const
{
	stream << m_id;
	stream << pos();
}

void MapItem::unserialize(QDataStream &stream)
{
	stream >> m_id;
	QPointF pos;
	stream >> pos;

	setPos(pos);
}
