#include "cartographyeditor.h"
#include "ui_cartographyeditor.h"

#include "utils/settings_global_keys.h"
#include "model/editableItems/cartography.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>

#include <QSettings>
#include <QFileDialog>
#include <QColorDialog>
#include <QImage>
#include <QSortFilterProxyModel>

namespace Sabrina {

bool CartographyEditor::qmlTypeRegistrationDone = false;
const QString CartographyEditor::CARTOGRAPHY_EDITOR_TYPE_ID = "sabrina_jdr_cartography_editor";

CartographyEditor::CartographyEditor(QWidget *parent) :
	Aline::EditableItemEditor(parent),
	ui(new Ui::CartographyEditor),
	_currentCartography(nullptr),
	_selectedItem(nullptr),
	_currentCategory(nullptr)
{
	ui->setupUi(this);

	ui->splitter->setSizes({this->width()*2/3,this->width()/3});
	ui->backgroundDisplayEdit->setReadOnly(true);

	ui->spinBoxItemScale->setEnabled(false);
	ui->comboBoxSelectCategory->setEnabled(false);

	ui->selectColorButton->setEnabled(false);
	ui->categoryScaleSpinBox->setEnabled(false);

	_categoryListProxy = new QSortFilterProxyModel(this);

	ui->categoryListView->setModel(_categoryListProxy);
	ui->comboBoxSelectCategory->setModel(_categoryListProxy);

	ui->legendPosComboBox->setModel(&CartographyItemLegendPosListModel::GlobalLegendposModel);
	ui->legendPosComboBox->setEnabled(false);

	_mapProxy = new CartographyMapProxy(this, nullptr);

	_editor = new QQuickWidget(this);

	_loader = new CartographyBackgroundLoader(_mapProxy);
	_editor->engine()->addImageProvider(QString("provider"), _loader);
	_editor->rootContext()->setContextProperty("mapProxy", _mapProxy);

	if (!qmlTypeRegistrationDone) {
		qmlRegisterUncreatableType<Sabrina::CartographyItem>("SabrinaCartography", 0, 1, "SabrinaCartographyItem", "Cartography items cannot be instanciated from qml.");
		qmlTypeRegistrationDone = true;
	}

	_editor->setResizeMode(QQuickWidget::SizeViewToRootObject);
	_editor->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_editor->setSource(QUrl("qrc:/qml/qml/mapEditorWidget.qml"));

	ui->scrollArea->setWidget(_editor);

	QQuickItem* rootObj = _editor->rootObject();

	_mapAreaItem = rootObj->findChild<QQuickItem *>("mapArea");

	connect(ui->removeItemButton, &QPushButton::clicked,
			this, &CartographyEditor::onMapItemDeletionRequested);

	connect(ui->backgroundOpenButton, &QPushButton::clicked,
			this, &CartographyEditor::onBackgroundImageLoadingRequest);

	connect(ui->zoomPlusButton, &QPushButton::clicked,
			_mapProxy, &CartographyMapProxy::increaseScale);

	connect(ui->zoomMinusButton, &QPushButton::clicked,
			_mapProxy, &CartographyMapProxy::decreaseScale);

	connect(ui->zoomOneButton, &QPushButton::clicked,
			_mapProxy, &CartographyMapProxy::resetScale);

	ui->spinBoxScale->setValue(_mapProxy->getScalePercent());

	connect(ui->spinBoxScale, static_cast<void (QDoubleSpinBox::*)(qreal) >(&QDoubleSpinBox::valueChanged),
			_mapProxy, &CartographyMapProxy::setScalePercent);

	connect(_mapProxy, &CartographyMapProxy::scalePercentChanged,
			ui->spinBoxScale, &QDoubleSpinBox::setValue);

	connect(_mapProxy, &CartographyMapProxy::imageBackgroundChanged,
			this, &CartographyEditor::onNewBackgroundImageLoaded);

	connect(_mapProxy, &CartographyMapProxy::sizeChanged,
			this, &CartographyEditor::onMapSizeChanged);

	connect(_mapProxy, &CartographyMapProxy::clearItemSelection,
			this, &CartographyEditor::clearSelectedItem);

	connect(ui->heightSpinBox, static_cast<void (QDoubleSpinBox::*)(qreal) >(&QDoubleSpinBox::valueChanged),
			this, &CartographyEditor::heightSpinBoxesChange);

	connect(ui->widthSpinBox, static_cast<void (QDoubleSpinBox::*)(qreal) >(&QDoubleSpinBox::valueChanged),
			this, &CartographyEditor::widthSpinBoxesChange);

	//category

	connect(ui->categoryAddButton, &QPushButton::clicked,
			this, &CartographyEditor::onCartoCategoryAdditionRequest);

	connect(ui->categoryRemoveButton,&QPushButton::clicked,
			this, &CartographyEditor::onCartoCategoryRemovalRequest);

	connect(ui->selectColorButton, &QPushButton::clicked, this,
			&CartographyEditor::onColorSelectionClicked);

	connect(ui->categoryScaleSpinBox, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),
			this, &CartographyEditor::onSpinBoxScaleCategoryChange);

	connect(ui->categoryListView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &CartographyEditor::onCategorySelectionChange);

	//item

	connect(ui->spinBoxItemScale, static_cast<void (QDoubleSpinBox::*)(qreal) >(&QDoubleSpinBox::valueChanged),
			this, &CartographyEditor::onSpinBoxItemScaleChange);

	connect(ui->comboBoxSelectCategory, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
			this, &CartographyEditor::onSelectCategoryComboxIndexChange);

	connect(ui->legendPosComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
			this, &CartographyEditor::onSelectLegendPosComboxIndexChange);

}

CartographyEditor::~CartographyEditor()
{
	delete ui;
}

QString CartographyEditor::getTypeId() const {
	return CARTOGRAPHY_EDITOR_TYPE_ID;
}

QString CartographyEditor::getTypeName() const {
	return QString(tr("Éditeur de cartes"));
}

QStringList CartographyEditor::editableTypes() const {
	return { Cartography::CARTOGRAPHY_TYPE_ID };
}

bool CartographyEditor::effectivelySetEditedItem(Aline::EditableItem* item) {

	Cartography* carto = qobject_cast<Cartography*>(item);

	if (carto == nullptr) {
		return false;
	}

	if (_currentCartography != nullptr) {

		disconnect(_currentCartography, &Cartography::cartographyItemInserted,
				this, &CartographyEditor::connectItem);
		disconnect(_currentCartography, &Cartography::cartographyItemRemoved,
				this, &CartographyEditor::onCartographyItemRemoved);

		disconnect(ui->addItemButton, &QPushButton::clicked,
				_currentCartography, static_cast<void (Cartography::*)()>(&Cartography::addCartoPoint));

		disconnect(ui->nameLineEdit, &QLineEdit::textEdited,
				   _currentCartography, &Cartography::setObjectName);

	}

	if (_nameWatchConnection) {
		disconnect(_nameWatchConnection);
	}

	_currentCartography = carto;

	_mapProxy->setConnectedCartography(carto);

	if (carto != nullptr) {

		connect(carto, &Cartography::cartographyItemInserted,
				this, &CartographyEditor::connectItem);
		connect(carto, &Cartography::cartographyItemRemoved,
				this, &CartographyEditor::onCartographyItemRemoved);

		connect(ui->addItemButton, &QPushButton::clicked,
				carto, static_cast<void (Cartography::*)()>(&Cartography::addCartoPoint));

		connect(ui->nameLineEdit, &QLineEdit::textEdited,
				carto, &Cartography::setObjectName);

		_nameWatchConnection = connect(carto, &Cartography::objectNameChanged, [this] (QString const& newName) {
			if (newName != ui->nameLineEdit->text()) {
				ui->nameLineEdit->setText(newName);
			}
		});

	}

	_categoryListProxy->setSourceModel(carto->getCategoryListModel());

	return true;
}

void CartographyEditor::connectItem(CartographyItem* item) {

	if (_currentItems.contains(item->getRef())) {
		return;
	}

	if (item->getCartographyParent() != _currentCartography) {
		return;
	}

	CartographyItemProxy* proxy = new CartographyItemProxy(this, item);

	QQmlContext* rootContext = _editor->rootContext();

	QQmlContext* localContext = new QQmlContext(rootContext, proxy);
	localContext->setContextProperty("mapItem", proxy);

	QQmlComponent component(_editor->engine(), QUrl("qrc:/qml/qml/mapEditorPoint.qml"));
	QObject* obj = component.create(localContext);
	QQuickItem* itemPointItem = qobject_cast<QQuickItem*>(obj);

	itemPointItem->setParent(_mapAreaItem);
	itemPointItem->setVisible(true);
	itemPointItem->setParentItem(_mapAreaItem);

	connect(proxy, &QObject::destroyed,
			itemPointItem, &QObject::deleteLater); //delete item if proxy is deleted

	CartographyItemContext cont = {item, proxy, itemPointItem};

	connect(proxy, &CartographyItemProxy::refSwap,
			this, &CartographyEditor::mapItemRefChanged);

	connect(proxy, &CartographyItemProxy::receivedSelectionTrigger,
			this, &CartographyEditor::setSelectedItem);

	_currentItems.insert(item->getRef(), cont);

}

void CartographyEditor::onCartographyItemRemoved(QString oldRef) {

	if (_currentItems.contains(oldRef)) {
		_currentItems.remove(oldRef);
	}

}

void CartographyEditor::mapItemRefChanged(QString oldRef, QString newRef) {

	if (_currentItems.contains(oldRef)) {
		CartographyItemContext cont = _currentItems.value(oldRef);

		_currentItems.remove(oldRef);
		_currentItems.insert(newRef, cont);
	}

}

void CartographyEditor::onMapItemDeletionRequested() {

	if (_selectedItem != nullptr) {
		if (_currentCartography != nullptr) {
			CartographyItem* target = _selectedItem->_item;
			clearSelectedItem();

			_currentCartography->removeCartoItem(target);
		}
	}

}

void CartographyEditor::onBackgroundImageLoadingRequest() {

	if (_currentCartography == nullptr) {
		return;
	}

	QSettings settings;

	QString dir = QDir::homePath();
	if (settings.contains(IMAGE_OPEN_DIR_KEY)) {
		dir = settings.value(IMAGE_OPEN_DIR_KEY, dir).toString();

		QDir d(dir);

		if (!d.exists()) {
			dir = QDir::homePath();
		}
	}

	QString file = QFileDialog::getOpenFileName((this->parentWidget() != nullptr) ? this->parentWidget() : this,
												tr("Choisir une image de fond."),
												dir,
												tr("Images (*.jpg *.jpeg *.png *.tif);;jpeg (*.jpg *.jpeg);;png (*.png);;tiff (*.tif)" ));

	if (file == "") {
		return;
	}

	QFileInfo infos(file);

	if (infos.exists()) {

		QString selectedDir = infos.dir().absolutePath();
		settings.setValue(IMAGE_OPEN_DIR_KEY, selectedDir);

		ui->backgroundDisplayEdit->setText(file);
		_currentCartography->setBackground(file);
	}


}

void CartographyEditor::onNewBackgroundImageLoaded() {

	if (_currentCartography != nullptr) {

		QSize imSize;
		_loader->requestImage(_mapProxy->getImageBackground(), &imSize, imSize);

		_currentCartography->setSize(imSize);

	}

}

void CartographyEditor::widthSpinBoxesChange() {

	if (_currentCartography == nullptr) {
		return;
	}

	QSizeF oldSize = _currentCartography->getSize();

	QSizeF newSize = oldSize;

	newSize.setWidth(ui->widthSpinBox->value());

	if (ui->fixMapAspectRatioCheckBox->isChecked()) {

		double newHeight = ui->widthSpinBox->value()/oldSize.width() * oldSize.height();

		ui->heightSpinBox->blockSignals(true);
		ui->heightSpinBox->setValue(newHeight);
		ui->heightSpinBox->blockSignals(false);

		newSize.setHeight(newHeight);
	}

	_currentCartography->setSize(newSize);

}
void CartographyEditor::heightSpinBoxesChange() {

	if (_currentCartography == nullptr) {
		return;
	}

	QSizeF oldSize = _currentCartography->getSize();

	QSizeF newSize = oldSize;

	newSize.setHeight(ui->heightSpinBox->value());

	if (ui->fixMapAspectRatioCheckBox->isChecked()) {

		double newWidth = ui->heightSpinBox->value()/oldSize.height() * oldSize.width();

		ui->widthSpinBox->blockSignals(true);
		ui->widthSpinBox->setValue(newWidth);
		ui->widthSpinBox->blockSignals(false);

		newSize.setWidth(newWidth);
	}

	_currentCartography->setSize(newSize);

}

void CartographyEditor::onMapSizeChanged(QSizeF size) {

	if (_currentCartography == nullptr) {
		return;
	}

	QSizeF currentSize = _currentCartography->getSize();

	ui->heightSpinBox->blockSignals(true);
	ui->widthSpinBox->blockSignals(true);

	ui->heightSpinBox->setValue(currentSize.height());
	ui->widthSpinBox->setValue(currentSize.width());

	ui->heightSpinBox->blockSignals(false);
	ui->widthSpinBox->blockSignals(false);

}


void CartographyEditor::onCartoCategoryAdditionRequest() {

	if (_currentCartography != nullptr) {

		_currentCartography->addCategory(tr("Nouvelle catégorie"));

	}

}

void CartographyEditor::onCartoCategoryRemovalRequest() {

	CartographyCategory* cat = _currentCategory;

	_currentCartography->removeCartoCategory(cat);

}

void CartographyEditor::onCategorySelectionChange() {

	QModelIndex id = ui->categoryListView->selectionModel()->currentIndex();

	QString ref = _categoryListProxy->data(id, Aline::EditableItemManager::ItemRefRole).toString();

	CartographyCategory* cat = _currentCartography->getCategoryByRef(ref);

	if (cat != nullptr) {

		if (_currentCategory != nullptr) {

			disconnect(_currentCategory, &CartographyCategory::colorChanged,
					   this, &CartographyEditor::onCurrentCategoryColorChange);

			disconnect(_currentCategory, &CartographyCategory::scaleChanged,
					   this, &CartographyEditor::onCurrentCategoryScaleChange);

		}

		_currentCategory = cat;

		if (cat != nullptr) {

			ui->selectColorButton->setEnabled(true);
			ui->categoryScaleSpinBox->setEnabled(true);

			ui->selectColorButton->setStyleSheet(QString("background-color:%1;").arg(cat->getColor().name()));

			ui->categoryScaleSpinBox->blockSignals(true);
			ui->categoryScaleSpinBox->setValue(cat->getScale()*100);
			ui->categoryScaleSpinBox->blockSignals(false);

			connect(cat, &CartographyCategory::colorChanged,
					this, &CartographyEditor::onCurrentCategoryColorChange);

			connect(cat, &CartographyCategory::scaleChanged,
					this, &CartographyEditor::onCurrentCategoryScaleChange);

		} else {

			ui->selectColorButton->setEnabled(false);
			ui->categoryScaleSpinBox->setEnabled(false);

		}

	}

}


void CartographyEditor::onColorSelectionClicked() {

	if (_currentCategory != nullptr) {

		QColor newSelection = QColorDialog::getColor(_currentCategory->getColor(), this, tr("Choisir une couleur"));

		if (newSelection.isValid()) {
			_currentCategory->setColor(newSelection);
		}

	}

}

void CartographyEditor::onSpinBoxScaleCategoryChange(qreal scalePercent) {

	if (_currentCategory != nullptr) {
		_currentCategory->setScale(scalePercent/100);
	}

}

void CartographyEditor::onCurrentCategoryScaleChange(qreal scale) {

	ui->categoryScaleSpinBox->blockSignals(true);
	ui->categoryScaleSpinBox->setValue(scale*100);
	ui->categoryScaleSpinBox->blockSignals(false);

}

void CartographyEditor::onCurrentCategoryColorChange(QColor col) {

	ui->selectColorButton->setStyleSheet(QString("background-color:%1;").arg(col.name()));

}

void CartographyEditor::clearSelectedItem() {
	setSelectedItem(QString());
}

void CartographyEditor::setSelectedItem(QString ref) {

	if (_selectedItem != nullptr) {
		_selectedItem->setFocus(false);

		disconnect(_selectedItem->_item, &CartographyItem::scaleChanged,
				   this, &CartographyEditor::onSelectedItemScaleChange);

		disconnect(_selectedItem->_item, &CartographyItem::categoryChanged,
				   this, &CartographyEditor::onSelectedItemCategoryChange);

		disconnect(_selectedItem, &CartographyItemProxy::legendPositionChanged,
				   this, &CartographyEditor::onSelectedItemLegendPosChange);

	}

	_selectedItem = nullptr;

	if (ref != QString() && _currentItems.contains(ref)) {
		CartographyItemContext cont = _currentItems.value(ref);

		_selectedItem = cont._proxy;
		_selectedItem->setFocus(true);

		connect(_selectedItem->_item, &CartographyItem::scaleChanged,
				this, &CartographyEditor::onSelectedItemScaleChange);

		connect(_selectedItem->_item, &CartographyItem::categoryChanged,
				this, &CartographyEditor::onSelectedItemCategoryChange);

		connect(_selectedItem, &CartographyItemProxy::legendPositionChanged,
				this, &CartographyEditor::onSelectedItemLegendPosChange);

		QString catRef = _selectedItem->_item->getCategoryRef();
		int row = categoryRow(catRef);

		ui->comboBoxSelectCategory->setEnabled(true);
		ui->legendPosComboBox->setEnabled(true);

		ui->legendPosComboBox->blockSignals(true);
		ui->legendPosComboBox->setCurrentIndex(_selectedItem->_item->getLegendPosition());
		ui->legendPosComboBox->blockSignals(false);

		ui->comboBoxSelectCategory->blockSignals(true);
		ui->comboBoxSelectCategory->setCurrentIndex(row);
		ui->comboBoxSelectCategory->blockSignals(false);

		ui->spinBoxItemScale->setEnabled(true);

		ui->spinBoxItemScale->blockSignals(true);
		ui->spinBoxItemScale->setValue(_selectedItem->getScale()*100);
		ui->spinBoxItemScale->blockSignals(false);

	} else {

		ui->comboBoxSelectCategory->setEnabled(false);
		ui->spinBoxItemScale->setEnabled(false);
		ui->legendPosComboBox->setEnabled(false);

	}

}

int CartographyEditor::categoryRow(QString categoryRef) {

	int ret = -1;

	for (int r = 0; r < _categoryListProxy->rowCount(); r++) {
		if (_categoryListProxy->data(_categoryListProxy->index(r, 0), Aline::EditableItemManager::ItemRefRole).toString() == categoryRef) {
			ret = r;
			break;
		}
	}

	return ret;
}

void CartographyEditor::onSelectCategoryComboxIndexChange(int index) {

	if (_selectedItem != nullptr && _currentCartography != nullptr) {

		QString ref = _categoryListProxy->data(_categoryListProxy->index(index, 0), Aline::EditableItemManager::ItemRefRole).toString();

		_selectedItem->_item->setCategory(_currentCartography->getCategoryByRef(ref, true));

	}

}

void CartographyEditor::onSpinBoxItemScaleChange(double scalePercent) {

	if (_selectedItem != nullptr) {
		_selectedItem->setScale(scalePercent/100);
	}
}

void CartographyEditor::onSelectLegendPosComboxIndexChange(int index) {

	if (_selectedItem != nullptr) {
		_selectedItem->setLegendPosition(index);
	}

}

void CartographyEditor::onSelectedItemCategoryChange(QString ref) {

	int row = categoryRow(ref);

	if (row >= 0) {

		ui->comboBoxSelectCategory->blockSignals(true);
		ui->comboBoxSelectCategory->setCurrentIndex(row);
		ui->comboBoxSelectCategory->blockSignals(false);

	}

}

void CartographyEditor::onSelectedItemScaleChange(qreal scale) {

	ui->spinBoxItemScale->blockSignals(true);
	ui->spinBoxItemScale->setValue(scale*100);
	ui->spinBoxItemScale->blockSignals(false);

}

void CartographyEditor::onSelectedItemLegendPosChange(int pos) {

	ui->legendPosComboBox->blockSignals(true);
	ui->legendPosComboBox->setCurrentIndex(pos);
	ui->legendPosComboBox->blockSignals(false);

}

CartographyEditor::CartographyEditorFactory::CartographyEditorFactory(QObject* parent) :
	Aline::EditorFactory(parent)
{

}

Aline::Editor* CartographyEditor::CartographyEditorFactory::createItem(QWidget* parent) const {
	return new CartographyEditor(parent);
}

CartographyItemProxy::CartographyItemProxy(CartographyEditor* parent, CartographyItem* item) :
	QObject(parent),
	_editorParent(parent),
	_item(item),
	_hasFocus(false)
{
	connect(_item, &QObject::destroyed, this, &QObject::deleteLater);

	connect(_item, &QObject::objectNameChanged,
			this, &CartographyItemProxy::itemNameChanged);

	connect(_item, &CartographyItem::positionChanged,
			this, &CartographyItemProxy::positionChanged);

	connect(_item, &CartographyItem::colorChanged,
			this, &CartographyItemProxy::colorChanged);

	connect(_item, &CartographyItem::scaleChanged, this, &CartographyItemProxy::scaleChanged);

	connect(_item, &CartographyItem::linkedStatusChanged,
			this, &CartographyItemProxy::linkedStatusChanged);

	connect(_item, &CartographyItem::refSwap,
			this, &CartographyItemProxy::refSwap);

	connect(_item, &CartographyItem::categoryChanged,
			this, &CartographyItemProxy::onCategoryChanged);

	connect(_item, &CartographyItem::legendPositionChanged,
			this, &CartographyItemProxy::legendPositionChanged);
}

QString CartographyItemProxy::itemName() const {
	return _item->objectName();
}
void CartographyItemProxy::setItemName(QString const& name) {
	_item->setObjectName(name);
}

QPointF CartographyItemProxy::getPosition() const {
	return _item->getPosition();
}
void CartographyItemProxy::setPosition(QPointF const& pos) {
	_item->setPosition(pos);
}

QColor CartographyItemProxy::getPointColor() const {
	return _item->getPointColor();
}

qreal CartographyItemProxy::getScale() const {
	return _item->getScale();
}
void CartographyItemProxy::setScale(qreal scale) const {
	_item->setScale(scale);
}

int CartographyItemProxy::getLegendPosition() const {
	return _item->getLegendPosition();
}

void CartographyItemProxy::setLegendPosition(int pos) {
	_item->setLegendPosition((CartographyItem::LegendPos) pos);
}

bool CartographyItemProxy::hasFocus() const {
	return _hasFocus;
}

bool CartographyItemProxy::isLinked() const {
	return _item->isLinked();
}

void CartographyItemProxy::setAsCurrentItem() {
	emit receivedSelectionTrigger(_item->getRef());
}

void CartographyItemProxy::setFocus(bool focus) {

	if (focus != _hasFocus) {
		_hasFocus = focus;
		emit focusChanged(focus);
	}

}

void CartographyItemProxy::onCategoryChanged() {

	emit colorChanged(getPointColor());

}

CartographyMapProxy::CartographyMapProxy(CartographyEditor* parent, Cartography* carto) :
	QObject(parent),
	_connectedCartography(nullptr),
	_scale(1.0)
{
	setConnectedCartography(carto);
}

QSizeF CartographyMapProxy::getSize() const {

	if (_connectedCartography != nullptr) {
		return _connectedCartography->getSize();
	}

	return QSize();
}

void CartographyMapProxy::setSize(QSizeF size) {

	if (_connectedCartography != nullptr) {
		return _connectedCartography->setSize(size);
	}
}

qreal CartographyMapProxy::getScale() const {
	return _scale;
}
void CartographyMapProxy::setScale(qreal scale) {

	if (fabs(_scale - scale) > 1e-4) {

		_scale = scale;
		emit scaleChanged(_scale);
		emit scalePercentChanged(_scale*100);

	}
}

qreal CartographyMapProxy::getScalePercent() const {
	return _scale*100;
}
void CartographyMapProxy::setScalePercent(qreal scale) {
	setScale(scale/100);
}

void CartographyMapProxy::increaseScale() {
	setScale(getScale()/0.9);
}
void CartographyMapProxy::decreaseScale() {
	setScale(getScale()*0.9);
}
void CartographyMapProxy::resetScale() {
	setScale(1.0);
}

QString CartographyMapProxy::getImageBackground() const {
	if (_connectedCartography != nullptr) {
		QString fullPath = _connectedCartography->background();
		QFileInfo infos(fullPath);

		return infos.fileName();
	}

	return "";
}
QString CartographyMapProxy::getImageBackgroundFile() const {
	if (_connectedCartography != nullptr) {
		return _connectedCartography->background();
	}
}

Cartography *CartographyMapProxy::getConnectedCartography() const
{
	return _connectedCartography;
}

void CartographyMapProxy::setConnectedCartography(Cartography *cartography)
{
	if (_connectedCartography != cartography) {

		if (_connectedCartography != nullptr) {
			disconnect(_connectedCartography, &Cartography::sizeChanged,
					   this, &CartographyMapProxy::sizeChanged);

			disconnect(_connectedCartography, &Cartography::backgroundChanged,
					   this, &CartographyMapProxy::onBackgroundChanged);
		}

		_connectedCartography = cartography;

		if (cartography != nullptr) {
			connect(cartography, &Cartography::sizeChanged,
					this, &CartographyMapProxy::sizeChanged);

			connect(cartography, &Cartography::backgroundChanged,
					this, &CartographyMapProxy::onBackgroundChanged);
		}

		emit sizeChanged(getSize());
		emit imageBackgroundChanged(getImageBackground());

	}
}

void CartographyMapProxy::insertEditableItem(QString ref) {

	if (_connectedCartography != nullptr) {
		_connectedCartography->addCartoItem(ref);
	}

}

void CartographyMapProxy::onBackgroundChanged() {
	emit imageBackgroundChanged(getImageBackground());
}

CartographyBackgroundLoader::CartographyBackgroundLoader(CartographyMapProxy* proxy) :
	QQuickImageProvider(QQmlImageProviderBase::Image),
	_proxy(proxy)
{

}

QImage CartographyBackgroundLoader::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {

	Q_UNUSED(requestedSize);

	if (id == "") {

		QImage img;

		*size = img.size();
		return img;

	}

	if (id == _loadedFileName) {
		*size = _loadedImage.size();
		return _loadedImage;
	}

	if (id == _proxy->getImageBackground()) {
		_loadedImage.load(_proxy->getImageBackgroundFile()); //TODO: try to be more clever to also load .ora and .kra files.

		*size = _loadedImage.size();
		_loadedFileName = id;

		return _loadedImage;
	}

	QImage img;

	*size = img.size();
	return img;

}

QQmlImageProviderBase::ImageType CartographyBackgroundLoader::imageType() const {
	return QQmlImageProviderBase::Image;
}

} // namespace Sabrina
