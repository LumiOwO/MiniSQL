#include "mytextfield.h"

void MyTextField::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;

	menu->addAction(
		"剪切(T)", this, SLOT(cut()), QKeySequence::Cut
	);

	menu->addAction(
		"复制(C)", this, SLOT(copy()), QKeySequence::Copy
	);

	QAction *paste = menu->addAction(
		"粘贴(P)", this, SLOT(paste()), QKeySequence::Paste
	);
	paste->setEnabled(canPaste());

	menu->addSeparator();

	menu->addAction(
		"全选(A)", this, SLOT(selectAll()), QKeySequence::SelectAll
	);

	menu->addSeparator();

	menu->addAction(
		"撤销(U)", this, SLOT(undo()), QKeySequence::Undo
	);

	menu->addAction(
		"重做(R)", this, SLOT(redo()), QKeySequence::Redo
	);

	menu->exec(event->globalPos());
	delete menu;
}
