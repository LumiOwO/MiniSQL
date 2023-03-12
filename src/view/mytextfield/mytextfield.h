#ifndef MYTEXTFIELD_H_
#define MYTEXTFIELD_H_

#include <QPlainTextEdit>
#include <QMenu>

class MyTextField: public QPlainTextEdit
{
protected:
	void contextMenuEvent(QContextMenuEvent *event);
};

#endif // MYCONSOLE_H
