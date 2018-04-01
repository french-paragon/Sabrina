#ifndef FOLDER_H
#define FOLDER_H

#include "model/editableitem.h"
#include "model/editableitemfactory.h"

namespace Sabrina {

class Folder : public EditableItem
{
public:

	static const QString FOLDER_TYPE_ID;

	class FolderFactory : public EditableItemFactory
	{
	public:
		explicit FolderFactory(QObject *parent = nullptr);
		EditableItem* createItem(QString ref, EditableItemManager* parent) const;

	};

	Folder(QString ref, EditableItemManager* parent = nullptr);

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual bool acceptChildrens() const;

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);
};

} // namespace Sabrina

#endif // FOLDER_H
