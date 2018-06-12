#ifndef SABRINA_EDITABLEITEM_H
#define SABRINA_EDITABLEITEM_H

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

#include "model_global.h"

#include <aline/src/model/editableitem.h>
#include <aline/src/model/editableitemmanager.h>

#include <QObject>
#include <QSet>
#include <QVector>

namespace Sabrina {

class EditableItemManager;
class NotesList;

class CATHIA_MODEL_EXPORT EditableItem : public Aline::EditableItem
{
	Q_OBJECT

public:

	explicit EditableItem(QString ref, Aline::EditableItemManager *parent = nullptr);

	static const QString PREF_PROP_NAME;

	static const QString REF_IN_PROP_NAME;
	static const QString REF_FROM_PROP_NAME;

	static const QString NOTES_PROP_NAME;

	/*!
	 * \brief getLinkedItemsRefs get the list of reference of item refered in the current item.
	 * \return the list of references.
	 */
	const QSet<QString> & getLinkedItemsRefs() const;

	/*!
	 * \brief getReferentItemRefs allow to acess the list of items which refer to this item.
	 * \return the list of refs of the refering items.
	 */
	const QSet<QString> & getReferentItemRefs() const;

	/*!
	 * \brief iconInternalUrl indicate a path to a url representing the item.
	 * \return a path to a ressource.
	 */
	virtual QString iconInternalUrl() const;

	void addOutRef(QString const& ref);
	virtual void addInRef(QString const& ref);

	NotesList *getNoteList() const;

signals:

public slots:

	/*!
	 * \brief suppress trigger a suppression, it will warn all the referent items that this item do not exist anymore.
	 */
	virtual void suppress();

	/*!
	 * \brief changeRef change the reference, it will trigger all refering items that the ref has changed.
	 */
	virtual void changeRef(QString const& newRef);

	/*!
	 * \brief warnReffering alert this item that a new item is refering it.
	 * \param refReferant the ref of the item that refer to that item.
	 */
	void warnReffering(QString refReferant);
	/*!
	 * \brief warnUnReffering alert this item that one of the referent item is not refering it anymore.
	 * \param refReferant the ref of the item that unrefer to that item.
	 */
	void warnUnReffering(QString refReferant);

protected:

	/*!
	 * \brief refferedItemAboutToBeDeleted warn the item than one item it refer to is about to be deleted.
	 * \param ref the ref of the item about to be deleted.
	 */
	void refferedItemAboutToBeDeleted(QString ref);

	/*!
	 * \brief refferedItemAboutToBeDeleted warn the item than one item it refer to is about to be moved.
	 * \param ref the ref of the item about to be moved.
	 */
	void refferedItemAboutToChangeRef(QString ref, QString newRef);

	virtual void treatDeletedRef(QString deletedRef) = 0;
	virtual void treatChangedRef(QString oldRef, QString newRef) = 0;

	QSet<QString> _referencedItems;
	QSet<QString> _referentItems;

	NotesList* _noteList;

};

} // namespace Cathia

#endif // SABRINA_EDITABLEITEM_H
