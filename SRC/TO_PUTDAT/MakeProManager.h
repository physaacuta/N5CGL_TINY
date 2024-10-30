#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"						// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"							// SQL�������⏕�N���X
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"						// DIV�萔 ���̎擾�N���X

// ���ʃC���N���[�h
#include "..\..\Include\OutInterface.h"											// �O���C���^�[�t�F�[�X�p�w�b�_�[

// �e���[�J�[�X���b�h
#include "MainConst.h"															// ���N���X�萔�w�b�_�[

class MakeProManager : public ThreadManager
{
//// ���J�\����
public:
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_KEY
	{
		char				cKey[SIZE_SERIAL_FF_NO];							// ��L�[
	};

//// �����o�[�\����	
private:
	//// �������Z�o�p�}�b�v
	struct MIX_RATE_MAP {
		struct GP {
			// �������Z�o�p
			int		nMixRateCnt;					// 1m�P�ʂ̍������� (����Z����ɂ���ꍇ�́A����)
			bool	bFlg[COIL_LEN_MAX];				// 1m�P�ʂł��r���݃}�b�v (false:�܂����� true:���ɗL��)

													// �r��O���[�v�ʂ̌���
			int		nCnt[MAX_PROGRP_TYPE];			// ����
		} gp[MAX_PROGRP_GRADE];						// �v���R���O���[�v(�O���[�h)
	};

//// ���J�֐�
public:
	MakeProManager(void);
	virtual ~MakeProManager(void);

	//// �O���A�N�Z�X
	void SetEvDefectDbStop()			{ SetEvent(m_evDefectDbStop); }			// �����L�����Z��
	void SetEvDefectDbAddOk(int nDbAns)							// �����Ǘ� ���я������݊���
	{
		//AutoLock lock(&m_csLock);

		m_nDefectDbAddAnser = nDbAns;
		SetEvent(m_evDefectDbAdd);
	}

	void SetHosei(double val)													// �␳�����Z�b�g
	{
		AutoLock lock(&m_csLock);

		m_dHosei = val;
		LOG(em_INF), "[%s] �␳���ύX %f", my_sThreadName, m_dHosei);
	}
	
//// ���J�����o�[�ϐ� 
public:
	ThreadQueue<DELIVERY_KEY>	gque_Deli;										// �󂯓n���L���[

//// �����o�[�֐�
private:
	int ThreadFirst();															// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();															// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);												// �X���b�h�C�x���g����

	bool Exec(char const* cID);													// �v���R���������� �`������
	void ClearMember();															// �������ϐ� ������

//------------>>> Exec����̈�A�̏��� (���̒��̊֐����� private�̏������ϐ����g�p�\)
	bool GetBufferCut(char const* cID);											// �v���R���J�b�g���ю�M�o�b�t�@�e�[�u���擾
	bool CheckEqalCut(char const* cID);											// �v���R���J�b�g���ю�M�o�b�t�@�e�[�u���擾
	bool GetCoilInf_KizukenNo();												// �R�C����񂩂�Ǘ�No���擾
	bool CheckDefectInsertOk();													// �����Ǘ���DB�������݊��������H

	void MakeSendData(int nZiErr, PRO_TYPE_ZISEKI* pTypZi);						// �v���R�����M�p ���ѓ`������
	bool CheckState_Kensa();													// �Ώ۔͈� �̌�����Ԃ͐��킩�`�F�b�N
	bool ExecDefectInf(PRO_TYPE_ZISEKI* pTypZi);								// �`���֌������уf�[�^�Z�b�g

	bool InsertZiBuf(const char *cID, const PRO_TYPE_ZISEKI *pTypZi);				// �v���R���p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^
	bool UpdateProconBuf(const char *cID);										// �J�b�g���уe�[�u�� �� �X�V
// <<<-----------

	bool Send_To_Procon(const char *cKey);										// �v���R���p�������ё��M�v��
	void Send_To_Defect(const PRO_TYPE_DATA_CUT *pData);						// �����Ǘ� �Ɏ��я������ݏ󋵖₢���킹���M�v��

//// �����o�[�ϐ�
private:
	// �V�O�i��
	HANDLE						m_evDefectDbStop;								// �����Ǘ� �������݋����I���V�O�i��
	HANDLE						m_evDefectDbAdd;								// �����Ǘ� �������݊����V�O�i��
	
	// ������ (�������̂݃f�[�^���L��)
	OdbcBase					*mcls_pDB;										// DB�ڑ��N���X
	char						m_cKizukenNo[SIZE_KIZUKEN_NO];					// �Ώۂ̊Ǘ�No
	PRO_TYPE_CUT				m_typBuf;										// ��M�f�[�^ (�J�b�g����)

	// ���̑��������
	CRITICAL_SECTION			m_csLock;										// �N���e�B�J���Z�N�V����
	bool						m_bCreateOk;									// ��������
	double						m_dHosei;										// �␳�� [%]
	int							m_nDefectDbAddAnser;							// �����Ǘ� �������݊������� (0:���� 1:�ُ�)

	// ini�t�@�C���l
	int							m_nMaxWaitTime;									// �R�C�����я������ݖ₢���킹�����҂����� [msec]
	int							m_nChkCoilDate;									// �R�C�������̑Ώۊ���

};
