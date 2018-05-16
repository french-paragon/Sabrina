#include "labelstree.h"

#include "label.h"

#include <QIcon>

namespace Sabrina {

LabelsTree::LabelsTree(QObject *parent) : QAbstractItemModel(parent)
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
		return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
	}

	return QAbstractItemModel::flags(index);

}

QVariant LabelsTree::data(const QModelIndex &index, int role) const {

	Label* label = (Label*) index.internalPointer();

	switch (role) {

	case Qt::DisplayRole :

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

	default:
		return false;

	}

	return false;
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

	endInsertRows();

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


} // namespace Sabrina
