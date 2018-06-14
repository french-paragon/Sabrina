#ifndef SABRINA_NOTESEDITWIDGET_H
#define SABRINA_NOTESEDITWIDGET_H

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

#include <QWidget>

#include <QSortFilterProxyModel>
#include "model/notes/noteslist.h"

#include "../gui_global.h"

namespace Sabrina {

namespace Ui {
class NotesEditWidget;
}

class CATHIA_GUI_EXPORT NotesEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit NotesEditWidget(QWidget *parent = 0);
	~NotesEditWidget();

	void setCurrentNoteList(NotesList* list);

private:

	void addItemToModel();
	void removeCurrentItemFromModel();

	Ui::NotesEditWidget *ui;

	NotesList* _currentList;
	QSortFilterProxyModel* _proxy;
};


} // namespace Sabrina
#endif // SABRINA_NOTESEDITWIDGET_H
