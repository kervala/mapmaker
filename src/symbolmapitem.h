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

#ifndef SYMBOLMAPITEM_H
#define SYMBOLMAPITEM_H

#include "mapitem.h"

class SymbolMapItem : public MapItem
{
public:
	SymbolMapItem(QGraphicsItem *parent);
	virtual ~SymbolMapItem();

	enum
	{
		Type = UserType + 3
	};

	enum Symbol
	{
		SymbolCross,
		SymbolCircle,
		SymbolCircleFilled,
		SymbolSquare,
		SymbolSquareFilled,
		SymbolTriangle,
		SymbolTriangleFilled
	};

	int type() const { return Type; }

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	Symbol getSymbol() const;
	void setSymbol(Symbol symbol);

	int getParentId() const;
	void setParentId(int id);

	MapItem::Details getDetails() const;

	void serialize(QDataStream &stream) const;
	void unserialize(QDataStream &stream);

	static void setSize(int size);
	static int getSize();

	static void setColor(const QColor &color);
	static QColor getColor();

	bool updateSymbol();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	virtual void createShape(QPainterPath &path, const QRectF &rect);

private:

	int m_parentId;
	Symbol m_symbol;

	static int s_size;
	static QColor s_color;
};

QDataStream& operator << (QDataStream &stream, const SymbolMapItem &item);
QDataStream& operator >> (QDataStream &stream, SymbolMapItem &item);

#endif
