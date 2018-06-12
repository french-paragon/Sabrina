#ifndef NOTESLIST_H
#define NOTESLIST_H

/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <QAbstractListModel>
#include <QDate>
#include <QColor>

namespace Sabrina {

class NotesList : public QAbstractListModel
{
	Q_OBJECT
public:

	enum Roles{
		TitleRole = Qt::UserRole+1,
		DateRole = Qt::UserRole+2,
		AuthorRole = Qt::UserRole+3
	};

	static QVector<int> savableRoles();

	explicit NotesList(QObject *parent = nullptr);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	virtual QHash<int, QByteArray> roleNames() const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

signals:

public slots:

protected:

	struct Note{
		QString _title;
		QString _data;
		QDateTime _date;
		QString _author;
		QColor _color;
	};

	QVector<Note> _content;
};

} //namespace Sabrina

#endif // NOTESLIST_H
