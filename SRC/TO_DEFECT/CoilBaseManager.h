//=====================================================================
// �R�C������{�Ǘ��N���X
//	�y�S���C���z
//		�E�R�C���̊�{�V�[�P���X�A�o�b�t�@�̈�̊Ǘ����s��
//		�E��{�I�ɂ́A�S���C���قƂ�Ǔ���\���Ƃł���Ǝv�����A���S���p�ɂ͂ł��Ȃ��Ǝv���Ă���
//=====================================================================

#pragma once

#include <winsock2.h>													// UDP���M�p

// �ӃփN���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\DefectLib\TrackingList.h"						// �����g���b�L���O�Ǘ��N���X

// ���ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Include\ToTbl.h"										// �������L�������e�[�u�� �C���N���[�h
#include "..\..\Include\OutInterface.h"									// �O���C���^�[�t�F�[�X�p�C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ�
#include "DBManager.h"													// DB�o�^����N���X
#include "EdgePosControl.h"												// �G�b�W���o�������Ǘ��N���X
#include "MaeManager.h"													// �O�H�����Ǘ��N���X
#include "DmemManager.h"												// �f�[�^�������Ǘ��N���X

#ifdef ENABLE_RPI_SAVE
	#include "IfRpiSave.h"												// �������S���摜�����Ƃ�I/F�N���X
#endif


using namespace KizuLib;

class CoilBaseManager :	public ThreadManager
{

public:
	CoilBaseManager(void);
	virtual ~CoilBaseManager(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v���p�e�B
public:
	//// ���[�N�X���b�h
	void SetStatusMgr(StatusManager* pCls)		{ mcls_pStatus = pCls; }					// �X�e�[�^�X����N���X�Z�b�g	
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }						// �p�����[�^�Ǘ��N���X�Z�b�g
	void SetDbMgr(DBManager* pCls)				{ mcls_pDbMgr = pCls; }						// DB�o�^����N���X
	void SetEdgePosCont(EdgePosControl* p)		{ mcls_pEdgePosCont = p; }					// �G�b�W���o�������Ǘ��N���X
	void SetMaeMgr(MaeManager* pCls)			{ mcls_pMae = pCls; }						// �O�H�����Ǘ�
	void SetDmemMgr(DmemManager* pCls)			{ mcls_pDmemMgr  = pCls; }					// �f�[�^�������Ǘ��N���X�Z�b�g

	//// �󂯓n���p�L���[
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData = pQue;}	// DB�o�^�p�̃L���[

	//// ���̑����X
	void SetDistPostion(int men){ m_nDistPos=men; m_nDistNotPos= 0==men?1:0; }				// ���(���㗬�̕�)�������\��(0:�\,1:��)

	//// �X���b�h�ւ̒ʒm�p�V�O�i��
	void SetEvNextCoilKen()		{ SetEvent(m_evNextCoilKen); }								// �����p �R�C������M�V�O�i��
	void SetEvNextCoilDsp()		{ SetEvent(m_evNextCoilDsp); }								// �\���p �R�C������M�V�O�i��

	//// �N���e�B�J���Z�N�V����
	CRITICAL_SECTION* GetCsLock() { return &m_csLock; }										// [��MainInstance�o�R�� OpSendFunc]


	void SetTrackingList(TrackingList* pTl) { mcls_pTlCoilOutSt = pTl; }					// �o�������R�C���P�ʂ̏�ԊǗ��p�g���b�L���O



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ʃA�N�Z�X�p
public:
	//// �N���e�B�J���Z�N�V����
	void SetCsLock()			{ EnterCriticalSection(&m_csLock); }
	void SetCsUnLock()			{ LeaveCriticalSection(&m_csLock); }

	//// ���L������
	TO_COIL_TBL::TO_COIL_REC* GetCoilTbl(int rec)	{return (TO_COIL_TBL::TO_COIL_REC*)&mtbl_pCoil->rec[rec];}	



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����
public:
	void AllDelete();													// �S�R�C����񂫂ꂢ�����ς菉���� [��MainInstance]
	void CoilClear(COIL_INF* pCoil);									// �R�C�����̏�����
	void OutCoilClear(COIL_OUT_INF* pCoil);								// �o�������R�C�����̏�����
	void Init(bool restart);											// �����J�n���̏��������� [��MainInstance]
protected:
	void DummyCoilInf(int rec, bool init, bool isWpd=true);				// �_�~�[�̓����_���R�C�����𐶐�
	void Create_CoilInf(COIL_INF* pCoil);								// �����_���R�C�����𐶐�
	void RecvNextCoil_Ken();											// ������_�̃R�C������ ���R�C�����G���A�ɃZ�b�g
	void RecvNextCoil_Dsp();											// �\����_�̃R�C������ ���R�C�����G���A�ɃZ�b�g
	void SetInCoilInf_Init(COIL_INF* pCoilIn, COIL_OUT_INF* pCoilOut, double dOutLen);	// �o���R�C���̎��̓����R�C�������Z�b�g
	void SetInCoilInf_End(COIL_OUT_INF* pCoilOut, double dInLen, double dOutLen);		// �Ō�̓����R�C�����̊m������Z�b�g



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �R�C���؂�ւ�
public:
	void ChangeCoilKenBase(bool isWpd=true, bool isNowCoil=false);				// �����ΏۃR�C���؂�ւ�(���) [��MainInstance]
	void ChangeCoilKenBase_NotPos();											// �����ΏۃR�C���؂�ւ�(������) [��MainInstance]
	bool ChangeCoilDspBase(double dLen, int nDspWpd=0);							// �\���ΏۃR�C���؂�ւ� [��MainInstance]

	void ChangeCoilKenPointer(int men);											// �����ΏۃR�C���̎Q�ƈʒu�ύX [��HtRecv]
	void ChangeCoilDspPointer(EM_POS_DSP pos, double dLenIn, double dLenOut);	// �\���p �e�\���ʒu �ΏۃR�C���̎Q�ƈʒu�ύX (���e�ʒu�̃��R�[�h�̓z�ɂ���) [��PlgManager]
	void ChangeCoilShaPointer(double dLen_In, double dLen_Out);					// �\���p �V���[�J�b�g�ʒu �ΏۃR�C���̎Q�ƈʒu�ύX
	
	void ChangeCoilOutKenPointer(int ou);										// �������u�ʒu �Ώۏo�������R�C���̎Q�ƈʒu�ύX (���������u�ʒu�̃��R�[�h�̓z�ɂ���) [��HtRecv]
	void ReloadInCoil();														// �������u�ʒu �o�������R�C���̓����R�C�������X�V [��MI]


	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ѕۑ��֘A
protected:
	void SaveDB_CoilInf();												// �R�C����� DB�ۑ�
	void SaveDB_CoilPosition(int ou, int row);							// �R�C���ʒu��� DB�ۑ�
	void SaveDB_CoilResult();											// �R�C�����ѕ� DB�ۑ�
	void SaveDB_UpdateCoilResult(EM_MODE_COILRESULT_UPDATE emMode,
		COIL_INF const* pCoilInf);
	void SaveDB_CoilCutPos(int nMode, SHRCUT_ITEM const* cut);			// �J�b�g�ʒu DB�ۑ� [��MI]
	void ResetDB_DefectImg(char const* cKizukenNo);						// �r�摜�ۑ������Z�b�g�v��


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �J�����R���g���[���I������
public:
	int	GetCamExpTime(int nMen, int nCamPos, int nAngId);				// �w��J�����̘I�����Ԃ��擾
#ifdef IS_TO_CAMERA_TBL
	int	GetCamMode(int nMen, int nCamPos, int nAngId);					// �w��J�����̏�Ԃ��擾
#endif
	float	GetCamGain(int nMen, int nCamPos, int nColor);				// �w��J�����̃J�����Q�C�����擾

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �Ɩ����u
public:
	int GetLedValue(int nMen, int nColor);								// �Ɩ����u�̒����l���擾

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����
protected:
	void SetTypeEpcCoil(TYPE_EPC_COIL& data, const PRO_TYPE_DATA_COILINF& p);	// TYPE_EPC_COIL����
	void SetTypeCoilInf(DBDATA_COIL_INF& data, const PRO_TYPE_DATA_COILINF& p);	// DBDATA_COIL_INF�쐬



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get
public:
	int				GetDistPos() const					{ return m_nDistPos; }							// ���(���㗬�̕�)�������\��(0:�\,1:��)
	int				GetRecIndex(int rec, int index);													// ���R�[�h�ʒu�擾 (0:�� 1:1�R�O 2: 3: )
	int				GetRecKizukenNo(char const* pKizukenNo);											// ���R�[�h�ʒu�擾 (�Ǘ�No)
	int				GetKenRecBase()						{ return m_nKenRecBase;}
	const COIL_INF* GetCoilNextKen() const				{ return &m_CoilNext_Ken;}						// �����p ���R�C�����
	const COIL_INF* GetCoilIndex(int rec, int index)	{ return &m_CoilInf[GetRecIndex(rec, index)]; }	// �w�肳�ꂽ�O�R�C�����̃R�C����� (0:�� 1:1�R�O 2: 3: ) (���̗���)
	const COIL_INF* GetCoilRec(int rec) 				{ return &m_CoilInf[rec]; }						// �w�肳�ꂽ���R�[�h�̃R�C�����
	const COIL_INF* GetCoilKizukenNo(char const* pKizukenNo);											// �w�肳�ꂽ�Ǘ�No�̃R�C�����

	int				GetShaRecIndex(int rec, int index);
	const COIL_INF* GetCoilAll(int pos) const;								// �S������ (���ۂ̗���)
	const COIL_INF* GetCoilBaseKen() const									// ������_�ʒu�ł̃R�C����� (���ۂ̗���)
			{ if(-1==m_nKenRecBase){return NULL;} else { return &m_CoilInf[m_nKenRecBase];} }	
	const COIL_INF* GetCoilKenV(int men)const	{ return m_pCoilKenV[men];}	// �������u�ʒu�ł̃R�C����� (�f�[�^�̗���)
	const COIL_INF* GetCoilKenR(int men)const	{ return m_pCoilKenR[men];}	// �������u�ʒu�ł̃R�C����� (���ۂ̗���)   �� ���ꂪ��Œǉ����ꂽ����AGetCoilBaseKen�̕K�v�������Ȃ蔖�ꂽ�B���������������E�E�E

	const COIL_INF* GetCoilBaseDsp() const		{ return m_pCoilBaseDsp; }	// �\����_�ʒu�ł̃R�C����� (���ۂ̗���)
	const COIL_INF* GetCoilDsp(int pos) const { return m_pCoilDsp[pos]; }	// �\���ʒu�ł̃R�C����� (���ۂ̗���)

#ifndef TRACKING_NON_DSP_COIL
	const PRO_TYPE_DATA_COILDSP* GetCoilNextDsp(int nDspWpd) const	{ return &m_CoilNext_Dsp[nDspWpd];}		// �\���p ���R�C�����
#endif

	const COIL_OUT_INF*	GetCoilOutKen(int ou) const { return m_pCoilOutKen[ou]; }		// �������u�ʒu�ł̏o�������R�C�����

	int GetKizuNo(COIL_INF* pCoil);											// �rNo���擾(�Ă΂�邽�тɃC���N���������g����邽�ߒ���)

	HANDLE GetNextCoilKen_Recv() { return m_evNextCoilKen_Recv; }		// �����p ���R�C������M�����ʒm

	const double    GetLastSpm()			{ return m_dLastSpm; }		// �O��R�C����SPM�L�ї�

																		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set
public:
	void SetBrCenter(int men, int cam, int angle, BYTE br){m_nBrCenter[men][cam][angle] = br;};	// �G�b�W�����ϖ��邳���Z�b�g [�� HtRecv]



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����p��������
public:
	void SetInitScode(int ou, int s) { if(s<1 || s>MAX_SCODE) {m_nInitScode[ou]=MAX_SCODE;} else {m_nInitScode[ou]=s;} }	// ���������\�ʏ�� �Z�b�g
	void SetInitPcode(int ou, int p) { if(p<1 || p>MAX_PCODE) {m_nInitPcode[ou]=MAX_PCODE;} else {m_nInitPcode[ou]=p;} }	// ���������p�^�[�� �Z�b�g
	int	 GetInitScode(int ou) const	{ return m_nInitScode[ou]; }		// ���������\�ʏ��
	int	 GetInitPcode(int ou) const	{ return m_nInitPcode[ou]; }		// ���������p�^�[��




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����o�[�֐�
protected:
	int  ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int  ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����o�ϐ�
protected:
	
	//=========================================
	// �e�C���X�^���X
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X	
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ��N���X
	DBManager*				mcls_pDbMgr;								// DB�o�^����N���X		
	EdgePosControl*			mcls_pEdgePosCont;							// �G�b�W���o�������Ǘ��N���X
	TrackingList*			mcls_pTlCoilOutSt;							// �o�������R�C���P�ʂ̏�ԊǗ��p�g���b�L���O
	MaeManager*				mcls_pMae;									// �O�H�����Ǘ��N���X
	DmemManager*			mcls_pDmemMgr;								// �f�[�^�������Ǘ��N���X
//#ifdef ENABLE_RPI_SAVE
//	IfRpiSave*				mcls_pIfRpiSave;
//#endif


	//=========================================
	// �󂯓n���L���[
	ThreadQueue<DBManager::DeliveryDB>*		mque_pAddSQLData;			// DB�N���X �󂯓n���f�[�^�p�L���[


	//=========================================
	// �n���h��
	HANDLE					m_evNextCoilKen;							// �����p �R�C������M�ʒm
	HANDLE					m_evNextCoilDsp;							// �\���p �R�C������M�ʒm


	//=========================================
	// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����


	//=========================================
	// ���L���������
	HANDLE					m_hCoil;									// �R�C����� �n���h��
	TO_COIL_TBL*			mtbl_pCoil;									// �R�C����� �f�[�^
#ifdef IS_TO_CAMERA_TBL
	HANDLE					m_hCam;										// �J�����ݒ��� �n���h��
	TO_CAMERA_TBL*			mtbl_pCam;									// �J�����ݒ��� �f�[�^
#endif

	//=========================================
	// �R�C�����o�b�t�@
	int						m_nKenRecBase;								// ��ʂ̌�����_�ʒu�ł̓����_���R�C���o�b�t�@�̔z��ʒu (-1:����) (���ݒl����-1����O�̃R�C�����ƂȂ�) (���ۂ̂��̒P�ʂŐ؂�ւ���)
	int						m_nDspRecBase;								// �\����_�ʒu�ł̓����_���R�C���o�b�t�@�̔z��ʒu (-1:����) (���ݒl����-1����O�̃R�C�����ƂȂ�) (���ۂ̂��̒P�ʂŐ؂�ւ���)
	double					m_dLastSpm;									// �Ō�ɃR�C���ؑւ��������p�R�C������SPM�L�ї� (�����l�́A0.00)
	COIL_INF				m_CoilInf[SIZE_COIL_BUF];					// ������/���������p�o�b�t�@ (�T�C�N���b�N) (��ʂ̌������u�ʒu�ʉ߂Ő؂�ւ��)
	COIL_INF				m_CoilNext_Ken;								// �����p ���R�C�����
#ifndef TRACKING_NON_DSP_COIL
	PRO_TYPE_DATA_COILDSP	m_CoilNext_Dsp[NUM_DSP_WPD];				// �\���p ���R�C�����
#endif


	//=========================================
	// �e�ʒu �ʉߒ��̃R�C�����|�C���^
	COIL_INF*				m_pCoilKenV[NUM_MEN];						// �e�������u�ʒu�ł̃R�C����� (NULL�͗L�肦�Ȃ�) (�f�[�^�̗���Ő؂�ւ���)
	COIL_INF*				m_pCoilKenR[NUM_MEN];						// �e�������u�ʒu�ł̃R�C����� (NULL�͗L�肦�Ȃ�) (���ۂ̂��̒P�ʂŐ؂�ւ���)
	COIL_INF*				m_pCoilBaseDsp;								// �\����_�ł̃R�C�����			(�R�C���؂�ւ����Ɏ��X�ƎQ�ƃ|�C���^�ʒu���ω����Ă���) (NULL�̉\���L��)
	COIL_INF*				m_pCoilDsp[MAX_POS_INDEX];					// �\���p�̊e�\���ł̃R�C�����   (�R�C���؂�ւ����Ɏ��X�ƎQ�ƃ|�C���^�ʒu���ω����Ă���) (NULL�̉\���L��)

	//=========================================
	// �o�����p
	COIL_OUT_INF			m_CoilOutInf;								// �o���R�C�����
	SHRCUT_ITEM				m_typCut;									// F�J�b�g�ꎞ�i�[�p

	COIL_OUT_INF*			m_pCoilOutKen[NUM_MEN];						// �e�������u�ʒu�ł̏o�������R�C����� (NULL�͗L�肦�Ȃ�) (�f�[�^�̗���Ő؂�ւ���)

	//=========================================
	// ���̑����X
	int						m_nDistPos;									// ���(���㗬�̕�)�������\��(0:�\,1:��)
	int						m_nDistNotPos;								// ��ʂƋt�̕�(0:�\,1:��)
	int						m_nInitScode[NUM_MEN];						// ���������\�ʏ�� (�\/��)(1�I���W��)
	int						m_nInitPcode[NUM_MEN];						// ���������p�^�[�� (�\/��)(1�I���W��)

	//=========================================
	// �G�b�W�����ϖ��邳(HtRecv���Z�b�g)
	BYTE					m_nBrCenter[NUM_MEN][NUM_CAM_POS][MAX_CAMANGLE];		// �G�b�W�����ϖ��邳(�g�p����̂́AR,G,B�̂�)


	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPC�� �S���ۑ�PC�p
private:
	BOOL SetUdpAllFrame(const char* ip, WORD port);						// UDP SOCKET����
	void ReSetUdpAllFrame();											// UDP SOCKET�j��

	SOCKET					m_sockAllFrame;								// �S���ۑ��p �\�P�b�g�{��
	sockaddr_in				m_addrAllFrame;								// �S���ۑ��p sockaddr_in�\����


#ifdef ENABLE_RECORDER_UDP
public:
	void SendUdpAllFrame(TYPE_EPC_COIL* pData);							// �R�C����� �� �S���ۑ�PC�ɑ��M
	TYPE_EPC_COIL	GetEpcCoilInfo(){ return m_typEpcCoil; }			// ���M�p�R�C�������擾 [��MI]
	int				GetEpcScode(int ou){ return m_nScode[ou]; }			// ���M�p�����\�ʏ�Ԃ��擾�iSPM�������j [��MI]
	int				GetEpcScode_spmoff(int ou) { return m_nScode_spmoff[ou]; }	// ���M�p�����\�ʏ�Ԃ��擾�iSPM�J�����j [��MI]

	HANDLE			m_evNextCoilKen_Recv;								// �����p ���R�C������M�����C�x���g[��MI]

private:
	TYPE_EPC_COIL	m_typEpcCoil;										// ���M�p�R�C�����
	int				m_nScode[NUM_MEN];									// ���M�p�����\�ʏ�ԁiSPM�������j
	int				m_nScode_spmoff[NUM_MEN];							// ���M�p�����\�ʏ�ԁiSPM�J�����j
#endif


};
