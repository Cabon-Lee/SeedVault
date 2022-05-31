#pragma once

#include <QWidget>
#include "ui_QObjectController.h"

class QGameObject;
class GameObject;
class SceneBase;

struct PhysicsActorWidget
{
	QLabel*			m_label_BSize = nullptr;
	QLineEdit*		m_lineEdit_BX = nullptr;
	QLineEdit*		m_lineEdit_BY = nullptr;
	QLineEdit*		m_lineEdit_BZ = nullptr;
	QLabel*			m_label_BX = nullptr;
	QLabel*			m_label_BY = nullptr;
	QLabel*			m_label_BZ = nullptr;

	QLabel*			m_label_SSize = nullptr;
	QLineEdit*		m_lineEdit_SRadius = nullptr;

	QString			m_PrevRigidType = "";
};

class QObjectController : public QWidget
{
	Q_OBJECT

public:
	QObjectController(QWidget* parent = Q_NULLPTR);
	~QObjectController();

	void Update();
	void UpdateTransformData();
private:
	Ui::QObjectController ui;


private slots:
	void addGameObejctWidget(GameObject* object);

	void initialize();

	void createObject();
	void deleteObject();
	void setCameraTocurrObj();

public slots:
	void clickRenderWidget();

	void changeObjectManager(SceneBase* current);						//신이 변경되었을때
	void updateObjectComboBox();
	void editObjectName();
	void selectCurrentObject();
	void checkEnable();
	void editPosX();
	void editPosY();
	void editPosZ();
	void editRotX();
	void editRotY();
	void editRotZ();
	void editScaleX();
	void editScaleY();
	void editScaleZ();

	void addMeshRenderer();
	void deleteMeshRenderer();

	void setFbxTransform();
	void checkShadow();
	void checkGizmo();
	void checkHardwareInstancing(int value);
	void checkReflection (int value);
	void selectMeshFilter();

	void updateMeshList();

	void setMaterialIndex(int index);

	void setAlbedoTexture();
	void setNormalTexture();
	void setMetalRoughTexture();
	void setEmissiveTexture();

	void setMetalicValue();
	void setEmissveValue();
	void setShinenessValue();
	void setNormalValue();

	void addLight();
	void deleteLight();

	void addReflectionProbe();
	void deleteReflectionProbe();
	void bakeReflectionProbe();
	void checkReflectionRender(int state);

	void addPhysicsActor();
	void initializePhysicsAcotrList();
	void deletePhysicsActor();
	void updateRigidType();
	void updatePhysicsActor();


signals:
	void addedLight(const QString& objectname);
	void deletedLight(const QString& objectname);
	void editedObjectName(const QString& prev, const QString& now);

private:
	void AddToGameObejctList(GameObject* object);
	void SetGameObejctWidgetVector();
	void SetObjectData();

private:
	bool			event(QEvent* event) override;

private:
	SceneBase* m_pCurrentScene;
	GameObject* m_pCurrentGameObject;

	PhysicsActorWidget m_PhysicsWidgets;

};

