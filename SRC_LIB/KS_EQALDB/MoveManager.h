#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// �X���b�h�L���[�N���X

#include "KizuSyncroManager.h"											// DB�����Ǘ��N���X

using namespace KizuLib;


class MoveManager : public WorkThread,
					public KizuSyncroManager
{

private :
	// �ړ��e�[�u����`
	struct TABLE {
		bool				bEnable;									// ���̃��R�[�h���L����true
		char				cTbl[PARAM_MAX_STRING];						// �Ώۃe�[�u��
		int					nExecCnt;									// 1��̏�������	

		int					nKeyCnt;									// �w�肳�ꂽ�L���Ȏ�L�[��
		char				cKey[PARAM_MAX_KEYCNT][PARAM_MAX_STRING];	// ��L�[
	};

public:
	MoveManager(CString cSession, LogFileManager* pLog);
	virtual ~MoveManager(void);


	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��

	char const*	GetType() const	{return (char const*)&"���шړ�";};		// ����^�C�v (�Œ薼��)

private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	bool Exec(bool bAtoB);												// A�n�̎��т�B�n�Ɉړ�������

	//// �����Ώۃe�[�u��
	TABLE					m_Tbl[PARAM_MAX_TABLE];						// �����Ώۃe�[�u��

};
