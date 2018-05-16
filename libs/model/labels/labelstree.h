#ifndef LABELSTREE_H
#define LABELSTREE_H

#include "model/model_global.h"

#include <QAbstractItemModel>
#include <QSet>

namespace Sabrina {

class Label;

class CATHIA_MODEL_EXPORT LabelsTree : public QAbstractItemModel
{
	Q_OBJECT
public:

	enum specialRoles{
		LabelRefRole = Qt::UserRole,
		LabelItemsRefsRole = Qt::UserRole+1
	};

	explicit LabelsTree(QObject *parent = nullptr);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	//TODO: create insertions and deletions of labels.
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool insertRows(int row, QVector<Label*> const& labels, const QModelIndex &parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

	bool makeRefUniq(QString &ref) const;

signals:

public slots:

protected:

	void registerRef(QString ref);
	void exchangeRef(QString oldRef, QString newRef);
	void removeRefs(QStringList const& refs);
	void insertRefs(QStringList const& refs);

	QVector<Label*> _labels;

	QSet<QString> _labelsRefs;

};

} // namespace Sabrina

#endif // LABELSTREE_H
