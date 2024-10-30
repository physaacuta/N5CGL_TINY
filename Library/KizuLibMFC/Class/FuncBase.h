// *********************************************************************************
//	�t�@���N�V�����p�̃x�[�X�N���X
//	[����]
//		ThreadManager�͌p�����Ȃ����ǁALogManager�Ƃ������͎g��������
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\..\Include\KizuCommon.h"								// �r���C���N���[�h
#include "..\..\KizuLib\KizuLib.h"										// �r�����C�u����
#include "..\..\KizuLib\Class\KizuMacro.h"								// �}�N���W
#include "..\..\KizuLibMFC\Class\LogFileManager.h"						// ���O�Ǘ��N���X

using namespace KizuLib;
class FuncBase
{
public:
	FuncBase(const char* pThreadName="");
	~FuncBase(void);

	void SetLogMgr(LogFileManager* pCls) {mcls_pLog = pCls;}	// ���O�N���X�Z�b�g
	char const*		GetThreadName()		 {return my_sThreadName.GetBuffer();}	// ���X���b�h��


protected:
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	CString					my_sThreadName;						// ���X���b�h��
};
