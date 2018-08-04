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

#include "jsoneditableitemmanager.h"

#include "editableitem.h"
#include "aline/src/model/editableitemfactory.h"
#include "aline/src/model/labels/labelstree.h"
#include "aline/src/model/labels/label.h"
#include "aline/src/utils/jsonutils.h"

#include "notes/noteslist.h"

#include <QFile>
#include <QMetaObject>
#include <QMetaProperty>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QModelIndex>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QPointF>
#include <QSizeF>

namespace Sabrina {

const QString JsonEditableItemManager::PROJECT_FILE_EXT = ".sabrinaproject";

const QString JsonEditableItemManager::TREE_REF_ID = "reference";
const QString JsonEditableItemManager::TREE_TYPE_ID = "type";
const QString JsonEditableItemManager::TREE_NAME_ID = "name";
const QString JsonEditableItemManager::TREE_CHILDRENS_ID = "childrens";
const QString JsonEditableItemManager::TREE_ACCEPT_CHILDRENS_ID = "accept_childrens";

const QString JsonEditableItemManager::LABEL_REF = "labels";

const QString JsonEditableItemManager::LABEL_REF_ID = "reference";
const QString JsonEditableItemManager::LABEL_NAME_ID = "name";
const QString JsonEditableItemManager::LABEL_ITEMS_REFS_ID = "marked_items";
const QString JsonEditableItemManager::LABEL_SUBLABELS_ID = "sublabels";

const QString JsonEditableItemManager::ITEM_FOLDER_NAME = "items/";
const QString JsonEditableItemManager::LABELS_FILE_NAME = "labels.json";

const QString JsonEditableItemManager::ITEM_SUBITEM_ID = "item_internalsubitems";

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
	encapsulateTreeLeafs(obj);

	if (noteList()->rowCount() > 0) {

		QJsonArray arr = encodeNotesArray(noteList());

		obj.insert(EditableItem::NOTES_PROP_NAME, arr);

	}

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

bool JsonEditableItemManager::saveLabels() {

	QJsonArray arr;

	int n_rootId = _labels->rowCount();

	for (int i = 0; i < n_rootId; i++) {

		QJsonObject encoded = encodeLabelAsJson(_labels->index(i, 0));

		arr.push_back(QJsonValue(encoded));

	}

	QJsonDocument doc(arr);

	QByteArray datas = doc.toJson();

	QString fileName =  _projectFolder + LABELS_FILE_NAME;

	QFile out(fileName);

	if(!out.open(QIODevice::WriteOnly)){
		throw ItemIOException(LABEL_REF, QString("Cannot write to file %1.").arg(fileName), this);
	}

	qint64 w_stat = out.write(datas);
	out.close();

	if(w_stat < 0){
		throw ItemIOException(LABEL_REF, QString("Cannot write to file %1.").arg(fileName), this);
	}

	return true;

}

QJsonObject JsonEditableItemManager::encodeLabelAsJson(QModelIndex const& index) {

	QJsonObject obj;

	QVariant name = _labels->data(index, Qt::DisplayRole);
	QVariant ref = _labels->data(index, Aline::LabelsTree::LabelRefRole);

	obj.insert(LABEL_NAME_ID, QJsonValue::fromVariant(name));
	obj.insert(LABEL_REF_ID, QJsonValue::fromVariant(ref));

	QVariant items_refs = _labels->data(index, Aline::LabelsTree::LabelItemsRefsRole);

	if (items_refs != QVariant()) {
		obj.insert(LABEL_ITEMS_REFS_ID, QJsonValue::fromVariant(items_refs));
	}

	if (_labels->rowCount(index) > 0) {

		QJsonArray subLabels;

		for (int i = 0; i < _labels->rowCount(index); i++) {
			QJsonObject subobj = encodeLabelAsJson(_labels->index(i, 0, index));
			subLabels.push_back(QJsonValue(subobj));
		}

		obj.insert(LABEL_SUBLABELS_ID, QJsonValue(subLabels));

	}

	return obj;

}

void JsonEditableItemManager::encapsulateTreeLeafs(QJsonObject & obj) {

	QJsonArray arrTypes;

	for (QString typeRef : _itemsByTypes.keys()) {

		QJsonObject collection;
		collection.insert(TREE_TYPE_ID, typeRef);

		QJsonArray items;

		for (treeStruct* branch : _itemsByTypes.value(typeRef)) {

			QJsonObject treeLeaf;

			treeLeaf.insert(TREE_REF_ID, branch->_ref);
			treeLeaf.insert(TREE_NAME_ID, branch->_name);
			treeLeaf.insert(TREE_ACCEPT_CHILDRENS_ID, branch->_acceptChildrens);

			items.push_back(treeLeaf);

		}

		collection.insert(TREE_CHILDRENS_ID, items);

		arrTypes.push_back(collection);

	}

	obj.insert(TREE_CHILDRENS_ID, arrTypes);

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

	extractTreeLeafs(obj);

	if (obj.contains(EditableItem::NOTES_PROP_NAME)) {

		QJsonArray vals = obj.value(EditableItem::NOTES_PROP_NAME).toArray();

		extractNotesFromJson(noteList(), vals);

	}

}

bool JsonEditableItemManager::isNetworkShared() const {
	return false;
}

void JsonEditableItemManager::extractTreeLeafs(QJsonObject &obj) {

	if (!obj.contains(TREE_CHILDRENS_ID)) {

		throw ItemIOException("", QString("Error while parsing JSON data, missing reference for childrens."), this);

	}

	QJsonValue val = obj.value(TREE_CHILDRENS_ID);

	if (!val.isArray()) {
		throw ItemIOException("", QString("Error while parsing JSON data, childrens reference point to a non array."), this);
	}

	beginResetModel();

	QJsonArray arr = val.toArray();

	for (QJsonValue v : arr) {

		if (v.isObject()) {

			QJsonObject o = v.toObject();

			if (o.contains(TREE_TYPE_ID) && o.contains(TREE_CHILDRENS_ID)) {

				QJsonValue typeV = o.value(TREE_TYPE_ID);

				if (!typeV.isString()) {
					continue;
				}

				QString typeId = typeV.toString();

				if (!_itemsByTypes.contains(typeId)) {
					_itemsByTypes.insert(typeId, QVector<treeStruct*>());
				}

				QJsonValue childrensV = o.value(TREE_CHILDRENS_ID);

				if (!childrensV.isArray()) {
					continue;
				}

				for (QJsonValue c : childrensV.toArray()) {

					if (c.isObject()) {
						QJsonObject leafO = c.toObject();

						if (leafO.contains(TREE_REF_ID) && leafO.contains(TREE_NAME_ID)) {

							QJsonValue refV = leafO.value(TREE_REF_ID);
							QJsonValue nameV = leafO.value(TREE_NAME_ID);

							if (refV.isString() && nameV.isString()) {

								treeStruct* leaf = new treeStruct();

								leaf->_ref = refV.toString();
								leaf->_name = nameV.toString();

								if (leafO.contains(TREE_ACCEPT_CHILDRENS_ID)) {
									QJsonValue ac = leafO.value(TREE_ACCEPT_CHILDRENS_ID);

									if (ac.isBool()) {
										leaf->_acceptChildrens = ac.toBool();
									} else {
										leaf->_acceptChildrens = false;
									}
								} else {
									leaf->_acceptChildrens = false;
								}

								leaf->_type_ref = typeId;

								_treeIndex.insert(leaf->_ref, leaf);
								_itemsByTypes[typeId].push_back(leaf);

							}
						}
					}

				}

			}

		}

	}

	endResetModel();

}

Aline::EditableItem* JsonEditableItemManager::effectivelyLoadItem(QString const& ref) {

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

	if (!obj.contains(Aline::EditableItem::TYPE_ID_NAME)) {
		throw ItemIOException(ref, QString("No field %2 found in file %1.").arg(fileName).arg(Aline::EditableItem::TYPE_ID_NAME), this);
	}

	QString id = obj.value(Aline::EditableItem::TYPE_ID_NAME).toString();

	if (!_factoryManager->hasFactoryInstalled(id)) {
		throw ItemIOException(ref, QString("Editable item type %1 found in file %2 is not registered.").arg(id).arg(fileName), this);
	}

	Aline::EditableItem* item = _factoryManager->createItem(id, ref, this);

	extractItemData(item, obj);
	return item;
}

void JsonEditableItemManager::extractItemData(Aline::EditableItem* item, QJsonObject const& obj) {

	Aline::JsonUtils::extractItemData(item, obj, _factoryManager, { EditableItem::NOTES_PROP_NAME}, true);

	item->blockSignals(true);

	if (obj.contains(EditableItem::NOTES_PROP_NAME)) {

		EditableItem* sab_item = qobject_cast<EditableItem*>(item);

		if (sab_item != nullptr) {
			QJsonArray vals = obj.value(EditableItem::NOTES_PROP_NAME).toArray();

			extractNotesFromJson(sab_item->getNoteList(), vals);
		}

	}

	item->blockSignals(false);

}

bool JsonEditableItemManager::clearItemData(QString itemRef) {

	QString fileName = _projectFolder + ITEM_FOLDER_NAME + itemRef + ".json";

	QFile file(fileName);

	if (file.exists()) {
		file.remove();
	}

}

void JsonEditableItemManager::effectivelyLoadLabels() {

	_labels = new Aline::LabelsTree(this);


	QString fileName = _projectFolder + LABELS_FILE_NAME;

	QFile file(fileName);

	if (!file.exists()) {
		return; //no labels saved now. nothing more to do.
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw ItemIOException(LABEL_REF, QString("File %1 is not readable.").arg(fileName), this);
	}

	QByteArray datas = file.readAll();
	file.close();

	QJsonParseError errors;
	QJsonDocument doc = QJsonDocument::fromJson(datas, &errors);

	if(errors.error != QJsonParseError::NoError){
		throw ItemIOException(LABEL_REF, QString("Error while parsing JSON data in file %1.").arg(fileName), this);
	}

	if (!doc.isArray()) {
		throw ItemIOException(LABEL_REF, QString("Expected JSON data in file %1 to represent an array of labels.").arg(fileName), this);
	}
	QJsonArray array = doc.array(); //should be a list of labels

	QVector<Aline::Label*> labels;
	labels.reserve(array.size());

	for (QJsonValue val : array) {

		Aline::Label* l;

		try {
			l = Aline::JsonUtils::extractJsonLabel(val, _labels);
		} catch (ItemIOException const& e) {
			Q_UNUSED(e);
			qDebug() << "Unexpected object while parsing data in " << fileName << "! Skip it for the moment."; //TODO: do we want some other kind of error ?
			continue;
		}

		labels.push_back(l);
	}

	_labels->insertRows(0, labels);
}

bool JsonEditableItemManager::effectivelySaveItem(const QString &ref) {

	Aline::EditableItem* item = loadItem(ref);

	if (item == nullptr) {
		return false;
	}

	QJsonObject obj = encapsulateItemToJson(item);

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


QJsonObject JsonEditableItemManager::encapsulateItemToJson(Aline::EditableItem* item) const {

	QJsonObject obj = Aline::JsonUtils::encapsulateItemToJson(item);

	EditableItem* sab_item = qobject_cast<EditableItem*>(item);

	if (sab_item != nullptr) {

		if (sab_item->getNoteList()->rowCount() > 0) {

			QJsonArray arr = encodeNotesArray(sab_item->getNoteList());

			obj.insert(EditableItem::NOTES_PROP_NAME, arr);

		}

	}

	return obj;
}


bool JsonEditableItemManager::extractNotesFromJson(NotesList* list, QJsonArray const& notesArray) {

	QHash<int, QByteArray> roleNames = list->roleNames();
	QHash<QByteArray, int> nameRoles;

	for (int key : roleNames.keys()) {
		nameRoles.insert(roleNames.value(key), key);
	}

	if (list->rowCount() != 0) {
		list->removeRows(0, list->rowCount());
	}

	list->insertRows(0, notesArray.size());

	int row = 0;
	for(QJsonValue val : notesArray) {

		QModelIndex index = list->index(row);

		if (val.isObject()) {

			QJsonObject obj = val.toObject();

			for (QString key : obj.keys()) {

				if (nameRoles.contains(key.toLatin1())) {
					int role = nameRoles.value(key.toLatin1());
					QVariant data = obj.value(key).toVariant();

					list->setData(index, data, role);
				}

			}

		}

		row++;

	}

}

QJsonArray JsonEditableItemManager::encodeNotesArray(NotesList const* list) const {

	QHash<int, QByteArray> roleNames = list->roleNames();

	QJsonArray ret;

	for (int i = 0; i < list->rowCount(); i++) {

		QModelIndex index = list->index(i);
		QJsonObject note;

		for (int role : NotesList::savableRoles()) {

			QString name(roleNames.value(role));

			note.insert(name, QJsonValue::fromVariant(index.data(role)));

		}

		ret.push_back(note);

	}

	return ret;

}


} // namespace Cathia
