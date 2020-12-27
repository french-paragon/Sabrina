#ifndef SABRINA_TEXTSTYLEMANAGER_H
#define SABRINA_TEXTSTYLEMANAGER_H

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

#include <QObject>
#include <QMap>

namespace Sabrina {

class AbstractTextNodeStyle;

class TextStyleManager : public QObject
{
	Q_OBJECT
public:
	TextStyleManager(QObject* parent = nullptr);

	bool registerStyle(AbstractTextNodeStyle* style);
	AbstractTextNodeStyle* getStyleByCode(int code);
	void removeStyle(int code);

	virtual int getDefaultStyleCode() const;

	enum class LevelJump {
		Below = 0,
		After = 1,
		Above = 2,
		UnderRoot = 3
	};

	struct NextNodeStyleAndPos {
		int code;
		LevelJump levelJump;
	};

	virtual QMap<Qt::KeyboardModifiers, NextNodeStyleAndPos> getNextNodeStyleAndPos(int code) const = 0;

Q_SIGNALS:

	void styleUpdated(int code);
	void styleInserted(int code);
	void styleRemoved(int code);

protected:

	QMap<int, AbstractTextNodeStyle*> _styles;
};

} // namespace Sabrina

#endif // SABRINA_TEXTSTYLEMANAGER_H
