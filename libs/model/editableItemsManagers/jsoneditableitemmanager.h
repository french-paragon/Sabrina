#ifndef JSONEDITABLEITEMMANAGER_H
#define JSONEDITABLEITEMMANAGER_H

/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "model/model_global.h"

#include "model/editableitemmanager.h"

namespace Aline {
	class EditableItem;
	class Label;
}


namespace Sabrina {

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

	typedef std::function<void(Aline::EditableItem*, QJsonObject const& , bool)> Extractor;
	typedef std::function<QJsonObject(Aline::EditableItem*)> Encapsulator;

	explicit JsonEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual void reset();

	void connectProject(QString projectFile);

	virtual bool saveStruct();
	virtual bool saveLabels();
	virtual bool loadStruct();

	virtual bool isNetworkShared() const;

	void addExtractorDelegate(QString const& type, Extractor const& e);
	void addEncapsulatorDelegate(QString const& type, Encapsulator const& e);

protected:

	static const QString ITEM_FOLDER_NAME;
	static const QString LABELS_FILE_NAME;

	virtual Aline::EditableItem* effectivelyLoadItem(QString const& ref);

	virtual bool clearItemData(QString itemRef);

	virtual void effectivelyLoadLabels();
	virtual bool effectivelySaveItem(QString const& ref);

	void extractItemData(Aline::EditableItem* item, QJsonObject const& encapsulated);
	QJsonObject encapsulateItemToJson(Aline::EditableItem* item) const;

	void encapsulateTreeLeafs(QJsonObject &obj);
	void extractTreeLeafs(QJsonObject &obj);

	QJsonObject encodeLabelAsJson(QModelIndex const& index);

	bool extractNotesFromJson(NotesList* list, QJsonArray const& notesArray);
	QJsonArray encodeNotesArray(const NotesList *list) const;

	bool _hasAProjectOpen;
	QString _projectFileName;
	QString _projectFolder;

	QMap<QString,Extractor> _delegate_extractors;
	QMap<QString,Encapsulator> _delegate_encapsulators;
};

} // namespace Cathia

#endif // JSONEDITABLEITEMMANAGER_H
