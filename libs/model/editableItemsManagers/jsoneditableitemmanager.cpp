#include "jsoneditableitemmanager.h"

#include "editableitem.h"
#include "editableitemfactory.h"

#include <QFile>
#include <QMetaObject>
#include <QMetaProperty>
#include <QFileInfo>
#include <QDir>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace Sabrina {

const QString JsonEditableItemManager::PROJECT_FILE_EXT = ".sabrinaproject";

const QString JsonEditableItemManager::TREE_REF_ID = "reference";
const QString JsonEditableItemManager::TREE_TYPE_ID = "type";
const QString JsonEditableItemManager::TREE_NAME_ID = "name";
const QString JsonEditableItemManager::TREE_CHILDRENS_ID = "childrens";
const QString JsonEditableItemManager::TREE_ACCEPT_CHILDRENS_ID = "accept_childrens";

const QString JsonEditableItemManager::ITEM_FOLDER_NAME = "items/";

JsonEditableItemManager::JsonEditableItemManager(QObject *parent) :
	EditableItemManager(parent)
{

}

bool JsonEditableItemManager::hasDataSource() const {
	return _hasAProjectOpen;
}

void JsonEditableItemManager::reset() {

	if (!hasDataSource()) {
		return;
	}

	closeAll();
	cleanTreeStruct();

	_hasAProjectOpen = false;
}

void JsonEditableItemManager::connectProject(QString projectFile) {

	QFileInfo info(projectFile);

	if (info.isFile() || projectFile.endsWith(PROJECT_FILE_EXT)) {

		_projectFolder = info.dir().absolutePath();
		_projectFileName = info.fileName();
	} else {
		_projectFolder = info.absoluteFilePath();
		_projectFileName = "project" + PROJECT_FILE_EXT;
	}

	_projectFolder = QDir::fromNativeSeparators(_projectFolder);

	if (!_projectFolder.endsWith('/')) {
		_projectFolder += '/';
	}

	if (!_projectFileName.endsWith(PROJECT_FILE_EXT)) {
		_projectFileName += PROJECT_FILE_EXT;
	}

	if (!info.exists()) {
		saveStruct();
	} else {
		loadStruct();
	}

	QDir itemDir(_projectFolder + ITEM_FOLDER_NAME);

	if (!itemDir.exists()) {
		itemDir.mkdir(itemDir.absolutePath());
	}

	_hasAProjectOpen = true;

}

bool JsonEditableItemManager::saveStruct() {

	QJsonObject obj;
	encapsulateTreeLeaf(_root, obj);

	QJsonDocument doc(obj);
	QByteArray datas = doc.toJson();

	QString fileName =  _projectFolder + _projectFileName;

	if (!_projectFileName.endsWith(PROJECT_FILE_EXT)) {
		fileName += PROJECT_FILE_EXT;
	}

	QFile out(fileName);

	if(!out.open(QIODevice::WriteOnly)){
		throw ItemIOException("root", QString("Cannot write to file %1.").arg(fileName), this);
	}

	qint64 w_stat = out.write(datas);
	out.close();

	if(w_stat < 0){
		throw ItemIOException("root", QString("Cannot write to file %1.").arg(fileName), this);
	}

	return true;
}

void JsonEditableItemManager::encapsulateTreeLeaf(treeStruct* branch, QJsonObject & obj) {

	obj.insert(TREE_REF_ID, branch->_ref);
	obj.insert(TREE_TYPE_ID, branch->_type_ref);
	obj.insert(TREE_NAME_ID, branch->_name);
	obj.insert(TREE_ACCEPT_CHILDRENS_ID, branch->_acceptChildrens);

	QJsonArray childrens;

	for (treeStruct* leaf : branch->_childrens) {
		QJsonObject obj;
		encapsulateTreeLeaf(leaf, obj);
		childrens.append(obj);
	}

	obj.insert(TREE_CHILDRENS_ID, childrens);

}

bool JsonEditableItemManager::loadStruct() {

	reset();

	QString fileName =  _projectFolder + _projectFileName;

	if (!_projectFileName.endsWith(PROJECT_FILE_EXT)) {
		fileName += PROJECT_FILE_EXT;
	}

	QFile file(fileName);

	if (!file.exists()) {
		throw ItemIOException("root", QString("File %1 do not exist.").arg(fileName), this);
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw ItemIOException("root", QString("File %1 is not readable.").arg(fileName), this);
	}

	QByteArray datas = file.readAll();
	file.close();

	QJsonParseError errors;
	QJsonDocument doc = QJsonDocument::fromJson(datas, &errors);

	if(errors.error != QJsonParseError::NoError){
		throw ItemIOException("root", QString("Error while parsing JSON data in file %1.").arg(fileName), this);
	}

	QJsonObject obj = doc.object();

	extractTreeLeaf(_root, obj);

}
void JsonEditableItemManager::extractTreeLeaf(treeStruct* leaf, QJsonObject &obj) {

	if (!obj.contains(TREE_REF_ID)) {

		if (leaf->_parent != nullptr) {
			throw ItemIOException(leaf->_parent->_ref, QString("Error while parsing JSON data in object %1, missing reference for children.").arg(leaf->_parent->_ref), this);
		}

	}

	leaf->_ref = obj.value(TREE_REF_ID).toString();

	if (!obj.contains(TREE_TYPE_ID)) {
		throw ItemIOException(leaf->_ref, QString("Error while parsing JSON data in object %1, missing type id.").arg(leaf->_ref), this);
	}

	leaf->_type_ref = obj.value(TREE_TYPE_ID).toString();
	leaf->_name = (obj.contains(TREE_NAME_ID)) ? obj.value(TREE_NAME_ID).toString() : leaf->_ref;
	leaf->_acceptChildrens = (obj.contains(TREE_ACCEPT_CHILDRENS_ID)) ? obj.value(TREE_ACCEPT_CHILDRENS_ID).toBool() : false;

	if (leaf->_acceptChildrens) {

		if (obj.contains(TREE_CHILDRENS_ID)) {

			QJsonValue val = obj.value(TREE_CHILDRENS_ID);

			if (val.isArray()) {

				QJsonArray array = val.toArray();

				for (QJsonValue v : array) {

					if (v.isObject()) {
						QJsonObject sub_obj = v.toObject();

						treeStruct* sub_leaf = new treeStruct();

						sub_leaf->_parent = leaf;

						extractTreeLeaf(sub_leaf, sub_obj);

						leaf->_childrens.append(sub_leaf);
						_treeIndex.insert(sub_leaf->_ref, sub_leaf);

					}

				}

			}

		}

	}

}

EditableItem* JsonEditableItemManager::effectivelyLoadItem(QString const& ref) {

	QString fileName = _projectFolder + ITEM_FOLDER_NAME + ref + ".json";

	QFile file(fileName);

	if (!file.exists()) {
		throw ItemIOException(ref, QString("File %1 do not exist.").arg(fileName), this);
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw ItemIOException(ref, QString("File %1 is not readable.").arg(fileName), this);
	}

	QByteArray datas = file.readAll();
	file.close();

	QJsonParseError errors;
	QJsonDocument doc = QJsonDocument::fromJson(datas, &errors);

	if(errors.error != QJsonParseError::NoError){
		throw ItemIOException(ref, QString("Error while parsing JSON data in file %1.").arg(fileName), this);
	}

	QJsonObject obj = doc.object();

	if (!obj.contains(EditableItem::TYPE_ID_NAME)) {
		throw ItemIOException(ref, QString("No field %2 found in file %1.").arg(fileName).arg(EditableItem::TYPE_ID_NAME), this);
	}

	QString id = obj.value(EditableItem::TYPE_ID_NAME).toString();

	if (!_factoryManager->hasFactoryInstalled(id)) {
		throw ItemIOException(ref, QString("Editable item type %1 found in file %2 is not registered.").arg(id).arg(fileName), this);
	}

	EditableItem* item = _factoryManager->createItem(id, ref, this);

	for (QString prop : obj.keys()) {

		if (prop == EditableItem::TYPE_ID_NAME) {
			continue;
		}

		if (prop == EditableItem::REF_IN_PROP_NAME) {
			continue;
		}

		if (prop == EditableItem::REF_FROM_PROP_NAME) {
			continue;
		}

		if (prop == EditableItem::CHILDREN_PROP_NAME) {
			continue;
		}

		if (prop == EditableItem::REF_PROP_NAME) {
			continue;
		}

		item->setProperty(prop.toStdString().c_str(), obj.value(prop).toVariant()); //set all the properties.
	}

	if (obj.contains(EditableItem::REF_IN_PROP_NAME)) {

		QJsonArray vals = obj.value(EditableItem::REF_IN_PROP_NAME).toArray();

		for (QJsonValue v : vals) {
			item->addInRef(v.toString());
		}
	}

	if (obj.contains(EditableItem::REF_FROM_PROP_NAME)) {

		QJsonArray vals = obj.value(EditableItem::REF_FROM_PROP_NAME).toArray();

		for (QJsonValue v : vals) {
			item->addOutRef(v.toString());
		}
	}


	return item;
}

bool JsonEditableItemManager::effectivelySaveItem(const QString &ref) {

	EditableItem* item = loadItem(ref);

	if (item == nullptr) {
		return false;
	}

	QJsonObject obj;

	const QMetaObject* mobj = item->metaObject();

	for (int i = 0; i < mobj->propertyCount(); i++) {

		const char* prop = mobj->property(i).name();
		QString sprop(prop);

		if (sprop == EditableItem::REF_PROP_NAME) {
			continue; //skip the ref, as it is stored in the file name.
		}

		obj.insert(sprop, QJsonValue::fromVariant(item->property(prop)) ); // insert the property.
	}

	QJsonArray referentItems;
	QJsonArray referedItems;

	const QSet<QString> & referentItemsSet = item->getReferentItemRefs();
	const QSet<QString> & referedItemsSet = item->getLinkedItemsRefs();

	for (QString s : referentItemsSet) {
		referentItems.push_back(QJsonValue(s));
	}

	for (QString s : referedItemsSet) {
		referedItems.push_back(QJsonValue(s));
	}

	obj.insert(EditableItem::REF_FROM_PROP_NAME, QJsonValue(referentItems));
	obj.insert(EditableItem::REF_IN_PROP_NAME, QJsonValue(referedItems));

	QJsonDocument doc(obj);
	QByteArray datas = doc.toJson();

	QString fileName =  _projectFolder + ITEM_FOLDER_NAME + ref + ".json";

	QFile out(fileName);

	if(!out.open(QIODevice::WriteOnly)){
		throw ItemIOException(ref, QString("Cannot write to file %1.").arg(fileName), this);
	}

	qint64 w_stat = out.write(datas);
	out.close();

	if(w_stat < 0){
		throw ItemIOException(ref, QString("Cannot write to file %1.").arg(fileName), this);
	}

	return true;

}

} // namespace Cathia
