#ifndef PERSONNAGE_H
#define PERSONNAGE_H

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

class CATHIA_MODEL_EXPORT Personnage : public EditableItem
{
	Q_OBJECT
public:

	static const QString PERSONNAGE_TYPE_ID;

	class CATHIA_MODEL_EXPORT PersonnageFactory : public Aline::EditableItemFactory
	{
	public:
		explicit PersonnageFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	Personnage(QString ref, Aline::EditableItemManager *parent = nullptr);

	Q_PROPERTY(QString persoRace READ persoRace WRITE setPersoRace)
	Q_PROPERTY(int age READ age WRITE setAge)

	Q_PROPERTY(QString persoBackground READ background WRITE setBackground)

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	QString background() const;

	QString persoRace() const;

	int age() const;

	virtual QString iconInternalUrl() const;

signals:

	void persoRaceChanged(QString newRace);
	void persoAgeChanged(int newAge);

	void persoBackgroundChanged(QString newBackground);

public slots:

	void setBackground(QString background);
	void setPersoRace(QString perso_race);
	void setAge(int age);

protected:

	virtual void treatDeletedRef(QString deletedRef);
	virtual void treatChangedRef(QString oldRef, QString newRef);

	QString _perso_race;

	int _age;

	QString _perso_background;
};

} // namespace Sabrina

#endif // PERSONNAGE_H
