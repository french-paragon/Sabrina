#ifndef LABELSELECTIONFORITEMPROXYMODEL_H
#define LABELSELECTIONFORITEMPROXYMODEL_H

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

#include <QIdentityProxyModel>

#include "model/model_global.h"

namespace Sabrina {

class LabelsTree;

class CATHIA_MODEL_EXPORT LabelSelectionForItemProxyModel : public QIdentityProxyModel
{
	Q_OBJECT
public:
	LabelSelectionForItemProxyModel(LabelsTree* parent);
	LabelSelectionForItemProxyModel(QObject* parent);

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	void setSourceModel(QAbstractItemModel *newSourceModel);
	void setSourceModel(LabelsTree* newSourceModel);

protected:

	void onActiveItemChanged();

	LabelsTree* _treeSource;

	QMetaObject::Connection _newActiveItemConnection;
};

} // namespace Sabrina

#endif // LABELSELECTIONFORITEMPROXYMODEL_H
