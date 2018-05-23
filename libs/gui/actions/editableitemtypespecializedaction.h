#ifndef EDITABLEITEMTYPESPECIALIZEDACTION_H
#define EDITABLEITEMTYPESPECIALIZEDACTION_H

#include "gui_global.h"

#include <QAction>

namespace Sabrina {

class CATHIA_GUI_EXPORT EditableItemTypeSpecializedAction : public QAction
{
	Q_OBJECT

public:

	explicit EditableItemTypeSpecializedAction(const QString & type_id,
											   QObject *parent = nullptr);

	explicit EditableItemTypeSpecializedAction(const QString & type_id,
											   const QString &text,
											   QObject *parent = nullptr);

	explicit EditableItemTypeSpecializedAction(const QString & type_id,
											   const QIcon &icon,
											   const QString &text,
											   QObject *parent = nullptr);

signals:

	void triggered(QString type_id, bool checked = false);

protected:

	void transferTrigger(bool checked);

	QString _type_id;
};

} // namespace Cathia

#endif // EDITABLEITEMTYPESPECIALIZEDACTION_H
