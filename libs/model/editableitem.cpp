#include "editableitem.h"

#include "editableitemmanager.h"

#include "utils/stringutils.h"

#include "notes/noteslist.h"

#include <QDebug>

namespace Sabrina {

const QString EditableItem::REF_PROP_NAME = "ref";
const QString EditableItem::PREF_PROP_NAME = "parentRef";
const QString EditableItem::TYPE_ID_NAME = "type_id";

const QString EditableItem::REF_IN_PROP_NAME = "ref_in_to_out";
const QString EditableItem::REF_FROM_PROP_NAME = "ref_from_out";
const QString EditableItem::CHILDREN_PROP_NAME = "ref_childrens";

const QString EditableItem::NOTES_PROP_NAME = "notes";

EditableItem::EditableItem(QString ref, EditableItemManager *parent) :
	Aline::EditableItem(parent),
	_ref(ref),
	_manager(parent)
{

	_noteList = new NotesList(this);

	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::newUnsavedChanges);
	connect(this, &EditableItem::objectNameChanged, this, &EditableItem::onVisibleStateChanged);
	connect(this, &EditableItem::unsavedStateChanged, this, &EditableItem::onVisibleStateChanged);

}

QString EditableItem::simplifyRef(QString ref) {

	return StringUtils::simplifyRef(ref);

}

const QSet<QString> & EditableItem::getLinkedItemsRefs() const {
	return _referencedItems;
}

const QSet<QString> & EditableItem::getReferentItemRefs() const {
	return _referentItems;
}

QVector<QString> EditableItem::getChildrenItemsRefs() const {
	if (_manager != nullptr) {
		return _manager->listChildren(getRef());
	}
	return QVector<QString>();
}

QString EditableItem::iconInternalUrl() const {
	return ":/icons/icons/editable_item_default.svg";
}

bool EditableItem::getHasUnsavedChanged() const
{
    return _hasUnsavedChanged;
}

QString EditableItem::getRef() const
{
	return _ref;
}

bool EditableItem::acceptChildrens() const {
	return false; //return false by default
}

void EditableItem::addOutRef(QString const& ref) {
	_referentItems.insert(ref);
}

void EditableItem::addInRef(QString const& ref) {
	_referencedItems.insert(ref);
}

void EditableItem::suppress() {

	for (QString ref : _referentItems) {
		EditableItem* referentItem;

		try {

			referentItem = _manager->loadItem(ref);

		} catch (ItemIOException & e) {

			qDebug() << e.what();

			continue;
		}

		referentItem->refferedItemAboutToBeDeleted(getRef());
	}

}

void EditableItem::changeRef(QString const& newRef) {

	for (QString ref : _referentItems) {
		EditableItem* referentItem;

		try {

			referentItem = _manager->loadItem(ref);

		} catch (ItemIOException & e) {

			qDebug() << e.what();

			continue;
		}

		referentItem->refferedItemAboutToChangeRef(getRef(), newRef);
	}

	_ref = newRef;
	emit refChanged(_ref);

}

void EditableItem::warnReffering(QString refReferant) {
	_referentItems.insert(refReferant);
}

void EditableItem::warnUnReffering(QString refReferant) {
	_referentItems.remove(refReferant);
}

bool EditableItem::save() {

	if (_manager != nullptr) {
		bool status = _manager->saveItem(getRef());

		if (status && _hasUnsavedChanged) {
			_hasUnsavedChanged = false;
			emit unsavedStateChanged(false);
		}
	}

	return false;
}

void EditableItem::clearHasUnsavedChanges() {
	if (_hasUnsavedChanged) {
		_hasUnsavedChanged = false;
		emit unsavedStateChanged(false);
	}
}

NotesList *EditableItem::getNoteList() const
{
	return _noteList;
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

void EditableItem::onVisibleStateChanged() {
	emit visibleStateChanged(_ref);
}

void EditableItem::newUnsavedChanges() {

	if (autoSave()) {
		save();
		return;
	}

	bool previous = _hasUnsavedChanged;

	_hasUnsavedChanged = true;

	if (previous != _hasUnsavedChanged) {
		emit unsavedStateChanged(_hasUnsavedChanged);
	}
}

} // namespace Cathia
