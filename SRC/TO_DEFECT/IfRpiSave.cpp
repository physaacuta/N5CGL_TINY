#include "StdAfx.h"
#include "IfRpiSave.h"
// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
IfRpiSave::IfRpiSave(void)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IfRpiSave::~IfRpiSave(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �S���摜�ۑ��֌W�̃��W���[���Q

//------------------------------------------
// �S���摜���� �� �����J�n�v��
//------------------------------------------
void IfRpiSave::Send_TiRpiSave_Start()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TI_RPISAVE_01;
	que.nLineNo = getlineid();

	// ���M
#ifndef SEND_RPI_MGR
	send_mail(TI_RPISAVE, "*", &que);
#else
	send_mail(TO_RPIMGR, ".", &que);
#endif
}
//------------------------------------------
// �S���摜���� �� ������~�v��
//------------------------------------------
void IfRpiSave::Send_TiRpiSave_Stop()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TI_RPISAVE_02;
	que.nLineNo = getlineid();

	// ���M
#ifndef SEND_RPI_MGR
	send_mail(TI_RPISAVE, "*", &que);
#else
	send_mail(TO_RPIMGR, ".", &que);
#endif
}
//------------------------------------------
// �S���摜���� �� ���R�C�����
// char const*          cKizukenNo �Ǘ�No
// COIL_SETTING const*  setting    �R�C����{�ݒ�
// COIL_BASEDATA const* base       �R�C����{���
// int                  framenum   ���s�t���[��No
// int                  front_pos  �擪����ʒu[mm]
// int                  nMen       ���s��(0:�\ 1:��)
//------------------------------------------
void IfRpiSave::Send_TiRpiSave_CoilSend(char const* cKizukenNo, COIL_SETTING const* setting, COIL_BASEDATA const * base, int framenum, int front_pos, int nMen)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TI_RPISAVE_04;
	que.nLineNo = getlineid();

	que.mix.idata[0] = nMen;		// �ΏۃC���X�^���X (-1:�S�� 0�`:�ΏۃC���X�^���X)
	//que.mix.idata[0] = -1;		// �ΏۃC���X�^���X (-1:�S�� 0�`:�ΏۃC���X�^���X) �����-1�Œ�
	que.mix.idata[1] = setting->pcode[0];
	que.mix.idata[2] = setting->pcode[1];
	que.mix.idata[3] = setting->scode[0];
	que.mix.idata[4] = setting->scode[1];
	que.mix.idata[5] = framenum;
	que.mix.idata[6] = front_pos;
	memcpy(que.mix.cdata[0], cKizukenNo, SIZE_KIZUKEN_NO);
	memcpy(que.mix.cdata[1], base->cCoilNoIn, sizeof(base->cCoilNoIn));

	// ���M
#ifndef SEND_RPI_MGR
	send_mail(TI_RPISAVE, "*", &que);
#else
	send_mail(TO_RPIMGR, ".", &que);
#endif
}

