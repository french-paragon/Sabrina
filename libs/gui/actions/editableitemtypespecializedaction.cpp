#include "editableitemtypespecializedaction.h"

namespace Cathia {

EditableItemTypeSpecializedAction::EditableItemTypeSpecializedAction(const QString &type_id,
																	 QObject *parent) :
	QAction(parent),
	_type_id(type_id)
{
	connect(this, &QAction::triggered, this, &EditableItemTypeSpecializedAction::transferTrigger);
}

EditableItemTypeSpecializedAction::EditableItemTypeSpecializedAction(const QString &type_id,
																	 const QString &text,
																	 QObject *parent) :
	QAction(text,
			parent),
	_type_id(type_id)
{
	connect(this, &QAction::triggered, this, &EditableItemTypeSpecializedAction::transferTrigger);
}

EditableItemTypeSpecializedAction::EditableItemTypeSpecializedAction(const QString &type_id,
																	 const QIcon &icon,
																	 const QString &text,
																	 QObject *parent) :
	QAction(icon,
			text,
			parent),
	_type_id(type_id)
{
	connect(this, &QAction::triggered, this, &EditableItemTypeSpecializedAction::transferTrigger);
}

void EditableItemTypeSpecializedAction::transferTrigger(bool checked) {

	emit triggered(_type_id, checked);

}

} // namespace Cathia
