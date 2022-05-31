#include "pch.h"
#include "QRenderWidget.h"

QRenderWidget::QRenderWidget(QWidget *parent)
	: QWidget(parent)
	, m_pDirectXWidget(nullptr)
{
	ui.setupUi(this);

	m_pDirectXWidget =ui.frame;
	ui.tab_SceneController->SetObjectController(ui.tab_ObjectController);
	connectSlot();
}

QRenderWidget::~QRenderWidget()
{
}

void QRenderWidget::initialize(bool success)
{
	if (!success)
	{
		QMessageBox::critical(this, "ERROR", "Direct3D widget initialization failed.",
			QMessageBox::Ok);
		return;
	}

	// TODO: Add here your extra initialization here.
	// ...
	// Start processing frames with a short delay in case things are still initializing/loading
	// in the background.
	QTimer::singleShot(500, this, [&] { m_pDirectXWidget->run(); });
	disconnect(m_pDirectXWidget, &QDirectX11Widget::deviceInitialized, this, &QRenderWidget::initialize);
}

void QRenderWidget::tick()
{

}

void QRenderWidget::render()
{

}

void QRenderWidget::connectSlot()
{
	connect(m_pDirectXWidget, &QDirectX11Widget::deviceInitialized, this, &QRenderWidget::initialize);

	connect(m_pDirectXWidget, &QDirectX11Widget::ticked, this, &QRenderWidget::tick);
	connect(m_pDirectXWidget, &QDirectX11Widget::rendered, this, &QRenderWidget::render);

	connect(m_pDirectXWidget, &QDirectX11Widget::getScene, ui.tab_SceneController, &QSceneController::setFirstScene);

}
