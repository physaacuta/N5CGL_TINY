'*********************************************************************************
' VB�p�S�Ӄւŋ��ʂ̃C���N���[�h
'	[Ver]
'					2016/06/15	���ō쐬
'
'	[����]
'		��{�I�ɁA����������
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

''' <summary>
''' ���C������`
''' </summary>
''' <remarks></remarks>
Public Module LineCommon
    '///////////////////////////////////////////////////////////////////////////
    '�X�C�b�`
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '�����
    '-------------------------------------------------------
    'False:�{�Ԋ��ATrue:�e�X�g��
#Const cLOCAL = False

    '-------------------------------------------------------
    '���W�J�p���C���敪
    '-------------------------------------------------------
    'LINE�X�C�b�`(�K��1�R�̂݃R���g�A�E�g���鎖)VB�̏ꍇ���W���[�����ɕK�v
#Const cLINE = "LINE_5CGL_TINY"

    '///////////////////////////////////////////////////////////////////////////
    '�萔
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '���C�����
    '-------------------------------------------------------
#If cLOCAL = False Then                                     '�{�Ԋ�
    Public Const COIL_LEN_MAX As Integer = 10000                                '�����R�C����MAX[m](�ǂ̍��ڂł����̒��������ƂȂ�)
#Else                                                       '�e�X�g��
	Public Const COIL_LEN_MAX As Integer = 10000								'�����R�C����MAX[m](�ǂ̍��ڂł����̒��������ƂȂ�)
#End If
    Public Const COIL_LEN_MIN As Integer = 200									'�����R�C����MIN[m](�悭�������R�C��)
	Public Const COIL_WID_MAX As Integer = 1750									'�R�C����MAX[mm](�ǂ̍��ڂł����̕������ƂȂ�)
	'Public Const COIL_THICK_MAX As Integer = 1600								'�ޗ���MAX[��m]

	'-------------------------------------------------------
	'���u
	'-------------------------------------------------------
	Public Const NUM_MEN As Integer = 2											'���u �ʒP��(���̒P�ʂŁA������~���\)���\/������Ȃ����ɒ���
	Public Const NUM_MAX_HANTEI As Integer = 7									'�Ж� ����PC�̐�(�I�t���C���@���܂߂��䐔)
	Public Const NUM_MAX_CYCLE As Integer = 1									'�Ж� ����PC�̐� (�I�t���C���@���܂߂��䐔) �� 0���ƃR���p�C�����ʂ�Ȃ���������̂ŁA0�ł�1�ɂ��Ă���
#If cLOCAL = False Then										'�{�Ԋ�
	Public Const NUM_HANTEI As Integer = 5										'NUM_CAM_POS // �Ж� ����PC�̐�(�ЖʂɌq���锻��PC�̐�)(�I�����C���@�̂�)
	Public Const NUM_HANTEI_CYCLE As Integer = 1								'�Ж� ����PC�̐�(�ЖʂɌq���锻��PC�̐�)(�I�����C���@�̂�)
	Public Const NUM_HANTEI_PARA As Integer = 2									'���W�b�N�p������PC�̐�
	Public Const NUM_CAMPAIR As Integer = 10									'MAX_CAMSET // �J�����y�A��(�J�����y�A�Ƃ́A�����̃J�����Z�b�g���P����PC��œ��삷��ꍇ�̃J�����Z�b�g�̉�̌�)(����́A1��1�ׁ̈A8��)
	Public Const NUM_CAMPAIR_CYCLE As Integer = 1								'�J�����y�A��(����PC�P��œ��삷��J�����Z�b�g)(����́A1��4)
#Else														'�e�X�g��
	Public Const NUM_HANTEI As Integer = 5										'�Ж� ����PC�̐�(�ЖʂɌq���锻��PC�̐�)(�I�����C���@�̂�)
	Public Const NUM_HANTEI_CYCLE As Integer = 1								'�Ж� ����PC�̐�(�ЖʂɌq���锻��PC�̐�)(�I�����C���@�̂�)
	Public Const NUM_HANTEI_PARA As Integer = 2									'���W�b�N�p������PC�̐�
	Public Const NUM_CAMPAIR As Integer = 10									'�J�����y�A��(�J�����y�A�Ƃ́A�����̃J�����Z�b�g���P����PC��œ��삷��ꍇ�̃J�����Z�b�g�̉�̌�)(����́A1��1�ׁ̈A8��)
	Public Const NUM_CAMPAIR_CYCLE As Integer = 1								'�J�����y�A��(����PC�P��œ��삷��J�����Z�b�g)(����́A1��4)
#End If
	Public Const NUM_ALL_HTBACK As Integer = 4									'�S�ʂ̔���o�b�N�A�b�vPC�̑䐔
	Public Const NUM_ALL_HTPARA As Integer = 2									'�S�ʂ̔��胍�W�b�N�p������PC�̑䐔
	Public Const NUM_ALL_HANTEI As Integer = (NUM_HANTEI*NUM_MEN) + NUM_ALL_HTBACK + NUM_ALL_HTPARA         '�S�ʂ̔���PC�̑䐔
    Public Const NUM_CAMERA As Integer = (MAX_CAMSET * MAX_PHYSICAL_CAMANGLE)   '�S�ʂ̃J�����䐔(�J�����Z�b�g �~ �J�����p�x)
    Public Const MAX_IMAGE_CNT As Integer = 4									'�摜����(�����܂ł�MAX_CAMANGLE��MAX_IMAGE_CNT�͓��`���������A�����ł́A�J�����P��ŕ����摜������΍�)

	Public Const NUM_LIGHT As Integer = 2										'�Ж� �Ɩ����u�̐�
	Public Const NUM_LED_COLOR As Integer = 3                                   '�Ɩ����u�̐F���iRGB�j
    'Public Const NUM_LED_BLOCK As Integer = 36                                  '��ʐݒ�\�Ȕz���u���b�N��(ini�t�@�C���͎��ۂ̃u���b�N��)(MAX256)
    Public Const NUM_LED_BLOCK As Integer = 48                                  '��ʐݒ�\�Ȕz���u���b�N��(ini�t�@�C���͎��ۂ̃u���b�N��)(MAX256)

    Public Const NUM_CAMLINKSW As Integer = 4									'�J���������N�X�C�b�`�䐔
	Public Const NUM_CAMLINK_BOARD As Integer = 2                               'PC1�䓖����̍ő���͐� (BOARD�ɂȂ��Ă��邪�A���ۂ͎�荞�݌����B)

    '-------------------------------------------------------
    '�V�X�e�����
    '-------------------------------------------------------
    '// 20200520 �r�ۑ�������g�� --->>>
    Public Const VRCL_DEFECT_NO_START As Integer = 64001                        'VRCL�V�K�r�̓��͔͈͂��rNo(�J�n)
    Public Const VRCL_DEFECT_NO_END As Integer = 65000                          'VRCL�V�K�r�̓��͔͈͂��rNo(�I��)
    Public Const VRCL_DEFECT_NUM As Integer = (VRCL_DEFECT_NO_END - VRCL_DEFECT_NO_START + 1)   'VRCL�V�K�r�̓����r��
    'Public Const MAX_DETECT_SAVE As Integer = 32000								'1�R�C��������̕ێ������r��(�\����)
    Public Const MAX_DETECT_SAVE As Integer = 524000 - CInt(VRCL_DEFECT_NUM / NUM_MEN) '1�R�C��������̕ێ������r��(�\����)
    '// 20200520 �r�ۑ�������g�� ---<<<
    Public Const MAX_DETECT_LOWGR_SAVE As Integer = 25000                       '1�R�C��������̕ێ����閳�QGr�r��(�\����)
    'Public Const MAX_DETECT_IMAGE_SAVE As Integer = 32000                       '1�R�C��������̕ێ������r�摜(�\����)
    'Public Const MAX_DEFECT_LOWER_IMG_SAVE As Integer = 7000					'1�R�C��������̕ێ����閳�Q�r�摜�i�\���ʁj
    Public Const MAX_MAP_DEFECT_NUM As Integer = 200000                         '1�R�C��������̕\���r��(�̂́A�񐔁~�s���~�ʂł悩�������A�[��������������߁A�ő匏�������߂�悤�ɂ���)


    Public Const MAP_REPRES_MIN As Integer = 5									'�ŏ���\�r�Ԋu[m]
	Public Const MAP_COL_INTERVAL As Integer = 200								'�}�b�v��1�񂠂���̕�[mm] ��CTL���g�p
	Public Const MAP_COL_NUM As Integer = 8										'�}�b�v�ő��
	Public Const MAP_ROW_NUM As Integer = CInt(COIL_LEN_MAX / MAP_REPRES_MIN)	'�}�b�v�ő�s��(�R�C���ő咷[m]���}�b�v�ŏ��Ԋu[m])
	Public Const MAP_STAGE_NUM As Integer = 3									'�}�b�v�̐؂�ւ��i��(0:�ŏ��Ԋu 1:���ۂ̕\���G���A�� 2:���ۂ̕\���G���A��)
	Public Const MAP_NEST_NUM As Integer = 5									'�}�b�v�̃Z�����̃��X�g����
	Public Const MAP_CYCLE_COL_NUM As Integer = 1								'�����r�}�b�v��
	Public Const ALARM_COL_NUM As Integer = 8									'�r�ڋ߃����v�ő��(�ߋ��̌݊����p �����폜�\��)

	Public Const SIZE_FRAME_IMG_X As Integer = 4096								'���t���[���摜�T�C�Y[pixel]
	Public Const SIZE_FRAME_IMG_Y As Integer = 4096								'�c�t���[���摜�T�C�Y[pixel]

	'-------------------------------------------------------
	'��������
	'-------------------------------------------------------
	'Public Const MAP_CYCLE_COLNUM As Integer = 1								'�����r�}�b�v��
    'Public Const STARTINDEX_CYCLE_PCID As Integer = (NUM_ALL_HANTEI + 1)		'����PC��PCID�̐擪�ʒu
	'Public Const GET_CYCLE_PCID(pcid) As Integer = (pcid - STARTINDEX_CYCLE_PCID + 1)	'����PC��PCID����A1�n�܂�̘A�Ԃ��Z�o����B
	'Public Const GET_CYCLE_PCKIND(pckind) As Integer = (pckind - PCNO_CYCLE)			'����PC��PCKIND����A1�n�܂�̘A�Ԃ��Z�o����B


	'-------------------------------------------------------
	'�J����������DS/WS�Ή�
	'-------------------------------------------------------
	Public Const CAMLEFT_IS_DS_T As Integer = 0									'0:�J����������DS  1:�J�����E����WS(�\��)
	Public Const CAMLEFT_IS_DS_B As Integer = 0									'0:�J����������DS  1:�J�����E����WS(����)

	'-------------------------------------------------------
	'�S���ۑ�
	'-------------------------------------------------------
	Public Const NUM_JOIN_FRAME As Integer = 1500								'���t���[������r���Q��������t���[����(���t���[���{��)

	'-------------------------------------------------------
	'���̑�(�e�V�X�e���ŗL)
	'-------------------------------------------------------
    Public Const SIZE_COIL_KEY As Integer = 20                                  '�R�C�����肷�邽�߂̃L�[���(����́A����No�{�R�C��No)
    Public Const SIZE_COIL_NO As Integer = PRO_SIZE_COIL_NO                     '�R�C��No�T�C�Y
    Public Const SIZE_MEI_NO As Integer = 4                                     '����No�T�C�Y(�R���p�C�����ʂ�悤�ɍŏ��P�Ƃ���)
    'Public Const MAX_CAMERA_GAP_WIDDIV As Integer = 10                          '����Y���␳ ������������(=�ő�O���[�v��)

    Public Const MAX_SHACUT_COUNT As Integer = 10                               '�ő�J�b�g�Ǘ���
    Public Const MAX_PROGRP_TYPE As Integer = 10                                '�v���R���֑��M�����r��O���[�v�i0�F����  1�`�F�e�Ή�����O���[�v�j
    Public Const MAX_PROGRP_GRADE As Integer = 6                                '�v���R���֑��M����O���[�h�O���[�v�i0�F����  1�`�F�e�Ή�����O���[�v�j

    Public Const NUM_DSP_WPD As Integer = 1										'�\����_��
	Public Const BASE_DSP_MEN As Integer = 1									'�\����_�̊�ʁi���ʁj(�\���@�\�ւ̒ʔ������M�Ŏg�p)

	Public Const MAX_SHALINK_COUNT As Integer = 3								'�ő�o�������Ǘ���

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
	'// �����r�֌W
	Public Const MAX_CYCLE As Integer = 16                                      ' �����r�̌��o�ݒ萔
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

	'-------------------------------------------------------
	'�O�H���֌W
	'-------------------------------------------------------
	Public Const MAX_MAE_LINE As Integer = 16                                   '�O�H���o�^��
	Public Const MAX_MAE_TYPE As Integer = 64                                   '�O�H���r��ő�o�^��
	Public Const MAX_MAE_GRADE As Integer = 10                                  '�O�H���O���[�h�ő�o�^��
	Public Const MAX_MAE_IMAGE As Integer = 4                                   '�O�H���摜�ő吔
	Public Const MAE_MAP_COL_NUM As Integer = 1                                 '�O�H��MAP�ő��

	'-------------------------------------------------------
	'�d���׈ꗗ���
	'-------------------------------------------------------
	Public Const MAX_DEFLIST_BUF As Integer = 30                                '�d���׈ꗗ��ʗp �o�b�t�@����

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	'-------------------------------------------------------
	'����������
	'-------------------------------------------------------
	Public Const MAX_LONGPD_FILTER As Integer = 16                              '�������̍ő�t�B���^��
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'-------------------------------------------------------
	'�\���\�P�b�g
	'-------------------------------------------------------
	Public Const HY_SOCK_NUM As Integer = 2                                     '�\���\�P�b�g��

	'-------------------------------------------------------
	'�\���ʒu
	'-------------------------------------------------------
	Public Const MAX_DISP_POS As Integer = EM_DIV_DISP.DIV_DISP_POS_END         '�\����ʒu �� �����L�������T�C�Y�ɌW�邽�߁A�傫�����ɍ��킹�Ă���

	Public Const MAX_ALARME_POS As Integer = 2                                  '�x��o�͈ʒu �� (0:�\�ʌ����� 1:���ʌ�����)

	'-------------------------------------------------------
	'�r�}�b�v�\�����(�U�z�})�p���r�V���{����`
	'0�F�E,1:��,2:��,3:��,4:��,5:��,6:��,7:��,8:�~,9:�{
	'-------------------------------------------------------
	'�r�}�b�v�\�����(�U�z�})�p���r�V���{����`
	Public PLOT_SYMBOL_TYPE() As String = {
		"�E", "��", "��", "��", "��", "��", "��", "��", "�~", "�{", "��", "��", "��", "��", "��", "��", "��", "��", "��"
	}

	Public CUT_RESULT_ID_NO() As String = {
		"�@", "�A", "�B", "�C", "�D", "�E", "�F", "�G", "�H", "�I", "�J", "�K", "�L", "�M", "�N", "�O"
	}

	'//=======================================
	'// �f�[�^�x�[�X
	'//=======================================
	'�R�C��
	Public Const DB_COIL_INF As String = "T_COIL_INF"                           '[DB]�R�C�����
	Public Const DB_COIL_RESULT As String = "T_COIL_RESULT"                     '[DB]�R�C������
	'Public Const DB_COIL_OUTINF_R As String = "T_COIL_OUTINF_R"                 '[DB]�o�������R�C������
	Public Const DB_COIL_POSITION As String = "T_COIL_POSITION"                 '[DB]�R�C���ʒu���
	Public Const DB_COIL_POSITION_OUT As String = "T_COIL_POSITION_OUT"         '[DB]�o���R�C���ʒu���
	Public Const DB_COIL_POSITION_ANA As String = "T_COIL_POSITION_ANA"         '[DB]�����R�C���ʒu���
	Public Const DB_COIL_TRAND As String = "T_COIL_TRAND"                       '[DB]�R�C���g�����h���
	Public Const DB_COIL_CUT_POS As String = "T_COIL_CUT_POS"                   '[DB]�J�b�g�ʒu�e�[�u��
	Public Const DB_COIL_SAISUN_RESULT As String = "T_COIL_SAISUN_RESULT"       '[DB]�̐����уe�[�u��
	Public Const DB_COIL_ORDER As String = "T_COIL_ORDER"                       '[DB]�R�C���ʔ��ʏ��

	'�\�r
	Public Const DB_DEFECT_INF_T As String = "T_DEFECT_INF_T"                   '[DB]�\�r���
	Public Const DB_DEFECT_IMG_T As String = "T_DEFECT_IMG_T"                   '[DB]�\�r�摜
	Public Const DB_DEFECT_ATTR0_T As String = "T_DEFECT_ATTR0_T"               '[DB]�\��{������
	Public Const DB_DEFECT_ATTR1_T As String = "T_DEFECT_ATTR1_T"               '[DB]�\�J�����p�x1������
	Public Const DB_DEFECT_ATTR2_T As String = "T_DEFECT_ATTR2_T"               '[DB]�\�J�����p�x2������
	Public Const DB_DEFECT_ATTR3_T As String = "T_DEFECT_ATTR3_T"               '[DB]�\�J�����p�x3������
	Public Const DB_DEFECT_ATTR4_T As String = "T_DEFECT_ATTR4_T"               '[DB]�\�J�����p�x3������

	'���r
	Public Const DB_DEFECT_INF_B As String = "T_DEFECT_INF_B"                   '[DB]���r���
	Public Const DB_DEFECT_IMG_B As String = "T_DEFECT_IMG_B"                   '[DB]���r�摜
	Public Const DB_DEFECT_ATTR0_B As String = "T_DEFECT_ATTR0_B"               '[DB]����{������
	Public Const DB_DEFECT_ATTR1_B As String = "T_DEFECT_ATTR1_B"               '[DB]���J�����p�x1������
	Public Const DB_DEFECT_ATTR2_B As String = "T_DEFECT_ATTR2_B"               '[DB]���J�����p�x2������
	Public Const DB_DEFECT_ATTR3_B As String = "T_DEFECT_ATTR3_B"               '[DB]���J�����p�x3������
	Public Const DB_DEFECT_ATTR4_B As String = "T_DEFECT_ATTR4_B"               '[DB]���J�����p�x3������

	'�G�b�W���o����
	Public Const DB_EDGE_COIL_INF As String = "T_EDGE_COIL_INF"                 '[DB]�G�b�W���o����
	Public Const DB_EDGE_CHANGE_INF As String = "T_EDGE_CHANGE_INF"             '[DB]�G�b�W���o���[�h�ω��Ǘ�

	'���H��
	Public Const DB_ACCESS_NEXT_COIL_INF As String = "T_ACCESS_NEXT_COIL_INF"   '[DB] ���H���R�C�����
	Public Const DB_ACCESS_NEXT_DEFECT_INF As String = "T_ACCESS_NEXT_DEFECT_INF"   '[DB] ���H���r���

	'����M�`���o�b�t�@
	Public Const DB_BUF_PRO_R_COILINF As String = "BUF_PRO_R_COILINF"           '[DB]�v���R���R�C������M�o�b�t�@
	Public Const DB_BUF_PRO_R_SAISUN As String = "BUF_PRO_R_SAISUN"             '[DB]�v���R���̐����ю�M�o�b�t�@
	Public Const DB_BUF_PRO_R_CUT As String = "BUF_PRO_R_CUT"                   '[DB]�v���R���J�b�g����M�o�b�t�@
	Public Const DB_BUF_PRO_S_ZISEKI As String = "BUF_PRO_S_ZISEKI"             '[DB]�v���R���������ё��M�`���o�b�t�@
	Public Const DB_BUF_FF_S_ZISEKI As String = "BUF_FF_S_ZISEKI"               '[DB]FF�������ё��M�o�b�t�@
	Public Const DB_BUF_FF_R_MAEINF As String = "BUF_FF_R_MAEINF"               '[DB]FF�O�H���r����M�o�b�t�@

	'// �O�H���p
	Public Const DB_MAE_MASTER As String = "M_MAE_MASTER"                       '[DB]�O�H���}�X�^�[
	Public Const DB_MAE_TYPE_NAME As String = "M_MAE_TYPE_NAME"                 '[DB]�O�H���r��}�X�^�[
	Public Const DB_MAE_GRADE_NAME As String = "M_MAE_GRADE_NAME"               '[DB]�O�H���O���[�h�}�X�^�[

	Public Const DB_MAE_COIL_INF As String = "T_MAE_COIL_INF"                   '[DB]�O�H���R�C�����
	Public Const DB_MAE_COIL_LINE As String = "T_MAE_COIL_LINE"                 '[DB]�O�H�����C���ʃR�C�����
	Public Const DB_MAE_FF_INF As String = "T_MAE_FF_INF"                       '[DB]�O�H���e�e�r���
	Public Const DB_MAE_DEFECT_INF As String = "T_MAE_DEFECT_INF"               '[DB]�O�H���r���
	Public Const DB_MAE_DEFECT_IMG As String = "T_MAE_DEFECT_IMG"               '[DB]�O�H���r�摜

	Public Const DB_DUMMY_DEFECT_INF As String = "T_DUMMY_DEFECT_INF"           '[DB]�[���r��� (�X�g�b�v�}�[�N (�_�~�[�r��p))

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	'// �������p
	Public Const DB_LONGPD_COMMON As String = "M_LONGPD_COMMON"                             '[DB] ���������׌��o_�}�X�^�[�ݒ�e�[�u��
	Public Const DB_LONGPD_PCODE As String = "M_LONGPD_PCODE"                               '[DB] ���������׌��o_�����p�^�[���R�[�h�ʐݒ�e�[�u��
	Public Const DB_LONGPD_PCODE_FILTER As String = "M_LONGPD_PCODE_FILTER"                 '[DB] ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�e�[�u��
	Public Const DB_LONGPD_PCODE_FILTER_OUTPUT As String = "M_LONGPD_PCODE_FILTER_OUTPUT"   '[DB] ����������_�r��ݒ�e�[�u��

	Public Const DB_LONGPD_COIL_RESULT As String = "T_LONGPD_COIL_RESULT"                   '[DB] ���������׌��o_���уe�[�u��
	Public Const DB_LONGPD_EXEC_RESULT_WIDTH As String = "T_LONGPD_EXEC_RESULT_WIDTH"       '[DB] ���������׌��o_�t�B���^�[���уe�[�u��
	Public Const DB_LONGPD_EXEC_RESULT_DEFECT_T As String = "T_LONGPD_EXEC_RESULT_DEFECT_T" '[DB] ����������_�\�r���e�[�u��
	Public Const DB_LONGPD_EXEC_RESULT_DEFECT_B As String = "T_LONGPD_EXEC_RESULT_DEFECT_B" '[DB] ����������_���r���e�[�u��
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'-------------------------------------------------------
	'�p�����[�^�ύX�敪(���C���ŗL����)
	'   �r�����ʉ�ʂ̏ꍇ�́AKizuCommon�ɋL�q���邱��
	'-------------------------------------------------------
	Public Const CH_PARAM_DEFLIST As Integer = 200                              '�r�ꗗ
	Public Const CH_PARAM_SETTIG_ALARM As Integer = 201                         '�r���x��o�͐ݒ�
	Public Const CH_PARAM_MAE_MAST As Integer = 209                             '�O�H���ݒ�
	Public Const CH_PARAM_TAJUUTHRESHOLD As Integer = 210                       '���d����\臒l�ݒ�
	Public Const CH_PARAM_REDUCTION As Integer = 211                            '�k�����ݒ�
	Public Const CH_PARAM_FILTER As Integer = 212                               '�t�B���^�ݒ�
	'Public Const CH_PARAM_CYCLETHRESHOLD As Integer = 213						'�����p����\臒l�ݒ�
	Public Const CH_PARAM_PARA As Integer = 214                                 '���W�b�N�p�������ݒ�
	Public Const CH_PARAM_CROP_SHRINK As Integer = 215                          '�Ԉ����ݒ�
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	Public Const CH_PARAM_LONGPD As Integer = 301                                '���������׌��o
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'-------------------------------------------------------
	'�^�X�N����
	'-------------------------------------------------------
	'���ʃ^�X�N(�኱���C�����̂�BLineCommon���g��Ȃ��ł悢�̂ł���΁AKizuCommon�ɒ�`)

	'�����@�\
	Public Const TO_DEFECT As String = "TO_DEFECT"                              '�����Ǘ�
	Public Const TO_CAMERA As String = "TO_CAMERA"                              '�΃J�����R���g���[���ʐM
	Public Const TO_LINKSW As String = "TO_LINKSW"                              '�΃J���������N�X�C�b�`�ʐM
	Public Const TO_SEQDM As String = "TO_SEQDM"                                '�΃V�[�P���T �f�[�^�������ʐM
	Public Const TO_GETDAT As String = "TO_GETDAT"                              '���̓f�[�^�ҏW�Ǘ�
	Public Const TO_PUTDAT As String = "TO_PUTDAT"                              '���уf�[�^�ҏW�Ǘ�
	Public Const TO_MAEDAT As String = "TO_MAEDAT"                              '�O�H�����ѕҏW����
	Public Const TO_PROCON As String = "TO_PROCON"                              '�΃v���R���ʐM
	Public Const TO_FFSERV As String = "TO_FFSERV"                              '��FF�T�[�o�[�ʐM
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	Public Const TO_CHKLONG As String = "TO_CHKLONG"                            '���������׊Ď�
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'����@�\
	Public Const HT_DEFECT As String = "HT_DEFECT"                              '����Ǘ�

	'�\���@�\
	Public Const HY_DEFECT As String = "HY_DEFECT"                              '�\���Ǘ�
	Public Const HY_TANSHO As String = "HY_TANSHO"                              '�T�����
	Public Const HY_DEFLIST As String = "HY_DEFLIST"                            '�r�ꗗ�\�����
	'Public Const HY_DEFMAP As String = "HY_DEFMAP"                              '�r�}�b�v�\�����

	'����@�\
	Public Const SO_GAMENN As String = "SO_GAMENN"                              '������
	Public Const SO_TRAND As String = "SO_TRAND"                                '�g�����h���
	Public Const SO_DEFMAP As String = "SO_DEFMAP"                              '�����P�� �r�_�}�b�v���


	'��͋@�\
	Public Const KA_PARSET As String = "KA_PARSET"                              '�p�����[�^�ݒ���[�V�X�e���p]
	Public Const KA_PARCOLOR As String = "KA_PARCOLOR"                          '�p�����[�^�ݒ���[�G���W���p]


	'//***************************************************************
	'// �N���v��
	'//***************************************************************
	'+++++++++++++++++++++++++++++++++++
	'�����@�\
	'+++++++++++++++++++++++++++++++++++
	'TO_DEFECT
	Public Const FACT_TO_DEFECT_01 As Integer = 2101        '�p�����[�^�ύX�v��					(��KA_PARSET)
	Public Const FACT_TO_DEFECT_02 As Integer = 2102        '�����J�n�v��						(��SO_GAMENN)
	Public Const FACT_TO_DEFECT_03 As Integer = 2103        '������~�v��						(��SO_GAMENN)
	Public Const FACT_TO_DEFECT_04 As Integer = 2104        '�ғ���Ԏ擾�v��					(��SO_GAMENN)
	'Public Const FACT_TO_DEFECT_05 As Integer = 2105		'�����񖢎g�p
	Public Const FACT_TO_DEFECT_06 As Integer = 2106        '����J�n�A���T�[					(��HT_DEFECT)
	Public Const FACT_TO_DEFECT_07 As Integer = 2107        '�����~�A���T�[					(��HT_DEFECT)
	'Public Const FACT_TO_DEFECT_08 As Integer = 2108		'����p�����[�^�ύX�A���T�[			(��HT_DEFECT)
	Public Const FACT_TO_DEFECT_09 As Integer = 2109        '�����Ԗ⍇����					(��HT_DEFECT)
	'Public Const FACT_TO_DEFECT_10 As Integer = 2110		'�t���[���摜�擾���s�ʒm			(��HT_DEFECT)
	Public Const FACT_TO_DEFECT_11 As Integer = 2111        '�R�C������M�ʒm					(��TO_GETDAT)
	Public Const FACT_TO_DEFECT_12 As Integer = 2112        '�O�H���r��񏑍��݊���				(��TO_MAEDAT)
	Public Const FACT_TO_DEFECT_13 As Integer = 2113        '���я������ݏ󋵖⍇��				(��TO_PUTDAT)
	Public Const FACT_TO_DEFECT_14 As Integer = 2114        '���I�����ԕ�                     (��TO_CAMERA)
	Public Const FACT_TO_DEFECT_15 As Integer = 2115        '�C�j�V�����`����M�ʒm				(��TO_PROCON)
	'Public Const FACT_TO_DEFECT_16 As Integer = 2116       '�J�b�g�͈͏��ʒm			    	(��TO_PUTDAT)
	'Public Const FACT_TO_DEFECT_17 As Integer = 2117		'�����񖢎g�p
	'Public Const FACT_TO_DEFECT_18 As Integer = 2118		'�����񖢎g�p
	'Public Const FACT_TO_DEFECT_19 As Integer = 2119		'�����񖢎g�p
	Public Const FACT_TO_DEFECT_20 As Integer = 2120        '�{�C�X�x��o�͉����v��				(��HY_TANSHO)
	Public Const FACT_TO_DEFECT_21 As Integer = 2121        '�r���J�n�v��					    (��SO_GAMENN)
	Public Const FACT_TO_DEFECT_22 As Integer = 2122        '�r����~�v��					    (��SO_GAMENN)
	Public Const FACT_TO_DEFECT_23 As Integer = 2123        '�r���J�n�A���T�[                   (��HT_DEFECT)
	Public Const FACT_TO_DEFECT_24 As Integer = 2124        '�r����~�A���T�[                   (��HT_DEFECT)
	'FACT_TO_DEFECT_30�ȍ~��TO_LINKSW����̉����ŗ\��
	Public Const FACT_TO_DEFECT_30 As Integer = 2130        '�J���������N�X�C�b�`�ݒ芮���ʒm	(��TO_LINKSW)
	Public Const FACT_TO_DEFECT_31 As Integer = 2131        '�r���Q���ݒ芮���ʒm				(��TO_LINKSW)
	Public Const FACT_TO_DEFECT_32 As Integer = 2132        '�J�����I��ݒ芮���ʒm             (��TO_LINKSW)

	'TO_CAMERA
	Public Const FACT_TO_CAMERA_01 As Integer = 2201        '�p�����[�^�ύX�v��					(��TO_DEFECT)
	Public Const FACT_TO_CAMERA_02 As Integer = 2202        '�����p�ݒ�R�}���h					(��TO_DEFECT)
	Public Const FACT_TO_CAMERA_03 As Integer = 2203        '�p���X�o�͐ݒ�						(��TO_DEFECT)
	Public Const FACT_TO_CAMERA_04 As Integer = 2204        'AGC�ݒ�							(��TO_DEFECT)
	Public Const FACT_TO_CAMERA_05 As Integer = 2205        '�R�C���ؑ�							(��TO_DEFECT)
	Public Const FACT_TO_CAMERA_06 As Integer = 2206        '���I�����Ԗ₢���킹				(��TO_DEFECT)
	Public Const FACT_TO_CAMERA_07 As Integer = 2207        '�Z���p�ݒ�R�}���h					(��TO_DEFECT)

	'TO_LINKSW
	Public Const FACT_TO_LINKSW_01 As Integer = 2301        '�p�����[�^�ύX�ʒm					(��TO_DEFECT)
	Public Const FACT_TO_LINKSW_02 As Integer = 2302        '�����p�ݒ�R�}���h					(��TO_DEFECT)
	Public Const FACT_TO_LINKSW_03 As Integer = 2303        '�J�����I��ݒ�R�}���h				(��TO_DEFECT)
	Public Const FACT_TO_LINKSW_04 As Integer = 2304        '�r���Q���ݒ�R�}���h				(��TO_DEFECT)
	Public Const FACT_TO_LINKSW_05 As Integer = 2305        '�������Z�b�g�ʒm					(��TO_DEFECT)
	Public Const FACT_TO_LINKSW_06 As Integer = 2306        'AOC���x�ύX�w��					(��TO_LINKSW)

	'KT_LEDMGR
	Public Const FACT_KT_LEDMGR_01 As Integer = 2401        '�p�����[�^�ύX�ʒm					(��TO_DEFECT)
	Public Const FACT_KT_LEDMGR_02 As Integer = 2402        '�ݒ�v�� �i�����\�ʏ�Ԏw��j		(��TO_DEFECT)
	Public Const FACT_KT_LEDMGR_03 As Integer = 2403        '�ݒ�v�� �i���ʎw��j				(��TO_DEFECT)
	Public Const FACT_KT_LEDMGR_04 As Integer = 2404        '�_���E�����w��
	Public Const FACT_KT_LEDMGR_05 As Integer = 2405        '�ݐϓ_�����ԃ��Z�b�g				(��SO_GAMENN)

	'TO_SEQDM
	'Public Const FACT_TO_SEQDM_01 As Integer  = 2401		'�����񖢎g�p

	'TO_GETDAT
	'Public Const FACT_TO_GETDAT_01 As Integer = 2501		'�p�����[�^�ύX�v��					(��TO_DEFECT)
	Public Const FACT_TO_GETDAT_02 As Integer = 2502        '�R�C������M�ʒm					(��TO_SEQDM)

	'TO_PUTDAT
	'Public Const FACT_TO_PUTDAT_01 As Integer = 2601		'�p�����[�^�ύX�v��					(��TO_DEFECT)
	Public Const FACT_TO_PUTDAT_02 As Integer = 2602        '�̐����ѓ`����M�ʒm				(��TO_PROCON)
	Public Const FACT_TO_PUTDAT_03 As Integer = 2603        '�J�b�g���ѓ`����M�ʒm				(��TO_PROCON)
	Public Const FACT_TO_PUTDAT_04 As Integer = 2604        '���я������݊����ʒm				(��TO_DEFECT)

	'TO_MAEDAT
	'Public Const FACT_TO_MAEDAT_01 As Integer = 2701		'�p�����[�^�ύX�v��					(��TO_DEFECT)
	Public Const FACT_TO_MAEDAT_02 As Integer = 2702        '�����p�R�C������M�����ʒm		(��TO_DEFECT)
	Public Const FACT_TO_MAEDAT_03 As Integer = 2703        '�O�H���r����M�����ʒm			(��TO_FFSERV)

	'TO_PROCON
	Public Const FACT_TO_PROCON_01 As Integer = 2801        '�v���R���������ё��M�v��			(��TO_PUTDAT)

	'TO_FFSERV
	Public Const FACT_TO_FFSERV_01 As Integer = 2901        '�������ѓ`�����M�v��				(��TO_PUTDAT)
	Public Const FACT_TO_FFSERV_02 As Integer = 2902        '�O�H���r���v���ʒm				(��TO_MAEDAT)



	'+++++++++++++++++++++++++++++++++++
	'����@�\
	'+++++++++++++++++++++++++++++++++++
	'HT_DEFECT
	Public Const FACT_HT_DEFECT_01 As Integer = 3101        '�p�����[�^�ύX�ʒm					(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_02 As Integer = 3102        '�����J�n�v��						(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_03 As Integer = 3103        '������~�v��						(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_04 As Integer = 3104        'Jpeg���k���ݒ�						(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_05 As Integer = 3105        '��ʏ��ݒ�v��					(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_06 As Integer = 3106		'�x�����s�L�����Z��					(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_07 As Integer = 3107        '���s�s�\ ����					(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_08 As Integer = 3108        '��Ԗ₢���킹						(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_09 As Integer = 3109		'�t���[���摜�擾�v��				(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_10 As Integer = 3110		'�G�b�W���o�T�[�`�͈͕ύX�˗�		(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_11 As Integer = 3111		'���S���]���f�[�^���W�v��			(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_12 As Integer = 3112		'�摜���R�[�f�B���O�v��				(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_13 As Integer = 3113		'����AGC�ݒ�						(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_14 As Integer = 3114		'��ʏ��ݒ�ύX�v��(������)		(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_15 As Integer = 3115		'�J�����Q�C���ύX�v��				(��TO_DEFECT)
	'Public Const FACT_HT_DEFECT_16 As Integer = 3116		'�I�����ԕύX�ʒm					(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_19 As Integer = 3119        '�����s�ݒ�̃L�����Z���v��			(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_20 As Integer = 3120        '���������J�E���^�ݒ�v��			(��TO_DEFECT)
	Public Const FACT_HT_DEFECT_21 As Integer = 3121        '�p�����[�^���O�Ǎ��v��				(��TO_DEFECT)

	'+++++++++++++++++++++++++++++++++++
	'�\���@�\
	'+++++++++++++++++++++++++++++++++++
	'HY_DEFECT
	Public Const FACT_HY_DEFECT_01 As Integer = 5101        '�p�����[�^�ύX�ʒm					(��TO_DEFECT)
	Public Const FACT_HY_DEFECT_02 As Integer = 5102        '�\����񏉊����ʒm					(��TO_DEFECT)
	Public Const FACT_HY_DEFECT_03 As Integer = 5103        '�O�H���r���Ǎ��ʒm				(��TO_DEFECT)
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	Public Const FACT_HY_DEFECT_04 As Integer = 5104        '�O�H���r���Ǎ������ʒm			(��HY_DEFECT)
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	Public Const FACT_HY_DEFECT_05 As Integer = 5105        '���������ה����ʒm					(��TO_LONGCHK)
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'HY_TANSHO
	Public Const FACT_HY_TANSHO_01 As Integer = 5201		'�p�����[�^�ύX�ʒm					(��HY_DEFECT)
	Public Const FACT_HY_TANSHO_02 As Integer = 5202		'�C�j�V�����ʒm						(��HY_DEFECT)

	'HY_DEFLIST
	Public Const FACT_HY_DEFLIST_01 As Integer = 5301		'�p�����[�^�ύX�ʒm					(��HY_DEFECT)
	'Public Const FACT_HY_DEFLIST_02 As Integer = 5302		'�C�j�V�����ʒm						(��HY_DEFECT)
	
	''HY_DEFMAP
	'Public Const FACT_HY_DEFMAP_01 As Integer = 5401		'�p�����[�^�ύX�ʒm					(��HY_DEFECT)
	'Public Const FACT_HY_DEFMAP_02 As Integer = 5402		'�C�j�V�����ʒm						(��HY_DEFECT)
	'Public Const FACT_HY_DEFMAP_03 As Integer = 5403		'�\���X�V�v��						(��HY_DEFECT)

	'+++++++++++++++++++++++++++++++++++
	'����@�\
	'+++++++++++++++++++++++++++++++++++
	'SO_GAMENN
	Public Const FACT_SO_GAMENN_01 As Integer = 6101		'�ғ���Ԓʒm						(��TO_DEFECT)
	'SO_TRAND
	Public Const FACT_SO_TRAND_01 As Integer = 6201			'��ʍX�V 							(��TO_DEFECT)
    'SO_DEFMAP
    Public Const FACT_SO_DEFMAP_01 As Integer = 6301        '�p�����[�^�ύX�ʒm 				(��TO_DEFECT)
    Public Const FACT_SO_DEFMAP_02 As Integer = 6302        '��ʍX�V 							(��SO_TRAND)
 
	'+++++++++++++++++++++++++++++++++++
	'�S���摜�ۑ��@�\
	'+++++++++++++++++++++++++++++++++++
	'TI_RPISAVE
	Public Const FACT_TI_RPISAVE_01 As Integer = 7101		'�����J�n�v��						(��TO_DEFECT)
	Public Const FACT_TI_RPISAVE_02 As Integer = 7102		'������~�v��						(��TO_DEFECT)
	Public Const FACT_TI_RPISAVE_03 As Integer = 7103		'�B���J�n/��~�v��					(��TO_DEFECT)
	Public Const FACT_TI_RPISAVE_04 As Integer = 7104		'���R�C����񎖑O�ʒm				(��TO_DEFECT)
	'Public Const FACT_TI_RPISAVE_05 As Integer = 7105		'�O���R�C���ؑ֒ʒm					(��TO_DEFECT)
	'Public Const FACT_TI_RPISAVE_06 As Integer = 7106		'�₢���킹�v��						(��TO_DEFECT)
	Public Const FACT_TI_RPISAVE_07 As Integer = 7107		'�ē����v��							(��TO_DEFECT)
	Public Const FACT_TI_RPISAVE_08 As Integer = 7108		'�r���Q�������ʒm					(��TO_DEFECT)


	'///////////////////////////////////////////////////////////////////////////
	'�񋓑�
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �\���ʒu
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_DISP
        ''����
        DIV_LEN_POS_KEN_TOP = 0                             '[�ʔ���] ������_ �\�@(�J�����ʒu)
        DIV_LEN_POS_KEN_BOT                                 '[�ʔ���] ������_ ���@(�J�����ʒu)
        DIV_LEN_POS_DSP_TOP                                 '[�ʔ���] �\����_ �\�@(������ʒu1)
        DIV_LEN_POS_DSP_BOT                                 '[�ʔ���] �\����_ ���@(������ʒu2)
        DIV_LEN_POS_BASE_END                                '���ʍ��ڂ̍Ō�

        ''��
        DIV_DISP_POS_DSP_P1                                 '[�ʔ���] �ʈʒu1
        DIV_DISP_POS_DSP_P2                                 '[�ʔ���] �ʈʒu2
        'DIV_DISP_POS_DSP_P3                                 '[�ʔ���] �ʈʒu3
        'DIV_DISP_POS_DSP_P4                                 '[�ʔ���] �ʈʒu4
        'DIV_DISP_POS_DSP_P5                                 '[�ʔ���] �ʈʒu5

        DIV_DISP_POS_END = DIV_LEN_POS_BASE_END             '�Ō�Ɏg�p

	End Enum

	''' <summary>
	''' �V�X�e�����
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_SYS
		DIV_SYS_INIT = 1									'[�V�X�e�����]��~���[�h
		DIV_SYS_IDLE										'[�V�X�e�����]��������
		DIV_SYS_INITSTART									'[�V�X�e�����]�����J�n��������
		DIV_SYS_WAITSTART									'[�V�X�e�����]�����J�n�҂�

		DIV_SYS_RUN											'[�V�X�e�����]������

		DIV_SYS_WAITSTOP									'[�V�X�e�����]������~�҂�
		DIV_SYS_STOP										'[�V�X�e�����]������~
		DIV_SYS_RETRY										'[�V�X�e�����]���������ċN����
	End Enum

    ''' <summary>
    ''' �����@�\�֘A
    ''' </summary>
    ''' <remarks></remarks>
	Public Enum EM_DIV_CYCLESTAT
		DIV_CYCLESTAT_NON = 1								'[�����@�\���] �ΏۊO (SPM�J������)
		DIV_CYCLESTAT_OK = 0								'[�����@�\���] ����
		DIV_CYCLESTAT_STOP = -9								'[�����@�\���] ��~  (�Жʈُ�̏ꍇ�A������~��)
	End Enum

    ''' <summary>
    ''' ���W�b�N�p������ �ғ����
    ''' </summary>
    ''' <remarks></remarks>
	Public Enum EM_DIV_PARA_KADOU
		DIV_PARA_KADOU_STOP = 0								'[�ғ����] ��~
		DIV_PARA_KADOU_WAITSTOP								'[�ғ����] ��~��
		DIV_PARA_KADOU_WAITSTART							'[�ғ����] �J�n��
		DIV_PARA_KADOU_START								'[�ғ����] ������
	End Enum

    ''' <summary>
    ''' �S������
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_HANTEI
		DIV_HANTEI_NON = -1									'[�S������]�֌W����
		
		DIV_HANTEI_BK = 0									'[�S������]�o�b�N�A�b�v
		DIV_HANTEI_HT										'[�S������]�P������
		DIV_HANTEI_CY										'[�S������]��������
		DIV_HANTEI_PR										'[�S������]�p����������
	End Enum

	''' <summary>
	''' �\���Ǘ��̎�M���
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_HY_RECVST
		DIV_HY_RECVST_SKIP = -1								'[��M���] �X�L�b�v (�����x��ɂ���M�f�[�^�j�����)
		DIV_HY_RECVST_NON = 0								'[��M���] ����M
		DIV_HY_RECVST_OK									'[��M���] �����M
	End Enum

	'//=======================================
	'// LED�Ɩ����u �J���[
	'//=======================================
	Public Enum EM_DIV_LED_COLOR
		DIV_LED_COLOR_R = 0									'[LED�Ɩ����u] R
		DIV_LED_COLOR_G1									'[LED�Ɩ����u] G1
		DIV_LED_COLOR_G2									'[LED�Ɩ����u] G2
		DIV_LED_COLOR_B1									'[LED�Ɩ����u] B1
		DIV_LED_COLOR_B2									'[LED�Ɩ����u] B2

		DIV_LED_COLOR_END									'�I�[�Ŏg�p���邽�ߒ���
	End Enum

	'//=======================================
	'// LED�Ɩ����u ���j�b�g
	'//=======================================
	Public Enum EM_DIV_LED_UNIT
		DIV_LED_UNIT_R = 0									'[LED�Ɩ����u] R
		DIV_LED_UNIT_G = DIV_LED_UNIT_R						'[LED�Ɩ����u] G
		DIV_LED_UNIT_B										'[LED�Ɩ����u] B

		DIV_LED_UNIT_END									'�I�[�Ŏg�p���邽�ߒ���
	End Enum
	Public Const MAX_LED_CH As Integer = 3					'1���j�b�g�ӂ�̃`�����l����
	Public Const MAX_LED_TEMP_UNIT As Integer = 3			'���x���j�b�g��

	'//=======================================
	'// �摜��� (�G���W������o�͂����摜�Z�b�g��)
	'//=======================================
    Public Enum EM_DIV_IMAGE_KIND
        DIV_IMAGE_KIND_R = 0                                ' ��
        DIV_IMAGE_KIND_G                                    ' ��
        DIV_IMAGE_KIND_B                                    ' ��
        DIV_IMAGE_KIND_GB                                   ' �F��(G-B)
    End Enum

    '//=======================================
    '// �J�b�g�敪��
    '//=======================================
    Public Enum EM_DIV_CUT
        DIV_CUT_FCUT = 1                                    ' F�J�b�g
        DIV_CUT_LCUT										' L�J�b�g
		DIV_CUT_DIV											' ����
		DIV_CUT_WPD_OTHER									' �َ�_

		'DIV_CUT_CUT = 1			0						' ���i ��[-���[�ʒu
    End Enum

    Public Const ROW_KIND_UPPER As Integer = &H1            ' ���i��[
    Public Const ROW_KIND_BOTTOM As Integer = &H2           ' ���i���[
    Public Const ROW_KIND_OTHER As Integer = &H4            ' �َ�_
    Public Const ROW_KIND_YOSETU As Integer = &H8           ' �n�ړ_

	'//=======================================
	'// SPM�����敪
	'//=======================================
	Public Enum EM_DIV_SPM_STAT
		DIV_SPM_INI = -1									' SPM���������l
		DIV_SPM_FALSE = 0									' SPM��������
		DIV_SPM_TRUE										' SPM�����L��
    End Enum

    '//=======================================
    '// ���ъi�[�敪
    '//=======================================
    Public Enum ENUM_END_MODE
        DIV_END_NON = 0                                     ' �s��
        DIV_END_NORMAL = 1                                  ' �ʏ�؂�ւ�
        DIV_END_BACKUP                                      ' �o�b�N�A�b�v�ؑ�(�����p�R�C�������Ǘ�����ɓ��B��)
        DIV_END_STOP_MANUAL                                 ' �蓮��~
        DIV_END_STOP_AUTO                                   ' ������~
    End Enum

    '//=======================================
    '// �v���R���̐��敪
    '//=======================================
    Public Enum EM_DIV_PRO_CUT
		DIV_PRO_CUT_NORMAL = 0								' [�v���R���̐��敪] �ʏ�J�b�g
		DIV_PRO_CUT_FRONT_UNION								' [�v���R���̐��敪] �O�R�C���ƌ���
		DIV_PRO_CUT_TAIL_UNION								' [�v���R���̐��敪] ��R�C���ƌ���
		DIV_PRO_CUT_UNION									' [�v���R���̐��敪] ����(�O�R�C���A���R�C��)����
    End Enum


	'//=======================================
	'// �{�V�X�e���ŗL�̍\����
	'//=======================================
	''' <summary>
	''' �摜�捞�J�n�`�������������܂ł̏��v����
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_REQUIRED_TIME
		EM_TIME_HT_IMGGETSTART_TO_RECVEND = 1				'�摜�����G���W�����摜�捞�J�n�`��������M����
		EM_TIME_HT_IMGGETSTART_TO_EXECEND					'�摜�����G���W�����摜�捞�J�n�`��������������
		EM_TIME_HT_IMGEXECEND_TO_RECVSTART					'�摜�����G���W�����摜���������`��������M�J�n
		EM_TIME_TO_RECVSTART_TO_RECVEND_CAM					'��������M�J�n�`������1�J�����Z�b�g���̎�M������
		EM_TIME_TO_RECVSTART_TO_RECVEND_ALL					'��������M�J�n�`�������S�J�����Z�b�g���̎�M������
		EM_TIME_HT_IMGGETSTART_HT_IMGEXECEND				'�摜�����G���W�����摜�捞�J�n�`�摜�����G���W�����摜��������
	End Enum
	Public Const MAX_REQUIRED_TIME As Integer = 10


    '***************************************************************
    ' �V�X�e�����ʍ\����
    '***************************************************************
    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '���������r���e�[�u��[TBL_HY_DEFECTINF]��������
    '-------------------------------------------------------
    '�r���[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DETECT_BASE_DATA As Integer = 192
    ''' <summary>
    ''' �r���[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DETECT_BASE_DATA, pack:=1)> _
    Public Structure DETECT_BASE_DATA
        Dim nKizuNo As Integer                              '�rNo(1�`�̘A��)
        Dim TorB As UShort                                  '�\���敪(0:�\ 1:��)
        Dim nTid As UShort                                  '�r��EdasysID
        Dim nGid As UShort                                  '�O���[�hEdasysID

        Dim cycle_div As UShort                             '�����r�敪(0:����r, 1:�ȈՎ����r,�A���r )
        Dim dsws As UShort                                  'DSWS�敪(0:DS 1:WS)
        Dim camset As UShort                                '�J�����Z�b�gID (1�`)	�\DS, �\WS, ��DS, ��WS

        Dim y As Single                                     'F����̈ʒu[mm]
        Dim y_out As Single                                 '�o�������R�C����[����̈ʒu[mm]�������ł�y�Ɠ��������B �\���Ǘ��ŁA�o�������ɐU�蒼��
        Dim ds As Single                                    'DS���G�b�W����̈ʒu[mm]
        Dim ws As Single                                    'WS���G�b�W����̈ʒu[mm]
        Dim dsc As Single                                   '�Z���^�[����̋���[mm]
        Dim wsc As Single                                   '�Z���^�[����̋���[mm]

        Dim speed As Integer                                '���x[mpm]
        Dim pitch As Integer                                '�s�b�`[mm]
        Dim roolkei As Single                               '���[���a[mm]

        Dim aria As Single                                  '�ʐ�[mm^2]
        Dim kizu_wid As Integer                             '�r��[mm]
        Dim kizu_len As Integer                             '�r����[mm]

        Dim h_res As Single                                 '�c����\[mm/pixel]
        Dim w_res As Single                                 '������\[mm/pixel]
        Dim nFrameNo As Integer                             '�t���[��No

        ' �t���[���摜�̍��ォ��̈ʒu
        Dim frm_x_min As Short                              '�t���[���O��Xmin[pixel]
        Dim frm_x_max As Short                              '�t���[���O��Xmax[pixel]
        Dim frm_y_min As Short                              '�t���[���O��Ymin[pixel]
        Dim frm_y_max As Short                              '�t���[���O��Ymax[pixel]

        ' �Ԉ����t���[���摜�̍��ォ��̈ʒu
        Dim cull_frm_x_min As Short                         '�Ԉ����t���[���O��Xmin[pixel]
        Dim cull_frm_x_max As Short                         '�Ԉ����t���[���O��Xmax[pixel]
        Dim cull_frm_y_min As Short                         '�Ԉ����t���[���O��Ymin[pixel]
        Dim cull_frm_y_max As Short                         '�Ԉ����t���[���O��Ymax[pixel]

        ' �؂�o���摜�֌W (���_�́A�؂�o���摜�̍��ォ��̈ʒu)
        Dim box_x_min As Short                              '�O��Xmin[pixel](�؂�o���摜�̍��ォ��̈ʒu)(�}�C�i�X�l���肦��)
        Dim box_x_max As Short                              '�O��Xmax[pixel]
        Dim box_y_min As Short                              '�O��Ymin[pixel]
        Dim box_y_max As Short                              '�O��Ymax[pixel]
        Dim center_x As Short                               '�r���S�ʒuX[pixel](�؂�o���摜�̍��ォ��̈ʒu)
        Dim center_y As Short                               '�r���S�ʒuY[pixel]
        Dim h_size As UShort                                '�c��f��[pixel]
        Dim w_size As UShort                                '����f��[pixel]

        '�����܂ł� 108byte ----------------------------------------------------
        Dim nTyuu As UShort                                 '�r��D��x
        Dim nGyuu As UShort                                 '�O���[�h�D��x
        Dim nWidAbsPos As Integer                           '��������Έʒu
        Dim nWidDiv As Short                                '�������敪 (0:DS �` 12:WS)
        Dim nObjTansho As Short                             '�T����ʑΏ��r(1:�Ώ�)
        Dim nObjDeflist As Short                            '�d���׈ꗗ��ʑΏ��r
        Dim nModeDeflist As Short                           '�d���׈ꗗ�����\���Ώ��r  (0:�����g����(�����r�폜)  1:�����g�\��(�蓮�r�폜)) 
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim time() As Short                                 '�r���������N���������b
        ' ���摜�̐؂�o���摜�֌W (���_�́A�؂�o���摜�̍��ォ��̈ʒu)
        Dim base_box_x_min As Short                         '���摜_�O��Xmin[pixel](�؂�o���摜�̍��ォ��̈ʒu)(�}�C�i�X�l���肦��)
        Dim base_box_x_max As Short                         '���摜_�O��Xmax[pixel]
        Dim base_box_y_min As Short                         '���摜_�O��Ymin[pixel]
        Dim base_box_y_max As Short                         '���摜_�O��Ymax[pixel]
        Dim base_center_x As Short                          '���摜_�r���S�ʒuX[pixel](�؂�o���摜�̍��ォ��̈ʒu)
        Dim base_center_y As Short                          '���摜_�r���S�ʒuY[pixel]
        Dim base_h_size As UShort                           '���摜_�c��f��[pixel]
        Dim base_w_size As UShort                           '���摜_����f��[pixel]
        Dim base_h_res As Single                            '���摜_�c����\[mm/pixel]
        Dim base_w_res As Single                            '���摜_������\[mm/pixel]
        Dim nShrinkRatio As Integer                         '�k���� (1, 2, 4)
		'// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
		'<VBFixedArray(28 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=28)>
		'Dim nYobi() As Byte                                 '�\��

		Dim nIFMergeFlg As Integer                           ' �t���[���ԘA���t���O(0:�P���r 1:�A���r�擪 2:�A���r�擪�ȊO)
		<VBFixedArray(24 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=24)>
		Dim nYobi() As Byte                                 '�\��
		'// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>

		Public Sub initialize()
			ReDim time(6 - 1)
			'// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
			'ReDim nYobi(28 - 1)
			ReDim nYobi(24 - 1)
			'// 2023.09.14 �摜�������u H�ES�\�t�g���� <<<<<--------------------
		End Sub
    End Structure

    '���������R�C�����e�[�u��[TBL_HY_COILINF]��������
    '-------------------------------------------------------
    '�V�X�e�����ʂ̃R�C�����[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_COIL_BASEDATA As Integer = 96
    ''' <summary>
    ''' �V�X�e�����ʂ̃R�C�����[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COIL_BASEDATA, pack:=1)> _
    Public Structure COIL_BASEDATA
        ' ��{�� (TO_GETDAT�ŃZ�b�g)
        <VBFixedArray(SIZE_MEI_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_MEI_NO + 1)> _
        Dim cMeiNo() As Byte                                '����No+NULL
        <VBFixedArray(SIZE_COIL_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COIL_NO + 1)> _
        Dim cCoilNoIn() As Byte                             '�����R�C��No+NULL
        <VBFixedArray(SIZE_COIL_KEY - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COIL_KEY)>
        Dim cCoilKey() As Byte                              '�R�C������ӂɓ��肷�邽�߂̃L�[+NULL
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cYobi1() As Byte                                '�\��[�o�E���_�����킹]
        Dim nLenIn As Integer                               '�����R�C����[mm]
        Dim nAtu As Integer                                 '��[��m]
        Dim nWid As Integer                                 '��[mm](=�ʏ��ʂ̔������A��ʏ�񂪖����ꍇ���́A�ő�����Z�b�g���Ă���)

        ''���������܂ł͌Œ�------------------------------
        Dim nCutLenF As Integer                             '�����t�����g�[����
        Dim nCutLenT As Integer                             '�����e�[���[����
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cYobi2() As Byte                                '�\��[�o�E���_�����킹]
        Dim dNobiSPM As Double                              'SPM�L�ї�
        Dim nLenOut As Integer                              '�o���R�C����
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)>
        Dim cYobi3() As Byte                                '�\��
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cCoilNoIn(SIZE_COIL_NO - 1 + 1)
            ReDim cMeiNo(SIZE_MEI_NO - 1 + 1)
            ReDim cCoilKey(SIZE_COIL_KEY - 1)
            ReDim cYobi1(2 - 1)
            ReDim cYobi2(4 - 1)
            ReDim cYobi3(20 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�R�C����{�ݒ�[TBL_TO_COIL]
    '-------------------------------------------------------
    '�\���@�\�ł͖��g�p�����uLineCommon.h�v�Ɠ����ɂ��邽��
    Public Const SIZE_COIL_SETTING As Integer = 64
    ''' <summary>
    ''' �R�C����{�ݒ�[TBL_TO_COIL]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COIL_SETTING, pack:=1)> _
    Public Structure COIL_SETTING
        Dim bWalm As Byte                                   '�E�H�[�}�ދ敪(ture:�E�H�[�}��)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim bDefScode() As Byte                             '�����\�ʏ�� �f�t�H���g�l�g�p(true:��������ُ̈�Ńf�t�H���g�l�g�p��)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim bDefPcode() As Byte                             '�����p�^�[�� �f�t�H���g�l�g�p(true:��������ُ̈�Ńf�t�H���g�l�g�p��)
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)>
        Dim cYobi1() As Byte                                '�\��[�o�E���_�����킹]
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim scode() As Integer                              '�����\�ʏ��(�\/��)(1�I���W��)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim pcode() As Integer                              '�����p�^�[��(�\/��)(1�I���W��)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim scode_spmoff() As Integer                       'SPM�J�����̌����\�ʏ��(�\/��)(1�I���W��)
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim pcode_spmoff() As Integer                       'SPM�J�����̌����p�^�[��(�\/��)(1�I���W��)
        <VBFixedArray(24 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=24)>
        Dim cYobi2() As Byte                                '�\��

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim bDefScode(NUM_MEN - 1)
            ReDim bDefPcode(NUM_MEN - 1)
            ReDim cYobi1(3 - 1)
            ReDim scode(NUM_MEN - 1)
            ReDim pcode(NUM_MEN - 1)
            ReDim scode_spmoff(NUM_MEN - 1)
            ReDim pcode_spmoff(NUM_MEN - 1)
            ReDim cYobi2(24 - 1)
        End Sub
    End Structure


    '=======================================
    ' ��ʕ\���p �\����
    '=======================================
    '-------------------------------------------------------
    '�R�C����񋤒ʕ�[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_COINFO_BASE As Integer = 128 + 1024
    Public Const SIZE_COINFO_RESERVE As Integer = 1024 - SIZE_COIL_BASEDATA - SIZE_PRO_TYPE_DATA_COILINF
    ''' <summary>
    ''' �R�C����񋤒ʕ�[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COINFO_BASE, pack:=1)> _
    Public Structure COINFO_BASE
        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)> _
        Dim cKizukenNo() As Byte                            '�Ǘ�No
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim scode() As Integer                              '�����\�ʏ��
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim pcode() As Integer                              '�����p�^�[��
        Dim nMapColNum As Integer                           '�}�b�v��

        Dim nCoilKensa As Integer                           '���̃R�C���ň��Ԃł��k�ގ�:1 ����:0
        <VBFixedArray(84 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=84)> _
        Dim yobi() As Byte                                  '�\��

        Dim base As COIL_BASEDATA                           '�V�X�e�����ʂ̃R�C�����
        Dim data As PRO_TYPE_DATA_COILINF                   '�R�C�����

        <VBFixedArray(SIZE_COINFO_RESERVE - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COINFO_RESERVE)> _
        Dim reserve() As Byte                               '�\��
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cKizukenNo(SIZE_KIZUKEN_NO - 1)
            ReDim scode(NUM_MEN - 1)
            ReDim pcode(NUM_MEN - 1)
            ReDim yobi(84 - 1)
            ReDim reserve(SIZE_COINFO_RESERVE - 1)
            base.initialize()
            data.initialize()

        End Sub
    End Structure

    '���������}�b�v�ʒu�e�[�u��[TBL_HY_MAP_ROW]��������
    '-------------------------------------------------------
    '�ڍ׏��[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_ROW As Integer = 64
    ''' <summary>
    ''' �ڍ׏��[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_ROW, pack:=1)> _
    Public Structure DSP_TYP_ROW
        Dim nEnable As Integer                              '����M���(�̂́A�s�����-1�Ŕ��f���Ă����z) (0:����M 1:��M)
        Dim emKadou As EM_DIV_KADOU                         '�ғ���� (DIV_KADOU_INIT���)
        Dim emDspKensa As EM_DIV_KENSA                      '�\���p������� (DIV_KENSA_NON���)
        Dim emDspKiki As EM_DIV_KIKI                        '�\���p�@���� (DIV_KIKI_OK���)
        Dim nKizuCnt As Integer                             '�s�P�ʕ\���Ώ��r��

        Dim nLen_i As Integer                               '��������(�v�͗n�ړ_����̋���) [m]
        Dim nLen_o As Integer                               '�o������(�v�̓J�b�g����̋���) [m]
        Dim nCut As Integer                                 '�V���[�J�b�g�敪(0:�V���[�J�b�g���� 1:�V���[�J�b�g���ꂽ)
        Dim nKind As Integer                                '�t����� (0:�t����񖳂� 0x01:���i��[  0x02�F���i���[)

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim pnt() As Integer                                '�r���ւ̃|�C���^[0:�ŏ�/1:�ő�]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim pntMae() As Integer                             '�O�H���r���ւ̃|�C���^[0:�ŏ�/1:�ő�]
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim yobi() As Integer                               '�\��
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim pnt(2 - 1)
            ReDim pntMae(2 - 1)
            ReDim yobi(3 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�s���[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_MAP_ROW_ROW As Integer = SIZE_DSP_TYP_ROW
    ''' <summary>
    ''' �s���[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAP_ROW_ROW, pack:=1)> _
    Public Structure MAP_ROW_ROW
        Dim typ As DSP_TYP_ROW                              '�ڍ׏��[TBL_HY_MAP_ROW]
        '�C�j�V�����C�Y
        Public Sub initialize()
            typ.initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    '�ʏ��[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_MAP_ROW_MEN As Integer = SIZE_MAP_ROW_ROW * MAP_ROW_NUM
    ''' <summary>
    ''' �ʏ��[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAP_ROW_MEN, pack:=1)> _
    Public Structure MAP_ROW_MEN
        <VBFixedArray(MAP_ROW_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_ROW_NUM)> _
        Dim row() As MAP_ROW_ROW                            '�s���[TBL_HY_MAP_ROW]
        '�C�j�V�����C�Y
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            If nRow = 0 Then
                ReDim row(MAP_ROW_NUM - 1)
            Else
                ' �s���w��̏��������s��ꂽ�ꍇ�̑Ή�
                ReDim row(nRow - 1)
            End If
            For ii As Integer = 0 To row.Length - 1
                row(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '�}�b�v�ؑ�[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_MAP_ROW_STAGE As Integer = SIZE_MAP_ROW_MEN * NUM_MEN
    ''' <summary>
    ''' �}�b�v�ؑ�[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAP_ROW_STAGE, pack:=1)> _
    Public Structure MAP_ROW_STAGE
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)> _
        Dim men() As MAP_ROW_MEN                            '�ʏ��[TBL_HY_MAP_ROW]
        '�C�j�V�����C�Y
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim men(NUM_MEN - 1)
            For ii As Integer = 0 To men.Length - 1
                men(ii).initialize(nRow)
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '���R�[�h[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_DSP_MAP_ROW As Integer = SIZE_MAP_ROW_STAGE * MAP_STAGE_NUM
    ''' <summary>
    ''' ���R�[�h[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_MAP_ROW, pack:=1)> _
    Public Structure DSP_MAP_ROW
        <VBFixedArray(MAP_STAGE_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_STAGE_NUM)> _
        Dim stage() As MAP_ROW_STAGE                        '�}�b�v�ؑ�[TBL_HY_MAP_ROW]
        '�C�j�V�����C�Y
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim stage(MAP_STAGE_NUM - 1)
            For ii As Integer = 0 To stage.Length - 1
                stage(ii).initialize(nRow)
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '�\���r���[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_DEFECTINF As Integer = 440
    Public Const SIZE_DSP_TYP_DEFECTINF_RESERVE As Integer = SIZE_DSP_TYP_DEFECTINF - SIZE_DETECT_BASE_DATA - 32 * 4
    ''' <summary>
    ''' �\���r���[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_DEFECTINF, pack:=1)> _
    Public Structure DSP_TYP_DEFECTINF
        Dim data As DETECT_BASE_DATA                        '�r���[TBL_HY_DEFECTINF]
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_s As String                          '�摜�̃p�X[��](�摜��������ALL NULL)(��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_r As String                          '�摜�̃p�X[��](�摜��������ALL NULL)(��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_r2 As String                         '�摜�̃p�X[��2](�摜��������ALL NULL)(��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim image_path_r3 As String                         '�摜�̃p�X[��3](�摜��������ALL NULL)(��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
        <VBFixedArray(SIZE_DSP_TYP_DEFECTINF_RESERVE - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_DSP_TYP_DEFECTINF_RESERVE)> _
        Dim reserve() As Byte                               '�\��
        '�C�j�V�����C�Y
        Public Sub initialize()
            data.initialize()
            ReDim reserve(SIZE_DSP_TYP_DEFECTINF_RESERVE - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�|�C���^[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DEFECTINF_PNT As Integer = SIZE_DSP_TYP_DEFECTINF
    ''' <summary>
    ''' �|�C���^[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DEFECTINF_PNT, pack:=1)> _
    Public Structure DEFECTINF_PNT
        Dim typ As DSP_TYP_DEFECTINF                        '�\���r���[TBL_HY_DEFECTINF]
        '�C�j�V�����C�Y
        Public Sub initialize()
            typ.initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    '�w�b�_�[���[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DEFECTINF_HEADER As Integer = SIZE_KIZUKEN_NO + 4 + 4 + 8
    ''' <summary>
    ''' �w�b�_�[���[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DEFECTINF_HEADER, pack:=1)> _
    Public Structure DEFECTINF_HEADER
        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)> _
        Dim KizukenNo() As Byte                             '�r���Ǘ�No
        <VBFixedArray(SIZE_MEI_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_MEI_NO + 1)> _
        Dim cMeiNo() As Byte                                '����No
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim cyobi() As Byte                                 '�\��
        Dim num As UInteger                                 '���݂̊i�[����
        Dim yobi As UInteger
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim KizukenNo(SIZE_KIZUKEN_NO - 1)
            ReDim cMeiNo(SIZE_MEI_NO - 1)
            ReDim cyobi(3 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '���R�[�h[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_MAP_DEFECTINF As Integer = SIZE_DEFECTINF_HEADER + SIZE_DEFECTINF_PNT * MAX_MAP_DEFECT_NUM
    ''' <summary>
    ''' ���R�[�h[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_MAP_DEFECTINF, pack:=1)> _
    Public Structure DSP_MAP_DEFECTINF
        Dim header As DEFECTINF_HEADER                      '�w�b�_�[���[TBL_HY_DEFECTINF]
        <VBFixedArray(MAX_MAP_DEFECT_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_MAP_DEFECT_NUM)> _
        Dim pnt() As DEFECTINF_PNT                          '�|�C���^[TBL_HY_DEFECTINF]
        '�C�j�V�����C�Y
        Public Sub initialize()
            header.initialize()
            ReDim pnt(MAX_MAP_DEFECT_NUM - 1)
            For ii As Integer = 0 To MAX_MAP_DEFECT_NUM - 1
                pnt(ii).initialize()
            Next
        End Sub
    End Structure

End Module
