#ifndef SABRINA_PLACEEDITOR_H
#define SABRINA_PLACEEDITOR_H

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

#include <aline/src/view/editableitemeditor.h>
#include <aline/src/view/editorfactory.h>
#include <QWidget>

#include "gui/gui_global.h"

namespace Sabrina {

class Place;

namespace Ui {
class PlaceEditor;
}

class CATHIA_GUI_EXPORT PlaceEditor : public Aline::EditableItemEditor
{
	Q_OBJECT

public:

	static const QString PLACE_EDITOR_TYPE_ID;

	class CATHIA_GUI_EXPORT PlaceEditorFactory : public Aline::EditorFactory
	{
		public :
			explicit PlaceEditorFactory(QObject* parent = nullptr);
			virtual Aline::Editor* createItem(QWidget* parent) const;
	};

	explicit PlaceEditor(QWidget *parent = 0);
	virtual ~PlaceEditor();

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	/*!
	 * \brief editableType allow to get the types the editor might support
	 * \return the type id string, or empty string if none is supported.
	 */
	virtual QStringList editableTypes() const;

protected:

	virtual bool effectivelySetEditedItem(Aline::EditableItem* item);

	void onBackgroundTextChanged();

private:
	Ui::PlaceEditor *ui;

	Place* _current_place;

	QMetaObject::Connection _nameWatchConnection;
};


} // namespace Sabrina
#endif // SABRINA_PLACEEDITOR_H
