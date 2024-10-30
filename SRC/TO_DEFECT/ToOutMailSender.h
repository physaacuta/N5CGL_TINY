//=====================================================================
// �r���@�� I/F�N���X
//	�y�S���C���z
//		�E�r���̋��ʎx���A���ʃ^�X�N�Ƃ�I/F��񋟂���
//=====================================================================

#pragma once

#include "..\..\Include\LineCommon.h"						// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"					// �r�����C�u����

class ToOutMailSender
{
public:
	ToOutMailSender(void);
	virtual ~ToOutMailSender(void);


	//// PC�}�X�^�[
	static void Send_KsMaster_TimeSet();								// PC�}�X�^�[��ԊǗ��� �����ݒ�ʒm [��Dlg]
	static void Send_KsMaster_PcChange();								// �}�X�^�[PC��ԊǗ���PC�����ύX���ꂽ����ʒm


	//// �΃J�����R���g���[��
	static void Send_ToCamera_AGC(int camerano, int mode, int ro);		// �J�����R���g���[���[�� AGC�ݒ� ��ʒm [��HY_RECV]
	static void Send_ToCamera_Param(int scode);							// �J�����R���g���[���[ �� �p�����[�^�ύX��ʒm [��ParamManager]
	static void Send_ToCamera_Palse(bool onoff);						// �J�����R���g���[���[�� �p���X�o�� ��ʒm [��PioManager, IPortViewer]
	static void Send_ToCamera_Start(EM_DIV_PLG em_plg);					// �J�����R���g���[���[�� �����J�n ��ʒm [��Dlg, IPortViewer]
	static void Send_ToCamera_CoilChange(int TorB, int scode);			// �J�����R���g���[���[�� �Y���R�C���ɍœK�Ȑݒ��ʒm����
	static void Send_ToCamera_AgcVal();									// �J�����R���g���[���[�� ���I�����Ԗ₢���킹�ʒm

	// �J���������N�X�C�b�`�ʐM
	static void Send_ToLinksw_Param(int scode);										// �J���������N�X�C�b�`�ʐM �� �p�����[�^�ύX�ʒm [��ParamManager]
	static void Send_ToLinksw_Start();												// �J���������N�X�C�b�`�ʐM �� �����J�n�ʒm [��MainInstance]
	static void Send_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamID, int nKind=1);	// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ� [���H]
	static void Send_ToLinksw_JoinReq(int ou, int JoinFrameNo);						// �J���������N�X�C�b�`�ʐM �� �r���Q���ݒ� [��MainInstance]
	static void Send_ToLinksw_ChangeLinkSpeed(int nBoardID, int nSpeed);			// �J���������N�X�C�b�`�ʐM �� �V���A���ʐM���x�ύX�v�� [��CamLinkSpeedManager]
	static void Create_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamNo, int nKind, COMMON_QUE *que);	// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ�f�[�^����
	static void Create_ToLinksw_JoinReq(int ou, int JoinFrameNo, COMMON_QUE *que);	// �J���������N�X�C�b�`�ʐM �� �r���Q���ݒ�f�[�^����
	static void Send_ToLinksw_ChangeCam(COMMON_QUE *que);							// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ� [���H]
	static void Send_ToLinksw_JoinReq(COMMON_QUE *que);								// �J���������N�X�C�b�`�ʐM �� �r���Q���ݒ� [��MainInstance]


	//// �ΏƖ����u
#ifndef KT_LEDMGR
	static void Send_ToLights_Param(int scode);							// �Ɩ����u�� �p�����[�^�ύX��ʒm [��ParamManager]
	static void Send_ToLights_CoilChange(int TorB, int scode);			// �Ɩ����u�� �Y���R�C���ɍœK�Ȑݒ��ʒm
#else
	static void Send_KtLedMgr_Param(int scode);										// �Ɩ����u�� �p�����[�^�ύX��ʒm [��ParamManager]
	static void Send_KtLedMgr_CoilChange(int TorB, int scode, int ch=0);			// �Ɩ����u�� �Y���R�C���ɍœK�Ȑݒ��ʒm
	static void Send_KtLedMgr_ValChange(int TorB, int valM, int valH, int ch=0);	// LED�Ɩ����u�� �œK�ȋP�x�ݒ��ʒm���� [��IportViewer]
	static void Send_KtLedMgr_OnOff(int TorB, BOOL onoff, int ch=0);				// LED�Ɩ����u�ʐM�� �_���E�����w�� ��ʒm [��MainInstance, ?] (2CGL�̂�)
#endif
};
