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
#include "imagemapitem.h"
#include "mapscene.h"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

QColor ImageMapItem::s_originForegroundColor;
QColor ImageMapItem::s_finalForegroundColor;

ImageMapItem::ImageMapItem(QGraphicsItem *parent):MapItem(parent)
{
	// placeholder
	updateImage();
}

ImageMapItem::~ImageMapItem()
{
}

QRectF ImageMapItem::boundingRect() const
{
	return m_rect;
}

QPainterPath ImageMapItem::shape() const
{
	return m_path;
}

void ImageMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();

	// to speed up drawing of image while resizing or rotating
//	painter->setRenderHint(QPainter::SmoothPixmapTransform, true); // put to false while resizing

	if (!m_image.isNull())
	{
		painter->drawPixmap(QPointF(0.f, 0.f), m_image);
	}
	else
	{
		painter->setPen(Qt::red);
		painter->setBrush(Qt::white);
		painter->drawRect(m_rect);

		QVector<QLineF> lines;
		lines.append(QLineF(m_rect.topLeft(), m_rect.bottomRight()));
		lines.append(QLineF(m_rect.topRight(), m_rect.bottomLeft()));

		painter->setPen(Qt::red);
		painter->drawLines(lines);
	}

//	if (m_svg.isValid()) m_svg.render(painter, m_rect);

	MapItem::paint(painter, option, widget);

	painter->restore();
}

QVariant ImageMapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionChange && scene())
	{
		// top left
		QPointF p1 = value.toPointF();

		// item size
		QSizeF size = m_rect.size();

		// bottom right
		QPointF p2 = p1 + QPointF(size.width(), size.height());

		// scene rect
		QRectF r = scene()->sceneRect();

		bool changed = false;

		if (p1.x() < 0)
		{
			p1.setX(0);

			changed = true;
		}
		else if (p2.x() > r.right())
		{
			p1.setX(r.right() - size.width());

			changed = true;
		}

		if (p1.y() < 0)
		{
			p1.setY(0);

			changed = true;
		}
		else if (p2.y() > r.bottom())
		{
			p1.setY(r.bottom() - size.height());

			changed = true;
		}

		if (changed) return p1;
	}

	if (change == ItemPositionHasChanged && scene())
	{
		MapScene::MapItemDetails details;
		details.position = value.toPoint();
		details.image = m_filename;
		details.number = -1;

		emit qobject_cast<MapScene*>(scene())->itemDetailsChanged(details);
	}

	// just return the QVariant
	return MapItem::itemChange(change, value);
}

void ImageMapItem::setFilename(const QString &filename)
{
	m_filename = filename;
}

QString ImageMapItem::getFilename() const
{
	return m_filename;
}

QSizeF ImageMapItem::getSize() const
{
	return m_rect.size();
}

bool ImageMapItem::importImage(const QString &filename)
{
	if (filename.isEmpty()) return false;

	QFile file(filename);

	if (!file.open(QFile::ReadOnly)) return false;

	m_filename = filename;
	m_rawImage = file.readAll();

	return updateImage();
}

bool ImageMapItem::updateImage()
{
	// notify that item is about to change
	prepareGeometryChange();

	if (m_rawImage.isEmpty())
	{
		m_rect.setRight(100);
		m_rect.setBottom(100);
	}
	else
	{
		// reload image
		if (!m_image.loadFromData(m_rawImage)) return false;

		// colorize image
		if (s_finalForegroundColor.isValid())
		{

			QBitmap mask = m_image.createMaskFromColor(s_originForegroundColor, Qt::MaskOutColor);

			QPainter p(&m_image);
			p.setPen(s_finalForegroundColor);
			p.drawPixmap(m_image.rect(), mask, mask.rect());
			p.end();
		}

		m_rect.setRight(m_image.width());
		m_rect.setBottom(m_image.height());
	}

	m_path = QPainterPath();
	m_path.addRect(m_rect);

	update();

	return true;
}

void ImageMapItem::setOriginForegroundColor(const QColor &color)
{
	s_originForegroundColor = color;
}

QColor ImageMapItem::getOriginForegroundColor()
{
	return s_originForegroundColor;
}

void ImageMapItem::setFinalForegroundColor(const QColor &color)
{
	s_finalForegroundColor = color;
}

QColor ImageMapItem::getFinalForegroundColor()
{
	return s_finalForegroundColor;
}

void ImageMapItem::serialize(QDataStream &stream) const
{
	MapItem::serialize(stream);

	stream << m_filename;
	stream << m_rawImage;
}

void ImageMapItem::unserialize(QDataStream &stream)
{
	MapItem::unserialize(stream);

	stream >> m_filename;
	stream >> m_rawImage;

	updateImage();
}

QDataStream& operator << (QDataStream &stream, const ImageMapItem &item)
{
	item.serialize(stream);
	return stream;
}

QDataStream& operator >> (QDataStream &stream, ImageMapItem &item)
{
	item.unserialize(stream);
	return stream;
}
