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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int random(unsigned int min, unsigned int max);
QString url2path(const QString &url, const QString &field, bool createFolder = false);
QString path2url(const QString &path, const QString &field, const QString &prefix = "");

QString decodeHtmlEntities(const QString &str);
QString encodeHtmlEntities(const QString &str);

qint64 getFreeDiskSpace(const QString &path);

bool setFileModificationDate(const QString &filename, const QDateTime &modTime);
QString removeAccents(const QString &str);
QString removeLastPoints(const QString &str);

#endif
