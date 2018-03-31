#ifndef JSONEDITABLEITEMMANAGER_H
#define JSONEDITABLEITEMMANAGER_H

#include "model/model_global.h"

#include "model/editableitemmanager.h"

namespace Sabrina {

class CATHIA_MODEL_EXPORT JsonEditableItemManager : public EditableItemManager
{
public:

	static const QString PROJECT_FILE_EXT;

	static const QString TREE_REF_ID;
	static const QString TREE_TYPE_ID;
	static const QString TREE_NAME_ID;
	static const QString TREE_CHILDRENS_ID;
	static const QString TREE_ACCEPT_CHILDRENS_ID;

	explicit JsonEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual void reset();

	void connectProject(QString projectFile);

	virtual bool saveStruct();
	virtual bool loadStruct();

protected:

	static const QString ITEM_FOLDER_NAME;

	virtual EditableItem* effectivelyLoadItem(QString const& ref);
	virtual bool effectivelySaveItem(QString const& ref);

	void encapsulateTreeLeaf(treeStruct* leaf, QJsonObject &obj);
	void extractTreeLeaf(treeStruct* leaf, QJsonObject &obj);

	bool _hasAProjectOpen;
	QString _projectFileName;
	QString _projectFolder;
};

} // namespace Cathia

#endif // JSONEDITABLEITEMMANAGER_H
