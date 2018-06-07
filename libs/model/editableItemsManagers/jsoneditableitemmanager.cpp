#include "jsoneditableitemmanager.h"

#include "editableitem.h"
#include "aline/src/model/editableitemfactory.h"
#include "labels/labelstree.h"
#include "labels/label.h"

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
	encapsulateTreeLeaf(_root, obj);

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
	QVariant ref = _labels->data(index, LabelsTree::LabelRefRole);

	obj.insert(LABEL_NAME_ID, QJsonValue::fromVariant(name));
	obj.insert(LABEL_REF_ID, QJsonValue::fromVariant(ref));

	QVariant items_refs = _labels->data(index, LabelsTree::LabelItemsRefsRole);

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

	if (obj.contains(EditableItem::NOTES_PROP_NAME)) {

		QJsonArray vals = obj.value(EditableItem::NOTES_PROP_NAME).toArray();

		extractNotesFromJson(noteList(), vals);

	}

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

	item->blockSignals(true);

	for (QString prop : obj.keys()) {

		if (prop == Aline::EditableItem::TYPE_ID_NAME) {
			continue;
		}

		if (prop == EditableItem::REF_IN_PROP_NAME) {
			continue;
		}

		if (prop == EditableItem::REF_FROM_PROP_NAME) {
			continue;
		}

		if (prop == Aline::EditableItem::CHILDREN_PROP_NAME) {
			continue;
		}

		if (prop == Aline::EditableItem::REF_PROP_NAME) {
			continue;
		}

		if (prop == EditableItem::NOTES_PROP_NAME) {
			continue;
		}

		if (prop == ITEM_SUBITEM_ID) {
			continue;
		}

		const QMetaObject* meta = item->metaObject();
		int prop_index = meta->indexOfProperty(prop.toStdString().c_str());
		QVariant var = decodeVariantFromJson(obj.value(prop), meta->property(prop_index).type());

		item->setProperty(prop.toStdString().c_str(), var); //set all the properties.
	}

	if (obj.contains(EditableItem::REF_IN_PROP_NAME)) {

		EditableItem* sab_item = qobject_cast<EditableItem*>(item);

		if (sab_item != nullptr) {
			QJsonArray vals = obj.value(EditableItem::REF_IN_PROP_NAME).toArray();

			for (QJsonValue v : vals) {
				sab_item->addInRef(v.toString());
			}
		}
	}

	if (obj.contains(EditableItem::REF_FROM_PROP_NAME)) {

		EditableItem* sab_item = qobject_cast<EditableItem*>(item);

		if (sab_item != nullptr) {
			QJsonArray vals = obj.value(EditableItem::REF_FROM_PROP_NAME).toArray();

			for (QJsonValue v : vals) {
				sab_item->addOutRef(v.toString());
			}
		}
	}

	if (obj.contains(EditableItem::NOTES_PROP_NAME)) {

		EditableItem* sab_item = qobject_cast<EditableItem*>(item);

		if (sab_item != nullptr) {
			QJsonArray vals = obj.value(EditableItem::NOTES_PROP_NAME).toArray();

			extractNotesFromJson(sab_item->getNoteList(), vals);
		}

	}

	if (obj.contains(ITEM_SUBITEM_ID)) {

		QJsonValue subItems_val = obj.value(ITEM_SUBITEM_ID);

		if (subItems_val.isArray()) {
			QJsonArray arr = subItems_val.toArray();

			for (QJsonValue v : arr) {

				if (v.isObject()) {
					QJsonObject subObj = v.toObject();

					QString ref;
					QString type;

					if (subObj.contains(EditableItem::REF_PROP_NAME) &&
							subObj.contains(EditableItem::TYPE_ID_NAME)) {

						ref = subObj.value(EditableItem::REF_PROP_NAME).toString();
						type = subObj.value(EditableItem::TYPE_ID_NAME).toString();

						if (_factoryManager->hasSubItemFactoryInstalled(type)) {
							Aline::EditableItem* subItem = _factoryManager->createSubItem(type, ref, item);

							extractItemData(subItem, subObj);

							if (subItem != nullptr) {
								item->insertSubItem(subItem);
							}
						}

					}
				}

			}
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

	_labels = new LabelsTree(this);


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

	QVector<Label*> labels;
	labels.reserve(array.size());

	for (QJsonValue val : array) {

		Label* l;

		try {
			l = extractJsonLabel(val, _labels);
		} catch (ItemIOException const& e) {
			Q_UNUSED(e);
			qDebug() << "Unexpected object while parsing data in " << fileName << "! Skip it for the moment."; //TODO: do we want some other kind of error ?
			continue;
		}

		labels.push_back(l);
	}

	_labels->insertRows(0, labels);
}

Label* JsonEditableItemManager::extractJsonLabel(QJsonValue const& val, LabelsTree* parent) {

	Label* l = new Label(parent);

	try {
		extractJsonLabelDatas(val, l);
	} catch (ItemIOException const& e) {
		Q_UNUSED(e);
		delete l;
		return nullptr;
	}


	return l;

}

Label* JsonEditableItemManager::extractJsonLabel(QJsonValue const& val, Label* parent) {

	Label* l = new Label(parent);

	try {
		extractJsonLabelDatas(val, l);
	} catch (ItemIOException const& e) {
		Q_UNUSED(e);
		delete l;
		return nullptr;
	}

	return l;
}

void JsonEditableItemManager::extractJsonLabelDatas(QJsonValue const& val, Label* label) {

	if (!val.isObject()) {
		throw ItemIOException(LABEL_REF, "Error while extracting labels", this);
	}

	QJsonObject obj = val.toObject();

	if (!obj.contains(LABEL_REF_ID) || !obj.contains(LABEL_NAME_ID)) {
		throw ItemIOException(LABEL_REF, "Missing label name or reference.", this);
	}

	QJsonValue ref = obj.value(LABEL_REF_ID);
	QJsonValue name = obj.value(LABEL_NAME_ID);


	if (!ref.isString() || !name.isString()) {
		throw ItemIOException(LABEL_REF, "Label name or reference can't be converted to string.", this);
	}

	label->setRef(ref.toString());
	label->setObjectName(name.toString());

	if (obj.contains(LABEL_ITEMS_REFS_ID)) {
		QJsonValue items = obj.value(LABEL_ITEMS_REFS_ID);

		if (items.isArray()) {
			for (QJsonValue value : items.toArray()) {

				if (value.isString()) {
					label->markItem(value.toString());
				}

			}
		}
	}

	if (obj.contains(LABEL_SUBLABELS_ID)) {
		QJsonValue sublabels = obj.value(LABEL_SUBLABELS_ID);

		if (sublabels.isArray()) {

			for (QJsonValue val : sublabels.toArray()) {
				Label* sublabel = extractJsonLabel(val, label);

				if (sublabel != nullptr) {
					label->addChild(sublabel);
				}
			}

		}
	}


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


QJsonObject JsonEditableItemManager::encapsulateItemToJson(Aline::EditableItem* item, int level) const {

	EditableItem* sab_item = qobject_cast<EditableItem*>(item);

	QJsonObject obj;

	const QMetaObject* mobj = item->metaObject();

	for (int i = 0; i < mobj->propertyCount(); i++) {

		if (!mobj->property(i).isStored(item)) {
			continue;
		}

		const char* prop = mobj->property(i).name();
		QString sprop(prop);

		if (level == 0) {
			if (sprop == Aline::EditableItem::REF_PROP_NAME) {
				continue; //skip the ref, as it is stored in the file name.
			}
		}

		obj.insert(sprop, encodeVariantToJson(item->property(prop)) ); // insert the property.
	}

	QList<QByteArray> dynamicProperties = item->dynamicPropertyNames();

	for (QByteArray cpropName : dynamicProperties) {

		QString sprop(cpropName);

		obj.insert(sprop, encodeVariantToJson(item->property(cpropName.constData())));

	}

	if (sab_item != nullptr) {

		if (sab_item->getNoteList()->rowCount() > 0) {

			QJsonArray arr = encodeNotesArray(sab_item->getNoteList());

			obj.insert(EditableItem::NOTES_PROP_NAME, arr);

		}

		QJsonArray referentItems;
		QJsonArray referedItems;

		const QSet<QString> & referentItemsSet = sab_item->getReferentItemRefs();
		const QSet<QString> & referedItemsSet = sab_item->getLinkedItemsRefs();

		for (QString s : referentItemsSet) {
			referentItems.push_back(QJsonValue(s));
		}

		for (QString s : referedItemsSet) {
			referedItems.push_back(QJsonValue(s));
		}

		obj.insert(EditableItem::REF_FROM_PROP_NAME, QJsonValue(referentItems));
		obj.insert(EditableItem::REF_IN_PROP_NAME, QJsonValue(referedItems));

	}

	QJsonArray internal_items_array;

	for (Aline::EditableItem* internal_item : item->getSubItems()) {
		QJsonObject sub_obj = encapsulateItemToJson(internal_item, level+1);

		internal_items_array.append(sub_obj);

	}

	if (internal_items_array.size() > 0) {
		obj.insert(ITEM_SUBITEM_ID, internal_items_array);
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

QJsonValue JsonEditableItemManager::encodeVariantToJson(QVariant var) const {

	if (var.type() == QVariant::PointF) {
		QPointF data = var.toPointF();

		QString rep(QString("%1,%2").arg(data.x()).arg(data.y()));

		return QJsonValue(rep);
	} else if (var.type() == QVariant::Color) {

		QColor col = var.value<QColor>();

		return QJsonValue(col.name(QColor::HexArgb));
	}

	return QJsonValue::fromVariant(var);
}

QVariant JsonEditableItemManager::decodeVariantFromJson(QJsonValue val, QVariant::Type type) const {

	if (type == QVariant::PointF) {

		QString rep = val.toString();

		QStringList reps = rep.split(",");

		if (reps.size() != 2) {
			return QVariant();
		}

		QPointF pt;

		bool ok;
		qreal x = QVariant(reps[0]).toReal(&ok);

		if (!ok) {
			return QVariant();
		}

		qreal y = QVariant(reps[1]).toReal(&ok);

		if (!ok) {
			return QVariant();
		}

		pt.setX(x);
		pt.setY(y);

		return QVariant(pt);

	} else if (type == QVariant::Color) {

		QString colorName = val.toString();

		QColor col(colorName);

		return QVariant(col);
	}

	return val.toVariant();

}

} // namespace Cathia
