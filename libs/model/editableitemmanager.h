#ifndef EDITABLEITEMMANAGER_H
#define EDITABLEITEMMANAGER_H

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

#include "model_global.h"

#include "aline/src/model/editableitemmanager.h"

#include <QAbstractItemModel>
#include <QMap>
#include <QException>

#include <functional>

namespace Aline {
	class EditableItem;
}

namespace Sabrina {

class EditableItem;
class EditableItemManager;
class LabelsTree;
class Label;
class NotesList;

class CATHIA_MODEL_EXPORT ItemIOException : public QException
{
public:

	ItemIOException (QString ref, QString infos, EditableItemManager const* manager);
	ItemIOException (ItemIOException const& other);

	const char* what() const throw();

	void raise() const;
	ItemIOException *clone() const;

	QString ref() const;
	QString infos() const;
	const EditableItemManager *manager() const;

protected:

	QString _ref;
	QString _infos;
	std::string _what;
	EditableItemManager const* _manager;
};

class CATHIA_MODEL_EXPORT EditableItemManager : public Aline::EditableItemManager
{
	Q_OBJECT
public:

	explicit EditableItemManager(QObject *parent = nullptr);

	NotesList *noteList() const;

signals:

public slots:

protected:

	NotesList* _noteList;

};

} // namespace Cathia

#endif // EDITABLEITEMMANAGER_H
