#include "editableitem.h"

#include "editableitemmanager.h"

#include "notes/noteslist.h"

#include <QDebug>

namespace Sabrina {

const QString EditableItem::PREF_PROP_NAME = "parentRef";

const QString EditableItem::REF_IN_PROP_NAME = "ref_in_to_out";
const QString EditableItem::REF_FROM_PROP_NAME = "ref_from_out";

const QString EditableItem::NOTES_PROP_NAME = "notes";

EditableItem::EditableItem(QString ref, Aline::EditableItemManager *parent) :
	Aline::EditableItem(ref, parent)
{

	_noteList = new NotesList(this);

}

const QSet<QString> & EditableItem::getLinkedItemsRefs() const {
	return _referencedItems;
}

const QSet<QString> & EditableItem::getReferentItemRefs() const {
	return _referentItems;
}

QString EditableItem::iconInternalUrl() const {
	return ":/icons/icons/editable_item_default.svg";
}

void EditableItem::addOutRef(QString const& ref) {
	_referentItems.insert(ref);
}

void EditableItem::addInRef(QString const& ref) {
	_referencedItems.insert(ref);
}

void EditableItem::changeRef(QString const& newRef) {

	for (QString ref : _referentItems) {
		EditableItem* referentItem;

		try {

			referentItem = qobject_cast<EditableItem*>(getManager()->loadItem(ref));

		} catch (ItemIOException & e) {

			qDebug() << e.what();

			continue;
		}

		if (referentItem != nullptr) {
			referentItem->refferedItemAboutToChangeRef(getRef(), newRef);
		}
	}

	QString oldRef = _ref;
	_ref = newRef;
	emit refSwap(oldRef, _ref);
	emit refChanged(_ref);

}

void EditableItem::warnReffering(QString refReferant) {
	_referentItems.insert(refReferant);
}

void EditableItem::warnUnReffering(QString refReferant) {
	_referentItems.remove(refReferant);
}

NotesList *EditableItem::getNoteList() const
{
	return _noteList;
}

void EditableItem::suppress() {

	for (QString ref : _referentItems) {
		EditableItem* referentItem;

		try {

			referentItem = qobject_cast<EditableItem*>(getManager()->loadItem(ref));

		} catch (ItemIOException & e) {

			qDebug() << e.what();

			continue;
		}

		if (referentItem != nullptr) {
			referentItem->refferedItemAboutToBeDeleted(getRef());
		}
	}

}

void EditableItem::refferedItemAboutToBeDeleted(QString ref) {
	treatDeletedRef(ref);
	_referencedItems.remove(ref);
}

void EditableItem::refferedItemAboutToChangeRef(QString ref, QString newRef) {
	treatChangedRef(ref, newRef);
	_referencedItems.remove(ref);
	_referencedItems.insert(newRef);
}

} // namespace Cathia
