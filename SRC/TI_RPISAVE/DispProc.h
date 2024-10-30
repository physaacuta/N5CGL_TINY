// *********************************************************************************
//	���ʕ\���N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\ImageWindow.h"					// �摜�\������N���X

#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X

#include "ProcessQueThread.h"											// ProcThread���

using namespace KizuLib;

class DispProc : public ThreadManager,
				 public ProcessQueThread<FrameDataManager, FrameDataManager>
{
//// ���J�֐�
public:
	DispProc(void);
	virtual ~DispProc(void);

	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// �������s�L��


	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	
	void SetImgWin(ImageWindow* pCls) { mcls_pImgWin = pCls; }			// �摜�\���N���X

	// ��ʕ\����\��
	void SetEv_ShowWin()	{ SetEvent(m_evShowWin); }
	void SetEv_CloseWin()	{ SetEvent(m_evCloseWin);}
	void WindowIsOnOff(bool bOnOff);					// ��ʕ`��On/Off



//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ����
	virtual FrameDataManager* OnAddNewItem( FrameDataManager* pIn );	// ������
	void Exec(FrameDataManager* p);										// �������s

	// ��ʕ\��
	void ShowWindow();
	void CloseWindow();

//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�
	ImageWindow*						mcls_pImgWin;					// �摜�\���N���X

	//// ���ʓI�ȏ��
	int									m_nIns;							// �C���X�^���X�ԍ� (0�I���W��)
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)

	//// �E�B���h�E�\���p
	HANDLE								m_evShowWin;					// ��ʕ\���v��
	HANDLE								m_evCloseWin;					// ��ʏI���v��
	bool								m_bIsDisp;						// ImgWindow�\���� true
	DWORD								m_nDspInterval[MAX_INS][MAX_COLOR_NUM];		// �O��\������ [ms]
	CRITICAL_SECTION					my_cs;							// �N���e�B�J���Z�N�V����


//// ��ʂւ̌��ʕ\���p������A�߂�ǂ������̂ŃO���[�o��
public:
	//// ���Ԋ֌W
	double								g_dTime_Iport[2];				// �ŏ��̉摜�擾����Ō�̉摜�擾�܂łɊ|���������� (����,�ő�)
	double								g_dTime_PlgSync[2];				// PLG�̕R�t���Ɋ|����������
//	double								g_dTime_CoilControl[2];			// �R�C���R�t������
	double								g_dTime_ImgUnion[2];			// �摜�A������
//	double								g_dTime_ImgColor[2];			// �A����摜��������
	double								g_dTime_ImgBuf[2];				// �摜Buf����
	double								g_dTime_ImgComp[2];				// �摜���k����
	double								g_dTime_RpiWrite[MAX_INS][2];	// RPI�t�@�C�������ݎ���
};
