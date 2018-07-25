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

#ifndef MAPSCENE_H
#define MAPSCENE_H

class MapScene : public QGraphicsScene
{
	Q_OBJECT

public:
	enum Mode
	{
		ModeSelect,
		ModeNumber
	};

	struct MapItemDetails
	{
		QPoint position;
		int number;
		QString image;
	};

	MapScene(QObject *parent);
	virtual ~MapScene();

	void reset();
	bool isEmpty() const;

	Mode getMode() const;
	void setMode(Mode mode);

	QString getFilename() const;
	bool hasFilename() const;

	bool load(const QString &filename);
	bool save(const QString &filename = "");

	bool exportImage(const QString& filename);

	bool importImage(const QString &filename, const QPointF &pos = QPointF(0.0, 0.0));
	bool importImages(const QStringList &filenames);

	bool changeSelectedImage(const QString &filename);

	void updateNumbers();
	void updateImages();
	void recomputeNumbers(int number);
	void validateNumber(int number);

	static QList<QByteArray> getImagesFormats(bool write);

	static bool isImage(const QString &filename);
	static bool isProject(const QString &filename);

signals:
	void itemDetailsChanged(const MapScene::MapItemDetails &details);

public slots:
	void onSelectionChanged();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void keyPressEvent(QKeyEvent *keyEvent);

	bool acceptMimeData(const QMimeData *data) const;
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
	bool isItemChange(int type);

	Mode m_mode;
	QString m_filename;
	int m_nextNumber;
	int m_nextId;
};

#endif
