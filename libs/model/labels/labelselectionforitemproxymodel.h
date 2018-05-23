#ifndef LABELSELECTIONFORITEMPROXYMODEL_H
#define LABELSELECTIONFORITEMPROXYMODEL_H

#include <QIdentityProxyModel>

#include "model/model_global.h"

namespace Sabrina {

class LabelsTree;

class CATHIA_MODEL_EXPORT LabelSelectionForItemProxyModel : public QIdentityProxyModel
{
	Q_OBJECT
public:
	LabelSelectionForItemProxyModel(LabelsTree* parent);
	LabelSelectionForItemProxyModel(QObject* parent);

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	void setSourceModel(QAbstractItemModel *newSourceModel);
	void setSourceModel(LabelsTree* newSourceModel);

protected:

	void onActiveItemChanged();

	LabelsTree* _treeSource;

	QMetaObject::Connection _newActiveItemConnection;
};

} // namespace Sabrina

#endif // LABELSELECTIONFORITEMPROXYMODEL_H
