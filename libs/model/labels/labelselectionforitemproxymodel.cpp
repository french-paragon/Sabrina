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

#include "labelselectionforitemproxymodel.h"

#include "labelstree.h"
#include "editableitemmanager.h"
#include "editableitem.h"
#include "label.h"

namespace Sabrina {

LabelSelectionForItemProxyModel::LabelSelectionForItemProxyModel(LabelsTree *parent) :
	QIdentityProxyModel(parent),
	_treeSource(parent)
{
	setSourceModel(parent);
}
LabelSelectionForItemProxyModel::LabelSelectionForItemProxyModel(QObject *parent) :
	QIdentityProxyModel(parent),
	_treeSource(nullptr)
{

}

Qt::ItemFlags LabelSelectionForItemProxyModel::flags(const QModelIndex &index) const {

	if (_treeSource != nullptr) {
		return Qt::ItemIsUserCheckable | QIdentityProxyModel::flags(index);
	}

	return QIdentityProxyModel::flags(index);
}

QVariant LabelSelectionForItemProxyModel::data(const QModelIndex &index, int role) const {

	if (role == Qt::CheckStateRole) {

		EditableItem* item = _treeSource->parentManager()->activeItem();

		if (item != nullptr) {
			QVariant labels = item->property(Label::PROP_LABELS);

			if (labels.isValid() && labels.canConvert(qMetaTypeId<QStringList>())) {

				QStringList labelslist = labels.toStringList();

				return (labelslist.contains(QIdentityProxyModel::data(index, LabelsTree::LabelRefRole).toString())) ? Qt::Checked : Qt::Unchecked;
			} else {
				return Qt::Unchecked;
			}
		} else {
			return Qt::Unchecked;
		}

	}

	return QIdentityProxyModel::data(index, role);

}

bool LabelSelectionForItemProxyModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (role == Qt::CheckStateRole) {

		if (!value.canConvert(qMetaTypeId<uint>())) {
			return false;
		}

		EditableItem* item = _treeSource->parentManager()->activeItem();

		if (item != nullptr) {

			QString ref = item->getRef();

			if (value.toUInt() == Qt::Checked) {
				return QIdentityProxyModel::setData(index, ref, LabelsTree::MarkLabelForItem);
			} else {
				return QIdentityProxyModel::setData(index, ref, LabelsTree::UnmarkLabelForItem);
			}

		} else {
			return false;
		}

	}

	return QIdentityProxyModel::setData(index, value, role);

}

void LabelSelectionForItemProxyModel::setSourceModel(QAbstractItemModel *newSourceModel) {

	LabelsTree* tree = qobject_cast<LabelsTree*> (newSourceModel);

	if (tree) {
		setSourceModel(tree);
	}

}

void LabelSelectionForItemProxyModel::setSourceModel(LabelsTree* newSourceModel) {

	_treeSource = newSourceModel;
	QIdentityProxyModel::setSourceModel(newSourceModel);

	if (_newActiveItemConnection) {
		disconnect(_newActiveItemConnection);
	}

	if (_treeSource != nullptr) {
		_newActiveItemConnection = connect(_treeSource->parentManager(), &EditableItemManager::activeItemChanged,
											this, &LabelSelectionForItemProxyModel::onActiveItemChanged);
	}

}

void LabelSelectionForItemProxyModel::onActiveItemChanged() {

	emit dataChanged(QModelIndex(), QModelIndex(), {Qt::CheckStateRole});
}

} // namespace Sabrina
