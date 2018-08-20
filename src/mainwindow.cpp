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
#include "coloraction.h"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

MainWindow::MainWindow():QMainWindow(), m_logsDialog(NULL), m_scene(NULL), m_numberColorAction(NULL), m_imageForegroundOriginColorAction(NULL), m_imageForegroundFinalColorAction(NULL)
{
	setupUi(this);

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
	connect(symbolButton, SIGNAL(clicked()), this, SLOT(onSymbolButton()));
	connect(zoomButton, SIGNAL(clicked()), this, SLOT(onZoomButton()));

	// position button
	connect(validatePositionButton, SIGNAL(clicked()), this, SLOT(onValidatePositionButton()));

	// images
	connect(changeImageButton, SIGNAL(clicked()), this, SLOT(onChangeImageButton()));

	// numbers
	connect(validateNumberButton, SIGNAL(clicked()), this, SLOT(onValidateNumberButton()));
	connect(recomputeButton, SIGNAL(clicked()), this, SLOT(onRecomputeButton()));
	connect(incrementButton, SIGNAL(clicked()), this, SLOT(onIncrementButton()));
	connect(fontButton, SIGNAL(clicked()), this, SLOT(onFontButton()));

	// symbols
	connect(symbolCrossButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));
	connect(symbolCircleButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));
	connect(symbolCircleFilledButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));
	connect(symbolSquareButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));
	connect(symbolSquareFilledButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));
	connect(symbolTriangleButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));
	connect(symbolTriangleFilledButton, SIGNAL(clicked()), this, SLOT(onSymbolImageButton()));

	connect(symbolSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onSymbolSizeChanged(int)));

	// color actions
	m_numberColorAction = new ColorAction(this);
	m_numberColorAction->setPixmap(QPixmap(":/icons/color.png"));
	m_numberColorAction->setCurrentColor(Qt::white);

	numberColorButton->setDefaultAction(m_numberColorAction);

	connect(m_numberColorAction, SIGNAL(triggered()), this, SLOT(onNumberColorButton()));

	m_imageForegroundOriginColorAction = new ColorAction(this);
	m_imageForegroundOriginColorAction->setPixmap(QPixmap(":/icons/color.png"));
	m_imageForegroundOriginColorAction->setCurrentColor(Qt::black);

	originForegroundColorButton->setDefaultAction(m_imageForegroundOriginColorAction);

	connect(m_imageForegroundOriginColorAction, SIGNAL(triggered()), this, SLOT(onOriginForegroundColorButton()));

	m_imageForegroundFinalColorAction = new ColorAction(this);
	m_imageForegroundFinalColorAction->setPixmap(QPixmap(":/icons/color.png"));
	m_imageForegroundFinalColorAction->setCurrentColor(Qt::white);

	finalForegroundColorButton->setDefaultAction(m_imageForegroundFinalColorAction);

	connect(m_imageForegroundFinalColorAction, SIGNAL(triggered()), this, SLOT(onFinalForegroundColorButton()));

	m_symbolColorAction = new ColorAction(this);
	m_symbolColorAction->setPixmap(QPixmap(":/icons/color.png"));
	m_symbolColorAction->setCurrentColor(Qt::white);

	symbolColorButton->setDefaultAction(m_symbolColorAction);

	connect(m_symbolColorAction, SIGNAL(triggered()), this, SLOT(onSymbolColorButton()));

	// images
	connect(imageButton, SIGNAL(clicked()), this, SLOT(onImageButton()));

	setInfo(tr("Starting %1").arg(PRODUCT));

	m_scene = new MapScene(this);

	graphicsView->setScene(m_scene);
	graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	graphicsView->verticalScrollBar()->setSliderPosition(1);
	graphicsView->horizontalScrollBar()->setSliderPosition(1);

	// scene
	connect(m_scene, SIGNAL(itemDetailsChanged(MapItem::Details)), this, SLOT(onItemDetailsChanged(MapItem::Details)));
	connect(m_scene, SIGNAL(imageAdded(QPointF)), this, SLOT(onImageAdded(QPointF)));
	connect(m_scene, SIGNAL(zoomChanged(qreal)), this, SLOT(onZoomChanged(qreal)));
	connect(m_scene, SIGNAL(projectLoaded()), this, SLOT(onLoadProject()));

	initSupportedFormats(true);
	initSupportedFormats(false);

	positionFrame->setVisible(false);
	imageFrame->setVisible(false);
	numberFrame->setVisible(false);
	symbolFrame->setVisible(false);

	// config file
	ConfigFile *config = new ConfigFile(this);

	// call this to initialize default values at first start
	setConfigFileDefaultValues();

	// load variables from config file
	config->load();

	QSize size = ConfigFile::getInstance()->getWindowSize();
	if (!size.isNull()) resize(size);

	QPoint pos = ConfigFile::getInstance()->getWindowPosition();
	if (!pos.isNull()) move(pos);

	if (ConfigFile::getInstance()->isMaximized()) showMaximized();

	// update values from config file
	getConfigFileDefaultValues();

	// initial values
	onLoadProject();

	m_windowTimer.setSingleShot(true);

	connect(&m_windowTimer, SIGNAL(timeout()), this, SLOT(doSaveWindowParameters()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	hide();

	setConfigFileDefaultValues();

	event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
//	qDebug() << "resizeEvent maximized ?" << ConfigFile::getInstance()->isMaximized();

	saveWindowParameters();
//	ConfigFile::getInstance()->setWindowSize(e->size());

	e->accept();
}

void MainWindow::moveEvent(QMoveEvent *e)
{
//	qDebug() << "moveEvent maximized ?" << ConfigFile::getInstance()->isMaximized();

	saveWindowParameters();
//	ConfigFile::getInstance()->setWindowPosition(QPoint(x(), y()));

	e->accept();
}

void MainWindow::changeEvent(QEvent* e)
{
	if (e->type() == QEvent::WindowStateChange)
	{
		QWindowStateChangeEvent* se = static_cast<QWindowStateChangeEvent*>(e);

		if (se->oldState() == Qt::WindowNoState && windowState() == Qt::WindowMaximized)
		{
//			ConfigFile::getInstance()->setMaximized(true);
			saveWindowParameters();
		}
		else
		{
//			ConfigFile::getInstance()->setMaximized(false);
			saveWindowParameters();
		}
	}
}

void MainWindow::saveWindowParameters()
{
	// disable previous timers
	if (m_windowTimer.isActive()) m_windowTimer.stop();

	// save window parameters with a delay
	m_windowTimer.start(250);
}

void MainWindow::doSaveWindowParameters()
{
	// never save parameters when minimized
	if (isMinimized()) return;

	if (isMaximized())
	{
		// if maximized, don't save windows position and size
		ConfigFile::getInstance()->setMaximized(true);
	}
	else
	{
		ConfigFile::getInstance()->setMaximized(false);

		ConfigFile::getInstance()->setWindowPosition(pos());
		ConfigFile::getInstance()->setWindowSize(size());
	}
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

void MainWindow::getConfigFileDefaultValues()
{
	// image
	ImageMapItem::setOriginForegroundColor(ConfigFile::getInstance()->getOriginForegroundColor());
	ImageMapItem::setFinalForegroundColor(ConfigFile::getInstance()->getFinalForegroundColor());

	// number
	NumberMapItem::setColor(ConfigFile::getInstance()->getNumberColor());
	NumberMapItem::setFont(ConfigFile::getInstance()->getNumberFont());

	// symbol
	SymbolMapItem::setColor(ConfigFile::getInstance()->getSymbolColor());
	SymbolMapItem::setSize(ConfigFile::getInstance()->getSymbolSize());
}

void MainWindow::setConfigFileDefaultValues()
{
	// image
	ConfigFile::getInstance()->setOriginForegroundColor(ImageMapItem::getOriginForegroundColor());
	ConfigFile::getInstance()->setFinalForegroundColor(ImageMapItem::getFinalForegroundColor());

	// number
	ConfigFile::getInstance()->setNumberColor(NumberMapItem::getColor());
	ConfigFile::getInstance()->setNumberFont(NumberMapItem::getFont());

	// symbol
	ConfigFile::getInstance()->setSymbolColor(SymbolMapItem::getColor());
	ConfigFile::getInstance()->setSymbolSize(SymbolMapItem::getSize());
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
	QString filename = m_scene->getFilename();

	// if no project has been loaded, use last saved directory
	if (filename.isEmpty()) filename = ConfigFile::getInstance()->getLastDirectory();

	filename = QFileDialog::getOpenFileName(this, tr("Open project"), filename, tr("Map files (*.kmf)"));

	if (filename.isEmpty())
		return;

	if (!m_scene->load(filename))
	{
		setError(tr("Unable to load %1").arg(filename));
	}
	else
	{
		ConfigFile::getInstance()->setLastDirectory(QFileInfo(filename).absolutePath());
	}
}

void MainWindow::onSave()
{
	bool saved = false;

	if (m_scene->hasFilename())
	{
		if (m_scene->save()) saved = true;
	}

	if (!saved) onSaveAs();
}

void MainWindow::onSaveAs()
{
	QString filename = m_scene->getFilename();

	// if no project has been loaded, use last saved directory
	if (filename.isEmpty()) filename = ConfigFile::getInstance()->getLastDirectory();

	filename = QFileDialog::getSaveFileName(this, tr("Save project"), filename, tr("Map files (*.kmf)"));
	
	if (filename.isEmpty())
		return;

	if (!m_scene->save(filename))
	{
		setError(tr("Unable to save %1").arg(filename));
	}
	else
	{
		ConfigFile::getInstance()->setLastDirectory(QFileInfo(filename).absolutePath());
	}
}

void MainWindow::onExport()
{
	QString filename = ConfigFile::getInstance()->getLastDirectory();

	filename = QFileDialog::getSaveFileName(this, tr("Export image"), filename, m_supportedWriteFormats + ";;" + tr("SVG file (*.svg)") + ";;" + tr("PDF file (*.pdf)"));

	if (filename.isEmpty())
		return;

	if (!m_scene->exportImage(filename))
	{
		setError(tr("Unable to export map to image %1").arg(filename));
	}
	else
	{
		ConfigFile::getInstance()->setLastDirectory(QFileInfo(filename).absolutePath());
	}
}

void MainWindow::onLogs(bool on)
{
	actionLogs->setChecked(on);
	m_logsDialog->setVisible(on);
}

void MainWindow::onAbout()
{
	QString br("<br>");

	QMessageBox::about(this,
		tr("About %1 %2").arg(QApplication::applicationName()).arg(QApplication::applicationVersion()),
		tr("MapMaker is a tool to compose maps from different images and annote them.") +
		br + br +
		tr("Author: %1").arg("<a href=\"http://kervala.deviantart.com\">Kervala</a><br>") +
		tr("Homepage: %1").arg("<a href=\"https://www.deviantart.com/kervala/art/MapMaker-by-Kervala-759684325\">DeviantArt</a><br>") +
		tr("Support: %1").arg("<a href=\"http://dev.kervala.net/projects/mapmaker\">http://dev.kervala.net/projects/mapmaker</a>"));
}

void MainWindow::onAboutQt()
{
	QMessageBox::aboutQt(this);
}

void MainWindow::onSelectButton()
{
	m_scene->setMode(MapScene::ModeSelect);

	MapItem::Details details;
	details.type = MapItem::None;

	onItemDetailsChanged(details);
}

void MainWindow::onImageButton()
{
	m_scene->setMode(MapScene::ModeImage);

	MapItem::Details details;
	details.type = MapItem::Image;

	onItemDetailsChanged(details);
}

void MainWindow::onNumberButton()
{
	m_scene->setMode(MapScene::ModeNumber);

	MapItem::Details details;
	details.type = MapItem::Number;
	details.number = m_scene->getNextNumber();

	onItemDetailsChanged(details);
}

void MainWindow::onSymbolButton()
{
	m_scene->setMode(MapScene::ModeSymbol);

	MapItem::Details details;
	details.type = MapItem::Symbol;
	details.symbol = m_scene->getCurrentSymbol();

	onItemDetailsChanged(details);
}

void MainWindow::onZoomButton()
{
	m_scene->setMode(MapScene::ModeZoom);

	MapItem::Details details;
	details.type = MapItem::None;

	onItemDetailsChanged(details);
}

void MainWindow::onChangeImageButton()
{
	QString filename = imageEdit->text();

	// if no project has been loaded, use last saved directory
	if (filename.isEmpty()) filename = ConfigFile::getInstance()->getLastDirectory();

	filename = QFileDialog::getOpenFileName(this, tr("Change image"), filename, m_supportedReadFormats);

	if (filename.isEmpty())
		return;

	if (!m_scene->changeSelectedImage(filename))
	{
		if (!m_scene->importImage(filename))
		{
			setError(tr("Unable to open image %1").arg(filename));
		}
		else
		{
			ConfigFile::getInstance()->setLastDirectory(QFileInfo(filename).absolutePath());
		}
	}
}

void MainWindow::onValidatePositionButton()
{
	int x = xEdit->text().toInt();
	int y = yEdit->text().toInt();

	if (x > -1 && y > -1) m_scene->validatePosition(x, y);
}

void MainWindow::onValidateNumberButton()
{
	int number = numberEdit->text().toInt();

	if (number >-1) m_scene->validateNumber(number);
}

void MainWindow::onRecomputeButton()
{
	m_scene->recomputeNumbers();
}

void MainWindow::onIncrementButton()
{
	int number = numberEdit->text().toInt();

	if (number >-1) m_scene->incrementNumber(number);
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

void MainWindow::onNumberColorButton()
{
	QColor oldColor = NumberMapItem::getColor();

	QColor newColor = QColorDialog::getColor(oldColor, this, tr("Choose color for numbers"));

	if (oldColor != newColor)
	{
		m_numberColorAction->setCurrentColor(newColor);

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
		m_imageForegroundOriginColorAction->setCurrentColor(newColor);

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
		m_imageForegroundFinalColorAction->setCurrentColor(newColor);

		ImageMapItem::setFinalForegroundColor(newColor);

		m_scene->updateImages();
	}
}

void MainWindow::onSymbolImageButton()
{
	QObject *button = sender();

	if (button == symbolCrossButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolCross);
	if (button == symbolCircleButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolCircle);
	if (button == symbolCircleFilledButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolCircleFilled);
	if (button == symbolSquareButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolSquare);
	if (button == symbolSquareFilledButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolSquareFilled);
	if (button == symbolTriangleButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolTriangle);
	if (button == symbolTriangleFilledButton) m_scene->setCurrentSymbol(SymbolMapItem::SymbolTriangleFilled);
}

void MainWindow::onSymbolColorButton()
{
	QColor oldColor = SymbolMapItem::getColor();

	QColor newColor = QColorDialog::getColor(oldColor, this, tr("Choose color for symbols"));

	if (oldColor != newColor)
	{
		m_symbolColorAction->setCurrentColor(newColor);

		SymbolMapItem::setColor(newColor);

		m_scene->updateSymbols();
	}
}

void MainWindow::onSymbolSizeChanged(int size)
{
	SymbolMapItem::setSize(size);

	m_scene->updateSymbols();
}

void MainWindow::onItemDetailsChanged(const MapItem::Details &details)
{
	if (details.type == MapItem::None)
	{
		// unselected
		positionFrame->setVisible(false);
		imageFrame->setVisible(false);
		numberFrame->setVisible(false);
		symbolFrame->setVisible(false);
	}
	else
	{
		positionFrame->setVisible(true);

		// selected
		xEdit->setText(QString::number(details.position.x()));
		yEdit->setText(QString::number(details.position.y()));

		if (details.type == MapItem::Image)
		{
			imageEdit->setText(details.image);

			imageFrame->setVisible(true);
			numberFrame->setVisible(false);
			symbolFrame->setVisible(false);
		}
		else if (details.type == MapItem::Symbol)
		{
			switch (details.symbol)
			{
			case SymbolMapItem::SymbolCross:
				symbolCrossButton->setChecked(true);
				break;

			case SymbolMapItem::SymbolCircle:
				symbolCircleButton->setChecked(true);
				break;

			case SymbolMapItem::SymbolCircleFilled:
				symbolCircleFilledButton->setChecked(true);
				break;

			case SymbolMapItem::SymbolSquare:
				symbolSquareButton->setChecked(true);
				break;

			case SymbolMapItem::SymbolSquareFilled:
				symbolSquareFilledButton->setChecked(true);
				break;

			case SymbolMapItem::SymbolTriangle:
				symbolTriangleButton->setChecked(true);
				break;

			case SymbolMapItem::SymbolTriangleFilled:
				symbolTriangleFilledButton->setChecked(true);
				break;
			}

			imageFrame->setVisible(false);
			numberFrame->setVisible(false);
			symbolFrame->setVisible(true);
		}
		else if (details.type == MapItem::Number)
		{
			numberEdit->setText(QString::number(details.number));

			imageFrame->setVisible(false);
			numberFrame->setVisible(true);
			symbolFrame->setVisible(false);
		}
	}
}

void MainWindow::onImageAdded(const QPointF &pos)
{
	QString filename = ConfigFile::getInstance()->getLastDirectory();

	filename = QFileDialog::getOpenFileName(this, tr("Select image"), filename, m_supportedReadFormats);

	if (filename.isEmpty())
		return;

	if (!m_scene->importImage(filename, pos))
	{
		setError(tr("Unable to open image %1").arg(filename));
	}
	else
	{
		ConfigFile::getInstance()->setLastDirectory(QFileInfo(filename).absolutePath());
	}
}

void MainWindow::onZoomChanged(qreal zoom)
{
	graphicsView->scale(zoom, zoom);
}

void MainWindow::onLoadProject()
{
	// update color icons
	m_numberColorAction->setCurrentColor(NumberMapItem::getColor());
	m_imageForegroundFinalColorAction->setCurrentColor(ImageMapItem::getFinalForegroundColor());
	m_imageForegroundOriginColorAction->setCurrentColor(ImageMapItem::getOriginForegroundColor());
	m_symbolColorAction->setCurrentColor(SymbolMapItem::getColor());

	symbolSizeSpinBox->setValue(SymbolMapItem::getSize());
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
	QString filename = ConfigFile::getInstance()->getLastDirectory();

	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Import images"), filename, m_supportedReadFormats);

	if (filenames.isEmpty())
		return;

	m_scene->importImages(filenames);
}
