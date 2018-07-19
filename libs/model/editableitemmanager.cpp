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

#include "editableitemmanager.h"

#include "editableitem.h"
#include "aline/src/model/editableitemfactory.h"

#include "labels/labelstree.h"
#include "labels/label.h"

#include "notes/noteslist.h"

namespace Sabrina {

EditableItemManager::EditableItemManager(QObject *parent) :
	Aline::EditableItemManager(parent),
	_labels(nullptr),
	_activeItem(nullptr)
{
	_noteList = new NotesList(this);
}

EditableItem* EditableItemManager::activeItem() const {
	return _activeItem;
}

LabelsTree* EditableItemManager::labelsTree() {

	if (!hasDataSource()) {
		return nullptr;
	}

	if (_labels == nullptr) {
		effectivelyLoadLabels();
	}

	return _labels;

}

NotesList *EditableItemManager::noteList() const
{
    return _noteList;
}

void EditableItemManager::setActiveItem(QString ref) {

	if (_activeItem == nullptr || _activeItem->getRef() != ref) {

		EditableItem* potential = qobject_cast<EditableItem*>(loadItem(ref));

		_activeItem = potential;
		emit activeItemChanged();

	}

}

ItemIOException::ItemIOException (QString ref,
								  QString infos,
								  EditableItemManager const* manager) :
	_ref(ref),
	_infos(infos),
	_manager(manager)
{

	QString what = "[Error while loading \"" + _ref + "\" from \"" + manager->objectName() + "\"]: " + _infos;
	_what = what.toStdString();

}

ItemIOException::ItemIOException (ItemIOException const& other) :
	_ref(other.ref()),
	_infos(other.infos()),
	_manager(other.manager())
{

}

const char* ItemIOException::what() const throw() {

	return _what.c_str();

}


QString ItemIOException::ref() const
{
	return _ref;
}

QString ItemIOException::infos() const
{
	return _infos;
}

const EditableItemManager *ItemIOException::manager() const
{
	return _manager;
}

void ItemIOException::raise() const {
	throw *this;
}

ItemIOException * ItemIOException::clone() const {
	return new ItemIOException(*this);
}

} // namespace Cathia
