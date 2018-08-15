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

#ifndef IMAGEMAPITEM_H
#define IMAGEMAPITEM_H

#include "mapitem.h"

class ImageMapItem : public MapItem
{
public:
	ImageMapItem(QGraphicsItem *parent);
	virtual ~ImageMapItem();

	enum { Type = UserType + 2 };

	int type() const { return Type; }

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setFilename(const QString &filename);
	QString getFilename() const;

	bool importImage(const QString &filename);

	MapItem::Details getDetails() const;

	void serialize(QDataStream &stream) const;
	void unserialize(QDataStream &stream);

	static void setOriginForegroundColor(const QColor &color);
	static QColor getOriginForegroundColor();

	static void setFinalForegroundColor(const QColor &color);
	static QColor getFinalForegroundColor();

	bool updateImage();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	QString m_filename;
	QPixmap m_image;
	QSvgRenderer m_svg;
	QByteArray m_rawImage;

	static QColor s_originForegroundColor;
	static QColor s_finalForegroundColor;
};

QDataStream& operator << (QDataStream &stream, const ImageMapItem &item);
QDataStream& operator >> (QDataStream &stream, ImageMapItem &item);

#endif
