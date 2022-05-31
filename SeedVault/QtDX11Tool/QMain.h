#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QMain.h"
class QRenderWidget;
class QMain : public QMainWindow
{
	Q_OBJECT

public:
	QMain(QWidget* parent = Q_NULLPTR);
	~QMain();

public slots:
	void initialize(bool success);
	void tick();
	void render();

	void createNewFile();
	void openFile();
	void saveFile();
	void helpMessage();


signals:
	void calledConstructor();

	void createdNewFile();
	void openedFile(const QString& filepath);
	void savedFile(const QString& filepath);


private:
	QString m_CurrentFilePath;
public:
	QRenderWidget* m_RenderWidget;
	Ui::QMainClass ui;
};
