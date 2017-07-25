#include "editableitemfactory.h"

#include "editableitem.h"

namespace Sabrina {

EditableItemFactoryManager EditableItemFactoryManager::GlobalEditableItemFactoryManager;

EditableItemFactory::EditableItemFactory(QObject *parent) : QObject(parent)
{
	_typeId = "";
	_typeName = "";
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

QString EditableItemFactory::getItemTypeName() const {

	if (_typeName == "") {
		//get the type name.
		EditableItem* temp = createItem(nullptr);
		_typeName = temp->getTypeName();

		delete temp;
	}

	return _typeName;

}

EditableItemFactoryManager::EditableItemFactoryManager(QObject *parent) : QAbstractListModel(parent)
{

}

void EditableItemFactoryManager::installFactory(EditableItemFactory* factory, bool takeOwnership) {

	if (hasFactoryInstalled(factory->getItemTypeId())) {
		if (takeOwnership) {
			factory->deleteLater();
		}
		return;
	}

	beginInsertRows(QModelIndex(), _installedFactoriesKeys.size(), _installedFactoriesKeys.size());

	_installedFactories.insert(factory->getItemTypeId(), factory);
	_installedFactoriesKeys.push_back(factory->getItemTypeId());

	endInsertRows();
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

int EditableItemFactoryManager::rowCount(const QModelIndex &parent) const {

	if (parent != QModelIndex()) {
		return 0;
	}

	return _installedFactoriesKeys.size();

}

QVariant EditableItemFactoryManager::data(const QModelIndex &index,
										  int role) const {

	int r = index.row();
	QString associatedKey = _installedFactoriesKeys.at(r);

	switch(role) {
	case Qt::DisplayRole:
		return _installedFactories.value(associatedKey)->getItemTypeName();
	case Qt::ToolTipRole:
		return associatedKey;
	default:
		break;
	}

	return QVariant();

}

} // namespace Cathia
