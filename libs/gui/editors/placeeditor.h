#ifndef SABRINA_PLACEEDITOR_H
#define SABRINA_PLACEEDITOR_H

#include <aline/src/editableitemeditor.h>
#include <aline/src/editorfactory.h>
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
