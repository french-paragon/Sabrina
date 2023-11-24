#ifndef SABRINA_COMICSCRIPT_H
#define SABRINA_COMICSCRIPT_H

/*
This file is part of the project Sabrina
Copyright (C) 2020-2023  Paragon <french.paragon@gmail.com>

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

#include "model/editableitem.h"
#include <Aline/model/editableitemfactory.h>

#include "text/comicscript.h"

#include <QJsonArray>
#include <QJsonObject>

namespace Sabrina {

class CATHIA_MODEL_EXPORT Comicscript : public EditableItem
{
	Q_OBJECT
public:

	static const QString COMICSTRIP_TYPE_ID;
	static const QString COMICSTRIP_TEXT_ID;

	static const QString TEXT_ID;
	static const QString CHARACTER_ID;
	static const QString CHILDREN_ID;

	static void extractComicScriptFromJson(Aline::EditableItem* script, QJsonObject const& obj, bool blockChangeTracks);
	static QJsonObject encapsulateComicScriptToJson(Aline::EditableItem* script);


	class CATHIA_MODEL_EXPORT ComicstripFactory : public Aline::EditableItemFactory
	{
	public:
		explicit ComicstripFactory(QObject *parent = nullptr);
		Aline::EditableItem* createItem(QString ref, Aline::EditableItemManager* parent) const;

	};

	Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QString synopsis READ synopsis WRITE setSynopsis NOTIFY synopsisChanged)

	explicit Comicscript(QString ref, Aline::EditableItemManager *parent);

	QString getTypeId() const override;
	QString getTypeName() const override;

	QString iconInternalUrl() const override;

	QString getTitle() const;
	void setTitle(const QString &title);

	QString synopsis() const;
	void setSynopsis(const QString &synopsis);

	TextNode* document();

signals:

	void titleChanged(QString title);
	void synopsisChanged(QString synopsys);

protected:

	void treatDeletedRef(QString deletedRef) override;
	void treatChangedRef(QString oldRef, QString newRef) override;


	QString _title;
	QString _synopsis;

	TextNode* _document;

};

} // namespace Sabrina

#endif // SABRINA_COMICSCRIPT_H
