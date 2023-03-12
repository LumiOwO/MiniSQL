#include "interpreter.h"
#include <QStringList>
#include <QRegExp>
#include <QSet>
#include "../myerror/myerror.h"

#include "../statement/statements.h"

Interpreter::Interpreter()
{

}

Interpreter::~Interpreter()
{

}

QVector<Statement*> Interpreter::parse(const QString& codeText)
{
	QVector<Statement*> result;
	try{
		// remove the space in the front and tail
		QString code = codeText;
		chopBothEnds(code, "[\\s\\n]*", "[\\s\\n]*");
		if(code.isEmpty())
			throw MyError(MyError::EMPTY_TEXT);

		// remove the comments
		int begin = 0;
		QRegExp lineComment("(?:#|//)[^\\n]*");
		while((begin = lineComment.indexIn(code, begin)) >= 0)
			code.remove(begin, lineComment.matchedLength());

		begin = 0;
		QRegExp blockComment("/\\*.*(?:\\*/|$)");
		blockComment.setMinimal(true);
		while((begin = blockComment.indexIn(code, begin)) >= 0)
			code.remove(begin, blockComment.matchedLength());

		// split by ";"
		QStringList statements = code.split(QRegExp("[\\s\\n]*;[\\s\\n]*"), QString::SkipEmptyParts);
		int size = statements.size();
		if(size == 0)
			throw MyError(MyError::EMPTY_TEXT);

		// parse the code text to statement struct
		for(int i=0; i<statements.size(); i++) try
		{
//			qDebug() << i;
			// get the key word
			QString nowStatement = statements[i];
			QString keyWord = removeFirst(nowStatement);
//			qDebug() << keyWord;

			// switch the keyword to generate statement struct
			if(equals(keyWord, "create"))
			{
				QString secondWord = removeFirst(nowStatement);
//				qDebug() << secondWord;
				if(equals(secondWord, "table"))
					result.push_back(getCreateTable(nowStatement));
				else if(equals(secondWord, "index"))
					result.push_back(getCreateIndex(nowStatement));
				else
					throw MyError::UNKNOWN_STATEMENT;
			}
			else if(equals(keyWord, "drop"))
			{
				QString secondWord = removeFirst(nowStatement);
//				qDebug() << secondWord;
				if(equals(secondWord, "table"))
					result.push_back(getDropTable(nowStatement));
				else if(equals(secondWord, "index"))
					result.push_back(getDropIndex(nowStatement));
				else
					throw MyError::UNKNOWN_STATEMENT;
			}
			else if(equals(keyWord, "select"))
				result.push_back(getSelect(nowStatement));
			else if(equals(keyWord, "delete"))
				result.push_back(getDelete(nowStatement));
			else if(equals(keyWord, "insert"))
				result.push_back(getInsert(nowStatement));
			else if(equals(keyWord, "quit"))
				result.push_back(getQuit(nowStatement));
			else if(equals(keyWord, "execfile"))
				result.push_back(getExecFile(nowStatement));
			else
				throw MyError::UNKNOWN_STATEMENT;

			// set origin code
			result.last()->setCode(statements[i]);

		} catch(MyError::ErrorType type) {
			throw MyError(type, getLineNum(codeText, i), statements[i]);
		}

	} catch (MyError e) {
		// clear the result collection
		QVectorIterator<Statement*> iter(result);
		while(iter.hasNext())
			delete iter.next();

		throw std::move(e);
	}
	return result;
}

bool Interpreter::chopBothEnds(QString &code, QString headExp, QString endExp)
{
	bool success = true;

	QRegExp beginExp("^" + headExp);
	int begin = beginExp.indexIn(code) + beginExp.matchedLength();
	int end = code.indexOf(QRegExp(endExp + "$"));
	int len = end - begin;

	if(begin < 0 || end < 0 || len < 0)
		success = false;
	code = code.mid(begin, len);

	return success;
}

QString Interpreter::removeFirst(QString& code)
{
	// get the first word
	QRegExp firstEmpty("[\\s\\n]+|$|\\(");
	int index = firstEmpty.indexIn(code);
	QString firstWord = code.left(index);
	// remove the first word in the statement
	int begin = firstEmpty.capturedTexts()[0] == "("?
			index : index + firstEmpty.matchedLength();
	code = code.mid(begin);

	return firstWord;
}

int Interpreter::getLineNum(const QString& text, int StateNum)
{
	int lineNum = 1;
	for(int i=0; i<text.size(); i++)
	{
		if(StateNum == 0 && !QRegExp("\\s").exactMatch(QString(text[i])))
			break;
		if(text[i] == '\n')
			lineNum ++;
		else if(text[i] == ';')
			StateNum --;
	}
	return lineNum;
}

QVector<Condition*> Interpreter::parseConditions(QString code)
{
	QVector<Condition*> conditions;
	// parse conditions
	QRegExp andExp("[\\s\\n]*and[\\s\\n]*");
	andExp.setCaseSensitivity(Qt::CaseInsensitive);
	QStringList infos = code.split(andExp);
	QStringListIterator iter(infos);
	while(iter.hasNext())
	{
		QString nowInfo = iter.next();
		QRegExp cmpExp("[\\s\\n]*([>=<!]+)[\\s\\n]*");

		// get compare symbol
		Condition::Compare compare;
		if(cmpExp.indexIn(nowInfo) < 0)
			throw MyError::SYNTAX_ERROR;
		QString symbol = cmpExp.capturedTexts()[1];
		if(symbol == "=" || symbol == "==")
			compare = Condition::EQUAL;
		else if(symbol == "<>" || symbol == "!=")
			compare = Condition::NOT_EQUAL;
		else if(symbol == "<")
			compare = Condition::LESS_THAN;
		else if(symbol == ">")
			compare = Condition::LARGER_THAN;
		else if(symbol == "<=")
			compare = Condition::NOT_LARGER_THAN;
		else if(symbol == ">=")
			compare = Condition::NOT_LESS_THAN;
		else
			throw MyError::UNKNOWN_COMPARE;

		// get attribute name and value
		QStringList parsedInfo = nowInfo.split(cmpExp);
		QString attriName = parsedInfo[0];
		QString value = parsedInfo[1];
		chopBothEnds(value, "[\\s\\n]*(?:\"|\')*[\\s\\n]*", "[\\s\\n]*(?:\"|\')*[\\s\\n]*");

		// add the condition to the list
		conditions.push_back(new Condition(attriName, value, compare));
	}

	return conditions;
}

Statement* Interpreter::getCreateTable(QString code)
{
	// get table name
	QString tableName = removeFirst(code);
	if(tableName.toStdString().length() >= Block::STRLEN_MAX)
		throw MyError::NAME_TOOLONG;

	// remove brackets
	if(!chopBothEnds(code, "[\\s\\n]*\\([\\s\\n]*", "[\\s\\n]*\\)[\\s\\n]*"))
		throw MyError::SYNTAX_ERROR;
	// get attributes
	QVector<Attribute*> attributes;
	QSet<QString> names;

	QStringList infos = code.split(QRegExp("[\\s\\n]*,[\\s\\n]*"));
	QStringListIterator iter(infos);
	while(iter.hasNext()) try
	{
		QString nowInfo = iter.next();
		QString first = removeFirst(nowInfo);
		QString second = removeFirst(nowInfo);
		if(equals(first, "primary") && equals(second, "key"))
		{
			// get the primary key name
			if(!chopBothEnds(nowInfo, "[\\s\\n]*\\([\\s\\n]*", "[\\s\\n]*\\)[\\s\\n]*"))
				throw MyError::SYNTAX_ERROR;
			QString name = removeFirst(nowInfo);
			if(!nowInfo.isEmpty())
				throw MyError::SYNTAX_ERROR;

			// change the uniqueness
			bool find = false;
			QVectorIterator<Attribute*> i(attributes);
			while(i.hasNext())
			{
				Attribute* nowAttr = i.next();
				if(equals(nowAttr->getName(), name))
				{
					find = true;
					nowAttr->setUnique(true);
					break;
				}
			}
			if(!find)
				throw MyError::PRIMARY_KEY_ERROR;
		}
		else
		{
			// get name
			QString name = first;
			if(name.toStdString().length() >= Block::STRLEN_MAX)
				throw MyError::NAME_TOOLONG;
			else if(names.contains(name))
				throw MyError::DUPLICATED_NAME;
			else
				names.insert(name);

			// get isUnique
			bool isUnique = false;
			QRegExp uniqueExp("[\\s\\n]*unique[\\s\\n]*$");
			uniqueExp.setCaseSensitivity(Qt::CaseInsensitive);
			int uniqueStart = uniqueExp.indexIn(nowInfo);
			if(uniqueStart >= 0)
			{
				isUnique = true;
				nowInfo = nowInfo.left(uniqueStart);
			}

			// get data type
			Attribute::DataType type;
			if(nowInfo.isEmpty() && equals(second, "int"))
				type = Attribute::INT;
			else if(nowInfo.isEmpty() && equals(second, "float"))
				type = Attribute::FLOAT;
			else if(equals(second, "char"))
			{
				QRegExp numExp("[^0-9+]*\\([^0-9+]*([0-9]+)[^0-9+]*\\)[^0-9+]*");
				if(numExp.indexIn(nowInfo) < 0)
					throw MyError::DATATYPE_ERROR;
				QString numText = numExp.capturedTexts()[1];
				if(numText.length() > 5)
					throw MyError::DATATYPE_OUTOFBOUND;
				type = numText.toInt();
				if(type > 255 || type < 1)
					throw MyError::DATATYPE_OUTOFBOUND;
			}
			else
				throw MyError::DATATYPE_ERROR;

			// add the attribute to the table
			if(attributes.size() < 32)
				attributes.push_back(new Attribute(name, type, isUnique));
			else
				throw MyError::ATTR_OUTOFBOUND;
		}
	} catch (MyError::ErrorType e) {
		QVectorIterator<Attribute*> i(attributes);
		while(i.hasNext())
			delete i.next();

		throw std::move(e);
	}

	CreateTable* ret = new CreateTable(tableName, attributes);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getCreateIndex(QString code)
{
	// get index name
	QString indexName = removeFirst(code);
	if(!equals(removeFirst(code), "on"))
		throw MyError::SYNTAX_ERROR;
	// get table name
	QString tableName = removeFirst(code);
	// remove brackets
	if(!chopBothEnds(code, "[\\s\\n]*\\([\\s\\n]*", "[\\s\\n]*\\)[\\s\\n]*"))
		throw MyError::SYNTAX_ERROR;
	// get attribute name
	QString attriName = removeFirst(code);
	if(!code.isEmpty())
		throw MyError::SYNTAX_ERROR;

	CreateIndex* ret = new CreateIndex(indexName, tableName, attriName);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getDropTable(QString code)
{
	QString tableName = removeFirst(code);
	if(!code.isEmpty())
		throw MyError::SYNTAX_ERROR;

	DropTable* ret = new DropTable(tableName);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getDropIndex(QString code)
{
	QString indexName = removeFirst(code);
	if(!code.isEmpty())
		throw MyError::SYNTAX_ERROR;

	DropIndex* ret = new DropIndex(indexName);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getSelect(QString code)
{
	// get select attributes
	QRegExp fromExp("[\\s\\n]*from[\\s\\n]*");
	fromExp.setCaseSensitivity(Qt::CaseInsensitive);
	if(fromExp.indexIn(code) < 0)
		throw MyError::SYNTAX_ERROR;
	QStringList splitedCode = code.split(fromExp);
	QStringList select = splitedCode[0].split(QRegExp("[\\s\\n]*,[\\s\\n]*"));

	// get from tables
	code = splitedCode[1];
	QString condCode;
	QRegExp whereExp("[\\s\\n]*where[\\s\\n]*");
	whereExp.setCaseSensitivity(Qt::CaseInsensitive);
	if(whereExp.indexIn(code) >= 0)
	{
		splitedCode = code.split(whereExp);
		code = splitedCode[0];
		condCode = splitedCode[1];
	}
	QStringList from = code.split(QRegExp("[\\s\\n]*,[\\s\\n]*"));

	// get conditions
	QVector<Condition*> conditions;
	if(!condCode.isEmpty())
		conditions = parseConditions(condCode);

	Select* ret = new Select(select, from, conditions);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getDelete(QString code)
{
	// get table name
	if(!equals(removeFirst(code), "from"))
		throw MyError::SYNTAX_ERROR;
	QString tableName = removeFirst(code);

	// get conditions
	QVector<Condition*> conditions;
	if(code.startsWith("where", Qt::CaseInsensitive))
	{
		removeFirst(code);
		conditions = parseConditions(code);
	}
	else if(code.isEmpty())
		;
	else
		throw MyError::SYNTAX_ERROR;

	Delete* ret = new Delete(tableName, conditions);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getInsert(QString code)
{
	// get table name
	if(!equals(removeFirst(code), "into"))
		throw MyError::SYNTAX_ERROR;
	QString tableName = removeFirst(code);

	if(!equals(removeFirst(code), "values"))
		throw MyError::SYNTAX_ERROR;
	// remove brackets
	if(!chopBothEnds(code, "[\\s\\n]*\\([\\s\\n]*", "[\\s\\n]*\\)[\\s\\n]*"))
		throw MyError::SYNTAX_ERROR;
	// get values
	QStringList values = code.split(QRegExp("[\\s\\n]*,[\\s\\n]*"));
	for(int i=0; i<values.size(); i++)
		chopBothEnds(values[i], "[\\s\\n]*(?:\"|\')*[\\s\\n]*", "[\\s\\n]*(?:\"|\')*[\\s\\n]*");

	Insert* ret = new Insert(tableName, values);
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getQuit(QString code)
{
	if(!code.isEmpty())
		throw MyError::SYNTAX_ERROR;

	Quit* ret = new Quit();
//	qDebug() << *ret;
	return ret;
}

Statement* Interpreter::getExecFile(QString code)
{
	// remove quotes
	if(!chopBothEnds(code, "[\\s\\n]*(?:\"|\')[\\s\\n]*", "[\\s\\n]*(?:\"|\')[\\s\\n]*$"))
		throw MyError::SYNTAX_ERROR;

	ExecFile* ret = new ExecFile(code);
//	qDebug() << *ret;
	return ret;
}



