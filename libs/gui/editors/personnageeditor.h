#ifndef SABRINA_PERSONNAGEEDITOR_H
#define SABRINA_PERSONNAGEEDITOR_H

#include <aline/src/editableitemeditor.h>
#include <aline/src/editorfactory.h>
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
