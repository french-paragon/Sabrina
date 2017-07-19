#ifndef EDITABLEITEMMANAGER_H
#define EDITABLEITEMMANAGER_H

#include "model_global.h"

#include <QObject>
#include <QMap>
#include <QException>

#include <functional>

namespace Cathia {

class EditableItem;
class EditableItemManager;

class CATHIA_MODEL_EXPORT ItemLoadingException : public QException
{
public:

	ItemLoadingException (QString ref, QString infos, EditableItemManager const* manager);
	ItemLoadingException (ItemLoadingException const& other);

	const char* what() const throw();

	void raise() const;
	ItemLoadingException *clone() const;

	QString ref() const;
	QString infos() const;
	const EditableItemManager *manager() const;

protected:

	QString _ref;
	QString _infos;
	std::string _what;
	EditableItemManager const* _manager;
};

class CATHIA_MODEL_EXPORT EditableItemManager : public QObject
{
	Q_OBJECT
public:

	static const QChar RefSeparator;

	explicit EditableItemManager(QObject *parent = nullptr);

	EditableItem* loadItem(QString const& ref);
	QStringList loadedItems() const;
	void forceUnloadItem(QString ref);
	bool isItemLoaded(const QString &ref) const;

signals:

	void loadingItemSucessFull(QString ref);

	void itemAboutToBeUnloaded(QString ref);
	void itemUnloaded(QString ref);

public slots:

protected:

	/*!
	 * \brief effectivelyLoadItem load an item without checking if it is cached before.
	 * \param ref the ref of the item to load.
	 */
	virtual EditableItem* effectivelyLoadItem(QString const& ref) = 0;

	QMap<QString, EditableItem*> _loadedItems;


};

} // namespace Cathia

#endif // EDITABLEITEMMANAGER_H
