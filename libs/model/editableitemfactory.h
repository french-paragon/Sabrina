#ifndef EDITABLEITEMFACTORY_H
#define EDITABLEITEMFACTORY_H

#include <QObject>
#include <QMap>

namespace Cathia {

class EditableItem;
class EditableItemManager;

class EditableItemFactory : public QObject
{
	Q_OBJECT
public:
	explicit EditableItemFactory(QObject *parent = nullptr);

	QString getItemTypeId() const;
	virtual EditableItem* createItem(EditableItemManager* parent) const = 0;

signals:

public slots:

protected:

	mutable QString _typeId;
};

class EditableItemFactoryManager : public QObject
{
	Q_OBJECT
public:

	static EditableItemFactoryManager GlobalEditableItemFactoryManager;

	explicit EditableItemFactoryManager(QObject *parent = nullptr);

	void installFactory(EditableItemFactory* factory, bool takeOwnership = true);

	bool hasFactoryInstalled(QString type_id) const;
	EditableItem* createItem(QString type_id, EditableItemManager* parent) const;


protected:

	QMap<QString, EditableItemFactory*> _installedFactories;
};

} // namespace Cathia

#endif // EDITABLEITEMFACTORY_H
