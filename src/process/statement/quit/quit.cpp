#include "quit.h"

Statement::ExecState Quit::exec()
{
	return EXEC_QUIT;
}

QDebug operator<<(QDebug debug, const Quit&)
{
	debug << "quit" << endl;
	return debug;
}
