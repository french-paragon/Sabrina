#ifndef JSONEDITABLEITEMMANAGER_H
#define JSONEDITABLEITEMMANAGER_H

#include "model/model_global.h"

#include "model/editableitemmanager.h"

namespace Aline {
	class EditableItem;
}


namespace Sabrina {

class Label;
class NotesList;

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

	static const QString ITEM_SUBITEM_ID;

	explicit JsonEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual void reset();

	void connectProject(QString projectFile);

	virtual bool saveStruct();
	virtual bool saveLabels();
	virtual bool loadStruct();

	virtual bool isNetworkShared() const;

protected:

	static const QString ITEM_FOLDER_NAME;
	static const QString LABELS_FILE_NAME;

	virtual Aline::EditableItem* effectivelyLoadItem(QString const& ref);

	virtual bool clearItemData(QString itemRef);

	virtual void effectivelyLoadLabels();
	virtual bool effectivelySaveItem(QString const& ref);

	void extractItemData(Aline::EditableItem* item, QJsonObject const& encapsulated);
	QJsonObject encapsulateItemToJson(Aline::EditableItem* item, int level = 0) const;

	void encapsulateTreeLeaf(treeStruct* leaf, QJsonObject &obj);
	void extractTreeLeaf(treeStruct* leaf, QJsonObject &obj);

	Label* extractJsonLabel(QJsonValue const& val, LabelsTree* parent);
	Label* extractJsonLabel(QJsonValue const& val, Label* parent);
	void extractJsonLabelDatas(QJsonValue const& val, Label* label);

	QJsonObject encodeLabelAsJson(QModelIndex const& index);

	bool extractNotesFromJson(NotesList* list, QJsonArray const& notesArray);
	QJsonArray encodeNotesArray(const NotesList *list) const;

	QJsonValue encodeVariantToJson(QVariant var) const;
	QVariant decodeVariantFromJson(QJsonValue val, QVariant::Type type) const;

	bool _hasAProjectOpen;
	QString _projectFileName;
	QString _projectFolder;
};

} // namespace Cathia

#endif // JSONEDITABLEITEMMANAGER_H
