#include "folder.h"

namespace Sabrina {

const QString Folder::FOLDER_TYPE_ID = "sabrina_general_folder";

Folder::Folder(QString ref, EditableItemManager *parent) :
	EditableItem(ref, parent)
{
}

QString Folder::getTypeId() const {

	return FOLDER_TYPE_ID;

}

QString Folder::getTypeName() const {

	return tr("Folder");

}

bool Folder::acceptChildrens() const {
	return true; //folder accept all childrens.
}

QString Folder::iconInternalUrl() const {
	return ":/icons/icons/editable_item_folder.svg";
}

bool Folder::autoSave() const {
	return true;
}

void Folder::treatDeletedRef(QString deletedRef) {

	(void) deletedRef;
	//Do nothing, since a folder is nothing more than a children container.

}

void Folder::treatChangedRef(QString oldRef, QString newRef) {

	(void) oldRef;
	(void) newRef;
	//Do nothing, since a folder is nothing more than a children container.

}

Folder::FolderFactory::FolderFactory(QObject *parent) :
	EditableItemFactory(parent)
{

}

EditableItem* Folder::FolderFactory::createItem(QString ref, EditableItemManager* parent) const {
	return new Folder(ref, parent);
}

} // namespace Sabrina
