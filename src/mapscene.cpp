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
#include "mapscene.h"
#include "numbermapitem.h"
#include "imagemapitem.h"

#include <qsvggenerator.h>
#include <QtPrintSupport/qprinter.h>

struct SMagicHeader
{
	union
	{
		char str[5];
		quint32 num;
	};
};

SMagicHeader s_header = { "PAMK" };
quint32 s_version = 2;

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

MapScene::MapScene(QObject *parent):QGraphicsScene(parent), m_mode(ModeSelect), m_nextNumber(1), m_nextId(0), m_zoom(1.0), m_currentSymbol(SymbolMapItem::SymbolCross)
{
	NumberMapItem::initFont();

	setSceneRect(0, 0, 1000, 1000);

	connect(this, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));
	connect(this, SIGNAL(sceneSizeUpdated()), SLOT(updateSceneSize()));
}

MapScene::~MapScene()
{
	NumberMapItem::releaseFont();
}

void MapScene::reset()
{
	m_nextNumber = 0;

	clear();
}

bool MapScene::isEmpty() const
{
	return items().isEmpty();
}

MapScene::Mode MapScene::getMode() const
{
	return m_mode;
}

void MapScene::setMode(Mode mode)
{
	m_mode = mode;

	// TODO: put a zoom cursor if zoom mode
	// setCusor(Qt::)
}

QString MapScene::getFilename() const
{
	return m_filename;
}

bool MapScene::hasFilename() const
{
	return !m_filename.isEmpty();
}

bool MapScene::load(const QString &filename)
{
	if (filename.isEmpty()) return false;

	m_filename = filename;

	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) return false;

	reset();

	QDataStream stream(&file);

	// Read and check the header
	SMagicHeader header;

	stream >> header.num;

	if (header.num != s_header.num) return false;

	// Read the version
	quint32 version;
	stream >> version;

	if (version > s_version) return false;

	// define version for items and other serialized objects
	stream.device()->setProperty("version", version);

	stream.setVersion(QDataStream::Qt_5_6);

	QFont font;

	// numbers font
	stream >> font;
	NumberMapItem::setFont(font);

	QColor color;

	// numbers color
	stream >> color;
	NumberMapItem::setColor(color);

	// foreground color
	stream >> color;
	ImageMapItem::setOriginForegroundColor(color);

	stream >> color;
	ImageMapItem::setFinalForegroundColor(color);

	// background color
	stream >> color;

	if (version >= 2)
	{
		// symbols size
		int size;
		stream >> size;
		SymbolMapItem::setSize(size);

		// symbols color
		stream >> color;
		SymbolMapItem::setColor(color);
	}

	stream >> m_nextNumber;
	stream >> m_nextId;

	qint32 imagesCount = 0;
	stream >> imagesCount;

	QMap<int, ImageMapItem*> imagesItems;

	for (int i = 0; i < imagesCount; ++i)
	{
		ImageMapItem *imageItem = new ImageMapItem(NULL);
		stream >> *imageItem;

		imagesItems[imageItem->getId()] = imageItem;

		addItem(imageItem);
	}

	qint32 numbersCount = 0;
	stream >> numbersCount;

	QMap<int, ImageMapItem*>::iterator it;

	for (int i = 0; i < numbersCount; ++i)
	{
		NumberMapItem *numberItem = new NumberMapItem(NULL);
		stream >> *numberItem;

		it = imagesItems.find(numberItem->getParentId());

		if (it != imagesItems.end())
		{
			// found a parent
			numberItem->setParentItem(*it);
		}
		else
		{
			qDebug() << "Unable to find parent" << numberItem->getParentId();

			delete numberItem;

			continue;
		}
	}

	if (version >= 2)
	{
		qint32 symbolsCount = 0;
		stream >> symbolsCount;

		for (int i = 0; i < symbolsCount; ++i)
		{
			SymbolMapItem *symbolItem = new SymbolMapItem(NULL);
			stream >> *symbolItem;

			it = imagesItems.find(symbolItem->getParentId());

			if (it != imagesItems.end())
			{
				// found a parent
				symbolItem->setParentItem(*it);
			}
			else
			{
				qDebug() << "Unable to find parent" << symbolItem->getParentId();

				delete symbolItem;

				continue;
			}
		}
	}

	updateSceneSize();

	emit projectLoaded();

	return true;
}

bool MapScene::save(const QString &filename)
{
	QString newFilename = filename;

	// no filename found
	if (newFilename.isEmpty())
	{
		if (m_filename.isEmpty()) return false;

		newFilename = m_filename;
	}

	QFile file(newFilename);

	if (!file.open(QIODevice::WriteOnly)) return false;

	QDataStream stream(&file);

	// Write a header with a "magic number" and a version
	stream << s_header.num;
	stream << s_version;

	stream.setVersion(QDataStream::Qt_5_6);

	stream << NumberMapItem::getFont();
	stream << NumberMapItem::getColor();

	// foreground color
	stream << ImageMapItem::getOriginForegroundColor(); // src
	stream << ImageMapItem::getFinalForegroundColor(); // dst

	// background color
	stream << QColor();

	stream << SymbolMapItem::getSize();
	stream << SymbolMapItem::getColor();

	stream << m_nextNumber;
	stream << m_nextId;

	QList<ImageMapItem *> images;
	QList<NumberMapItem *> numbers;
	QList<SymbolMapItem *> symbols;

	foreach(QGraphicsItem *item, items())
	{
		switch (item->type())
		{
			case ImageMapItem::Type:
				images << qgraphicsitem_cast<ImageMapItem*>(item);
				break;

			case NumberMapItem::Type:
				numbers << qgraphicsitem_cast<NumberMapItem*>(item);
				break;

			case SymbolMapItem::Type:
				symbols << qgraphicsitem_cast<SymbolMapItem*>(item);
				break;

			default:
				qDebug() << "Unknown item";
		}
	}

	stream << images.size();

	foreach(ImageMapItem *item, images)
	{
		stream << *item;
	}

	stream << numbers.size();

	foreach(NumberMapItem *item, numbers)
	{
		stream << *item;
	}

	stream << symbols.size();

	foreach(SymbolMapItem *item, symbols)
	{
		stream << *item;
	}

	return true;
}

bool MapScene::importImage(const QString &filename, const QPointF &pos)
{
	ImageMapItem *item = new ImageMapItem(NULL);
	item->setId(m_nextId);
	item->setPos(pos);

	if (!item->importImage(filename)) return false;

	addItem(item);

	++m_nextId;

	updateSceneSize();

	return true;
}

bool MapScene::importImages(const QStringList &filenames)
{
	foreach(const QString &filename, filenames)
	{
		if (!importImage(filename)) return false;
	}

	return true;
}

bool MapScene::changeSelectedImage(const QString &filename)
{
	foreach(QGraphicsItem *item, selectedItems())
	{
		ImageMapItem *imageItem = qgraphicsitem_cast<ImageMapItem*>(item);

		if (imageItem)
		{
			// only one item should be changed
			return imageItem->importImage(filename);
		}
	}

	return false;
}

bool MapScene::getNextNumber() const
{
	return m_nextNumber;
}

SymbolMapItem::Symbol MapScene::getCurrentSymbol() const
{
	return m_currentSymbol;
}

void MapScene::setCurrentSymbol(SymbolMapItem::Symbol symbol)
{
	m_currentSymbol = symbol;
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	bool mustProcess = true;
	
	if (mouseEvent->button() == Qt::LeftButton && m_mode != ModeSelect)
	{
		if (m_mode == ModeZoom)
		{
			bool unzoom = (mouseEvent->modifiers() & Qt::ControlModifier);

			qreal zoom = unzoom ? 0.5 : 2;

			m_zoom *= zoom;

			emit zoomChanged(zoom);

			mustProcess = false;
		}
		else
		{
			QGraphicsItem *wbItem = itemAt(mouseEvent->scenePos(), QTransform());

			ImageMapItem *parentItem = qgraphicsitem_cast<ImageMapItem*>(wbItem);

			// only allow to put numbers or symbols on images
			if (parentItem)
			{
				MapItem *item = NULL;

				if (m_mode == ModeNumber)
				{
					NumberMapItem *numberItem = new NumberMapItem(parentItem);
					numberItem->setParentId(parentItem->getId());
					numberItem->setNumber(m_nextNumber);

					item = numberItem;

					++m_nextNumber;
				}
				else if (m_mode == ModeSymbol)
				{
					SymbolMapItem *symbolItem = new SymbolMapItem(parentItem);
					symbolItem->setParentId(parentItem->getId());
					symbolItem->setSymbol(m_currentSymbol);

					item = symbolItem;
				}

				if (item)
				{
					item->setId(m_nextId);
					item->setPos(parentItem->mapFromParent(mouseEvent->scenePos()));

					++m_nextId;

					mustProcess = false;
				}
			}
			else if (m_mode == ModeImage)
			{
				emit imageAdded(mouseEvent->scenePos());

				mustProcess = false;
			}
		}
	}

	// manage standard mouse events
	if (mustProcess) QGraphicsScene::mousePressEvent(mouseEvent);
}

void MapScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (m_mode == ModeZoom)
	{
		mousePressEvent(mouseEvent);
		return;
	}

	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void MapScene::keyPressEvent(QKeyEvent *keyEvent)
{
	if (keyEvent->key() == Qt::Key_Delete)
	{
		foreach(QGraphicsItem *item, selectedItems())
		{
			removeItem(item);

			// items are not deleted automatically
			delete item;
		}
	}
}

bool MapScene::isItemChange(int type)
{
	foreach(QGraphicsItem *item, selectedItems())
	{
		if (item->type() == type)
			return true;
	}

	return false;
}

QList<QByteArray> MapScene::getImagesFormats(bool write)
{
	static QList<QByteArray> readFormats = QImageReader::supportedImageFormats();
	static QList<QByteArray> writeFormats = QImageWriter::supportedImageFormats();

	return write ? writeFormats : readFormats;
}

bool MapScene::isImage(const QString &filename)
{
	QString ext = QFileInfo(filename).suffix();

	QList<QByteArray> formats = getImagesFormats(false);

	foreach(const QByteArray &format, formats)
		if (ext == format) return true;

	return false;
}

bool MapScene::isProject(const QString &filename)
{
	QString ext = QFileInfo(filename).suffix();

	return ext == "kmf";
}

void MapScene::onSelectionChanged()
{
	QList<QGraphicsItem*> items = selectedItems();

	if (!items.isEmpty())
	{
		foreach(QGraphicsItem *item, items)
		{
			// always a MapItem
			MapItem *mapItem = (MapItem*)item;

			emit itemDetailsChanged(mapItem->getDetails());
		}
	}
	else
	{
		MapItem::Details details;
		details.type = MapItem::None;

		emit itemDetailsChanged(details);
	}
}

bool MapScene::acceptMimeData(const QMimeData *data) const
{
	if (!data->hasUrls()) return false;

	QList<QUrl> urls = data->urls();

	bool ok = false;

	foreach(const QUrl &url, urls)
	{
		QString filename = url.toLocalFile();

		if (isImage(filename) || isProject(filename)) ok = true;
	}

	return ok;
}

void MapScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	if (acceptMimeData(event->mimeData()))
		event->acceptProposedAction();
	else
		event->ignore();
}

void MapScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	if (!acceptMimeData(event->mimeData())) return;

	// get the list of all files to upload
	QList<QUrl> urlList = event->mimeData()->urls();

	for (int i = 0; i < urlList.size() && i < 32; ++i)
	{
		QString filename = urlList.at(i).toLocalFile();

		if (isProject(filename))
		{
			if (load(filename))
			{
				// don't add images or other projects
				return;
			}

			qDebug() << "Couldn't load" << filename;
		}
		else if (isImage(filename))
		{
			if (!importImage(filename, event->scenePos()))
			{
				qDebug() << "Couldn't insert" << filename;
			}
		}
	}

	event->acceptProposedAction();
}

bool MapScene::exportImage(const QString& filename)
{
	QString ext = QFileInfo(filename).suffix();

	// get source and destination rectangles
	QRectF srcRect = itemsBoundingRect();
	QRectF dstRect = QRectF(QPointF(0, 0), srcRect.size());

	QPaintDevice *image = NULL;
	QSvgGenerator *svgGenerator = NULL;
	QPixmap *pixmap = NULL;
	QPrinter *printer = NULL;

	if (ext == "svg")
	{
		// create a SVG file
		svgGenerator = new QSvgGenerator();
		svgGenerator->setFileName(filename);
		svgGenerator->setTitle(tr("MapMaker SVG Generator"));
		svgGenerator->setDescription(tr("A MapMaker map exported as SVG"));
		svgGenerator->setSize(srcRect.size().toSize());
		svgGenerator->setViewBox(dstRect);

		image = svgGenerator;
	}
	else if (ext == "pdf")
	{
		// create a PDF document
		printer = new QPrinter(QPrinter::HighResolution);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setPaperSize(QPrinter::A4);
		printer->setOutputFileName(filename);
		printer->setResolution(300);
//		printer->setPageSize(QPageSize())

		image = printer;
	}
	else
	{
		// create a transparent image
		pixmap = new QPixmap(srcRect.width(), srcRect.height());
		pixmap->fill(Qt::transparent);

		image = pixmap;
	}

	// unselect all items
	QList<QGraphicsItem*> selection = selectedItems();

	foreach(QGraphicsItem *item, selection) item->setSelected(false);

	// paint the scene content on the image
	QPainter painter;
//	painter.setWindow(dstRect.toRect());
//	painter.setViewport(dstRect.toRect());
	painter.begin(image);
	render(&painter, dstRect, srcRect);
	painter.end();

	// reselect all selected items
	foreach(QGraphicsItem *item, selection) item->setSelected(true);

	delete image;

	// save the image
	return pixmap ? pixmap->save(filename):true;
}

void MapScene::updateNumbers()
{
	foreach(QGraphicsItem *item, items())
	{
		NumberMapItem *numberItem = qgraphicsitem_cast<NumberMapItem*>(item);

		if (numberItem)
		{
			numberItem->updateNumber();

			if (item->isSelected())
			{
				emit itemDetailsChanged(numberItem->getDetails());
			}
		}
	}
}

void MapScene::updateSymbols()
{
	foreach(QGraphicsItem *item, items())
	{
		SymbolMapItem *symbolItem = qgraphicsitem_cast<SymbolMapItem*>(item);

		if (symbolItem)
		{
			symbolItem->updateSymbol();

			if (item->isSelected())
			{
				emit itemDetailsChanged(symbolItem->getDetails());
			}
		}
	}
}

void MapScene::updateImages()
{
	foreach(QGraphicsItem *item, items())
	{
		ImageMapItem *imageItem = qgraphicsitem_cast<ImageMapItem*>(item);

		if (imageItem)
		{
			imageItem->updateImage();
		}
	}
}

void MapScene::recomputeNumbers()
{
	QVector<QVector<NumberMapItem*> > numberItems;

	numberItems.resize(m_nextNumber+1);

	foreach(QGraphicsItem *aitem, items())
	{
		NumberMapItem *item = qgraphicsitem_cast<NumberMapItem*>(aitem);

		if (item)
		{
			numberItems[item->getNumber()].push_back(item);
		}
	}
	
	int offset = 0;

	int lastNumber = 0;

	for (int i = 1; i < m_nextNumber; ++i)
	{
		const QVector<NumberMapItem*> &iis = numberItems[i];

		if (iis.isEmpty())
		{
			--offset;
		}
		else
		{
			if (offset)
			{
				for (int j = 0, jlen = iis.size(); j < jlen; ++j)
				{
					iis[j]->setNumber(i + offset);
				}

				lastNumber = i + offset;
			}
			else
			{
				lastNumber = i;
			}
		}
	}

	m_nextNumber = lastNumber + 1;

	updateNumbers();
}

void MapScene::validateNumber(int number)
{
	QList<QGraphicsItem*> sitems = selectedItems();

	// only if a number is selected
	if (sitems.isEmpty()) return;

	NumberMapItem *sitem = NULL;

	foreach(QGraphicsItem *item, sitems)
	{
		sitem = qgraphicsitem_cast<NumberMapItem*>(item);

		if (sitem)
		{
			sitem->setNumber(number);
			break;
		}
	}

	updateNumbers();
}

void MapScene::incrementNumber(int number)
{
	QList<QGraphicsItem*> sitems = selectedItems();

	// only if a number is selected
	if (sitems.isEmpty()) return;

	NumberMapItem *sitem = NULL;

	foreach(QGraphicsItem *item, sitems)
	{
		sitem = qgraphicsitem_cast<NumberMapItem*>(item);

		if (sitem) break;
	}

	int oldNumber = sitem->getNumber();
	int newNumber = number;

	// only accept greater numbers
	if (newNumber <= oldNumber) return;

	int offset = newNumber - oldNumber;

	QVector<QVector<NumberMapItem*> > numberItems;

	numberItems.resize(m_nextNumber + offset + 1);

	foreach(QGraphicsItem *aitem, items())
	{
		NumberMapItem *item = qgraphicsitem_cast<NumberMapItem*>(aitem);

		if (item)
		{
			numberItems[item->getNumber()].push_back(item);
		}
	}

	int lastNumber = 0;

	for (int i = oldNumber; i < m_nextNumber; ++i)
	{
		const QVector<NumberMapItem*> &iis = numberItems[i];

		if (!iis.isEmpty())
		{
			for (int j = 0, jlen = iis.size(); j < jlen; ++j)
			{
				iis[j]->setNumber(i + offset);
			}

			lastNumber = i + offset;
		}
	}

	m_nextNumber = lastNumber + 1;

	updateNumbers();
}

void MapScene::decrementNumber(int number)
{
}

void MapScene::updateSceneSize()
{
	// increase size of the scene
	QSizeF oldSize = sceneRect().size();
	QSizeF newSize = itemsBoundingRect().size() + QSizeF(1000, 1000);

	setSceneRect(QRectF(QPointF(0, 0), newSize));

	qDebug() << "old" << oldSize << "new" << newSize;
}
