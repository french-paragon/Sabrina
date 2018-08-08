#include <QTest>

#include "core/app.h"
#include "aline/tests/tests_utils/editableitemtestmodel.h"
#include "aline/tests/tests_utils/testeditableitemmanager.h"

#include "model/editableItems/personnage.h"

class PersonnageTest : public Aline::Tests::EditableItemTestModel
{
	Q_OBJECT
public:
private slots :
	void initTestCase();

	void StaticPropertiesTypes();
	void StaticPropertiesChanges();

	void cleanupTestCase();
};


void PersonnageTest::initTestCase() {

	Aline::Tests::EditableItemTestModel::initTestCase();

	Sabrina::App::loadEditableFactories(); //load the factories for Sabrina items

	QString ref;
	_manager->createItem(Sabrina::Personnage::PERSONNAGE_TYPE_ID, "subject", &ref);

	_subject = _manager->loadItem(ref);

}

void PersonnageTest::StaticPropertiesTypes() {
	Aline::Tests::EditableItemTestModel::StaticPropertiesTypes();
}
void PersonnageTest::StaticPropertiesChanges() {
	Aline::Tests::EditableItemTestModel::StaticPropertiesChanges();
}

void PersonnageTest::cleanupTestCase() {

	Aline::Tests::EditableItemTestModel::cleanupTestCase();

}

QTEST_MAIN(PersonnageTest)
#include "testcharactereditableitem.moc"
