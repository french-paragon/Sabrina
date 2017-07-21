#include "editableitem.h"

#include "editableitemmanager.h"

#include <QDebug>

namespace Cathia {

const QString EditableItem::REF_PROP_NAME = "ref";
const QString EditableItem::PREF_PROP_NAME = "parentRef";
const QString EditableItem::TYPE_ID_NAME = "type_id";

const QString EditableItem::REF_IN_PROP_NAME = "ref_in_to_out";
const QString EditableItem::REF_FROM_PROP_NAME = "ref_from_out";
const QString EditableItem::CHILDREN_PROP_NAME = "ref_childrens";

EditableItem::EditableItem(EditableItemManager *parent) : QObject(parent)
{

}

QString removeAccents(QString s) {

	static QString diacriticLetters_;
	static QStringList noDiacriticLetters_;

	if (diacriticLetters_.isEmpty()) {
		diacriticLetters_ = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
		noDiacriticLetters_ << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
	}

	QString output = "";
	for (int i = 0; i < s.length(); i++) {
		QChar c = s[i];
		int dIndex = diacriticLetters_.indexOf(c);
		if (dIndex < 0) {
			output.append(c);
		} else {
			QString replacement = noDiacriticLetters_[dIndex];
			output.append(replacement);
		}
	}

	return output;
}

QString EditableItem::simplifyRef(QString ref) {

	QString text = removeAccents(ref);

	text.replace(QRegExp("\\s+"), "_"); //remove whiteSpaces
	text.replace(QRegExp("[^A-Za-z0-9_]"), ""); //remove non accepted chars.
	text.replace(QRegExp("_+"), "_"); //clean groups off spaces which were separated by forbidden chars up.

	QString simpl_text = text.simplified();
	simpl_text = simpl_text.toLower();
	simpl_text = simpl_text.normalized(QString::NormalizationForm_C);

	return simpl_text;

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
		return _manager->saveItem(getRef());
	}

	return false;
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
