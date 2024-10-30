// *********************************************************************************
//	Contec PIO�{�[�h�Ǘ��N���X
//	[Ver]
//		Ver.01    2007/03/20  vs2005 �Ή�
//		Ver.02	  2009/09/02  WMD�Ńh���C�o�ɑΉ�
//		Ver.03	  2010/10/29  �V�~�����[�^�ɑΉ�
//		Ver.03	  2010/11/10  �����[�gPIO�ɑΉ�
//
//	[����]
//		PC98�Ł^WMD�ł� DEFECTLIB_CONTEC_PIO_WMD �̃f�t�@�C����؂�ւ��邱��
//		�yPC98�Łz
//			C:\Program Files\CONTEC\API-PAC(W32)\Dio\Samples\Inc		APIDIO.H
//			C:\Program Files\CONTEC\API-PAC(W32)\Dio\Samples\Lib		APIDIO.lib
//
//		�yWMD x86�z
//			C:\Program Files\CONTEC\API-PAC(W32)\DIOWDM\Samples\Inc		CDio.h
//			C:\Program Files\CONTEC\API-PAC(W32)\DIOWDM\Samples\Lib		CDIO.lib
//
//		�yWMD x64�z
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\DIOWDM\Samples\Inc		CDio.h
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\DIOWDM\Samples\Lib_amd64	CDIO.lib
//
//		�y�����[�gPIO�z
//			C:\Program Files\CONTEC\API-CAP(W32)\Samples\Inc	ccapdio.h
//			C:\Program Files\CONTEC\API-CAP(W32)\Samples\Lib	ccapdio.lib
// *********************************************************************************
#pragma once

// ���Œ�
#include "..\..\Include\LibSelect.h"									// ���C�u�����ؑփw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h


#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W
#include "..\KizuLib\Class\KizuFunction.h"								// �X�^�e�B�b�N���C�u�����W
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X
		
using namespace KizuLib;

#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// ���S������
  #include "..\..\Include\KizuSmyu.h"										// �V�~�����[�V�����w�b�_�[

class ContecPioBaseManager : public WorkThread
{
public:	
	//// �󂯓n���\���̂̔ԍ�
	typedef enum PIO_BIT {
		BIT_0 = (0x01),
		BIT_1 = (0x02),
		BIT_2 = (0x04),
		BIT_3 = (0x08),
		BIT_4 = (0x10),
		BIT_5 = (0x20),
		BIT_6 = (0x40),
		BIT_7 = (0x80)
	};

	//// �󂯓n���\���̂̔ԍ�
	typedef enum PIO_PORT {
		PORT_0 = 0,
		PORT_1 = 1,
		PORT_2 = 2,
		PORT_3 = 3,
		PORT_4 = 4,
		PORT_5 = 5,
		PORT_6 = 6,
		PORT_7 = 7
	};

protected:
//#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
//	#define CONTEC_PIO_NOID				INVALID_HANDLE_VALUE			 
//#else									// WMD��
//	#define CONTEC_PIO_NOID				-1
//#endif
	static const int mc_nReadInterval	= 25;							// PIO�ǂݍ��ݎ��� [ms]
	static const int mc_nMaxCheckCount	= 64;							// �ő�`�F�b�NDI�_��
	static const int mc_nMaxDoPulsCount	= 16;							// �ő�DO�p���X�o�͐���_��


	struct CHECK_ARRAY {											// �`�F�b�N�ݒ���
		PIO_BIT		addr;												// �`�F�b�N����A�h���X
		PIO_PORT	port;												// �`�F�b�N����|�[�g
		HANDLE		evSignOn;											// ON�̎��V�O�i���ɂ���n���h��
		HANDLE		evSignOff;											// ON�̎��V�O�i���ɂ���n���h��
	};

	struct CHECK_DOPULS_ARRAY {										// DO�p���X�o�͐ݒ���
		PIO_BIT		addr;												// �`�F�b�N����A�h���X
		PIO_PORT	port;												// �`�F�b�N����|�[�g
		bool		bOnOff;												// �Z�b�g���̏�� (true:OFF��ON��OFF  false:ON��OFF��ON)

		bool		bExec;												// ���s���̏ꍇ True
		HANDLE		evTimer;											// �Z�b�g���ԗp�^�C�}�[
	};


public:
	ContecPioBaseManager();
	virtual ~ContecPioBaseManager();

//	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
//	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessage�̑���� (���C���C���X�^���X)


	//=========================================
	// ����
	virtual bool Alloc(WORD drvno, WORD grpno, WORD port_num, WORD kid) {return true;};			// �̈�m��
	virtual bool Alloc(WORD port_num, WORD kid, char const* dev_name="DIO000") {return true;};	// �̈�m��
	virtual void Free() {};																		// �̈�J��
	

	//=========================================
	// Get
	BOOL GetStatusDI() const { return m_bStatusDi;}						// �{�[�h��� (DI)
	BOOL GetStatusDO() const { return m_bStatusDo;}						// �{�[�h��� (DO)
	bool GetDI(PIO_PORT port, PIO_BIT addr) const	{ return ( m_pDI[port] & addr ) ? true : false; }		// DI �w��ʒu�� BIT ON/OFF ��� (�A�N�Z�X����ϐ����Ⴄ�̂Œ���)
	bool GetDO(PIO_PORT port, PIO_BIT addr) const	{ return ( m_bufOut[port] & addr ) ? true : false; }	// DO �w��ʒu�� BIT ON/OFF ��� (�A�N�Z�X����ϐ����Ⴄ�̂Œ���)
	BYTE const* GetDI() const { return m_pDI;}							// DI���
	BYTE const* GetDO() const { return m_bufOut;}						// DO���
	HANDLE GetDiErrEvt() const { return m_evDiErr; }					// �G���[�F���p�C�x���g
	HANDLE GetDoErrEvt() const { return m_evDoErr; }					// �G���[�F���p�C�x���g
	void GetDIRefresh();												// �����X�V (�Ǎ��݊�������܂Ńu���b�N�����)


	//=========================================
	// Set
	void SetCheckArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn=NULL, HANDLE evSignOff=NULL);	// �`�F�b�N��� �ݒ�
	void SetDO(PIO_PORT port, PIO_BIT addr, bool onoff, bool evt = false);		// DO �w��ʒu�r�b�g ON/OFF
	void WriteDO()			 { SetEvent(m_evOutputEvt); };				// �����o�b�t�@�l��S����������


	//=========================================
	// DEBUG�p
	void SetAutoRead(bool mode) { m_bAuto = mode; }						// DI�ǂݍ��݉�(����true)�E�s��(�蓮false)
	void SetDI(PIO_PORT port, PIO_BIT addr, bool onoff);				// DO �w��ʒu�r�b�g ON/OFF

	//=========================================
	// ���̑�Static�ϊ�
	static int	GetBitToIndex(BYTE addr);								// �w��ʒu�r�b�g �� No �ɕϊ�
	static BYTE GetIndexToBit(int index);								// �w��ʒuNo �� �r�b�g �ɕϊ�
	static int PioNowStateCheck(PIO_PORT port, PIO_BIT addr, BYTE const* now, BYTE const* old ); // PIO�ω���ԃ`�F�b�N

protected:
//	virtual static UINT WINAPI MainThread(LPVOID param) {};				// ���C���X���b�h

	int PioNowState(PIO_PORT port, PIO_BIT addr, BYTE* now, BYTE* old);	// �f�W�^���l�X�V
	virtual BOOL Input()  {return TRUE;};								// �f�W�^���f�[�^�̓���
	virtual BOOL Output() {return TRUE;};								// �f�W�^���f�[�^�̏o��

	//=========================================
	// �e�C���X�^���X	
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X


	//=========================================
	// Contec�ŕK�v
	WORD					m_nPortNum;									// �|�[�g��
//#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
//	HANDLE					m_hDrv;										// �f�o�C�X�n���h��
//	DINP					m_lpDInp;									// �f�W�^�����͂̐ݒ��Ԃ��i�[����DINP�\����
//	DOUT					m_lpDOut;									// �f�W�^���o�͂̐ݒ��Ԃ��i�[����DOUT�\����			
//#else									// WMD��
//	short					m_hDrv;										// �f�o�C�XID
//#endif
	short*					m_inpPortNo;								// �f�W�^�����͂��s���_���|�[�g�ԍ����i�[����Ă���z��								
	short*					m_outPortNo;								// �f�W�^���o�͂��s���_���|�[�g�ԍ����i�[����Ă���z��
	BYTE*					m_bufInp;									// �f�W�^�����̓f�[�^���i�[����o�b�t�@
	BYTE*					m_bufOut;									// �f�W�^���o�̓f�[�^���i�[����o�b�t�@
	BYTE*					m_bufWk;									// �f�W�^�����̓f�[�^���ꎞ�i�[


	//=========================================
	// ���o�̓`�F�b�N�p
	BYTE*					m_pDI;										// �M����ԕێ�
	BYTE*					m_pDO;										// �M����ԕێ�
	BOOL					m_bStatusDi;								// DI�X�e�[�^�X(TRUE:����, FALSE:�ُ�)
	BOOL					m_bStatusDo;								// DO�X�e�[�^�X(TRUE:����, FALSE:�ُ�)
	DWORD					m_nCheckNum;								// �`�F�b�N����_��
	CHECK_ARRAY				m_CheckArray[mc_nMaxCheckCount];			// IO�`�F�b�N����|�[�g�E�r�b�g�̏��

	//=========================================
	// �V�O�i��
	HANDLE					m_evDiErr;									// �G���[�F���p�C�x���g
	HANDLE					m_evDoErr;									// �G���[�F���p�C�x���g
	HANDLE					m_evOutputEvt;								// �o�͗v���C�x���g
	HANDLE					m_evLockReadStart;							// �����Ǎ��݊J�n�C�x���g
	HANDLE					m_evLockReadEnd;							// �����Ǎ��݊����C�x���g

	// ���̑��������
	WORD					m_nKID;										// �@��ID

	// DEBUG�p
	bool					m_bAuto;									// �蓮�ݒ莞 false




	//=========================================
	//// DO�p���X�o�͗p
public:
	void SetPulsDoSetting(PIO_PORT port, PIO_BIT addr, bool bOnOff = true);	// DO�p���X�ݒ�(Start�O�Ɏ��s���邱��)
	bool SetPulsDO(PIO_PORT port, PIO_BIT addr, int nTime);					// DO �w��ʒu�r�b�g�� �w�莞�ԃp���X�o��
	bool IsPulsDO(PIO_PORT port, PIO_BIT addr);								// DO�p���X�o�͎��s���H

protected:
	int						m_nPulsDoNum;								// �ő�Z�b�g��
	CHECK_DOPULS_ARRAY		m_DoPulsArray[mc_nMaxDoPulsCount];			// �p���X�o�̓A�C�e��
	


	//=========================================
	//// �V�~�����[�^�֌W
public:
	bool	SetSmyuMode(bool bMode); 									// �V�~�����[�^���[�h�̐ݒ�
	bool	GetSmyuMode() { return m_bSmyu; }

protected:
	void	SmyuInput();												// �f�W�^���f�[�^�̓���
	void	SmyuOutput();												// �f�W�^���f�[�^�̏o��

protected:
	bool					m_bSmyu;									// �V�~�����[�^�� true; �ʏ펞 false;
	CString					m_sSmyuName;								// PIO��� ����
	HANDLE					m_hTblSmyu;									// PIO��� �n���h��
	SMYU_TBL_PIO*			mtbl_pSmyu;									// PIO��� �f�[�^

};

#endif