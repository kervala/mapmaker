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
#include "logsdialog.h"
#include "moc_logsdialog.cpp"

#ifdef DEBUG_NEW
	#define new DEBUG_NEW
#endif

LogsDialog::LogsDialog(QWidget* parent):QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	setupUi(this);

	QTextDocument *doc = new QTextDocument(this);
	doc->setDefaultStyleSheet(".error { color: #f00; }\n.warning { color: #f80; }\n.info { }\n");
	logsEdit->setDocument(doc);
}

void LogsDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	emit windowVisibility(false);
}

void LogsDialog::addLog(const QString &style, const QString &text)
{
	logsEdit->append(QString("<div class='%2'>[%1] %3</div>").arg(QDateTime::currentDateTime().toString(Qt::ISODate).replace('T', ' ')).arg(style).arg(text));
	logsEdit->moveCursor(QTextCursor::End);
	logsEdit->ensureCursorVisible();
}

void LogsDialog::addError(const QString &error)
{
	addLog("error", error);

	emit windowVisibility(true);
}

void LogsDialog::addWarning(const QString &warning)
{
	addLog("warning", warning);
}

void LogsDialog::addInfo(const QString &info)
{
	addLog("info", info);
}
