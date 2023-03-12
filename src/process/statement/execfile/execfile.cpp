#include "execfile.h"
#include "../../api/api.h"
#include "../../myerror/myerror.h"
#include <fstream>
using namespace std;

Statement::ExecState ExecFile::exec()
{
	fstream in(filename.toStdString(), ios::in);
	if(!in)
		throw MyError::FILE_NOTFOUND;
	QString text;
	char buffer;
	while(in.get(buffer))
		text.push_back(buffer);
	in.close();

	qDebug() << text;

	try {
		API::getInstance()->execSQL(text);
	} catch (MyError e) {
		throw std::move(e);
	}
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const ExecFile& info)
{
	debug << "execfile: " << info.filename << endl;
	return debug;
}
