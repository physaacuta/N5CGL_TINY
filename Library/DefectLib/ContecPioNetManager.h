// *********************************************************************************
//	Contec �����[�gPIO�{�[�h�Ǘ��N���X
//		�I���{�[�hPIO�ƃ����[�gPIO �� ����EXE�œ���������ꍇ�́A
//		�I���{�[�hPIO�̕��̃h���C�o�� �]���łɂ���K�v����I
//		WMD ���� �X�^�e�B�b�N�����N����������NG�ƂȂ�
// *********************************************************************************
#pragma once

#include "ContecPioBaseManager.h"										// ���N���X

#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// ���S������

// �R���e�b�N �C���N���[�h
//	�I���{�[�h�ł��� WMD�ł�������
#include "..\..\include\Other\CCAPDIO.h"								// PIO�{�[�h �C���N���[�h

// VS�̃o�O���ۂ��B
#ifdef _DEBUG
	#pragma comment(lib, "C:\\Program Files\\CONTEC\\API-CAP(W32)\\Samples\\Lib\\ccapdio.lib")
#else
	#pragma comment(lib, "\"C:\\Program Files\\CONTEC\\API-CAP(W32)\\Samples\\Lib\\ccapdio.lib\"")
#endif

using namespace KizuLib;


class ContecPioNetManager : public ContecPioBaseManager
{
private:
	//static const int CONTEC_PIO_NOID	= -1;

public:
	ContecPioNetManager();
	virtual ~ContecPioNetManager();

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��


	//=========================================
	// ����
	bool Alloc(WORD port_num, WORD kid, char const* dev_name="DIO000");	// �̈�m��
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
	short					m_hDrv;										// �f�o�C�XID
	char					m_dev_name[64];								// �{�[�h����

};
#endif