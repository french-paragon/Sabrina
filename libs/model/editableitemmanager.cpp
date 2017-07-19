#include "editableitemmanager.h"

#include "editableitem.h"

namespace Cathia {

const QChar EditableItemManager::RefSeparator = QChar('/');

EditableItemManager::EditableItemManager(QObject *parent) : QObject(parent)
{

}

EditableItem* EditableItemManager::loadItem(QString const& ref) {

	if (isItemLoaded(ref)) {
		return _loadedItems.value(ref);
	}

	return effectivelyLoadItem(ref);

}

QStringList EditableItemManager::loadedItems() const{
	return _loadedItems.keys();
}
void EditableItemManager::forceUnloadItem(QString ref) {

	if (!isItemLoaded(ref)) {
		return; //no need to unload a not present item.
	}

	emit itemAboutToBeUnloaded(ref);

	EditableItem* item = _loadedItems.value(ref, nullptr);
	_loadedItems.remove(ref);
	delete item; //effectively delete item.

	emit itemUnloaded(ref);

}

bool EditableItemManager::isItemLoaded(QString const& ref) const {
	return _loadedItems.contains(ref);
}


ItemLoadingException::ItemLoadingException (QString ref,
											QString infos,
											EditableItemManager const* manager) :
	_ref(ref),
	_infos(infos),
	_manager(manager)
{

	QString what = "[Error while loading \"" + _ref + "\" from \"" + manager->objectName() + "\"]: " + _infos;
	_what = what.toStdString();

}

ItemLoadingException::ItemLoadingException (ItemLoadingException const& other) :
	_ref(other.ref()),
	_infos(other.infos()),
	_manager(other.manager())
{

}

const char* ItemLoadingException::what() const throw() {

	return _what.c_str();

}


QString ItemLoadingException::ref() const
{
	return _ref;
}

QString ItemLoadingException::infos() const
{
	return _infos;
}

const EditableItemManager *ItemLoadingException::manager() const
{
	return _manager;
}

void ItemLoadingException::raise() const {
	throw *this;
}

ItemLoadingException * ItemLoadingException::clone() const {
	return new ItemLoadingException(*this);
}

} // namespace Cathia
