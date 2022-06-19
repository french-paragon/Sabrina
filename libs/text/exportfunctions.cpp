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

#include "exportfunctions.h"
#include "textnode.h"
#include "textstylemanager.h"
#include "abstracttextstyle.h"

#include <QtPrintSupport/QPrinter>
#include <QPainter>

namespace Sabrina {

bool savePdf(TextNode* node,
			 TextStyleManager* stylesheet,
			 QPageLayout const& pLayout,
			 QString const& outFile) {

	if (node == nullptr) {
		return false;
	}

	if (stylesheet == nullptr) {
		return false;
	}

	QPageLayout layout = pLayout;
	layout.setMode(QPageLayout::StandardMode);

	QPrinter printer;

	printer.setPageLayout(layout);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(outFile);

	QRect area = layout.paintRectPixels(printer.resolution());

	QPainter painter;
	painter.setWindow(area);
	painter.begin(&printer);

	TextNode* currentNode = node;
	TextNode* final = node->lastNode()->nextNode();

	int h = 0;

	do {

		AbstractTextNodeStyle* style = stylesheet->getStyleByCode(currentNode->styleId());

		style->clearCache();
		style->layNodeOut(currentNode, area.width());

		if (h + style->nodeHeight(currentNode, area.width()) >= area.height()) {

			bool ok = printer.newPage();

			if (!ok) {
				return false;
			}

			h = 0;
		}

		style->renderNode(currentNode, QPointF(0, h), area.width(), painter);

		h += style->nodeHeight(currentNode, area.width());

		currentNode = currentNode->nextNode();

	} while (currentNode != final and currentNode != nullptr);

	painter.end();

	return true;

}


} //namespace Sabrina
