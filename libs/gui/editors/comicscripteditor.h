#ifndef COMICSCRIPTEDITOR_H
#define COMICSCRIPTEDITOR_H

/*
This file is part of the project Sabrina
Copyright (C) 2020  Paragon <french.paragon@gmail.com>

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

#include "gui/gui_global.h"

#include "aline/src/view/editableitemeditor.h"
#include "aline/src/view/editorfactory.h"

namespace Sabrina {

namespace Ui {
class ComicscriptEditor;
}

class CATHIA_GUI_EXPORT ComicscriptEditor : public Aline::EditableItemEditor
{
	Q_OBJECT

public:
	static const QString COMICSCRIPT_EDITOR_TYPE_ID;

	class CATHIA_GUI_EXPORT ComicscriptEditorFactory : public Aline::EditorFactory
	{
		public :
			explicit ComicscriptEditorFactory(QObject* parent = nullptr);
			virtual Aline::Editor* createItem(QWidget* parent) const;
	};

	explicit ComicscriptEditor(QWidget *parent = nullptr);
	~ComicscriptEditor();

	QString getTypeId() const override;
	QString getTypeName() const override;

	/*!
	 * \brief editableType allow to get the types the editor might support
	 * \return the type id string, or empty string if none is supported.
	 */
	QStringList editableTypes() const override;

protected:

	bool effectivelySetEditedItem(Aline::EditableItem* item) override;

	void checkAddButtonsActivation();

private:
	Ui::ComicscriptEditor *ui;
};

} // namespace Sabrina

#endif // COMICSCRIPTEDITOR_H
