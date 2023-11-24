#ifndef LIEU_H
#define LIEU_H

/*
This file is part of the project Sabrina
Copyright (C) 2018-2023  Paragon <french.paragon@gmail.com>

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

#include "model/editableitem.h"
#include <Aline/model/editableitemfactory.h>

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
