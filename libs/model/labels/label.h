#ifndef LABEL_H
#define LABEL_H

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
