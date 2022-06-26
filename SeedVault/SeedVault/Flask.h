#pragma once
#include "IItem.h"

/// <summary>
/// �ö�ũ ������ Ŭ����
/// ��ô���� �� ������ ���ư��� (������Ʈ�� �ٴ� ������Ʈ)���� �ƴ�
/// �κ��丮���� �����νḸ �����Ѵ�.
/// 
/// ����(���콺��Ŭ��)���� ��ô�� �����ϰ�
/// ������ �ϸ� ���ο� �������� ���� �� �ִ�.
/// 
/// �ö�ũ + ���ڿ� = ȭ�� �ö�ũ
/// �ö�ũ + ���   = �� �ֻ��
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

class Flask : public EquipmentItem
{
public:
	Flask(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~Flask();

	virtual bool Use() override;
	virtual void CoolDown() override;
	virtual bool Throw();

	bool IsThrowing() const;

protected:
	float m_ThrowingSpeed;	// ��ô ���� �ӵ�

private:

};

/// <summary>
/// ȭ�� �ö�ũ Ŭ����
/// 
/// ��ô�ϸ� ȭ���ö�ũ�� �����ؼ� ������
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class FireFlask : public Flask
{
public:
	FireFlask(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~FireFlask();

	virtual bool Use() override;
	virtual void CoolDown() override;
	virtual bool Throw() override;

protected:

private:

};



