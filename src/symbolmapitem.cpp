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
#include "symbolmapitem.h"
#include "mapscene.h"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

int SymbolMapItem::s_size = 10;
QColor SymbolMapItem::s_color(Qt::black);

SymbolMapItem::SymbolMapItem(QGraphicsItem *parent):MapItem(parent), m_symbol(SymbolCross), m_parentId(-1)
{
}

SymbolMapItem::~SymbolMapItem()
{
}

void SymbolMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	painter->setPen(QPen(s_color, 1));

	switch (m_symbol)
	{
	case SymbolCross:
	{
		QVector<QLine> lines;
		lines << QLineF(m_rect.topLeft(), m_rect.bottomRight()).toLine();
		lines << QLineF(m_rect.topRight(), m_rect.bottomLeft()).toLine();
		painter->drawLines(lines);
		break;
	}

	case SymbolCircle:
		painter->drawEllipse(m_rect);
		break;

	case SymbolCircleFilled:
		painter->setBrush(QBrush(s_color));
		painter->drawEllipse(m_rect);
		break;

	case SymbolSquare:
		painter->drawRect(m_rect);
		break;

	case SymbolSquareFilled:
		painter->setBrush(QBrush(s_color));
		painter->drawRect(m_rect);
		break;

	case SymbolTriangle:
	{
		QPolygon polygon;
		polygon << QPoint(m_rect.left() + m_rect.right() / 2, m_rect.top());
		polygon << QPoint(m_rect.left(), m_rect.bottom());
		polygon << QPoint(m_rect.right(), m_rect.bottom());
		painter->drawPolygon(polygon);
		break;
	}

	case SymbolTriangleFilled:
	{
		painter->setBrush(QBrush(s_color));

		QPolygon polygon;
		polygon << QPoint(m_rect.left() + m_rect.right() / 2, m_rect.top());
		polygon << QPoint(m_rect.left(), m_rect.bottom());
		polygon << QPoint(m_rect.right(), m_rect.bottom());
		painter->drawPolygon(polygon);
		break;
	}
	}

	MapItem::paint(painter, option, widget);

	painter->restore();
}

MapItem::Details SymbolMapItem::getDetails() const
{
	MapItem::Details details = MapItem::getDetails();
	details.type = MapItem::Symbol;
	details.symbol = m_symbol;
	return details;
}

QVariant SymbolMapItem::itemChange(GraphicsItemChange change, const QVariant &value)
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

SymbolMapItem::Symbol SymbolMapItem::getSymbol() const
{
	return m_symbol;
}

void SymbolMapItem::setSymbol(Symbol symbol)
{
	m_symbol = symbol;

	updateSymbol();
}

bool SymbolMapItem::updateSymbol()
{
	// notify that item is about to change
	prepareGeometryChange();

	m_rect.setLeft(-s_size / 2);
	m_rect.setTop(-s_size / 2);
	m_rect.setWidth(s_size);
	m_rect.setHeight(s_size);

	m_path = QPainterPath();
	m_path.addEllipse(m_rect);

	update();

	return true;
}

int SymbolMapItem::getParentId() const
{
	return m_parentId;
}

void SymbolMapItem::setParentId(int id)
{
	m_parentId = id;
}

int SymbolMapItem::getSize()
{
	return s_size;
}

void SymbolMapItem::setSize(int size)
{
	s_size = size;
}

void SymbolMapItem::setColor(const QColor &color)
{
	s_color = color;
}

QColor SymbolMapItem::getColor()
{
	return s_color;
}

void SymbolMapItem::serialize(QDataStream &stream) const
{
	MapItem::serialize(stream);

	stream << m_parentId;
	stream << (qint32)m_symbol;
}

void SymbolMapItem::unserialize(QDataStream &stream)
{
	MapItem::unserialize(stream);

	qint32 symbol;

	stream >> m_parentId;
	stream >> symbol;

	m_symbol = (Symbol)symbol;

	updateSymbol();
}

QDataStream& operator << (QDataStream &stream, const SymbolMapItem &item)
{
	item.serialize(stream);
	return stream;
}

QDataStream& operator >> (QDataStream &stream, SymbolMapItem &item)
{
	item.unserialize(stream);
	return stream;
}
