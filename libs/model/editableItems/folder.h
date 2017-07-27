#ifndef FOLDER_H
#define FOLDER_H

#include "editableitem.h"
#include "editableitemfactory.h"

namespace Sabrina {

class Folder : public EditableItem
{
public:
	Folder(EditableItemManager* parent = nullptr);

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual bool acceptChildrens() const;

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);
};

} // namespace Sabrina

#endif // FOLDER_H
