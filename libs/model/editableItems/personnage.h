#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include "editableitem.h"
#include "editableitemfactory.h"

namespace Sabrina {

class Personnage : public EditableItem
{
	Q_OBJECT
public:

	class PersonnageFactory : public EditableItemFactory
	{
	public:
		explicit PersonnageFactory(QObject *parent = nullptr);
		EditableItem* createItem(EditableItemManager* parent) const;

	};

	Personnage(EditableItemManager *parent = nullptr);

	Q_PROPERTY(QString persoRace READ persoRace WRITE setPersoRace)
	Q_PROPERTY(int age READ age WRITE setAge)

	Q_PROPERTY(QString persoBackground READ background WRITE setBackground)

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	QString background() const;

	QString persoRace() const;

	int age() const;

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
