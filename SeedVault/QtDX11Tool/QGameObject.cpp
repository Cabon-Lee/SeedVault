#include "pch.h"
#include "QGameObject.h"

QGameObject::QGameObject(QWidget *parent)
	: QDockWidget(parent)
	, m_pGameObject(nullptr)
{
	ui.setupUi(this);

	connect(ui.lineEdit_ObjectName, SIGNAL(editingFinished()), this, SLOT(editObjectName));
	connect(this, SIGNAL(createdQGameObject(GameObject*)), this, SLOT(setGameObject(GameObject*)));
}

QGameObject::~QGameObject()
{
}


void QGameObject::setGameObject(GameObject* object)
{
	m_pGameObject = object;

	updateGameObject();
}

void QGameObject::updateGameObject()
{
	ui.checkBox_ObjectEnable->setCheckState(m_pGameObject->GetIsEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	ui.checkBox_ObjectPhysics->setCheckState(m_pGameObject->GetHasPhysics() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	ui.lineEdit_ObjectID->setText(QString::fromStdString(std::to_string(m_pGameObject->GetGameObjectId())));

	ui.comboBox_ObjectParent->clear();
	//ui.comboBox_ObjectParent->addItem(QString::fromStdString(m_pGameObject->GetObjectName()));

}

void QGameObject::editObjectName()
{
	m_pGameObject->SetObjectName(ui.lineEdit_ObjectName->text().toStdString());

}

void QGameObject::selectParent()
{

}

void QGameObject::checkEnable()
{

}
