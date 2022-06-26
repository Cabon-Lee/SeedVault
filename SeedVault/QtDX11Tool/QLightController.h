#pragma once

#include <QWidget>
#include "ui_QLightController.h"

class Light;
class QLightController : public QWidget
{
	Q_OBJECT

public:
	QLightController(QWidget *parent = Q_NULLPTR);
	~QLightController();

	void Update();
	void SetLightListItems();
	void SetLightData();
public slots:
	void setCurrnetLight(const QString& objectname);
	void setLightType(int type);
	void setSpotPower(double value);
	void setShadowCheck(int checked);
	void setLightIntensity(int value);
	void setColor();
	void setColor(float r, float g, float b);
	void setFallOffStart(double value);
	void setFallOffEnd(double value);

	void editPosX();
	void editPosY();
	void editPosZ();
	void editRotX();
	void editRotY();
	void editRotZ();
	void editScaleX();
	void editScaleY();
	void editScaleZ();

	void addLightList(const QString& objectname);
	void deleteLightList(const QString& objectname);
	void editedLightList(const QString& prev , const QString& now);


private:
	void UpdateTransformData();


private:
	Ui::QLightController ui;

	Light* m_pCurrentLight;

};
