/*
This file is part of the project Sabrina
Copyright (C) 2018  Paragon <french.paragon@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "editableitemtypespecializedaction.h"

namespace Sabrina {

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
