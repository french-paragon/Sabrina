#ifndef LABELSMODELITEMDELEGATE_H
#define LABELSMODELITEMDELEGATE_H

#include <QItemDelegate>
#include <QTreeView>

namespace Sabrina {

class LabelsModelItemDelegate : public QItemDelegate
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
