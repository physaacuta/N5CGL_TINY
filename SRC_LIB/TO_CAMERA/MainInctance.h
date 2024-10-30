// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		MAX_CAMPAIR �� NUM_CAMERA �̐���ς��邾���ŁA
//			�ŏ� �Жʂ̂�1��̌v1�� �` �ő� ���ʂ�4��̌v16�܂őΉ��\
// *********************************************************************************

#pragma once


// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\include\LineCommon.h"									// ���C�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ���[�J�[�X���b�h
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// �r�����C�u���� �C���N���[�h

// �e���[�J�[�X���b�h
#include "CameraManager.h"

using namespace KizuLib;


class MainInctance : public WorkThread
{
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// �X���b�h�J�n
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// �X���b�h�I��
	BOOL Init();														// ����������
	void Exit();														// �I������
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	// �e���[�J�[�X���b�h
	CameraManager*			gcls_pCam[MAX_THREAD];						// �J�����R���g���[���ʐM�N���X



private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	
	// �����V�O�i��
	HANDLE					m_evThStart;								// MainInctance�X���b�h�̎��s�v��

	// ���̑��������
	int						m_nUnitNum;									// �Ǘ��Ώۋ@�퐔







// ======================================================================
// ����
public:
	// ���[�v�Z�o
	void GetChLoop(int nUnit, int& ii_s, int& ii_e)						// ���j�b�g�w�莞�̑ΏێZ�o
	{
		if(0==nUnit) { ii_s=0; ii_e=m_nUnitNum; }
		else	     { ii_s=nUnit-1; ii_e=nUnit;}
	}

	//// �J����ID����X���b�h�C���f�b�N�X��
	//// nCamId 0�I���W��
	//// nThreadIdx, nIdx 0�I���W��
	//bool GetCamIdToThreadID(int nCamId, int& nThreadIdx, int& nIdx)
	//{
	//	for(int ii=0; ii<MAX_THREAD; ii++) {
	//		if( NULL == gcls_pCam[ii] ) continue;
	//		int nStart	= gcls_pCam[ii]->GetMyCamStartIdx() ;
	//		int nMax	= gcls_pCam[ii]->GetMyCamNum() + nStart;  
	//		if( nCamId >= nStart &&  nMax > nCamId ) {
	//			nThreadIdx	= ii;
	//			nIdx		= nCamId - nStart;
	//			return true;
	//		}
	//	}
	//	return false;		// �����܂ŗ�����NG
	//}




};
