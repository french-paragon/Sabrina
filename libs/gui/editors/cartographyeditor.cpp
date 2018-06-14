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
#include <QFont>

#include <cmath>

namespace Sabrina {

bool CartographyEditor::qmlTypeRegistrationDone = false;
const QString CartographyEditor::CARTOGRAPHY_EDITOR_TYPE_ID = "sabrina_jdr_cartography_editor";

CartographyEditor::CartographyEditor(QWidget *parent) :
	Aline::EditableItemEditor(parent),
	ui(new Ui::CartographyEditor),
	_currentCartography(nullptr),
	_selectedItem(nullptr),
	_currentCategory(nullptr),
	_resizeMapOnNewBackground(true)
{
	ui->setupUi(this);

	ui->splitter->setSizes({this->width()*2/3,this->width()/3});
	ui->backgroundDisplayEdit->setReadOnly(true);

	ui->spinBoxItemScale->setEnabled(false);
	ui->comboBoxSelectCategory->setEnabled(false);

	setCategoryEditWidgetsEnabled(false);

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

	connect(ui->categoryRadiusSpinBox, static_cast<void(QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged),
			this, &CartographyEditor::onSpinBoxRadiusCategoryChange);

	connect(ui->categoryListView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &CartographyEditor::onCategorySelectionChange);

	connect(ui->selectBorderColorPushButton, &QPushButton::clicked,
			this, &CartographyEditor::onBorderColorSelectionClicked);

	connect(ui->legendFontComboBox, &QFontComboBox::currentFontChanged,
			this, &CartographyEditor::onLegendFontSelectionChanged);

	connect(ui->legendColorPushButton, &QPushButton::clicked,
			this, &CartographyEditor::onLegendColorSelectionClicked);

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

	_resizeMapOnNewBackground = false;
	_mapProxy->setConnectedCartography(carto);
	_resizeMapOnNewBackground = true;

	for (CartographyItem* it : carto->getItems()) {
		connectItem(it);
	}

	if (carto != nullptr) {

		connect(carto, &Cartography::cartographyItemInserted,
				this, &CartographyEditor::connectItem);
		connect(carto, &Cartography::cartographyItemRemoved,
				this, &CartographyEditor::onCartographyItemRemoved);

		connect(ui->addItemButton, &QPushButton::clicked,
				carto, static_cast<void (Cartography::*)()>(&Cartography::addCartoPoint));

		ui->nameLineEdit->setText(carto->objectName());
		ui->backgroundDisplayEdit->setText(carto->background());

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

		if (_resizeMapOnNewBackground) {

			QSize imSize;
			_loader->requestImage(_mapProxy->getImageBackground(), &imSize, imSize);

			_currentCartography->setSize(imSize);

		}

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

			disconnect(_currentCategory, &CartographyCategory::radiusChanged,
					   this, &CartographyEditor::onCurrentCategoryRadiusChange);

			disconnect(_currentCategory, &CartographyCategory::borderColorChanged,
					   this, &CartographyEditor::onCurrentCategoryBorderColorChange);

			disconnect(_currentCategory, &CartographyCategory::borderChanged,
					   ui->categoryBorderSpinBox, &QDoubleSpinBox::setValue);
			disconnect(ui->categoryBorderSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					   _currentCategory, &CartographyCategory::setBorder);


			disconnect(_currentCategory, &CartographyCategory::legendFontChanged,
					   this, &CartographyEditor::onCurrentCategoryFontChanges);

			disconnect(_currentCategory, &CartographyCategory::legendSizeChanged,
					   ui->legendSizeSinBox, &QSpinBox::setValue);
			disconnect(ui->legendSizeSinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					   _currentCategory, &CartographyCategory::setLegendSize);

			disconnect(_currentCategory, &CartographyCategory::legendBoldChanged,
					   ui->legendBoldButton, &QPushButton::setChecked);
			disconnect(ui->legendBoldButton, &QPushButton::clicked,
					   _currentCategory, &CartographyCategory::setLegendBold);

			disconnect(_currentCategory, &CartographyCategory::legendItalicChanged,
					   ui->legendItalicButton, &QPushButton::setChecked);
			disconnect(ui->legendItalicButton, &QPushButton::clicked,
					   _currentCategory, &CartographyCategory::setLegendItalic);

			disconnect(_currentCategory, &CartographyCategory::legendUnderlinedChanged,
					   ui->legendUnderlinedButton, &QPushButton::setChecked);
			disconnect(ui->legendUnderlinedButton, &QPushButton::clicked,
					   _currentCategory, &CartographyCategory::setLegendUnderlined);

			disconnect(_currentCategory, &CartographyCategory::legendColorChanged,
					   this, &CartographyEditor::onCurrentCategoryLegendColorChange);

		}

		_currentCategory = cat;

		if (cat != nullptr) {

			setCategoryEditWidgetsEnabled(true);

			ui->selectColorButton->setStyleSheet(QString("border: none;\nbackground-color:%1;").arg(cat->getColor().name()));

			ui->categoryRadiusSpinBox->blockSignals(true);
			ui->categoryRadiusSpinBox->setValue(cat->getRadius());
			ui->categoryRadiusSpinBox->blockSignals(false);

			ui->selectBorderColorPushButton->setStyleSheet(QString("border: none;\nbackground-color:%1;").arg(cat->getBorderColor().name()));

			ui->categoryBorderSpinBox->blockSignals(true);
			ui->categoryBorderSpinBox->setValue(cat->getBorder());
			ui->categoryBorderSpinBox->blockSignals(false);


			ui->legendFontComboBox->blockSignals(true);
			ui->legendFontComboBox->setFont(QFont(cat->getLegendFont()));
			ui->legendFontComboBox->blockSignals(false);

			ui->legendSizeSinBox->blockSignals(true);
			ui->legendSizeSinBox->setValue(cat->getLegendSize());
			ui->legendSizeSinBox->blockSignals(false);

			ui->legendBoldButton->blockSignals(true);
			ui->legendBoldButton->setChecked(cat->getLegendBold());
			ui->legendBoldButton->blockSignals(false);

			ui->legendItalicButton->blockSignals(true);
			ui->legendItalicButton->setChecked(cat->getLegendItalic());
			ui->legendItalicButton->blockSignals(false);

			ui->legendUnderlinedButton->blockSignals(true);
			ui->legendUnderlinedButton->setChecked(cat->getLegendUnderlined());
			ui->legendUnderlinedButton->blockSignals(false);

			ui->legendColorPushButton->setStyleSheet(QString("border: none;\nbackground-color:%1;").arg(cat->getLegendColor().name()));


			connect(cat, &CartographyCategory::colorChanged,
					this, &CartographyEditor::onCurrentCategoryColorChange);

			connect(cat, &CartographyCategory::radiusChanged,
					this, &CartographyEditor::onCurrentCategoryRadiusChange);

			connect(cat, &CartographyCategory::borderColorChanged, this,
					&CartographyEditor::onCurrentCategoryBorderColorChange);

			connect(cat, &CartographyCategory::borderChanged,
					ui->categoryBorderSpinBox, &QDoubleSpinBox::setValue);
			connect(ui->categoryBorderSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					cat, &CartographyCategory::setBorder);

			connect(cat, &CartographyCategory::legendFontChanged,
					this, &CartographyEditor::onCurrentCategoryFontChanges);

			connect(cat, &CartographyCategory::legendSizeChanged,
					ui->legendSizeSinBox, &QSpinBox::setValue);
			connect(ui->legendSizeSinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					cat, &CartographyCategory::setLegendSize);

			connect(cat, &CartographyCategory::legendBoldChanged,
					ui->legendBoldButton, &QPushButton::setChecked);
			connect(ui->legendBoldButton, &QPushButton::clicked,
					cat, &CartographyCategory::setLegendBold);

			connect(cat, &CartographyCategory::legendItalicChanged,
					ui->legendItalicButton, &QPushButton::setChecked);
			connect(ui->legendItalicButton, &QPushButton::clicked,
					cat, &CartographyCategory::setLegendItalic);

			connect(cat, &CartographyCategory::legendUnderlinedChanged,
					ui->legendUnderlinedButton, &QPushButton::setChecked);
			connect(ui->legendUnderlinedButton, &QPushButton::clicked,
					cat, &CartographyCategory::setLegendUnderlined);

			connect(cat, &CartographyCategory::legendColorChanged,
					this, &CartographyEditor::onCurrentCategoryLegendColorChange);

		} else {

			setCategoryEditWidgetsEnabled(false);

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

void CartographyEditor::onSpinBoxRadiusCategoryChange(qreal radius) {

	if (_currentCategory != nullptr) {
		_currentCategory->setRadius(radius);
	}

}

void CartographyEditor::onBorderColorSelectionClicked() {

	if (_currentCategory != nullptr) {

		QColor newSelection = QColorDialog::getColor(_currentCategory->getBorderColor(), this, tr("Choisir une couleur"));

		if (newSelection.isValid()) {
			_currentCategory->setBorderColor(newSelection);
		}

	}

}

void CartographyEditor::onLegendFontSelectionChanged() {

	if (_currentCartography != nullptr) {

		QString fontName = ui->legendFontComboBox->currentFont().family();

		_currentCategory->setLegendFont(fontName);

	}

}

void CartographyEditor::onLegendColorSelectionClicked() {


	if (_currentCategory != nullptr) {

		QColor newSelection = QColorDialog::getColor(_currentCategory->getLegendColor(), this, tr("Choisir une couleur"));

		if (newSelection.isValid()) {
			_currentCategory->setLegendColor(newSelection);
		}
	}

}

void CartographyEditor::onCurrentCategoryRadiusChange(qreal radius) {

	ui->categoryRadiusSpinBox->blockSignals(true);
	ui->categoryRadiusSpinBox->setValue(radius);
	ui->categoryRadiusSpinBox->blockSignals(false);

}

void CartographyEditor::onCurrentCategoryColorChange(QColor col) {

	ui->selectColorButton->setStyleSheet(QString("border: none;\nbackground-color:%1;").arg(col.name()));

}

void CartographyEditor::onCurrentCategoryBorderColorChange(QColor col) {

	ui->selectBorderColorPushButton->setStyleSheet(QString("border: none;\nbackground-color:%1;").arg(col.name()));

}

void CartographyEditor::onCurrentCategoryFontChanges(QString fontName) {

	ui->legendFontComboBox->setFont(QFont(fontName));

}

void CartographyEditor::onCurrentCategoryLegendColorChange(QColor col) {

	ui->legendColorPushButton->setStyleSheet(QString("border: none;\nbackground-color:%1;").arg(col.name()));

}

void CartographyEditor::setCategoryEditWidgetsEnabled(bool enabled) {

	ui->selectColorButton->setEnabled(enabled);
	ui->categoryRadiusSpinBox->setEnabled(enabled);

	ui->selectBorderColorPushButton->setEnabled(enabled);
	ui->categoryBorderSpinBox->setEnabled(enabled);

	ui->groupBoxPoint->setEnabled(enabled);


	ui->legendFontComboBox->setEnabled(enabled);

	ui->legendSizeSinBox->setEnabled(enabled);

	ui->legendBoldButton->setEnabled(enabled);
	ui->legendItalicButton->setEnabled(enabled);
	ui->legendUnderlinedButton->setEnabled(enabled);

	ui->legendColorPushButton->setEnabled(enabled);

	ui->groupBoxLegend->setEnabled(enabled);

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

	connect(_item, &CartographyItem::scaleChanged,
			this, &CartographyItemProxy::scaleChanged);

	connect(_item, &CartographyItem::colorChanged,
			this, &CartographyItemProxy::colorChanged);

	connect(_item, &CartographyItem::radiusChanged,
			this, &CartographyItemProxy::radiusChanged);

	connect(_item, &CartographyItem::borderColorChanged,
			this, &CartographyItemProxy::borderColorChanged);

	connect(_item, &CartographyItem::borderChanged,
			this, &CartographyItemProxy::borderChanged);

	connect(_item, &CartographyItem::legendFontChanged,
			this, &CartographyItemProxy::legendFontChanged);

	connect(_item, &CartographyItem::legendSizeChanged,
			this, &CartographyItemProxy::legendSizeChanged);

	connect(_item, &CartographyItem::legendBoldChanged,
			this, &CartographyItemProxy::legendBoldChanged);

	connect(_item, &CartographyItem::legendItalicChanged,
			this, &CartographyItemProxy::legendItalicChanged);

	connect(_item, &CartographyItem::legendUnderlinedChanged,
			this, &CartographyItemProxy::legendUnderlinedChanged);

	connect(_item, &CartographyItem::legendColorChanged,
			this, &CartographyItemProxy::legendColorChanged);

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

qreal CartographyItemProxy::getScale() const {
	return _item->getScale();
}
void CartographyItemProxy::setScale(qreal scale) const {
	_item->setScale(scale);
}

QColor CartographyItemProxy::getPointColor() const {
	return _item->getPointColor();
}
qreal CartographyItemProxy::getRadius() const {
	return _item->getRadius();
}

QColor CartographyItemProxy::getBorderColor() const {
	return _item->getBorderColor();
}
qreal CartographyItemProxy::getBorder() const {
	return _item->getBorder();
}

QString CartographyItemProxy::getLegendFont() const {
	return _item->getLegendFont();
}
bool CartographyItemProxy::getLegendUnderlined() const {
	return _item->getLegendUnderlined();
}
bool CartographyItemProxy::getLegendBold() const {
	return _item->getLegendBold();
}
bool CartographyItemProxy::getLegendItalic() const {
	return _item->getLegendItalic();
}
int CartographyItemProxy::getLegendSize() const {
	return _item->getLegendSize();
}
QColor CartographyItemProxy::getLegendColor() const {
	return _item->getLegendColor();
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
	emit radiusChanged(getRadius());

	emit borderColorChanged(getBorderColor());
	emit borderChanged(getBorder());

	emit legendFontChanged(getLegendFont());
	emit legendSizeChanged(getLegendSize());
	emit legendBoldChanged(getLegendBold());
	emit legendItalicChanged(getLegendItalic());
	emit legendUnderlinedChanged(getLegendUnderlined());
	emit legendColorChanged(getLegendColor());

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

void CartographyMapProxy::insertEditableItem(QByteArray refDatas) {

	if (_connectedCartography != nullptr) {

		QDataStream stream(&refDatas, QIODevice::ReadOnly);
		QStringList newItems;
		int rows = 0;

		while (!stream.atEnd()) {
			QString text;
			stream >> text;
			newItems << text;
			++rows;
		}


		for (QString ref : newItems) {

			_connectedCartography->addCartoItem(ref);

		}
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
