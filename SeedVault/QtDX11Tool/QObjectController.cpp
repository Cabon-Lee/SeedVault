#include "pch.h"
#include "QObjectController.h"

#include "QGameObject.h"

#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "PhysicsGeometry.h"
#include "PhysicsActor.h"


/// <summary>
/// Transform, (o)
/// MeshFilter, MeshRenderer,
/// Physics
/// Component 삽입 및 조작 가능 하게 해야합니다.
/// </summary>
/// <param name="parent"></param>
/// 

QObjectController::QObjectController(QWidget* parent)
	: QWidget(parent)
	, m_pCurrentScene(nullptr)
	, m_pCurrentGameObject(nullptr)
{
	ui.setupUi(this);
	setFocusPolicy(Qt::StrongFocus);

	//QDoubleValidator* _tempValidator = nullptr;
	////3자리까지만 double형 입력만 받는다.
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_PositionX);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_PositionX->setValidator(_tempValidator);
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_PositionY);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_PositionY->setValidator(_tempValidator);
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_PositionZ);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_PositionZ->setValidator(_tempValidator);
	//
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_RotationX);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_RotationX->setValidator(_tempValidator);
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_RotationY);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_RotationY->setValidator(_tempValidator);
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_RotationZ);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_RotationZ->setValidator(_tempValidator);
	//
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_ScaleX);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_ScaleX->setValidator(_tempValidator);
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_ScaleY);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_ScaleY->setValidator(_tempValidator);
	//_tempValidator = new QDoubleValidator(ui.doubleSpinBox_ScaleZ);
	//_tempValidator->setDecimals(3);
	//ui.doubleSpinBox_ScaleZ->setValidator(_tempValidator);

	connect(ui.checkBox_Enable, SIGNAL(stateChanged(int)), this, SLOT(checkEnable()));
	connect(ui.pushButton_CreateObject, SIGNAL(clicked()), this, SLOT(createObject()));
	connect(ui.pushButton_DebugRender, SIGNAL(clicked()), this, SLOT(setDebugRender()));
	connect(ui.pushButton_DeleteObject, SIGNAL(clicked()), this, SLOT(deleteObject()));
	connect(ui.lineEdit_ObjectName, SIGNAL(editingFinished()), this, SLOT(editObjectName()));
	connect(ui.comboBox_ObjectList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(selectCurrentObject()));
	connect(ui.lineEdit_Tag, SIGNAL(editingFinished()), this, SLOT(editTag()));

	connect(ui.doubleSpinBox_PositionX, SIGNAL(valueChanged(double)), this, SLOT(editPosX()));
	connect(ui.doubleSpinBox_PositionY, SIGNAL(valueChanged(double)), this, SLOT(editPosY()));
	connect(ui.doubleSpinBox_PositionZ, SIGNAL(valueChanged(double)), this, SLOT(editPosZ()));

	connect(ui.doubleSpinBox_RotationX, SIGNAL(valueChanged(double)), this, SLOT(editRotX()));
	connect(ui.doubleSpinBox_RotationY, SIGNAL(valueChanged(double)), this, SLOT(editRotY()));
	connect(ui.doubleSpinBox_RotationZ, SIGNAL(valueChanged(double)), this, SLOT(editRotZ()));

	connect(ui.doubleSpinBox_ScaleX, SIGNAL(valueChanged(double)), this, SLOT(editScaleX()));
	connect(ui.doubleSpinBox_ScaleY, SIGNAL(valueChanged(double)), this, SLOT(editScaleY()));
	connect(ui.doubleSpinBox_ScaleZ, SIGNAL(valueChanged(double)), this, SLOT(editScaleZ()));

	connect(ui.pushButton_MeshRendererAdd, SIGNAL(clicked()), this, SLOT(addMeshRenderer()));
	connect(ui.pushButton_MeshRendererDelete, SIGNAL(clicked()), this, SLOT(deleteMeshRenderer()));

	connect(ui.pushButton_fbxTrans, SIGNAL(clicked()), this, SLOT(setFbxTransform()));

	connect(ui.checkBox_Shadow, SIGNAL(stateChanged(int)), this, SLOT(checkShadow()));
	connect(ui.checkBox_Gizmo, SIGNAL(stateChanged(int)), this, SLOT(checkGizmo()));
	connect(ui.checkBox_HardwareInstancing, SIGNAL(stateChanged(int)), this, SLOT(checkHardwareInstancing(int)));
	connect(ui.checkBox_Reflection, SIGNAL(stateChanged(int)), this, SLOT(checkReflection(int)));

	connect(ui.comboBox_MeshList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(selectMeshFilter()));

	connect(ui.comboBox_MateralList, SIGNAL(currentIndexChanged(int)), this, SLOT(setMaterialIndex(int)));

	connect(ui.comboBox_Albedo, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setAlbedoTexture()));
	connect(ui.comboBox_Normal, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setNormalTexture()));
	connect(ui.comboBox_MetalRough, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setMetalRoughTexture()));
	connect(ui.comboBox_Emissive, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setEmissiveTexture()));

	connect(ui.horizontalSlider_Metalic, SIGNAL(valueChanged(int)), this, SLOT(setMetalicValue()));
	connect(ui.horizontalSlider_EmissiveFactor, SIGNAL(valueChanged(int)), this, SLOT(setEmissveValue()));
	connect(ui.horizontalSlider_NormalFactor, SIGNAL(valueChanged(int)), this, SLOT(setNormalValue()));
	connect(ui.horizontalSlider_Shineness, SIGNAL(valueChanged(int)), this, SLOT(setShinenessValue()));

	connect(ui.pushButton_AddLight, SIGNAL(clicked()), this, SLOT(addLight()));
	connect(ui.pushButton_DeleteLight, SIGNAL(clicked()), this, SLOT(deleteLight()));

	connect(ui.pushButton_AddReflectionProbe, SIGNAL(clicked()), this, SLOT(addReflectionProbe()));
	connect(ui.pushButton_DeleteReflectionProbe, SIGNAL(clicked()), this, SLOT(deleteReflectionProbe()));
	connect(ui.pushButton_BakeReflectionProbe, SIGNAL(clicked()), this, SLOT(bakeReflectionProbe()));
	connect(ui.checkBox_ReflectionRender, SIGNAL(stateChanged(int)), this, SLOT(checkReflectionRender(int)));

	connect(ui.pushButton_PhysicsActorAdd, SIGNAL(clicked()), this, SLOT(addPhysicsActor()));
	connect(ui.pushButton_PhysicsActorDelete, SIGNAL(clicked()), this, SLOT(deletePhysicsActor()));
	connect(ui.comboBox_PhysicsActor, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setPhysicsActorType(const QString&)));
	connect(ui.doubleSpinBox_BoxSizeX, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
	connect(ui.doubleSpinBox_BoxSizeY, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
	connect(ui.doubleSpinBox_BoxSizeZ, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
	connect(ui.doubleSpinBox_SphereRadius, SIGNAL(valueChanged(double)), this, SLOT(editSphereRadius()));
	connect(ui.checkBox_FreezePositionX, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
	connect(ui.checkBox_FreezePositionY, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
	connect(ui.checkBox_FreezePositionZ, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
	connect(ui.checkBox_FreezeRotationX, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
	connect(ui.checkBox_FreezeRotationY, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
	connect(ui.checkBox_FreezeRotationZ, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
	connect(ui.checkBox_UseGravity, SIGNAL(stateChanged(int)), this, SLOT(checkUseGravity(int)));
	connect(ui.checkBox_IsKinematic, SIGNAL(stateChanged(int)), this, SLOT(checkIsKinematic(int)));
	connect(ui.radioButton_Dynamic, SIGNAL(clicked()), this, SLOT(clickDynamic()));
	connect(ui.radioButton_Static, SIGNAL(clicked()), this, SLOT(clickStatic()));


	ui.label_BoxSize->setVisible(false);
	ui.label_BoxSizeX->setVisible(false);
	ui.label_BoxSizeY->setVisible(false);
	ui.label_BoxSizeZ->setVisible(false);
	ui.doubleSpinBox_BoxSizeX->setVisible(false);
	ui.doubleSpinBox_BoxSizeY->setVisible(false);
	ui.doubleSpinBox_BoxSizeZ->setVisible(false);
	ui.label_SphereRadius->setVisible(false);
	ui.doubleSpinBox_SphereRadius->setVisible(false);

	QString comboStyle = "QComboBox { combobox-popup: 0; }";

	ui.comboBox_ObjectList->setStyleSheet(comboStyle);
	ui.comboBox_ObjectList->setMaxVisibleItems(20);

	ui.comboBox_MeshList->setStyleSheet(comboStyle);
	ui.comboBox_MateralList->setStyleSheet(comboStyle);
	ui.comboBox_Albedo->setStyleSheet(comboStyle);
	ui.comboBox_Normal->setStyleSheet(comboStyle);
	ui.comboBox_MetalRough->setStyleSheet(comboStyle);
	ui.comboBox_Emissive->setStyleSheet(comboStyle);
	ui.comboBox_PhysicsActor->setStyleSheet(comboStyle);

}

QObjectController::~QObjectController()
{
}

void QObjectController::addGameObejctWidget(GameObject* object)
{
	//std::shared_ptr<QGameObject> qGameObjectW = std::make_shared<QGameObject>();
	//qGameObjectW->setObjectName(QString::fromStdString(object->GetObjectName()));

	////virticalLayout->addWidget(qGameObjectW.get());
	//layout()->addWidget(qGameObjectW.get());
	//m_pGameObjectWidget_V.push_back(qGameObjectW);

	//emit qGameObjectW->createdQGameObject(object);
}

void QObjectController::initialize()
{
	updateObjectComboBox();
	SetObjectData();
}

void QObjectController::createObject()
{
	GameObject* tempObejct = DLLEngine::CreateObject(m_pCurrentScene);

	char name[100] = "Object";
	char buffer[100];
	_itoa_s(tempObejct->GetGameObjectId(), buffer, 10);
	strcat(name, buffer);

	DLLGameObject::SetName(tempObejct, name);
	AddToGameObejctList(tempObejct);

	m_pCurrentGameObject = tempObejct;

	SetObjectData();
}

void QObjectController::deleteObject()
{
	DLLGameObject::DeleteGameObject(m_pCurrentScene, m_pCurrentGameObject);
	ui.comboBox_ObjectList->removeItem(ui.comboBox_ObjectList->currentIndex());

	m_pCurrentGameObject = DLLEngine::GetGameObject(m_pCurrentScene, 0);

	SetObjectData();
}

void QObjectController::setDebugRender()
{
	static bool renderMode = false;
	DLLEngine::SetDebugRenderMode(!renderMode);
	renderMode = !renderMode;
}

void QObjectController::clickRenderWidget()
{
	unsigned int clicked_id = DLLEngine::GetCursorObjectID();
	unsigned int curr_id = m_pCurrentGameObject->GetGameObjectId();

	//선택된 것이 있고&&현 선택된 오브젝트와 다른 오브젝트라면
	if (clicked_id != 0 && m_pCurrentGameObject->GetGameObjectId() != clicked_id)
	{
		QString _objectname = DLLGameObject::GetName(DLLEngine::FindGameObjectById(DLLEngine::GetCursorObjectID()));
		ui.comboBox_ObjectList->setCurrentText(_objectname);
	}
}

void QObjectController::Update()
{
	//업데이트 될때마다 불리는 곳
	//if (m_pCurrentGameObject != nullptr)
	//{
	//	float a = 0.2f;
	//	if (DLLInput::InputKey(VK_UP))
	//	{
	//		m_pCurrentGameObject->m_Transform->m_Position.x += a;
	//		ui.doubleSpinBox_PositionX->setValue(m_pCurrentGameObject->m_Transform->m_Position.x);
	//	}
	//
	//	if (DLLInput::InputKey(VK_DOWN))
	//	{
	//		m_pCurrentGameObject->m_Transform->m_Position.x -= a;
	//		ui.doubleSpinBox_PositionX->setValue(m_pCurrentGameObject->m_Transform->m_Position.x);
	//	}
	//
	//	if (DLLInput::InputKey(VK_LEFT))
	//	{
	//		m_pCurrentGameObject->m_Transform->m_Position.z -= a;
	//		ui.doubleSpinBox_PositionZ->setValue(m_pCurrentGameObject->m_Transform->m_Position.z);
	//	}
	//
	//	if (DLLInput::InputKey(VK_RIGHT))
	//	{
	//		m_pCurrentGameObject->m_Transform->m_Position.z += a;
	//		ui.doubleSpinBox_PositionZ->setValue(m_pCurrentGameObject->m_Transform->m_Position.z);
	//	}
	//}



	updateMeshList();
	UpdateTransformData();
}

void QObjectController::UpdateTransformData()
{
	if (ui.doubleSpinBox_PositionX->hasFocus() == true || ui.doubleSpinBox_PositionY->hasFocus() == true || ui.doubleSpinBox_PositionZ->hasFocus() == true ||
		ui.doubleSpinBox_RotationX->hasFocus() == true || ui.doubleSpinBox_RotationY->hasFocus() == true || ui.doubleSpinBox_RotationZ->hasFocus() == true ||
		ui.doubleSpinBox_ScaleX->hasFocus() == true || ui.doubleSpinBox_ScaleY->hasFocus() == true || ui.doubleSpinBox_ScaleZ->hasFocus() == true)
	{
		return;
	}

	ui.doubleSpinBox_PositionX->setValue(m_pCurrentGameObject->m_Transform->m_Position.x);
	ui.doubleSpinBox_PositionY->setValue(m_pCurrentGameObject->m_Transform->m_Position.y);
	ui.doubleSpinBox_PositionZ->setValue(m_pCurrentGameObject->m_Transform->m_Position.z);

	ui.doubleSpinBox_RotationX->setValue(m_pCurrentGameObject->m_Transform->m_EulerAngles.x);
	ui.doubleSpinBox_RotationY->setValue(m_pCurrentGameObject->m_Transform->m_EulerAngles.y);
	ui.doubleSpinBox_RotationZ->setValue(m_pCurrentGameObject->m_Transform->m_EulerAngles.z);

	ui.doubleSpinBox_ScaleX->setValue(m_pCurrentGameObject->m_Transform->m_Scale.x);
	ui.doubleSpinBox_ScaleY->setValue(m_pCurrentGameObject->m_Transform->m_Scale.y);
	ui.doubleSpinBox_ScaleZ->setValue(m_pCurrentGameObject->m_Transform->m_Scale.z);

}

void QObjectController::changeObjectManager(SceneBase* current)
{
	m_pCurrentScene = current;
	m_pCurrentGameObject = nullptr;


	initialize();
}

void QObjectController::updateObjectComboBox()
{
	//선택된 현재 오브젝트가 없다면 첫번째 오브젝트를 선택한다.
	if (m_pCurrentGameObject == nullptr)
	{
		ui.comboBox_ObjectList->clear();

		DLLEngine::GetGameObject(m_pCurrentScene, 0);
		ui.comboBox_ObjectList->setCurrentIndex(0);
	}

	for (int index = 0; DLLEngine::GetGameObject(m_pCurrentScene, index) != nullptr; index++)
	{
		AddToGameObejctList(DLLEngine::GetGameObject(m_pCurrentScene, index));
	}

	SetObjectData();
}

void QObjectController::editObjectName()
{
	emit editedObjectName(QString(DLLGameObject::GetName(m_pCurrentGameObject)), ui.lineEdit_ObjectName->text());

	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.lineEdit_ObjectName->text(), buff);
	DLLGameObject::SetName(m_pCurrentGameObject, buff);
	ui.comboBox_ObjectList->setItemText(ui.comboBox_ObjectList->currentIndex(), ui.lineEdit_ObjectName->text());
}

void QObjectController::selectCurrentObject()
{
	//clear한 후에는 -1이 된다.
	if (ui.comboBox_ObjectList->currentIndex() == -1)
	{
		return;
	}

	m_pCurrentGameObject = DLLEngine::GetGameObject(m_pCurrentScene, ui.comboBox_ObjectList->currentIndex());

	SetObjectData();
}

void QObjectController::checkEnable()
{
	ui.checkBox_Enable->isChecked() ?		//체크되어있으면 enable
		m_pCurrentGameObject->OnEnable() : m_pCurrentGameObject->OnDisable();
}

void QObjectController::editTag()
{
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.lineEdit_Tag->text(), buff);
	DLLEngine::SetTag(buff, m_pCurrentGameObject);
}

void QObjectController::editPosX()
{
	m_pCurrentGameObject->m_Transform->m_Position.x = ui.doubleSpinBox_PositionX->value();
	m_pCurrentGameObject->m_Transform->SetPosition({ m_pCurrentGameObject->m_Transform->m_Position.x,
													 m_pCurrentGameObject->m_Transform->m_Position.y,
													 m_pCurrentGameObject->m_Transform->m_Position.z });
}

void QObjectController::editPosY()
{
	m_pCurrentGameObject->m_Transform->m_Position.y = ui.doubleSpinBox_PositionY->value();
	m_pCurrentGameObject->m_Transform->SetPosition({ m_pCurrentGameObject->m_Transform->m_Position.x,
													 m_pCurrentGameObject->m_Transform->m_Position.y,
													 m_pCurrentGameObject->m_Transform->m_Position.z });
}

void QObjectController::editPosZ()
{
	m_pCurrentGameObject->m_Transform->m_Position.z = ui.doubleSpinBox_PositionZ->value();
	m_pCurrentGameObject->m_Transform->SetPosition({ m_pCurrentGameObject->m_Transform->m_Position.x,
													 m_pCurrentGameObject->m_Transform->m_Position.y,
													 m_pCurrentGameObject->m_Transform->m_Position.z });
}

void QObjectController::editRotX()
{
	m_pCurrentGameObject->m_Transform->m_EulerAngles.x = ui.doubleSpinBox_RotationX->value();
	m_pCurrentGameObject->m_Transform->SetRotationFromVec({ m_pCurrentGameObject->m_Transform->m_EulerAngles.x,
														   m_pCurrentGameObject->m_Transform->m_EulerAngles.y,
														   m_pCurrentGameObject->m_Transform->m_EulerAngles.z });
}

void QObjectController::editRotY()
{
	m_pCurrentGameObject->m_Transform->m_EulerAngles.y = ui.doubleSpinBox_RotationY->value();
	m_pCurrentGameObject->m_Transform->SetRotationFromVec({ m_pCurrentGameObject->m_Transform->m_EulerAngles.x,
															m_pCurrentGameObject->m_Transform->m_EulerAngles.y,
															m_pCurrentGameObject->m_Transform->m_EulerAngles.z });
}

void QObjectController::editRotZ()
{

	m_pCurrentGameObject->m_Transform->m_EulerAngles.z = ui.doubleSpinBox_RotationZ->value();
	m_pCurrentGameObject->m_Transform->SetRotationFromVec({ m_pCurrentGameObject->m_Transform->m_EulerAngles.x,
															m_pCurrentGameObject->m_Transform->m_EulerAngles.y,
															m_pCurrentGameObject->m_Transform->m_EulerAngles.z });
}

void QObjectController::editScaleX()
{
	m_pCurrentGameObject->m_Transform->m_Scale.x = ui.doubleSpinBox_ScaleX->value();
}

void QObjectController::editScaleY()
{
	m_pCurrentGameObject->m_Transform->m_Scale.y = ui.doubleSpinBox_ScaleY->value();
}

void QObjectController::editScaleZ()
{
	m_pCurrentGameObject->m_Transform->m_Scale.z = ui.doubleSpinBox_ScaleZ->value();
}

void QObjectController::addMeshRenderer()
{
	///dll의 힙영역과 이 프로젝트의 힙영역이 다르기 때문에 여기서 new로 할당한 매쉬 필터와 랜더러는 dll에서 제대로 작동하지 못한다.
	//MeshRenderer* _meshrenderer = new MeshRenderer();
	//MeshFilter* _meshfilter = new MeshFilter();
	//m_pCurrentGameObject->AddComponent<MeshRenderer>(_meshrenderer);
	//m_pCurrentGameObject->AddComponent<MeshFilter>(_meshfilter);

	DLLGameObject::AddMeshFilter(m_pCurrentGameObject);
	DLLGameObject::AddMeshRenderer(m_pCurrentGameObject);
	MeshRenderer* meshRenderer = DLLGameObject::GetMeshRenderer(m_pCurrentGameObject);
	ui.pushButton_MeshRendererAdd->setDisabled(true);
	ui.pushButton_MeshRendererDelete->setDisabled(false);

	ui.checkBox_Shadow->setDisabled(false);
	ui.checkBox_Shadow->setChecked(meshRenderer->GetCastShadow());
	ui.checkBox_Gizmo->setDisabled(false);
	ui.checkBox_Gizmo->setChecked(meshRenderer->GetIsGizmoOn());
	ui.checkBox_HardwareInstancing->setDisabled(false);
	ui.checkBox_Reflection->setDisabled(false);
	ui.checkBox_Reflection->setChecked(meshRenderer->GetReflection());

	ui.comboBox_MeshList->setDisabled(false);
	ui.comboBox_MateralList->setDisabled(false);
	ui.comboBox_Albedo->setDisabled(false);
	ui.comboBox_Normal->setDisabled(false);
	ui.comboBox_MetalRough->setDisabled(false);
	ui.comboBox_Emissive->setDisabled(false);

	ui.comboBox_Albedo->setDisabled(false);
	ui.comboBox_Normal->setDisabled(false);
	ui.comboBox_MetalRough->setDisabled(false);
	ui.comboBox_Emissive->setDisabled(false);

	ui.horizontalSlider_Metalic->setDisabled(false);
	ui.horizontalSlider_NormalFactor->setDisabled(false);
	ui.horizontalSlider_Shineness->setDisabled(false);
	ui.horizontalSlider_EmissiveFactor->setDisabled(false);

	///start전이기 때문에 meshlist가 없음
	//MeshFilter* _meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	//ui.comboBox_MeshList->setCurrentText(ConstCharPtrToQString(strcmp(_meshfilter->GetName_char(), "") == 0 ? "box.bin" : _meshfilter->GetName_char()));
}

void QObjectController::deleteMeshRenderer()
{
	DLLEngine::DerigisterComponent(DLLGameObject::GetMeshRenderer(m_pCurrentGameObject));
	DLLEngine::DerigisterComponent(DLLGameObject::GetMeshFilter(m_pCurrentGameObject));

	ui.pushButton_MeshRendererAdd->setDisabled(false);
	ui.pushButton_MeshRendererDelete->setDisabled(true);

	SetObjectData();
}

void QObjectController::setFbxTransform()
{
	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	meshfilter->SetFbxTransform();
}

void QObjectController::checkShadow()
{
	MeshRenderer* meshrenderer = DLLGameObject::GetMeshRenderer(m_pCurrentGameObject);

	ui.checkBox_Shadow->isChecked() ?		//체크되어있으면
		meshrenderer->SetCastShadow(true) : meshrenderer->SetCastShadow(false);
}

void QObjectController::updateMeshList()
{
	//임시로 한번만 업데이트 되게 해놓았다.
	static bool _isUpdated = false;
	MeshFilter* _meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);

	if (_meshfilter != nullptr && _isUpdated == false)
	{

		disconnect(ui.comboBox_MeshList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(selectMeshFilter()));
		for (int index = 0; index < _meshfilter->GetMeshCount(); index++)
		{
			ui.comboBox_MeshList->addItem(ConstCharPtrToQString(_meshfilter->GetMeshName_char(index)));
		}
		connect(ui.comboBox_MeshList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(selectMeshFilter()));

		disconnect(ui.comboBox_Albedo, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setAlbedoTexture()));
		disconnect(ui.comboBox_Normal, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setNormalTexture()));
		disconnect(ui.comboBox_MetalRough, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setMetalRoughTexture()));
		disconnect(ui.comboBox_Emissive, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setEmissiveTexture()));
		for (int texIndex = 0; texIndex < _meshfilter->GetGraphicMaterialTextureCount(); texIndex++)
		{
			ui.comboBox_Albedo->addItem(ConstCharPtrToQString(_meshfilter->GetTextureName(texIndex)));
			ui.comboBox_Normal->addItem(ConstCharPtrToQString(_meshfilter->GetTextureName(texIndex)));
			ui.comboBox_MetalRough->addItem(ConstCharPtrToQString(_meshfilter->GetTextureName(texIndex)));
			ui.comboBox_Emissive->addItem(ConstCharPtrToQString(_meshfilter->GetTextureName(texIndex)));
		}
		connect(ui.comboBox_Albedo, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setAlbedoTexture()));
		connect(ui.comboBox_Normal, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setNormalTexture()));
		connect(ui.comboBox_MetalRough, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setMetalRoughTexture()));
		connect(ui.comboBox_Emissive, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setEmissiveTexture()));


		//ui.comboBox_MeshList->setCurrentText(ConstCharPtrToQString(strcmp(_meshfilter->GetName_char(), "") == 0 ? "box.bin" : _meshfilter->GetName_char()));
		ui.comboBox_MeshList->setCurrentText(_meshfilter->GetName_char());

		_isUpdated = true;
	}

}

void QObjectController::setMaterialIndex(int index)
{
	//머테리얼 index
	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);

	//아무것도 셀렉트되지 않으면 -1이 들어오는듯 : clear()일때 여기로 들어옴
	if (index < 0)
	{
		return;
	}

	ui.comboBox_Albedo->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(index, ALBEDO));
	ui.comboBox_Normal->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(index, NORMAL));
	ui.comboBox_MetalRough->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(index, METALROUGH));
	ui.comboBox_Emissive->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(index, EMISSIVE));

	ui.horizontalSlider_Metalic->setValue(meshfilter->GetMaterialFactor_(index, METALLIC) * 100.f);
	ui.horizontalSlider_EmissiveFactor->setValue(meshfilter->GetMaterialFactor_(index, EMISSIVEFACTOR) * 100.f);
	ui.horizontalSlider_Shineness->setValue(meshfilter->GetMaterialFactor_(index, SHINENESS) * 100.f);
	ui.horizontalSlider_NormalFactor->setValue(meshfilter->GetMaterialFactor_(index, NORMALFACFOR) * 100.f);

	ui.label_Metalic_2->setText(QString::number(ui.horizontalSlider_Metalic->value()));
	ui.label_NormalFactor_2->setText(QString::number(ui.horizontalSlider_NormalFactor->value()));
	ui.label_Shineness_2->setText(QString::number(ui.horizontalSlider_Shineness->value()));
	ui.label_EmissiveFactor_2->setText(QString::number(ui.horizontalSlider_EmissiveFactor->value()));
}

void QObjectController::setAlbedoTexture()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.comboBox_Albedo->currentText(), buff);
	meshfilter->SetGraphicMaterialTextureName_char(ui.comboBox_MateralList->currentIndex(), ALBEDO, buff);

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setNormalTexture()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.comboBox_Normal->currentText(), buff);
	meshfilter->SetGraphicMaterialTextureName_char(ui.comboBox_MateralList->currentIndex(), NORMAL, buff);

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setMetalRoughTexture()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.comboBox_MetalRough->currentText(), buff);
	meshfilter->SetGraphicMaterialTextureName_char(ui.comboBox_MateralList->currentIndex(), METALROUGH, buff);

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setEmissiveTexture()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	char buff[200] = { 0, };
	QStringToConstCharPtr(ui.comboBox_Emissive->currentText(), buff);
	meshfilter->SetGraphicMaterialTextureName_char(ui.comboBox_MateralList->currentIndex(), EMISSIVE, buff);

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setMetalicValue()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	meshfilter->SetMaterialFactor(ui.comboBox_MateralList->currentIndex(), METALLIC, ui.horizontalSlider_Metalic->value() / 100.f);
	ui.label_Metalic_2->setText(QString::number(ui.horizontalSlider_Metalic->value()));

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setEmissveValue()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	meshfilter->SetMaterialFactor(ui.comboBox_MateralList->currentIndex(), EMISSIVEFACTOR, ui.horizontalSlider_EmissiveFactor->value() / 100.f);
	ui.label_EmissiveFactor_2->setText(QString::number(ui.horizontalSlider_EmissiveFactor->value()));

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setShinenessValue()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	meshfilter->SetMaterialFactor(ui.comboBox_MateralList->currentIndex(), SHINENESS, ui.horizontalSlider_Shineness->value() / 100.f);
	ui.label_Shineness_2->setText(QString::number(ui.horizontalSlider_Shineness->value()));

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::setNormalValue()
{
	if (ui.comboBox_MateralList->currentIndex() == -1)
	{
		return;
	}

	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);
	meshfilter->SetMaterialFactor(ui.comboBox_MateralList->currentIndex(), NORMALFACFOR, ui.horizontalSlider_NormalFactor->value() / 100.f);
	ui.label_NormalFactor_2->setText(QString::number(ui.horizontalSlider_NormalFactor->value()));

	meshfilter->SaveMaterial(ui.comboBox_MateralList->currentIndex());
}

void QObjectController::addLight()
{
	DLLGameObject::AddLight(m_pCurrentGameObject);
	ui.pushButton_AddLight->setDisabled(true);
	ui.pushButton_DeleteLight->setDisabled(false);

	emit addedLight(DLLGameObject::GetName(m_pCurrentGameObject));
}

void QObjectController::deleteLight()
{
	DLLEngine::DerigisterComponent(DLLGameObject::GetLight(m_pCurrentGameObject));
	ui.pushButton_AddLight->setDisabled(false);
	ui.pushButton_DeleteLight->setDisabled(true);

	emit deletedLight(DLLGameObject::GetName(m_pCurrentGameObject));
}

void QObjectController::addReflectionProbe()
{
	DLLGameObject::AddReflectionProbeCam(m_pCurrentGameObject);
	ui.pushButton_AddReflectionProbe->setDisabled(true);
	ui.pushButton_DeleteReflectionProbe->setDisabled(false);

	ui.pushButton_BakeReflectionProbe->setDisabled(false);
	ui.checkBox_ReflectionRender->setDisabled(false);
	checkReflectionRender(DLLGameObject::GetReflectionProbeCam(m_pCurrentGameObject)->GetIsRender());
}

void QObjectController::deleteReflectionProbe()
{
	DLLEngine::DerigisterComponent(DLLGameObject::GetReflectionProbeCam(m_pCurrentGameObject));
	ui.pushButton_AddReflectionProbe->setDisabled(false);
	ui.pushButton_DeleteReflectionProbe->setDisabled(true);

	ui.pushButton_BakeReflectionProbe->setDisabled(true);
	ui.checkBox_ReflectionRender->setDisabled(true);
}

void QObjectController::bakeReflectionProbe()
{
	DLLGameObject::GetReflectionProbeCam(m_pCurrentGameObject)->BakeReflectionProbe();
}

void QObjectController::checkReflectionRender(int state)
{
	DLLGameObject::GetReflectionProbeCam(m_pCurrentGameObject)->SetIsRender(state);
}

void QObjectController::addPhysicsActor()
{
	DLLGameObject::AddPhysicsActor(m_pCurrentGameObject);

	updatePhysicsActor();
	ui.checkBox_IsKinematic->setChecked(true);
	//checkIsKinematic(true);
}

void QObjectController::setPhysicsActorType(const QString& type)
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);

	if (type == "Box")
	{
		ui.label_BoxSize->setVisible(true);
		ui.label_BoxSizeX->setVisible(true);
		ui.label_BoxSizeY->setVisible(true);
		ui.label_BoxSizeZ->setVisible(true);
		ui.doubleSpinBox_BoxSizeX->setVisible(true);
		ui.doubleSpinBox_BoxSizeY->setVisible(true);
		ui.doubleSpinBox_BoxSizeZ->setVisible(true);

		ui.label_SphereRadius->setVisible(false);
		ui.doubleSpinBox_SphereRadius->setVisible(false);

		if (dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry()) == nullptr)
		{
			physicsActor->SetBoxCollider({ 1,1,1 },
				physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
		}
		else
		{
			physicsActor->SetBoxCollider({ dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().x,
											dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().y,
											dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().z },
				physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
		}
	}
	else if (type == "Sphere")
	{
		ui.label_SphereRadius->setVisible(true);
		ui.doubleSpinBox_SphereRadius->setVisible(true);

		ui.label_BoxSize->setVisible(false);
		ui.label_BoxSizeX->setVisible(false);
		ui.label_BoxSizeY->setVisible(false);
		ui.label_BoxSizeZ->setVisible(false);
		ui.doubleSpinBox_BoxSizeX->setVisible(false);
		ui.doubleSpinBox_BoxSizeY->setVisible(false);
		ui.doubleSpinBox_BoxSizeZ->setVisible(false);

		if (dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry()) == nullptr)
		{
			physicsActor->SetSphereCollider(1,
				physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
		}
		else
		{
			physicsActor->SetSphereCollider(dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry())->GetSphereRadius(),
				physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
		}
	}

}

void QObjectController::deletePhysicsActor()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	DLLEngine::DerigisterComponent(physicsActor);

	ui.pushButton_PhysicsActorAdd->setDisabled(false);
	ui.pushButton_PhysicsActorDelete->setDisabled(true);

	ui.comboBox_PhysicsActor->setDisabled(true);
	ui.radioButton_Dynamic->setDisabled(true);
	ui.radioButton_Static->setDisabled(true);

	ui.checkBox_UseGravity->setDisabled(true);
	ui.checkBox_IsKinematic->setDisabled(true);
	ui.checkBox_FreezePositionX->setDisabled(true);
	ui.checkBox_FreezePositionY->setDisabled(true);
	ui.checkBox_FreezePositionZ->setDisabled(true);
	ui.checkBox_FreezeRotationX->setDisabled(true);
	ui.checkBox_FreezeRotationY->setDisabled(true);
	ui.checkBox_FreezeRotationZ->setDisabled(true);

	ui.label_BoxSize->setVisible(false);
	ui.label_BoxSizeX->setVisible(false);
	ui.label_BoxSizeY->setVisible(false);
	ui.label_BoxSizeZ->setVisible(false);
	ui.doubleSpinBox_BoxSizeX->setVisible(false);
	ui.doubleSpinBox_BoxSizeY->setVisible(false);
	ui.doubleSpinBox_BoxSizeZ->setVisible(false);
	ui.label_SphereRadius->setVisible(false);
	ui.doubleSpinBox_SphereRadius->setVisible(false);
}

void QObjectController::editBoxSize()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	physicsActor->SetBoxCollider({ static_cast<float>(ui.doubleSpinBox_BoxSizeX->value()),
									static_cast<float>(ui.doubleSpinBox_BoxSizeY->value()),
									static_cast<float>(ui.doubleSpinBox_BoxSizeZ->value()) },
		ui.radioButton_Dynamic->isChecked() ? RigidType::Dynamic : RigidType::Static);
}

void QObjectController::editSphereRadius()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	physicsActor->SetSphereCollider(static_cast<float>(ui.doubleSpinBox_SphereRadius->value()),
		ui.radioButton_Dynamic->isChecked() ? RigidType::Dynamic : RigidType::Static);
}

void QObjectController::checkUseGravity(int value)
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	physicsActor->SetGravity(static_cast<bool>(value));

	if (ui.comboBox_PhysicsActor->currentText() == "Box")
	{
		physicsActor->SetBoxCollider({ dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().x,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().y,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().z },
			physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
	}
	else
	{
		physicsActor->SetSphereCollider(dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry())->GetSphereRadius(),
			physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
	}
}

void QObjectController::checkIsKinematic(int value)
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	physicsActor->SetKinematic(static_cast<bool>(value));

	if (ui.comboBox_PhysicsActor->currentText() == "Box")
	{
		physicsActor->SetBoxCollider({ dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().x,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().y,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().z },
			physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
	}
	else
	{
		physicsActor->SetSphereCollider(dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry())->GetSphereRadius(),
			physicsActor->GetRigidType() == RigidType::Dynamic ? RigidType::Dynamic : RigidType::Static);
	}
}


void QObjectController::checkFreezePosition()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	physicsActor->SetFreezePosition(ui.checkBox_FreezePositionX->isChecked(), ui.checkBox_FreezePositionY->isChecked(), ui.checkBox_FreezePositionZ->isChecked());
}

void QObjectController::checkFreezeRotation()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	physicsActor->SetFreezeRotation(ui.checkBox_FreezeRotationX->isChecked(), ui.checkBox_FreezeRotationY->isChecked(), ui.checkBox_FreezeRotationZ->isChecked());
}

void QObjectController::clickDynamic()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	if (ui.comboBox_PhysicsActor->currentText() == "Box")
	{
		physicsActor->SetBoxCollider({ dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().x,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().y,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().z },
			RigidType::Dynamic);
	}
	else
	{
		physicsActor->SetSphereCollider(dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry())->GetSphereRadius(),
			RigidType::Dynamic);
	}
}

void QObjectController::clickStatic()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	if (ui.comboBox_PhysicsActor->currentText() == "Box")
	{
		physicsActor->SetBoxCollider({ dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().x,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().y,
								dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().z },
			RigidType::Static);
	}
	else
	{
		physicsActor->SetSphereCollider(dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry())->GetSphereRadius(),
			RigidType::Static);
	}
}

void QObjectController::setRigidType()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);
	if (ui.comboBox_PhysicsActor->currentText() == "Box")
	{
		editBoxSize();
	}
	else if (ui.comboBox_PhysicsActor->currentText() == "Sphere")
	{
		editSphereRadius();
	}
}

void QObjectController::updatePhysicsActor()
{
	PhysicsActor* physicsActor = DLLGameObject::GetPhysicsActor(m_pCurrentGameObject);

	//존재 안한다.
	if (physicsActor == nullptr)
	{
		ui.pushButton_PhysicsActorAdd->setDisabled(false);
		ui.pushButton_PhysicsActorDelete->setDisabled(true);

		ui.comboBox_PhysicsActor->setDisabled(true);
		ui.radioButton_Dynamic->setDisabled(true);
		ui.radioButton_Static->setDisabled(true);

		ui.checkBox_UseGravity->setDisabled(true);
		ui.checkBox_IsKinematic->setDisabled(true);
		ui.checkBox_FreezePositionX->setDisabled(true);
		ui.checkBox_FreezePositionY->setDisabled(true);
		ui.checkBox_FreezePositionZ->setDisabled(true);
		ui.checkBox_FreezeRotationX->setDisabled(true);
		ui.checkBox_FreezeRotationY->setDisabled(true);
		ui.checkBox_FreezeRotationZ->setDisabled(true);

		ui.label_BoxSize->setVisible(false);
		ui.label_BoxSizeX->setVisible(false);
		ui.label_BoxSizeY->setVisible(false);
		ui.label_BoxSizeZ->setVisible(false);
		ui.doubleSpinBox_BoxSizeX->setVisible(false);
		ui.doubleSpinBox_BoxSizeY->setVisible(false);
		ui.doubleSpinBox_BoxSizeZ->setVisible(false);
		ui.label_SphereRadius->setVisible(false);
		ui.doubleSpinBox_SphereRadius->setVisible(false);
	}
	//존재한다?!
	else
	{
		ui.pushButton_PhysicsActorAdd->setDisabled(true);
		ui.pushButton_PhysicsActorDelete->setDisabled(false);

		ui.comboBox_PhysicsActor->setDisabled(false);
		ui.radioButton_Dynamic->setDisabled(false);
		ui.radioButton_Static->setDisabled(false);

		ui.checkBox_UseGravity->setDisabled(false);
		ui.checkBox_IsKinematic->setDisabled(false);
		ui.checkBox_FreezePositionX->setDisabled(false);
		ui.checkBox_FreezePositionY->setDisabled(false);
		ui.checkBox_FreezePositionZ->setDisabled(false);
		ui.checkBox_FreezeRotationX->setDisabled(false);
		ui.checkBox_FreezeRotationY->setDisabled(false);
		ui.checkBox_FreezeRotationZ->setDisabled(false);

		setPhysicsActorType(physicsActor->GetGetGeometryType() == GeometryType::Box ? "Box" : "Shpere");
		if (physicsActor->GetGetGeometryType() == GeometryType::Box)
		{
			disconnect(ui.doubleSpinBox_BoxSizeX, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
			disconnect(ui.doubleSpinBox_BoxSizeY, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
			disconnect(ui.doubleSpinBox_BoxSizeZ, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));

			ui.doubleSpinBox_BoxSizeX->setValue(dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().x);
			ui.doubleSpinBox_BoxSizeY->setValue(dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().y);
			ui.doubleSpinBox_BoxSizeZ->setValue(dynamic_cast<BoxGeometry*>(physicsActor->GetGeometry())->GetBoxSize().z);

			connect(ui.doubleSpinBox_BoxSizeX, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
			connect(ui.doubleSpinBox_BoxSizeY, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
			connect(ui.doubleSpinBox_BoxSizeZ, SIGNAL(valueChanged(double)), this, SLOT(editBoxSize()));
		}
		else
		{
			disconnect(ui.doubleSpinBox_SphereRadius, SIGNAL(valueChanged(double)), this, SLOT(editSphereRadius()));
			ui.doubleSpinBox_SphereRadius->setValue(dynamic_cast<SphereGeometry*>(physicsActor->GetGeometry())->GetSphereRadius());
			connect(ui.doubleSpinBox_SphereRadius, SIGNAL(valueChanged(double)), this, SLOT(editSphereRadius()));
		}

		//라디오버튼
		if (physicsActor->GetRigidType() == RigidType::Dynamic)
		{
			ui.radioButton_Dynamic->setChecked(true);
		}
		else if (physicsActor->GetRigidType() == RigidType::Static)
		{
			ui.radioButton_Static->setChecked(true);
		}


		ui.checkBox_UseGravity->setChecked(physicsActor->IsGravity());
		ui.checkBox_IsKinematic->setChecked(physicsActor->IsKinematic());

		disconnect(ui.checkBox_FreezePositionX, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
		disconnect(ui.checkBox_FreezePositionY, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
		disconnect(ui.checkBox_FreezePositionZ, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
		disconnect(ui.checkBox_FreezeRotationX, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
		disconnect(ui.checkBox_FreezeRotationY, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
		disconnect(ui.checkBox_FreezeRotationZ, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
		ui.checkBox_FreezePositionX->setChecked(physicsActor->GetFreezePosition().x);
		ui.checkBox_FreezePositionY->setChecked(physicsActor->GetFreezePosition().y);
		ui.checkBox_FreezePositionZ->setChecked(physicsActor->GetFreezePosition().z);
		ui.checkBox_FreezeRotationX->setChecked(physicsActor->GetFreezeRotation().x);
		ui.checkBox_FreezeRotationY->setChecked(physicsActor->GetFreezeRotation().y);
		ui.checkBox_FreezeRotationZ->setChecked(physicsActor->GetFreezeRotation().z);
		connect(ui.checkBox_FreezePositionX, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
		connect(ui.checkBox_FreezePositionY, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
		connect(ui.checkBox_FreezePositionZ, SIGNAL(stateChanged(int)), this, SLOT(checkFreezePosition()));
		connect(ui.checkBox_FreezeRotationX, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
		connect(ui.checkBox_FreezeRotationY, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
		connect(ui.checkBox_FreezeRotationZ, SIGNAL(stateChanged(int)), this, SLOT(checkFreezeRotation()));
	}
}

void QObjectController::checkGizmo()
{
	MeshRenderer* meshrenderer = DLLGameObject::GetMeshRenderer(m_pCurrentGameObject);

	ui.checkBox_Gizmo->isChecked() ?		//체크되어있으면
		meshrenderer->SetGizmo(true) : meshrenderer->SetGizmo(false);
}

void QObjectController::checkHardwareInstancing(int value)
{
	//아직 없음.
}

void QObjectController::checkReflection(int value)
{
	MeshRenderer* meshrenderer = DLLGameObject::GetMeshRenderer(m_pCurrentGameObject);
	meshrenderer->SetReflection(static_cast<bool>(value));
}

void QObjectController::selectMeshFilter()
{
	MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);

	if (ui.comboBox_MeshList->currentIndex() == -1)
	{
		updateMeshList();
	}

	meshfilter->SetMesh(ui.comboBox_MeshList->currentText().toStdString());
	meshfilter->SetMesh();

	ui.pushButton_fbxTrans->setDisabled(false);
	ui.comboBox_MateralList->setDisabled(false);

	disconnect(ui.comboBox_MateralList, SIGNAL(currentIndexChanged(int)), this, SLOT(setMaterialIndex(int)));
	ui.comboBox_MateralList->clear();
	for (int i = 0; i < meshfilter->GetGraphicMaterialCount(); i++)
	{
		ui.comboBox_MateralList->addItem(meshfilter->GetGraphicMaterialName_char(i));
	}
	connect(ui.comboBox_MateralList, SIGNAL(currentIndexChanged(int)), this, SLOT(setMaterialIndex(int)));

	ui.comboBox_MateralList->setCurrentIndex(0);
}

void QObjectController::AddToGameObejctList(GameObject* object)
{
	for (int i = 0; i < ui.comboBox_ObjectList->count(); i++)
	{
		//같은 명칭의 오브젝트가 존재한다는 뜻
		if (ui.comboBox_ObjectList->findText(QString(DLLGameObject::GetName(object))) != -1)
		{
			QString name = DLLGameObject::GetName(object);
			name.append("_2");

			char buff[100] = { 0, };
			QStringToConstCharPtr(name, buff);
			DLLGameObject::SetName(object, buff);
			break;
		}

	}

	ui.comboBox_ObjectList->addItem(QString(DLLGameObject::GetName(object)));
}

void QObjectController::SetGameObejctWidgetVector()
{
	///// <summary>
	///// 오브젝트 갯수만큼 Q오브젝트 위젯을 생성하고 벡터에 삽입한다.
	///// </summary>
	//for (int index =0; index < DLLEngine::GetGameObjectV(m_pObjectManager.lock()).size(); index++)
	//{
	//	addGameObejctWidget(DLLEngine::GetGameObjectV(m_pObjectManager.lock())[index]);
	//}
}

void QObjectController::SetObjectData()
{
	ui.comboBox_ObjectList->setCurrentText(ConstCharPtrToQString(DLLGameObject::GetName(m_pCurrentGameObject)));

	ui.checkBox_Enable->setChecked(m_pCurrentGameObject->GetIsEnabled());
	ui.lineEdit_ObjectId->setText(QString::number(m_pCurrentGameObject->GetGameObjectId()));
	ui.lineEdit_ObjectName->setText(ConstCharPtrToQString(DLLGameObject::GetName(m_pCurrentGameObject)));

	disconnect(ui.lineEdit_Tag, SIGNAL(editingFinished()), this, SLOT(editTag()));
	ui.lineEdit_Tag->setText(ConstCharPtrToQString(DLLGameObject::GetTag(m_pCurrentGameObject)));
	connect(ui.lineEdit_Tag, SIGNAL(editingFinished()), this, SLOT(editTag()));


	UpdateTransformData();

	//매쉬랜더러 컴포넌트가 존재한다면

	if (DLLGameObject::GetMeshRenderer(m_pCurrentGameObject) != nullptr)
	{
		MeshRenderer* meshrenderer = DLLGameObject::GetMeshRenderer(m_pCurrentGameObject);
		MeshFilter* meshfilter = DLLGameObject::GetMeshFilter(m_pCurrentGameObject);

		ui.pushButton_MeshRendererDelete->setDisabled(false);
		ui.pushButton_MeshRendererAdd->setDisabled(true);

		ui.checkBox_Gizmo->setDisabled(false);
		ui.checkBox_Gizmo->setChecked(meshrenderer->GetIsGizmoOn());

		ui.checkBox_Shadow->setDisabled(false);
		ui.checkBox_Shadow->setChecked(meshrenderer->GetCastShadow());

		ui.checkBox_HardwareInstancing->setDisabled(false);
		ui.checkBox_Reflection->setDisabled(false);
		ui.checkBox_Reflection->setChecked(meshrenderer->GetReflection());

		updateMeshList();
		ui.comboBox_MeshList->setDisabled(false);
		ui.comboBox_MateralList->setDisabled(false);
		//ui.comboBox_MeshList->setCurrentText(ConstCharPtrToQString(strcmp(meshfilter->GetName_char(),"")==0? "box.bin" : meshfilter->GetName_char()));
		ui.comboBox_MeshList->setCurrentText(meshfilter->GetName_char());

		ui.comboBox_Albedo->setDisabled(false);
		ui.comboBox_Normal->setDisabled(false);
		ui.comboBox_MetalRough->setDisabled(false);
		ui.comboBox_Emissive->setDisabled(false);

		ui.comboBox_Albedo->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(0, ALBEDO));
		ui.comboBox_Normal->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(0, NORMAL));
		ui.comboBox_MetalRough->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(0, METALROUGH));
		ui.comboBox_Emissive->setCurrentText(meshfilter->GetGraphicMaterialTextureName_char(0, EMISSIVE));

		ui.horizontalSlider_Metalic->setDisabled(false);
		ui.horizontalSlider_NormalFactor->setDisabled(false);
		ui.horizontalSlider_Shineness->setDisabled(false);
		ui.horizontalSlider_EmissiveFactor->setDisabled(false);

		ui.horizontalSlider_Metalic->setValue(meshfilter->GetMaterialFactor_(0, METALLIC) * 100);
		ui.horizontalSlider_NormalFactor->setValue(meshfilter->GetMaterialFactor_(0, NORMALFACFOR) * 100);
		ui.horizontalSlider_Shineness->setValue(meshfilter->GetMaterialFactor_(0, SHINENESS) * 100);
		ui.horizontalSlider_EmissiveFactor->setValue(meshfilter->GetMaterialFactor_(0, EMISSIVEFACTOR) * 100);
		ui.label_Metalic_2->setText(QString::number(ui.horizontalSlider_Metalic->value()));
		ui.label_NormalFactor_2->setText(QString::number(ui.horizontalSlider_NormalFactor->value()));
		ui.label_Shineness_2->setText(QString::number(ui.horizontalSlider_Shineness->value()));
		ui.label_EmissiveFactor_2->setText(QString::number(ui.horizontalSlider_EmissiveFactor->value()));

	}
	//없다면
	else
	{
		ui.pushButton_MeshRendererDelete->setDisabled(true);
		ui.pushButton_MeshRendererAdd->setDisabled(false);
		ui.pushButton_fbxTrans->setDisabled(true);

		ui.checkBox_Gizmo->setDisabled(true);
		ui.checkBox_Shadow->setDisabled(true);
		ui.checkBox_HardwareInstancing->setDisabled(true);
		ui.checkBox_Reflection->setDisabled(true);

		ui.comboBox_MeshList->setDisabled(true);
		ui.comboBox_MeshList->setCurrentText("");
		ui.comboBox_MateralList->setDisabled(true);

		ui.comboBox_Albedo->setDisabled(true);
		ui.comboBox_Normal->setDisabled(true);
		ui.comboBox_MetalRough->setDisabled(true);
		ui.comboBox_Emissive->setDisabled(true);

		ui.horizontalSlider_Metalic->setDisabled(true);
		ui.horizontalSlider_NormalFactor->setDisabled(true);
		ui.horizontalSlider_Shineness->setDisabled(true);
		ui.horizontalSlider_EmissiveFactor->setDisabled(true);
	}

	if (DLLGameObject::GetLight(m_pCurrentGameObject) != nullptr)
	{
		ui.pushButton_DeleteLight->setDisabled(false);
		ui.pushButton_AddLight->setDisabled(true);
	}
	else
	{
		ui.pushButton_DeleteLight->setDisabled(true);
		ui.pushButton_AddLight->setDisabled(false);
	}

	ReflectionProbeCamera* rfc = DLLGameObject::GetReflectionProbeCam(m_pCurrentGameObject);
	if (rfc != nullptr)
	{
		ui.pushButton_AddReflectionProbe->setDisabled(true);
		ui.pushButton_DeleteReflectionProbe->setDisabled(false);
		ui.pushButton_BakeReflectionProbe->setDisabled(false);
		ui.checkBox_ReflectionRender->setDisabled(false);
		checkReflectionRender(rfc->GetIsRender());
	}
	else
	{
		ui.pushButton_AddReflectionProbe->setDisabled(false);
		ui.pushButton_DeleteReflectionProbe->setDisabled(true);
		ui.pushButton_BakeReflectionProbe->setDisabled(true);
		ui.checkBox_ReflectionRender->setDisabled(true);
	}

	updateMeshList();
	updatePhysicsActor();
}


bool QObjectController::event(QEvent* event)
{
	HWND m_hWnd = reinterpret_cast<HWND>(this->winId());

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
	}

	return QWidget::event(event);
}