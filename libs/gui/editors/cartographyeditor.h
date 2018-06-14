#ifndef SABRINA_CARTOGRAPHYEDITOR_H
#define SABRINA_CARTOGRAPHYEDITOR_H

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

#include "gui/gui_global.h"

#include <aline/src/view/editableitemeditor.h>
#include <aline/src/view/editorfactory.h>

#include <QPointF>
#include <QColor>

#include <QQuickImageProvider>

#include "model/editableItems/cartography.h"

class QQuickItem;
class QQuickWidget;
class QSortFilterProxyModel;

namespace Sabrina {

class Cartography;
class CartographyItem;
class CartographyCategory;
class CartographyEditor;

namespace Ui {
class CartographyEditor;
}

class CartographyItemProxy : public QObject {

	Q_OBJECT

	friend class CartographyEditor;

public:

	explicit CartographyItemProxy(CartographyEditor* parent, CartographyItem* item);

	Q_PROPERTY(QString itemName READ itemName WRITE setItemName NOTIFY itemNameChanged)
	Q_PROPERTY(QPointF position READ getPosition WRITE setPosition NOTIFY positionChanged)
	Q_PROPERTY(qreal scale READ getScale WRITE setScale NOTIFY scaleChanged)

	Q_PROPERTY(QColor color READ getPointColor NOTIFY colorChanged)
	Q_PROPERTY(qreal radius READ getRadius NOTIFY radiusChanged)

	Q_PROPERTY(QColor borderColor READ getBorderColor NOTIFY borderColorChanged)
	Q_PROPERTY(qreal border READ getBorder NOTIFY borderChanged)

	Q_PROPERTY(QString legendFont READ getLegendFont NOTIFY legendFontChanged)
	Q_PROPERTY(bool legendUnderlined READ getLegendUnderlined NOTIFY legendUnderlinedChanged)
	Q_PROPERTY(bool legendBold READ getLegendBold NOTIFY legendBoldChanged)
	Q_PROPERTY(bool legendItalic READ getLegendItalic NOTIFY legendItalicChanged)
	Q_PROPERTY(int legendSize READ getLegendSize NOTIFY legendSizeChanged)
	Q_PROPERTY(QColor legendColor READ getLegendColor NOTIFY legendColorChanged)

	Q_PROPERTY(bool hasFocus READ hasFocus NOTIFY focusChanged)
	Q_PROPERTY(bool isLinked READ isLinked NOTIFY linkedStatusChanged)
	Q_PROPERTY(int legendPosition READ getLegendPosition WRITE setLegendPosition NOTIFY legendPositionChanged)

	QString itemName() const;
	void setItemName(QString const& name);

	QPointF getPosition() const;
	void setPosition(QPointF const& pos);

	qreal getScale() const;
	void setScale(qreal scale) const;

	QColor getPointColor() const;
	qreal getRadius() const;

	QColor getBorderColor() const;
	qreal getBorder() const;

	QString getLegendFont() const;
	bool getLegendUnderlined() const;
	bool getLegendBold() const;
	bool getLegendItalic() const;
	int getLegendSize() const;
	QColor getLegendColor() const;

	int getLegendPosition() const;
	void setLegendPosition(int pos);

	bool hasFocus() const;

	bool isLinked() const;

signals:

	void itemNameChanged(QString name);
	void positionChanged(QPointF pos);
	void scaleChanged(qreal scale);

	void colorChanged(QColor col);
	void radiusChanged(qreal radius);

	void borderColorChanged(QColor col);
	void borderChanged(qreal border);

	void legendFontChanged(QString name);
	void legendUnderlinedChanged(bool underlined);
	void legendBoldChanged(bool bold);
	void legendItalicChanged(bool italic);
	void legendSizeChanged(int size);
	void legendColorChanged(QColor col);

	void focusChanged(bool focus);
	void linkedStatusChanged(bool link);

	void refSwap(QString oldRef, QString newRef);
	void receivedSelectionTrigger(QString ref);

	void legendPositionChanged(int pos);

public slots:

	void setAsCurrentItem();

private:

	void setFocus(bool focus);
	void onCategoryChanged();

	CartographyEditor* _editorParent;
	CartographyItem* _item;

	bool _hasFocus;

};

class CartographyMapProxy : public QObject
{
	Q_OBJECT

public:

	explicit CartographyMapProxy(CartographyEditor* parent, Cartography* carto);

	Q_PROPERTY(QSizeF size READ getSize WRITE setSize NOTIFY sizeChanged)
	Q_PROPERTY(qreal scale READ getScale WRITE setScale NOTIFY scaleChanged)
	Q_PROPERTY(QString imageBackground READ getImageBackground NOTIFY imageBackgroundChanged)

	QSizeF getSize() const;
	void setSize(QSizeF size);

	qreal getScale() const;
	void setScale(qreal scale);

	qreal getScalePercent() const;
	void setScalePercent(qreal scale);

	void increaseScale();
	void decreaseScale();
	void resetScale();

	QString getImageBackground() const;
	QString getImageBackgroundFile() const;

	Cartography *getConnectedCartography() const;
	void setConnectedCartography(Cartography *cartography);

signals:

	void sizeChanged(QSizeF size);
	void scaleChanged(qreal scale);
	void scalePercentChanged(qreal scale);

	void imageBackgroundChanged(QString img);

	void clearItemSelection();

public slots:

	void insertEditableItem(QByteArray refDatas);

protected:

	void onBackgroundChanged();

	Cartography* _connectedCartography;

	qreal _scale;

};

class CartographyBackgroundLoader: public QQuickImageProvider {

public:

	explicit CartographyBackgroundLoader(CartographyMapProxy* proxy);

	virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

	virtual QQmlImageProviderBase::ImageType imageType() const;

protected:

	QString _loadedFileName;
	QImage _loadedImage;

	CartographyMapProxy* _proxy;

};

class CATHIA_GUI_EXPORT CartographyEditor : public Aline::EditableItemEditor
{
	Q_OBJECT

public:
	static const QString CARTOGRAPHY_EDITOR_TYPE_ID;

	class CATHIA_GUI_EXPORT CartographyEditorFactory : public Aline::EditorFactory
	{
		public :
			explicit CartographyEditorFactory(QObject* parent = nullptr);
			virtual Aline::Editor* createItem(QWidget* parent) const;
	};

	explicit CartographyEditor(QWidget *parent = 0);
	~CartographyEditor();

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual QStringList editableTypes() const;

protected:

	virtual bool effectivelySetEditedItem(Aline::EditableItem* item);

	void connectItem(CartographyItem* item);
	void onCartographyItemRemoved(QString oldRef);

	void mapItemRefChanged(QString oldRef, QString newRef);

	void onMapItemDeletionRequested();

	void onBackgroundImageLoadingRequest();
	void onNewBackgroundImageLoaded();

	void widthSpinBoxesChange();
	void heightSpinBoxesChange();
	void onMapSizeChanged(QSizeF size);

	//categories

	void onCartoCategoryAdditionRequest();
	void onCartoCategoryRemovalRequest();

	void onCategorySelectionChange();

	void onColorSelectionClicked();
	void onSpinBoxRadiusCategoryChange(qreal radius);
	void onBorderColorSelectionClicked();
	void onLegendFontSelectionChanged();
	void onLegendColorSelectionClicked();

	void onCurrentCategoryRadiusChange(qreal radius);
	void onCurrentCategoryColorChange(QColor col);
	void onCurrentCategoryBorderColorChange(QColor col);
	void onCurrentCategoryFontChanges(QString fontName);
	void onCurrentCategoryLegendColorChange(QColor col);

	void setCategoryEditWidgetsEnabled(bool enabled);

	//items
	void clearSelectedItem();
	void setSelectedItem(QString ref);
	int categoryRow(QString categoryRef);

	void onSelectCategoryComboxIndexChange(int index);
	void onSpinBoxItemScaleChange(double scalePercent);
	void onSelectLegendPosComboxIndexChange(int index);

	void onSelectedItemCategoryChange(QString ref);
	void onSelectedItemScaleChange(qreal scale);
	void onSelectedItemLegendPosChange(int pos);

private:

	static bool qmlTypeRegistrationDone;

	struct CartographyItemContext {
		CartographyItem* _item;
		CartographyItemProxy* _proxy;
		QQuickItem* _associatedItem;
	};

	Ui::CartographyEditor *ui;

	Cartography* _currentCartography;

	CartographyBackgroundLoader* _loader;
	CartographyItemProxy* _selectedItem;
	CartographyMapProxy* _mapProxy;
	CartographyCategory* _currentCategory;

	QMap<QString, CartographyItemContext> _currentItems;

	QQuickItem *_mapAreaItem;

	QSortFilterProxyModel* _categoryListProxy;

	QMetaObject::Connection _nameWatchConnection;

	QQuickWidget* _editor;

	bool _resizeMapOnNewBackground;
};


} // namespace Sabrina
#endif // SABRINA_CARTOGRAPHYEDITOR_H
