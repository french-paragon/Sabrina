#ifndef SABRINA_TEXT_EXPORTFUNCTIONS_H
#define SABRINA_TEXT_EXPORTFUNCTIONS_H

/*
This file is part of the project Sabrina
Copyright (C) 2022  Paragon <french.paragon@gmail.com>

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

#include <QPageLayout>

namespace Sabrina {

class TextNode;
class TextLine;
class TextStyleManager;

/*!
 * \brief savePdf save a text document (or part of it) to pdf
 * \param node the document node
 * \param stylesheet the stylesheet to use.
 * \param layout The page layout
 * \param outFile The target file
 * \param warningParent A widget to use as parent for confirmation messages. If nullptr, no warning will be displayed or error reported.
 * \return true if the file was saved, false otherwise.
 */
bool savePdf(TextNode* node,
			 TextStyleManager* stylesheet,
			 QPageLayout const& layout,
			 QString const& outFile);

} //namespace Sabrina

#endif // SABRINA_TEXT_EXPORTFUNCTIONS_H
