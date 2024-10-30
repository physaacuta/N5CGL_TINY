//=====================================================================
// iPortViewer I/F�N���X
//	�y�S���C���z
//		�EiPortViewer�Ƃ�I/F���s��
//=====================================================================
#pragma once

//#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
//#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
//#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
//#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X
//
//// �r���N���X �C���N���[�h
//#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\CLASS\MailSlotManager.h"				// ���[���X���b�g�Ǘ�

// ���ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"
#include "StatusManager.h"												// �X�e�[�^�X�Ǘ��N���X
#include "ToOutMailSender.h"											// �O�����[���X���b�g���M���W���[��

using namespace KizuLib;

class IPortViewerManager : public ThreadManager  //WorkThread //
{

//// ���J�֐�
public:
	IPortViewerManager(void);
	virtual ~IPortViewerManager(void);

	void	Stop();												// �X���b�h�I��

	// �O���v���p�e�B
	void	SetStatusMgr(StatusManager* p)	{ mcls_pStatus = p; }		// �X�e�[�^�X�Ǘ��N���X[��MI]


//// �����o�[�֐�
private :
	// ���[�J�[�X���b�h�Ƃ��ē��삳���Ȃ����߁A�֌W�֐������J�ɕύX

	StatusManager*	mcls_pStatus;										// �X�e�[�^�X�Ǘ��N���X

	int				ThreadFirst()				{return 0;}				// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int				ThreadLast()				{return 0;}				// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void			ThreadEvent(int nEventNo)	{;}						// �X���b�h�C�x���g����

	int				ThreadMainLoop();

	void			SetLightVal(int camset, int index, BYTE val);		// �����pLED�Ɩ����ʐݒ� �������̂�
	void			SetLightOnOff(int cam, int index, int onoff);		// �����pLED�Ɩ�onoff�ݒ� �������̂�
	void			SetAGC(int camset, int ro);							// �I�����Ԑݒ�
	void			SetCameraChange(int hantei, int cam);				// �J�����ؑ֐ݒ� ���J���������N�X�C�b�`�̂�

};

