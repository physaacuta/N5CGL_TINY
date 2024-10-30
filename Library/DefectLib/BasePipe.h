// *********************************************************************************
//	���O�t���p�C�v�̑���M�p���N���X
//	[Ver]
//		Ver.01    2010/03/02  vs2005 �Ή�
//
//	[����]
//		�E����R���s���[�^���m�̃^�X�N�ԒʐM���́A����
//			�E\\\\.\\pipe\aaa		���� 25ns�`50ns �ŒʐM�\
//			�E\\\\tokatu\\			���� 180ns�`280ns �Ƃ��Ȃ�x���Ȃ� (����PC�ł��A�R���s���[�^���w��͕��ʂ��炢�̑��x�ɂȂ�)
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ���O�Ǘ��N���X

#include "..\KizuLib\Class\PipeManager.h"								// ���O�t���p�C�v�N���X

using namespace KizuLib;

class BasePipe
{
//// ���J
public:

	//// �A���T�[���̃X�e�[�^�X
	enum DELI_ANS_STATE {
		ANS_ACCEPT = 0,													// �A�N�Z�v�g�ڑ������ʒm
		ANS_RECVEND,													// �񓯊���M�����ʒm
		ANS_ERR_CLOSE,													// �ؒf�ʒm
		ANS_ERR_CONNECT													// �R�l�N�g���s�ʒm
	};

	//// ����M���
	struct PIPE_BASE_HEAD {
		int		nSize;													// ����M�f�[�^�̒���(�������܂߂�����)
		int		nKind;													// �`���敪
		int		nId;													// �����ڑ��Ή� (�N���킩��悤��ID�Z�b�g) ��{0�I���W��
	};


	//// �A���T�[���
	struct DELIVERY_ANS {
		DELI_ANS_STATE		state;										// �A���T�[���̃X�e�[�^�X
		PIPE_BASE_HEAD*		pData;										// ��� (��M���̂݁B����ȊO��NULL)
	};



//// ���J
public:
	BasePipe(void);
	virtual ~BasePipe(void);

	// �p�����[�^
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g
	void SetIsServer(bool bMode)		{ my_bIsServer = bMode; }		// �T�[�o�[or�N���C�A���g �ݒ�
	void SetPipeName(LPCSTR sName)		{ my_sPipeName = sName; }		// ���O�t���p�C�v����


	//// �p�C�v����
	bool	Connect();													// �R�l�N�g (�ڑ������܂Ńu���b�N�����)
	bool	Listen();													// ���X�j���O (�m���u���b�N)
	void	Accepting();												// �A�N�Z�v�g����
	void	Close();													// �N���[�Y

	bool	StartRecvHeader();											// �w�b�_�[����M�J�n
	DWORD	EndRecvHeader();											// �w�b�_�[����M������
	DWORD	GetRecvBody(BYTE* buf, DWORD len );							// �f�[�^����M (����)

	bool	SendLock(BYTE* buf, DWORD len, int nMaxWait);				// �f�[�^���M (����)
	bool	SendUnLock(BYTE* buf, DWORD len);							// �f�[�^���M (�񓯊�)

	//// �V�O�i�����J
	HANDLE	GetEvSendEnd() { return my_ovlSend.hEvent;}					// ���M������
	HANDLE	GetEvRecvEnd() { return my_ovlRecv.hEvent;}					// ��M������
	HANDLE	GetEvAcceptEnd() { return my_ovlAccept.hEvent;}				// �A�N�Z�v�g������


	//// Get
	bool	IsConnect() const			{ return my_nConnect==0?false:true;}
	int		GetConnectCnt() const		{ return my_nConnect;}
	CString GetPipeName() const			{ return my_sPipeName;}
	PIPE_BASE_HEAD&	GetRecvHeadBuf()	{ return my_cRecvHeadBuf;}		// �w�b�_�[���f�[�^�ɃA�N�Z�X
	int GetError()						{ return mcls_pPipe->GetError(); }	// �ŏI�G���[�擾


protected :

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	PipeManager*			mcls_pPipe;									// Pipe�N���X


	// �񓯊�IO
	OVERLAPPED				my_ovlSend;									// ���M����
	OVERLAPPED				my_ovlRecv;									// ��M����
	OVERLAPPED				my_ovlAccept;								// �A�N�Z�v�g����
	CRITICAL_SECTION		my_cs;										// ���b�N


	// ���
	int						my_nConnect;								// �ڑ��l��
	bool					my_bRecvHead;								// �w�b�_�[������M���� true
	PIPE_BASE_HEAD			my_cRecvHeadBuf;							// �w�b�_�[��M��


	// ���̑��������
	bool					my_bIsServer;								// Server��true
	CString					my_sPipeName;								// ���O�t���p�C�v����
	CString					my_sThreadName;								// �X���b�h����
};
