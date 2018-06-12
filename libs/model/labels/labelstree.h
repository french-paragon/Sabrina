#ifndef LABELSTREE_H
#define LABELSTREE_H

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

#include <QAbstractItemModel>
#include <QSet>

namespace Sabrina {

class Label;
class EditableItemManager;

class CATHIA_MODEL_EXPORT LabelsTree : public QAbstractItemModel
{
	Q_OBJECT
public:

	enum specialRoles{
		LabelRefRole = Qt::UserRole,
		LabelItemsRefsRole = Qt::UserRole+1,
		MarkLabelForItem = Qt::UserRole+2,
		UnmarkLabelForItem = Qt::UserRole+3
	};

	static const QString LabelRefMimeType;

	explicit LabelsTree(EditableItemManager *parent = nullptr);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

	virtual Qt::DropActions supportedDropActions() const;

	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool insertRows(int row, QVector<Label*> const& labels, const QModelIndex &parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

	bool makeRefUniq(QString &ref) const;

	EditableItemManager *parentManager() const;

	void setActiveLabel(const QModelIndex &activeLabelIndex);

	QStringList activeLabelFilter() const;

signals:

	void activeLabelFilterChanged(QStringList labelFilter);
	void activeLabelFilterExtend(QString newRef);
	void activeLabelFilterReduce(QString oldRef);

public slots:

protected:

	void registerRef(QString ref);
	void exchangeRef(QString oldRef, QString newRef);
	void removeRefs(QStringList const& refs);
	void insertRefs(QStringList const& refs);

	int getRowFromLabel(Label* label);
	bool moveRefsToParent(QStringList refs, QModelIndex const& parent);
	bool moveRefToParent(QString ref, QModelIndex const& parent);

	Label* findLabelByRef(QString const& ref, Label* parentLabel = nullptr);

	QVector<Label*> _labels;

	QSet<QString> _labelsRefs;

	EditableItemManager* _parentManager;

	Label* _activeLabel;

};

} // namespace Sabrina

#endif // LABELSTREE_H
