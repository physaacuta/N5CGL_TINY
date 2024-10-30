#include "StdAfx.h"
#include "DivNameManager.h"

using namespace KizuLib;

//------------------------------------------
// �g��BOOL
//------------------------------------------
CString DivNameManager::GetDivBool(EM_DIV_BOOL em) 
{
	CString wk;
	if		(DIV_BOOL_TRUE == em)				{ wk = "��"; }
	else if (DIV_BOOL_FALSE == em)				{ wk = "�~";}
	else if (DIV_BOOL_NON == em)				{ wk = "�|";}
	else if (DIV_BOOL_NULL == em)				{ wk = "��";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}

//------------------------------------------
// PLG�敪 ����
//------------------------------------------
CString DivNameManager::GetDivPlg(EM_DIV_PLG em) 
{
	CString wk;
	if		(DIV_PLG_LINE == em)				{ wk = "���C��PLG"; }
	else if (DIV_PLG_TOP == em)					{ wk = "�\�Z��"; }
	else if (DIV_PLG_BOT == em)					{ wk = "���Z��";}
	else if (DIV_PLG_TEST == em)				{ wk = "�e�X�g�p���X";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}
//------------------------------------------
// �^�]��� ����
//------------------------------------------
CString DivNameManager::GetDivUnten(EM_DIV_UNTEN em) 
{
	CString wk;
	if		(DIV_UNTEN_STOP == em)				{ wk = "��~���[�h"; }
	else if (DIV_UNTEN_NOMAL == em)				{ wk = "�ʏ�^�]���[�h"; }
	else if (DIV_UNTEN_SAMP == em)				{ wk = "�e�X�g�^�] (�T���v���B�e)";}
	else if (DIV_UNTEN_CAMERA == em)			{ wk = "�Z�����[�h (�J��������)";}
	else if (DIV_UNTEN_SMYU == em)				{ wk = "�V�~�����[�V�������[�h";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}
//------------------------------------------
// ���Ə�� ����
//------------------------------------------
CString DivNameManager::GetDivSogyo(EM_DIV_SOGYO em) 
{
	CString wk;
	if		(DIV_SOGYO_NON == em)				{ wk = "�s��"; }
	else if (DIV_SOGYO_STOP == em)				{ wk = "�x�~��";}
	else if (DIV_SOGYO_SLOW == em)				{ wk = "�ᑬ����";}
	else if (DIV_SOGYO_RUN == em)				{ wk = "���ƒ�"; }
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}
//------------------------------------------
// �ғ���� ����
//------------------------------------------
CString DivNameManager::GetDivKadou(EM_DIV_KADOU em) 
{
	CString wk;
	if		(DIV_KADOU_INIT == em)				{ wk = "��������"; }
	else if (DIV_KADOU_STOP == em)				{ wk = "��~��"; }
	else if (DIV_KADOU_START == em)				{ wk = "�ғ���";}
	else if (DIV_KADOU_RETRY == em)				{ wk = "���g���C��";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}
//------------------------------------------
// ������� ����
//------------------------------------------
CString DivNameManager::GetDivKensa(EM_DIV_KENSA em) 
{
	CString wk;

	if		(DIV_KENSA_OK   == em)				{ wk = "����"; }
	else if (DIV_KENSA_NG   == em)				{ wk = "�k��";}
	else if (DIV_KENSA_MENTE== em)				{ wk = "�����e��"; }

// �g���� �Ȃ̂ŁA���V�X�e���Œ萔����`���L�蓾��̂ŁA�萔�͎g��Ȃ�---->>
	else if	(4 == em)							{ wk = "���ޕs��"; }
	else if	(5 == em)							{ wk = "���ۑ�"; }
	else if	(6 == em)							{ wk = "������"; }

	else if	(-1 == em)							{ wk = "��~ (����)"; }
//<<<----

	else if	(DIV_KENSA_STOP == em)				{ wk = "��~"; }
	else if	(DIV_KENSA_NON  == em)				{ wk = "��~ (�ΏۊO)"; }
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}
//------------------------------------------
// �@���� ����
//------------------------------------------
CString DivNameManager::GetDivKiki(EM_DIV_KIKI em) 
{
	CString wk;
	if		(DIV_KIKI_OK == em)					{ wk = "����"; }
	else if (DIV_KIKI_KEI == em)				{ wk = "�y�̏�"; }
	//else if (DIV_KIKI_TYU == em)				{ wk = "���̏�";}
	else if (DIV_KIKI_JYU == em)				{ wk = "�d�̏�";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}

//------------------------------------------
// �J�����Z�b�g����
// int id �J�����Z�b�g (1�`8) 0:�o�b�N�A�b�v
//------------------------------------------
CString DivNameManager::GetDivCamSet(EM_DIV_CAMSET id) 
{
	CString wk;
	if		(DIV_CAMSET_BACK == id)				{ wk = "�o�b�N�A�b�v"; }

#ifdef NUM_CAM_POS_1
	else if (DIV_CAMSET_T_C  == id)				{ wk = "�\�� C";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "���� C";}
#endif
#ifdef NUM_CAM_POS_2
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_3
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_C  == id)				{ wk = "�\�� C";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "���� C";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_4
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_5
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1";}
	else if (DIV_CAMSET_T_C  == id)				{ wk = "�\�� C";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "���� C";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_6
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1";}
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2";}
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1";}
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2";}
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_7
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1";}
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2";}
	else if (DIV_CAMSET_T_C  == id)				{ wk = "�\�� C";}
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1";}
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "���� C";}
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_8
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1";}
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2";}
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3";}
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3";}
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1";}
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2";}
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3";}
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3";}
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS";}
#endif
#ifdef NUM_CAM_POS_9
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "�\�� C"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "���� C"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_10
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_11
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "�\�� C"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "���� C"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_12
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "�\�� D5"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "�\�� W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "���� D5"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "���� W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_13
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "�\�� D5"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "�\�� C"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "�\�� W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "���� D5"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "���� C"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "���� W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_14
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "�\�� D5"; }
	else if (DIV_CAMSET_T_D6 == id)				{ wk = "�\�� D6"; }
	else if (DIV_CAMSET_T_W6 == id)				{ wk = "�\�� W6"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "�\�� W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "���� D5"; }
	else if (DIV_CAMSET_B_D6 == id)				{ wk = "���� D6"; }
	else if (DIV_CAMSET_B_W6 == id)				{ wk = "���� W6"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "���� W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_15
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "�\�� D5"; }
	else if (DIV_CAMSET_T_D6 == id)				{ wk = "�\�� D6"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "�\�� C"; }
	else if (DIV_CAMSET_T_W6 == id)				{ wk = "�\�� W6"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "�\�� W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "���� D5"; }
	else if (DIV_CAMSET_B_D6 == id)				{ wk = "���� D6"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "���� C"; }
	else if (DIV_CAMSET_B_W6 == id)				{ wk = "���� W6"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "���� W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
#ifdef NUM_CAM_POS_16
	else if (DIV_CAMSET_T_DS == id)				{ wk = "�\�� DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "�\�� D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "�\�� D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "�\�� D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "�\�� D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "�\�� D5"; }
	else if (DIV_CAMSET_T_D6 == id)				{ wk = "�\�� D6"; }
	else if (DIV_CAMSET_T_D7 == id)				{ wk = "�\�� D7"; }
	else if (DIV_CAMSET_T_W7 == id)				{ wk = "�\�� W7"; }
	else if (DIV_CAMSET_T_W6 == id)				{ wk = "�\�� W6"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "�\�� W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "�\�� W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "�\�� W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "�\�� W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "�\�� W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "�\�� WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "���� DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "���� D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "���� D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "���� D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "���� D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "���� D5"; }
	else if (DIV_CAMSET_B_D6 == id)				{ wk = "���� D6"; }
	else if (DIV_CAMSET_B_D7 == id)				{ wk = "���� D7"; }
	else if (DIV_CAMSET_B_W7 == id)				{ wk = "���� W7"; }
	else if (DIV_CAMSET_B_W6 == id)				{ wk = "���� W6"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "���� W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "���� W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "���� W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "���� W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "���� W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "���� WS"; }
#endif
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", id);}
	return wk;
}

