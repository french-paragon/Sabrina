#ifndef JSONEDITABLEITEMMANAGER_H
#define JSONEDITABLEITEMMANAGER_H

#include "model_global.h"

#include "editableitemmanager.h"

namespace Cathia {

class CATHIA_MODEL_EXPORT JsonEditableItemManager : public EditableItemManager
{
public:
	JsonEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

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
