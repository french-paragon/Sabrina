#include "folder.h"

namespace Sabrina {

Folder::Folder(EditableItemManager *parent) : EditableItem(parent)
{

}

QString Folder::getTypeId() const {

	return QString("sabrina_general_folder");

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

} // namespace Sabrina
