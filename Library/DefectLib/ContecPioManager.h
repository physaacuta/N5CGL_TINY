// *********************************************************************************
//	Contec PIO�{�[�h�Ǘ��N���X
// *********************************************************************************
#pragma once

#include "ContecPioBaseManager.h"										// ���N���X

#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// ���S������
// �R���e�b�N �C���N���[�h
#ifdef DEFECTLIB_CONTEC_PIO_PATH_H
	#include DEFECTLIB_CONTEC_PIO_PATH_H
#else
	#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
		#include "..\..\include\Other\APIDIO.H"								// PIO�{�[�h �C���N���[�h
	#else									// WMD��
		#include "..\..\include\Other\CDio.h"								// PIO�{�[�h �C���N���[�h
	#endif
#endif

// �R���e�b�N ���C�u����
#ifndef LIB_MANUAL
#ifdef DEFECTLIB_CONTEC_PIO_PATH_L
	#pragma comment(lib, DEFECTLIB_CONTEC_PIO_PATH_L)
#else
	#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
		// VS�̃o�O���ۂ��B
		#ifdef _DEBUG
			#pragma comment(lib, "C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib")
		#else
			#pragma comment(lib, "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib\"")
		#endif

	#else									// WMD��
		// VS�̃o�O���ۂ��B
		#ifdef _M_X64			// x64 �R���p�C��
			#ifdef _DEBUG			 
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib\"")
			#endif
		#else
			#ifdef _DEBUG			 
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib\"")
			#endif
		#endif
	#endif
#endif
#endif

using namespace KizuLib;


class ContecPioManager : public ContecPioBaseManager
{
private:
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	#define CONTEC_PIO_NOID				INVALID_HANDLE_VALUE			 
#else									// WMD��
	#define CONTEC_PIO_NOID				-1
#endif

	struct CHECK_CALLBACK_ARRAY {									// ���荞�݃`�F�b�N�ݒ���
		PIO_BIT		addr;												// �`�F�b�N����A�h���X
		PIO_PORT	port;												// �`�F�b�N����|�[�g
		HANDLE		evSignOn;											// ���荞�ݔF�����V�O�i���ɂ���n���h��
		bool		bMode;												// ���荞�� (true:0��1��  false:1��0��)
	};

public:
	ContecPioManager();
	virtual ~ContecPioManager();

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��


	//=========================================
	// ����
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	bool Alloc(WORD drvno, WORD grpno, WORD port_num, WORD kid);		// �̈�m��
#else									// WMD��
	bool Alloc(WORD port_num, WORD kid, char const* dev_name="DIO000");	// �̈�m��
#endif
	void Free();														// �̈�J��
	

private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	BOOL Input();														// �f�W�^���f�[�^�̓���
	BOOL Output();														// �f�W�^���f�[�^�̏o��


	//=========================================
	// �ڑ�����
	bool	Open(bool bDsp);											// �I�[�v��
	bool	Close();													// �N���[�Y
	bool	m_bIsOpen;													// �A�N�Z�X�\�� true


	//=========================================
	// Contec�ŕK�v

#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	HANDLE					m_hDrv;										// �f�o�C�X�n���h��
	DINP					m_lpDInp;									// �f�W�^�����͂̐ݒ��Ԃ��i�[����DINP�\����
	DOUT					m_lpDOut;									// �f�W�^���o�͂̐ݒ��Ԃ��i�[����DOUT�\����
	WORD					m_drvno;									// PIO�{�[�h �h���C�oNo
	WORD					m_grpno;									// PIO�{�[�h �O���[�vNo

#else									// WMD��
	short					m_hDrv;										// �f�o�C�XID
	char					m_dev_name[64];								// �{�[�h����

#endif


#ifdef DEFECTLIB_CONTEC_PIO_WMD			// WMD��
	//=========================================
	//// ���荞�ݗp
public:
	void SetCheckCallBackArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn, bool bMode=true);	// ���荞�݃`�F�b�N��� �ݒ�
	BOOL InitCallBackInp();												// ���荞�ݓ��͏��� (Alloc�ASetCheckCallBackArray��������)

private:
	static void _stdcall CallBackProc(short hDrv, WPARAM wParam, LPARAM lParam, void *Param);	// ���荞�݃R�[���o�b�N


	DWORD					m_nCheckCallBackNum;						// ���荞�݃`�F�b�N����_��
	CHECK_CALLBACK_ARRAY 	m_CheckCallBackArray[mc_nMaxCheckCount];	// IO���荞�݃`�F�b�N����|�[�g�E�r�b�g�̏��
#endif
};

#endif