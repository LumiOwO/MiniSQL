#ifndef EXECFILE_H_
#define EXECFILE_H_

#include "../statement.h"
#include <QString>

class ExecFile: public Statement
{
private:
	QString filename;
public:
	ExecFile(QString filename):
		filename(filename) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const ExecFile& info);
};


#endif
