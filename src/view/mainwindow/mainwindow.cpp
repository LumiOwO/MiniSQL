/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"

MainWindow::MainWindow()
{
	// get font
	QFont myFont;
	int index = QFontDatabase::addApplicationFont(":/font/consya.ttf");
	if(index != -1)
	{
		QStringList fontList(QFontDatabase::applicationFontFamilies(index));
		if(fontList.count() > 0)
			myFont = QFont(fontList.at(0));
	}
	tableView = new MyTableView;
	textEdit = new MyCodeText(myFont);
	console = new MyConsole(myFont);
	lineNumbers = new MyLineNum(textEdit->getWidget()->verticalScrollBar(), myFont);
	table = new MyTable;
	helpWindow = new MyHelpWindow(myFont);
	api = API::getInstance(this);

	tableView->updateTableView();

	// layout management
	QWidget *field = new QWidget;
	QGridLayout *layout = new QGridLayout;

	QWidget* widgetsTemp[] = {
		tableView->getWidget(),
		lineNumbers->getWidget(),
		textEdit->getWidget(),
		table->getTable(),
	};
	int size = sizeof(widgetsTemp) / sizeof(widgetsTemp[0]);

	for(int i=0; i<size; i++)
		layout->addWidget(widgetsTemp[i], 0, i, 1, 1);

	layout->addWidget(console->getWidget(), 1, 0, 1, size);
	
	layout->setSpacing(0);
	layout->setMargin(0);
	// row stretch
	layout->setRowStretch(0, 2);
	layout->setRowStretch(1, 1);
	// column stretch
	layout->setColumnStretch(0, 4);
	layout->setColumnStretch(2, 15);
	layout->setColumnStretch(3, 13);
	// set layout
	field->setLayout(layout);
	setCentralWidget(field);

	// create menu and toolbars
	createActions();
	// create status bar
	createStatusBar();

	readSettings();

	// set highlight
	MyHighlighter *highlighter = new MyHighlighter(textEdit->getWidget()->document());
	Q_ASSERT(highlighter != NULL);

	// lines synchronize
	connect(textEdit->getWidget()->document(), &QTextDocument::contentsChanged,
			this, &MainWindow::updateLineNumbers);
	connect(textEdit->getWidget(), &QPlainTextEdit::cursorPositionChanged,
			this, &MainWindow::updateLineNumbers, Qt::QueuedConnection);
	// scroll synchronize
	connect(textEdit->getWidget()->verticalScrollBar(), SIGNAL(valueChanged(int)),
			this, SLOT(updateScrollPosition()), Qt::QueuedConnection);

	connect(textEdit->getWidget()->document(), &QTextDocument::contentsChanged,
			this, &MainWindow::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
	QGuiApplication::setFallbackSessionManagementEnabled(false);
	connect(qApp, &QGuiApplication::commitDataRequest,
			this, &MainWindow::commitData);
#endif

	setCurrentFile(QString());
	setUnifiedTitleAndToolBarOnMac(true);

	// resize window
	this->resize(QSize(WIN_WIDTH, WIN_HEIGHT));
}

MainWindow::~MainWindow()
{
	delete textEdit->getWidget();
	delete console;
	delete table;
	api->free();
}

void MainWindow::readSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
	if (geometry.isEmpty()) {
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			 (availableGeometry.height() - height()) / 2);
	} else {
		restoreGeometry(geometry);
	}
}

void MainWindow::writeSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry", saveGeometry());
}

QString MainWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
	if (manager.allowsInteraction()) {
		if (!maybeSave())
			manager.cancel();
	} else {
		// Non-interactive: save without asking
		if (textEdit->getWidget()->document()->isModified())
			save();
	}
}
#endif
