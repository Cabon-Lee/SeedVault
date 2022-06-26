#include "pch.h"
#include <QPalette>
#include <QColorDialog>
#include "QLightController.h"

QLightController::QLightController(QWidget* parent)
	: QWidget(parent)
	, m_pCurrentLight(nullptr)
{
	ui.setupUi(this);

	connect(ui.comboBox_LightList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setCurrnetLight(const QString&)));
	connect(ui.comboBox_LightType, SIGNAL(currentIndexChanged(int)), this, SLOT(setLightType(int)));

	connect(ui.doubleSpinBox_SpotPower, SIGNAL(valueChanged(double)), this, SLOT(setSpotPower(double)));
	connect(ui.checkBox_Shadow, SIGNAL(stateChanged(int)), this, SLOT(setShadowCheck(int)));
	connect(ui.horizontalSlider_Intensity, SIGNAL(valueChanged(int)), this, SLOT(setLightIntensity(int)));
	connect(ui.pushButton_LightColor, SIGNAL(clicked()), this, SLOT(setColor()));
	connect(ui.doubleSpinBox_FallOffStart, SIGNAL(valueChanged(double)), this, SLOT(setFallOffStart(double)));
	connect(ui.doubleSpinBox_FallOffEnd, SIGNAL(valueChanged(double)), this, SLOT(setFallOffEnd(double)));

	connect(ui.doubleSpinBox_PositionX, SIGNAL(valueChanged(double)), this, SLOT(editPosX()));
	connect(ui.doubleSpinBox_PositionY, SIGNAL(valueChanged(double)), this, SLOT(editPosY()));
	connect(ui.doubleSpinBox_PositionZ, SIGNAL(valueChanged(double)), this, SLOT(editPosZ()));

	connect(ui.doubleSpinBox_RotationX, SIGNAL(valueChanged(double)), this, SLOT(editRotX()));
	connect(ui.doubleSpinBox_RotationY, SIGNAL(valueChanged(double)), this, SLOT(editRotY()));
	connect(ui.doubleSpinBox_RotationZ, SIGNAL(valueChanged(double)), this, SLOT(editRotZ()));

	connect(ui.doubleSpinBox_ScaleX, SIGNAL(valueChanged(double)), this, SLOT(editScaleX()));
	connect(ui.doubleSpinBox_ScaleY, SIGNAL(valueChanged(double)), this, SLOT(editScaleY()));
	connect(ui.doubleSpinBox_ScaleZ, SIGNAL(valueChanged(double)), this, SLOT(editScaleZ()));

	QString comboStyle = "QComboBox { combobox-popup: 0; }";

	ui.comboBox_LightList->setStyleSheet(comboStyle);
	ui.comboBox_LightList->setMaxVisibleItems(20);

	ui.comboBox_LightType->setStyleSheet(comboStyle);
}

QLightController::~QLightController()
{
}

void QLightController::Update()
{
	//신 변경을 감지하고 스스로 클리어업데이트한다.
	SetLightListItems();

	//전부 비활성화 시킨다
	if (m_pCurrentLight == nullptr)
	{
		ui.comboBox_LightType->setDisabled(true);
		ui.horizontalSlider_Intensity->setDisabled(true);
		ui.pushButton_LightColor->setDisabled(true);
		ui.doubleSpinBox_FallOffStart->setDisabled(true);
		ui.doubleSpinBox_FallOffEnd->setDisabled(true);
	}
	else
	{
		ui.comboBox_LightType->setDisabled(false);
		ui.horizontalSlider_Intensity->setDisabled(false);
		ui.pushButton_LightColor->setDisabled(false);
		ui.doubleSpinBox_FallOffStart->setDisabled(false);
		ui.doubleSpinBox_FallOffEnd->setDisabled(false);
	}

	UpdateTransformData();
}

void QLightController::SetLightListItems()
{
	static ObjectManager* prevOM = nullptr;
	//신이 변경되었으면 LightList를 재 로드 해야한다. //변경안되었으면 그냥 나가라
	ObjectManager* nowOM = DLLEngine::GetSceneObjectManager(DLLEngine::GetNowScene());
	if (prevOM != nowOM)
	{
		nowOM = DLLEngine::GetSceneObjectManager(DLLEngine::GetNowScene());
		disconnect(ui.comboBox_LightList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setCurrnetLight(const QString&)));

		ui.comboBox_LightList->clear();
		SceneBase* nowScene = DLLEngine::GetNowScene();
		for (int index = 0; DLLEngine::GetGameObject(nowScene, index) != nullptr; index++)
		{
			//Light Component가 검출된 오브젝트의 이름으로 삽입한다.
			if (DLLGameObject::GetLight(DLLEngine::GetGameObject(nowScene, index)) != nullptr)
			{
				ui.comboBox_LightList->addItem(DLLGameObject::GetName(DLLEngine::GetGameObject(nowScene, index)));
			}
		}
		connect(ui.comboBox_LightList, SIGNAL(currentTextChanged(const QString&)), this, SLOT(setCurrnetLight(const QString&)));
		char buff[100] = { 0, };
		QStringToConstCharPtr(ui.comboBox_LightList->currentText(), buff);
		m_pCurrentLight = DLLGameObject::GetLight(DLLEngine::FindGameObjectByName(buff));
		SetLightData();

		prevOM = nowOM;
	}
}

void QLightController::SetLightData()
{
	ui.comboBox_LightType->setCurrentIndex((static_cast<int>(m_pCurrentLight->m_eLightType)));
	ui.checkBox_Shadow->setChecked(m_pCurrentLight->GetCastShadow());
	ui.horizontalSlider_Intensity->setValue(static_cast<int>(m_pCurrentLight->GetIntensity() * 10));
	/*ui.horizontalSlider_SpotPower->setValue(static_cast<int>(m_pCurrentLight->G)*/
	setColor(m_pCurrentLight->GetColor().x, m_pCurrentLight->GetColor().y, m_pCurrentLight->GetColor().z);
	ui.doubleSpinBox_FallOffStart->setValue(static_cast<int>(m_pCurrentLight->GetFalloffStart()));
	ui.doubleSpinBox_FallOffEnd->setValue(static_cast<int>(m_pCurrentLight->GetFalloffEnd()));

	UpdateTransformData();
}

void QLightController::setCurrnetLight(const QString& objectname)
{
	char buff[100] = { 0, };
	QStringToConstCharPtr(objectname, buff);
	m_pCurrentLight = DLLGameObject::GetLight(DLLEngine::FindGameObjectByName(buff));

	SetLightData();
}

void QLightController::setLightType(int type)
{
	m_pCurrentLight->m_eLightType = static_cast<eLIGHT_TYPE>(type);

	if (type == 2)
	{
		ui.label_SpotPower->setVisible(true);
		ui.doubleSpinBox_SpotPower->setVisible(true);
	}
	else
	{
		ui.label_SpotPower->setVisible(false);
		ui.doubleSpinBox_SpotPower->setVisible(false);
	}

}

void QLightController::setSpotPower(double value)
{
	m_pCurrentLight->SetSpotPower((static_cast<float>(value)));
}

void QLightController::setShadowCheck(int checked)
{
	m_pCurrentLight->SetCastShadow(static_cast<bool>(checked));
}

void QLightController::setLightIntensity(int value)
{
	m_pCurrentLight->SetIntensity(static_cast<float>(value) / 10.f);
	ui.label_Intensity_2->setText(QString::number(value));
}

void QLightController::setColor()
{
	QColor color = QColorDialog::getColor();

	//버튼 색을 바꾼다.
	QPalette pal = ui.pushButton_LightColor->palette();
	pal.setColor(QPalette::Button, color);
	ui.pushButton_LightColor->setPalette(pal);

	QString colordata = "R : " + QString::number(color.redF(), 'f', 2) + " G : " + QString::number(color.greenF(), 'f', 2) + " B : " + QString::number(color.blueF(), 'f', 2);
	ui.pushButton_LightColor->setText(colordata);
	m_pCurrentLight->SetColor(color.redF(), color.greenF(), color.blueF());
}

void QLightController::setColor(float r, float g, float b)
{
	QColor color;
	color.setRgbF(r, g, b);

	//버튼 색을 바꾼다.
	QPalette pal = ui.pushButton_LightColor->palette();
	pal.setColor(QPalette::Button, color);
	ui.pushButton_LightColor->setPalette(pal);

	QString colordata = "R : " + QString::number(r, 'f', 2) + " G : " + QString::number(g, 'f', 2) + " B : " + QString::number(b, 'f', 2);
	ui.pushButton_LightColor->setText(colordata);
	m_pCurrentLight->SetColor(color.redF(), color.greenF(), color.blueF());
}

void QLightController::setFallOffStart(double value)
{
	m_pCurrentLight->SetFalloffStart(static_cast<float>(value));
}


void QLightController::setFallOffEnd(double value)
{
	m_pCurrentLight->SetFalloffEnd(static_cast<float>(value));
}

void QLightController::addLightList(const QString& objectname)
{
	ui.comboBox_LightList->addItem(objectname);
}

void QLightController::deleteLightList(const QString& objectname)
{
	int index = ui.comboBox_LightList->findText(objectname);
	ui.comboBox_LightList->removeItem(index);
}

void QLightController::editedLightList(const QString& prev, const QString& now)
{
	int index = ui.comboBox_LightList->findText(prev);
	ui.comboBox_LightList->setItemText(index, now);
}

void QLightController::UpdateTransformData()
{
	if (ui.doubleSpinBox_PositionX->hasFocus() == true || ui.doubleSpinBox_PositionY->hasFocus() == true || ui.doubleSpinBox_PositionZ->hasFocus() == true ||
		ui.doubleSpinBox_RotationX->hasFocus() == true || ui.doubleSpinBox_RotationY->hasFocus() == true || ui.doubleSpinBox_RotationZ->hasFocus() == true ||
		ui.doubleSpinBox_ScaleX->hasFocus() == true || ui.doubleSpinBox_ScaleY->hasFocus() == true || ui.doubleSpinBox_ScaleZ->hasFocus() == true)
	{
		return;
	}

	ui.doubleSpinBox_PositionX->setValue(m_pCurrentLight->m_Transform->m_Position.x);
	ui.doubleSpinBox_PositionY->setValue(m_pCurrentLight->m_Transform->m_Position.y);
	ui.doubleSpinBox_PositionZ->setValue(m_pCurrentLight->m_Transform->m_Position.z);

	ui.doubleSpinBox_RotationX->setValue(m_pCurrentLight->m_Transform->m_EulerAngles.x);
	ui.doubleSpinBox_RotationY->setValue(m_pCurrentLight->m_Transform->m_EulerAngles.y);
	ui.doubleSpinBox_RotationZ->setValue(m_pCurrentLight->m_Transform->m_EulerAngles.z);

	ui.doubleSpinBox_ScaleX->setValue(m_pCurrentLight->m_Transform->m_Scale.x);
	ui.doubleSpinBox_ScaleY->setValue(m_pCurrentLight->m_Transform->m_Scale.y);
	ui.doubleSpinBox_ScaleZ->setValue(m_pCurrentLight->m_Transform->m_Scale.z);
}

void QLightController::editPosX()
{
	m_pCurrentLight->m_Transform->m_Position.x = ui.doubleSpinBox_PositionX->value();
	m_pCurrentLight->m_Transform->SetPosition({ m_pCurrentLight->m_Transform->m_Position.x,
													 m_pCurrentLight->m_Transform->m_Position.y,
													 m_pCurrentLight->m_Transform->m_Position.z });
}

void QLightController::editPosY()
{
	m_pCurrentLight->m_Transform->m_Position.y = ui.doubleSpinBox_PositionY->value();
	m_pCurrentLight->m_Transform->SetPosition({ m_pCurrentLight->m_Transform->m_Position.x,
													 m_pCurrentLight->m_Transform->m_Position.y,
													 m_pCurrentLight->m_Transform->m_Position.z });
}

void QLightController::editPosZ()
{
	m_pCurrentLight->m_Transform->m_Position.z = ui.doubleSpinBox_PositionZ->value();
	m_pCurrentLight->m_Transform->SetPosition({ m_pCurrentLight->m_Transform->m_Position.x,
													 m_pCurrentLight->m_Transform->m_Position.y,
													 m_pCurrentLight->m_Transform->m_Position.z });
}

void QLightController::editRotX()
{
	m_pCurrentLight->m_Transform->m_EulerAngles.x = ui.doubleSpinBox_RotationX->value();
	m_pCurrentLight->m_Transform->SetRotationFromVec({ m_pCurrentLight->m_Transform->m_EulerAngles.x,
														   m_pCurrentLight->m_Transform->m_EulerAngles.y,
														   m_pCurrentLight->m_Transform->m_EulerAngles.z });
}

void QLightController::editRotY()
{
	m_pCurrentLight->m_Transform->m_EulerAngles.y = ui.doubleSpinBox_RotationY->value();
	m_pCurrentLight->m_Transform->SetRotationFromVec({ m_pCurrentLight->m_Transform->m_EulerAngles.x,
															m_pCurrentLight->m_Transform->m_EulerAngles.y,
															m_pCurrentLight->m_Transform->m_EulerAngles.z });
}

void QLightController::editRotZ()
{

	m_pCurrentLight->m_Transform->m_EulerAngles.z = ui.doubleSpinBox_RotationZ->value();
	m_pCurrentLight->m_Transform->SetRotationFromVec({ m_pCurrentLight->m_Transform->m_EulerAngles.x,
															m_pCurrentLight->m_Transform->m_EulerAngles.y,
															m_pCurrentLight->m_Transform->m_EulerAngles.z });
}

void QLightController::editScaleX()
{
	m_pCurrentLight->m_Transform->m_Scale.x = ui.doubleSpinBox_ScaleX->value();
}

void QLightController::editScaleY()
{
	m_pCurrentLight->m_Transform->m_Scale.y = ui.doubleSpinBox_ScaleY->value();
}

void QLightController::editScaleZ()
{
	m_pCurrentLight->m_Transform->m_Scale.z = ui.doubleSpinBox_ScaleZ->value();
}