#include "stringutils.h"

#include <QRegExp>
#include <QStringList>

QString removeAccents(QString s) {

	static QString diacriticLetters_;
	static QStringList noDiacriticLetters_;

	if (diacriticLetters_.isEmpty()) {
		diacriticLetters_ = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
		noDiacriticLetters_ << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
	}

	QString output = "";
	for (int i = 0; i < s.length(); i++) {
		QChar c = s[i];
		int dIndex = diacriticLetters_.indexOf(c);
		if (dIndex < 0) {
			output.append(c);
		} else {
			QString replacement = noDiacriticLetters_[dIndex];
			output.append(replacement);
		}
	}

	return output;
}

QString Sabrina::StringUtils::simplifyRef(QString const& model) {

	QString text = removeAccents(model);

	text.replace(QRegExp("\\s+"), "_"); //remove whiteSpaces
	text.replace(QRegExp("[^A-Za-z0-9_]"), ""); //remove non accepted chars.
	text.replace(QRegExp("_+"), "_"); //clean groups off spaces which were separated by forbidden chars up.

	QString simpl_text = text.simplified();
	simpl_text = simpl_text.toLower();
	simpl_text = simpl_text.normalized(QString::NormalizationForm_C);

	return simpl_text;

}
