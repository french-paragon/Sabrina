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

#include "labelsmodelitemdelegate.h"

#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

namespace Sabrina {

LabelsModelItemDelegate::LabelsModelItemDelegate(QTreeView *parent) :
	QStyledItemDelegate(parent)
{

}

bool LabelsModelItemDelegate::editorEvent(QEvent* event,
													QAbstractItemModel* model,
													const QStyleOptionViewItem& option,
													const QModelIndex& index ) {

	QMouseEvent* mouseEvent = nullptr;

	if (event->type() == QEvent::MouseButtonPress) {
		// This is only safe because we've checked the type first.
		mouseEvent = static_cast<QMouseEvent*>(event);
	}

	if (mouseEvent and mouseEvent->button() == Qt::RightButton) {

		showContextMenu(model, index, mouseEvent->globalPos());

		// Return true to indicate that we have handled the event.
		// Note: This means that we won't get any default behavior!
		return true;
	}

	return QStyledItemDelegate::editorEvent(
		event, model, option, index);

}

void LabelsModelItemDelegate::showContextMenu(QAbstractItemModel *model,
											  QModelIndex const& index,
											  const QPoint& globalPos) {

	QMenu menu;

	QAction createSubAction(QIcon(":/icons/icons/new_simple.svg"), tr("nouveau sous-label"), &menu);
	auto callBackAdd = [model, &index] () {model->insertRow(model->rowCount(index), index); };
	connect(&createSubAction, &QAction::triggered, callBackAdd);

	QAction removeLabelAction(QIcon(":/icons/icons/delete_simple.svg"), tr("supprimer le label"), &menu);
	auto callBackRemove = [model, &index] () {model->removeRow(index.row(), index.parent()); };
	connect(&removeLabelAction, &QAction::triggered, callBackRemove);

	menu.addAction(&createSubAction);
	menu.addAction(&removeLabelAction);

	menu.exec(globalPos);

}

} //namespace Sabrina
