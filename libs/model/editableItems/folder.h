#ifndef FOLDER_H
#define FOLDER_H

#include "model/editableitem.h"
#include "aline/src/model/editableitemfactory.h"

#include "model/model_global.h"

namespace Sabrina {

class CATHIA_MODEL_EXPORT Folder : public EditableItem
{
public:

	static const QString FOLDER_TYPE_ID;

	class CATHIA_MODEL_EXPORT FolderFactory : public Aline::EditableItemFactory
	{
	public:
		explicit FolderFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	Folder(QString ref, Aline::EditableItemManager* parent = nullptr);

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual bool acceptChildrens() const;

	virtual QString iconInternalUrl() const;

	virtual bool autoSave() const;

protected:

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);
};

} // namespace Sabrina

#endif // FOLDER_H
