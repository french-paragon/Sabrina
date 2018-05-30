#ifndef SABRINA_NOTESEDITWIDGET_H
#define SABRINA_NOTESEDITWIDGET_H

#include <QWidget>

#include <QSortFilterProxyModel>
#include "model/notes/noteslist.h"

namespace Sabrina {

namespace Ui {
class NotesEditWidget;
}

class NotesEditWidget : public QWidget
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
