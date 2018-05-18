#ifndef EDITABLEITEMMANAGER_H
#define EDITABLEITEMMANAGER_H

#include "model_global.h"

#include <QAbstractItemModel>
#include <QMap>
#include <QException>

#include <functional>

namespace Sabrina {

class EditableItem;
class EditableItemManager;
class EditableItemFactoryManager;
class LabelsTree;
class Label;

class CATHIA_MODEL_EXPORT ItemIOException : public QException
{
public:

	ItemIOException (QString ref, QString infos, EditableItemManager const* manager);
	ItemIOException (ItemIOException const& other);

	const char* what() const throw();

	void raise() const;
	ItemIOException *clone() const;

	QString ref() const;
	QString infos() const;
	const EditableItemManager *manager() const;

protected:

	QString _ref;
	QString _infos;
	std::string _what;
	EditableItemManager const* _manager;
};

class CATHIA_MODEL_EXPORT EditableItemManager : public QAbstractItemModel
{
	Q_OBJECT
public:

	enum InternalDataRole{
		ItemRefRole = Qt::UserRole + 1,
		ItemAcceptChildrenRole = Qt::UserRole + 2
	};

	static const QChar RefSeparator;
	static const QString RefRoot;

	static const QString RefMimeType;

	explicit EditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const = 0;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

	EditableItem* activeItem() const;
	EditableItem* loadItem(QString const& ref);
	QStringList loadedItems() const;
	void forceUnloadItem(QString ref);
	bool isItemLoaded(const QString &ref) const;
	bool containItem(const QString & ref) const;

	LabelsTree* labelsTree();

	bool createItem(QString typeRef, QString ref, QString parent_ref = "");

	QVector<QString> listChildren(QString ref);

	virtual bool saveItem(QString ref);
	virtual bool saveAll();
	virtual bool saveStruct() = 0;
	virtual bool saveLabels() = 0;
	virtual bool loadStruct() = 0;

	EditableItemFactoryManager *factoryManager() const;

signals:

	void loadingItemSucessFull(QString ref);

	void itemAboutToBeUnloaded(QString ref);
	void itemUnloaded(QString ref);

	void activeItemChanged();

public slots:

	void setActiveItem(QString ref);

	virtual void reset() = 0;
	virtual void closeAll();

protected:

	void cleanTreeStruct();

	/*!
	 * \brief The treeStruct struct allow for the EditableItemManager to store the structure of loadable items in tree.
	 */
	struct treeStruct {
		treeStruct* _parent;
		QString _ref;
		QString _name;
		QString _type_ref;
		QVector<treeStruct*> _childrens;
		bool _acceptChildrens;
	};

	struct loadedItem {
		treeStruct* _treeStruct;
		EditableItem* _item;
	};

	QModelIndex indexFromLeaf(treeStruct* leaf) const;

	void itemVisibleStateChanged(QString ref);

	/*!
	 * \brief effectivelyLoadItem load an item without checking if it is cached before.
	 * \param ref the ref of the item to load.
	 */
	virtual EditableItem* effectivelyLoadItem(QString const& ref) = 0;
	/*!
	 * \brief effectivelyLoadLabels load the labels from the datasource.
	 */
	virtual void effectivelyLoadLabels() = 0;

	virtual bool effectivelySaveItem(QString const& ref) = 0;

	/*!
	 * \brief insertItem insert an item in the manager, the manager take ownership of the item.
	 * \param item the item to insert
	 */
	virtual bool insertItem(EditableItem* item, treeStruct *parent_branch);

	QMap<QString, loadedItem> _loadedItems;

	treeStruct* _root; //the root of the tree.
	QMap<QString, treeStruct*> _treeIndex; //build an index of the tree.

	EditableItemFactoryManager* _factoryManager;

	LabelsTree* _labels;

	EditableItem* _activeItem;

};

} // namespace Cathia

#endif // EDITABLEITEMMANAGER_H
