#ifndef LABELSMODELITEMDELEGATE_H
#define LABELSMODELITEMDELEGATE_H

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
