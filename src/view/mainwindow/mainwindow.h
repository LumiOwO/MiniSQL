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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "../mytable/mytable.h"
#include "../mycodetext/mycodetext.h"
#include "../myconsole/myconsole.h"
#include "../mylinenum/mylinenum.h"
#include "../myhighlighter/myhighlighter.h"
#include "../myhelpwindow/myhelpwindow.h"
#include "../myTableView/myTableView.h"

#include "../../process/api/api.h"

#define WIN_WIDTH	1800
#define WIN_HEIGHT	900
#define WIN_POS_X	50
#define WIN_POS_Y	50

#define DesktopPath QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QTableWidget;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow() override;

	void loadFile(const QString &fileName);

protected:
	void closeEvent(QCloseEvent *event) override;

private slots:
	void newFile();
	void open();
	bool save();
	bool saveAs();
	void help();
	void documentWasModified();
	void updateLineNumbers();
	void updateScrollPosition();

	void execSQL();
	void updateDBFiles();
#ifndef QT_NO_SESSIONMANAGER
	void commitData(QSessionManager &);
#endif

private:
	void createActions();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	bool saveFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);

	void createFileMenu();
	void createEditMenu();
	void createSQLMenu();
	void createHelpMenu();

	MyTableView *tableView;
	MyCodeText *textEdit;
	MyConsole *console;
	MyLineNum *lineNumbers;
	MyTable *table;
	MyHelpWindow *helpWindow;
	QString curFile;

	API *api;

};
//! [0]

#endif
