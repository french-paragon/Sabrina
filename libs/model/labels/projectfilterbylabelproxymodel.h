#ifndef PROJECTFILTERBYLABELPROXYMODEL_H
#define PROJECTFILTERBYLABELPROXYMODEL_H

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

#include <QAbstractProxyModel>

#include "model/model_global.h"

namespace Sabrina {

class LabelsTree;

class CATHIA_MODEL_EXPORT ProjectFilterByLabelProxyModel : public QAbstractProxyModel
{
	Q_OBJECT
public:
	explicit ProjectFilterByLabelProxyModel(QObject *parent = nullptr);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(QModelIndex const& index) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

	virtual void setSourceModel(QAbstractItemModel *sourceModel);

	void connectLabelTree(LabelsTree * labelTree);

signals:

public slots:

protected:

	virtual void setRefsInLabel(QStringList const& refs);
	virtual void addRefsInLabel(QString const& newRef);
	virtual void removeRefsInLabel(QString const& oldRef);

	void resetFilter();
	void exploreSourceModelIndex(QModelIndex const& sourceIndex);

	QStringList _refInLabel;
	QModelIndexList _selectedIndices;

	QMetaObject::Connection _connectionProject;

	QMetaObject::Connection _connectionChange;
	QMetaObject::Connection _connectionAdd;
	QMetaObject::Connection _connectionRemove;

};

} //namespace Sabrina

#endif // PROJECTFILTERBYLABELPROXYMODEL_H
