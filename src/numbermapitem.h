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

#ifndef NUMBERMAPITEM_H
#define NUMBERMAPITEM_H

#include "mapitem.h"

class NumberMapItem : public MapItem
{
public:
	NumberMapItem(QGraphicsItem *parent);
	virtual ~NumberMapItem();

	enum { Type = UserType + 1 };

	int type() const { return Type; }

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	int getNumber() const;
	void setNumber(int number);

	int getParentId() const;
	void setParentId(int id);

	MapItem::Details getDetails() const;

	void serialize(QDataStream &stream) const;
	void unserialize(QDataStream &stream);

	static void initFont();
	static void releaseFont();

	static void setFont(const QFont &font);
	static QFont getFont();

	static void setColor(const QColor &color);
	static QColor getColor();

	bool updateNumber();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	int m_parentId;
	int m_number;

	static QFont *s_font;
	static QColor s_color;
};

QDataStream& operator << (QDataStream &stream, const NumberMapItem &item);
QDataStream& operator >> (QDataStream &stream, NumberMapItem &item);

#endif
