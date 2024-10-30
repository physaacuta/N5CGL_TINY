//======================================================================================
//
// �J���������N�X�C�b�`�p �V���A���ʐM���x�ύX�N���X
//
//--------------------------------------------------------------------------------------
// �摜�����G���W�������UDP���󂯂āATO_LINKSW�ɑ��x�ύX�ʒm�𑗐M����B
// UDP�́A�J�����Z�b�g�P�ʂő��M����Ă���O��
// �i�F���E����Ƃ���1�J�����Z�b�g������1��B����̏ꍇ�́A1��̒ʒm�Ő��E���̃J�����𑬓x�ύX����j
//======================================================================================


#pragma once


#include <winsock2.h>


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Include\EPC\NCL2DETECT_if.h"

#include "StatusManager.h"
#include "MainConst.h"

#include "ToOutMailSender.h"


class CamLinkSpeedManager :	public ThreadManager
{

//// ���N���X���ؑփX�C�b�`
#define		ENABLE_DEBUG_CAMLINK			// ��`���A�f�o�b�O�p���O�o��(�{�Ԏ��R�����g)

//// ���N���X����p�̒萔
private:

	static const int DEFAULT_BAUD_RATE = 9600;							// �f�t�H���g�̃V���A���ʐM���x[bps]



//// �����o�[�\����
private:

	// �V���A���ʐM���x �ύX�v���p�\����(�G���W������A���̃f�[�^�Z�b�g�ł���Ă���)
	struct TYP_SER_CHANGE_REQ{
	
		int PacketId;					// �p�P�b�gID
		int CamId;						// �J����ID(1�I���W��)
		int nSpeed;						// �V���A���ʐM���x[bps]
	};


//// ���J�֐�
public:
	CamLinkSpeedManager(void);
	virtual ~CamLinkSpeedManager(void);


	// �O���Z�b�g�v���p�e�B
	void SetStatusMgr(StatusManager* p) { mcls_pStatus = p; }			// �X�e�[�^�X�Ǘ��N���X[��MI]

	void InitCamLinkSpeed(int nBaudrate = DEFAULT_BAUD_RATE);			// �V���A�����x�������ݒ�ɖ߂�[��MI]




//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	void OpenUdpPort();													// UDP�|�[�g �I�[�v��
	void CloseUdpPort();												// UDP�|�[�g �N���[�Y

	void GetIniSetting();												// Ini�t�@�C������ڑ�CH���擾���Ĕz��ɃZ�b�g

	int  SelectBoardID(const int nCamId, int* nBoardID);				// �J����ID����A�t���[���O���o�[�{�[�hNo�����


	
//// �����o�[�ϐ�
private:

	// ���[�J�[�X���b�h
	StatusManager*		mcls_pStatus;									// �X�e�[�^�X�Ǘ��N���X

	// PCID�ɕR�t���J���������N�X�C�b�`�̏o��CH
	// [PCID][�ڑ��{�[�h] �̓񎟌��z��ɁA�o��CH���Z�b�g(FPGA��ID0����̘A�ԁ@�^�@���ڑ����A0)
	int					m_nBoardID[NUM_ALL_HANTEI][NUM_CAMLINK_BOARD];

	SOCKET				m_udp;											// UDP��M�\�P�b�g
	HANDLE				m_hEventRecv;									// ��M�����n���h��

};
