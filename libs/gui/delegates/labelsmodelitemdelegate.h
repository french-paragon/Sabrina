#ifndef LABELSMODELITEMDELEGATE_H
#define LABELSMODELITEMDELEGATE_H

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

#include <QStyledItemDelegate>
#include <QTreeView>

#include "gui/gui_global.h"

namespace Sabrina {

class CATHIA_GUI_EXPORT LabelsModelItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit LabelsModelItemDelegate(QTreeView *parent = nullptr);

	virtual bool editorEvent(
		QEvent* event,
		QAbstractItemModel* model,
		const QStyleOptionViewItem& option,
		const QModelIndex& index );

private:
	void showContextMenu(QAbstractItemModel* model, const QModelIndex &index, const QPoint& globalPos);

signals:

public slots:
};

} //namespace Sabrina

#endif // LABELSMODELITEMDELEGATE_H
