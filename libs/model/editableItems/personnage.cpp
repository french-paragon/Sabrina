#include "personnage.h"

namespace Sabrina {


const QString Personnage::PERSONNAGE_TYPE_ID = "sabrina_jdr_personnage";

Personnage::Personnage(QString ref, EditableItemManager *parent) :
	EditableItem(ref, parent)
{

}

Personnage::PersonnageFactory::PersonnageFactory(QObject *parent) : EditableItemFactory(parent) {

}

EditableItem* Personnage::PersonnageFactory::createItem(QString ref, EditableItemManager* parent) const {
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
