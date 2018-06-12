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

#include "folder.h"

namespace Sabrina {

const QString Folder::FOLDER_TYPE_ID = "sabrina_general_folder";

Folder::Folder(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent)
{
}

QString Folder::getTypeId() const {

	return FOLDER_TYPE_ID;

}

QString Folder::getTypeName() const {

	return tr("Folder");

}

bool Folder::acceptChildrens() const {
	return true; //folder accept all childrens.
}

QString Folder::iconInternalUrl() const {
	return ":/icons/icons/editable_item_folder.svg";
}

bool Folder::autoSave() const {
	return true;
}

void Folder::treatDeletedRef(QString deletedRef) {

	(void) deletedRef;
	//Do nothing, since a folder is nothing more than a children container.

}

void Folder::treatChangedRef(QString oldRef, QString newRef) {

	(void) oldRef;
	(void) newRef;
	//Do nothing, since a folder is nothing more than a children container.

}

Folder::FolderFactory::FolderFactory(QObject *parent) :
	EditableItemFactory(parent)
{

}

Aline::EditableItem* Folder::FolderFactory::createItem(QString ref, Aline::EditableItemManager* parent) const {
	return new Folder(ref, parent);
}

} // namespace Sabrina
