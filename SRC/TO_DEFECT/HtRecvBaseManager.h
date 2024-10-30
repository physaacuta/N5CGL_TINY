#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X
#include "..\..\Library\KizuLib\Class\LockManager.h"					// �X���b�h�ԓ�������N���X
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// ���Ԍv���Ǘ��N���X
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X

#include "..\..\Library\DefectLib\KizuHtRecv.h"							// ����˓����ԒʐM�Ǘ��N���X


// ���ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// ���C���C���N���[�h

// EPC�w�b�_�[
//#include "..\..\Include\EPC\NCsmemData.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`
//#include "..\..\Include\EPC\NCAttr.h"

//=====================================================================
// �����M�x�[�X�Ǘ��N���X
//	�y�S���C���z
//		�E���聨�����Ԃ̒ʐM����A�X���b�h�N���ȂǊ�{������S��
//=====================================================================

// ���[�J��
#include "StatusBaseManager.h"											// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X


using namespace KizuLib;

class HtRecvBaseManager : public ThreadManager,
						  public KizuHtRecv
{
//// �����o�[�萔
private :
	static const int		TIME_FRAME_SKIP_WAIT		= 10*1000;			// 1��t���[���X�L�b�v�����������ꍇ�A���̎��Ԓ��ُ�ɂ�����
	static const int		TIME_EDGE_NG_WAIT			= 10*1000;			// 1��G�b�W�s�������������ꍇ�A���̎��Ԓ��ُ�ɂ�����

	static const int		TRACKING_ITEM_REPEAT_NUM	= 20;				// �g���b�L���O�A�C�e�����擾����J��Ԃ���
	static const int		TRACKING_ITEM_SLEEP			= 15;				// �g���b�L���O�A�C�e�����擾���s��̃X���[�v����(ms)

	static const int		HT_RECV_EVENT_MAX			= 60;				// �{�N���X�ɂ�����C�x���g������i��ThreadFirst����AddEventNum�̑����͖{��`�����Ƃ��邱�Ɓj

//// ���J�萔
public :
//=======================================
// �����@�\ ���M�˗��L���[
//=======================================
// �����N���X���画��N���X�ւ̎󂯓n���\����
typedef struct {
	int						camset;
	int						framenum;
	int						framepos;

	NCL_DEFECT_RESULT		Resu;									// �������ʏ��
	double					AttrBase[MAX_ATTR_BASE];				// ��{������
	double					AttrAngle[MAX_HT1_CAMANGLE][MAX_ATTR_ANGLE];	// �p�x�ʓ�����
	
	// �摜�f�[�^��JPEG�̏ꍇ
	int						nHeight;								// �摜����
	int						nWidth;									// �摜��
	bool					isBmp;									// true�̏ꍇBMP�摜�Ɣ��肷��
	int						nImgsize[MAX_HT1_CAMANGLE];				// �摜�T�C�Y
	BYTE*					pImg[MAX_HT1_CAMANGLE];					// ��/���摜 �|�C���^
} DeliveryCYCLE;


//// ���J�֐�
public:
	HtRecvBaseManager(const char *cSession, int TorB, bool bDistPostion, int accept_num, int recv_num, int HtCy=0);
	virtual ~HtRecvBaseManager(void);


	//=========================================
	// �v���p�e�B
	void SetLogMgr(LogFileManager* pCls)	{ mcls_pLog = pCls; ThreadManager::SetLogMgr(pCls); KizuHtRecv::SetLogMgr(pCls); };	// ���O�t�@�C���Z�b�g 
	void SetLockMgr(LockManager* pCls)		{ mcls_pLock = pCls;}			// �X���b�h�ԓ�������N���X
	void SetStatusMgr(StatusBaseManager* pCls) { mcls_pStatus = pCls;};		// �X�e�[�^�X����N���X�Z�b�g
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }			// �p�����[�^�Ǘ��N���X�Z�b�g
	void SetOneFrameLen(double dVal) { m_dOneFrameLen = dVal; }
	void SetTrFrmMgr(TrackingFrame<TR_FRAME>* pCls)		{ mcls_pTrFrm = pCls; }	// �t���[��No�g���b�L���O�N���X

	//void SetDistLen(DWORD len)				{ m_nDistLen = len;	}			// �\���ԋ��� [mm]

	//=========================================
	// �O���A�N�Z�X
	bool	IsConnectAll()		const { for(int ii=0; ii<my_nAcceptNum; ii++) { if(STATE_NONE == my_emSockState[ii]) return false; } return true; }	// �S���肪���� [��MainInstance]
	void	SetEvOpNewConnect()	  { SetEvent(m_evOpNewConnect); }		// �V�K�\���@�\�V�K�ڑ�
	int		GetPerFramePos() const { return m_nPerFramePos; }


	// ��ʗp�A�N�Z�X
	DWORD	GetFrameNo(int id)	const { return my_nFrameNo[id]; }		// �t���[��No
	double	GetAveTime()		const { return mcls_Time.GetAveTime(); }// ���Ϗ������� [ms]
	double	GetMaxTime()		const { return mcls_Time.GetMaxTime(); }// �ő又������ [ms]




//// �����o�[�֐�
protected:
	virtual int ThreadFirst();											// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	virtual int ThreadLast();											// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    virtual void ThreadEvent(int nEventNo);								// �X���b�h�C�x���g����
	

	//=========================================
	// ���z�֐�
	// �O�����i�K
	virtual void ThreadFirst_AddHandle() {};							// �V�O�i���҂��s��̒ǉ�
	virtual void OnAddSignal(int nEventNo) {};							// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)

	// �V�O�i����M��
	virtual void OnRecvEnd(int id);										// ��M�����ʒm
	virtual void OnExecDetectWaitStart() {};//= 0;						// �����J�n�҂��J�n����
	virtual void OnExecDetectRun() {};									// �����J�n����
	virtual void OnExecDetectStop() {};//= 0;							// ������~����

	virtual void OnMainDefect()	{};//= 0;								// �Ɩ��`�� ��M��̃��C������
	virtual void OnOpNewConnect() {};//= 0;								// �V�K�\���@�\�V�K�ڑ�

	virtual void OnMainHls(SOCK_F_HEAD* p) {};							// �w���V�[�`�� ���C������ �����̏����Ԃ̓u���b�N����Ă���̂Œ���


	// ����
	virtual const char* OnGetPcName(int id)	{return HANTEI(id); };		// PC���̎擾
	virtual const char* OnGetPcNameCycle(int id)	{return CYCLE(id); };		// PC���̎擾

	// �V�[�P���X����
	void ExecDetectWaitStart();											// �����J�n�҂��J�n����
	void ExecDetectRun();												// �����J�n����
	void ExecDetectStop();												// ������~����
	void MainDefect();													// �Ɩ��`�� ��M��̃��C������



//// �����o�[�ϐ�
protected:

	//=========================================
	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// ThreadManager,KizuHtRecv�����Œ�`����Ă��邽�߁A�����܂��������̂��߂ɐ錾
	LockManager*			mcls_pLock;									// �X���b�h�ԓ�������N���X
	StatusBaseManager*		mcls_pStatus;								// �X�e�[�^�X����N���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ��N���X
	KizuPerformanceCounter	mcls_Time;									// ���ԑ���X���b�h

	TrackingFrame<TR_FRAME>*	mcls_pTrFrm;							// �t���[��No�g���b�L���O�N���X

	//=========================================
	// �V�O�i��
	HANDLE					m_evOpNewConnect;							// �V�K�\���@�\�V�K�ڑ�

	//=========================================
	// ���̑��������
	//DWORD					m_nDistLen;									// �\���ԋ��� [mm]
	bool					m_bDistPostion;								// ��ʂ̏ꍇture
	int						m_nDistPos;									// ���(���㗬�̕�)�������\��(0:�\,1:��)
	int						m_nDistNotPos;								// ��ʂƋt�̕�(0:�\,1:��)

	//long					m_nPerFramePos;								// �O��t���[���̒���ʒu [mm]
	long					m_nNowMinRow;								// ���݂̍ŏ�(1m)�s�ʒu (0�I���W��)
	//long					m_nNowKizuSeq;								// �r�A�� (�\����  1�`)

	int						m_nPerFramePos;								// �����ς݃t���[������ʒu [mm]
	long					m_nNowGridRow;								// ���݂̑��M�O���b�h�ʒu (0�I���W��)
	double					m_dOneFrameLen;								// 1�t���[���̒��� [mm]

	int						m_nHtCy;									// 1:�P������ 2:�������� (GroupID)

	int						m_nTrId;									// �t���[��No�g���b�L���OID
	int						m_nTrOtherId;								// �����ʂ̃t���[��No�g���b�L���OID
	TR_FRAME				m_trFrm;									// �t���[��No�g���b�L���O���

	//=========================================
	// �t���[���X�L�b�v �p	
public:
	void	SetEvFrameSkip()			{ SetEvent(m_evNowFrameSkip); }
	void	SetFrameSkipId(int nVal)	{ m_nFrameSkipId = nVal; }
	bool	IsFrameSkipState() const	{ return m_bFrameSkipState; }

private:
	HANDLE					m_evNowFrameSkip;							// �X�L�b�v����
	HANDLE					m_evTimeFrameSkip;							// �X�L�b�v�^�C�}�[

	// �ێ����
	int						m_nFrameSkipId;								// �X�L�b�v�@����ID
	bool					m_bFrameSkipState;							// �X�L�b�v���

	void FrameSkipStart();												// �X�L�b�v �J�n
	void FrameSkipEnd();												// �X�L�b�v ����


	//=========================================
	// �G�b�W�s�� �p	
public:
	void	SetEvEdgeNg()			{ SetEvent(m_evNowEdgeNg); }
	void	SetEdgeNgId(int nVal)	{ m_nEdgeNgId = nVal; }
	bool	IsEdgeNgState() const	{ return m_bEdgeNgState; }

private:
	HANDLE					m_evNowEdgeNg;								// �G�b�W�s������
	HANDLE					m_evTimeEdgeNg;								// �G�b�W�s���^�C�}�[

	// �ێ����
	int						m_nEdgeNgId;								// �G�b�W�s���@����ID
	bool					m_bEdgeNgState;								// �G�b�W�s�����

	void EdgeNgStart();													// �G�b�W�s�� �J�n
	void EdgeNgEnd();													// �G�b�W�s�� ����
	

	//=========================================
	// �t���[��No���� �p	
public:
	void	SetnFrameUnmatchId(int nVal)	{ m_nFrameUnmatchId = nVal; }
	bool	IsnFrameUnmatchState() const	{ return m_bFrameUnmatchState; }

private:
	// �ێ����
	int						m_nFrameUnmatchId;							// �t���[��No����@����ID
	bool					m_bFrameUnmatchState;						// �t���[��No������

////////////////////////////////////////////////////////////////////////////////////////////////
// ���v���Ԍv�� �p

public:			// ��ʗp�A�N�Z�X
	double	GetHtProcAveTime() const			{ return m_dHtProcAveTime;	}			// �摜�捞�`���聨�����`�������܂ł̕��Ϗ��v���� [ms]
	double	GetHtProcMaxTime() const			{ return m_dHtProcMaxTime;	}			// �摜�捞�`���聨�����`�������܂ł̍ő及�v���� [ms]

	double	GetToProcAveTime() const			{ return m_dToProcAveTime;			}	// �摜�捞�`�������������܂ł̕��Ϗ��v���� [ms]
	double	GetToProcMaxTime() const			{ return m_dToProcMaxTime;			}	// �摜�捞�`�������������܂ł̍ő及�v���� [ms]

	double	GetRecvAveTime() const				{ return m_dRecvProcAveTime;		}	// DS(WS)���r����M�J�n�`WS(DS)���r����M�J�n�܂ł̕��Ϗ������� [ms]
	double	GetRecvMaxTime() const				{ return m_dRecvProcMaxTime;		}	// DS(WS)���r����M�J�n�`WS(DS)���r����M�J�n�܂ł̍ő又������ [ms]

	double	GetRecvTotalAveTime() const			{ return m_dRecvProcTotalAveTime;	}	// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̕��Ϗ������� [ms]
	double	GetRecvTotalMaxTime() const			{ return m_dRecvProcTotalMaxTime;	}	// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̍ő又������ [ms]

	double	GetNcHtProcAveTime(int dsws) const	{ return m_dNcHtProcAveTime[dsws];	}	// �摜�����G���W�����������`�����`���J�n�܂ł̕��Ϗ��v���� [ms]
	double	GetNcHtProcMaxTime(int dsws) const	{ return m_dNcHtProcMaxTime[dsws];	}	// �摜�����G���W�����������`�����`���J�n�܂ł̍ő及�v���� [ms]


protected:
	__int64					m_pf;														// ����PC CPU���g��

	// ����PC
	double					m_dHtProcNowTime;											// �摜�捞�`���聨�����`�������܂ł̌��ݏ��v���� [ms]			
	double					m_dHtProcAveTime;											// �摜�捞�`���聨�����`�������܂ł̕��Ϗ��v���� [ms]
	double					m_dHtProcMaxTime;											// �摜�捞�`���聨�����`�������܂ł̍ő及�v���� [ms]
	// ����PC
	double					m_dToProcNowTime;											// �摜�捞�`�������������܂ł̌��ݏ��v���� [ms]
	double					m_dToProcAveTime;											// �摜�捞�`�������������܂ł̕��Ϗ��v���� [ms]
	double					m_dToProcMaxTime;											// �摜�捞�`�������������܂ł̍ő及�v���� [ms]

	__int64					m_pc_RecvFirst;												// DS(WS)���r����M���̃^�C���X�^���v
	__int64					m_pc_RecvSecond;											// WS(DS)���r����M���̃^�C���X�^���v

	double					m_dRecvProcNowTime;											// DS(WS)���r����M�J�n�`DS(WS)���r����M�J�n�܂ł̌��ݏ��v���� [ms]
	double					m_dRecvProcAveTime;											// DS(WS)���r����M�J�n�`DS(WS)���r����M�J�n�܂ł̕��Ϗ��v���� [ms]
	double					m_dRecvProcMaxTime;											// DS(WS)���r����M�J�n�`DS(WS)���r����M�J�n�܂ł̍ő及�v���� [ms]

	double					m_dRecvProcTotalNowTime;									// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̌��ݏ��v���� [ms]
	double					m_dRecvProcTotalAveTime;									// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̕��Ϗ��v���� [ms]
	double					m_dRecvProcTotalMaxTime;									// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̍ő及�v���� [ms]

	double					m_dNcHtProcNowTime[NUM_CAM_POS];							// �摜�����G���W�����������`�����`���J�n�܂ł̌��ݏ��v���� [ms]
	double					m_dNcHtProcAveTime[NUM_CAM_POS];							// �摜�����G���W�����������`�����`���J�n�܂ł̕��Ϗ��v���� [ms]
	double					m_dNcHtProcMaxTime[NUM_CAM_POS];							// �摜�����G���W�����������`�����`���J�n�܂ł̍ő及�v���� [ms]



////////////////////////////////////////////////////////////////////////////////////////////////
// �o�����������Z�o�p
public:
	void					InitNowOutGridRow()	{ m_nNowOutGridRow = 0; };				// �o�������R�C���P�ʂ̃O���b�h�ʒu��������(��MI)
protected:
	long					m_nNowOutGridRow;											// �o�������R�C���P�ʂ̃O���b�h�ʒu(���ۂɂ�HY���M�͂����A�J�E���g���ɗp����)
	double					m_dNowOutLength;											// �o�������R�C���P�ʂ̒ʔ���(���肩��Ԃ��ė����t���[������ɎZ�o����̂ŁAPlgMgr�̒l�ƈႤ)
	double					m_dResV;													// �c����\ [mm/pixel]
	DWORD					m_nFrameNo;													// �t���[��No


};
