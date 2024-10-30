// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************


// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/03  vs2005 �Ή�
//
//	[����]
//	�� MX�R���|�[�l���g���C���X�g�[������Ă���[���ŏC�����s���Ă��������B
//     �C���X�g�[������Ă��Ȃ��ꍇ�́ACD����C���X�g�[�����s���Ă��������B
//
//	�� �V�[�P���T�[�̃��[�e�B���e�B���C���X�g�[�����s���Ă��������B
//     �� �ʐM�ݒ���s���A�ڑ��m�F���s���Ă��������B
//     �� �_���ǔԂ��R���|�[�l���g�̐ݒ�g�p���܂��B
//     �� �}�j���A����MELSEC\Manual����sh080274i.pdf���Q�Ƃ��Ă�������
//
//	�� �V�[�P���T�[��CPU�^�C�v�A�ʐM�菇(TCP,UDP,COM��)�ɂ���ĉ�ʂɒ��t����R���g���[����
//     �ݒ肷��CPUNO���ς��̂ŁA�ʐM�Ɏg�p����V�[�P���T��CPU�^�C�v���m�F���ĉ������B
//     �� N5CGL�ݒ�l �c �R���|�[�l���g�FACTQJ71E71UDP
//     �� CD�ɕt������Ă���}�j���A�����Q�Ƃ��邱��
//        MELSEC\Manual����sh080275f.pdf���Q��
//        �R��ACT�R���g���[���̏ڍ� P.51  �c �g�p����R���g���[��
//        �R��ACT�R���g���[���̏ڍ� P.52  �c CPUNO
//
//	�� MX�R���|�[�l���g�̐ݒ�	
//	   �E�V�[�P���T�[�Ƃ̐ݒ�́A�R���g���[���Ɏ����Ă���̂ŕύX���s���ꍇ�͒��ӂ��邱��	
//		�iCPU�^�C�v�A�ʐM�^�C���A�E�g�l�AIP�A�h���X�A�|�[�gNO)
//     �� N5CGL�ݒ�l �c �R���|�[�l���g��					�F ACTQJ71E71UDP
//                       �_���ǔ�(ActLogicalStationNumber	�F 1
//                       CPUNO�iActCpuType)					�F 35
//                       �^�C���A�E�g(ActTimeOut)	[m�b]	�F			(�v���O�������ōĐݒ�)
//                       IP�A�h���X(ActHostAddress)�@		�F			(�v���O�������ōĐݒ�)
//						 �|�[�gNO(ActPortNumber)			�F 5001		(�v���O�������ōĐݒ�)
//                       ���̍��ڂ̓f�t�H���g�ݒ�
//
//	  �E�V�K�ō쐬�����ꍇ�A�R���g���[�����c�[���{�b�N�X�ɕ\������Ȃ���	
//	    �蓮�ŃR���g���[����Ǎ��K�v������B	
//	    ���̂Ƃ������̃R���g���[�����\�������̂ŁA�K�v�ȃR���g���[��������ǂݍ��ޕK�v������܂��B	
//
//	�� �R���g���[������ʂɒ��t����ꍇ�́A�R���|�[�l���g��OPEN��OnInitDialog�ł���K�v������܂��B
//
//  �� �R���|�[�l���g��OPEN������OnInitDialog�ōs���Ă���ׁA
//     �ēxOPEN���邱�Ƃ��o���Ȃ��̂ŁA�^�X�N���ċN������K�v������܂��B
//
//  �� MX�R���|�[�l���g�̊֐�����̃G���[�R�[�h��HEX�Ŋm�F���Ă��������B
//     �U�� �G���[�R�[�h P.232
//
//  �� VS2015 ����@�����J�[/�ڍאݒ�/�f�[�^���s�h�~(DEP) �� �f�t�H���gON �ɕύX����Ă���
//     ����ɔ����AMX�R���|�[�l���g��DLL���ŗ����邽�߁A NO�ɕύX���K�v
//
// *********************************************************************************

#pragma once


// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ���[�J�[�X���b�h
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
//#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X


// �e���[�J�[�X���b�h
#include "DMemManager.h"


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
	DMemManager 			gcls_DMem;									// �f�[�^�������Ǘ��N���X


private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	void testsub();

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	
	// �����V�O�i��
	HANDLE					m_evThStart;								// MainInctance�X���b�h�̎��s�v��

	// ���̑��������


};
