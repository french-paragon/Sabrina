#ifndef LIEU_H
#define LIEU_H

#include "model/editableitem.h"
#include "aline/src/model/editableitemfactory.h"

#include "model/model_global.h"

namespace Sabrina {

class CATHIA_MODEL_EXPORT Place : public EditableItem
{
	Q_OBJECT
public:

	static const QString PLACE_TYPE_ID;

	class CATHIA_MODEL_EXPORT PlaceFactory : public Aline::EditableItemFactory
	{
	public:
		explicit PlaceFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	Place(QString ref, Aline::EditableItemManager *parent = nullptr);

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
