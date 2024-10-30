'*******************************************************************************
'   �Ӄ֑I��萔��`
'
'   [Ver]
'       Ver.01  2016/06/09  ����
'
'   [����]
'       ��{�I�ɏ���������(�؂�ւ��X�C�b�`�p�̂�)
'*******************************************************************************
Option Strict On

''' <summary>
''' �Ӄ֑I��萔��`
''' </summary>
''' <remarks></remarks>
Public Module KizuCommonSelect
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
    'LINE�X�C�b�`(�K��1�R�̂ݗL���ɂ��鎖)VB�̏ꍇ���W���[�����ɕK�v
#Const cLINE = "LINE_5CGL_TINY"

    '-------------------------------------------------------
    '�Ӄ֐ݔ��֌W
    '-------------------------------------------------------
    '�J�����ݒu�ʒu �������ʒu
    '#Const cNumCamPos = "NUM_CAM_POS_1"                         '�ʓ���̃J�����ʒu��(C)
    '#Const cNumCamPos = "NUM_CAM_POS_2"                         '�ʓ���̃J�����ʒu��(DS,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_3"                         '�ʓ���̃J�����ʒu��(DS,C,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_4"                         '�ʓ���̃J�����ʒu��(DS,D1,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_5"                         '�ʓ���̃J�����ʒu��(DS,D1,C,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_6"                         '�ʓ���̃J�����ʒu��(DS,D1,D2,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_7"                         '�ʓ���̃J�����ʒu��(DS,D1,D2,C,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_8"                         '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_9"                         '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,C,W3,W2,W1,WS)
#Const cNumCamPos = "NUM_CAM_POS_10"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_11"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,C,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_12"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_13"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,C,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_14"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,D6,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_15"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,D6,C,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_16"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,D6,D7,W7,W6,W5,W4,W3,W2,W1,WS)

    '�J�����ݒu �p�x
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_1"                     '�J�����p�x��(1=�P��)
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_2"                     '�J�����p�x��(2=���E��)
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_3"                     '�J�����p�x��(3=���E���E��)
#Const cNumCamAngle = "NUM_CAM_ANGLE_4"                     '�J�����p�x��(4=���E���E���E��)
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_5"                     '�J�����p�x��(5=���E���E���E���E��)
    '///////////////////////////////////////////////////////////////////////////
    '�萔
    '///////////////////////////////////////////////////////////////////////////
    '����敪
#If cLOCAL = False Then                                     '�{�Ԋ�
    Public Const LOCAL As Boolean = False                                       '����敪(�{��)
#Else                                                       '�e�X�g��
    Public Const LOCAL As Boolean = True                                        '����敪(�e�X�g)
#End If

    '-------------------------------------------------------
    '�J�����ݒu�ʒu �������ʒu
    '-------------------------------------------------------
    Public Const NUM_CAM_POS_1 As Integer = 1
    Public Const NUM_CAM_POS_2 As Integer = 2
    Public Const NUM_CAM_POS_3 As Integer = 3
    Public Const NUM_CAM_POS_4 As Integer = 4
    Public Const NUM_CAM_POS_5 As Integer = 5
    Public Const NUM_CAM_POS_6 As Integer = 6
    Public Const NUM_CAM_POS_7 As Integer = 7
    Public Const NUM_CAM_POS_8 As Integer = 8
    Public Const NUM_CAM_POS_9 As Integer = 9
    Public Const NUM_CAM_POS_10 As Integer = 10
    Public Const NUM_CAM_POS_11 As Integer = 11
    Public Const NUM_CAM_POS_12 As Integer = 12
    Public Const NUM_CAM_POS_13 As Integer = 13
    Public Const NUM_CAM_POS_14 As Integer = 14
    Public Const NUM_CAM_POS_15 As Integer = 15
    Public Const NUM_CAM_POS_16 As Integer = 16

#If cNumCamPos = "NUM_CAM_POS_1" Then
	Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_1
#ElseIf cNumCamPos = "NUM_CAM_POS_2" Then
	Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_2
#ElseIf cNumCamPos = "NUM_CAM_POS_3" Then
	Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_3
#ElseIf cNumCamPos = "NUM_CAM_POS_4" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_4
#ElseIf cNumCamPos = "NUM_CAM_POS_5" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_5
#ElseIf cNumCamPos = "NUM_CAM_POS_6" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_6
#ElseIf cNumCamPos = "NUM_CAM_POS_7" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_7
#ElseIf cNumCamPos = "NUM_CAM_POS_8" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_8
#ElseIf cNumCamPos = "NUM_CAM_POS_9" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_9
#ElseIf cNumCamPos = "NUM_CAM_POS_10" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_10
#ElseIf cNumCamPos = "NUM_CAM_POS_11" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_11
#ElseIf cNumCamPos = "NUM_CAM_POS_12" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_12
#ElseIf cNumCamPos = "NUM_CAM_POS_13" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_13
#ElseIf cNumCamPos = "NUM_CAM_POS_14" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_14
#ElseIf cNumCamPos = "NUM_CAM_POS_15" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_15
#ElseIf cNumCamPos = "NUM_CAM_POS_16" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_16
#End If

    '-------------------------------------------------------
    '�J�����ݒu �p�x
    '-------------------------------------------------------
    Public Const NUM_CAM_ANGLE_1 As Integer = 1
    Public Const NUM_CAM_ANGLE_2 As Integer = 2
    Public Const NUM_CAM_ANGLE_3 As Integer = 3
    Public Const NUM_CAM_ANGLE_4 As Integer = 4
    Public Const NUM_CAM_ANGLE_5 As Integer = 5

#If cNumCamAngle = "NUM_CAM_ANGLE_1" Then
	Public Const NUM_CAM_ANGLE as Integer =	NUM_CAM_ANGLE_1
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_2" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_2
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_3" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_3
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_4" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_4
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_5" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_5
#End If

    '-------------------------------------------------------
    '���W�J�pID
    '-------------------------------------------------------
    Public Const LINE_ID_N5CGL_TINY As Integer = 0

    '�����C��ID(VB�ł́A�ɗ�ini�t�@�C���̃��C��ID���g�p���鎖)
    Public Const LINE_ID As Integer = LINE_ID_N5CGL_TINY

    '-------------------------------------------------------
    '�Ǘ�No�̐擪��2���R�[�h
    '-------------------------------------------------------
    Public Const KIZUKEN_CODE As String = KIZUKEN_CODE_N5CGL_TINY

    '-------------------------------------------------------
    '�H���R�[�h
    '-------------------------------------------------------
    Public Const KOUTEI_CODE As String = KOUTEI_CODE_5CGL

	'// �V�X�e�����i�F�ؗp�j
    ' INI�t�@�C���Ń��C���X�C�b�`�����������Ȃ������̂ŁAKizuCommonSelect����X�C�b�`������

    Public Const ENABLE_USERCHECK As Boolean = True    ' false���A���[�U�F�ؖ���(�ŏI�I�ɂ͕K��OFF)


    Public Const SYSTEM_NAME As String = "N5CGL_TINY"

End Module
