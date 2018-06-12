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

#include "personnage.h"

namespace Sabrina {


const QString Personnage::PERSONNAGE_TYPE_ID = "sabrina_jdr_personnage";

Personnage::Personnage(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent)
{
	connect(this, &Personnage::persoRaceChanged, this, &Personnage::newUnsavedChanges);
	connect(this, &Personnage::persoAgeChanged, this, &Personnage::newUnsavedChanges);
	connect(this, &Personnage::persoBackgroundChanged, this, &Personnage::newUnsavedChanges);
}

Personnage::PersonnageFactory::PersonnageFactory(QObject *parent) : EditableItemFactory(parent) {

}

Aline::EditableItem* Personnage::PersonnageFactory::createItem(QString ref, Aline::EditableItemManager* parent) const {
	return new Personnage(ref, parent);
}

QString Personnage::getTypeId() const {
	return PERSONNAGE_TYPE_ID;
}
QString Personnage::getTypeName() const {
	return tr("Personnage");
}

QString Personnage::background() const {
	return _perso_background;
}

void Personnage::setBackground(QString background) {
	if (_perso_background != background) {
		_perso_background = background;
		emit persoBackgroundChanged(_perso_background);
	}
}

QString Personnage::persoRace() const
{
	return _perso_race;
}

void Personnage::setPersoRace(QString perso_race)
{
	if (_perso_race != perso_race) {
		_perso_race = perso_race;
		emit persoRaceChanged(_perso_race);
	}
}

int Personnage::age() const
{
	return _age;
}

QString Personnage::iconInternalUrl() const {
	return ":/icons/icons/editable_item_perso.svg";
}

void Personnage::setAge(int age)
{
	if (_age != age) {
		_age = age;
		emit persoAgeChanged(_age);
	}
}

void Personnage::treatDeletedRef(QString deletedRef) {
	_perso_background.replace(deletedRef, ""); //TODO: check if we want something more complex.
}

void Personnage::treatChangedRef(QString oldRef, QString newRef) {

	_perso_background.replace(oldRef, newRef); //TODO: check if we want something more complex.

}

} // namespace Sabrina
