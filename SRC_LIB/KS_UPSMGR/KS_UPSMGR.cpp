// KS_UPSMGR.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <windows.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �G���g���[�|�C���g
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q

int _tmain(int argc, _TCHAR* argv[])
{
	// �^�X�N����������
//	task_init(KS_UPSMGR, 0);
	setlineid(LINE_ID);
	task_init(KS_UPSMGR);

	// UPS�ُ�
	syslog(SYSNO_UPS_NG, "[UPS�ُ팟�m]");
	
	// �V�X�e���V���b�g�_�E���ʒm
	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_06;
	que.nLineNo = getlineid();											// ���C��ID
	send_mail(KS_MASTER, "*", &que);									// �ǂ����ɂ���PC�}�X�^�[�Ǘ��֒ʒm

	task_exit();
	return 0;
}

