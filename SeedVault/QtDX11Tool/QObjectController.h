#pragma once

#include <QWidget>
#include "ui_QObjectController.h"

class QGameObject;
class GameObject;
class SceneBase;

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
	void setDebugRender();

public slots:
	void clickRenderWidget();

	void changeObjectManager(SceneBase* current);						//신이 변경되었을때
	void updateObjectComboBox();
	void editObjectName();
	void selectCurrentObject();
	void checkEnable();
	void editTag();
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
	void setPhysicsActorType(const QString& type);
	void deletePhysicsActor();
	void editBoxSize();

	void editSphereRadius();
	void checkUseGravity(int value);
	void checkIsKinematic(int value);
	void checkFreezePosition();
	void checkFreezeRotation();
	void clickDynamic();
	void clickStatic();
	void setRigidType();
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
};

