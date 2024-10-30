#pragma once
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

// �r�� �C���N���[�h
#include "..\..\include\ToHtCommon.h"									// ��������p�̒�`�w�b�_


// �e���[�J�[�X���b�h
#include "MainConst.h"													// ���N���X�萔�w�b�_�[


// �t�@���N�V�����̎g���������AThreadManager���p�����Ă����B���[�J�[�X���b�h�͋N�����Ȃ�
class StatusManager : public ThreadManager
{
//// ���J�萔
public:
	// �^�X�N �X�e�[�^�X
	enum MAIN_STATE
	{		
		STATE_NON = 0,			// ��������
		STATE_INIT,				// ������� (�����J�����)
		STATE_WAIT,				// �������� (������L���)
		STATE_RUN,				// ������   (�I�����C��)
		STATE_CANNOT			// ���s�s�\
	};


//// ���J�֐�
public:
	StatusManager(void);
	virtual ~StatusManager(void);


	// �^�X�N�X�e�[�^�X����
	bool			IsRunning()			const { return STATE_RUN == m_emMainState ? true : false;}	// ������:true
	MAIN_STATE		GetMainState()		const { return m_emMainState; }								// ���݂̃X�e�[�^�X�擾
	CString			GetMainStateName()	const { return GetMainStateName(m_emMainState); };			// ���݂̃X�e�[�^�X���̎擾
	CString			GetMainStateName(MAIN_STATE st) const;											// �w��̃X�e�[�^�X���̎擾
	bool			SetMainState(MAIN_STATE st);													// �X�e�[�^�X�ύX

	// �^�]���[�h
	EM_DIV_UNTEN	GetUnten()			const { return m_emUnten; }											// ���݂̉^�]���[�h�擾
	CString			GetUntenName()		const { return GetUntenName(m_emUnten); };							// ���݂̉^�]���[�h���̎擾
	CString			GetUntenName(EM_DIV_UNTEN unten) const { return DivNameManager::GetDivUnten(unten); };	// �w��̉^�]���[�h���̎擾
	bool			SetUnten(EM_DIV_UNTEN unten);													// �^�]���[�h�ύX


	// �J�����y�A�֌W
	bool			GetParam_CamPairInf();											// �p�����[�^�擾
	int				GetCamPairItemNum(int nCamPair, EM_DIV_HANTEI em) {
							if(em == DIV_HANTEI_HT)		return m_CamPairInf.item[nCamPair-1].num;
							else if(em == DIV_HANTEI_CY)return m_CamPairInf.item_cycle[nCamPair-1].num;
							else if(em == DIV_HANTEI_PR)return m_CamPairInf.item_para[0].num;
							else return 0;
	}
	int				GetCamPairItemCamset(int nCamPair, int index, EM_DIV_HANTEI em) {
							if(em == DIV_HANTEI_HT)		return m_CamPairInf.item[nCamPair-1].camset[index];
							else if(em == DIV_HANTEI_CY)return m_CamPairInf.item_cycle[nCamPair - 1].camset[index];
							else if(em == DIV_HANTEI_PR)return m_CamPairInf.item_para[0].camset[index];
							else return 0;
	}
	// �p�������̓J�����Z�b�g�w��Ȃ̂ŊO������Z�b�g����K�v������
	void			SetParaCamset(int nCamSet1, int nCamSet2) {
							m_CamPairInf.item_para[0].camset[0] = nCamSet1;
							m_CamPairInf.item_para[0].camset[1] = nCamSet2;
							m_CamPairInf.item_para[0].num		= 2;
	}

//// �����o�[�֐�
private:
	// ���[�J�[�X���b�h�Ƃ��ē��삳���Ȃ����߁A�֌W�֐������J�ɕύX
	int				ThreadFirst()				{return 0;}				// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int				ThreadLast()				{return 0;}				// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void			ThreadEvent(int nEventNo)	{;}						// �X���b�h�C�x���g����


//// �����o�[�ϐ�
private:

	//// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����

	//// �X�e�[�^�X���
	MAIN_STATE				m_emMainState;								// �^�X�N�X�e�[�^�X
	EM_DIV_UNTEN			m_emUnten;									// �^�]���[�h

	CAMPAIR_INF				m_CamPairInf;								// �S�J�����y�A��` (�e����@�\���S������J�����Z�b�g�Q) 




//// �e���ԕێ�
public :
	bool IsEngStatNew() const { return m_bGetEngStatNew; }
	void SetEngStat(ENG_STAT const*  pTyp) { AutoLock lock(&m_csLock); m_bGetEngStatNew=true; memcpy( &m_EngStat, pTyp, sizeof(m_EngStat)); }
	void GetEngStat(ENG_STAT*  pTyp) { AutoLock lock(&m_csLock); m_bGetEngStatNew=false; memcpy( pTyp, &m_EngStat, sizeof(m_EngStat)); }


private:
	bool					m_bGetEngStatNew;
	ENG_STAT				m_EngStat;									// �G���W����� (�ŐV)


};
