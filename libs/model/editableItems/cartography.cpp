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

#include "cartography.h"
#include "model/editableitemmanager.h"

#include <QFileInfo>
#include <QMetaEnum>
#include <QIcon>

namespace Sabrina {

const QString Cartography::CARTOGRAPHY_TYPE_ID = "sabrina_jdr_cartography";
const QString CartographyItem::CARTOGRAPHY_ITEM_TYPE_ID = Cartography::CARTOGRAPHY_TYPE_ID + "_item";
const QString CartographyCategory::CARTOGRAPHY_CATEGORY_TYPE_ID = Cartography::CARTOGRAPHY_TYPE_ID + "_category";

Cartography::Cartography(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent),
	_background(""),
	_categoryListModel(nullptr)
{
	connect(this, &Cartography::backgroundChanged,
			this, &Cartography::newUnsavedChanges);
	connect(this, &Cartography::sizeChanged,
			this, &Cartography::newUnsavedChanges);

	connect(this, &Cartography::cartographyCategoryInserted,
			this, &Cartography::newUnsavedChanges);
	connect(this, &Cartography::cartographyCategoryRemoved,
			this, &Cartography::newUnsavedChanges);

	connect(this, &Cartography::cartographyItemInserted,
			this, &Cartography::newUnsavedChanges);
	connect(this, &Cartography::cartographyItemRemoved,
			this, &Cartography::newUnsavedChanges);
}

QString Cartography::getTypeId() const {
	return CARTOGRAPHY_TYPE_ID;
}
QString Cartography::getTypeName() const {
	return tr("Carte");
}

QString Cartography::iconInternalUrl() const {
	return ":/icons/icons/editable_item_map.svg";
}

QString Cartography::background() const {
	return _background;
}

void Cartography::insertSubItem(Aline::EditableItem* item) {

	CartographyItem* cartoItem = qobject_cast<CartographyItem*>(item);
	CartographyCategory* cartoCat = qobject_cast<CartographyCategory*>(item);

	if (cartoItem != nullptr) {
		insertCartoItem(cartoItem);
	} else if (cartoCat != nullptr) {
		insertCartoCat(cartoCat);
	} else {
		Aline::EditableItem::insertSubItem(item);
	}

}

void Cartography::setBackground(QString url) {
	if (url != _background) {
		_background = url;
		emit backgroundChanged(_background);
	}
}

void Cartography::addCartoItem(QString const& referedItemRef) {

	Sabrina::EditableItem* referedItem;

	try {

		referedItem = qobject_cast<Sabrina::EditableItem*>(getManager()->loadItem(referedItemRef));

	} catch (ItemIOException const& e) {
		return;
	}

	if (referedItem == nullptr) {
		return;
	}

	referedItem->addOutRef(getRef());
	addInRef(referedItemRef);

	QString carto_item_ref = EditableItem::simplifyRef(referedItemRef + "_cartoitem");

	carto_item_ref = makeSubItemRefUniq(carto_item_ref);

	CartographyItem* item = new CartographyItem(carto_item_ref, this);
	item->setReferedItem(referedItemRef);

	CartographyCategory* defaultCat = getDefaultCat();
	item->setCategory(defaultCat);

	QSizeF _middle = _size/2.0;
	QPointF _mPos(_middle.width(), _middle.height());
	item->setPosition(_mPos);

	insertCartoItem(item);

}

void Cartography::addCartoPoint(QString const& name) {

	QString carto_item_ref = EditableItem::simplifyRef(name + "_cartoitem");

	carto_item_ref = makeSubItemRefUniq(carto_item_ref);

	CartographyItem* item = new CartographyItem(carto_item_ref, this);

	CartographyCategory* defaultCat = getDefaultCat();
	item->setCategory(defaultCat);

	QSizeF _middle = _size/2.0;
	QPointF _mPos(_middle.width(), _middle.height());
	item->setPosition(_mPos);

	insertCartoItem(item);
}

void Cartography::addCartoPoint() {
	addCartoPoint(tr("Nouveau point"));
}

void Cartography::insertCartoItem(CartographyItem* item) {

	if (!_items.contains(item)) {
		Aline::EditableItem::insertSubItem(item);
		_items.push_back(item);
		emit cartographyItemInserted(item);
	}

}

void Cartography::addCategory(QString const& cat_name) {

	QString ref = EditableItem::simplifyRef(cat_name + "_cartocategory");

	ref = makeSubItemRefUniq(ref);

	CartographyCategory* cat = new CartographyCategory(ref, this);

	cat->setObjectName(cat_name);

	insertCartoCat(cat);

}

void Cartography::clearCartographyItems() {

	QVector<CartographyItem*> items = _items;

	for (CartographyItem* item : items) {
		removeCartoItem(item);
	}

}

void Cartography::loadCartographyItems(QList<Aline::EditableItem*> const& items) {

	if (_items.count() > 0) {
		clearCartographyItems();
	}

	for (Aline::EditableItem* item : items) {

		CartographyItem* cartoItem = qobject_cast<CartographyItem*>(item);

		if (cartoItem != nullptr) {
			insertCartoItem(cartoItem);
		}

	}

}

void Cartography::clearCartographyCategories() {

	QList<CartographyCategory*> categories = _categories.values();

	for (CartographyCategory* category : categories) {
		removeCartoCategory(category);
	}

}

void Cartography::loadCartographyCategories(QList<Aline::EditableItem*> const& items) {

	if (_categories.count() > 0) {
		clearCartographyCategories();
	}

	for (Aline::EditableItem* item : items) {

		CartographyCategory* cat = qobject_cast<CartographyCategory*>(item);

		if (cat != nullptr) {
			insertCartoCat(cat);
		}

	}

}

void Cartography::removeCartoItem(CartographyItem* item) {

	if (item->_cartographyParent == this) {

		_items.removeOne(item);
		emit cartographyItemRemoved(item->getRef());

		item->deleteLater();

		removeSubItemRef(item);

	}

}

void Cartography::removeCartoCategory(CartographyCategory* item) {

	if (item->_cartographyParent == this) {

		_categories.remove(item->getRef());
		emit cartographyCategoryRemoved(item->getRef());

		item->deleteLater();

		removeSubItemRef(item);

	}
}

void Cartography::insertCartoCat(CartographyCategory* item) {

	if (!_categories.values().contains(item)) {
		Aline::EditableItem::insertSubItem(item);
		_categories.insert(item->getRef(), item);
		emit cartographyCategoryInserted(item);
	}

}

QSizeF Cartography::getSize() const
{
	return _size;
}

void Cartography::setSize(const QSizeF &size)
{
	if (size != _size) {
		_size = size;
		emit sizeChanged(_size);
	}
}

QList<Aline::EditableItem *> Cartography::cartographyCategories() const {

	QList<Aline::EditableItem *> ret;
	ret.reserve(_categories.size());

	for (CartographyCategory* cat : _categories.values()) {
		ret << cat;
	}

	return ret;

}

QList<QString> Cartography::getCurrentCategoriesRefs() const {
	return _categories.keys();
}

CartographyCategory* Cartography::getCategoryByRef(QString ref, bool returnDefaultIfNotFound) {

	CartographyCategory* cat = _categories.value(ref, nullptr);

	if (returnDefaultIfNotFound && cat == nullptr) {
		return getDefaultCat();
	}

	return cat;

}

CartographyCategroryListModel* Cartography::getCategoryListModel() {

	if (_categoryListModel == nullptr) {
		_categoryListModel = new CartographyCategroryListModel(this);
	}

	return _categoryListModel;

}

QStringList Cartography::getFileReferencePropertiesName() const {

	QFileInfo info(_background);


	if (info.exists()) {
		return {"background"};
	}

	return {};
}

CartographyCategory* Cartography::getDefaultCat() {

	if (_categories.size() == 0) {
		createDefaultCat();
	}

	return _categories.first();

}
void Cartography::createDefaultCat() {

	addCategory("default");

}

void Cartography::treatDeletedRef(QString deletedRef) {

	for (CartographyItem* item : _items) {

		if (item->getReferedItem() == deletedRef) {
			removeCartoItem(item);
			return;
		}

	}
}

void Cartography::treatChangedRef(QString oldRef, QString newRef) {

	for (CartographyItem* item : _items) {

		if (item->getReferedItem() == oldRef) {
			item->setReferedItem(newRef, false);
			return;
		}

	}
}

QList<Aline::EditableItem*> Cartography::cartographyItems() const {

	QList<Aline::EditableItem*> ret;
	ret.reserve(_items.size());

	for (CartographyItem * item : _items) {
		ret << item;
	}

	return ret;
}

QVector<CartographyItem *> const& Cartography::getItems() const
{
    return _items;
}

Cartography::CartographyFactory::CartographyFactory(QObject *parent) :
    Aline::EditableItemFactory(parent)
{

}

Aline::EditableItem* Cartography::CartographyFactory::createItem(QString ref, Aline::EditableItemManager* parent) const {
	return new Cartography(ref, parent);
}

CartographyCategory::CartographyCategory(QString ref, Cartography* parent) :
	Aline::EditableItem(ref, parent),
	_cartographyParent(parent),
	_color(207, 85, 64),
	_radius(5),
	_border_color(0, 0, 0),
	_border(2),
	_legend_font("sans"),
	_legend_underlined(false),
	_legend_bold(true),
	_legend_italic(false),
	_legend_size(10)
{
	connect(this, &CartographyCategory::colorChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::radiusChanged,
			this, &CartographyCategory::newUnsavedChanges);

	connect(this, &CartographyCategory::borderColorChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::borderChanged,
			this, &CartographyCategory::newUnsavedChanges);

	connect(this, &CartographyCategory::legendFontChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::legendUnderlinedChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::legendBoldChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::legendItalicChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::legendSizeChanged,
			this, &CartographyCategory::newUnsavedChanges);
	connect(this, &CartographyCategory::legendColorChanged,
			this, &CartographyCategory::newUnsavedChanges);
}

QString CartographyCategory::getTypeId() const {
	return CARTOGRAPHY_CATEGORY_TYPE_ID;
}

QString CartographyCategory::getTypeName() const {
	return tr("catégorie de carte");
}

QString CartographyCategory::iconInternalUrl() const {
	return ":/icons/icons/editable_item_map.svg";
}

QColor CartographyCategory::getColor() const
{
	return _color;
}

void CartographyCategory::setColor(const QColor &color)
{
	if (color != _color) {
		_color = color;
		emit colorChanged(_color);
	}
}

qreal CartographyCategory::getRadius() const {
	return _radius;
}

void CartographyCategory::setRadius(qreal radius) {

	if (radius != _radius) {
		_radius = radius;
		emit radiusChanged(radius);
	}

}

QColor CartographyCategory::getBorderColor() const
{
	return _border_color;
}

void CartographyCategory::setBorderColor(const QColor &border_color)
{
	if (border_color != _border_color) {
		_border_color = border_color;
		emit borderColorChanged(border_color);
	}
}

qreal CartographyCategory::getBorder() const
{
	return _border;
}

void CartographyCategory::setBorder(const qreal &border)
{
	if (border != _border) {
		_border = border;
		emit borderChanged(border);
	}
}

QString CartographyCategory::getLegendFont() const
{
	return _legend_font;
}

void CartographyCategory::setLegendFont(const QString &legend_font)
{
	if (_legend_font != legend_font) {
		_legend_font = legend_font;
		emit legendFontChanged(legend_font);
	}
}

bool CartographyCategory::getLegendUnderlined() const
{
	return _legend_underlined;
}

void CartographyCategory::setLegendUnderlined(bool legend_underlined)
{
	if (legend_underlined != _legend_underlined) {
		_legend_underlined = legend_underlined;
		emit legendUnderlinedChanged(legend_underlined);
	}
}

bool CartographyCategory::getLegendBold() const
{
	return _legend_bold;
}

void CartographyCategory::setLegendBold(bool legend_bold)
{
	if (_legend_bold != legend_bold) {
		_legend_bold = legend_bold;
		emit legendBoldChanged(legend_bold);
	}
}

bool CartographyCategory::getLegendItalic() const
{
	return _legend_italic;
}

void CartographyCategory::setLegendItalic(bool legend_italic)
{
	if (_legend_italic != legend_italic) {
		_legend_italic = legend_italic;
		emit legendItalicChanged(legend_italic);
	}
}

int CartographyCategory::getLegendSize() const
{
	return _legend_size;
}

void CartographyCategory::setLegendSize(int legend_size)
{
	if (_legend_size != legend_size) {
		_legend_size = legend_size;
		emit legendSizeChanged(legend_size);
	}
}

QColor CartographyCategory::getLegendColor() const
{
	return _legendColor;
}

void CartographyCategory::setLegendColor(const QColor &legendColor)
{
	if (_legendColor != legendColor) {
		_legendColor = legendColor;
		emit legendColorChanged(legendColor);
	}
}

CartographyCategory::CartographyCategoryFactory::CartographyCategoryFactory(QObject *parent) :
	Aline::EditableSubItemFactory(parent)
{

}

Aline::EditableItem* CartographyCategory::CartographyCategoryFactory::createItem(QString ref, Aline::EditableItem* parent) const {

	Cartography* cParent = qobject_cast<Cartography*>(parent);

	if (cParent == nullptr && parent != nullptr) {
		return nullptr; //parent has to be a Cartography or a nullptr.
	}

	return new CartographyCategory(ref, cParent);

}

CartographyItem::CartographyItem(QString ref, Cartography *parent) :
	Aline::EditableItem(ref, parent),
	_cartographyParent(parent),
	_category(""),
	_referedItem(QString()),
	_scale(1.0),
	_legendPos(TOP_MIDDLE)
{
	this->blockSignals(true);
	setObjectName(EditableItem::simplifyRef(ref));
	this->blockSignals(false);

	if (_cartographyParent != nullptr) {
		connect(_cartographyParent, &Cartography::sizeChanged, this, &CartographyItem::checkWithin);
	}

	connect(this, &CartographyItem::referedItemChanged,
			this, &CartographyItem::newUnsavedChanges);
	connect(this, &CartographyItem::positionChanged,
			this, &CartographyItem::newUnsavedChanges);
	connect(this, &CartographyItem::categoryChanged,
			this, &CartographyItem::newUnsavedChanges);
	connect(this, &CartographyItem::scaleChanged,
			this, &CartographyItem::newUnsavedChanges);
	connect(this, &CartographyItem::legendPositionChanged,
			this, &CartographyItem::newUnsavedChanges);
}

QString CartographyItem::getTypeId() const {
	return CARTOGRAPHY_ITEM_TYPE_ID;
}

QString CartographyItem::getTypeName() const {
	return tr("Élément de carte");
}

QString CartographyItem::iconInternalUrl() const {
	return ":/icons/icons/editable_item_place.svg";
}

void CartographyItem::setCategory(CartographyCategory* category) {

	disconnectPipedSignals();

	linkCategory(category);
}

void CartographyItem::setCategory(QString ref) {

	disconnectPipedSignals();

	_category = ref;
	linkCategory(_cartographyParent->getCategoryByRef(_category, true));
}

QString CartographyItem::getReferedItem() const
{
	return _referedItem;
}

void CartographyItem::setReferedItem(const QString &referedItem, bool rename)
{
	if (referedItem == _referedItem) {
		return;
	}

	if (referedItem == QString()) {
		_referedItem = referedItem;
		emit referedItemChanged(_referedItem);
		emit linkedStatusChanged(false);
		return;
	}

	if (_cartographyParent != nullptr && rename) {

		Aline::EditableItemManager* pManager = _cartographyParent->getManager();

		if (pManager != nullptr) {

			QModelIndex index = pManager->indexFromRef(referedItem);

			if (index.isValid()) {

				QString name = pManager->data(index, Qt::DisplayRole).toString();

				setObjectName(name);

			}

		}

	}

	if (_referedItem == QString()) {
		_referedItem = referedItem;
		emit referedItemChanged(_referedItem);
		emit linkedStatusChanged(true);
		return;
	}

	_referedItem = referedItem;
	emit referedItemChanged(_referedItem);
}

QPointF CartographyItem::getPosition() const
{
	return _position;
}

void CartographyItem::setPosition(const QPointF &position)
{
	if (position != _position) {
		_position = position;
		emit positionChanged(_position);
	}
}

QString CartographyItem::getCategoryRef() const
{
	return _category;
}

QColor CartographyItem::getPointColor() const {

	return _linkedCategory->getColor();

}
qreal CartographyItem::getRadius() const {
	return _linkedCategory->getRadius();
}

QColor CartographyItem::getBorderColor() const {
	return _linkedCategory->getBorderColor();
}

qreal CartographyItem::getBorder() const {
	return _linkedCategory->getBorder();
}

QString CartographyItem::getLegendFont() const {
	return _linkedCategory->getLegendFont();
}

bool CartographyItem::getLegendUnderlined() const {
	return _linkedCategory->getLegendUnderlined();
}

bool CartographyItem::getLegendBold() const {
	return _linkedCategory->getLegendBold();
}

bool CartographyItem::getLegendItalic() const {
	return _linkedCategory->getLegendItalic();
}

int CartographyItem::getLegendSize() const {
	return _linkedCategory->getLegendSize();
}

QColor CartographyItem::getLegendColor() const {
	return _linkedCategory->getLegendColor();
}

qreal CartographyItem::getScale() const {
	return _scale;
}

void CartographyItem::setScale(qreal scale) {

	if (_scale != scale) {
		_scale = scale;
		emit scaleChanged(_scale);
	}

}

CartographyItem::LegendPos CartographyItem::getLegendPosition() const {
	return _legendPos;
}

void CartographyItem::setLegendPosition(LegendPos pos) {

	if (pos != _legendPos) {
		_legendPos = pos;
		emit legendPositionChanged(pos);
	}

}

bool CartographyItem::isLinked() const {
	return _referedItem != QString();
}

void CartographyItem::checkWithin(QSizeF const& mapSize) {

	if (_position.x() > mapSize.width() || _position.y() > mapSize.height()) {
		QPointF newPos = _position;

		if (_position.x() > mapSize.width()) {
			newPos.setX(mapSize.width());
		}

		if (_position.y() > mapSize.height()) {
			newPos.setY(mapSize.height());
		}

		setPosition(newPos);
	}

}

void CartographyItem::linkCategory(CartographyCategory* category) {

	_category = category->getRef();
	_linkedCategory = category;

	_colorPipeConnection = connect(category, &CartographyCategory::colorChanged,
								   this, &CartographyItem::colorChanged);

	_radiusPipeConnection = connect(category, &CartographyCategory::radiusChanged,
									this, &CartographyItem::radiusChanged);


	_borderColorPipeConnection = connect(category, &CartographyCategory::borderColorChanged,
										 this, &CartographyItem::borderColorChanged);

	_borderPipeConnection = connect(category, &CartographyCategory::borderChanged,
									this, &CartographyItem::borderChanged);


	_legendFontPipeConnection = connect(category, &CartographyCategory::legendFontChanged,
										this, &CartographyItem::legendFontChanged);

	_legendUnderlinedPipeConnection = connect(category, &CartographyCategory::legendUnderlinedChanged,
											  this, &CartographyItem::legendUnderlinedChanged);

	_legendBoldPipeConnection = connect(category, &CartographyCategory::legendBoldChanged,
										this, &CartographyItem::legendBoldChanged);

	_legendItalicPipeConnection = connect(category, &CartographyCategory::legendItalicChanged,
										  this, &CartographyItem::legendItalicChanged);

	_legendSizePipeConnection = connect(category, &CartographyCategory::legendSizeChanged,
										this, &CartographyItem::legendSizeChanged);

	_legendColorPipeConnection = connect(category, &CartographyCategory::legendColorChanged,
										 this, &CartographyItem::legendColorChanged);

	emit categoryChanged(category->getRef());

}

void CartographyItem::disconnectPipedSignals() {

	if (_colorPipeConnection) {
		disconnect(_colorPipeConnection);
	}

	if (_radiusPipeConnection) {
		disconnect(_radiusPipeConnection);
	}


	if (_borderColorPipeConnection) {
		disconnect(_borderColorPipeConnection);
	}

	if (_borderPipeConnection) {
		disconnect(_borderPipeConnection);
	}


	if (_legendFontPipeConnection) {
		disconnect(_legendFontPipeConnection);
	}

	if (_legendUnderlinedPipeConnection) {
		disconnect(_legendUnderlinedPipeConnection);
	}

	if (_legendBoldPipeConnection) {
		disconnect(_legendBoldPipeConnection);
	}

	if (_legendItalicPipeConnection) {
		disconnect(_legendItalicPipeConnection);
	}

	if (_legendFontPipeConnection) {
		disconnect(_legendItalicPipeConnection);
	}

	if (_legendColorPipeConnection) {
		disconnect(_legendColorPipeConnection);
	}

}

Cartography *CartographyItem::getCartographyParent() const
{
    return _cartographyParent;
}


CartographyItem::CartographyItemFactory::CartographyItemFactory(QObject *parent) :
    Aline::EditableSubItemFactory(parent)
{

}

Aline::EditableItem* CartographyItem::CartographyItemFactory::createItem(QString ref, Aline::EditableItem* parent) const {

	Cartography* cParent = qobject_cast<Cartography*>(parent);

	if (cParent == nullptr && parent != nullptr) {
		return nullptr; //parent has to be a Cartography or a nullptr.
	}

	return new CartographyItem(ref, cParent);

}


CartographyCategroryListModel::CartographyCategroryListModel(Cartography* carto) :
	QAbstractListModel(carto),
	_parentCarto(carto)
{

	QList<QString> keys = carto->getCurrentCategoriesRefs();

	for (QString ref : keys) {
		CartographyCategory* cat = carto->getCategoryByRef(ref);

		if (cat != nullptr) {
			_dict.insert(ref, cat);
			_categories.push_back(cat);
		}
	}

	connect(carto, &Cartography::cartographyCategoryInserted, this,
			&CartographyCategroryListModel::insertCategory);

	connect(carto, &Cartography::cartographyCategoryRemoved,
			this, &CartographyCategroryListModel::removeCategory);

}

int CartographyCategroryListModel::rowCount(const QModelIndex &parent) const {
	return _categories.size();
}

QVariant CartographyCategroryListModel::data(const QModelIndex &index, int role) const {

	CartographyCategory* cat = _categories.at(index.row());

	switch (role) {

	case Aline::EditableItemManager::ItemRefRole:
		return cat->getRef();

	case Qt::DisplayRole:
	case Qt::EditRole:
		return cat->objectName();

	case Qt::DecorationRole:
		return cat->getColor();

	default:
		return QVariant();
	}

}

QVariant CartographyCategroryListModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		return tr("Catégories");
	}

	return QVariant();
}

Qt::ItemFlags CartographyCategroryListModel::flags(const QModelIndex &index) const {
	return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool CartographyCategroryListModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	int row = index.row();

	if (role == Qt::EditRole || role == Qt::DisplayRole) {
		_categories.at(row)->setObjectName(value.toString());
		return true;
	}

	return false;

}

void CartographyCategroryListModel::insertCategory(CartographyCategory* cat) {

	if (_dict.contains(cat->getRef())) {
		return;
	}

	beginInsertRows(QModelIndex(), _categories.count(), _categories.count());

	_categories.push_back(cat);
	_dict.insert(cat->getRef(), cat);
	connect(cat, &Aline::EditableItem::refSwap,
			this, &CartographyCategroryListModel::onRefSwap);

	std::function<void()> onCartographyCategroyDataChanged = [this, cat] () {
		int row = this->_categories.indexOf(cat);
		emit this->dataChanged(this->index(row), this->index(row));
	};

	connect(cat, &QObject::objectNameChanged, onCartographyCategroyDataChanged);
	connect(cat, &CartographyCategory::colorChanged, onCartographyCategroyDataChanged);

	endInsertRows();

}
void CartographyCategroryListModel::removeCategory(QString ref) {

	CartographyCategory* oldCat = _dict.value(ref, nullptr);

	if (oldCat != nullptr) {

		int row = _categories.indexOf(oldCat);

		beginRemoveRows(QModelIndex(), row, row);

		_categories.removeAt(row);
		_dict.remove(ref);
		disconnect(oldCat, &Aline::EditableItem::refSwap,
				   this, &CartographyCategroryListModel::onRefSwap);

		endRemoveRows();

	}

}

void CartographyCategroryListModel::onRefSwap(QString oldRef, QString newRef) {

	if (_dict.contains(oldRef)) {
		CartographyCategory* cat = _dict.value(oldRef, nullptr);

		if (cat != nullptr) {
			_dict.remove(oldRef);
			_dict.insert(newRef, cat);

			int row = _categories.indexOf(cat);

			emit dataChanged(index(row), index(row), {Aline::EditableItemManager::ItemRefRole});
		}
	}

}


CartographyItemLegendPosListModel CartographyItemLegendPosListModel::GlobalLegendposModel;

CartographyItemLegendPosListModel::CartographyItemLegendPosListModel(QObject* parent):
	QAbstractListModel(parent)
{

}

int CartographyItemLegendPosListModel::rowCount(const QModelIndex &parent) const {
	QMetaEnum metaEnum = QMetaEnum::fromType<CartographyItem::LegendPos>();
	return metaEnum.keyCount();
}
QVariant CartographyItemLegendPosListModel::data(const QModelIndex &index, int role) const {

	QMetaEnum metaEnum = QMetaEnum::fromType<CartographyItem::LegendPos>();

	switch (role) {
	case Qt::DisplayRole:
		return QString(metaEnum.valueToKey(index.row()));
	case Qt::DecorationRole:
		switch (index.row()) {
		case CartographyItem::TOP_LEFT:
			return QIcon(":/icons/icons/legendAnchors/top_left.svg");
		case CartographyItem::TOP_MIDDLE:
			return QIcon(":/icons/icons/legendAnchors/top_middle.svg");
		case CartographyItem::TOP_RIGHT:
			return QIcon(":/icons/icons/legendAnchors/top_right.svg");
		case CartographyItem::MIDDLE_LEFT:
			return QIcon(":/icons/icons/legendAnchors/middle_left.svg");
		case CartographyItem::MIDDLE_RIGHT:
			return QIcon(":/icons/icons/legendAnchors/middle_right.svg");
		case CartographyItem::BOTTOM_LEFT:
			return QIcon(":/icons/icons/legendAnchors/bottom_left.svg");
		case CartographyItem::BOTTOM_MIDDLE:
			return QIcon(":/icons/icons/legendAnchors/bottom_middle.svg");
		case CartographyItem::BOTTOM_RIGHT:
			return QIcon(":/icons/icons/legendAnchors/bottom_right.svg");
		default:
			break;
		}
	default:
		break;
	}

	return QVariant();
}

} // namespace Sabrina
