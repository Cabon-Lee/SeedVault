#pragma once

#include <QWidget>
#include "ui_QSceneController.h"


//경로 문제때문에 임시로 선언
#define QT_HJ

class JsonLoader_QT;
class SceneBase;
class Camera;
class QObjectController;
class ObjectManager;
class QSceneController : public QWidget
{
	Q_OBJECT

public:
	QSceneController(QWidget *parent = Q_NULLPTR);
	~QSceneController();

private:
	Ui::QSceneController ui;

public slots:
	void initialize();

	void createNewScene();
	void saveScene(const QString& filepath);
	void openScene(const QString& fullfilepath);

	void sceneSelect();
	void sceneChange(SceneBase* scene);
	void setFirstScene(SceneBase* scene);

	void selectCamera();

	void editSceneName();
	void checkPysics();

	void updateSceneComboBox();
	void updateCameraComboBox();
	void setCamSpeed();

signals:
	void changedScene(SceneBase* current);

private:
	void AddToSceneList(SceneBase* scene);
	void AddToCameraList(Camera* camera);

public:
	void SetObjectController(QObjectController* controller);


public:


private:
	std::shared_ptr<JsonLoader_QT> m_pJsonLoader;

	SceneBase* m_pScene;
	QObjectController* m_pObjectController;
};
