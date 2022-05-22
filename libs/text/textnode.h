#ifndef SABRINA_TEXTNODE_H
#define SABRINA_TEXTNODE_H

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

#include "./text_global.h"

#include <QObject>
#include <QMap>
#include <QJsonObject>


namespace Sabrina {

class TextNode;

class SABRINA_TEXT_EXPORT TextLine : public QObject
{
	Q_OBJECT
public:

	explicit TextLine(TextNode *parent = nullptr);

	QString getText() const;
	void setText(QString const& text);

	TextNode * nodeParent() const;
	TextLine* nextLine();
	TextLine* previousLine();
	TextLine* lineAfterOffset(int initialPos, int offset, int & newPos, int* unusedOffset = nullptr);

	int lineLineNumber() const;
	int lineNodeIndexNumber() const;

	//! \brief the numer of character in the document before the line
	int nCharsBefore() const;
	//! \brief the numer of character in the node before the line
	int nCharsBeforeInNode() const;
	//! \brief the numer of character in the line
	int nChars() const;
	//! \brief the numer of character in the document after the line
	int nCharsAfter() const;
	//! \brief the numer of character in the node after the line
	int nCharsAfterInNode() const;

Q_SIGNALS:

	void lineEdited(TextLine* line);

protected:

	QString _text;
};

class SABRINA_TEXT_EXPORT TextNode : public QObject
{
	Q_OBJECT
public:

	/*!
	 * \brief The TextNodeJsonRepresentationInfos struct store some const static member representing usefull key for a textnode json representation.
	 */
	struct TextNodeJsonRepresentationInfos {
		static const QString LINES_KEY;
		static const QString STYLE_ID_KEY;
		static const QString STYLE_NAME_KEY;
		static const QString CHILDREN_KEY;
		static const QString JUMPS_KEY;

		static const QString NODES_KEY;
	};

	struct TextNodeMimeTypeInfos {
		static const QString DocumentData;
	};

	/*!
	 * \brief The NodeCoordinate struct store a line index and a line pos.
	 * The line pos is relative to the start of the line, but the line index can be related to the parent node or document note depending on context.
	 */
	struct NodeCoordinate{
		NodeCoordinate() : lineIndex(-1), linePos(-1) {};
		NodeCoordinate(int lI, int lP) : lineIndex(lI), linePos(lP) {};
		inline bool isValid() const { return lineIndex >= 0 and linePos >= 0; }
		int lineIndex;
		int linePos;
	};

	typedef std::pair<TextNode::NodeCoordinate, TextNode::NodeCoordinate> DocumentInterval;

	/*!
	 * \brief nCharsBetweenNodes gives the number of text character present between two nodes
	 * \param start The first node (inclusive)
	 * \param end The last node (exclusive)
	 * \return the number of character in the interval.
	 */
	static int nCharsBetweenNodes(const TextNode* start, const TextNode* end = nullptr);

	static DocumentInterval intervalWithFlatParentsLevel(TextNode* n1, TextNode* n2);

	explicit TextNode(QObject *parent = nullptr, int nbLines = 1);

	int styleId() const;
	void setStyleId(int style_id);

	int nbTextLines() const;
	void setNbTextLines(int nb_text_line);

	TextLine* lineAt(int id);
	const TextLine* lineAt(int id) const;

	inline bool isRootNode() const {
		return parentNode() == nullptr;
	}
	TextNode* parentNode() const;

	int height() const;

	//! \brief the position in the chain of nodes
	int nodePos() const;

	//! \brief the position in the list of children of the parent node
	int nodeIndex() const;

	//! \brief the starting line of the node in the document
	int nodeLine() const;

	//! \brief the level of the node in the document
	int nodeLevel() const;

	//! \brief the numer of line to the last line of the last child of the node
	int maxLine() const;

	//! \brief the numer of character in the document part spanned by the node
	int nChars() const;

	//! \brief the numer of characters in the node (without child nodes).
	int nCharsInNode() const;

	//! \brief the numer of character in the document before the node
	int nCharsBefore() const;

	//! \brief the numer of character in the document after the node
	int nCharsAfter() const;

	//! \brief remove the node from the document, will suceed if the node is not a root node.
	bool clearFromDoc(bool deleteLater = true);

	int nbChildren() const;

	QList<TextNode*> const& childNodes();
	QList<const TextNode *> childNodes() const;
	QList<TextLine *> const& lines();

	TextNode* nextNode();
	const TextNode* nextNode() const;
	TextNode* previousNode();
	TextNode* lastNode() const;
	TextNode* rootNode();
	const TextNode* rootNode() const;
	TextNode* nodeAbove(int lvl);
	const TextNode* nodeAbove(int lvl) const;
	TextNode* subRootNode();
	TextNode* nodeAtLine(int line, int* nLine = nullptr);
	TextLine* getLineAtLine(int line);

	NodeCoordinate coordinateAtLineStart(NodeCoordinate coord) const;
	NodeCoordinate coordinateAtLineEnd(NodeCoordinate coord) const;

	NodeCoordinate coordinateAtNodeStart(NodeCoordinate coord) const;
	NodeCoordinate coordinateAtNodeEnd(NodeCoordinate coord) const;
	NodeCoordinate coordinateAfterNodeChildrens(NodeCoordinate coord) const;

	NodeCoordinate getCoordinateAfterOffset(NodeCoordinate coord, int offset) const;
	int offsetBetweenCoordinates(NodeCoordinate start, NodeCoordinate end) const;

	/*!
	 * \brief isBetweenNode determine if the node is between the two input nodes.
	 * \param sNode The starting node of the interval
	 * \param eNode The ending node of the interval, the function will search until that node, or until the end of document if it does not come after sNode.
	 * \param withBoundaries If true the function consider the start and end node to be part of the interval.
	 * \return true if the node is in the interval, false otherwise.
	 */
	bool isBetweenNode(TextNode const& sNode, TextNode const& eNode, bool withBoundaries = true);

	TextNode* insertNodeAbove(int styleCode = 0);
	TextNode* insertNodeAfter(int styleCode = 0);
	TextNode* insertNodeBefore(int styleCode = 0);
	TextNode* insertNodeSubRoot(int styleCode = 0);
	TextNode* insertNodeBelow(int styleCode = 0, int pos = 0);

	TextNode* moveNode(TextNode* newParent, int newPos);

	QString getHtmlRepresentation(NodeCoordinate start = NodeCoordinate(),
								  NodeCoordinate end = NodeCoordinate(),
								  QMap<int, QString> const& styleNameMap = {}) const;

	QJsonObject rangeJsonRepresentation(NodeCoordinate start = NodeCoordinate(),
										NodeCoordinate end = NodeCoordinate(),
										QMap<int, QString> const& styleNameMap = {}) const;

	QJsonObject fullJsonRepresentation(QMap<int, QString> const& styleNameMap = {}, const
									   TextNode *limitNode = nullptr) const;

Q_SIGNALS:

	void nodeRemoved(TextNode* parent, int oldRow);
	void nodeAdded(TextNode* parent, int newRow);
	void nodeEdited(TextNode* node, TextLine* line);
	void nodeMoved(TextNode* node, TextNode* oldParent);
	void nodeLineLayoutChanged(TextNode* node);

protected:

	QJsonObject fullJsonRepresentation(QMap<int, QString> const& styleNameMap = {}, const
									   TextNode *limitNode = nullptr,
									   bool *hitLimit = nullptr) const;

	void onLineEdited(TextLine* line);

	QList<TextNode*> _children;
	QList<TextLine*> _lines;

	int _style_id;

};

} // namespace Sabrina

#endif // SABRINA_TEXTNODE_H
