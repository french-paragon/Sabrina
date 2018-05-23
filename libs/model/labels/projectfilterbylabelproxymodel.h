#ifndef PROJECTFILTERBYLABELPROXYMODEL_H
#define PROJECTFILTERBYLABELPROXYMODEL_H

#include <QAbstractProxyModel>

#include "model/model_global.h"

namespace Sabrina {

class LabelsTree;

class CATHIA_MODEL_EXPORT ProjectFilterByLabelProxyModel : public QAbstractProxyModel
{
	Q_OBJECT
public:
	explicit ProjectFilterByLabelProxyModel(QObject *parent = nullptr);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(QModelIndex const& index) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

	virtual void setSourceModel(QAbstractItemModel *sourceModel);

	void connectLabelTree(LabelsTree * labelTree);

signals:

public slots:

protected:

	virtual void setRefsInLabel(QStringList const& refs);
	virtual void addRefsInLabel(QString const& newRef);
	virtual void removeRefsInLabel(QString const& oldRef);

	void resetFilter();
	void exploreSourceModelIndex(QModelIndex const& sourceIndex);

	QStringList _refInLabel;
	QModelIndexList _selectedIndices;

	QMetaObject::Connection _connectionProject;

	QMetaObject::Connection _connectionChange;
	QMetaObject::Connection _connectionAdd;
	QMetaObject::Connection _connectionRemove;

};

} //namespace Sabrina

#endif // PROJECTFILTERBYLABELPROXYMODEL_H
