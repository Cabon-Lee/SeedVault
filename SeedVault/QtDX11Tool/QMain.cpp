#include "pch.h"
#include "QMain.h"

#include "QRenderWidget.h"

QMain::QMain(QWidget *parent)
    : QMainWindow(parent)
	, m_RenderWidget(new QRenderWidget)
{
	ui.setupUi(this);

	ui.actionNew_File->setShortcut(tr("Ctrl+N"));
	ui.actionOpen_File->setShortcut(tr("Ctrl+O"));
	ui.actionSave->setShortcut(tr("Ctrl+S"));
	ui.actionHelp->setShortcut(tr("Ctrl+H"));

	connect(ui.actionNew_File,		SIGNAL(triggered()), this, SLOT(createNewFile()));
	connect(ui.actionOpen_File,		SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionSave,			SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(ui.actionHelp,			SIGNAL(triggered()), this, SLOT(helpMessage()));

    setCentralWidget(m_RenderWidget);

	emit calledConstructor();
}

QMain::~QMain()
{

}

void QMain::initialize(bool success)
{

}

void QMain::tick()
{

}

void QMain::render()
{

}

void QMain::createNewFile()
{
	QMessageBox::information(this, "Create Scene", "Create Scene");

	emit createdNewFile();
}

void QMain::openFile()
{
	static QString path = QDir::currentPath();
	
	QString strFilter = "json file (*.json) ;; text file (*.txt) ;; All files (*.*)";
	QString strFileName = QFileDialog::getOpenFileName(this, "Open a file", path, strFilter);

	m_CurrentFilePath = strFileName;

	emit openedFile(strFileName);
}

void QMain::saveFile()
{
	static QString path = QDir::currentPath();

	const QString strFilePath = QFileDialog::getExistingDirectory(this, "Open a directory", path);
	path = strFilePath;

	emit savedFile(strFilePath);
}

void QMain::helpMessage()
{
	QMessageBox::information(this, "Help", 
		"Create New Scene - [Ctr+N]\nOpen Scene(json) - [Ctr+O]\nSave Scene(json) - [Ctr+S]\nHelp - [Ctr+H]\nCamera Move [W,A,S,D]");
}
