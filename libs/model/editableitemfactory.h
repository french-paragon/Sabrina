#ifndef EDITABLEITEMFACTORY_H
#define EDITABLEITEMFACTORY_H

#include <QObject>
#include <QMap>
#include <QAbstractListModel>

#include "model/model_global.h"

namespace Sabrina {

class EditableItem;
class EditableItemManager;

class CATHIA_MODEL_EXPORT EditableItemFactory : public QObject
{
	Q_OBJECT
public:
	explicit EditableItemFactory(QObject *parent = nullptr);

	QString getItemTypeId() const;
	QString getItemTypeName() const;
	QString getItemTypeIconUrl() const;
	virtual EditableItem* createItem(QString ref, EditableItemManager* parent) const = 0;

signals:

public slots:

protected:

	mutable QString _typeId;
	mutable QString _typeName;
	mutable QString _typeIconUrl;
};

class CATHIA_MODEL_EXPORT EditableItemFactoryManager : public QAbstractListModel
{
	Q_OBJECT
public:

	enum InternalDataRole{
		ItemRefRole = Qt::UserRole + 1
	};

	static EditableItemFactoryManager GlobalEditableItemFactoryManager;

	explicit EditableItemFactoryManager(QObject *parent = nullptr);

	void installFactory(EditableItemFactory* factory, bool takeOwnership = true);

	bool hasFactoryInstalled(QString type_id) const;
	EditableItem* createItem(QString type_id, QString item_id, EditableItemManager* parent) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	QString itemIconUrl(QString type_ref);

protected:

	QMap<QString, EditableItemFactory*> _installedFactories;
	QVector<QString> _installedFactoriesKeys;
};

} // namespace Cathia

#endif // EDITABLEITEMFACTORY_H
