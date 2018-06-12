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

#include "place.h"

namespace Sabrina {

const QString Place::PLACE_TYPE_ID = "sabrina_jdr_place";

Place::Place(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent)
{
	connect(this, &Place::placeDescriptionChanged, this, &Place::newUnsavedChanges);
}

Place::PlaceFactory::PlaceFactory(QObject *parent) : EditableItemFactory(parent) {

}

Aline::EditableItem* Place::PlaceFactory::createItem(QString ref, Aline::EditableItemManager* parent) const {
	return new Place(ref, parent);
}

QString Place::getTypeId() const {
	return PLACE_TYPE_ID;
}

QString Place::getTypeName() const {
	return tr("Lieu");
}

QString Place::placeDescription() const
{
	return _place_description;
}

QString Place::iconInternalUrl() const {
	return ":/icons/icons/editable_item_place.svg";
}

void Place::setplaceDescription(QString place_description)
{
	if (place_description != _place_description) {
		_place_description = place_description;
		emit placeDescriptionChanged(_place_description);
	}
}

void Place::treatDeletedRef(QString deletedRef) {
	_place_description.replace(deletedRef, ""); //TODO: check if we want something more complex.
}

void Place::treatChangedRef(QString oldRef, QString newRef) {

	_place_description.replace(oldRef, newRef); //TODO: check if we want something more complex.

}

} // namespace Sabrina
