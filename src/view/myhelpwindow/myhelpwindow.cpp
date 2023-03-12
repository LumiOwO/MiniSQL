#include "myhelpwindow.h"

MyHelpWindow::MyHelpWindow(QFont font):
	helpWindow(new QMainWindow),
	helpTextField(new MyTextField),
	button(new QDialogButtonBox(QDialogButtonBox::Close))
{
	// get help text
	QFile helpFile(":/text/help.txt");
	helpFile.open(QFile::ReadOnly);
	QTextStream in(&helpFile);
	QString helpText = in.readAll();
	helpFile.close();
	helpTextField->setPlainText(helpText);
	// set text background
	QPalette palette;
	palette.setColor(QPalette::Base, QColor(240, 240, 240));
	helpTextField->setPalette(palette);
	// set text font
	font.setPixelSize(20);
	helpTextField->setFont(font);
	// text setting
	helpTextField->setReadOnly(true);
	helpTextField->setFrameShape(QFrame::NoFrame);
	// button setting
	button->button(QDialogButtonBox::Close)->setText("确定");
	QObject::connect(button->button(QDialogButtonBox::Close), &QPushButton::clicked, helpWindow, &QMainWindow::close);
	// layout setting
	QWidget *centerWidget = new QWidget;
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(helpTextField, 0, 0);
	layout->addWidget(button, 1, 0);
	centerWidget->setLayout(layout);
	// window setting
	helpWindow->setCentralWidget(centerWidget);
	helpWindow->setWindowTitle("帮助");
	helpWindow->setFixedSize(HELPWIN_WIDTH, HELPWIN_HEIGHT);
	helpWindow->setWindowFlags(
		helpWindow->windowFlags() &
		~Qt::WindowMaximizeButtonHint &
		~Qt::WindowMinimizeButtonHint
	);
}

MyHelpWindow::~MyHelpWindow()
{
	delete helpWindow;
	delete helpTextField;
	delete button;
}

void MyHelpWindow::show()
{
	helpTextField->verticalScrollBar()->setSliderPosition(0);
	helpWindow->show();
}
