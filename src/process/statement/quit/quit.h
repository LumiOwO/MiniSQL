#ifndef QUIT_H_
#define QUIT_H_

#include "../statement.h"
#include <QString>

class Quit: public Statement
{
public:
	Quit() {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const Quit& info);
};


#endif
