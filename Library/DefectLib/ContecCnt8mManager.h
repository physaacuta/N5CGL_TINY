// *********************************************************************************
//	Contec CNT�{�[�h�Ǘ��N���X
//	[Ver]
//		Ver.01    2007/03/20  vs2005 �Ή�
//		Ver.02	  2009/09/02  WMD�Ńh���C�o�ɑΉ�
//
//	[����]
//		PC98�Ł^WMD�ł� DEFECTLIB_CONTEC_PIO_WMD �̃f�t�@�C����؂�ւ��邱��
//		�yPC98�Łz
//			C:\Program Files\CONTEC\API-PAC(W32)\CNT\Samples\inc		APICNT8M.H
//			C:\Program Files\CONTEC\API-PAC(W32)\CNT\Samples\lib		APICNT.lib
//		�yWMD x64�z
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\CNTWDM\Samples\Inc		CCnt.h
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\CNTWDM\Samples\Lib_amd64	CCNT.lib
// *********************************************************************************
#pragma once

#include <winsock2.h>													// UDP���M�p

// ���Œ�
#include "..\..\Include\LibSelect.h"									// ���C�u�����ؑփw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W


#ifndef  DEFECTLIB_CONTEC_CNT_NON		////// ���S������
  #include "..\..\Include\KizuSmyu.h"										// �V�~�����[�V�����w�b�_�[


// �R���e�b�N �C���N���[�h
#ifdef DEFECTLIB_CONTEC_CNT_PATH_H
	#include DEFECTLIB_CONTEC_CNT_PATH_H
#else
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		#include "..\..\include\Other\APICNT8M.H"							// �J�E���^�[�{�[�h �C���N���[�h
	#else									// WMD��
		#include "..\..\include\Other\CCnt.h"								// CNT�{�[�h �C���N���[�h
	#endif
#endif

// ���C�u����
#ifndef LIB_MANUAL
#ifdef DEFECTLIB_CONTEC_CNT_PATH_L
	#pragma comment(lib, DEFECTLIB_CONTEC_CNT_PATH_L)
#else
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		// VS�̃o�O���ۂ��B
		#ifdef _DEBUG
			#pragma comment(lib, "C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib")
		#else
			#pragma comment(lib, "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib\"")
		#endif
	#else									// WMD��
		// VS�̃o�O���ۂ��B
		#ifdef _M_X64			// x64 �R���p�C��
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib\"")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib\"")
			#endif
		#endif
	#endif
#endif
#endif

// �e�X�g�p���X�o��
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	extern "C" {		
		DWORD __stdcall CntTestPulse(HANDLE, WORD, WORD);
	}
#endif

class ContecCnt8mManager
{
public:
	//// API�̃A�N�Z�X���̃G���[
	enum ENUM_CNT_ERRCODE {
			CntOpenExErr = 1,
			CntSelectChannelSignalErr,
			CntSetOperationModeErr,
			CCntSetZModeErr,
			CntSetZLogicErr,
			CntSetCountDirectionErr,
			CntSetDigitalFilterErr,
			CntSetSamplingStartTrgErr,
			CntSetSamplingClockTrgErr,
			CntSetSamplingClockErr,
			CntSetSamplingStopTrgErr,
			CntSetSamplingChannelErr,
			CntResetChannelErr,
			CntStartCountErr,
			CntResetSamplingErr,
			CntSetSamplingBuffErr,
			CntStartSamplingErr,
			CntStopSamplingErr,
			CntStopCountErr,
			CntGetSamplingCountErr,
			ContecCnt8mManagerErr,
			CntTestPulseErr,
			CntGetSamplingStatusErr,

			CntSetOutputHardwareEventErr,
			CntSetCountMatchHardwareEventErr,
			CntNotifyCountUpErr,
			CntNotifySamplingCountErr,
			CntCountUpCallbackProcErr,
			CntSamplingCountCallbackProcErr
	};
	
	//// �J�E���^�[�{�[�h�S�ʂ̐ݒ�p�����[�^
	struct Param_CntBase {
		bool		m_bTest;						// �e�X�g�p���X���o�͂��邩�ǂ���
	};
	//// �J�E���^�[�{�[�h�`�����l�����̐ݒ�p�����[�^
	struct Param_CntCH {
//#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
//		WORD		m_nSigType;						// �M���� [CNT_SIGTYPE_LINERECEIVER, CNT_SIGTYPE_TTL](CntSelectChannelSignal)
//		WORD		m_nPhase;						// ���� [CNT_MODE_2PHASE, CNT_MODE_1PHASE]
//		WORD		m_nMul;							// ���{ [CNT_MUL_X4, CNT_MUL_X2 ,CNT_MUL_X1]
//		WORD		m_nSyncClr;						// �����N���A�^�񓯊��N���A [CNT_CLR_ASYNC]
//		WORD		m_nZmode;						// Z�����[�h [CNT_ZPHASE_NOT_USE]
//		WORD		m_nZlogic;						// Z���_�� [CNT_ZLOGIC_NEGATIVE]
//		WORD		m_nDir;							// �J�E���g���� [CNT_DIR_UP, CNT_DIR_DOWN]
//		WORD		m_nFilterValue;					// �t�B���^�̎��萔
//#else									// WMD��
		short		m_nSigType;						// �M���� [CNT_SIGTYPE_LINERECEIVER, CNT_SIGTYPE_TTL](CntSelectChannelSignal)
		short		m_nPhase;						// ���� [CNT_MODE_2PHASE, CNT_MODE_1PHASE]
		short		m_nMul;							// ���{ [CNT_MUL_X4, CNT_MUL_X2 ,CNT_MUL_X1]
		short		m_nSyncClr;						// �����N���A�^�񓯊��N���A [CNT_CLR_ASYNC]
		short		m_nZmode;						// Z�����[�h [CNT_ZPHASE_NOT_USE]
		short		m_nZlogic;						// Z���_�� [CNT_ZLOGIC_NEGATIVE]
		short		m_nDir;							// �J�E���g���� [CNT_DIR_UP, CNT_DIR_DOWN]
		short		m_nFilterValue;					// �t�B���^�̎��萔
//#endif
	};
	//// �J�E���^�[�{�[�h�T���v�����O�����ݒ�p�����[�^
	struct Param_CntSamp {
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		DWORD		m_nStart;						// �T���v�����O�X�^�[�g���� [CNT8M_START_SOFT] (CntSetSamplingStartTrg)
		WORD		m_nCountStart;					// �T���v�����O�J�n���ɃJ�E���^���X�^�[�g���邩 [CNT8M_START_CNT_NOT_WITH_SAMP] (CntSetSamplingStartTrg)
		DWORD		m_nClock;						// �T���v�����O�N���b�N���� [CNT8M_CLOCK_EXT_TRG, CNT8M_CLOCK_CLOCK]
		DWORD		m_nStop;						// �T���v�����O�X�g�b�v���� [CNT8M_STOP_SOFT]
		WORD		m_nCountStop;					// �T���v�����O��~���ɃJ�E���^���X�g�b�v���邩�ǂ��� [CNT8M_STOP_CNT_NOT_WITH_SAMP]
#else									// WMD��
		short		m_nStart;						// �T���v�����O�X�^�[�g���� [CNT8M_START_SOFT] (CntSetSamplingStartTrg)
		short		m_nCountStart;					// �T���v�����O�J�n���ɃJ�E���^���X�^�[�g���邩 [CNT8M_START_CNT_NOT_WITH_SAMP] (CntSetSamplingStartTrg)
		short		m_nClock;						// �T���v�����O�N���b�N���� [CNT8M_CLOCK_EXT_TRG, CNT8M_CLOCK_CLOCK]
		short		m_nStop;						// �T���v�����O�X�g�b�v���� [CNT8M_STOP_SOFT]
		short		m_nCountStop;					// �T���v�����O��~���ɃJ�E���^���X�g�b�v���邩�ǂ��� [CNT8M_STOP_CNT_NOT_WITH_SAMP]
#endif	
	};

private :
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	#define CONTEC_CNT_NOID				INVALID_HANDLE_VALUE			 
#else									// WMD��
	#define CONTEC_CNT_NOID				-1
#endif
	static const int mc_nMaxCannel	= 8;			// �{�N���X�ŊǗ�����ő�`�����l����
	static const int mc_nMaxUdp		= 4;			// UDP�Ǘ��ő吔

/*
	// �T���v�����O�o�b�t�@�\����
	struct SampData {	
		long	plg;								// ch0 �̒l (= ���C��PLG�̃p���X��)
		DWORD	clk;								// ch1 �̒l (= �e�X�g�p���X10��sec�����̐�)
	};
*/
public:
	
	ContecCnt8mManager(WORD nChannelNum, WORD nSampChannelNum, DWORD nSampBuffSize, DWORD nSampBuffNum);
	virtual ~ContecCnt8mManager();


	//// �p�����[�^�A�N�Z�X (�I�[�v���O�ɐݒ���������Ă�����)
	Param_CntBase&	SetParamCntBase()		{return m_parmCntBase;  }	// �J�E���^�[�{�[�h�S�ʂ̐ݒ�p�����[�^
	Param_CntCH&	SetParamCntCH(WORD ch)	{return m_parmCntCH[ch];}	// �J�E���^�[�{�[�h�`�����l�����̐ݒ�p�����[�^
	Param_CntSamp&	SetParamCntSamp()		{return m_parmCntSamp;  }	// �J�E���^�[�{�[�h�T���v�����O�����ݒ�p�����[�^

	// Set (�K�{�ݒ�)
	void SetEvSampStart()			{ SetEvent(my_evSamplingStart);	}	// �V�O�i���Z�b�g �T���v�����O�X�^�[�g
	void SetEvSampStop()			{ SetEvent(my_evSamplingStop);	}	// �V�O�i���Z�b�g �T���v�����O�X�g�b�v
	void SetEvTimerCancel()			{ CancelWaitableTimer(my_evTimer);}	// �^�C�}�V�O�i���L�����Z��
	void SetDistPostion(int torb)	{ my_nDistPos = torb; my_nDistNotPos = 0==torb ? 1 : 0;}	// ���(���㗬�̕�)�������\��(0:�\,1:��)
	void SetDistLen(DWORD len)		{ my_nDistLen = len;	}			// �\���ԋ��� [mm]
	void SetEvTimerStart(int interval) {								// �^�C�}�V�O�i���Z�b�g
		__int64 ts = 0;
		SetWaitableTimer(my_evTimer, (LARGE_INTEGER *)&ts, interval, NULL, NULL, FALSE);
	}

	//// �C�ӂɐݒ�\
	DWORD TestPulse(short int_ext=0);									// �e�X�g�p���X�̓����^�O���ؑ֐���
	// ��v�o��
	DWORD OutputHardwareEvent(WORD ChNum, short EventType, short PulseWidth);	// ����o�͐M���̃n�[�h�E�F�A�C�x���g�ݒ�
	DWORD CountMatchHardwareEvent(WORD ChNo, short RegisterNo, short eventType);// �J�E���g��v�̃n�[�h�E�F�A�C�x���g�ݒ�
	DWORD ResetChannel(int nChIndex, int ChNum);						// �J�E���g�l���[���N���A���܂��B
	DWORD NotifyCountUp(WORD ChNo, short RegNo, long plsCnt);			// �J�E���g��v�ɂ��ʒm�̎w��ݒ�
	DWORD NotifySamplingCount(long Count);								// �w����T���v�����O�����̒ʒm�ݒ�
	DWORD CountUpCallbackProc(void* CallBackProc, void* param);			// CntNotifyCountUp�֐��Őݒ肵���J�E���g��v�ɂ��C�x���g�����������ꍇ�ɌĂяo�����R�[���o�b�N�֐���o�^
	DWORD SamplingCountCallbackProc(void* CallBackProc, void* param);


	// Get
	HANDLE	GetEvCntError() const			{ return my_evCntError;  }	// �G���[�����V�O�i��
	DWORD	GetDistLen()	const			{ return my_nDistLen; }		// �\���ԋ��� [mm]
	int		GetDistPos()	const			{ return my_nDistPos; }		// ���(���㗬�̕�)�������\��(0:�\,1:��)
	int		GetDistNotPos()	const			{ return my_nDistNotPos; }	// ��ʂƋt�̕�(0:�\,1:��)
	bool	GetStatus()     const			{ return m_bStatus;		 }	// �J�E���^�[�{�[�h��� �擾

	// ����
	int		GetCntMul(int nWk) { 
		int mul = CNT_MUL_X1;
		if( 1 == nWk )		mul = CNT_MUL_X1;
		else if( 2 == nWk )	mul = CNT_MUL_X2;
		else if( 4 == nWk ) mul = CNT_MUL_X4;
		return mul;
	}

protected:
	DWORD CountOpen(char* dev_name = "CNT00");							// �J�E���^�[�{�[�h�ݒ�
	DWORD SampOpen();													// �T���v�����O�ݒ�
	DWORD CountStart();													// �J�E���^�[�X�^�[�g
	DWORD SampStart();													// �T���v�����O�X�^�[�g
	DWORD CountStop();													// �J�E���^�[�X�g�b�v
	DWORD CountClose();													// �J�E���^�[�{�[�h�J��
	DWORD SampStop();													// �T���v�����O�X�g�b�v
	BOOL  SetUdp(int no, const char* ip, WORD port);					// UDP�ݒ�
	virtual void ContecError(DWORD dwRet) = 0;							// ��L�֐��G���[���̏���

	// Get
	long  GetErrorCode()				 const { return m_errCode;	}				// �ُ�R�[�h
	DWORD GetSampStat(DWORD* status, DWORD* err);									// �T���v�����O�X�e�[�^�X���擾
	DWORD GetSamplingCount(DWORD* sampCnt, DWORD* sampCarry);						// ���T���v�����O�J�E���^�[�l�擾
	DWORD GetSamplingCount(DWORD* sampCnt);											// ���T���v�����O�J�E���^�[�l�擾
	DWORD GetCount(WORD ChNum, short* ChNo, DWORD* count);							// �����̃J�E���g�l�擾
	DWORD GetCount(DWORD* count, WORD channel);										// �P��̃J�E���g�l�擾
//	DWORD GetSampBuffClk(DWORD buff_pos) const { return m_pSampBuf[buff_pos].clk;}	// �T���v�����O�N���b�N�l
//	long  GetSampBuffPlg(DWORD buff_pos) const { return m_pSampBuf[buff_pos].plg;}	// �T���v�����OPLG�l
//	DWORD GetSampBuff(DWORD buff_pos, int ch) const { return m_pSampBuf[buff_pos*m_nSampChannelNum+ch]; } // �T���v�����O�N���b�N�l
	DWORD GetSampBuff(DWORD buff_pos, int ch) const									// �T���v�����O�N���b�N�l
			{ 
				if(!m_bSmyu) return m_pSampBuf[buff_pos*m_nSampChannelNum+ch];
				else		 return mtbl_pSmyu->nSampBuf[buff_pos][ch];
			} 
	DWORD GetSampBuffClk(DWORD buff_pos) const { return GetSampBuff(buff_pos, 1);}			// �T���v�����O�N���b�N�l (ch1�Œ�)
	long  GetSampBuffPlg(DWORD buff_pos) const { return (long)GetSampBuff(buff_pos, 0);}	// �T���v�����OPLG�l (ch0�Œ�)

	// Set
	void  SetStatus(bool mode)				   { m_bStatus = mode; }	// �J�E���^�[�{�[�h��� �Z�b�g

	// �\�P�b�g
	SOCKET GetSock(int no)				 const { return m_sock[no];	}				// UDP�\�P�b�g�C���X�^���X
	sockaddr_in* GetAddr(int no) 			   { return &m_addr[no]; }				// UDP�\�P�b�gsockaddr_in�\����


	// �����I�u�W�F�N�g
	HANDLE			my_evSamplingStart;									// �T���v�����O�X�^�[�g
	HANDLE			my_evSamplingStop;									// �T���v�����O�X�g�b�v
	HANDLE			my_evCntError;										// �G���[
	HANDLE			my_evTimer;											// ������^�C�}�[�p

	//// ��������
	CRITICAL_SECTION	my_csLock;										// �N���e�B�J���Z�N�V���� (�g���Ȃ炲���R��)

	// ���̑��������
	int				my_nDistPos;										// ���(���㗬�̕�)�������\��(0:�\,1:��)
	int				my_nDistNotPos;										// ��ʂƋt�̕�(0:�\,1:��)
	DWORD			my_nDistLen;										// �\���ԋ��� [mm]
	bool			my_bSendflg;										// ���M�L��(TRUE:���M�L, FALSE:���M��)


private:
	// �J�E���^�[�{�[�h�֘A
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	HANDLE			m_hDrv;												// �f�o�C�X�n���h��
	WORD			m_nChannel[mc_nMaxCannel];							// �`�����l���ԍ�
#else									// WMD��
	short			m_hDrv;												// �f�o�C�XID
	short			m_nChannel[mc_nMaxCannel];							// �`�����l���ԍ�
#endif
	long			m_errCode;											// �G���[�R�[�h
	Param_CntBase	m_parmCntBase;										// �J�E���^�[�{�[�h�S�ʂ̐ݒ�p�����[�^
	WORD			m_nChannelNum;										// �g�p�`�����l����
	Param_CntCH		m_parmCntCH[mc_nMaxCannel];							// �J�E���^�[�{�[�h�`�����l�����̐ݒ�p�����[�^
	Param_CntSamp	m_parmCntSamp;										// �J�E���^�[�{�[�h�T���v�����O�����ݒ�p�����[�^

	// �T���v�����O�֘A
	//SampData*		m_pSampBuf;											// �T���v�����O�o�b�t�@
	DWORD*			m_pSampBuf;											// �T���v�����O�o�b�t�@
	WORD			m_nSampChannelNum;									// �T���v�����O���s���`�����l���� (Ch0�`�w�肵����(=ch����) �̘A������Ch�𓯎��ɃT���v�����O���� )
	DWORD			m_nSampBufSize;										// �T���v�����O�o�b�t�@�T�C�Y
	DWORD			m_nSampBufNum;										// �T���v�����O��

	// UDP�֘A
	//int				m_sock[mc_nMaxUdp];									// �\�P�b�g�{��
	SOCKET			m_sock[mc_nMaxUdp];									// �\�P�b�g�{��		
	sockaddr_in		m_addr[mc_nMaxUdp];									// sockaddr_in�\����

	// ���̑��������
	bool			m_bStatus;											// �J�E���^�[�{�[�h���



	//=========================================
	//// �V�~�����[�^�֌W
public:
	bool	SetSmyuMode(bool bMode); 									// �V�~�����[�^���[�h�̐ݒ�
	bool	GetSmyuMode() { return m_bSmyu; }

private:
	void	SmyuCntResetSampling();										// �T���v�����O�o�b�t�@�̏�����
	DWORD	SmyuGetSamplingCount(DWORD* sampCnt);						// �ŐV�̃T���v�����O�J�E���g�l �擾
	DWORD	SmyuGetReadCount(WORD ChNum, short* ChNo, DWORD* count);	// �ŐV�̃J�E���g�l �擾

private:
	bool					m_bSmyu;									// �V�~�����[�^�� true;
	CString					m_sSmyuName;								// PLG��� ����
	HANDLE					m_hTblSmyu;									// PLG��� �n���h��
	SMYU_TBL_PLG*			mtbl_pSmyu;									// PLG��� �f�[�^
};

#endif