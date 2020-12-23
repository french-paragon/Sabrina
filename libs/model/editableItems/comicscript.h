#ifndef SABRINA_COMICSCRIPT_H
#define SABRINA_COMICSCRIPT_H

/*
This file is part of the project Sabrina
Copyright (C) 2020  Paragon <french.paragon@gmail.com>

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

#include "model/editableitem.h"
#include "aline/src/model/editableitemfactory.h"

namespace Sabrina {

class Comicscript;

class CATHIA_MODEL_EXPORT ComicscriptModel : public QAbstractItemModel
{
	Q_OBJECT
public:

	enum DataRoles : int {
		TextRole = Qt::UserRole,
		DescrRole = Qt::UserRole + 1,
		TypeRole = Qt::UserRole + 2,
		TypeDescrRole = Qt::UserRole + 3,
		NbLinesRole = Qt::UserRole + 4
	};

	class CATHIA_MODEL_EXPORT ComicstripBlock {

	public:

		friend class ComicscriptModel;

		enum BlockType : int{
			OTHER = -1,
			PAGE = 0,
			PANEL = 1,
			CAPTION = 2,
			DIALOG = 3
		};

		explicit ComicstripBlock(ComicscriptModel* model, ComicstripBlock* parent = nullptr);
		virtual ~ComicstripBlock();

		virtual BlockType type() const = 0;

		QString typeDescr() const;

		QString getText() const;
		void setText(QString const& str);

		ComicstripBlock* getParent() const;
		void setParent(ComicstripBlock* parent);
		int getChildrenId(const ComicstripBlock* b) const;

		virtual QString getDescr() const;

	protected:
		ComicstripBlock* _parent;
		ComicscriptModel* _model;
		QList<ComicstripBlock*> _children;

		QString _text;
	};

	class CATHIA_MODEL_EXPORT PageBlock : public ComicstripBlock {

	public:

		explicit PageBlock(ComicscriptModel* model);
		ComicstripBlock::BlockType type() const override;

		QString getDescr() const override;
	};

	class CATHIA_MODEL_EXPORT PanelBlock : public ComicstripBlock {

	public:

		explicit PanelBlock(ComicscriptModel* model, PageBlock* parent);
		ComicstripBlock::BlockType type() const override;

		QString getDescr() const override;
	};

	class CATHIA_MODEL_EXPORT CaptionBlock : public ComicstripBlock {

	public:
		explicit CaptionBlock(ComicscriptModel* model, PanelBlock* parent);
		ComicstripBlock::BlockType type() const override;
	};

	class CATHIA_MODEL_EXPORT DialogBlock : public ComicstripBlock {

	public:
		explicit DialogBlock(ComicscriptModel* model, PanelBlock* parent);
		ComicstripBlock::BlockType type() const override;

		void setCharacter(QString const& name);
		QString getDescr() const override;

	protected:
		QString _characterName;
		bool _isRef;
	};

	explicit ComicscriptModel(Comicscript* script);
	~ComicscriptModel();

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	QModelIndex getNextItem(QModelIndex const& index);
	QModelIndex getPrevItem(QModelIndex const& index);

	void addPage(int row);
	void addPage(int row, int subrow);
	void removePage(int row);

	void addPanel(QModelIndex const& pageIndex, int row);
	void addPanel(QModelIndex const& pageIndex, int row, int subrow);
	void removePanel(QModelIndex const& pageIndex, int row);

	void addCaption(QModelIndex const& panelIndex, int row);
	void removeCaption(QModelIndex const& panelIndex, int row);

	void addDialog(QModelIndex const& panelIndex, int row);
	void removeDialog(QModelIndex const& panelIndex, int row);

protected:

	QList<PageBlock*> _pages;
};

class CATHIA_MODEL_EXPORT Comicscript : public EditableItem
{
	Q_OBJECT
public:

	static const QString COMICSTRIP_TYPE_ID;

	class CATHIA_MODEL_EXPORT ComicstripFactory : public Aline::EditableItemFactory
	{
	public:
		explicit ComicstripFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	explicit Comicscript(QString ref, Aline::EditableItemManager *parent);

	QString getTypeId() const override;
	QString getTypeName() const override;

	virtual QString iconInternalUrl() const;

	QString getTitle() const;
	void setTitle(const QString &title);

	ComicscriptModel* getModel();
signals:

	void titleChanged(QString title);

protected:

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);


	QString _title;

	ComicscriptModel* _model;

};

} // namespace Sabrina

#endif // SABRINA_COMICSCRIPT_H
