#pragma once

#include <QWidget>
#include "ui_QRenderWidget.h"

class QDirectX11Widget;
class QRenderWidget : public QWidget
{
	Q_OBJECT

public:
	QRenderWidget(QWidget *parent = Q_NULLPTR);
	~QRenderWidget();

	Ui::QRenderWidget ui;
private:

	QDirectX11Widget* m_pDirectXWidget;

public slots:
	void initialize(bool success);
	void tick();
	void render();

private:
	void connectSlot();

};
