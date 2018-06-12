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

#include "projectfilterbylabelproxymodel.h"

#include "editableitemmanager.h"

#include "labels/labelstree.h"

namespace Sabrina {

ProjectFilterByLabelProxyModel::ProjectFilterByLabelProxyModel(QObject *parent) :
	QAbstractProxyModel(parent)
{

}

QModelIndex ProjectFilterByLabelProxyModel::index(int row, int column, const QModelIndex &parent) const {

	if (parent != QModelIndex()) {
		return QModelIndex();
	}

	if (row < 0 || row >= _selectedIndices.size() || column != 0) {
		return QModelIndex();
	}

	return createIndex(row, column);

}

QModelIndex ProjectFilterByLabelProxyModel::parent(QModelIndex const& index) const {

	Q_UNUSED(index);
	return QModelIndex();

}

int ProjectFilterByLabelProxyModel::rowCount(const QModelIndex &parent) const {

	if (parent == QModelIndex()) {
		return _selectedIndices.size();
	}

	return 0;

}

int ProjectFilterByLabelProxyModel::columnCount(const QModelIndex &parent) const {

	if (sourceModel() == nullptr) {
		return 0;
	}

	return sourceModel()->columnCount(mapToSource(parent));

}

QModelIndex ProjectFilterByLabelProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {

	int index = _selectedIndices.indexOf(sourceIndex);

	if (index >= 0) {
		return createIndex(index, 0);
	}

	return QModelIndex();
}

QModelIndex ProjectFilterByLabelProxyModel::mapToSource(const QModelIndex &proxyIndex) const {

	if (proxyIndex.isValid()) {

		return _selectedIndices.at(proxyIndex.row());

	}

	return QModelIndex();

}


void ProjectFilterByLabelProxyModel::setRefsInLabel(QStringList const& refs) {

	_refInLabel = refs;

	resetFilter();

}
void ProjectFilterByLabelProxyModel::addRefsInLabel(QString const& newRef) {

	_refInLabel.push_back(newRef);

	resetFilter();

}

void ProjectFilterByLabelProxyModel::removeRefsInLabel(QString const& oldRef) {

	_refInLabel.removeOne(oldRef);

	resetFilter();

}

void ProjectFilterByLabelProxyModel::setSourceModel(QAbstractItemModel *sourceModel) {

	QAbstractProxyModel::setSourceModel(sourceModel);

	if (_connectionProject) {
		disconnect(_connectionProject);
	}

	if (sourceModel != nullptr) {
		_connectionProject = connect(sourceModel, &QAbstractItemModel::dataChanged, this, &ProjectFilterByLabelProxyModel::resetFilter);
	}

	resetFilter();
}

void ProjectFilterByLabelProxyModel::connectLabelTree(LabelsTree * labelTree) {

	if (_connectionChange) {
		disconnect(_connectionChange);
	}

	if (_connectionAdd) {
		disconnect(_connectionAdd);
	}

	if (_connectionRemove) {
		disconnect(_connectionRemove);
	}

	if (labelTree == nullptr) {

		beginResetModel();
		_selectedIndices.clear();
		_refInLabel.clear();
		endResetModel();

		return;
	}

	_connectionChange = connect(labelTree, &LabelsTree::activeLabelFilterChanged,
								this, &ProjectFilterByLabelProxyModel::setRefsInLabel);

	_connectionChange = connect(labelTree, &LabelsTree::activeLabelFilterExtend,
								this, &ProjectFilterByLabelProxyModel::addRefsInLabel);

	_connectionChange = connect(labelTree, &LabelsTree::activeLabelFilterReduce,
								this, &ProjectFilterByLabelProxyModel::removeRefsInLabel);

	setRefsInLabel(labelTree->activeLabelFilter());
}

void ProjectFilterByLabelProxyModel::resetFilter() {

	beginResetModel();

	_selectedIndices.clear();

	int nRow = (sourceModel() != nullptr) ? sourceModel()->rowCount() : 0;

	for(int i = 0; i < nRow; i++) {
		exploreSourceModelIndex(sourceModel()->index(i, 0));
	}

	endResetModel();

}


void ProjectFilterByLabelProxyModel::exploreSourceModelIndex(QModelIndex const& sourceIndex) {

	QVariant data = sourceIndex.data(EditableItemManager::ItemRefRole);

	if (data.isValid() && data.canConvert(qMetaTypeId<QString>())) {
		QString ref = data.toString();

		if (_refInLabel.contains(ref)) {
			_selectedIndices.push_back(sourceIndex);
		}
	}

	int nRow = sourceModel()->rowCount(sourceIndex);

	for(int i = 0; i < nRow; i++) {
		exploreSourceModelIndex(sourceModel()->index(i, 0, sourceIndex));
	}
}

} //namespace Sabrina
