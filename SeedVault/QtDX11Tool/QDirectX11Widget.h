#pragma once

#include <QFrame>
#include "ui_QDirectX11Widget.h"

class SceneBase;
class QT::Input;
class QRenderWidget;

class QDirectX11Widget : public QFrame
{
	Q_OBJECT

public:
	QDirectX11Widget(QWidget* parent = Q_NULLPTR);
	~QDirectX11Widget();

private:
	Ui::QDirectX11Widget ui;

public:
	bool initialize();
	void tick();
	void run();
	void render();

	void continueFrames();
	void pauseFrames();
	
	void SetRenderWidget(QRenderWidget* renderWidget);

	QTimer m_qTimer;
private slots:
	void OnFrame();
	void OnReset();


private:
	bool m_bDeviceInitialized;
	bool m_bRenderActive;
	std::unique_ptr<QT::Input> m_pInput;
	HWND m_hWnd;
	QRenderWidget* m_pRenderWidget;

public:
	//시그널은 구현하지않는다.
signals:
	void deviceInitialized(bool success);
	void eventHandled();
	void widgetResized();
	void ticked();
	void rendered();
	void getScene(SceneBase* scene);

	void keyPressed(QKeyEvent* event);
	void mouseMoved(QMouseEvent* event);
	void mouseClicked(QMouseEvent* event);
	void mouseReleased(QMouseEvent* event);

private:
	bool			event(QEvent* event) override;
	void			showEvent(QShowEvent* event) override;
	QPaintEngine*	paintEngine() const override;
	void			paintEvent(QPaintEvent* event) override;
	void			resizeEvent(QResizeEvent* event) override;
	void			wheelEvent(QWheelEvent* event) override;
	/*#if QT_VERSION >= 0x050000
		bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
	#else
		bool winEvent(MSG* message, long* result) override;
	#endif*/

	LRESULT WINAPI WndProc(MSG* pMsg);

};
