#ifndef EDITABLEITEM_H
#define EDITABLEITEM_H

#include "model_global.h"

#include <QObject>
#include <QSet>
#include <QVector>

namespace Cathia {

class EditableItemManager;

class CATHIA_MODEL_EXPORT EditableItem : public QObject
{
	Q_OBJECT
public:

	static QString simplifyRef(QString ref);

	explicit EditableItem(EditableItemManager *parent = nullptr);

	Q_PROPERTY(bool hasUnsavedChanges READ getHasUnsavedChanged NOTIFY unsavedStateChanged)

	Q_PROPERTY(QString ref READ getRef WRITE changeRef NOTIFY refChanged)
	Q_PROPERTY(QString parentRef MEMBER _p_ref NOTIFY parentChanged)

	/*!
	 * \brief getTypeId allow to get info on the type of the editable item.
	 * \return a type id as a string.
	 */
	virtual QString getTypeId() const = 0;

	/*!
	 * \brief getLinkedItemsRefs get the list of reference of item refered in the current item.
	 * \return the list of references.
	 */
	QSet<QString> getLinkedItemsRefs() const;

	/*!
	 * \brief getReferentItemRefs allow to acess the list of items which refer to this item.
	 * \return the list of refs of the refering items.
	 */
	QSet<QString> getReferentItemRefs() const;

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

	bool getHasUnsavedChanged() const;

	QString getRef() const;

	/*!
	 * \brief acceptChildrens indicate if the item accept childs or not.
	 * \return true if the item accept children, false otherwise.
	 */
	virtual bool acceptChildrens() const;

signals:

	void refChanged(QString newRef);
	void unsavedStateChanged(bool saveState);
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
	bool save();

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

	QVector<QString>  _childrenItemRefs;

	EditableItemManager* _manager;

};

} // namespace Cathia

#endif // EDITABLEITEM_H
