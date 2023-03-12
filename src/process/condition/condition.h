#ifndef CONDITION_H_
#define CONDITION_H_

#include "../attribute/attribute.h"
#include <QString>
#include <QSet>

class Condition
{
public:
	enum Compare{
		EQUAL,
		NOT_EQUAL,
		LESS_THAN,
		LARGER_THAN,
		NOT_LESS_THAN,
		NOT_LARGER_THAN
	};
private:
	QString attrName;
	QString value;
	Compare compare;

	// whether right value is an attribute
	bool rValueAttr;
	// which table this attribute in
	QString tableName;
public:
	// constructors
	Condition(QString attrName, QString value, Compare compare):
		attrName(attrName), value(value), compare(compare), rValueAttr(false) {}
	Condition(QString attrName, int value, Compare compare):
		Condition(attrName, QString::number(value), compare) {}
	Condition(QString attrName, double value, Compare compare):
		Condition(attrName, QString::number(value), compare) {}

	// getters
	QString getAttrName() const { return attrName; }
	QString getValue() const { return value; }
	Compare getCompare() const { return compare; }
	bool isRValueAttr() const { return rValueAttr; }
	QString getTableName() const { return tableName; }
	// setters
	void setRValueAttr(bool isAttr) { rValueAttr = isAttr; }
	void setTableName(QString name) { tableName = name; }

	// static judge function
	static bool meets(const QStringList& attrNames, const QVector<Attribute::DataType>& dataTypes,
					  const QStringList& record, const QVector<Condition*>& conditions);
	// judge for this instance
	bool meets(const QStringList& attrNames, const QVector<Attribute::DataType>& dataTypes,
			   const QStringList& record) const;
	// get satisfied record index
	QSet<QPair<int, int>>* getSatisfied() const;

private:
	template<class T>
	bool CompareExec(const T &leftValue, const T& rightValue) const
	{
		bool ret = false;
		if((compare == EQUAL			&& leftValue == rightValue)
		|| (compare == NOT_EQUAL		&& leftValue != rightValue)
		|| (compare == LESS_THAN		&& leftValue <  rightValue)
		|| (compare == LARGER_THAN		&& leftValue >  rightValue)
		|| (compare == NOT_LESS_THAN	&& leftValue >= rightValue)
		|| (compare == NOT_LARGER_THAN	&& leftValue <= rightValue))
			ret = true;
		return ret;
	}
};

#endif
