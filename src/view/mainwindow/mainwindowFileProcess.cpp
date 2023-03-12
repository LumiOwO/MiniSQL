#include "mainwindow.h"

bool MainWindow::maybeSave()
{
	if (!textEdit->getWidget()->document()->isModified())
		return true;
	const int ret = QMessageBox::warning(
		this,
		tr("提示"),								// title
		tr("当前文档尚未保存。\n"
		   "想要保存当前的修改吗?"),			// text
		tr("保存"), tr("不保存"), tr("取消"),	// button names
		0										// default button number
	);
	switch (ret) {
	case 0:
		return save();
	case 1:
		return true;
	case 2:
		return false;
	}
	return true;
}

void MainWindow::open()
{
	if (maybeSave())
	{
		QString path = curFile.isEmpty()? DesktopPath: curFile;
		QString fileName = QFileDialog::getOpenFileName(
			this,
			tr("打开文件"),						// dialog title
			path,								// begin path
			tr("SQL代码 (*.sql);;"				// options
			   "文本文件 (*.txt);;"				// separate by ";;"
			   "所有文件 (*)")
		);
		if (!fileName.isEmpty())
		{
			loadFile(fileName);
			console->clearText();
			table->clearTable();
		}
	}
}

bool MainWindow::save()
{
	if (curFile.isEmpty()) {
		return saveAs();
	} else {
		return saveFile(curFile);
	}
}

bool MainWindow::saveAs()
{
	QFileDialog dialog(this, "当前文本另存为");
	QStringList filters;
	filters << "SQL代码 (*.sql)"
			<< "文本文件 (*.txt)"
			<< "所有文件 (*)";
	dialog.setNameFilters(filters);
	QString path = curFile.isEmpty()? DesktopPath: curFile;
	dialog.setDirectory(path);
	dialog.setDefaultSuffix(".");
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if (dialog.exec() != QDialog::Accepted)
		return false;
	return saveFile(dialog.selectedFiles().first());
}

void MainWindow::loadFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("读取文件"),
							 tr("无法读取文件 %1:\n%2.")
							 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
		return;
	}

	QTextStream in(&file);
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
	textEdit->getWidget()->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif

	QFileInfo info(file);
	setCurrentFile(info.absoluteFilePath());
	statusBar()->showMessage(tr("文件已读取"), 2000);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	textEdit->getWidget()->document()->setModified(false);
	setWindowModified(false);

	QString shownName = curFile;
	if (curFile.isEmpty())
		shownName = "untitled.txt";
	shownName += "[*] - MiniSQLV1.0";
	setWindowFilePath(shownName);
	setWindowTitle(shownName);
}

bool MainWindow::saveFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("保存文件"),
							 tr("无法保存文件 %1:\n%2.")
							 .arg(QDir::toNativeSeparators(fileName),
								  file.errorString()));
		return false;
	}

	QTextStream out(&file);
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
	out << textEdit->getWidget()->toPlainText();
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif

	QFileInfo info(file);
	setCurrentFile(info.absoluteFilePath());
	statusBar()->showMessage(tr("文件已保存"), 2000);
	return true;
}
