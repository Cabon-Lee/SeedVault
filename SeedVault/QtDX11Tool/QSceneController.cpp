#include "pch.h"
#include "QSceneController.h"

#include "QObjectController.h"

#include "JsonLoader_QT.h"
#include "QMain.h"
#include "SceneBase.h"

extern QMain* g_MainWindow;
QSceneController::QSceneController(QWidget* parent)
	: QWidget(parent)
	, m_pJsonLoader(std::make_shared<JsonLoader_QT>())
{
	ui.setupUi(this);

}

QSceneController::~QSceneController()
{
}

void QSceneController::initialize()
{
	updateSceneComboBox();
	updateCameraComboBox();

	ui.lineEdit_CameraSpeed->setValidator(new QDoubleValidator());

	//main window가 생성된 후면 되는데 마땅한 때를 못찾겠다.
	connect(ui.pushButton_CreateNewScene, SIGNAL(clicked()), g_MainWindow, SLOT(createNewFile()));
	connect(ui.pushButton_SaveScene, SIGNAL(clicked()), g_MainWindow, SLOT(saveFile()));

	connect(g_MainWindow, SIGNAL(createdNewFile()), this, SLOT(createNewScene()));
	connect(g_MainWindow, SIGNAL(savedFile(const QString&)), this, SLOT(saveScene(const QString&)));
	connect(g_MainWindow, SIGNAL(openedFile(const QString&)), this, SLOT(openScene(const QString&)));

	connect(ui.comboBox_SceneList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(sceneSelect()));
	connect(ui.lineEdit_SceneName, SIGNAL(editingFinished()), this, SLOT(editSceneName()));

	connect(ui.comboBox_CameraList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(selectCamera()));
	connect(ui.lineEdit_CameraSpeed, SIGNAL(editingFinished()), this, SLOT(setCamSpeed()));
	connect(ui.checkBox_Physics, SIGNAL(stateChanged()), this, SLOT(checkPysics()));

	connect(this, SIGNAL(changedScene(SceneBase*)), m_pObjectController, SLOT(changeObjectManager(SceneBase*)));

	//ui.lineEdit_SceneID->setText(QString::fromStdString(std::to_string(m_pScene->GetSceneId())));
	//ui.lineEdit_SceneName->setText(QString::fromStdString(m_pScene->GetSceneName()));
	//ui.checkBox_Physics->setCheckState(m_pScene->IsPhysicsScene() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

}

void QSceneController::createNewScene()
{
	static int number = 0;
	char name[100] = "NoName";
	char buffer[100];
	_itoa_s(number++, buffer, 10);
	strcat(name, buffer);

	SceneBase* _scene = DLLEngine::CreateNewScene(name);
	DLLEngine::AddScene(DLLEngine::GetSceneName(_scene), _scene);

	AddToSceneList(_scene);

	sceneChange(_scene);
}

void QSceneController::saveScene(const QString& filepath)
{
	char buff[200] = { 0, };
	QStringToConstCharPtr(filepath, buff);
	DLLEngine::WriteScene(m_pScene, buff);
}

void QSceneController::openScene(const QString& fullfilepath)
{
	if (fullfilepath.isEmpty())
	{
		return;
	}

	QFileInfo file = QFileInfo(fullfilepath);

	//"Scene_~~"이기 때문에 (0~5까지 지워준다.)
	char pathbuffer[100] = { 0, };
	char namebuffer[100] = { 0, };
	QStringToConstCharPtr(file.canonicalPath(), pathbuffer);
	QStringToConstCharPtr(file.baseName(), namebuffer);
	SceneBase* _scene = DLLEngine::CreateScene(m_pJsonLoader.get(), pathbuffer, namebuffer + sizeof(char) * 6);
	AddToSceneList(_scene);

	sceneChange(_scene);
}

void QSceneController::sceneSelect()
{
	/// <summary>
	/// 신 변경- using combo box
	/// </summary>
	/// 
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.comboBox_SceneList->currentText(), buff);
	DLLEngine::SelectScene(buff);
	m_pScene = DLLEngine::GetNowScene();
	DLLGameObject::AddCamraController(DLLEngine::FindGameObjectByName("EngineCamera"));
	

	ui.lineEdit_SceneID->setText(QString::number(DLLEngine::GetSceneID(m_pScene)));
	ui.lineEdit_SceneName->setText(QString(DLLEngine::GetSceneName(m_pScene)));
	ui.checkBox_Physics->setCheckState(DLLEngine::GetSceneIsPhysics(m_pScene) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	ui.lineEdit_CameraSpeed->setText(QString::number(DLLGameObject::GetCamraController(DLLEngine::FindGameObjectByName("EngineCamera"))->GetMovingVelocity()));

	updateCameraComboBox();

	emit changedScene(m_pScene);
}

void QSceneController::sceneChange(SceneBase* scene)
{
	/// <summary>
	/// 신 변경
	/// </summary>
	DLLEngine::SelectScene(DLLEngine::GetSceneName(scene));
	m_pScene = scene;
	ui.comboBox_SceneList->setCurrentText(ConstCharPtrToQString((DLLEngine::GetSceneName(scene))));
	DLLGameObject::AddCamraController(DLLEngine::FindGameObjectByName("EngineCamera"));

	//sceneSelect();
	//ui.lineEdit_SceneID->setText(QString::number(DLLEngine::GetSceneID(m_pScene)));
	//ui.lineEdit_SceneName->setText(QString(DLLEngine::GetSceneName(m_pScene)));
	//ui.checkBox_Physics->setCheckState(DLLEngine::GetSceneIsPhysics(m_pScene) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	//updateCameraComboBox();

	emit changedScene(m_pScene);
}

void QSceneController::AddToSceneList(SceneBase* scene)
{
	if (strcmp(DLLEngine::GetSceneName(scene), "Test") != 0)
	{
		DLLEngine::AddScene(DLLEngine::GetSceneName(scene), scene);
	}

	ui.comboBox_SceneList->addItem(ConstCharPtrToQString((DLLEngine::GetSceneName(scene))));
}

void QSceneController::AddToCameraList(Camera* camera)
{
	ui.comboBox_CameraList->addItem(ConstCharPtrToQString(DLLEngine::GetCameraName(camera)));
}

void QSceneController::SetObjectController(QObjectController* controller)
{
	m_pObjectController = controller;
}

void QSceneController::editSceneName()
{
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.lineEdit_SceneName->text(), buff);
	DLLEngine::SetSceneName(m_pScene, buff);
	ui.comboBox_SceneList->setItemText(ui.comboBox_SceneList->currentIndex(), ui.lineEdit_SceneName->text());
}

void QSceneController::checkPysics()
{
	DLLEngine::SetSceneIsPhysics(m_pScene, ui.checkBox_Physics->isChecked());
}

void QSceneController::updateSceneComboBox()
{
	ui.comboBox_CameraList->clear();

	for (int index = 0; DLLEngine::GetSceneByIndex(index) != nullptr; index++)
	{
		AddToSceneList(DLLEngine::GetSceneByIndex(index));
	}


	ui.comboBox_SceneList->setCurrentText(ConstCharPtrToQString(DLLEngine::GetSceneName(DLLEngine::GetNowScene())));

	ui.lineEdit_SceneID->setText(QString::number(DLLEngine::GetSceneID(m_pScene)));
	ui.lineEdit_SceneName->setText(ConstCharPtrToQString(DLLEngine::GetSceneName(m_pScene)));
	ui.checkBox_Physics->setCheckState(DLLEngine::GetSceneIsPhysics(m_pScene) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void QSceneController::updateCameraComboBox()
{
	ui.comboBox_CameraList->clear();

	for (int index = 0; DLLEngine::GetCameraByIndex(index) != nullptr; index++)
	{
		AddToCameraList(DLLEngine::GetCameraByIndex(index));
	}
	ui.comboBox_CameraList->setCurrentText(ConstCharPtrToQString(DLLEngine::GetCameraName(DLLEngine::GetNowCamera())));
}

void QSceneController::setCamSpeed()
{
	//엔진카메라에만 카메라컨트롤러를 붙여놓았으니...
	GameObject* _cam = DLLEngine::FindGameObjectByName("EngineCamera");

	if (_cam != nullptr)
	{
		ECameraController* _camControll = DLLGameObject::GetCamraController(_cam);
		if (_camControll != nullptr)
		{
			_camControll->SetMovingVelocity((ui.lineEdit_CameraSpeed->text().toFloat()));
		}
	}
}

void QSceneController::setFirstScene(SceneBase* scene)
{
	m_pScene = scene;

	if (m_pScene != nullptr)
	{
		initialize();
	}

	emit changedScene(m_pScene);
}



void QSceneController::selectCamera()
{
	char buff[100] = { 0, };
	QStringToConstCharPtr(ui.comboBox_CameraList->currentText(), buff);
	DLLEngine::SetNowCamera(buff);
}
