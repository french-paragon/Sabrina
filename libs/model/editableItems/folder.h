#ifndef FOLDER_H
#define FOLDER_H

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

#include "model/editableitem.h"
#include "aline/src/model/editableitemfactory.h"

#include "model/model_global.h"

namespace Sabrina {

class CATHIA_MODEL_EXPORT Folder : public EditableItem
{
public:

	static const QString FOLDER_TYPE_ID;

	class CATHIA_MODEL_EXPORT FolderFactory : public Aline::EditableItemFactory
	{
	public:
		explicit FolderFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	Folder(QString ref, Aline::EditableItemManager* parent = nullptr);

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual bool acceptChildrens() const;

	virtual QString iconInternalUrl() const;

	virtual bool autoSave() const;

protected:

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);
};

} // namespace Sabrina

#endif // FOLDER_H
