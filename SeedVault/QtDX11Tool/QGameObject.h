#pragma once

#include <QDockWidget>
#include "ui_QGameObject.h"

class GameObject;
class QGameObject : public QDockWidget
{
	Q_OBJECT

public:
	QGameObject(QWidget *parent = Q_NULLPTR);
	~QGameObject();

private:
	Ui::QGameObject ui;


private slots:
	void setGameObject(GameObject* object);
	void updateGameObject();

	void editObjectName();
	void selectParent();
	void checkEnable();

signals:
	void createdQGameObject(GameObject* object);

private:
	GameObject* m_pGameObject;

};
