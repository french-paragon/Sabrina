#include "editableitemfactory.h"

#include "editableitem.h"

namespace Cathia {

EditableItemFactoryManager EditableItemFactoryManager::GlobalEditableItemFactoryManager;

EditableItemFactory::EditableItemFactory(QObject *parent) : QObject(parent)
{
	_typeId = "";
}

QString EditableItemFactory::getItemTypeId() const {

	if (_typeId == "") {
		//get the type id.
		EditableItem* temp = createItem(nullptr);
		_typeId = temp->getTypeId();

		delete temp;
	}

	return _typeId;
}

EditableItemFactoryManager::EditableItemFactoryManager(QObject *parent) : QObject(parent)
{

}

void EditableItemFactoryManager::installFactory(EditableItemFactory* factory, bool takeOwnership) {

	if (hasFactoryInstalled(factory->getItemTypeId())) {
		if (takeOwnership) {
			factory->deleteLater();
		}
	}

	_installedFactories.insert(factory->getItemTypeId(), factory);
}

bool EditableItemFactoryManager::hasFactoryInstalled(QString type_id) const {

	return _installedFactories.contains(type_id);

}

EditableItem* EditableItemFactoryManager::createItem(QString type_id, EditableItemManager* parent) const {

	const EditableItemFactory* f = _installedFactories.value(type_id, nullptr);

	if (f != nullptr) {
		return f->createItem(parent);
	}

	return nullptr;

}

} // namespace Cathia
