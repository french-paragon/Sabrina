#include "cartography.h"

namespace Sabrina {

const QString Cartography::CARTOGRAPHY_TYPE_ID = "sabrina_jdr_cartography";
const QString CartographyItem::CARTOGRAPHY_ITEM_TYPE_ID = Cartography::CARTOGRAPHY_TYPE_ID + "_item";
const QString CartographyCategory::CARTOGRAPHY_CATEGORY_TYPE_ID = Cartography::CARTOGRAPHY_TYPE_ID + "_category";

Cartography::Cartography(QString ref, Aline::EditableItemManager *parent) :
	EditableItem(ref, parent),
	_background(""),
	_categoryListModel(nullptr)
{

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

void Cartography::insertSubItem(EditableItem* item) {

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

	Sabrina::EditableItem* referedItem = qobject_cast<Sabrina::EditableItem*>(getManager()->loadItem(referedItemRef));

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
		emit cartographyItemRemoved(item->getRef());

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
	_scale(1)
{

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

qreal CartographyCategory::getScale() const {
	return _scale;
}

void CartographyCategory::setScale(qreal scale) {

	if (scale != _scale) {
		_scale = scale;
		emit scaleChanged(scale);
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
	_scale(1.0)
{
	setObjectName(EditableItem::simplifyRef(ref));

	if (_cartographyParent != nullptr) {
		connect(_cartographyParent, &Cartography::sizeChanged, this, &CartographyItem::checkWithin);
	}
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

	if (_colorPipeConnection) {
		disconnect(_colorPipeConnection);
	}

	linkCategory(category);
}

void CartographyItem::setCategory(QString ref) {

	if (_colorPipeConnection) {
		disconnect(_colorPipeConnection);
	}

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

qreal CartographyItem::getScale() const {
	return _scale;
}

void CartographyItem::setScale(qreal scale) {

	if (_scale != scale) {
		_scale = scale;
		emit scaleChanged(_scale);
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

	_colorPipeConnection = connect(category, &CartographyCategory::colorChanged, this, &CartographyItem::colorChanged);

	emit categoryChanged(category->getRef());

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

} // namespace Sabrina
