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

#include "textstylemanager.h"

#include "abstracttextstyle.h"

namespace Sabrina {

TextStyleManager::TextStyleManager(QObject *parent) :
	QObject(parent)
{

}

bool TextStyleManager::registerStyle(AbstractTextNodeStyle* style) {
	if (!_styles.contains(style->typeId())) {
		_styles.insert(style->typeId(), style);
		Q_EMIT styleInserted(style->typeId());
		return true;
	}
	return false;
}
AbstractTextNodeStyle* TextStyleManager::getStyleByCode(int code) {
	return _styles.value(code, nullptr);
}

void TextStyleManager::removeStyle(int code) {
	if (_styles.contains(code)) {
		_styles.remove(code);
		Q_EMIT styleRemoved(code);
	}
}

int TextStyleManager::getDefaultStyleCode() const {
	return 0;
}

} // namespace Sabrina
