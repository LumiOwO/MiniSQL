#include "myerror.h"

MyError::MyError(ErrorType errorType, int lineNum, QString errorStatement):
	errorType(errorType), lineNumber(lineNum),
	errorStatement(errorStatement), subError(nullptr)
{

}

MyError::MyError(const MyError& error):
	errorType(error.errorType), lineNumber(error.lineNumber),
	errorStatement(error.errorStatement), subError(nullptr)
{
	if(error.subError)
		this->subError = new MyError(*error.subError);
}

QString MyError::getErrorInfo()
{
	QString ret;
	if(errorType == EMPTY_TEXT)
		ret = "当前文档无内容。";
	else if(errorType == UNKNOWN_STATEMENT)
		ret = "输入了未定义或不支持的关键词。";
	else if(errorType == SYNTAX_ERROR)
		ret = "输入的语句语法有误，不符合格式要求。";
	else if(errorType == PRIMARY_KEY_ERROR)
		ret = "主键定义错误，表中没有该属性名称。";
	else if(errorType == DATATYPE_ERROR)
		ret = "输入了不支持的数据类型，或语句的格式有误。";
	else if(errorType == DUPLICATED_NAME)
		ret = "同一张表中不能出现相同的属性名。";
	else if(errorType == DATATYPE_OUTOFBOUND)
		ret = "字符串的长度必须在[1, 255]的闭区间内。";
	else if(errorType == UNKNOWN_COMPARE)
		ret = "输入了未知的比较符号。";
	else if(errorType == CONDITION_INCOMPLETE)
		ret = "输入的比较条件不完整。";
	else if(errorType == ATTR_OUTOFBOUND)
		ret = "定义的属性过多，一张表最多只能定义32个属性。";
	else if(errorType == TABLE_EXIST)
		ret = "该表在数据库中已存在，不能再次定义该表。";
	else if(errorType == TABLE_NOTFOUND)
		ret = "该表在数据库中不存在。";
	else if(errorType == INSERT_ATTRINUM_ERROR)
		ret = "插入值的个数与表格的属性个数不相符。";
	else if(errorType == INSERT_DATATYPE_ERROR)
		ret = "某个插入值的数据类型不匹配。";
	else if(errorType == INSERT_STRING_TOOLONG)
		ret = "某个字符串值的长度大于属性中定义的长度。";
	else if(errorType == NAME_TOOLONG)
		ret = "定义的名称长度必须小于20字节。";
	else if(errorType == JOIN_ERROR)
		ret = "要查询的表中存在空表。该程序不支持空表的查询。";
	else if(errorType == JOIN_ITSELT)
		ret = "该程序不支持连接时出现名称相同的表。";
	else if(errorType == ATTR_NOTFOUND)
		ret = "输入了未定义的属性名。";
	else if(errorType == ATTR_AMBIGUOUS)
		ret = "多张表中有同名属性，请声明需要哪张表中的属性，如table.attr。";
	else if(errorType == ATTR_NOTFOUND_COND)
		ret = "条件中输入了未定义的属性名";
	else if(errorType == ATTR_AMBIGUOUS_COND)
		ret = "多张表中有同名属性，请在条件中声明需要哪张表中的属性，如table.attr = value。";
	else if(errorType == COND_TYPEERROR)
		ret = "条件中用于比较的值不符合表中定义的数据类型。";
	else if(errorType == FILE_NOTFOUND)
		ret = "无法打开该语句中的文件或文件不存在。";
	else if(errorType == EXECFILE_ERROR)
		ret = "所执行的文件中有语法错误，错误信息将在下方递归地列出。";
	else if(errorType == UNIQUE_DUPLICATE)
		ret = "对于表中某个声明为unique的属性，想要插入的值已经在表中存在。";
	else if(errorType == ATTR_NOTUNIQUE)
		ret = "该属性没有声明为unique。不能创建索引。";
	else if(errorType == INDEX_DUPLICATENAME)
		ret = "该索引名已存在，不能定义相同的索引名称。";
	else if(errorType == INDEX_DUPLICATEATTR)
		ret = "该属性已构建索引，一个属性不能对应多个索引。";
	else if(errorType == INDEX_NOTFOUND)
		ret = "该索引在数据库中不存在。";
	else
		ret = "发生了一个预料之外的错误。";

	if(!errorStatement.isEmpty())
		ret = "第 " + QString::number(lineNumber) + " 行: "
			  + ret + "\n" + errorStatement;
	ret = subError? ret + "\n" + subError->getErrorInfo(): ret;
	return ret;
}
