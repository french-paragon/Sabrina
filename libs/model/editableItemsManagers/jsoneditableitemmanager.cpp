#include "jsoneditableitemmanager.h"

#include "editableitem.h"
#include "editableitemfactory.h"

#include <QFile>
#include <QMetaObject>
#include <QMetaProperty>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace Cathia {

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

	EditableItem* item = _factoryManager->createItem(id, this);

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

bool JsonEditableItemManager::effectivelySaveItem(QString ref) {

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
