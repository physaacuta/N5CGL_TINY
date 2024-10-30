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

#include "MainConst.h"


class TransUdpManager :	public ThreadManager
{

//// ���N���X���ؑփX�C�b�`
#define		ENABLE_DEBUG_MAILTR			// ��`���A�f�o�b�O�p���O�o��(�{�Ԏ��R�����g)

//// ���N���X����p�̒萔
private:


//// �����o�[�\����
private:


//// ���J�֐�
public:
	TransUdpManager(void);
	virtual ~TransUdpManager(void);

	void Send_KsMailtr_MailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len);		// ���[���X���b�g�]����M���烁�[�����M

	UdpMail*		GetRecvUdp() { return &m_RecvUdp; }					// UDP��M���擾 [��Dlg]
	void			ClearRecvUdp() { memset(&m_RecvUdp, 0x00, sizeof(m_RecvUdp)); };	// UDP��M��񏉊��� [��Dlg]

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	void OpenUdpPort();													// UDP�|�[�g �I�[�v��
	void CloseUdpPort();												// UDP�|�[�g �N���[�Y


	
//// �����o�[�ϐ�
private:

	SOCKET				m_udp;											// UDP��M�\�P�b�g
	HANDLE				m_hEventRecv;									// ��M�����n���h��

	UdpMail				m_RecvUdp;										// UDP��M���

};
