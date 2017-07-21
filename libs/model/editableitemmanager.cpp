#include "editableitemmanager.h"

#include "editableitem.h"

namespace Cathia {

const QChar EditableItemManager::RefSeparator = QChar('/');

EditableItemManager::EditableItemManager(QObject *parent) : QAbstractItemModel(parent)
{

}

QModelIndex EditableItemManager::index(int row, int column, const QModelIndex &parent) const {

	if (column > 0) {
		return QModelIndex();
	}

	if (parent == QModelIndex()) {
		return createIndex(row, column, _root->_childrens.at(row));
	}

	void * dataPtr = parent.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data != nullptr) {
		if (data->_childrens.size() > row) {
			return createIndex(row, column, data->_childrens.at(row));
		}
	}

	return QModelIndex();

}
QModelIndex EditableItemManager::parent(const QModelIndex &index) const {

	void * dataPtr = index.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data != nullptr) {
		if (data->_parent != nullptr) {
			if (data->_parent != _root) {
				return createIndex(data->_parent->_parent->_childrens.indexOf(data->_parent), 0, data->_parent);
			}
		}
	}

	return QModelIndex();

}
int EditableItemManager::rowCount(const QModelIndex &parent) const {

	void * dataPtr = parent.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data != nullptr) {
		return data->_childrens.size();
	}

	return 0;

}
int EditableItemManager::columnCount(const QModelIndex &parent) const {
	(void) parent;
	return 1;
}

QVariant EditableItemManager::data(const QModelIndex &index, int role) const {

	void * dataPtr = index.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data == nullptr) {
		return QVariant();
	}

	switch (role) {
	case Qt::DisplayRole:
		return data->_name;
	case ItemRefRole:
		return data->_ref;
	case ItemAcceptChildrenRole:
		return data->_acceptChildrens;
	default:
		break;
	}

	return QVariant();
}




EditableItem* EditableItemManager::loadItem(QString const& ref) {

	if (isItemLoaded(ref)) {
		return _loadedItems.value(ref)._item;
	}

	return effectivelyLoadItem(ref);

}

QStringList EditableItemManager::loadedItems() const{
	return _loadedItems.keys();
}
void EditableItemManager::forceUnloadItem(QString ref) {

	if (!isItemLoaded(ref)) {
		return; //no need to unload a not present item.
	}

	emit itemAboutToBeUnloaded(ref);

	EditableItem* item = _loadedItems.value(ref)._item;
	_loadedItems.remove(ref);
	delete item; //effectively delete item.

	emit itemUnloaded(ref);

}

bool EditableItemManager::isItemLoaded(QString const& ref) const {
	return _loadedItems.contains(ref);
}


ItemLoadingException::ItemLoadingException (QString ref,
											QString infos,
											EditableItemManager const* manager) :
	_ref(ref),
	_infos(infos),
	_manager(manager)
{

	QString what = "[Error while loading \"" + _ref + "\" from \"" + manager->objectName() + "\"]: " + _infos;
	_what = what.toStdString();

}

ItemLoadingException::ItemLoadingException (ItemLoadingException const& other) :
	_ref(other.ref()),
	_infos(other.infos()),
	_manager(other.manager())
{

}

const char* ItemLoadingException::what() const throw() {

	return _what.c_str();

}


QString ItemLoadingException::ref() const
{
	return _ref;
}

QString ItemLoadingException::infos() const
{
	return _infos;
}

const EditableItemManager *ItemLoadingException::manager() const
{
	return _manager;
}

void ItemLoadingException::raise() const {
	throw *this;
}

ItemLoadingException * ItemLoadingException::clone() const {
	return new ItemLoadingException(*this);
}

} // namespace Cathia
