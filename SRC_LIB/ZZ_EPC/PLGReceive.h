// PLGReceive.h: PLGReceive �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLGRECEIVE_H__1D3BDA8B_1C93_4A46_90A6_0641DA079BFF__INCLUDED_)
#define AFX_PLGRECEIVE_H__1D3BDA8B_1C93_4A46_90A6_0641DA079BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �C���N���[�h
#include <winsock2.h>

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X


// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCspeed.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`
#include "..\..\Include\ToHtCommon.h"

#include "DataSender.h"


using namespace KizuLib;

class PLGReceive : public WorkThread  
{
public:
	PLGReceive();
	virtual ~PLGReceive();

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g

	void SetDataSender(DataSender* pCls) { mcls_pData = pCls; };			// ���O�t�@�C���Z�b�g


private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	// ���̑��������
	int		m_nSock;								// UDP��M�\�P�b�g
	
	// �V�O�i��
	HANDLE	m_evReset;								// ��M�҂���ԂɃ��Z�b�g����
	
	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	DataSender*				mcls_pData;									// �f�[�^���M

};

#endif // !defined(AFX_PLGRECEIVE_H__1D3BDA8B_1C93_4A46_90A6_0641DA079BFF__INCLUDED_)
