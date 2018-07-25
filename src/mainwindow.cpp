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
#include "mainwindow.h"
#include "moc_mainwindow.cpp"
#include "configfile.h"
#include "logsdialog.h"
#include "mapscene.h"
#include "numbermapitem.h"
#include "imagemapitem.h"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

MainWindow::MainWindow():QMainWindow()
{
	setupUi(this);

//	m_loginLabel = new QLabel(this);
//	m_loginLabel->setText(tr("Not logged"));

//	statusbar->addPermanentWidget(m_loginLabel);

	new ConfigFile(this);

	// init logs dialog
	m_logsDialog = new LogsDialog(this);

	// Files menu
	connect(actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
	connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));
	connect(actionExport, SIGNAL(triggered()), this, SLOT(onExport()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

	// Items menu
	connect(actionImportImages, SIGNAL(triggered()), this, SLOT(onImportImages()));

	// Help menu
	connect(actionLogs, SIGNAL(triggered(bool)), this, SLOT(onLogs(bool)));
	connect(m_logsDialog, SIGNAL(windowVisibility(bool)), this, SLOT(onLogs(bool)));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(actionAboutQt, SIGNAL(triggered()), this, SLOT(onAboutQt()));

	// tool bars
	connect(selectButton, SIGNAL(clicked()), this, SLOT(onSelectButton()));
	connect(imageButton, SIGNAL(clicked()), this, SLOT(onImageButton()));
	connect(numberButton, SIGNAL(clicked()), this, SLOT(onNumberButton()));
	connect(fontButton, SIGNAL(clicked()), this, SLOT(onFontButton()));
	connect(colorButton, SIGNAL(clicked()), this, SLOT(onColorButton()));
	connect(originForegroundColorButton, SIGNAL(clicked()), this, SLOT(onOriginForegroundColorButton()));
	connect(finalForegroundColorButton, SIGNAL(clicked()), this, SLOT(onFinalForegroundColorButton()));

	setInfo(tr("Starting %1").arg(PRODUCT));

	m_scene = new MapScene(this);
//	m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

	graphicsView->setScene(m_scene);
//	graphicsView->setCacheMode(QGraphicsView::CacheBackground);
	graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//	graphicsView->setRenderHint(QPainter::Antialiasing);
//	graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	graphicsView->verticalScrollBar()->setSliderPosition(1);
	graphicsView->horizontalScrollBar()->setSliderPosition(1);

	// scene
	connect(m_scene, SIGNAL(itemDetailsChanged(MapScene::MapItemDetails)), this, SLOT(onItemDetailsChanged(MapScene::MapItemDetails)));

	initSupportedFormats(true);
	initSupportedFormats(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	hide();

	event->accept();
}

void MainWindow::initSupportedFormats(bool write)
{
	// get supported image file types
	QStringList mimeTypeFilters;

	const QByteArrayList &supportedImageFormats = MapScene::getImagesFormats(write);

	// compose filter for all supported types
	QStringList allSupportedFormats;

	for (const QByteArray &mimeTypeFilter : supportedImageFormats)
	{
		QString ext = QString::fromLatin1(mimeTypeFilter);

		allSupportedFormats << "*." + ext;
	}

	QString supportedFormats = tr("All supported formats (%1)").arg(allSupportedFormats.join(' '));

	if (write)
	{
		m_supportedWriteFormats = supportedFormats;
	}
	else
	{
		m_supportedReadFormats = supportedFormats;
	}
}

void MainWindow::onNew()
{
	if (!m_scene->isEmpty() && QMessageBox::question(this, tr("Confirm"), tr("Do you confirm to reset this map?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		m_scene->reset();
	}
}

void MainWindow::onOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open project"), m_scene->getFilename(), tr("Map files (*.kmf)"));

	if (filename.isEmpty())
		return;

	if (!m_scene->load(filename))
	{
		setError(tr("Unable to load %1").arg(filename));
	}
}

void MainWindow::onSave()
{
	if (m_scene->hasFilename())
	{
		m_scene->save();
	}
	else
	{
		onSaveAs();
	}
}

void MainWindow::onSaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save project"), m_scene->getFilename(), tr("Map files (*.kmf)"));

	if (filename.isEmpty())
		return;

	if (!m_scene->save(filename))
	{
		setError(tr("Unable to save %1").arg(filename));
	}
}

void MainWindow::onExport()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Export image"), "", m_supportedWriteFormats);

	if (filename.isEmpty())
		return;

	if (!m_scene->exportImage(filename))
	{
		setError(tr("Unable to export map to image %1").arg(filename));
	}
}

void MainWindow::onLogs(bool on)
{
	actionLogs->setChecked(on);
	m_logsDialog->setVisible(on);
}

void MainWindow::onAbout()
{
	QMessageBox::about(this, tr("About %1 %2 by %3").arg(PRODUCT).arg(VERSION).arg(AUTHOR), tr("Map editor"));
}

void MainWindow::onAboutQt()
{
	QMessageBox::aboutQt(this);
}

void MainWindow::onSelectButton()
{
	m_scene->setMode(MapScene::ModeSelect);
}

void MainWindow::onNumberButton()
{
	m_scene->setMode(MapScene::ModeNumber);
}

void MainWindow::onImageButton()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Change image"), imageEdit->text(), m_supportedReadFormats);

	if (filename.isEmpty())
		return;

	if (!m_scene->changeSelectedImage(filename))
	{
		if (!m_scene->importImage(filename))
		{
			setError(tr("Unable to open image %1").arg(filename));
		}
	}
}

void MainWindow::onFontButton()
{
	bool ok = false;

	QFont oldFont = NumberMapItem::getFont();

	QFont newFont = QFontDialog::getFont(&ok, oldFont, this, tr("Choose font options for numbers"));

	if (ok && oldFont != newFont)
	{
		NumberMapItem::setFont(newFont);

		m_scene->updateNumbers();
	}
}

void MainWindow::onColorButton()
{
	QColor oldColor = NumberMapItem::getColor();

	QColor newColor = QColorDialog::getColor(oldColor, this, tr("Choose color for numbers"));

	if (oldColor != newColor)
	{
		NumberMapItem::setColor(newColor);

		m_scene->updateNumbers();
	}
}

void MainWindow::onOriginForegroundColorButton()
{
	QColor oldColor = ImageMapItem::getOriginForegroundColor();

	QColor newColor = QColorDialog::getColor(oldColor, this, tr("Choose color for origin foreground"));

	if (oldColor != newColor)
	{
		ImageMapItem::setOriginForegroundColor(newColor);

		m_scene->updateImages();
	}
}

void MainWindow::onFinalForegroundColorButton()
{
	QColor oldColor = ImageMapItem::getFinalForegroundColor();

	QColor newColor = QColorDialog::getColor(oldColor, this, tr("Choose color for final foreground"));

	if (oldColor != newColor)
	{
		ImageMapItem::setFinalForegroundColor(newColor);

		m_scene->updateImages();
	}
}

void MainWindow::onItemDetailsChanged(const MapScene::MapItemDetails &details)
{
	xEdit->setText(QString::number(details.position.x()));
	yEdit->setText(QString::number(details.position.y()));

	numberEdit->setText(QString::number(details.number));
	imageEdit->setText(details.image);
}

void MainWindow::setError(const QString &error)
{
	m_logsDialog->addError(error);
}

void MainWindow::setWarning(const QString &error)
{
	m_logsDialog->addWarning(error);
}

void MainWindow::setInfo(const QString &error)
{
	m_logsDialog->addInfo(error);
}

void MainWindow::onImportImages()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Import images"), "", m_supportedReadFormats);

	if (filenames.isEmpty())
		return;

	m_scene->importImages(filenames);
}
