// *********************************************************************************
//	DB�����݊�{�N���X
//	[Ver]
//		Ver.01    2007/03/14  vs2005 �Ή�
//
//	[����]
//		�����C���X�^���X�ɂ�铯�������݂ɑΉ��������Ƃ���
// *********************************************************************************
#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X
#include "..\KizuLib\Class\ThreadQueue.h"								// �X���b�h�L���[�N���X
//#include "..\KizuLib\Class\AutoLock.h"									// �N���e�B�J���Z�N�V�����N���X
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// ���ԑ���N���X

#include "..\KizuODBC\OdbcBase.h"										// DB����N���X

using namespace KizuLib;
using namespace KizuODBC;

class DBSaveManager : public WorkThread  
{
public:
	static const int PARAM_DB_MAX_BAINARY		= 10;					// DB�o�^ �o�C�i���f�[�^�ő匏��
	static const int PARAM_DB_RETRY_TIME		= 1000;					// DB�Đڑ����� [ms]

	//// �󂯓n���\����
	typedef struct DELIVERYSQL{
		// �o�^�f�[�^
		CString strSql;													// SQL�� (0�����̏ꍇ�A�󋵖₢���킹�ɂ��A�n���ꂽ�V�O�i����@��)
		bool	addmode;												// �ŏI���ɂ���Γo�^��true
		bool	bCalcTime;												// ���Ԍv������SQL����true
	
		bool	bIsLock;												// �����ۏ�Ŏ��s����ꍇtrue
		DWORD	nLockNo;												// ���ԕۏ�p(�A�Ԃł��邱�ƁB)

		int		cnt;													// �o�C�i�C�f�[�^���� (�ő�10��) (0���A�o�C�i���o�^����=�ʏ�SQL��)
		DWORD	size[PARAM_DB_MAX_BAINARY];								// �摜�T�C�Y
		BYTE*	pImg[PARAM_DB_MAX_BAINARY];								// �摜�f�[�^

		// �o�^�������Ƀ��[���X���b�g���M�p
		MAIL_ITEM*	SendMailItem;										// ���M�A�C�e���ꎮ
		
	public:
		DELIVERYSQL() {													// �f�t�H���g�R���X�g���N�^
			strSql		= "";
			addmode		= false;
			bCalcTime	= true;

			bIsLock		= false;
			nLockNo		= 0;

			cnt			= 0;
			memset(size, 0x00, sizeof(size));
			memset(pImg, 0x00, sizeof(pImg));
			SendMailItem= NULL;	
			
		};
	} DeliverySQL;


public:
	DBSaveManager(int myID, ThreadQueue<DeliverySQL>* que_pDeli);
	virtual ~DBSaveManager();


	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	//void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetSession(char const* pSession) { m_cSession = pSession; }	// DB�ڑ��Z�b�V�����w��
	void SetReTry(bool bMode) { m_bReTry = m_bReTryExecOk = bMode; }	// DB�ُ펞�ɑ����ēx���킷�邩 (true:�Ē���)

	//// �O���A�N�Z�X�p
	double GetAveTime() const { return mcls_Counter.GetAveTime(); }		// 1��̕��Ϗ������� [ms]
	double GetMaxTime()	const { return mcls_Counter.GetMaxTime(); }		// �ő又������ [ms]
	void InitTime()			  { mcls_Counter.InitAve(); }				// ������

	void SetSendDbManager(bool flg) { m_bSendDbManager = flg; }			// DB�Ǘ��ɑ��M���邩 (true:���M)
	void SetEvSynclock(HANDLE evSynclock) { m_evSynclock = evSynclock;}	// �o�^�󋵖₢���킹�C�x���g���Z�b�g

	//// �O���V�O�i��
	void SetEvReSet()	{ SetEvent(m_evReSet);}							// DB�ڑ����Z�b�g



private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	void Send_To_DbManage();											// DB�폜�Ǘ��ɒʒm
	void sqlExecute(DeliverySQL * pDeli);								// SQL���s
	bool ConnectDB(bool bLogDsp);										// DB�ڑ�
	void CloseDB();														// DB�ؒf


	// �󂯓n���L���[
	ThreadQueue<DeliverySQL>*	mque_pDeli;								// SQL��� �L���[

	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	KizuPerformanceCounter	mcls_Counter;								// ���ԑ���X���b�h
	OdbcBase*				mcls_pDB;									// ODBC����N���X
	
	// �V�O�i��
	HANDLE					m_evReTryConnect;							// �Đڑ��v���^�C�}�C�x���g
	HANDLE					m_evSynclock;								// �o�^�󋵖₢���킹�C�x���g (�O������n�����)
	HANDLE					m_evReSet;									// DB�ڑ����Z�b�g�C�x���g
	

	// ���̑��������
	int						m_nMyID;									// ������ID (0�n�܂�)
	bool					m_bDBConnect;								// DB�ڑ���� (true:�ڑ���)
	bool					m_bDBDelete;								// DB�Ǘ���� (ture:�폜�D��)
	bool					m_bSendDbManager;							// DB�Ǘ��ɑ��M���邩 (true:���M)
	bool					m_bReTry;									// DB�ُ펞�ɑ����ēx���킷�邩 (true:�Ē���)
	bool					m_bReTryExecOk;								// ��񃊃g���C������A�Ăу��g���C����̂́A�P��Ő���ɂȂ��Ă���Ƃ���B(= SQL�ُ�Ƃ��ŁA����Đڑ�����̂��~�߂���)

	CString					m_cSession;									// DB�ڑ��Z�b�V����


	// �����ۏ�֘A (�K�� CS ��ʂ��āA�ϐ��ɃA�N�Z�X)
private:
	static CRITICAL_SECTION	m_csLock;									// ���N���X�̑��C���X�^���X�Ԃ̔r������
	static DWORD			m_nEndLockNo;								// �������������� nLockNo
	static DWORD			m_nPushLockNo;								// AP�����o���� nLockNo

	DWORD	ReadEndLockNo() const { AutoLock  lock(&m_csLock); return m_nEndLockNo; }
public:
	static void SetLockSeqens(DELIVERYSQL* p) {							// �����ۏ���Z�b�g
		AutoLock  lock(&m_csLock); 
		m_nPushLockNo ++;

		p->bIsLock = true;
		p->nLockNo = m_nPushLockNo;
	}
	static void ResetLockNo () {										// �����J�n���ȂǂɁB���Ȃ��Ă�OK�B(������~����NG�B�܂��o�^Que���c���Ă��邩������)
		AutoLock  lock(&m_csLock); 
		m_nEndLockNo = 0;
		m_nPushLockNo= 0;
	}

};
