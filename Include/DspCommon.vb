'*******************************************************************************
'   �\�����ʒ�`
'
'   [Ver]
'       Ver.01  2015/06/04  ���ō쐬
'
'   [����]
'
'*******************************************************************************
Option Strict On

''' <summary>
''' �\�����ʒ�`
''' </summary>
''' <remarks></remarks>
Public Module DspCommon
	'///////////////////////////////////////////////////////////////////////////
	'�X�C�b�`
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'���W�J�p���C���敪
	'-------------------------------------------------------
	'LINE�X�C�b�`(�K��1�R�̂ݗL���ɂ��鎖)VB�̏ꍇ���W���[�����ɕK�v
#Const cLINE = "LINE_5CGL_TINY"

	'///////////////////////////////////////////////////////////////////////////
	'�萔
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'��ʋ��ʃ^�C�g��
	'-------------------------------------------------------
	Public Const SYSTEM_TITLE As String = "5CGL�ɔ����Ӄ�"

	'-------------------------------------------------------
	'�r�}�b�v�\���F
	'-------------------------------------------------------
	Public g_ColorTanshoNon As Color = Color.DarkGray							'���T��
	Public g_ColorTanshoOk As Color = Color.White								'����T��
	Public g_ColorTanshoGai As Color = Color.FromKnownColor(KnownColor.Control)	'�T���͈͊O
	Public g_ColorJyusinOkure As Color = Color.Pink								'��M�x��
	Public g_ColorOmoKizu As Color = Color.Cyan									'�\�ʂ��r
	Public g_ColorUraKizu As Color = Color.Lime									'���ʂ��r
	Public g_ColorNewKizu As Color = Color.Cyan									'�ŐV���r
	Public g_ColorOldKizu As Color = Color.Lime									'��O���r
	Public g_ColorOverlay As Color = Color.Cyan									'�I�[�o�[���C�F
	Public g_ColorMemori As Color = Color.Lime									'�������F
	'�ꎞ��~�p
	Public g_ColorStop1st As Color = Color.Cyan									'1�ڂ��r
	Public g_ColorStop2nd As Color = Color.Lime									'2�ڂ��r

	Public g_ColorYousetu As Color = Color.Blue									'�n�ڐ�
	Public g_ColorSharStart As Color = Color.Red								'�t�@�[�X�g�J�b�g��
	Public g_ColorSharEnd As Color = Color.Red									'���X�g�J�b�g��
	Public g_ColorShar As Color = Color.Red										'�����J�b�g��
    Public g_ColorKensa As Color = Color.HotPink                                '������
    Public g_ColorIsyuten As Color = Color.Blue                                 '�َ�_
    'Public g_ColorIrigawa1 As Color = Color.FromArgb(255, 255, 255)			'�����R�C���P
    'Public g_ColorIrigawa2 As Color = Color.FromArgb(255, 255, 255)			'�����R�C���Q
    'Public g_ColorIrigawa3 As Color = Color.FromArgb(255, 255, 255)			'�����R�C���R

    '-------------------------------------------------------
    '�d���׈ꗗ�\���F
    '-------------------------------------------------------
    Public g_ColLstOverlay As Color = Color.DarkGreen                           '�I�[�o�[���C�A�ڐ���\���F
    Public g_ColLstKensadaiMae As Color = Color.Cyan                            '������O
    Public g_ColLstNewKizu As Color = Color.Cyan                                '�ŐV�摜�t���[���F
    Public g_ColLstNewSp As Color = Color.BlueViolet                            '�ŐV+�����摜�t���[���F
    Public g_ColLstFix As Color = Color.Orange                                  '�Œ�摜�t���[���F(���b�N)
    Public g_ColLstSp As Color = Color.Red                                      '�����g�\���t���[���F

    '-------------------------------------------------------
    '���׃O���[�h�\���F
    '-------------------------------------------------------
    Public g_ColorGrNon As Color = Color.White                                  '���Q
    Public g_ColorGrKei As Color = Color.Orange									'�y����
	Public g_ColorGrTyu As Color = Color.Magenta								'������
	Public g_ColorGrJyu As Color = Color.Red									'�d����

	'// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
	'-------------------------------------------------------
	'�r���֘A
	'-------------------------------------------------------
	Public g_ColorTypeFMerge As Color = Color.Red                           ' �t���[���ԘA���r
	'// 2023.09.14 �摜�������u H�ES�\�t�g���� <<<<<--------------------

	'-------------------------------------------------------
	'RAS��ʃp�X���[�h
	'-------------------------------------------------------
	Public Const PASSWORD As String = "KIZU"

    Public g_ColorSimpleCycle As Color = Color.Yellow                           ' �ȈՎ����r
    Public g_ColorKizuCycle As Color = Color.Red                                ' �����r

    '-------------------------------------------------------
    '�T����ʗp
    '-------------------------------------------------------
    Public Const DSP_COIL_MAX As Integer = 5                                    '�T����ʕ\���R�C��MAX(���݃R�C���{�ߋ�4�R�C���\���\)
    Public Const DSP_IMAGE_MAX As Integer = 2									'�r�摜�\����(1�r������摜��2��)
	Public Const DSP_DEFECT_MAX As Integer = 2                                  '�r���\����[���A���X]
    Public Const DSP_KIRI_IMAGE_MAX As Integer = 2                              '�g��摜�\���\�r��
    Public Const DSP_TRACKING_POS As Integer = 2                                '�g���b�L���O�\���J�n�����ʒu(����͑S�ă��A���^�C���\��)

    Public Const TANSHO_MAP_DSP_ROW_MAX As Integer = 39                         '�T����ʃO���b�h�ő�s��
    '   Public Const TANSHO_SCALE_INTVAL_X As Integer = 30							'�r�摜�̃X�P�[����X���������Ԋu[mm]
    'Public Const TANSHO_SCALE_INTVAL_Y As Integer = 30                          '�r�摜�̃X�P�[����Y���������Ԋu[mm]
    Public Const TANSHO_SCALE_INTVAL_X As Integer = 10                          '�r�摜�̃X�P�[����X���������Ԋu[mm]
    Public Const TANSHO_SCALE_INTVAL_Y As Integer = 10                          '�r�摜�̃X�P�[����Y���������Ԋu[mm]
    Public Const TANSHO_SCALE_NUM_X As Integer = 2                              '�r�摜�̃X�P�[����X����������
    Public Const TANSHO_SCALE_NUM_Y As Integer = 2                              '�r�摜�̃X�P�[����Y����������

    '-------------------------------------------------------
    '���щ�ʗp
    '-------------------------------------------------------
    Public Const JISSEKI_MAP_DSP_ROW_MAX As Integer = 41                        '���щ�ʃO���b�h�ő�s��
    '   Public Const JISSEKI_SCALE_INTVAL_X As Integer = 30							'�r�摜�̃X�P�[����X���������Ԋu[mm]
    'Public Const JISSEKI_SCALE_INTVAL_Y As Integer = 30                         '�r�摜�̃X�P�[����Y���������Ԋu[mm]
    'Public Const JISSEKI_SCALE_NUM_X As Integer = 2                             '�r�摜�̃X�P�[����X����������
    'Public Const JISSEKI_SCALE_NUM_Y As Integer = 2                             '�r�摜�̃X�P�[����Y����������
    Public Const JISSEKI_SCALE_INTVAL_X As Integer = 10                         '�r�摜�̃X�P�[����X���������Ԋu[mm]
    Public Const JISSEKI_SCALE_INTVAL_Y As Integer = 10                         '�r�摜�̃X�P�[����Y���������Ԋu[mm]
    Public Const JISSEKI_SCALE_NUM_X As Integer = 3                             '�r�摜�̃X�P�[����X����������
    Public Const JISSEKI_SCALE_NUM_Y As Integer = 3                             '�r�摜�̃X�P�[����Y����������
    Public Const JISSEKI_MAP_START_POS As Integer = 1                           '�}�b�v�����A�o���J�n�ʒu0:0�`[0,5,10���]�A1:�ŏ��`[5,10,15���]
    Public Const JISSEKI_DSP_KIRI_IMAGE_MAX As Integer = 4                      '�����r�g��摜�\����

    '-------------------------------------------------------
    '�T���E���щ�ʋ���
    '-------------------------------------------------------
    Public Const KZCNT_CNT_MAX As Integer = 99                                  '�r���\�� �ő�l

    '-------------------------------------------------------
    '�T���E���щ�ʋ���(���T�C�Y)
    '-------------------------------------------------------
    Public Const WIN_WIDTH_MAX As Integer = 1920                                '��ʍő啝
    Public Const WIN_HEIGHT_MAX As Integer = 1080                               '��ʍő卂��
    Public Const WIN_WIDTH_MIN As Integer = 800                                 '��ʍŏ���
    Public Const WIN_HEIGHT_MIN As Integer = 600                                '��ʍŏ�����

    '//=======================================
    '// �T����ʁE���щ�� �摜���́@
    '//=======================================
    '�S�p1���� or ���p�Q����
    Public Const DIRECTION_NAME_DS As String = "DS"
    Public Const DIRECTION_NAME_WS As String = "WS"

    '// �摜����
    Public DEFECT_IMG_NAME() As String = {"������(R)", "������(G)", "������(B)", "������(G-B)"}

    '-------------------------------------------------------
    '�p�����[�^�ݒ��ʁA�r�ꗗ�\����ʋ���(�V�X�e�����ɕύX)
    '-------------------------------------------------------
    Public Const MAP_ROW_HIGHT As Integer = 19
    Public Const HED_ROW_HIGHT As Integer = 16

    '�ݒ�ؑ֒萔
    Public Const MC_SHOUSAI As Integer = 0										'�ڍאݒ�
	Public Const MC_KANI As Integer = 1                                         '�ȈՐݒ�

    '�ȈՐݒ莞�̍��ڐ�
    Public Const MC_KIZUSHU_NUM As Integer = 8                                  '�r���5�܂�
    Public Const MC_GRADE_NUM As Integer = 1									'�O���[�h��1�܂�

	'///////////////////////////////////////////////////////////////////////////
	'�\����
	'///////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ���������z
	''' </summary>
	Public Enum EM_DIST_WIDTH
		WS = 0								' WS�ʒu�Ɍ��חL��
		C									' C�ʒu�Ɍ��חL��
		DS									' DS�ʒu�Ɍ��חL��
	End Enum

	''' <summary>
	''' ���׃t���O
	''' </summary>
	Public Enum EM_DEFECT_FLG
		KEI = 0								' �y����
		JYU									' �d����
	End Enum

	'///////////////////////////////////////////////////////////////////////////
	'�񋓑�
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �r�}�b�v��
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_MAP_COL_INF
		DEGAWA = 0							'�o������
		COL_1								'��1
		COL_2								'��2
		COL_3								'��3
		COL_4								'��4
		COL_5								'��5
		COL_6								'��6
		COL_7								'��7
		COL_8								'��8
		IRIGAWA                             '��������
        KIZUNUM                             '�r��
        OTH1                                '���̑��P
        OTH2                                '���̑��Q
    End Enum

    ''' <summary>
    ''' �T����ʓ������[�h
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_SYNC_MODE
		ALL = 0								'�S��ʓ���
		BY_DSP								'������ʒu���ɓ���
		NONE								'�������Ȃ�
	End Enum

	''' <summary>
	''' �T������r�\���X�V���[�h
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DEFECT_MODE
		TRACKING = 0						'�g���b�L���O�\��
		REALTIME							'���A���^�C���\��
	End Enum

	''' <summary>
	''' �T����ʍX�V���[�h
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_REFRESH_MODE
		AUTO = 0							'�����X�V���[�h
		PAUSE								'�ꎞ��~���[�h
	End Enum

	'''<summary>
	'''����Ԋu
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_LONG_POS
		MOSTSMALL = 0						'�ŏ�
		SMALL								'����Ԋu��
		BIG									'����Ԋu��
	End Enum

	'''<summary>
	'''�r�}�b�v�\���ݒ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_SIDE
		NORMAL = 0							'����DS
		REVERSE								'����WS
	End Enum

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>
	'''<summary>
	'''�r�}�b�v�\���ݒ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_WIDTH_NO
		NORMAL = 0                          '������1,2,�E�E�E
		REVERSE                             '�E����1,2,�E�E�E
	End Enum
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------

	'''<summary>
	'''�r�}�b�v�Z���ݒ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_CELL
		NORMAL = 0							'���E���]����
		REVERSE								'���E���]�L��
	End Enum

	'''<summary>
	'''�r�}�b�v�i�s�����ݒ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_MAP_SCROOL
		TAIL_TO_HEAD = 0					'��������
		HEAD_TO_TAIL						'�ォ�牺��
	End Enum

	'''<summary>
	'''�摜�㉺���]�ݒ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_TOP
		NORMAL = 0							'�㉺���]�L��
		REVERSE								'�㉺���]����
	End Enum

	'''<summary>
	'''�摜���E���]�ݒ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_SIDE
		NORMAL = 0							'���E���]�L��
		REVERSE								'���E���]����
	End Enum
	'''<summary>
	'''�摜90�x��]
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_90
		NORMAL = 0							'��]�Ȃ�
		LOTATE								'90�x��]
	End Enum

	'''<summary>
	'''�摜�ʒu
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_POS
		IMG1 = 0							'��
		IMG2 = 1							'��
	End Enum

	'''<summary>
	'''�摜�ԍ�
	'''</summary>
	'''<remarks></remarks>
	Public Enum EM_IMAGE_NO
		S = 1								'������
		R1									'�����˂P
		R2									'�����˂Q
		R3									'�����˂R(�F��)
	End Enum

	''' <summary>
	''' �N������
	''' </summary>
	''' <remarks></remarks>
	Public Structure START_ARGS
		Dim nFormPos As Integer         ' ��ʈʒu�ԍ� (FORM_POS:=)

		Dim sArgs() As String           ' ���������H
	End Structure

	Public Const START_FORM_POS As String = "FORM_POS:="

	'''<summary>
	'''�O�H���r�����\������
	'''</summary>
	'''<remarks>�V�K���グ���͖��g�p</remarks>
	Public Enum EM_MAEIMAGE_AUTOPOP
		DISABLE = 0							'�����\���Ȃ�
		ENABLE								'�����\������
	End Enum

    ''-------------------------------------------------------
    ''�@����(RAS��ʂŎg�p)
    ''�V�X�e���⃉�C���ɂ���ĕύX���邱�� 
    ''-------------------------------------------------------
    '���Ə�ԕ\����(STATUS_MASTER �Ώ�IDNo)
    Public Const DIV_KADOU_ID As Integer = 663              '�ғ����
    Public Const DIV_UNTEN_ID As Integer = 664              '�^�]���[�h
    Public Const DIV_SOGYO_ID As Integer = 665              '���Ə��
    Public Const DIV_PLGMODE_ID As Integer = 666            'PLG�敪

    '��������(STATUS_MASTER �Ώ�IDNo)
    Public Const DIV_SECURITY_DOOR_ID As Integer = 705      '�����
    Public Const DIV_SECURITY_KEY_ID As Integer = 706       '�����

End Module
