#include "pch.h"
#include "QDirectX11Widget.h"
#include "QtScene00.h"


#include "QMain.h"
#include "QRenderWidget.h"
#include "QObjectController.h"
constexpr int FPS_LIMIT = 60.0f;
constexpr int MS_PER_FRAME = (int)((1.0f / FPS_LIMIT) * 1000.0f);

extern QMain* g_MainWindow;

QDirectX11Widget::QDirectX11Widget(QWidget* parent)
	: QFrame(parent)
	, m_bDeviceInitialized(false)
	, m_bRenderActive(false)
	, m_pInput(std::make_unique<QT::Input>())
	, m_hWnd(reinterpret_cast<HWND>(this->winId()))
	, m_pRenderWidget(nullptr)
{
	ui.setupUi(this);

	setAutoFillBackground(false);

	setFocusPolicy(Qt::ClickFocus);

	setAttribute(Qt::WA_NativeWindow);
	// Setting these attributes to our widget and returning null on paintEngine event
	// tells Qt that we'll handle all drawing and updating the widget ourselves.
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
}

QDirectX11Widget::~QDirectX11Widget()
{
}

bool QDirectX11Widget::initialize()
{
	DLLEngine::CreateEngine();
	DLLEngine::ActiveImgui(false);
	DLLEngine::Initialize(reinterpret_cast<int>(m_hWnd), this->width(), this->height());
	QtScene00* temp = new QtScene00();


	DLLEngine::SetSceneName(temp, "Test");
	DLLEngine::AddScene("Test", temp);
	DLLEngine::SelectScene("Test");

	/// <summary>
	///임시 확인용
	/// </summary>
	/// <returns></returns>
	emit getScene(temp);

	m_bDeviceInitialized = true;
	m_bRenderActive = true;

	connect(&m_qTimer, &QTimer::timeout, this, &QDirectX11Widget::OnFrame);
	connect(g_MainWindow->m_RenderWidget->ui.tab_ObjectController, SIGNAL(addedLight(const QString&)), g_MainWindow->m_RenderWidget->ui.tab_LightController, SLOT(addLightList(const QString&)));
	connect(g_MainWindow->m_RenderWidget->ui.tab_ObjectController, SIGNAL(deletedLight(const QString&)), g_MainWindow->m_RenderWidget->ui.tab_LightController, SLOT(deleteLightList(const QString&)));
	connect(g_MainWindow->m_RenderWidget->ui.tab_ObjectController, SIGNAL(editedObjectName(const QString& , const QString&)), g_MainWindow->m_RenderWidget->ui.tab_LightController, SLOT(editedLightList(const QString&, const QString&)));

	connect(this, SIGNAL(mouseClicked(QMouseEvent*)), g_MainWindow->m_RenderWidget->ui.tab_ObjectController, SLOT(clickRenderWidget()));

	tick();
	return true;
}

void QDirectX11Widget::tick()
{
	DLLEngine::QTUpdate(m_qTimer.intervalAsDuration().count());
	DLLTime::SetTick(static_cast<double>(m_qTimer.intervalAsDuration().count() / 1000.f));

	if (hasFocus()==true)
	{
		DLLEngine::QTInput(m_qTimer.intervalAsDuration().count());
		m_pInput->Update();

		//g_MainWindow->m_RenderWidget->ui.tab_ObjectController->UpdateTransformData();
	}

	g_MainWindow->m_RenderWidget->ui.tab_ObjectController->Update();
	g_MainWindow->m_RenderWidget->ui.tab_LightController->Update();
	
	//emit이라는 키워드로 signal을 발생시킨다.
	emit ticked();
}

void QDirectX11Widget::run()
{
	m_qTimer.start(MS_PER_FRAME);
	m_bRenderActive = true;
}

void QDirectX11Widget::render()
{
	DLLEngine::QTRender();
	emit rendered();
}

void QDirectX11Widget::continueFrames()
{
	connect(&m_qTimer, &QTimer::timeout, this, &QDirectX11Widget::OnFrame);
	m_qTimer.start(MS_PER_FRAME);

	m_bRenderActive = true;
}

void QDirectX11Widget::pauseFrames()
{
	disconnect(&m_qTimer, &QTimer::timeout, this, &QDirectX11Widget::OnFrame);
	m_qTimer.stop();

	m_bRenderActive = false;
}

void QDirectX11Widget::SetRenderWidget(QRenderWidget* renderWidget)
{
	m_pRenderWidget = renderWidget;
}

void QDirectX11Widget::OnFrame()
{
	if (m_bRenderActive)
	{
		tick();
		render();
	}

	DLLTime::Reset();
}

void QDirectX11Widget::OnReset()
{

}

bool QDirectX11Widget::event(QEvent* event)
{
	switch (event->type())
	{
		// Workaround for https://bugreports.qt.io/browse/QTBUG-42183 to get key strokes.
		// To make sure that we always have focus on the widget when we enter the rect area.
	case QEvent::Enter:
	case QEvent::FocusIn:
	case QEvent::FocusAboutToChange:
		if (::GetFocus() != m_hWnd)
		{
			QWidget* nativeParent = this;
			while (true)
			{
				if (nativeParent->isWindow()) break;

				QWidget* parent = nativeParent->nativeParentWidget();
				if (!parent) break;

				nativeParent = parent;
			}

			if (nativeParent && nativeParent != this &&
				::GetFocus() == reinterpret_cast<HWND>(nativeParent->winId()))
				::SetFocus(m_hWnd);
		}
		break;
	case QEvent::KeyPress:
		emit keyPressed((QKeyEvent*)event);
		break;
	case QEvent::MouseMove:
		emit mouseMoved((QMouseEvent*)event);
		break;
	case QEvent::MouseButtonPress:
		emit mouseClicked((QMouseEvent*)event);
		break;
	case QEvent::MouseButtonRelease:
		emit mouseReleased((QMouseEvent*)event);
		break;
	case QEvent::Resize:
		QDirectX11Widget::resizeEvent((QResizeEvent*)event);
		break;
	}

	return QWidget::event(event);
}

void QDirectX11Widget::showEvent(QShowEvent* event)
{
	if (!m_bDeviceInitialized)
	{
		m_bDeviceInitialized = initialize();
		emit deviceInitialized(m_bDeviceInitialized);
	}

	QWidget::showEvent(event);
}

QPaintEngine* QDirectX11Widget::paintEngine() const
{
	return Q_NULLPTR;
}

void QDirectX11Widget::paintEvent(QPaintEvent* event)
{
	//아무것도 하지 않는다.
}

void QDirectX11Widget::resizeEvent(QResizeEvent* event)
{
	if (m_bDeviceInitialized)
	{
		DLLEngine::OnResize(event->size().height(), event->size().width());
		

		emit widgetResized();
	}

	QWidget::resizeEvent(event);
}

void QDirectX11Widget::wheelEvent(QWheelEvent* event)
{
	QWidget::wheelEvent(event);
}

LRESULT WINAPI QDirectX11Widget::WndProc(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_MOUSEHWHEEL) return false;

	if (DLLEngine::ImGuiHandler(m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam))
	{
		return true;
	}


	return false;
}
