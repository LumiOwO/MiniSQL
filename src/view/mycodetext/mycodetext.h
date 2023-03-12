#ifndef MYCODETEXT_H
#define MYCODETEXT_H

#include <QString>
#include "../mytextfield/mytextfield.h"

class MyCodeText
{
private:
	MyTextField *textEdit;
public:
	MyCodeText(QFont font);
	~MyCodeText();
	MyTextField *getWidget();
	int getLines();
	QString getCodeText();
};

#endif // MYCODETEXT_H
