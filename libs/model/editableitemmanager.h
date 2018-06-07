#ifndef EDITABLEITEMMANAGER_H
#define EDITABLEITEMMANAGER_H

#include "model_global.h"

#include "aline/src/model/editableitemmanager.h"

#include <QAbstractItemModel>
#include <QMap>
#include <QException>

#include <functional>

namespace Aline {
	class EditableItem;
}

namespace Sabrina {

class EditableItem;
class EditableItemManager;
class LabelsTree;
class Label;
class NotesList;

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

class CATHIA_MODEL_EXPORT EditableItemManager : public Aline::EditableItemManager
{
	Q_OBJECT
public:

	explicit EditableItemManager(QObject *parent = nullptr);

	EditableItem* activeItem() const;

	LabelsTree* labelsTree();

	NotesList *noteList() const;

signals:

	void activeItemChanged();

public slots:

	void setActiveItem(QString ref);

protected:

	/*!
	 * \brief effectivelyLoadLabels load the labels from the datasource.
	 */
	virtual void effectivelyLoadLabels() = 0;

	LabelsTree* _labels;

	EditableItem* _activeItem;

	NotesList* _noteList;

};

} // namespace Cathia

#endif // EDITABLEITEMMANAGER_H
