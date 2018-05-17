#include "labelsmodelitemdelegate.h"

#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

namespace Sabrina {

LabelsModelItemDelegate::LabelsModelItemDelegate(QTreeView *parent) :
	QItemDelegate(parent)
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

	return QAbstractItemDelegate::editorEvent(
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
