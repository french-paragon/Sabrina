#include "editableitemmanager.h"

#include "editableitem.h"
#include "editableitemfactory.h"

#include "labels/labelstree.h"
#include "labels/label.h"

#include "aline/src/editorfactorymanager.h"

#include <QIcon>
#include <QMimeData>

namespace Sabrina {

const QChar EditableItemManager::RefSeparator = QChar('/');
const QString EditableItemManager::RefRoot = QString("root");

const QString EditableItemManager::RefMimeType = "text/editableitemref";

EditableItemManager::EditableItemManager(QObject *parent) :
	QAbstractItemModel(parent),
	_factoryManager(&EditableItemFactoryManager::GlobalEditableItemFactoryManager),
	_labels(nullptr),
	_activeItem(nullptr)
{
	cleanTreeStruct();
}

QModelIndex EditableItemManager::index(int row, int column, const QModelIndex &parent) const {

	if (column > 0) {
		return QModelIndex();
	}

	if (parent == QModelIndex()) {

		if (row >= _root->_childrens.size()) {
			return QModelIndex();
		}

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

	case Qt::EditRole:

		return data->_name;

	case ItemRefRole:

		return data->_ref;

	case ItemAcceptChildrenRole:

		return data->_acceptChildrens;

	case ItemTypeRefRole:

		return data->_type_ref;

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

bool EditableItemManager::setData(const QModelIndex &index, const QVariant &value, int role) {

	switch (role) {

	case Qt::EditRole:

	{
		QString ref = data(index, ItemRefRole).toString();

		EditableItem* item = loadItem(ref);

		void * dataPtr = index.internalPointer();
		treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

		if (item != nullptr) {
			item->setObjectName(value.toString());
			data->_name = value.toString();
			emit dataChanged(index, index, {Qt::DisplayRole});
			return true;
		}
	}

	default:
		return false;
	}

	return false;

}


Qt::ItemFlags EditableItemManager::flags(const QModelIndex &index) const {

	if (index.isValid()) {

		Qt::ItemFlags f = Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

		QString itemTypeRef = index.data(ItemTypeRefRole).toString();

		if (!Aline::EditorFactoryManager::GlobalEditorFactoryManager.hasFactoryInstalledForItem(itemTypeRef)) {
			f |= Qt::ItemIsEditable;
		}

		return f;
	}

	return QAbstractItemModel::flags(index);

}

QVariant EditableItemManager::headerData(int section, Qt::Orientation orientation, int role) const {

	Q_UNUSED(section);
	Q_UNUSED(orientation);

	if (role == Qt::DisplayRole) {
		return "";
	}

	return QVariant();

}

QStringList EditableItemManager::mimeTypes() const {

	QStringList mimes;
	mimes << RefMimeType;

	return mimes;

}

QMimeData* EditableItemManager::mimeData(const QModelIndexList &indexes) const {

	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	for (QModelIndex index : indexes) {
		if (index.isValid()) {
			QString text = data(index, ItemRefRole).toString();
			stream << text;
		}
	}

	mimeData->setData(RefMimeType, encodedData);
	return mimeData;
}

bool EditableItemManager::dropMimeData(const QMimeData *data,
										Qt::DropAction action,
										int row,
										int column,
										const QModelIndex &parent) {

	Q_UNUSED(row);
	Q_UNUSED(column);

	if (action == Qt::IgnoreAction) {
		return true;
	}

	if (action != Qt::LinkAction && action != Qt::CopyAction) {
		return false;
	}

	if (!data->hasFormat(RefMimeType)) { //only accept refs to editableitems.
		return false;
	}


	QByteArray encodedData = data->data(EditableItemManager::RefMimeType);
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QStringList newItems;

	while (!stream.atEnd()) {
		QString text;
		stream >> text;
		newItems << text;
	}

	return moveItemsToParent(newItems, parent);
}

Qt::DropActions EditableItemManager::supportedDropActions() const {
	return Qt::LinkAction | Qt::CopyAction;
}

EditableItem* EditableItemManager::activeItem() const {
	return _activeItem;
}

EditableItem* EditableItemManager::loadItem(QString const& ref) {

	if (isItemLoaded(ref)) {
		return _loadedItems.value(ref)._item;
	}

	EditableItem* item = effectivelyLoadItem(ref);

	connect(item, &EditableItem::visibleStateChanged, this, &EditableItemManager::itemVisibleStateChanged);

	treeStruct* node = _treeIndex.value(ref, nullptr);

	if (node == nullptr) {
		insertItem(item, _root);
	} else {
		_loadedItems.insert(item->getRef(), {node, item});
	}

	return item;

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

LabelsTree* EditableItemManager::labelsTree() {

	if (!hasDataSource()) {
		return nullptr;
	}

	if (_labels == nullptr) {
		effectivelyLoadLabels();
	}

	return _labels;

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

bool EditableItemManager::createItem(QString typeRef, QString pref, QString parent_ref) {

	QString ref = pref;

	if (!_factoryManager->hasFactoryInstalled(typeRef)) {
		return false;
	}

	if (containItem(ref)) {
		makeRefUniq(ref);
	}

	if (parent_ref != "") {
		if (!_treeIndex.contains(parent_ref)) {
			return false; //the parent need to be in the project.
		}
	}

	EditableItem* item = _factoryManager->createItem(typeRef, ref, this);

	if (item != nullptr) {

		if (parent_ref == "") {
			if (insertItem(item, _root) ) {
				connect(item, &EditableItem::visibleStateChanged, this, &EditableItemManager::itemVisibleStateChanged);
				return true;
			} else {
				return false;
			}
		}

		if (insertItem(item, _treeIndex.value(parent_ref, nullptr)) ) {
			connect(item, &EditableItem::visibleStateChanged, this, &EditableItemManager::itemVisibleStateChanged);
			return true;
		}

	}

	return false;

}


bool EditableItemManager::clearItem(QString itemRef) {

	EditableItem* item = loadItem(itemRef);

	if (item != nullptr) {
		item->suppress();
		item->deleteLater();
	}

	treeStruct* node = _treeIndex.value(itemRef, nullptr);

	QModelIndex index = indexFromLeaf(node);

	if (node != nullptr) {
		beginRemoveRows(index.parent(), index.row(), index.row());

		treeStruct* parent = node->_parent;
		parent->_childrens.removeOne(node);

		endRemoveRows();
	}

	_loadedItems.remove(itemRef);

	clearItemData(itemRef);



}

bool EditableItemManager::clearItems(QStringList itemRefs) {

	for (QString ref : itemRefs) {
		clearItem(ref);
	}

}

bool EditableItemManager::saveItem(QString ref) {
	if (isItemLoaded(ref)) {
		bool status = effectivelySaveItem(ref);

		if (status) {
			EditableItem* item = loadItem(ref);
			item->clearHasUnsavedChanges();
		}
	}
	return false;
}

bool EditableItemManager::saveAll() {

	for (QString ref : _loadedItems.keys()) {
		saveItem(ref);
	}

	saveLabels();
	saveStruct();

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

	treeStruct* item_leaf = new treeStruct();

	item_leaf->_parent = parent_branch;
	item_leaf->_acceptChildrens = item->acceptChildrens();
	item_leaf->_name = item->objectName();
	item_leaf->_ref = item->getRef();
	item_leaf->_type_ref = item->getTypeId();

	beginInsertRows(indexFromLeaf(parent_branch), parent_branch->_childrens.size(), parent_branch->_childrens.size());

	parent_branch->_childrens.append(item_leaf);
	_treeIndex.insert(item_leaf->_ref, item_leaf);

	_loadedItems.insert(item_leaf->_ref, {item_leaf, item});

	endInsertRows();

	return true;

}

QModelIndex EditableItemManager::indexFromLeaf(treeStruct* leaf) const {

	if (leaf == _root || leaf == nullptr) {
		return QModelIndex();
	}

	int row = leaf->_parent->_childrens.indexOf(leaf);

	if (row >= 0) {
		return createIndex(row, 0, leaf);
	}

	return QModelIndex();
}

bool EditableItemManager::moveItemsToParent(QStringList items, QModelIndex const& index) {

	treeStruct* leaf = reinterpret_cast<treeStruct*>(index.internalPointer());

	if (!leaf->_acceptChildrens) {
		return false; //move items as childrens only if they are accepted.
	}

	if (index == QModelIndex()) {
		leaf = nullptr;
	}

	QSet<QString> refsHierarchy;

	treeStruct* next = leaf;

	while(next != nullptr) {
		refsHierarchy.insert(next->_ref);
		next = next->_parent;
	}

	for (QString ref : items) {
		if (refsHierarchy.contains(ref)) { //can't move item, or parent item on itself.
			return false;
		}
	}

	for (QString ref : items) {
		moveItemToParent(ref, leaf);
	}

	return true;

}

void EditableItemManager::moveItemToParent(QString item, treeStruct *leaf) {

	treeStruct* itemLeaf = _treeIndex.value(item, nullptr);

	if (itemLeaf != nullptr) {

		QModelIndex itemOldIndex = indexFromLeaf(itemLeaf);
		QModelIndex itemOldParent = itemOldIndex.parent();

		QModelIndex itemNewParent = indexFromLeaf(leaf);

		beginMoveRows(itemOldParent, itemOldIndex.row(), itemOldIndex.row(), itemNewParent, leaf->_childrens.size());

		itemLeaf->_parent->_childrens.removeOne(itemLeaf);

		leaf->_childrens.push_back(itemLeaf);
		itemLeaf->_parent = leaf;

		endMoveRows();
	}

}

void EditableItemManager::itemVisibleStateChanged(QString ref) {

	if (_treeIndex.contains(ref)) {

		treeStruct* leaf = _treeIndex.value(ref);

		EditableItem* item = loadItem(ref);

		leaf->_name = item->objectName() + ((item->getHasUnsavedChanged()) ? " *" : "");

		QModelIndex index = indexFromLeaf(leaf);

		emit dataChanged(index, index, {Qt::DisplayRole});

	}

}

void EditableItemManager::cleanTreeStruct() {

	beginResetModel();

	for (treeStruct* leaf : _treeIndex.values()) {
		delete leaf;
	}

	_treeIndex.clear();
	_root = new treeStruct();

	_root->_parent = nullptr;
	_root->_ref = RefRoot;
	_root->_name = RefRoot;
	_root->_childrens = {};
	_root->_acceptChildrens = true;

	endResetModel();

}

EditableItemFactoryManager *EditableItemManager::factoryManager() const
{
	return _factoryManager;
}

bool EditableItemManager::makeRefUniq(QString &ref) const {

	if (!containItem(ref)) {
		return true;
	}

	int i = 1;

	while(containItem(QString("%1_%2").arg(ref).arg(i))) {
		i++;

		if (i <= 0) {
			return false; //impossible to make the ref uniq.
		}
	}

	ref = QString("%1_%2").arg(ref).arg(i);

	return true;

}

void EditableItemManager::setActiveItem(QString ref) {

	if (_activeItem == nullptr || _activeItem->getRef() != ref) {

		EditableItem* potential = loadItem(ref);

		if (potential != nullptr) {
			_activeItem = potential;
			emit activeItemChanged();
		}

	}

}

void setActiveLabel(QString ref) {



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
