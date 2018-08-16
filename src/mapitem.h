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

#ifndef MAPITEM_H
#define MAPITEM_H

class MapItem : public QGraphicsItem
{
public:
	MapItem(QGraphicsItem *parent);
	virtual ~MapItem();

	enum Type
	{
		None,
		Image,
		Number,
		Symbol
	};

	struct Details
	{
		int type;
		QPoint position;
		QString image;
		int number;
		int symbol;
	};

	int getId() const;
	void setId(int id);

	QRectF boundingRect() const;
	QPainterPath shape() const;

	QSizeF getSize() const;

	virtual Details getDetails() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual void serialize(QDataStream &stream) const;
	virtual void unserialize(QDataStream &stream);

protected:
	virtual void createShape(QPainterPath &path, const QRectF &rect);

	void drawSelection(QPainter *painter, const QStyleOptionGraphicsItem *option);

	int m_id;

	QPainterPath m_path;
	QPainterPath m_selectionPath;
	QRectF m_rect;
	QRectF m_selectionRect;
};

#endif
