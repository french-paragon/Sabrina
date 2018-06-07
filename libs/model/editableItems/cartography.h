#ifndef CARTOGRAPHY_H
#define CARTOGRAPHY_H

#include "model/editableitem.h"
#include "aline/src/model/editableitemfactory.h"
#include "aline/src/model/editableitem.h"

#include "model/model_global.h"

#include <QColor>
#include <QPointF>
#include <QSize>

namespace Sabrina {

class Cartography;
class CartographyCategory;

class CATHIA_MODEL_EXPORT CartographyItem : public Aline::EditableItem
{
	Q_OBJECT

	friend class Cartography;

public:

	static const QString CARTOGRAPHY_ITEM_TYPE_ID;

	class CATHIA_MODEL_EXPORT CartographyItemFactory : public Aline::EditableSubItemFactory
	{
	public:
		explicit CartographyItemFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItem* parent) const;

	};

	explicit CartographyItem(QString ref, Cartography* parent);

	Q_PROPERTY(QString referedItem READ getReferedItem WRITE setReferedItem NOTIFY referedItemChanged)
	Q_PROPERTY(QPointF position READ getPosition WRITE setPosition NOTIFY positionChanged)
	Q_PROPERTY(QString category READ getCategoryRef WRITE setCategory)
	Q_PROPERTY(QColor color READ getPointColor NOTIFY colorChanged STORED false)
	Q_PROPERTY(qreal scale READ getScale WRITE setScale NOTIFY scaleChanged )

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual QString iconInternalUrl() const;

	void setCategory(CartographyCategory* category);
	void setCategory(QString ref);

	QString getReferedItem() const;
	void setReferedItem(const QString &referedItem, bool rename = true);

	QPointF getPosition() const;
	void setPosition(const QPointF &position);

	QString getCategoryRef() const;

	QColor getPointColor() const;

	qreal getScale() const;
	void setScale(qreal scale);

	bool isLinked() const;

	Cartography *getCartographyParent() const;

signals:

	void referedItemChanged(QString ref);
	void positionChanged(QPointF position);
	void colorChanged(QColor color);

	void scaleChanged(qreal scale);

	void linkedStatusChanged(bool linked);

	void categoryChanged(QString ref);

public slots:

protected:

	void checkWithin(QSizeF const& mapSize);

	void linkCategory(CartographyCategory* cat);

	Cartography* _cartographyParent;
	mutable QString _category;
	mutable CartographyCategory* _linkedCategory;

	QString _referedItem;
	QPointF _position;

	mutable QMetaObject::Connection _colorPipeConnection;

	qreal _scale;

};

class CATHIA_MODEL_EXPORT CartographyCategory : public Aline::EditableItem
{
	Q_OBJECT

	friend class Cartography;

public:

	static const QString CARTOGRAPHY_CATEGORY_TYPE_ID;

	class CATHIA_MODEL_EXPORT CartographyCategoryFactory : public Aline::EditableSubItemFactory
	{
	public:
		explicit CartographyCategoryFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItem* parent) const;

	};

	Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
	Q_PROPERTY(qreal scale READ getScale WRITE setScale NOTIFY scaleChanged)

	explicit CartographyCategory(QString ref, Cartography* parent);

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual QString iconInternalUrl() const;

	QColor getColor() const;
	void setColor(const QColor &color);

	qreal getScale() const;
	void setScale(qreal scale);

signals:

	void colorChanged(QColor col);
	void scaleChanged(qreal scale);

public slots:

protected:

	Cartography* _cartographyParent;

	QColor _color;
	qreal _scale;
};

class CATHIA_MODEL_EXPORT CartographyCategroryListModel : public QAbstractListModel
{
	Q_OBJECT

public:

	explicit CartographyCategroryListModel(Cartography* carto);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

protected:

	void insertCategory(CartographyCategory* cat);
	void removeCategory(QString ref);
	void onRefSwap(QString oldRef, QString newRef);

	QVector<CartographyCategory*> _categories;
	QMap<QString, CartographyCategory*> _dict;

	Cartography* _parentCarto;
};

class CATHIA_MODEL_EXPORT Cartography : public Sabrina::EditableItem
{
	Q_OBJECT
public:

	static const QString CARTOGRAPHY_TYPE_ID;

	class CATHIA_MODEL_EXPORT CartographyFactory : public Aline::EditableItemFactory
	{
	public:
		explicit CartographyFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	explicit Cartography(QString ref, Aline::EditableItemManager* parent = nullptr);

	Q_PROPERTY(QString background READ background WRITE setBackground NOTIFY backgroundChanged)
	Q_PROPERTY(QSizeF size READ getSize WRITE setSize NOTIFY sizeChanged)

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual QString iconInternalUrl() const;

	QString background() const;

	virtual void insertSubItem(EditableItem* item);

	QSizeF getSize() const;
	void setSize(const QSizeF &size);

	QList<QString> getCurrentCategoriesRefs() const;
	CartographyCategory* getCategoryByRef(QString ref, bool returnDefaultIfNotFound = false);

	CartographyCategroryListModel* getCategoryListModel();

signals:

	void backgroundChanged(QString bg);
	void sizeChanged(QSizeF size);

	void cartographyItemInserted(CartographyItem* item);
	void cartographyCategoryInserted(CartographyCategory* category);

	void cartographyItemRemoved(QString oldRef);
	void cartographyCategoryRemoved(QString oldRef);

public slots:

	void setBackground(QString url);

	void addCartoItem(QString const& referedItemRef);
	void addCartoPoint(QString const& name);
	void addCartoPoint();
	void addCategory(QString const& cat_name);

	void removeCartoItem(CartographyItem* item);
	void removeCartoCategory(CartographyCategory* item);

protected:
	CartographyCategory* getDefaultCat();
	void createDefaultCat();

	void insertCartoItem(CartographyItem* item);
	void insertCartoCat(CartographyCategory* item);

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);

	QString _background;

	QVector<CartographyItem*> _items;
	QMap<QString, CartographyCategory*> _categories;

	QSizeF _size;

	CartographyCategroryListModel* _categoryListModel;
};

} // namespace Sabrina

#endif // CARTOGRAPHY_H
