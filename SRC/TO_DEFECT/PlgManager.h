#pragma once

#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\LockManager.h"					// �X���b�h�ԓ�������N���X


// �r�� �C���N���[�h
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

//#ifdef DISABLE_ZZ_EPC
//// EPC �C���N���[�h
//#include "..\..\Include\EPC\NCspeed.h"									// EPC�� UDP���C���N���[�h
//#endif


// ���[�J���̃N���X
#include "MainConst.h"													// ���N���X�萔�w�b�_
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ�
#include "CoilManager.h"												// �R�C�����Ǘ��N���X

#include "SeqFrameRecv.h"												// �V�[�P���T�ʐM��M�N���X
#include "DmemManager.h"												// �f�[�^�������Ǘ��N���X
#include "SogyoManager.h"												// ���Ə�ԊǗ��N���X
#include "EdgePosControl.h"												// �G�b�W���o�������Ǘ��N���X
#include "TrackingFrame.h"												// �t���[���g���b�L���O�N���X
#include "EdgeRestrictSender.h"											// �G�b�W���o�͈͎w�� UDP���M�N���X

#include "..\..\Library\DefectLib\Tracking.h"							// �g���b�L���O�Ǘ��N���X

#include "..\..\Include\KizuSmyu.h"										// �V�~�����[�V�����w�b�_�[

class PlgManager : public ThreadManager
{

public:
	PlgManager();
	virtual ~PlgManager();

private :
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�萔
private:
	static const int MAX_NUM_UDP				= 1;					// UDP�Ǘ��ő吔
	static const int TIME_INTERVAL_DSP_PLG		= 20;					// �\��PLG�̒ʔ��� �F���^�C�}���� [ms]
	static const int TIME_INTERVAL_DSP_LEN		= 200;					// �\����_�ʒu�̃R�C���ʔ��� ���M�^�C�}���� [ms]

	static const int DSP_PLG_NUM				= 1;					// �\���pPLG��

	// �V�~�����[�V�����p
	#define SAMPBUFFNUM				16									// �T���v�����O�o�b�t�@�̃`���l����
	#define SAMPBUFFSIZE			SIZE_FRAME_IMG_Y					// �T���v�����O�o�b�t�@�̃T�C�Y
	#define SAMPCOPYNUM				64									// �T���v�����O�o�b�t�@���R�s�[���鐔

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �v���p�e�B
public:
	void SetLogPlgMgr(LogFileManager* pCls)	{ mcls_pLogPlg = pCls;}		// ���O�N���X

	void SetLockMgr(LockManager* pCls)		{ mcls_pLock = pCls;}		// �X���b�h�ԓ�������N���X
	void SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;};	// �X�e�[�^�X����N���X
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// �p�����[�^�Ǘ��N���X
	void SetCoilMgr(CoilManager* pCls)		{ mcls_pCoil = pCls;}		// �R�C�����Ǘ��N���X
	void SetSogyoMgr(SogyoManager* pCls)	{ mcls_pSogyo = pCls; }		// ���Ə�ԊǗ��N���X
	void SetEdgePosCont(EdgePosControl* p)	{ mcls_pEdgePosCont = p; }	// �G�b�W���o�������Ǘ��N���X
	void SetDmemMgr(DmemManager* pCls)		{ mcls_pDmem = pCls; }		// �f�[�^�������Ǘ��N���X

	void SetSeqFrameR(SeqFrameRecv* pCls)	{ mcls_pSeqFrameR = pCls; }	// �V�[�P���T�ʐM��M�N���X
	void SetEdgeRestMgr(EdgeRestrictSender* pCls)	{ mcls_pEdgeRestSd = pCls; }	// �G�b�W���o�͈͎w�著�M�N���X
	void SetTrFrmMgr(TrackingFrame<TR_FRAME>* pCls)	{ mcls_pTrFrm = pCls; }			// �t���[��No�g���b�L���O�N���X
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �C�x���g���J
public:
	HANDLE	GetEvPLG_KenNotPos()		{ return m_evKenNotPos; }						// ��ʂƈقȂ���̌������u�ʒu �n�ړ_�ʉ߃V�O�i�� [�� MI]
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �O���Q�Ɨp�v���p�e�B
public:	
	// �����Q�Ɨp
	const double* Get_pLenKenDist()			{ return &m_dLen_Ken[m_nDistPos];}	// ��ʌ������u�ʒu�ł̒ʔ��� [mm]
	const double* Get_pLenKenDistNot()		{ return &m_dLen_Ken[m_nDistNotPos]; }	// ��ʂƂ͋t�ʂ̌������u�ʒu�ł̒ʔ��� [mm]
	const double* Get_pLenKenDist(int men)		{ return &m_dLen_Ken[men]; }		// �w�肵���ʂ̌������u�ʒu�ł̒ʔ��� [mm]
	const double* Get_pLenDspDist()			{ return &m_dLen_BaseDsp;}			// ��ʌ������u�ʒu�ł̒ʔ��� [mm]
	bool	IsCoilChange_Ken() const		{ return m_bCoilChange_Ken; }		// �����p�R�C���؂�ւ����� true
	bool	IsCoilChange_Dsp(int pos) const	{ return m_bCoilChange_Dsp[pos]; }	// �\���p�R�C���؂�ւ����� true (�ʒu�w��)
	bool	IsCoilChange_Dsp() const		{ 									// �\���p�R�C���؂�ւ����� true�i�S�ʒu�j
		bool bRet = false;
		for (int ii = 0; ii < POS_DSP_END; ii++) bRet |= m_bCoilChange_Dsp[ii];
		return bRet;
	}


	// ��ʗp�A�N�Z�X
	int		GetFrameIntervalTime()const		{ return m_nTime; }					
	double	GetDistance() const				{ return m_dDistance; }
	DWORD	GetFrameNo() const				{ return m_nFrameNo; }				 
	long	GetSpeed() const				{ return (long)((m_nSpeed * 60.0f / 1000.0f)+0.5f);}	// [mpm]
	long	GetSpeedDsp() const				{ return (long)((m_nSpeedDsp * 60.0f / 1000.0f)+0.5f);}	// [mpm]
	double	GetResV() const					{ return m_dResV; }
	double	GetResVCycle() const			{ return m_dResVCycle; }

	// �ʔ���
	double	GetLen_All(int pos);												// �S�����苗��
	double	GetLen_KenDist() const			{ return m_dLen_Ken[m_nDistPos];}	// ��ʌ������u�ʒu�ł̒ʔ��� [mm]
	double	GetLen_KenDistNot() const		{ return m_dLen_Ken[m_nDistNotPos];}// �����ʌ������u�ʒu�ł̒ʔ��� [mm]
	double	GetLen_OutCut() const			{ return m_dLen_OutCut; }			// �V���[�ʒu�̏o���J�b�g�P�ʂł̒ʔ��� [mm]

	// ������_
	double	GetLen_Ken(int men) const		{ return m_dLen_Ken[men]; }			// �e�������u�ʒu�ł̒ʔ��� [mm] (�\/�� ��)
	double	GetLen_KenSub() const			{ return m_dLen_KenSub; }			// ��ʂ̌������u�ʒu�ł�PLG�T�u�̒ʔ���
	double	GetCoilOldLen_Ken() const		{ return m_dCoilOldLen_Ken; }		// ��ʑ������ʒu�̑O��R�C���� [mm]
	double  GetLen_WpdKenPer() const		{ return m_dLen_WpdKenPer;}			// ������_�n�ړ_��O���� [mm]


	// �\����_
	double	GetLen_BaseDsp() const			{ return m_dLen_BaseDsp; }			// �\����_�ł̒ʔ��� [mm]
	double	GetLen_Dsp(int pos) const			{ return m_dLen_Dsp[pos]; }		// �e�\��  �ʒu�ł̒ʔ��� [mm]
	double	GetCoilOldLen_BaseDsp() const	{ return m_dCoilOldLen_BaseDsp;}	// �\����_�̑O��R�C���� [mm]

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �O���A�N�Z�X(�R�C���ؑ֊֘A)
public:
	void	CoilInit();													// ������
	void	SetEvSampStart()		{ SetEvent(m_evSamplingStart);	}	// �V�O�i���Z�b�g �T���v�����O�X�^�[�g
	void	SetEvSampStop()			{ SetEvent(m_evSamplingStop);	}	// �V�O�i���Z�b�g �T���v�����O�X�g�b�v


	void	CoilChange_KenBase();										// ������_�ŃR�C���؂�ւ�
	void	CoilChange_Ken(int ou);										// �������u�ʒu�ŃR�C���؂�ւ� [��MainInctance]
	void	CoilChange_BaseDsp();										// �\����_�ʒu�ŃR�C���؂�ւ� [��MainInctance]
	void	CoilChange_OutCut();										// �V���[�J�b�g�ʒu�ŏo�������R�C���؂�ւ� (PIO�Ƃ��ő����ʒm��) [��MainInctance]
	void	CoilChange_FirstCut() { m_bCoilOutChange = true; }			// ���X�g�J�b�g�ʒu�ŏo�������R�C���؂�ւ� [��MI]
	void	CoilChange_RecvInit() { m_bRecvInit = true; }				// �C�j�V�����`����M�ɂ��R�C���؂�ւ� [��MI]

	void	SetCoilChange_Frame(int ou, int nFrameNo) { m_nChgFrameNo[ou] = nFrameNo; }	// �R�C���ؑփt���[��No�Z�b�g

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���J�֐�
public:
	void Alloc();														// �֐�������

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ContecCnt8mManager�֐�
public:
	// Set (�K�{�ݒ�)
	void SetDistPostion(int men)	{ m_nDistPos = men; m_nDistNotPos = men==0?1:0; }	// ���(���㗬�̕�)
	void SetDistLen(DWORD len)		{ m_nDistLen = len;}							// �\���ԋ��� [mm]
	// Get
	DWORD	GetDistLen()	const			{ return m_nDistLen; }		// �\���ԋ��� [mm]
	int		GetDistPos()	const			{ return m_nDistPos; }		// ���(���㗬�̕�)�������\��(0:�\,1:��)
	int		GetDistNotPos()	const			{ return m_nDistNotPos; }	// ��ʂƋt�̕�(0:�\,1:��)

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�֐�
private:

	//// ������
	void	ClearMember();												// ������

	// �ʔ����Ǘ��֘A
	void NextLength_Ken(SEQ_TYPE_DATA_FRAME const* p);					// ������_�ł̋����Ǘ� (1�t���[����荞�݊���)
	void NextLength_Dsp();												// �\����_�ł̋����Ǘ�
	void NextLength_Dsp(double dDistance);								// �\����_�ł̋����Ǘ�
	void ExecCycle();													// ���������

	//// ���ю��W�֘A
	void	CheckIntervalStatus();										// �ŏ���Ԃ̏�Ԕc��
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�ϐ�
private:
	// �e�C���X�^���X
	LogFileManager*			mcls_pLogPlg;
	LockManager*			mcls_pLock;									// �X���b�h�ԓ�������N���X
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ�
	CoilManager*			mcls_pCoil;									// �R�C�����Ǘ�
	SogyoManager*			mcls_pSogyo;								// ���Ə�ԊǗ��N���X
	DmemManager*			mcls_pDmem;									// �f�[�^�������Ǘ��N���X
	EdgePosControl*			mcls_pEdgePosCont;							// �G�b�W���o�������Ǘ��N���X
	SeqFrameRecv*			mcls_pSeqFrameR;							// �V�[�P���T�ʐM��M�N���X
	EdgeRestrictSender*		mcls_pEdgeRestSd;							// �G�b�W���o�͈͎w�著�M�N���X
	TrackingFrame<TR_FRAME>*	mcls_pTrFrm;							// �t���[��No�g���b�L���O�N���X

	// ���N���X���Ő���
	KizuPerformanceCounter	mcls_Time;									// ���ԑ���X���b�h
	KizuPerformanceCounter	mcls_TimeDspSpeed;							// �\���p���x�Z�o�p

	// �����V�O�i��
	HANDLE					m_evKenNotPos;								// ��ʂƈقȂ���̌������u�ʒu �n�ړ_�ʉ߃V�O�i��		
	HANDLE					m_evSamplingStart;							// �T���v�����O�X�^�[�g
	HANDLE					m_evSamplingStop;							// �T���v�����O�X�g�b�v
	HANDLE					m_evTimer;									// ������^�C�}�[�p
	HANDLE					m_evTimerDspLen;							// �\�������c���p�ɒ�����^�C�}�[

	// �p���X���[�g
	double					m_dPlsR_Ken;								// �����p �p���X���[�g
	double					m_dPlsR_Dsp;								// �\���p �p���X���[�g
	double					m_dPlsR_Sub;								// ��r�p �p���X���[�g

	// PLG�֘A
//#ifdef DISABLE_ZZ_EPC
//	nc_speed_info			m_udp;										// UDP���M��� (�\������)
//#endif
#ifdef DISABLE_ZZ_EPC
	DEBUG_SPEED_INFO		m_udp;										// PLG	UDP���M��� (�\������) ��ZZ_EPC�p�̑��xUDP
#endif
	TR_FRAME				m_trFrm;									// �g���b�L���O�t���[�����
	int						m_nDly[NUM_MEN];							// �x�����s�L�[ ���s���� (0:���� 1�`:���̒l�Ŏ��s)
	int						m_nTime;									// �t���[���捞���� [ms]
	double					m_dDistance;								// �P�t���[���Ői�񂾋��� [mm]

	DWORD					m_nFrameNo;									// �t���[��No (0�I���W��)
	DWORD					m_nSpeed;									// ���C�����x [mm/sec]
	DWORD					m_nSpeedDsp;								// ���C�����x�i�\��PLG�j [mm/sec]
	double					m_dResV;									// �c����\ [mm/pixel]
	double					m_dResVCycle;								// ����c����\ [mm/pixel] (���������]���̏c����\)

	DWORD					m_nPreExSync;								// ExSync�̑O��l

#ifndef PLG_DSP_KEN_EQAL
	// �\���֘A
	DWORD					m_preCountDsp;								// PLG�̑O��l
#endif
	
	DWORD					m_preLen_KenSub;							// ��r�pPLG���������̑O��l

	DWORD					m_nChgFrameNo[NUM_MEN];						// �R�C���ؑփt���[��No (0�I���W��)

	//=========================================
	// �����Ǘ�
	double					m_dLen_Ken[NUM_MEN];						// �e�������u�ʒu�ł̒ʔ��� [mm] (�ʕ�)
	double					m_dLen_KenSub;								// ��ʂ̌������u�ʒu�ł�PLG�T�u�̒ʔ��� [mm] (�P�Ȃ�f�o�b�N�p)
	double					m_dLen_WpdKenPer;							// �n�ړ_��O���� WPDON���Ɍ������u�ʒu�ŗn�ړ_�������Ƃ��̋��� [mm] (�悤�́AWPD ON�� 10,9���0, -1, -2����ƂȂ�)

	double					m_dLen_BaseDsp;								// �\����_�ł̒ʔ���       [mm] (�悤�͕\����_WPD�ʒu�ł̒ʔ���)
	double					m_dLen_Dsp[MAX_POS_INDEX];					// �e�\���ʒu�ł̒ʔ��� [mm] (�����P��)
	double					m_dLen_OutCut;								// �o���J�b�g�P�ʂł̒ʔ��� [mm] (�悤�͏o�������R�C���P�ʂł̒ʔ���)

	// ���� / �O��l�p (������_�p)
	double					m_dCoilOldLen_Ken;							// ��ʑ������ʒu�̑O��R�C���� [mm]
	bool					m_bCoilChange_Ken;							// ��ʂƂ����Е��̖ʂ̐؂�ւ����� (�؂�ւ��� true) (������_�őS��ON)

	// ���� / �O��l�p (�\����_�p)
	double					m_dCoilOldLen_BaseDsp;						// �\����_WPD�ʒu�̑O��R�C���� [mm]
	bool					m_bCoilChange_Dsp[MAX_POS_INDEX];			// �e�\���̐؂�ւ����� (�؂�ւ��� true)  (�\����_�őS��ON)
	
	//
	//=========================================
	// ���̑��������
	bool					m_bMaxLen_Ken;								// �R�C����������B
	bool					m_bMaxLen_Dsp;

	EM_DIV_PLG				m_emIsLinePLG;								// ���C��PLG�̏��
	EM_DIV_UNTEN			m_emUnten;									// �^�]���
	bool					m_bIsDisableDspPLG;							// �\��PLG����(false:�����l,true:����)
	bool					m_bDebugMode;								// �f�o�b�N�� true
	bool				 	m_bCoilOutChange;							// �o�������R�C���؂�ւ��t���O (false:�����l,true:�t�@�[�X�g�J�b�g)	
	bool				 	m_bRecvInit;								// �C�j�V�����`����M�t���O(false:�����l,true:�C�j�V�����`����M)	

	int						m_nDistPos;									// ���(���㗬�̕�) (0�I���W��)
	int						m_nDistNotPos;								// ���(���㗬�̕�) (0�I���W��)
	int						m_nDistLen;									// �\���ԋ��� [mm]

	// UDP�֘A
	BOOL			SetUdp(int no, const char* ip, WORD port);			// UDP�ݒ�
	SOCKET			GetSock(int no) const	{ return m_sock[no]; }		// UDP�\�P�b�g�C���X�^���X
	sockaddr_in*	GetAddr(int no)			{ return &m_addr[no]; }		// UDP�\�P�b�gsockaddr_in�\����

	SOCKET					m_sock[MAX_NUM_UDP];						// �\�P�b�g�{��		
	sockaddr_in				m_addr[MAX_NUM_UDP];						// sockaddr_in�\����

	DWORD					m_nMin;
	DWORD					m_nMax;

};

