#include "noteslist.h"

#include <QColor>
#include <QDate>

namespace Sabrina {

QVector<int> NotesList::savableRoles() {

	return {Qt::DisplayRole, TitleRole, Qt::DecorationRole, DateRole, AuthorRole};

}

NotesList::NotesList(QObject *parent) : QAbstractListModel(parent)
{

}

int NotesList::rowCount(const QModelIndex &parent) const {
	if (parent != QModelIndex()) {
		return 0;
	}

	return _content.size();

}

QVariant NotesList::data(const QModelIndex &index, int role) const {

	if (index.parent() != QModelIndex()) {
		return QVariant();
	}

	switch (role) {

	case TitleRole:
		return _content[index.row()]._title;

	case Qt::DisplayRole:
		return _content[index.row()]._data;

	case Qt::DecorationRole:
	{
		QColor c = _content[index.row()]._color;
		if (!c.isValid()) {
			c = QColor(255, 249, 185);
		}
		return c;
	}

	case DateRole:
		return _content[index.row()]._date;

	case AuthorRole:
		return _content[index.row()]._author;

	default:
		break;
	}

	return QVariant();
}

QHash<int, QByteArray> NotesList::roleNames() const {

	QHash<int, QByteArray> ret = QAbstractListModel::roleNames();

	ret.insert(TitleRole, "title");
	ret.insert(DateRole, "date");
	ret.insert(AuthorRole, "author");

	return ret;
}

Qt::ItemFlags NotesList::flags(const QModelIndex &index) const {

	return Qt::ItemIsEditable | QAbstractListModel::flags(index);

}

bool NotesList::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (index.parent() != QModelIndex()) {
		return false;
	}

	switch (role) {

	case TitleRole:
		_content[index.row()]._title = value.toString();
		emit dataChanged(index, index, {TitleRole});
		return true;

	case Qt::EditRole:
	case Qt::DisplayRole:
		_content[index.row()]._data = value.toString();
		emit dataChanged(index, index, {Qt::DisplayRole});
		return true;

	case AuthorRole:
		_content[index.row()]._author = value.toString();
		emit dataChanged(index, index, {AuthorRole});
		return true;

	case DateRole:
		if (value.canConvert(qMetaTypeId<QDateTime>())) {
			_content[index.row()]._date = value.toDateTime();
			emit dataChanged(index, index, {DateRole});
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

bool NotesList::insertRows(int row, int count, const QModelIndex &parent) {

	if (parent != QModelIndex()) {
		return false;
	}

	beginInsertRows(parent, row, row+count-1);

	QString username = qgetenv("USER");
	if (username.isEmpty()) {
		username = qgetenv("USERNAME");
	}

	_content.insert(row, count, {"", "", QDateTime::currentDateTime(), username, QColor()});

	endInsertRows();

}
bool NotesList::removeRows(int row, int count, const QModelIndex &parent) {

	if (parent != QModelIndex()) {
		return false;
	}

	beginRemoveRows(parent, row, row+count-1);

	_content.remove(row, count);

	endRemoveRows();

}

} //namespace Sabrina
