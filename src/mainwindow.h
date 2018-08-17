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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "mapscene.h"

class LogsDialog;
class ColorAction;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow();

signals:
	void displayError(const QString &error);
	void displayWarning(const QString &error);
	void displayInfo(const QString &error);

public slots:
	// menus

	// File
	void onNew();
	void onOpen();
	void onSave();
	void onSaveAs();
	void onExport();

	// Items
	void onImportImages();

	// help
	void onLogs(bool on);
	void onAbout();
	void onAboutQt();

	// toolbar
	void onSelectButton();
	void onNumberButton();
	void onSymbolButton();
	void onZoomButton();
	void onImageButton();

	void onValidatePositionButton();

	void onValidateNumberButton();
	void onRecomputeButton();
	void onIncrementButton();
	void onFontButton();
	void onNumberColorButton();

	void onChangeImageButton();
	void onOriginForegroundColorButton();
	void onFinalForegroundColorButton();

	void onSymbolImageButton();
	void onSymbolColorButton();

	void onSymbolSizeChanged(int size);
	void onItemDetailsChanged(const MapItem::Details &details);
	void onImageAdded(const QPointF &pos);
	void onZoomChanged(qreal zoom);
	void onLoadProject();

	void setError(const QString &error);
	void setWarning(const QString &error);
	void setInfo(const QString &error);

	void doSaveWindowParameters();

protected:
	void closeEvent(QCloseEvent *e);
	void resizeEvent(QResizeEvent *e);
	void moveEvent(QMoveEvent *e);
	void changeEvent(QEvent* e);

	void initSupportedFormats(bool write);

	void getConfigFileDefaultValues();
	void setConfigFileDefaultValues();

	void saveWindowParameters();

	LogsDialog *m_logsDialog;
	MapScene *m_scene;

	ColorAction* m_numberColorAction;
	ColorAction* m_imageForegroundOriginColorAction;
	ColorAction* m_imageForegroundFinalColorAction;
	ColorAction* m_symbolColorAction;

	QString m_supportedReadFormats;
	QString m_supportedWriteFormats;

	QTimer m_windowTimer;
};

#endif
