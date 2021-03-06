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
#include "numbermapitem.h"
#include "mapscene.h"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

QFont* NumberMapItem::s_font = NULL;
QColor NumberMapItem::s_color(Qt::black);

NumberMapItem::NumberMapItem(QGraphicsItem *parent):MapItem(parent), m_number(0), m_parentId(-1)
{
}

NumberMapItem::~NumberMapItem()
{
}

void NumberMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	painter->setPen(QPen(s_color, 0));
	painter->setFont(*s_font);
	painter->drawText(option->rect, Qt::AlignCenter, QString::number(m_number));

	MapItem::paint(painter, option, widget);

	painter->restore();
}

MapItem::Details NumberMapItem::getDetails() const
{
	MapItem::Details details = MapItem::getDetails();
	details.type = MapItem::Number;
	details.number = m_number;
	return details;
}

QVariant NumberMapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange && parentItem())
	{
		QPointF newPos = value.toPointF();
		QRectF r = parentItem()->boundingRect();

		if (!r.contains(newPos))
		{
			newPos.setX(qMin(r.right(), qMax(newPos.x(), r.left())));
			newPos.setY(qMin(r.bottom(), qMax(newPos.y(), r.top())));
			return newPos;
		}
	}

	if (change == ItemPositionHasChanged && scene())
	{
		emit qobject_cast<MapScene*>(scene())->itemDetailsChanged(getDetails());
	}

	// just return the QVariant
	return MapItem::itemChange(change, value);
}

void NumberMapItem::initFont()
{
	if (!s_font) s_font = new QFont("Arial", 20);
}

void NumberMapItem::releaseFont()
{
	if (s_font)
	{
		delete s_font;
		s_font = NULL;
	}
}

int NumberMapItem::getNumber() const
{
	return m_number;
}

void NumberMapItem::setNumber(int number)
{
	m_number = number;

	updateNumber();
}

bool NumberMapItem::updateNumber()
{
	// notify that item is about to change
	prepareGeometryChange();

	QFontMetrics fm(*s_font);
	QString str = QString::number(m_number);

	QSize size = fm.size(Qt::TextSingleLine, str);
	int width = size.width() / 2 + 5;
	int height = size.height() / 2;

	m_rect.setLeft(-width);
	m_rect.setTop(-height);
	m_rect.setRight(width);
	m_rect.setBottom(height);

	createShape(m_path, m_rect);

	m_selectionPath = m_path;
	m_selectionRect = m_rect;

	update();

	return true;
}

int NumberMapItem::getParentId() const
{
	return m_parentId;
}

void NumberMapItem::setParentId(int id)
{
	m_parentId = id;
}

void NumberMapItem::setFont(const QFont &font)
{
	*s_font = font;
}
QFont NumberMapItem::getFont()
{
	return *s_font;
}

void NumberMapItem::setColor(const QColor &color)
{
	s_color = color;
}

QColor NumberMapItem::getColor()
{
	return s_color;
}

void NumberMapItem::serialize(QDataStream &stream) const
{
	MapItem::serialize(stream);

	stream << m_parentId;
	stream << m_number;
}

void NumberMapItem::unserialize(QDataStream &stream)
{
	MapItem::unserialize(stream);

	stream >> m_parentId;
	stream >> m_number;

	updateNumber();
}

QDataStream& operator << (QDataStream &stream, const NumberMapItem &item)
{
	item.serialize(stream);
	return stream;
}

QDataStream& operator >> (QDataStream &stream, NumberMapItem &item)
{
	item.unserialize(stream);
	return stream;
}
