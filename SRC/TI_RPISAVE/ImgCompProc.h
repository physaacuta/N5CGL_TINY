// *********************************************************************************
//	�摜���k�N���X (���񏈗�)
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E�A����摜�t���[���P�ʂŏ���
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X

///#include "..\..\Library\DefectLib\KizuJpegManager.h"					// Bitmap��Jpeg�ϊ��N���X
#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"


// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X
#include "ProcessQueThread.h"											// ProcThread���

//#include "LacManager.h"												// ���x���k�N���X
#include "../../LibPG/lac/LacManager.h"									// ���x���k�N���X
#include "RpiFileManager.h"												// ������RPI�t�@�C���N���X

using namespace KizuLib;

class ImgCompProc :	public ThreadManager,
					public ProcessQueThread<FrameDataManager, FrameDataManager>
{
//// ���J�֐�
public:
	ImgCompProc(int nId);
	virtual ~ImgCompProc(void);

	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// �������s�L��

	
	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	

	// ���
	bool	IsComp() const{return m_bIsComp;}


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ����
	virtual FrameDataManager* OnAddNewItem( FrameDataManager* pIn );	// ������
	void Exec(FrameDataManager* p);										// �������s



//// �����o�[�ϐ�
private:
	//// �󂯓n���L���[

	//// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�
	
///	KizuJpegManager						mcls_Jpeg;						// JPEG���k�N���X
	LacManager							mcls_Lac;						// ���x���k�N���X

	// ���ʓI�ȏ��
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)
	bool								m_bIsComp;						// ���k��true

	BYTE*								m_imgwk;

	int									m_nCompQuality;					// ���k��
	int									m_nUionWidth;					// �t���[���摜�A����̉摜��
	int									m_nUionHeight;					// �t���[���摜�A����̉摜����

};
