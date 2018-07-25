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
#include "commandline.h"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef QT_STATICPLUGIN

#include <QtPlugin>

#ifdef Q_OS_WIN32
	Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
	Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
#endif
	
#ifdef Q_OS_MAC
	Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#endif

	Q_IMPORT_PLUGIN(QSvgPlugin)
	Q_IMPORT_PLUGIN(QSvgIconPlugin)

#endif

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

int main(int argc, char *argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Q_INIT_RESOURCE(resources);

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
	QScopedPointer<QCoreApplication> app(new QApplication(argc, argv));
#else
	QScopedPointer<QCoreApplication> app(getenv("DISPLAY") ? new QApplication(argc, argv):new QCoreApplication(argc, argv));
#endif

	QCoreApplication::setApplicationName(PRODUCT);
	QCoreApplication::setOrganizationName(AUTHOR);
	QCoreApplication::setApplicationVersion(VERSION);

	QLocale locale = QLocale::system();

	QString folder;
	QDir dir(QCoreApplication::applicationDirPath());
	
#if defined(Q_OS_WIN32)
	folder = dir.absolutePath();
#else
	dir.cdUp();

#ifdef Q_OS_MAC
	folder = dir.absolutePath() + "/Resources";
#elif defined(SHARE_PREFIX)
	folder = SHARE_PREFIX;
#else
	folder = QString("%1/share/%2").arg(dir.absolutePath()).arg(TARGET);
#endif

#endif

	folder += "/translations";

	// load application translations
	QTranslator localTranslator;
	if (localTranslator.load(locale, TARGET, "_", folder))
	{
		QCoreApplication::installTranslator(&localTranslator);
	}

	// load Qt default translations
	QTranslator qtTranslator;
	if (qtTranslator.load(locale, "qt", "_", folder))
	{
		QCoreApplication::installTranslator(&qtTranslator);
	}

	CommandLine cmd;

	QStringList argList = app->arguments();
	argList.pop_front();

	if (cmd.parseArguments(argList))
	{
		if (cmd.processCommand())
		{
			// only memory leaks are from plugins
			return app->exec();
		}
	}
	else
	{
		if (qobject_cast<QApplication *>(app.data()))
		{
			QApplication::setWindowIcon(QIcon(":/icons/icon.svg"));

			MainWindow mainWindow;
			mainWindow.setWindowTitle(QString("%1 %2").arg(app->applicationName()).arg(app->applicationVersion()));
			mainWindow.show();

			// only memory leaks are from plugins
			return app->exec();
		}
	}

	return 1;
}
