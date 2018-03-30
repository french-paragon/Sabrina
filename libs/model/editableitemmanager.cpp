#include "editableitemmanager.h"

#include "editableitem.h"
#include "editableitemfactory.h"

#include <QIcon>

namespace Sabrina {

const QChar EditableItemManager::RefSeparator = QChar('/');
const QString EditableItemManager::RefRoot = QString("root");

EditableItemManager::EditableItemManager(QObject *parent) :
	QAbstractItemModel(parent),
	_factoryManager(&EditableItemFactoryManager::GlobalEditableItemFactoryManager)
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

	if (parent == QModelIndex()) {
		return _root->_childrens.size();
	}

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

	case Qt::DecorationRole: //icon

		{
			QString iconPath = _factoryManager->itemIconUrl(data->_type_ref);

			if (iconPath != "") {
				return QIcon(iconPath);
			}
		}
		break;
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

bool EditableItemManager::containItem(const QString & ref) const {
	return _treeIndex.keys().contains(ref);
}

QVector<QString> EditableItemManager::listChildren(QString ref) {
	treeStruct* s = _treeIndex.value(ref);

	if (s != nullptr) {
		QVector<QString> r;
		r.reserve(s->_childrens.size());

		for (treeStruct* ss : s->_childrens) {
			r.push_back(ss->_ref);
		}

		return r;
	}

	return QVector<QString>();
}

bool EditableItemManager::createItem(QString typeRef, QString ref, QString parent_ref) {

	if (!_factoryManager->hasFactoryInstalled(typeRef)) {
		return false;
	}

	if (containItem(ref)) {
		return false;
	}

	if (parent_ref != nullptr) {
		if (!_treeIndex.contains(parent_ref)) {
			return false; //the parent need to be in the project.
		}
	}

	EditableItem* item = _factoryManager->createItem(typeRef, this);

	if (item != nullptr) {

		if (insertItem(item, _treeIndex.value(parent_ref, nullptr)) ) {
			return true;
		}

	}

	return false;

}

bool EditableItemManager::saveItem(QString ref) {
	if (isItemLoaded(ref)) {
		return effectivelySaveItem(ref);
	}
	return false;
}

void EditableItemManager::closeAll() {

	for (QString ref : _loadedItems.keys()) {

		emit itemAboutToBeUnloaded(ref); //at that point the items can still be saved or other operations can be carried on by the watchers.

		_loadedItems.remove(ref);

		emit itemUnloaded(ref);

	}

}

bool EditableItemManager::insertItem(EditableItem* item, treeStruct* parent_branch) {

	if (parent_branch == nullptr) {
		return false;
	}

	treeStruct item_leaf;

	item_leaf._parent = parent_branch;
	item_leaf._acceptChildrens = item->acceptChildrens();
	item_leaf._name = item->objectName();
	item_leaf._ref = item->getRef();
	item_leaf._type_ref = item->getTypeId();

	beginInsertRows(indexFromLeaf(parent_branch), parent_branch->_childrens.size(), parent_branch->_childrens.size());

	_tree.push_back(item_leaf);
	parent_branch->_childrens.append(&_tree.back());
	_treeIndex.insert(_tree.back()._ref, &_tree.back());

	_loadedItems.insert(_tree.back()._ref, {&_tree.back(), item});

	endInsertRows();

	return true;

}

QModelIndex EditableItemManager::indexFromLeaf(treeStruct* leaf) const {

	if (leaf == _root || leaf == nullptr) {
		return QModelIndex();
	}

	int row = leaf->_parent->_childrens.indexOf(leaf);

	if (row > 0) {
		return createIndex(row, 0, leaf);
	}

	return QModelIndex();
}

void EditableItemManager::cleanTreeStruct() {

	beginResetModel();

	_treeIndex.clear();
	_tree.clear();
	_tree.push_back({nullptr, RefRoot, RefRoot, RefRoot, {}, true});
	_root = &_tree.first();

	endResetModel();

}

EditableItemFactoryManager *EditableItemManager::factoryManager() const
{
	return _factoryManager;
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
