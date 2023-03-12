#include "mainwindow.h"

void MainWindow::createActions()
{
	createFileMenu();
	createEditMenu();
	createSQLMenu();
	createHelpMenu();
}

void MainWindow::createFileMenu()
{
	// file menu
	QMenu *fileMenu = menuBar()->addMenu(tr("&文件(F)"));
	QToolBar *fileToolBar = addToolBar(tr("文件工具"));
	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/icons/new.png"));
	QAction *newAct = new QAction(newIcon, tr("&新建(N)"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("创建新文件"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
	fileMenu->addAction(newAct);
	fileToolBar->addAction(newAct);

	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/icons/open.png"));
	QAction *openAct = new QAction(openIcon, tr("&打开(O)..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("打开文件"));
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);
	fileToolBar->addAction(openAct);

	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/icons/save.png"));
	QAction *saveAct = new QAction(saveIcon, tr("&保存(S)"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("保存当前文本"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);
	fileToolBar->addAction(saveAct);

	const QIcon saveAsIcon = QIcon::fromTheme("document-save-as", QIcon(":/icons/saveas.png"));
	QAction *saveAsAct = new QAction(saveAsIcon, tr("&另存为(A)..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("将当前文本另存为..."));
	connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(saveAsAct);
	fileToolBar->addAction(saveAsAct);

	fileMenu->addSeparator();

	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction *exitAct = fileMenu->addAction(exitIcon, tr("&退出程序(X)"), this, &QWidget::close);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("退出程序"));
}

void MainWindow::createEditMenu()
{
	//edit menu
	QMenu *editMenu = menuBar()->addMenu(tr("&编辑(E)"));
	QToolBar *editToolBar = addToolBar(tr("编辑工具"));
#ifndef QT_NO_CLIPBOARD
	const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/icons/cut.png"));
	QAction *cutAct = new QAction(cutIcon, tr("&剪切(T)"), this);

	cutAct->setShortcuts(QKeySequence::Cut);
	cutAct->setStatusTip(tr("剪切"));
	connect(cutAct, &QAction::triggered, textEdit->getWidget(), &QPlainTextEdit::cut);
	editMenu->addAction(cutAct);
	editToolBar->addAction(cutAct);

	const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/icons/copy.png"));
	QAction *copyAct = new QAction(copyIcon, tr("&复制(C)"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("复制"));
	connect(copyAct, &QAction::triggered, textEdit->getWidget(), &QPlainTextEdit::copy);
	editMenu->addAction(copyAct);
	editToolBar->addAction(copyAct);

	const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/icons/paste.png"));
	QAction *pasteAct = new QAction(pasteIcon, tr("&粘贴(P)"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("粘贴"));
	connect(pasteAct, &QAction::triggered, textEdit->getWidget(), &QPlainTextEdit::paste);
	editMenu->addAction(pasteAct);
	editToolBar->addAction(pasteAct);

	QAction *selectAllAct = new QAction(tr("&全选(A)"), this);
	selectAllAct->setShortcuts(QKeySequence::SelectAll);
	selectAllAct->setStatusTip(tr("全选"));
	connect(selectAllAct, &QAction::triggered, textEdit->getWidget(), &QPlainTextEdit::selectAll);
	editMenu->addAction(selectAllAct);

	menuBar()->addSeparator();
	editMenu->addSeparator();

	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/icons/undo.png"));
	QAction *undoAct = new QAction(undoIcon, tr("&撤销(U)"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("撤销"));
	connect(undoAct, &QAction::triggered, textEdit->getWidget(), &QPlainTextEdit::undo);
	editMenu->addAction(undoAct);
	editToolBar->addAction(undoAct);

	const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(":/icons/redo.png"));
	QAction *redoAct = new QAction(redoIcon, tr("&重做(R)"), this);
	redoAct->setShortcuts(QKeySequence::Redo);
	redoAct->setStatusTip(tr("重做"));
	connect(redoAct, &QAction::triggered, textEdit->getWidget(), &QPlainTextEdit::redo);
	editMenu->addAction(redoAct);
	editToolBar->addAction(redoAct);

	cutAct->setEnabled(false);
	copyAct->setEnabled(false);
	connect(textEdit->getWidget(), &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
	connect(textEdit->getWidget(), &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);

	undoAct->setEnabled(false);
	redoAct->setEnabled(false);
	connect(textEdit->getWidget(), &QPlainTextEdit::undoAvailable, undoAct, &QAction::setEnabled);
	connect(textEdit->getWidget(), &QPlainTextEdit::redoAvailable, redoAct, &QAction::setEnabled);

#endif // !QT_NO_CLIPBOARD
}

void MainWindow::createSQLMenu()
{
	// SQL menu
	QMenu *SQLMenu = menuBar()->addMenu(tr("&语句(SQL)"));
	QToolBar *SQLToolBar = addToolBar(tr("SQL语句工具"));
	const QIcon execIcon = QIcon::fromTheme("SQL-exec", QIcon(":/icons/run.png"));
	QAction *execAct = new QAction(execIcon, tr("&执行SQL语句"), this);
	execAct->setShortcut(QKeySequence(tr("Ctrl+R")));
	execAct->setStatusTip(tr("执行SQL语句"));
	connect(execAct, &QAction::triggered, this, &MainWindow::execSQL);
	SQLMenu->addAction(execAct);
	SQLToolBar->addAction(execAct);

	const QIcon updataIcon = QIcon::fromTheme("SQL-update", QIcon(":/icons/update.png"));
	QAction *updateAct = new QAction(updataIcon, tr("&将内存中的缓冲块数据同步到硬盘数据中"), this);
	updateAct->setShortcut(QKeySequence(tr("Ctrl+T")));
	updateAct->setStatusTip(tr("将内存中的缓冲块数据同步到硬盘数据中"));
	connect(updateAct, &QAction::triggered, this, &MainWindow::updateDBFiles);
	SQLMenu->addAction(updateAct);
	SQLToolBar->addAction(updateAct);
}

void MainWindow::createHelpMenu()
{
	// help menu
	QMenu *helpMenu = menuBar()->addMenu(tr("&帮助(H)"));
	QAction *aboutAct = helpMenu->addAction(tr("&帮助"), this, &MainWindow::help);
	aboutAct->setStatusTip(tr("获取帮助信息"));

	QAction *aboutQtAct = helpMenu->addAction(tr("关于 &Qt"), qApp, &QApplication::aboutQt);
	aboutQtAct->setStatusTip(tr("关于Qt"));
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("准备就绪"));
}
