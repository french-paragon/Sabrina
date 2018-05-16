#ifndef JSONEDITABLEITEMMANAGER_H
#define JSONEDITABLEITEMMANAGER_H

#include "model/model_global.h"

#include "model/editableitemmanager.h"

namespace Sabrina {

class Label;

class CATHIA_MODEL_EXPORT JsonEditableItemManager : public EditableItemManager
{
public:

	static const QString PROJECT_FILE_EXT;

	static const QString TREE_REF_ID;
	static const QString TREE_TYPE_ID;
	static const QString TREE_NAME_ID;
	static const QString TREE_CHILDRENS_ID;
	static const QString TREE_ACCEPT_CHILDRENS_ID;

	static const QString LABEL_REF;

	static const QString LABEL_REF_ID;
	static const QString LABEL_NAME_ID;
	static const QString LABEL_ITEMS_REFS_ID;
	static const QString LABEL_SUBLABELS_ID;

	explicit JsonEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual void reset();

	void connectProject(QString projectFile);

	virtual bool saveStruct();
	virtual bool saveLabels();
	virtual bool loadStruct();

protected:

	static const QString ITEM_FOLDER_NAME;
	static const QString LABELS_FILE_NAME;

	virtual EditableItem* effectivelyLoadItem(QString const& ref);
	virtual void effectivelyLoadLabels();
	virtual bool effectivelySaveItem(QString const& ref);

	void encapsulateTreeLeaf(treeStruct* leaf, QJsonObject &obj);
	void extractTreeLeaf(treeStruct* leaf, QJsonObject &obj);

	Label* extractJsonLabel(QJsonValue const& val, LabelsTree* parent);
	Label* extractJsonLabel(QJsonValue const& val, Label* parent);
	void extractJsonLabelDatas(QJsonValue const& val, Label* label);

	QJsonObject encodeLabelAsJson(QModelIndex const& index);

	bool _hasAProjectOpen;
	QString _projectFileName;
	QString _projectFolder;
};

} // namespace Cathia

#endif // JSONEDITABLEITEMMANAGER_H
