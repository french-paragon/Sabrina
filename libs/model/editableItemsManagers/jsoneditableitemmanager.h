#ifndef JSONEDITABLEITEMMANAGER_H
#define JSONEDITABLEITEMMANAGER_H

#include "model_global.h"

#include "editableitemmanager.h"

namespace Sabrina {

class CATHIA_MODEL_EXPORT JsonEditableItemManager : public EditableItemManager
{
public:
	explicit JsonEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual void reset();

protected:

	static const QString ITEM_FOLDER_NAME;

	virtual EditableItem* effectivelyLoadItem(QString const& ref);
	virtual bool effectivelySaveItem(QString ref);

	bool _hasAProjectOpen;
	QString _projectFileName;
	QString _projectFolder;
};

} // namespace Cathia

#endif // JSONEDITABLEITEMMANAGER_H
