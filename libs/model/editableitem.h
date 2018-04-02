#ifndef SABRINA_EDITABLEITEM_H
#define SABRINA_EDITABLEITEM_H

#include "model_global.h"

#include <aline/src/editableitem.h>

#include <QObject>
#include <QSet>
#include <QVector>

namespace Sabrina {

class EditableItemManager;

class CATHIA_MODEL_EXPORT EditableItem : public Aline::EditableItem
{
	Q_OBJECT
public:

	static QString simplifyRef(QString ref);

	explicit EditableItem(QString ref, EditableItemManager *parent = nullptr);

	static const QString REF_PROP_NAME;
	static const QString PREF_PROP_NAME;
	static const QString TYPE_ID_NAME;

	static const QString REF_IN_PROP_NAME;
	static const QString REF_FROM_PROP_NAME;
	static const QString CHILDREN_PROP_NAME;

	Q_PROPERTY(QString ref READ getRef WRITE changeRef NOTIFY refChanged)
	Q_PROPERTY(QString parentRef MEMBER _p_ref NOTIFY parentChanged)
	Q_PROPERTY(QString type_id READ getTypeId)

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
	 * \brief getChildrenItemsRefs is a function to fetch all children item of that item.
	 * \return a list of children items.
	 */
	QVector<QString> getChildrenItemsRefs() const;

	/*!
	 * \brief iconInternalUrl indicate a path to a url representing the item.
	 * \return a path to a ressource.
	 */
	virtual QString iconInternalUrl() const;

	virtual bool getHasUnsavedChanged() const;

	QString getRef() const;

	/*!
	 * \brief acceptChildrens indicate if the item accept childs or not.
	 * \return true if the item accept children, false otherwise.
	 */
	virtual bool acceptChildrens() const;

	void addOutRef(QString const& ref);
	virtual void addInRef(QString const& ref);

signals:

	void refChanged(QString newRef);
	void parentChanged(QString newRef);

public slots:

	/*!
	 * \brief suppress trigger a suppression, it will warn all the referent items that this item do not exist anymore.
	 */
	void suppress();

	/*!
	 * \brief changeRef change the reference, it will trigger all refering items that the ref has changed.
	 */
	void changeRef(QString const& newRef);

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

	/*!
	 * \brief save save the item.
	 * \return true if sucess, false otherwise.
	 */
	virtual bool save();

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

	bool _hasUnsavedChanged;

	QString _ref;
	QString _p_ref;

	QSet<QString> _referencedItems;
	QSet<QString> _referentItems;

	EditableItemManager* _manager;

};

} // namespace Cathia

#endif // SABRINA_EDITABLEITEM_H
