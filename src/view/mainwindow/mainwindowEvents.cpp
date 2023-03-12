#include "mainwindow.h"
#include "../../process/myerror/myerror.h"
#include <QTime>

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) {

		updateDBFiles();

		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

void MainWindow::newFile()
{
	if (maybeSave()) {
		textEdit->getWidget()->clear();
		setCurrentFile(QString());
		console->clearText();
		table->clearTable();
	}
}

void MainWindow::help()
{
   helpWindow->show();
}

void MainWindow::documentWasModified()
{
	setWindowModified(textEdit->getWidget()->document()->isModified());
}

void MainWindow::updateLineNumbers()
{
	int lines = textEdit->getLines();
	QTextCursor cursor = textEdit->getWidget()->textCursor();
	int nowLine = cursor.blockNumber();
	lineNumbers->setLines(lines, nowLine);
}

void MainWindow::updateScrollPosition()
{
	lineNumbers->updateScrollPosition();
}

void MainWindow::execSQL()
{
	// clear and refresh the console
	console->clearText();
	Sleep(100);
	QString filepath = curFile.isEmpty()?
		"untitle.txt": QFileInfo(curFile).absoluteFilePath();

	QString codeText = textEdit->getCodeText();
	QTime time;
	time.start();

	// execute SQL statements
	QString errMessage;
	try {
		api->execSQL(codeText);
	} catch (MyError e) {
		errMessage = e.getErrorInfo();
	}

	// update result
	tableView->updateTableView();
	table->update(api->getViewHeader(), api->getViewRecords());
	// update console
	console->printFilePath(filepath);
	console->appendRunningTime(time.elapsed());
	if(errMessage.isEmpty())
		console->appendMessage("SQL语句执行成功。");
	else {
		console->appendMessage(errMessage);
		console->appendHelpInfo();
	}
}

void MainWindow::updateDBFiles()
{
	// clear and refresh the console
	console->clearText();
	Sleep(100);
	// print info
	console->appendMessage("正在将内存中数据写入硬盘...");
	console->getWidget()->repaint();
	Sleep(300);

	// save buffer pages to file
	api->storePages();

	console->appendMessage("数据同步成功。");
	console->getWidget()->repaint();
	Sleep(300);
}
