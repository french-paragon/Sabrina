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

#include "labelstree.h"

#include "label.h"
#include "editableitemmanager.h"

#include <QIcon>
#include <QMimeData>

namespace Sabrina {

const QString LabelsTree::LabelRefMimeType = "text/itemlabelref";

LabelsTree::LabelsTree(EditableItemManager *parent) :
	QAbstractItemModel(parent),
	_parentManager(parent),
	_activeLabel(nullptr)
{

}

QModelIndex LabelsTree::index(int row, int column, const QModelIndex &parent) const {

	if (parent == QModelIndex()) {
		if (column == 0 && row < _labels.size()) {
			return createIndex(row, column, _labels.at(row));
		}
	} else {
		Label* parentLabel = (Label*) parent.internalPointer();

		if (column == 0 && row < parentLabel->subLabels().size()) {
			return createIndex(row, column, parentLabel->subLabels().at(row));
		}
	}

	return QModelIndex();

}

QModelIndex LabelsTree::parent(const QModelIndex &index) const {

	if (index == QModelIndex()) {
		return QModelIndex();
	}

	Label* label = (Label*) index.internalPointer();

	Label* parentLabel = label->parentLabel();

	if (parentLabel == nullptr) {
		return QModelIndex();
	}

	Label* parentParent = parentLabel->parentLabel();

	int col = 0;
	int row = -1;

	if (parentParent == nullptr) {
		row = _labels.indexOf(parentLabel);
	} else {
		row = parentParent->children().indexOf(parentLabel);
	}

	if (row < 0) {
		return QModelIndex();
	}

	return createIndex(row, col, parentLabel);

}

int LabelsTree::rowCount(const QModelIndex &parent) const {

	if (parent == QModelIndex()) {
		return _labels.size();
	}

	Label* label = (Label*) parent.internalPointer();

	if (label != nullptr) {
		return label->children().size();
	}

	return 0;

}

int LabelsTree::columnCount(const QModelIndex &parent) const {

	Q_UNUSED(parent);

	return 1;
}

Qt::ItemFlags LabelsTree::flags(const QModelIndex &index) const {

	if (index != QModelIndex()) {
		return Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
	}

	return QAbstractItemModel::flags(index);

}

QVariant LabelsTree::headerData(int section, Qt::Orientation orientation, int role) const {

	Q_UNUSED(section);
	Q_UNUSED(orientation);

	if (role == Qt::DisplayRole) {
		return "";
	}

	return QVariant();

}

QVariant LabelsTree::data(const QModelIndex &index, int role) const {

	Label* label = (Label*) index.internalPointer();

	switch (role) {

	case Qt::DisplayRole :
	case Qt::EditRole :

		return label->objectName();

	case LabelRefRole :

		return label->getRef();

	case LabelItemsRefsRole :

		if (label->itemsRefs().empty()) {
			return QVariant();
		}

		return QVariant(QStringList::fromVector(label->itemsRefs()));

	case Qt::DecorationRole : //icon

		return QIcon(":/icons/icons/label_simple.svg");

	default:
		return QVariant();

	}

	return QVariant();

}

bool LabelsTree::setData(const QModelIndex &index, const QVariant &value, int role) {

	Label* label = (Label*) index.internalPointer();

	switch (role) {

	case Qt::EditRole :

		label->setObjectName(value.toString());

		emit dataChanged(index, index, {Qt::DisplayRole});

		return true;

	case MarkLabelForItem:
	{
		QString itemRef = value.toString();

		Aline::EditableItem* item = _parentManager->loadItem(itemRef);

		if (item != nullptr) {
			label->markItem(item);
		}

		emit dataChanged(index, index, {Qt::CheckStateRole});

		return true;

	}
		break;

	case UnmarkLabelForItem:
	{
		QString itemRef = value.toString();

		Aline::EditableItem* item = _parentManager->loadItem(itemRef);

		if (item != nullptr) {
			label->unmarkItem(item);
		}

		emit dataChanged(index, index, {Qt::CheckStateRole});

		return true;

	}
		break;

	default:
		return false;

	}

	return false;
}

QStringList LabelsTree::mimeTypes() const {

	QStringList mimes;
	mimes << EditableItemManager::RefMimeType;
	mimes << LabelRefMimeType;

	return mimes;

}

QMimeData* LabelsTree::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	for (QModelIndex index : indexes) {
		if (index.isValid()) {
			QString text = data(index, LabelRefRole).toString();
			stream << text;
		}
	}

	mimeData->setData(LabelRefMimeType, encodedData);
	return mimeData;
}

bool LabelsTree::dropMimeData(const QMimeData *data,
							  Qt::DropAction action,
							  int row,
							  int column,
							  const QModelIndex &parent) {

	if (action == Qt::IgnoreAction) {
		return true;
	}

	if (action != Qt::LinkAction && action != Qt::CopyAction) {
		return false;
	}

	if (!data->hasFormat(EditableItemManager::RefMimeType) && !data->hasFormat(LabelRefMimeType)) { //only accept refs to editableitems.
		return false;
	}

	if (data->hasFormat(EditableItemManager::RefMimeType)) {

		QModelIndex target = index(row, column, parent);

		if (row == -1) {
			target = parent;
		}

		if (!target.isValid()) {
			return false;
		}

		QByteArray encodedData = data->data(EditableItemManager::RefMimeType);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QStringList newItems;
		int rows = 0;

		while (!stream.atEnd()) {
			QString text;
			stream >> text;
			newItems << text;
			++rows;
		}

		Label* label = reinterpret_cast<Label*> (target.internalPointer());

		if (label == nullptr) {
			return false;
		}

		for (QString ref : newItems) {

			if (label->itemsRefs().contains(ref)) {
				continue;
			}

			Aline::EditableItem* item = _parentManager->loadItem(ref);

			if (item != nullptr) {
				label->markItem(item);
			}
		}

		return true;

	}

	if (data->hasFormat(LabelRefMimeType)) {

		QByteArray encodedData = data->data(LabelRefMimeType);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QStringList movedItems;
		int rows = 0;

		while (!stream.atEnd()) {
			QString text;
			stream >> text;
			movedItems << text;
			++rows;
		}

		return moveRefsToParent(movedItems, parent);
	}

	return false;

}

Qt::DropActions LabelsTree::supportedDropActions() const {
	return Qt::LinkAction | Qt::CopyAction;
}

bool LabelsTree::insertRows(int row, int count, const QModelIndex &parent) {

	beginInsertRows(parent, row, row+count-1);

	if (parent == QModelIndex()) {
		_labels.insert(row, count, nullptr);

		for (int i = row; i < row + count; i++) {
			_labels[i] = new Label(this);

			connect(_labels[i], &Label::refChanged, this, &LabelsTree::exchangeRef);
			connect(_labels[i], &Label::gettedRef, this, &LabelsTree::registerRef);
		}
	} else {

		Label* pLabel = (Label*) parent.internalPointer();

		pLabel->insertNewLabels(row, count);

		for (int i = row; i < row + count; i++) {
			connect(pLabel->subLabels()[i], &Label::refChanged, this, &LabelsTree::exchangeRef);
			connect(pLabel->subLabels()[i], &Label::gettedRef, this, &LabelsTree::registerRef);
		}

	}

	endInsertRows();

	return true;

}

bool LabelsTree::insertRows(int row, QVector<Label*> const& labels, const QModelIndex &parent) {

	QStringList insertedRefs;

	for (Label* l : labels) {
		QStringList refs = l->refAndSubRefs();

		for (QString ref : refs) {

			if (_labelsRefs.contains(ref)) {
				return false; //can't insert labels with redundant references.
			}
		}

		insertedRefs << refs;
	}

	int count = labels.size();

	beginInsertRows(parent, row, row+count-1);

	if (parent == QModelIndex()) {
		_labels.insert(row, count, nullptr);

		for (int i = row; i < row + count; i++) {
			_labels[i] = labels[i-row];

			connect(_labels[i], &Label::refChanged, this, &LabelsTree::exchangeRef);
			connect(_labels[i], &Label::gettedRef, this, &LabelsTree::registerRef);
		}
	} else {

		Label* pLabel = (Label*) parent.internalPointer();

		pLabel->insertSubLabels(row, labels);

		for (int i = row; i < row + count; i++) {
			connect(pLabel->subLabels()[i], &Label::refChanged, this, &LabelsTree::exchangeRef);
			connect(pLabel->subLabels()[i], &Label::gettedRef, this, &LabelsTree::registerRef);
		}

	}

	insertRefs(insertedRefs);

	endInsertRows();

	return true;
}

bool LabelsTree::removeRows(int row, int count, const QModelIndex &parent) {

	beginRemoveRows(parent, row, row+count-1);

	QStringList removedRefs;

	if (parent == QModelIndex()) {

		for (int i = row; i < row + count; i++) {

			removedRefs << _labels[i]->refAndSubRefs();

			disconnect(_labels[i], &Label::refChanged, this, &LabelsTree::exchangeRef);
			disconnect(_labels[i], &Label::gettedRef, this, &LabelsTree::registerRef);

			_labels[i]->deleteLater();
		}

		_labels.remove(row, count);

	} else {

		Label* pLabel = (Label*) parent.internalPointer();

		for (int i = row; i < row + count; i++) {

			removedRefs << pLabel->subLabels()[i]->refAndSubRefs();

			disconnect(pLabel->subLabels()[i], &Label::refChanged, this, &LabelsTree::exchangeRef);
			disconnect(pLabel->subLabels()[i], &Label::gettedRef, this, &LabelsTree::registerRef);
		}

		pLabel->removeLabels(row, count);

	}

	removeRefs(removedRefs);

	endRemoveRows();

	return true;

}

bool LabelsTree::makeRefUniq(QString & ref) const {

	if (!_labelsRefs.contains(ref)) {
		return true;
	}

	int i = 1;

	while(_labelsRefs.contains(QString("%1_%2").arg(ref).arg(i))) {
		i++;

		if (i <= 0) {
			return false; //impossible to make the ref uniq.
		}
	}

	ref = QString("%1_%2").arg(ref).arg(i);

	return true;

}

void LabelsTree::registerRef(QString ref) {
	_labelsRefs.insert(ref);
}

void LabelsTree::exchangeRef(QString oldRef, QString newRef) {
	_labelsRefs.remove(oldRef);
	_labelsRefs.insert(newRef);
}

void LabelsTree::removeRefs(const QStringList &refs) {

	for (QString const& ref : refs) {
		_labelsRefs.remove(ref);
	}

}

void LabelsTree::insertRefs(const QStringList &refs) {

	for (QString const& ref : refs) {
		_labelsRefs.insert(ref);
	}

}

int LabelsTree::getRowFromLabel(Label* label) {

	if (label->parentLabel() == nullptr) {
		return _labels.indexOf(label);
	}

	return label->parentLabel()->subLabels().indexOf(label);

}
bool LabelsTree::moveRefsToParent(QStringList refs, QModelIndex const& parent) {

	QSet<QString> parentHierarchy;

	QModelIndex next = parent;

	while (next.isValid()) {
		parentHierarchy.insert(next.data(LabelRefRole).toString());
		next = next.parent();
	}

	for(QString ref : refs) {
		if (parentHierarchy.contains(ref)) {
			return false; //can't move one of the label as the target is a direct descendant of the ref to move.
		}
	}

	for(QString ref : refs) {
		moveRefToParent(ref, parent);
	}

	return true;

}

bool LabelsTree::moveRefToParent(QString ref, QModelIndex const& parent) {

	Label* label = findLabelByRef(ref);
	Label* oldParentLabel = label->parentLabel();

	int sourceRow = (oldParentLabel != nullptr) ? oldParentLabel->subLabels().indexOf(label) : _labels.indexOf(label);

	if (sourceRow < 0) {
		return false;
	}

	if (parent.isValid()) {
		Label* newParentLabel = reinterpret_cast<Label*>(parent.internalPointer());
		if (oldParentLabel == newParentLabel ) {
			return true; //label's parent is already the correct one.
		}

		int oldParentRow = getRowFromLabel(oldParentLabel);

		if (oldParentRow < 0) {
			return false;
		}

		QModelIndex oldParentIndex = createIndex(oldParentRow, 0, oldParentLabel);

		if (newParentLabel != nullptr) {

			beginMoveRows(oldParentIndex, sourceRow, sourceRow, parent, rowCount(parent));

			oldParentLabel->removeChild(label);
			newParentLabel->addChild(label);

			endMoveRows();

			return true;

		}

		beginMoveRows(oldParentIndex, sourceRow, sourceRow, QModelIndex(), rowCount());

		oldParentLabel->removeChild(label);
		label->setParentLabel(nullptr);
		label->setParent(this);
		_labels.push_back(label);

		endMoveRows();

		return true;
	}

	if (parent == QModelIndex()) {
		return true; //label's parent is already the correct one.
	}

	beginMoveRows(QModelIndex(), sourceRow, sourceRow, parent, rowCount(parent));

	_labels.removeOne(label);

	Label* newParentLabel = reinterpret_cast<Label*>(parent.internalPointer());
	newParentLabel->addChild(label);

	endMoveRows();

	return true;
}

Label* LabelsTree::findLabelByRef(QString const& ref, Label *parentLabel) {

	QVector<Label*> const* searchSpace;

	if (parentLabel == nullptr) {
		searchSpace = &_labels;
	} else {
		searchSpace = &parentLabel->subLabels();
	}

	for (Label* l : *searchSpace) {
		if (l->getRef() == ref) {
			return l;
		}

		Label* located = findLabelByRef(ref, l);

		if (located != nullptr) {
			return located;
		}
	}

	return nullptr;

}

EditableItemManager *LabelsTree::parentManager() const
{
	return _parentManager;
}

void LabelsTree::setActiveLabel(QModelIndex const& activeLabelIndex) {

	Label* newLabel = nullptr;

	if (activeLabelIndex.model() == static_cast<QAbstractItemModel*>(this)) {

		void* data = activeLabelIndex.internalPointer();

		newLabel = (Label*) data;

	} else if (!activeLabelIndex.isValid()) {

		newLabel = nullptr;

	}

	if (_activeLabel != nullptr && newLabel != nullptr && newLabel != _activeLabel) {
		disconnect(_activeLabel, &Label::itemRefAdded, this, &LabelsTree::activeLabelFilterExtend);
		disconnect(_activeLabel, &Label::itemRefRemoved, this, &LabelsTree::activeLabelFilterReduce);
	}

	if (newLabel != nullptr && newLabel != _activeLabel) {
		connect(newLabel, &Label::itemRefAdded, this, &LabelsTree::activeLabelFilterExtend);
		connect(newLabel, &Label::itemRefRemoved, this, &LabelsTree::activeLabelFilterReduce);
		_activeLabel = newLabel;
		emit activeLabelFilterChanged(QStringList::fromVector(_activeLabel->itemsRefs()));
	}


}

QStringList LabelsTree::activeLabelFilter() const {

	if (_activeLabel == nullptr) {
		return QStringList();
	}

	return QStringList::fromVector(_activeLabel->itemsRefs());
}

} // namespace Sabrina
