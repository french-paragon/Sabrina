#include "place.h"

namespace Sabrina {

const QString Place::PLACE_TYPE_ID = "sabrina_jdr_place";

Place::Place(QString ref, EditableItemManager *parent) :
	EditableItem(ref, parent)
{

}

Place::PlaceFactory::PlaceFactory(QObject *parent) : EditableItemFactory(parent) {

}

EditableItem* Place::PlaceFactory::createItem(QString ref, EditableItemManager* parent) const {
	return new Place(ref, parent);
}

QString Place::getTypeId() const {
	return QString("sabrina_jdr_lieu");
}

QString Place::getTypeName() const {
	return tr("Lieu");
}

QString Place::placeDescription() const
{
	return _place_description;
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
