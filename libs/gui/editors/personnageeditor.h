#ifndef SABRINA_PERSONNAGEEDITOR_H
#define SABRINA_PERSONNAGEEDITOR_H

/*
This file is part of the project Sabrina
Copyright (C) 2018-2023  Paragon <french.paragon@gmail.com>

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

#include <Aline/view/editableitemeditor.h>
#include <Aline/view/editorfactory.h>

#include <QWidget>

#include "gui/gui_global.h"

namespace Sabrina {

class Personnage;

namespace Ui {
class PersonnageEditor;
}

class CATHIA_GUI_EXPORT PersonnageEditor : public Aline::EditableItemEditor
{
	Q_OBJECT

public:
	static const QString PERSONNAGE_EDITOR_TYPE_ID;

	class CATHIA_GUI_EXPORT PersonnageEditorFactory : public Aline::EditorFactory
	{
		public :
			explicit PersonnageEditorFactory(QObject* parent = nullptr);
			virtual Aline::Editor* createItem(QWidget* parent) const;
	};

	explicit PersonnageEditor(QWidget *parent = 0);
	virtual ~PersonnageEditor();

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
	Ui::PersonnageEditor *ui;

	Personnage* _currentPerso;

	bool _has_intermediate_connections;

	QMetaObject::Connection _nameWatchConnection;
	QMetaObject::Connection _raceWatchConnection;
	QMetaObject::Connection _ageWatchConnection;

};


} // namespace Sabrina
#endif // SABRINA_PERSONNAGEEDITOR_H
