#ifndef LIEU_H
#define LIEU_H

#include "model/editableitem.h"
#include "model/editableitemfactory.h"

namespace Sabrina {

class Place : public EditableItem
{
	Q_OBJECT
public:

	static const QString PLACE_TYPE_ID;

	class PlaceFactory : public EditableItemFactory
	{
	public:
		explicit PlaceFactory(QObject *parent = nullptr);
		EditableItem* createItem(QString ref, EditableItemManager* parent) const;

	};

	Place(QString ref, EditableItemManager *parent = nullptr);

	Q_PROPERTY(QString placeDescription READ placeDescription WRITE setplaceDescription)

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	QString placeDescription() const;

	virtual QString iconInternalUrl() const;

signals:

	void placeDescriptionChanged(QString newDescription);

public slots:

	void setplaceDescription(QString placeDescription);

protected:

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);

	QString _place_description;
};

} // namespace Sabrina

#endif // LIEU_H
