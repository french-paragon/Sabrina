#ifndef LABEL_H
#define LABEL_H

#include "model_global.h"

#include <QObject>
#include <QVector>

namespace Aline {
	class EditableItem;
}

namespace Sabrina {

class LabelsTree;

class CATHIA_MODEL_EXPORT Label : public QObject
{
	Q_OBJECT
public:

	static const char* PROP_LABELS;

	explicit Label(Label *parent = nullptr);
	explicit Label(LabelsTree *parent = nullptr);

	Q_PROPERTY(QString ref READ getRef WRITE setRef NOTIFY refChanged)

	QString getRef() const;
	void setRef(QString ref);
	void setParentLabel(Label* parent);

	bool addChild(Label* child);
	void removeChild(Label* child);
	bool hasChild(Label* child) const;
	bool insertNewLabels(int row, int count);
	bool insertSubLabels(int row, QVector<Label*> const& labels);
	bool removeLabels(int row, int count);
	QVector<Label*> const& subLabels() const;

	bool markItem(Aline::EditableItem* item);
	bool unmarkItem(Aline::EditableItem* item);

	bool markItem(QString const& itemRef);
	bool unmarkItem(QString const& itemRef);

	QVector<QString> const& itemsRefs() const;

	Label* parentLabel() const;
	LabelsTree* parentTree() const;

	bool hasBeenRefed() const;

	QStringList refAndSubRefs() const;

signals:

	void refChanged(QString oldRef, QString newRef);
	void gettedRef(QString newRef);

	void itemRefAdded(QString newRef);
	void itemRefRemoved(QString oldRef);

public slots:

protected:

	QVector<Label*> _childrens;
	QVector<QString> _itemsRefs;

	Label* _parentLabel;
	LabelsTree* _treeParent;

	QString _ref;
	bool _hasBeenRefed;

	void hasBeenNamed(const QString &name);
	bool _hasBeenNamed;

};

} // namespace Sabrina

#endif // LABEL_H
