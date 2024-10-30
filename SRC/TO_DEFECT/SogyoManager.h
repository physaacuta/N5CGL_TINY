//=====================================================================
// ���Ə�� �Ǘ��N���X
//	�y�S���C���z
//		�E�ғ����A��~���A�ᑬ���Ƃ̔��f���s���B
//=====================================================================

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

// ���ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J���̃N���X
#include "StatusManager.h"												// �X�e�[�^�X����N���X


using namespace KizuLib;

class SogyoManager : public ThreadManager
{
//// �����o�[�\����
private:

	//=======================================
	// ���Ə��� 臒l �\����
	//=======================================
	struct SOGYO_SETTING {
		bool	bUmu;													// ���̏������L���� True

		int		nLimitSpeed;											// 臒l ���C�����x [mpm]
		int		nCheckCnt;												// 臒l ���񑱂���������Ɉ�v���邩

		int		nNowCnt;												// �������
	};


//// ���J�֐�
public:
	SogyoManager(LPCSTR cSession);
	virtual ~SogyoManager(void);


	//=========================================
	// �v���p�e�B
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls;};		// �X�e�[�^�X����N���X�Z�b�g


	//=========================================
	// �O������֐�

	// ������`�F�b�N�^�C�}�[ 
	void CheckStart(DWORD sampCnt);										// ������`�F�b�N�^�C�}�[�J�n
	void CheckStop();													// ������`�F�b�N�^�C�}�[��~

	// �O���A�N�Z�X
	void SetNowSampCnt(DWORD sampCnt, double dResV);					// ���݂̌����p�T���v�����O�N���b�N�l �Z�b�g [��PlgManager]
#ifdef DSP_SOGYO_CHECK
	void SetNowSampCntDsp(DWORD sampCnt, double dResV);					// ���݂̕\���p�T���v�����O�N���b�N�l �Z�b�g [��PlgManager]
#endif


	//=========================================
	// �f�o�b�N
	void GetIniParam();													// ���I ���Ɣ��������ǂݍ��� [��Dlg]

	// ���
	int GetSpeed() const	{ return m_nMpm; }
#ifdef DSP_SOGYO_CHECK
	int GetSpeedDsp() const { return m_nMpmDsp; }
#endif



//// �����o�[�֐�
private :
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����


	void CheckSampCntDiff();											// �ʔ� ������`�F�b�N
#ifdef DSP_SOGYO_CHECK
	void CheckSampCntDiffDsp();											// �ʔ̒� ������`�F�b�N�\���p
#endif


//// �����o�[�ϐ�
private :


	//// �e�C���X�^���X
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X


	//// �����V�O�i��
	HANDLE					m_evCheckTimer;								// ������^�C�}�[�V�O�i��
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����


	//// �T���v�����O�J�E���g�֘A
	/// �����p
	double					m_dResV;									// �c����\ [mm/pixel]
	__int64					m_pf;										// PC���g��

	__int64					m_pcNow, m_pcPre;							// ���݂̑���_, �O��̑���_
	DWORD					m_nNowSampCnt, m_nPreSampCnt;				// ���݂̃N���b�N�l, �O��̃N���b�N�l

	int						m_nMpm;										// ���݂̃��C�����x [mpm]

#ifdef DSP_SOGYO_CHECK
	/// �\���p
	double					m_dResVDsp;									// �c����\ [mm/pixel]

	__int64					m_pcNowDsp, m_pcPreDsp;						// ���݂̑���_, �O��̑���_
	DWORD					m_nNowSampCntDsp, m_nPreSampCntDsp;			// ���݂̃N���b�N�l, �O��̃N���b�N�l

	int						m_nMpmDsp;									// ���݂̃��C�����x [mpm]
#endif
	//// 臒l
	union {
		SOGYO_SETTING		array[4];
		struct{
			SOGYO_SETTING	InfUpToDown_Slow;							// �ғ�����~���� �ᑬ����
			SOGYO_SETTING	InfUpToDown_Stop;							// �ғ�����~���� ���C����~
			SOGYO_SETTING	InfDownToUp_Slow;							// ��~���ғ����� �ᑬ����
			SOGYO_SETTING	InfDownToUp_Run;							// ��~���ғ����� ���C���ғ�
		};
#ifndef DSP_SOGYO_CHECK
	}						SogyoSetting;
#else
	}						SogyoSetting, SogyoSettingDsp;
#endif

	//// ���̑��������
	CString					m_cSession;									// ini�t�@�C���Z�b�V����
};
