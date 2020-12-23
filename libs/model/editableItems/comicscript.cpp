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

#include "comicscript.h"

namespace Sabrina {

const QString Comicscript::COMICSTRIP_TYPE_ID = "sabrina_comic_script";

Comicscript::Comicscript(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent),
	_title("Titre")
{
	_model = new ComicscriptModel(this);
}

QString Comicscript::getTypeId() const {
	return COMICSTRIP_TYPE_ID;
}
QString Comicscript::getTypeName() const {
	return tr("Comic script");
}

QString Comicscript::iconInternalUrl() const {
	return ":/icons/icons/editable_item_comic.svg";
}

QString Comicscript::getTitle() const
{
	return _title;
}

void Comicscript::setTitle(const QString &title)
{
	if (title != _title) {
		_title = title;
		emit titleChanged(_title);
	}
}

ComicscriptModel* Comicscript::getModel() {
	return _model;
}

void Comicscript::treatDeletedRef(QString deletedRef) {
	Q_UNUSED(deletedRef);
}
void Comicscript::treatChangedRef(QString oldRef, QString newRef) {
	Q_UNUSED(oldRef);
	Q_UNUSED(newRef);
}


Comicscript::ComicstripFactory::ComicstripFactory(QObject *parent) :
	Aline::EditableItemFactory(parent)
{

}
Aline::EditableItem* Comicscript::ComicstripFactory::createItem(QString ref, Aline::EditableItemManager* parent) const {
	return new Comicscript(ref, parent);
}



ComicscriptModel::ComicscriptModel(Comicscript* script) :
	QAbstractItemModel(script)
{
	_pages.push_back(new PageBlock(this));
}

ComicscriptModel::~ComicscriptModel() {
	for (PageBlock* b : _pages) {
		delete b;
	}
}

QModelIndex ComicscriptModel::index(int row, int column, const QModelIndex &parent) const {

	if (column != 0) {
		return QModelIndex();
	}

	if (parent == QModelIndex()) {
		if (row >= 0 and row < _pages.size()) {
			return createIndex(row, 0, static_cast<ComicstripBlock*>(_pages.at(row)));
		} else {
			return QModelIndex();
		}
	}

	ComicstripBlock* p = reinterpret_cast<ComicstripBlock*>(parent.internalPointer());

	if (row >= 0 and row < p->_children.size()) {
		return createIndex(row, 0, static_cast<ComicstripBlock*>(p->_children.at(row)));
	}

	return QModelIndex();
}
QModelIndex ComicscriptModel::parent(const QModelIndex &index) const {

	ComicstripBlock* n = reinterpret_cast<ComicstripBlock*>(index.internalPointer());

	if (n->_parent == nullptr) {
		return QModelIndex();
	}

	if (n->_parent->_parent == nullptr and n->_parent->type() == ComicstripBlock::PAGE) {
		PageBlock* p = static_cast<PageBlock*>(n->_parent);
		int row = _pages.indexOf(p);

		if (row >= 0) {
			return createIndex(row, 0, static_cast<ComicstripBlock*>(p));
		} else {
			return QModelIndex();
		}
	}

	if (n->_parent->_parent != nullptr) {
		int row = n->_parent->_parent->_children.indexOf(n->_parent);

		if (row >= 0) {
			return createIndex(row, 0, n->_parent);
		} else {
			return QModelIndex();
		}
	}

	return QModelIndex();

}
int ComicscriptModel::rowCount(const QModelIndex &parent) const {

	if (parent == QModelIndex()) {
		return _pages.size();
	}

	ComicstripBlock* n = reinterpret_cast<ComicstripBlock*>(parent.internalPointer());

	return n->_children.size();

}
int ComicscriptModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return 1;
}
QVariant ComicscriptModel::data(const QModelIndex &index, int role) const {

	ComicstripBlock* n = reinterpret_cast<ComicstripBlock*>(index.internalPointer());

	switch (role) {
	case Qt::DisplayRole :
		return QString("%1 %2").arg(n->typeDescr(), index.row());
	case TextRole :
		return n->getText();
	case DescrRole :
		return n->getDescr();
	case TypeDescrRole:
		return n->typeDescr();
	case TypeRole:
		return static_cast<int>(n->type());
	case NbLinesRole:
		return (n->type() == ComicstripBlock::DIALOG) ? 2 : 1;
	default:
		break;
	}

	return QVariant();

}
Qt::ItemFlags ComicscriptModel::flags(const QModelIndex &index) const {
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool ComicscriptModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	ComicstripBlock* n = reinterpret_cast<ComicstripBlock*>(index.internalPointer());

	switch (role) {
	case DescrRole:
		if (n->type() == ComicstripBlock::DIALOG) {
			DialogBlock* d = static_cast<DialogBlock*>(n);
			d->setCharacter(value.toString());
			emit dataChanged(index, index, {DescrRole});
			return true;
		}
		return false;
	case TextRole:
		n->setText(value.toString());
		emit dataChanged(index, index, {TextRole});
		return true;
	default:
		return false;
	}
}


QModelIndex ComicscriptModel::getNextItem(QModelIndex const& index) {

	if (index == QModelIndex()) {
		return this->index(0,0);
	}

	if (index.model() != qobject_cast<QAbstractItemModel*>(this)) {
		return QModelIndex();
	}

	if (rowCount(index)) {
		return this->index(0, 0, index);
	}

	QModelIndex p = index.parent();
	QModelIndex i = index;

	while (i.row() == rowCount(p)-1) {

		if (p == QModelIndex()) {
			break;
		}

		i = p;
		p = i.parent();
	}

	return this->index(i.row()+1,0,p);

}

QModelIndex ComicscriptModel::getPrevItem(QModelIndex const& index) {


	if (index.model() != qobject_cast<QAbstractItemModel*>(this)) {
		return QModelIndex();
	}

	if (index.row() == 0) {
		return index.parent();
	}

	QModelIndex p = index.parent();

	p = this->index(index.row()-1,0,p);

	while (rowCount(p) > 0) {
		p = this->index(rowCount(p)-1,0,p);
	}

	return p;
}


void ComicscriptModel::addPage(int row) {

	int n = row;

	if (n < 0) {
		n = 0;
	}

	if (n > _pages.size()) {
		n = _pages.size();
	}

	beginInsertRows(QModelIndex(), n, n);

	_pages.insert(n, new PageBlock(this));

	endInsertRows();

}

void ComicscriptModel::addPage(int row, int subrow) {

	if (row <= 0 or row > _pages.size()) {
		addPage(row);
		return;
	}

	PageBlock* previous = _pages.at(row-1);

	if (previous->_children.size() <= subrow) {
		addPage(row);
		return;
	}

	beginInsertRows(QModelIndex(), row, row);

	PageBlock* added = new PageBlock(this);
	_pages.insert(row, added);

	endInsertRows();

	beginMoveRows(createIndex(row-1, 0, static_cast<ComicstripBlock*>(previous)),
				  subrow,
				  previous->_children.size()-1,
				  createIndex(row, 0, static_cast<ComicstripBlock*>(added)),
				  0);

	int p_size = previous->_children.size();
	for (int i = subrow; i < p_size; i++) {
		previous->_children.at(subrow)->setParent(added);
	}

	endMoveRows();
}

void ComicscriptModel::removePage(int row) {

	if (row < 1 or row >= _pages.size()) {
		return;
	}

	PageBlock* previous = _pages.at(row-1);
	PageBlock* target = _pages.at(row);

	beginMoveRows(createIndex(row-1, 0, static_cast<ComicstripBlock*>(target)),
				  0,
				  target->_children.size()-1,
				  createIndex(row, 0, static_cast<ComicstripBlock*>(previous)),
				  previous->_children.size());

	int p_size = target->_children.size();
	for (int i = 0; i < p_size; i++) {
		target->_children.at(0)->setParent(previous);
	}

	endMoveRows();

	beginRemoveRows(QModelIndex(), row, row);
	_pages.removeAt(row);
	delete target;
	endRemoveRows();

}

void ComicscriptModel::addPanel(QModelIndex const& pageIndex, int row) {

	int type = pageIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PAGE) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(pageIndex.internalPointer());
	PageBlock* p = static_cast<PageBlock*>(b);

	int n = row;

	if (n < 0) {
		n = 0;
	}

	if (n > p->_children.size()) {
		n = p->_children.size();
	}

	beginInsertRows(pageIndex, n, n);

	PanelBlock* pan = new PanelBlock(this, p);
	Q_UNUSED(pan);
	p->_children.move(p->_children.size()-1,row);

	endInsertRows();

}
void ComicscriptModel::addPanel(QModelIndex const& pageIndex, int row, int subrow) {

	int type = pageIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PAGE) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(pageIndex.internalPointer());
	PageBlock* p = static_cast<PageBlock*>(b);

	if (row <= 0 or row > p->_children.size()) {
		addPanel(pageIndex, row);
		return;
	}

	PanelBlock* previous = static_cast<PanelBlock*>(p->_children.at(row-1));

	if (previous->_children.size() <= subrow) {
		addPanel(pageIndex, row);
		return;
	}

	beginInsertRows(pageIndex, row, row);

	PanelBlock* added = new PanelBlock(this, p);
	p->_children.move(p->_children.size()-1,row);

	endInsertRows();

	beginMoveRows(createIndex(row-1, 0, static_cast<ComicstripBlock*>(previous)),
				  subrow,
				  previous->_children.size()-1,
				  createIndex(row, 0, static_cast<ComicstripBlock*>(added)),
				  0);

	int p_size = previous->_children.size();
	for (int i = subrow; i < p_size; i++) {
		previous->_children.at(subrow)->setParent(added);
	}

	endMoveRows();

}
void ComicscriptModel::removePanel(QModelIndex const& pageIndex, int row) {

	int type = pageIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PAGE) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(pageIndex.internalPointer());
	PageBlock* p = static_cast<PageBlock*>(b);

	if (row < 1 or row >= p->_children.size()) {
		return;
	}

	PanelBlock* previous = static_cast<PanelBlock*>(p->_children.at((row - 1)));
	PanelBlock* target = static_cast<PanelBlock*>(p->_children.at((row)));

	beginMoveRows(createIndex(row-1, 0, static_cast<ComicstripBlock*>(target)),
				  0,
				  target->_children.size()-1,
				  createIndex(row, 0, static_cast<ComicstripBlock*>(previous)),
				  previous->_children.size());

	int p_size = target->_children.size();
	for (int i = 0; i < p_size; i++) {
		target->_children.at(0)->setParent(previous);
	}

	endMoveRows();

	beginRemoveRows(QModelIndex(), row, row);
	p->_children.removeOne(static_cast<ComicstripBlock*>(target));
	delete target;
	endRemoveRows();


}

void ComicscriptModel::addCaption(QModelIndex const& panelIndex, int row) {

	int type = panelIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PANEL) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(panelIndex.internalPointer());
	PanelBlock* p = static_cast<PanelBlock*>(b);

	int n = row;

	if (n < 0) {
		n = 0;
	}

	if (n > p->_children.size()) {
		n = p->_children.size();
	}

	beginInsertRows(panelIndex, n, n);

	CaptionBlock* capt = new CaptionBlock(this, p);
	Q_UNUSED(capt);
	p->_children.move(p->_children.size()-1,row);

	endInsertRows();

}
void ComicscriptModel::removeCaption(QModelIndex const& panelIndex, int row) {

	int type = panelIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PANEL) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(panelIndex.internalPointer());
	PanelBlock* p = static_cast<PanelBlock*>(b);

	if (row < 0 or row >= p->_children.size()) {
		return;
	}

	ComicstripBlock* target = p->_children.at((row));

	beginRemoveRows(QModelIndex(), row, row);
	p->_children.removeOne(static_cast<ComicstripBlock*>(target));
	delete target;
	endRemoveRows();

}

void ComicscriptModel::addDialog(QModelIndex const& panelIndex, int row) {

	int type = panelIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PANEL) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(panelIndex.internalPointer());
	PanelBlock* p = static_cast<PanelBlock*>(b);

	int n = row;

	if (n < 0) {
		n = 0;
	}

	if (n > p->_children.size()) {
		n = p->_children.size();
	}

	beginInsertRows(panelIndex, n, n);

	DialogBlock* diag = new DialogBlock(this, p);
	Q_UNUSED(diag);
	p->_children.move(p->_children.size()-1,row);

	endInsertRows();

}
void ComicscriptModel::removeDialog(QModelIndex const& panelIndex, int row) {

	int type = panelIndex.data(TypeRole).toInt();
	if (type != ComicstripBlock::PANEL) {
		return;
	}

	ComicstripBlock* b = reinterpret_cast<ComicstripBlock*>(panelIndex.internalPointer());
	PanelBlock* p = static_cast<PanelBlock*>(b);

	if (row < 0 or row >= p->_children.size()) {
		return;
	}

	ComicstripBlock* target = p->_children.at((row));

	beginRemoveRows(QModelIndex(), row, row);
	p->_children.removeOne(static_cast<ComicstripBlock*>(target));
	delete target;
	endRemoveRows();
}


ComicscriptModel::ComicstripBlock::ComicstripBlock(ComicscriptModel* model, ComicstripBlock* parent) :
	_parent(parent),
	_model(model),
	_children()
{
	if (_parent != nullptr) {
		_parent->_children.push_back(this);
	}
}

ComicscriptModel::ComicstripBlock::~ComicstripBlock() {
	for (ComicstripBlock * c : _children) {
		delete c;
	}
}

QString ComicscriptModel::ComicstripBlock::typeDescr() const {
	switch (type()) {
	case OTHER:
		return tr("Other");
	case PAGE:
		return tr("Page");
	case PANEL:
		return tr("Panel");
	case CAPTION:
		return tr("Caption");
	case DIALOG:
		return tr("Dialog");
	}
}

QString ComicscriptModel::ComicstripBlock::getText() const {
	return _text;
}
void ComicscriptModel::ComicstripBlock::setText(QString const& str) {
	_text = str;
}

ComicscriptModel::ComicstripBlock* ComicscriptModel::ComicstripBlock::getParent() const {
	return _parent;
}
void ComicscriptModel::ComicstripBlock::setParent(ComicstripBlock* parent) {

	if (_parent != nullptr) {
		_parent->_children.removeOne(this);
	}

	_parent = parent;

	if (_parent != nullptr) {
		_parent->_children.push_back(this);
	}
}

int ComicscriptModel::ComicstripBlock::getChildrenId(const ComicstripBlock *b) const {
	return _children.indexOf(const_cast<ComicstripBlock*>(b));
}

QString ComicscriptModel::ComicstripBlock::getDescr() const {
	return "";
}


ComicscriptModel::PageBlock::PageBlock(ComicscriptModel* model) :
	ComicstripBlock(model, nullptr)
{

}

ComicscriptModel::ComicstripBlock::BlockType ComicscriptModel::PageBlock::type() const {
	return PAGE;
}

QString ComicscriptModel::PageBlock::getDescr() const {

	int n = 0;

	if (_model != nullptr) {
		int id = _model->_pages.indexOf(const_cast<PageBlock*>(this));
		if (id >= 0) {
			n = id;
		}
	}

	return QString("Page %1").arg(n+1);

}


ComicscriptModel::PanelBlock::PanelBlock(ComicscriptModel* model, PageBlock* parent) :
	ComicstripBlock(model, parent)
{

}

ComicscriptModel::ComicstripBlock::BlockType ComicscriptModel::PanelBlock::type() const {
	return PANEL;
}

QString ComicscriptModel::PanelBlock::getDescr() const {
	int n = 0;

	if (_parent != nullptr) {
		if (_parent->type() == PAGE) {
			int id = _parent->getChildrenId(this);
			if (id >= 0) {
				n = id;
			}
		}
	}

	return QString("Panel %1").arg(n+1);
}


ComicscriptModel::CaptionBlock::CaptionBlock(ComicscriptModel* model, PanelBlock* parent) :
	ComicstripBlock(model, parent)
{

}

ComicscriptModel::ComicstripBlock::BlockType ComicscriptModel::CaptionBlock::type() const {
	return CAPTION;
}


ComicscriptModel::DialogBlock::DialogBlock(ComicscriptModel* model, PanelBlock* parent) :
	ComicstripBlock(model, parent)
{

}
ComicscriptModel::ComicstripBlock::BlockType ComicscriptModel::DialogBlock::type() const {
	return DIALOG;
}
void ComicscriptModel::DialogBlock::setCharacter(QString const& name) {
	_characterName = name;
}
QString ComicscriptModel::DialogBlock::getDescr() const {
	return _characterName;
}


} // namespace Sabrina
