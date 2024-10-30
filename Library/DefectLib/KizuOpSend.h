// *********************************************************************************
//	�������\���E����PC�ւ̑��M�N���X (�T�[�o�[�@�\�̂�)
//	[Ver]
//		Ver.01    2007/03/20  vs2005 �Ή�
//
//	[����]
//		��ɁA����������PC�E�\��PC�ւ̑��M�Ɏg�p
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

#include "..\KizuSOCK3\SockBase.h"										// �\�P�b�g����N���X

using namespace KizuLib;
using namespace KizuSock3;


class KizuOpSend : public WorkThread  
{
protected :
	// ���[�J���萔
	static const int PARAM_SOCK_MAX				= 6;					// �ő�ڑ��l��
	static const int PARAM_SOCK_TIMEOUT			= 60000; //20161031 //10000; //5000;					// �\�P�b�g �^�C���A�E�g
	static const int PARAM_SOCK_SEND_BUFSIZE	= (4*1024*1024);		// LAN�{�[�h�̑��M�o�b�t�@�T�C�Y (4M)
	static const int PARAM_TIME_HELCY_SEND		= (2*60*1000);			// �w���V�[�`�����M�^�C�}�[���� [ms] (2��)

public:
	KizuOpSend(const char *cSession);
	virtual ~KizuOpSend();

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessage�̑���� (���C���C���X�^���X)
	
	// �󂯓n���L���[
	ThreadQueue<SOCK_BASE_HEAD> gque_Deli;								// ���M�f�[�^�󂯓n���L���[

	//// �O���A�N�Z�X
	BOOL IsConnect(int id) const {return my_bIsConnect[id]; }			// �ڑ����
	int	 GetConnectCount() const { if(NULL!=mcls_pSock) {return mcls_pSock->GetActive();} else {return 0;} }	// �ڑ������擾
	
	HANDLE	GetEvConnect() const {return my_evConnect; };				// �O���ւ̐ڑ��C�x���g
	HANDLE	GetEvClose()   const {return my_evClose; };					// �O���ւ̐ؒf�C�x���g

protected :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	void SendStart();													// �r���`�����M �J�n����
	void SendEnd();														// �r���`�����M ��������
	void SendHelcy();													// �w���V�[�`�����M ����
	void QueBufFree();													// ���o�b�t�@���J������
	void QueAllFree();													// ���ׂẴL���[�J��

	// �\�P�b�g����
	BOOL Listen(bool bLogDsp);											// Listen
	void Close(int id);													// �ؒf����
	BOOL Send();														// �r���`�����M

	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X

	// ���M�p
	BOOL					my_bIsConnect[PARAM_SOCK_MAX];				// �ڑ���ԕێ�(TRUE:�ڑ���, FALSE:�ؒf��)
	BOOL					my_bIsNowSend;								// ���ݑ��M������(TRUE:���M��, FALSE:���M���łȂ�)
	SOCK_BASE_HEAD*			my_pHead;									// �X���b�h�L���[�󂯎�� && ���M�f�[�^�o�b�t�@
	int						my_nNowSendCount;							// ���M��
	BOOL					my_bIsAllSend;								// �S�Ă�Client�֑��M����?(TRUE:��, FALSE:��)
	long					my_nSendingDataSize;						// ���݂̃f�[�^���݂̂̑��M�o�C�g�� (�������M�Ή��΍�)

	// �C�x���g�n���h��
	HANDLE					my_evConnect;								// �ڑ��C�x���g
	HANDLE					my_evClose;									// �ؒf�C�x���g

	// ���̑��������
	CString					my_cSession;								// ini�t�@�C���̃Z�b�V������
};