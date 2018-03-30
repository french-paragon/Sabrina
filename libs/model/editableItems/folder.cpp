#include "folder.h"

namespace Sabrina {

const QString Folder::FOLDER_TYPE_ID = "sabrina_general_folder";

Folder::Folder(EditableItemManager *parent) : EditableItem(parent)
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

EditableItem* Folder::FolderFactory::createItem(EditableItemManager* parent) const {
	return new Folder(parent);
}

} // namespace Sabrina
