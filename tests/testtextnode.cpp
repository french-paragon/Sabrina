#include <QTest>
#include <QMetaType>

#include "text/textnode.h"

typedef Sabrina::TextNode::NodeCoordinate TextCoordinate;

Q_DECLARE_METATYPE(TextCoordinate)

class TextNodeTest : public QObject
{
	Q_OBJECT
public:
private slots :
	void initTestCase();

	void testNumberOfCharsCalculator_data();
	void testNumberOfCharsCalculator();

	void testTextCoordinateOffsetCalculator_data();
	void testTextCoordinateOffsetCalculator();

	void testOffsetBetweenPosCalculator_data();
	void testOffsetBetweenPosCalculator();

	void cleanupTestCase();
};

void TextNodeTest::initTestCase() {

}

void TextNodeTest::testNumberOfCharsCalculator_data() {
	QTest::addColumn<QStringList>("documentLines");

	QStringList doc;
	doc << "ABCDEFG" << "HIJKLMNOP";
	QTest::newRow("Simple test") << doc;

	QStringList doc3lines;
	doc3lines << "ABCDEFG" << "HIJKLMNOP" << "QRSTUV";
	QTest::newRow("Three lines") << doc3lines;
}

void TextNodeTest::testNumberOfCharsCalculator() {

	QFETCH(QStringList, documentLines);

	Sabrina::TextNode* root1 = new Sabrina::TextNode();
	root1->lineAt(0)->setText(documentLines.at(0));

	int totalDocSize = documentLines.at(0).length();

	for (int i = 1; i < documentLines.size(); i++) {
		Sabrina::TextNode* n = root1->insertNodeBelow(0,-1);
		n->lineAt(0)->setText(documentLines.at(i));
		totalDocSize += documentLines.at(i).length() + 1;
	}

	int totalNodeSize = documentLines.at(0).length();

	for (int i = 1; i < documentLines.size(); i++) {

		totalNodeSize += 1;

		QCOMPARE(root1->childNodes().at(i-1)->nChars(), documentLines.at(i).length());
		QCOMPARE(root1->childNodes().at(i-1)->nCharsInNode(), documentLines.at(i).length());
		QCOMPARE(root1->childNodes().at(i-1)->nCharsBefore(), totalNodeSize);

		totalNodeSize += documentLines.at(i).length();

		QCOMPARE(root1->childNodes().at(i-1)->nCharsAfter(), totalDocSize - totalNodeSize);
	}

	QCOMPARE(root1->nChars(), totalNodeSize);
	QCOMPARE(root1->nCharsInNode(), documentLines.at(0).length());
	QCOMPARE(root1->nCharsBefore(), 0);
	QCOMPARE(root1->nCharsAfter(), totalDocSize - documentLines.at(0).length());
}

void TextNodeTest::testTextCoordinateOffsetCalculator_data() {
	QTest::addColumn<QStringList>("documentLines");
	QTest::addColumn<TextCoordinate>("initialPos");
	QTest::addColumn<int>("offset");
	QTest::addColumn<TextCoordinate>("expectedFinalPos");

	QStringList doc;
	doc << "ABCDEFG" << "HIJKLMNOP";
	QTest::newRow("Simple test") << doc << TextCoordinate({0,3}) << 8 << TextCoordinate({1,3});

	QTest::newRow("Single line") << doc << TextCoordinate({0,3}) << 3 << TextCoordinate({0,6});

	QTest::newRow("Negative offset") << doc << TextCoordinate({1,3}) << -8 << TextCoordinate({0,3});
}
void TextNodeTest::testTextCoordinateOffsetCalculator() {

	QFETCH(QStringList, documentLines);
	QFETCH(TextCoordinate, initialPos);
	QFETCH(int, offset);
	QFETCH(TextCoordinate, expectedFinalPos);

	Sabrina::TextNode* root1 = new Sabrina::TextNode();
	root1->lineAt(0)->setText(documentLines.at(0));

	for (int i = 1; i < documentLines.size(); i++) {
		Sabrina::TextNode* n = root1->insertNodeBelow(0,-1);
		n->lineAt(0)->setText(documentLines.at(i));
	}

	TextCoordinate computed1 = root1->getCoordinateAfterOffset(initialPos, offset);

	QCOMPARE(computed1.lineIndex, expectedFinalPos.lineIndex);
	QCOMPARE(computed1.linePos, expectedFinalPos.linePos);

	delete root1;

	Sabrina::TextNode* root2 = new Sabrina::TextNode();
	root2->setNbTextLines(documentLines.size());

	for (int i = 0; i < documentLines.size(); i++) {
		root2->lineAt(i)->setText(documentLines.at(i));
	}

	TextCoordinate computed2 = root2->getCoordinateAfterOffset(initialPos, offset);

	QCOMPARE(computed2.lineIndex, expectedFinalPos.lineIndex);
	QCOMPARE(computed2.linePos, expectedFinalPos.linePos);

	delete root2;

}

void TextNodeTest::testOffsetBetweenPosCalculator_data() {
	QTest::addColumn<QStringList>("documentLines");
	QTest::addColumn<TextCoordinate>("initialPos");
	QTest::addColumn<TextCoordinate>("finalPos");
	QTest::addColumn<int>("expectedOffset");

	QStringList doc;
	doc << "ABCDEFG" << "HIJKLMNOP";
	QTest::newRow("Simple test") << doc << TextCoordinate({0,3}) << TextCoordinate({1,3}) << 8;

	QTest::newRow("Single line") << doc << TextCoordinate({0,3}) << TextCoordinate({0,6}) << 3;

	QTest::newRow("Negative offset") << doc << TextCoordinate({1,3}) << TextCoordinate({0,3}) << -8;

}
void TextNodeTest::testOffsetBetweenPosCalculator() {

	QFETCH(QStringList, documentLines);
	QFETCH(TextCoordinate, initialPos);
	QFETCH(TextCoordinate, finalPos);
	QFETCH(int, expectedOffset);


	Sabrina::TextNode* root1 = new Sabrina::TextNode();
	root1->lineAt(0)->setText(documentLines.at(0));

	for (int i = 1; i < documentLines.size(); i++) {
		Sabrina::TextNode* n = root1->insertNodeBelow(0,-1);
		n->lineAt(0)->setText(documentLines.at(i));
	}

	int offset1 = root1->offsetBetweenCoordinates(initialPos, finalPos);

	QCOMPARE(offset1, expectedOffset);

	delete root1;


	Sabrina::TextNode* root2 = new Sabrina::TextNode();
	root2->setNbTextLines(documentLines.size());

	for (int i = 0; i < documentLines.size(); i++) {
		root2->lineAt(i)->setText(documentLines.at(i));
	}

	int offset2 = root2->offsetBetweenCoordinates(initialPos, finalPos);

	QCOMPARE(offset2, expectedOffset);

	delete root2;

}

void TextNodeTest::cleanupTestCase() {

}


QTEST_MAIN(TextNodeTest)
#include "testtextnode.moc"
