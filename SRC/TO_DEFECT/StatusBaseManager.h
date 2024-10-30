//=====================================================================
// �X�e�[�^�X�x�[�X�Ǘ��N���X
//	�y�S���C���z
//		�E�r���ň�ʓI�ȃX�e�[�^�X���Ǘ�
//		�E�@���Ԃ̓ǂݍ��ݏ������s��
//=====================================================================

#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

#include "..\..\Library\DefectLib\KikiManager.h"						// �@���ԊǗ��N���X

// ���ʃw�b�_�[
#include "..\..\Include\ToHtCommon.h"									// ��������p�C���N���[�h

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "StatusLogFunc.h"												// �X���[�^�X���O�N���X

using namespace KizuLib;

class StatusBaseManager : public ThreadManager
{

public:
	//// �󂯓n���\����
	struct TYP_MY_ERRWAR_INF {
		bool			bErr;											// ��敪 (true:Err false:War) Err���́A������~
		int				nKind;											// �敪
		int				nSt[2];
	};

protected :
	//// ���[�J���萔
	static const int PARAM_DB_READ_INTERVAL_TIME	= 8000;//3000;		// DB�`�F�b�N���� [ms]	�� �̏Ⴭ�O�`�F�b�N���Ԃ��͂�����ƒZ������
	static const int PARAM_SYSLOG_BASENO			= 300;				// �V�X���O�̊���No (��������10�R�́A���̃N���X��p�Ƃ���)


public:
	StatusBaseManager(void);
	virtual ~StatusBaseManager(void);


	//=========================================
	// �v���p�e�B
	void SetStatusFunc(StatusLogFunc* pCls) { mcls_pStatusFunc = pCls; }	// �X�e�[�^�X���O



	//=========================================
	// �����`
	void SetDspSyslog(bool bUmu)	{ m_bDspSyslog = bUmu; }				// �@���ԂփV�X���O�ǉ� �L��
	void SetAddStlog(bool bUmu)		{ m_bAddStlog = bUmu; }					// �X�e�[�^�X���O�ǉ� �L��
	



	//=========================================
	// �^�X�N�X�e�[�^�X��Ԏ擾
	bool			IsDoSequence();																		// �V�[�P���X���s�����Ԃ�
	bool			IsRun()						  const { return m_emKadou==DIV_KADOU_START ? true : false; }	// ������
	bool			GetMenMode()				  const { return m_bMenMode;}							// �Жʌ����L��  ����:true / �s����:false 
	int				GetMenState()				  const { return m_nMenState;}							// �����ʏ�� (-1:���ʌ��� 0:�\�݂̂Ŏ��{ 1:���݂̂Ŏ��{)

	EM_DIV_PLG		GetPlg()					  const { return m_emPlg; }								// PLG�敪�擾
	EM_DIV_UNTEN	GetUnten()					  const { return m_emUnten; }							// �^�]��Ԏ擾
	EM_DIV_SOGYO	GetSogyo()					  const { return m_emSogyo; }							// ���Ə�Ԏ擾
#ifdef DSP_SOGYO_CHECK
	EM_DIV_SOGYO	GetSogyoDsp()				  const { return m_emSogyoDsp; }						// ���Ə�Ԏ擾 �\���p
#endif
	EM_DIV_KADOU	GetKadou()					  const { return m_emKadou; }							// �ғ����
	EM_DIV_SYS		GetSys(int men)				  const { return m_emSys[men]; }						// �V�X�e����Ԏ擾
	EM_DIV_SYS		GetCycleSys(int men)		  const	{ return m_emCycleSys[men];	}					// �����V�X�e����Ԏ擾			
	EM_DIV_SYS		GetParaSys(int pcno)		  const { return m_emParaSys[pcno]; }					// �p�������V�X�e����Ԏ擾

	EM_DIV_KENSA 	GetKensa(int men)			  const { return m_emKensa[men]; }						// ������Ԏ擾
	EM_DIV_KIKI 	GetKiki(int men)			  const { return m_emKiki[men]; }						// �@���Ԏ擾


	CString			GetMenModeName(bool st)		  const { return st?"�Жʌ�������":"�Жʌ����s����(���ʕK�{)"; };	// �w��̕Жʌ����L�����̎擾 
	CString			GetHanteiName(EM_DIV_HANTEI st)
		{
				if( DIV_HANTEI_HT == st )   return "�P������";
				if( DIV_HANTEI_CY == st )   return "��������";
				if( DIV_HANTEI_PR == st )   return "�p����������";
				return "�s��";
		};
	CString			GetMarkStateName(bool st)	  const { return st?"����":"�ُ�"; };					// �}�[�L���O���u���

	EM_DIV_PARA_KADOU Get_ParaKadou(EM_DIV_SYS nParaSys)												// �p������PC �V�X�e����Ԃ��ғ���Ԃɕϊ��iRAS�\���ASO_GAMENN���M�p�j
	{
		EM_DIV_PARA_KADOU nKadou = DIV_PARA_KADOU_STOP;

		switch (nParaSys)
		{
			// [�V�X�e�����] ������~�҂�
		case DIV_SYS_WAITSTOP:
			nKadou = DIV_PARA_KADOU_WAITSTOP;
			break;

			// [�V�X�e�����] �����J�n��������
		case DIV_SYS_INITSTART:
			// [�V�X�e�����] �����J�n�҂�
		case DIV_SYS_WAITSTART:
			// [�V�X�e�����] ���������ċN����
		case DIV_SYS_RETRY:
			nKadou = DIV_PARA_KADOU_WAITSTART;
			break;

			// [�V�X�e�����] ������
		case DIV_SYS_RUN:
			nKadou = DIV_PARA_KADOU_START;
			break;

			// [�V�X�e�����] ��~���[�h
		case DIV_SYS_INIT:
			// [�V�X�e�����] ��������
		case DIV_SYS_IDLE:
			// [�V�X�e�����] ������~
		case DIV_SYS_STOP:
		default:
			nKadou = DIV_PARA_KADOU_STOP;
			break;
		}

		return nKadou;
	};

	//=========================================
	// ��Ԑݒ�
	bool			SetMenMode(bool st);																// �Жʌ����L��
	bool			SetMenState(int st);																// �����ʏ�ԕύX
	bool			SetPlg(EM_DIV_PLG st);																// PLG�敪�ύX
	bool			SetUnten(EM_DIV_UNTEN st);															// �^�]��ԕύX
	bool			SetSogyo(EM_DIV_SOGYO st, int nMpm);												// ���Ə�ԕύX
#ifdef DSP_SOGYO_CHECK
	bool			SetSogyoDsp(EM_DIV_SOGYO st, int nMpm);												// ���Ə�ԕύX �\���p
#endif
	bool			SetKadou(EM_DIV_KADOU st);															// �ғ���ԕύX
	bool			SetSys(int men, EM_DIV_SYS st);														// �V�X�e����ԕύX
	bool			SetCycleSys(int men, EM_DIV_SYS st);												// �����V�X�e����ԕύX
	bool			SetParaSys(int pcno, EM_DIV_SYS st);												// �p�������V�X�e����ԕύX

	//=========================================
	// �������ُ�c��
	inline ThreadQueue<TYP_MY_ERRWAR_INF>&	GetQueMyErrWar()	{ return mque_MyErrWar; }
	virtual void	AddQueMyErrWar(bool bErr, int nKind, int nSt1=0, int nSt2=0);						// �������ُ픭��
	virtual bool	OnMyErrWar(TYP_MY_ERRWAR_INF const* pInf) { return false;};							// �������̏�Ԕc�� (���^�X�N���ُ̈�c��)


	//=========================================
	// �@���Ԏ擾
	const TYP_STATUS_READ& GetStatusRead()								// ���݂̋@���Ԉꗗ��Ԃ�
		{ 
			AutoLock lock(&m_csLock);
			return m_oldStateRead;
		}
	const TYP_STATUS_NOWSYSTEM& GetStatusNowSystem()					// ���݂̋@���Ԉꗗ��Ԃ�
		{ 
			AutoLock lock(&m_csLock); 
			return m_oldNowSystem;
		} 


	//=========================================
	// �X�e�[�^�X����
	inline KikiManager const& GetKikiMgr()	const	{ return (KikiManager&)mcls_pKiki; } 


	//// �@���� �񓯊���������
	void SetStatusQue(int id, int state, bool bLogDsp =true, char const * cMsg = "" )   {mcls_pKiki->SetStatusQue(id,state,bLogDsp,cMsg);}
	void SetStatusQue(int id, bool state, bool bLogDsp =true, char const * cMsg  = "" ) {mcls_pKiki->SetStatusQue(id,state,bLogDsp,cMsg);}	
	void SetStatusQue(char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" ) {mcls_pKiki->SetStatusQue(cKey,state,bLogDsp,cMsg);}
	void SetStatusQue(char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" )  {mcls_pKiki->SetStatusQue(cKey,state,bLogDsp,cMsg);}
	void SetStatusQue(char const * cKeyFormat, int id, int state, bool bLogDsp =true, char const * cMsg = "" )  
			{	char cKey[256]; memset(cKey, 0x00, sizeof(cKey));
				sprintf_s(cKey, sizeof(cKey), cKeyFormat, id);
				mcls_pKiki->SetStatusQue(cKey,state,bLogDsp,cMsg);
			}

	//// �@���� �񓯊��ǂݍ��ݗv��
	void SetStatusRead() { mcls_pKiki->SetStatusRead(); }				// (�񓯊�)�����Ǎ��݊J�n�C�x���g�B����������ʏ�ʂ� m_evReadEnd ���V�O�i���ƂȂ�
	void RefreshStatus() { mcls_pKiki->Refresh(); ReadKikiStatus(); }	// �@���� �����X�V



//// �����o�[�֐�
protected:
	int				ThreadFirst();										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int				ThreadLast();										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void			ThreadEvent(int nEventNo);							// �X���b�h�C�x���g����

	void			ReadKikiStatus();									// �@���Ԏ擾 & ��ԃ`�F�b�N
	void			CheckStatusLog();									// �X�e�[�^�X���`�F�b�N

	//=========================================
	// ���z�֐�
	virtual void	OnReadKikiStatusEnd(bool bChange) {};				// �@���Ԏ擾 & ��ԃ`�F�b�N ������
	virtual void	ThreadFirst_AddHandle() {};							// �V�O�i���҂��s��̒ǉ�
	virtual void	OnAddSignal(int nEventNo) {};						// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)



//// ���̑��������
protected :

	//=========================================
	// �e�C���X�^���X
	KikiManager*			mcls_pKiki;									// �@���ԊǗ��N���X
	StatusLogFunc*			mcls_pStatusFunc;							// �X�e�[�^�X���O�N���X		

	//=========================================
	// �󂯓n���L���[
	ThreadQueue<SOCK_BASE_HEAD>*	mque_pOpSend;						// �������\�����M�ʒm�̃L���[
	ThreadQueue<TYP_MY_ERRWAR_INF>	mque_MyErrWar;						// �������̏�Ԕc��


	//=========================================
	// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����


	//=========================================
	// �Ǘ��X�e�[�^�X
	bool					m_bMenMode;									// �Жʌ�������:true / �s����:false 
	int						m_nMenState;								// �����ʏ�� (-1:���ʌ��� 0:�\�݂̂Ŏ��{ 1:���݂̂Ŏ��{)
	EM_DIV_PLG				m_emPlg;									// PLG�敪
	EM_DIV_UNTEN			m_emUnten;									// �^�]���
	EM_DIV_SOGYO			m_emSogyo;									// ���Ə��
#ifdef DSP_SOGYO_CHECK
	EM_DIV_SOGYO			m_emSogyoDsp;								// ���Ə�� �\���p
#endif
	EM_DIV_KADOU			m_emKadou;									// �ғ����
	EM_DIV_SYS				m_emSys[NUM_MEN];							// �V�X�e����� [��]
	EM_DIV_SYS				m_emCycleSys[NUM_MEN];						// ��������@�\�P�Ƃ̃V�X�e����� [��]
	EM_DIV_SYS				m_emParaSys[NUM_ALL_HTPARA];				// �p����������@�\�P�Ƃ̃V�X�e����� [��]
	EM_DIV_KENSA			m_emKensa[NUM_MEN];							// ������� [��]
	EM_DIV_KIKI				m_emKiki[NUM_MEN];							// �@���� [��]


	//=========================================
	// �X�e�[�^�X
	TYP_STATUS_NOWSYSTEM	m_oldNowSystem;								// �O��`�F�b�N���̋@����
	TYP_STATUS_READ			m_oldStateRead;								// �O��`�F�b�N���̑S�@����


	//=========================================
	// ���
	char					m_cKizukenNo[SIZE_KIZUKEN_NO];				// �Ǘ�No
	COIL_BASEDATA*			m_pCoilBase;								// ��{���

	//=========================================
	// �����`
	bool					m_bDspSyslog;								// �@���ԂփV�X���O�ǉ� �L��
	bool					m_bAddStlog;								// �X�e�[�^�X���O�ǉ� �L��
};
