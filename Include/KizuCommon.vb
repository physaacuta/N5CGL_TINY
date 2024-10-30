'*******************************************************************************
'   �Ӄ֋��ʒ�`
'
'   [Ver]
'       Ver.01  2013/12/11  ����
'
'   [����]
'
'*******************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

''' <summary>
''' �Ӄ֋��ʒ�`
''' </summary>
''' <remarks></remarks>
Public Module KizuCommon
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
    '#Const cNumCamPos = "NUM_CAM_POS_10"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_11"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,C,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_12"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_13"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,C,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_14"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,D6,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_15"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,D6,C,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_16"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,D5,D6,D7,W7,W6,W5,W4,W3,W2,W1,WS)

#Const cNumCamPos = "NUM_CAM_POS_10"                        '�ʓ���̃J�����ʒu��(DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)

    '///////////////////////////////////////////////////////////////////////////
    '�萔
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '�ݒ�l[ini�t�@�C��]
    '-------------------------------------------------------
    '�r���ݒ�t�@�C��(�@�\�P�ʂ̐ݒ�t�@�C��)
#If cLOCAL = False Then                                     '�{�Ԋ�
    Public Const TASKINI_NAME As String = ".\KizuTask.ini"                      '�r���^�X�N�ݒ�t�@�C��
#Else                                                       '�e�X�g��
	Public Const TASKINI_NAME As String = ".\KizuTask.ini"	                    '�r���^�X�N�ݒ�t�@�C��
#End If

    '�r���ݒ�t�@�C��(PC�P�ʂ̐ݒ�t�@�C��)
#If cLOCAL = False Then                                     '�{�Ԋ�
    Public Const PCINI_NAME As String = ".\KizuPc.ini"                          'PC�ݒ�t�@�C��
#Else                                                       '�e�X�g��
	Public Const PCINI_NAME As String = ".\KizuPc.ini"			                'PC�ݒ�t�@�C��
#End If

    '�r���ݒ�t�@�C��(PC�̃��[�J���P�ʂ̐ݒ�t�@�C��)
    '�����ini�t�@�C���Ȃ̂ŁA�ȉ��̎��ɒ���
    '�E�X�̃p�\�R���̈ꎞ�I�Ȑݒ����ۑ����Ă����t�@�C��
    '�E���O�I�t�����Ă��㏑������Ȃ�
    '�E��ini�t�@�C���������Ă��A�K���^�X�N���͓��삷��(�f�t�H���g�l�Ȃǂ�)�悤�ɁA����Ă�����
#If cLOCAL = False Then                                     '�{�Ԋ�
    Public Const LOCALINI_NAME As String = ".\KizuLocal.ini"                    'Local�ݒ�t�@�C��
#Else                                                       '�e�X�g��
	Public Const LOCALINI_NAME As String = ".\KizuLocal.ini"	                'Local�ݒ�t�@�C��
#End If

    '���ʃZ�b�V����
    Public Const INI_COMMON As String = "COMMON"                                '���ʐݒ� �p�Z�b�V����
    Public Const INI_DB As String = "DB"                                        'DB�ڑ� �p�Z�b�V����
    Public Const INI_DB_A As String = "DB_A"                                    'DB�ڑ� �p�Z�b�V����(A�n)
    Public Const INI_DB_B As String = "DB_B"                                    'DB�ڑ� �p�Z�b�V����(B�n)
    Public Const INI_STATUS As String = "STATUS"                                '�@��ID �p�Z�b�V����
    Public Const INI_MEM As String = "MEM"                                      '���L������ �p�Z�b�V����
    Public Const INI_EXE As String = "EXECUTE"                                  '�v���Z�X �p�Z�b�V����

    '-------------------------------------------------------
    '�Œ�T�C�Y
    '-------------------------------------------------------
    Public Const SIZE_MSL As Integer = 256                                      'send_mail,recv_mail�T�C�Y
    Public Const SIZE_MSL_DATA As Integer = SIZE_MSL - 8                        'nEventNo�ȍ~�̃f�[�^��
    Public Const SIZE_NAME_PC As Integer = 16                                   '�p�\�R������
    Public Const SIZE_NAME_TASK As Integer = 16                                 '�^�X�N����
    Public Const SIZE_IMAGE_PATH As Integer = 32                                '�摜�ۑ��p�̃p�X��
    Public Const SIZE_KIZUKEN_NO As Integer = 20                                '�Ǘ�No
    Public Const SIZE_SERIAL_NO As Integer = 16                                 '��ӂȊǗ�No([yyyymmdd hhnnss\0]�̌`��)
    Public Const SIZE_SERIAL_FF_NO As Integer = 20                              '��ӂȊǗ�No([yyyymmdd hhnnssff\0]�̌`��)

    '-------------------------------------------------------
    'EPC�֘A�̃}�X�^�[
    '-------------------------------------------------------
    Public Const MAX_HT1_CAMSET As Integer = 16                                 '����PC1��̍ő�J�����Z�b�g(EPC��8�Z�b�g�܂ł����Ή����Ă��Ȃ�)
    Public Const MAX_HT1_CAMANGLE As Integer = 4                                '����PC1��̍ő�J�����p�x (EPC��4�p�x�܂ł����Ή����Ă��Ȃ�)
    Public Const MAX_ATTR_BASE As Integer = 256                                 '�ő�����ʐ�(��{������)
    'Public Const MAX_ATTR_ANGLE As Integer = 128                                '�ő�����ʐ�(�p�x�ʓ�����) �� ����
    Public Const MAX_ATTR_ANGLE As Integer = 256                                '�ő�����ʐ�(�p�x�ʓ�����) �� �F��

    Public Const MAX_TAJUUTHRESHOLD As Integer = 30                             '���d臒l��� MAX
    Public Const MAX_REDUCTION As Integer = 30                                  '�k������� MAX
    Public Const MAX_FILTER As Integer = 30                                     '�t�B���^��� MAX
    Public Const MAX_CYCLETHRESHOLD As Integer = 30                             '�����p臒l��� MAX
    Public Const MAX_CROP_SHRINK As Integer = 4                                 '�Ԉ����� MAX

    '-------------------------------------------------------
    'DB�}�X�^�[����
    '-------------------------------------------------------
    Public Const MAX_STATUS As Integer = 1024                                   '�ő� �@���ԃ}�X�^�[��
    Public Const MAX_TYPE As Integer = 256                                      '�r����MAX
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
	'Public Const MAX_GRADE As Integer = 8                                       '�O���[�h���MAX
	Public Const MAX_GRADE As Integer = 32                                       '�O���[�h���MAX
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
    Public Const MAX_CAMSET As Integer = NUM_CAM_POS * 2                        '�J�����Z�b�g��MAX
    Public Const MAX_CAMANGLE As Integer = NUM_CAM_ANGLE                        '�J�����p�x��MAX(�_���I�Ȋp�x)
    'Public Const MAX_PHYSICAL_CAMANGLE As Integer = NUM_CAM_ANGLE               '�J�����p�x��MAX (�����I�Ȋp�x)		�� ����
    Public Const MAX_PHYSICAL_CAMANGLE As Integer = 1                           '�J�����p�x��MAX (�����I�Ȋp�x)		�� �F��
    Public Const MAX_SCODE As Integer = 64                                      '�����\�ʏ��MAX(�f�t�H���g����1�܂�)
    Public Const MAX_PCODE As Integer = 64                                      '�����p�^�[��MAX(�f�t�H���g����1�܂�)
    Public Const MAX_CCODE As Integer = 64                                      '�J�E���g�p�^�[��MAX(�f�t�H���g����1�܂�)
    Public Const MAX_CODE_JUDGE As Integer = 32                                 '�����\�ʏ�ԁE�����p�^�[���̏���MAX

    '-------------------------------------------------------
    'NOA��`
    '-------------------------------------------------------
    Public Const NOA_ONLINE As Integer = 0                                      '�I�����C��
    Public Const NOA_OFFLINE As Integer = 1                                     '�I�t���C��
    Public Const NOA_DEBUG As Integer = 9                                       '�f�o�b�O�p (�g���₷���p�ɑS������)
    Public Const NOA_KAISEKI As Integer = -1                                    '��͗p

    '-------------------------------------------------------
    'PC/�^�X�N�ݒ�
    '-------------------------------------------------------
    '20171222 KizuLibCLI.KizuMod.MasterPcName���g�p���邱��
    '       ��frmMain_Ras�AfrmMain_Ras2�Ŏg�p���Ă���ׁA�c����
    '         MASTER_PC_NAME�ASYSLOG_PC_NAME�͎g�p�֎~�Ƃ���B
#If cLOCAL = False Then                                     '�{�Ԋ�
    Public Const MASTER_PC_NAME As String = "TOKATU"                            '�r����ԊǗ������PC [KS_MASTER�̋N��PC���̂��w��]("."�w��̓_��)
    Public Const SYSLOG_PC_NAME As String = "TOKATU"                            'syslog�����PC��
#Else                                                       '�e�X�g��
	Public Const MASTER_PC_NAME as String =	"TOKATU"				            '�r����ԊǗ������PC [KS_MASTER�̋N��PC���̂��w��]("."�w��̓_��)
	Public Const SYSLOG_PC_NAME as String =	"TOKATU"				            'syslog�����PC��
#End If

    '-------------------------------------------------------
    '�p�����[�^�ύX�敪(�r�����ʕ���)
    '   �e���C���ŗL��ʂ̏ꍇ�́ALineCommon�ɋL�q���邱��
    '-------------------------------------------------------
    Public Const CH_SCODE_NON As Integer = 0                                    '�����\�ʏ�ԑΏۊO
    Public Const CH_SCODE_ALL As Integer = 999                                  '�����\�ʏ�ԑS���ύX�敪

    '���삳�񃍃W�b�N���A�S�p�����[�^�ꊇ�Ǎ��݂ƂȂ����ׁA��ʒP�ʂł̒ʒm�ɕύX
    Public Const CH_PARAM_ALL As Integer = 0                                    '�S�e�[�u��(�f�o�b�N�p)
    Public Const CH_PARAM_TYPE_GRADE As Integer = 1                             '�r��EGr

    Public Const CH_PARAM_ATTR_NAME As Integer = 5                              '�����ʖ���
    Public Const CH_PARAM_PCINFODESC As Integer = 6                             '��ʏ��

    Public Const CH_PARAM_COMMON As Integer = 10                                '�Œ�p�����[�^
    Public Const CH_PARAM_CAMERA As Integer = 11                                '�J�����ݒ�
    Public Const CH_PARAM_EDGE As Integer = 12                                  '�G�b�W���o
    Public Const CH_PARAM_SHD As Integer = 13                                   '�V�F�[�f�B���O�␳
    Public Const CH_PARAM_THRESHOLD As Integer = 14                             '臒l�ݒ�
    Public Const CH_PARAM_DETECT As Integer = 15                                '�r���o
    Public Const CH_PARAM_CYCLE As Integer = 16                                 '�����r����
    Public Const CH_PARAM_MERGE As Integer = 17                                 '�J�������E����
    Public Const CH_PARAM_LIGHT As Integer = 18                                 '�Ɩ�
    Public Const CH_PARAM_TINY As Integer = 19                                  '�������אݒ�

    Public Const CH_PARAM_SYNCADD As Integer = 20                               '�摜���Z
    Public Const CH_PARAM_COLOR_DIV As Integer = 21                             '���F����
    'Public Const CH_PARAM_YOUSET As Integer = 22                                '�n�ړ_���o

    Public Const CH_PARAM_SETTIG_SCODE As Integer = 110                         '�����\�ʏ�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
    Public Const CH_PARAM_SETTIG_PCODE As Integer = 111                         '�����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
    Public Const CH_PARAM_SETTIG_COMMON As Integer = 112                        '�����萔�ݒ�
    Public Const CH_PARAM_SETTIG_PCOMMON As Integer = 113                       '�����p�^�[�� �ŗL �o�͐ݒ�
    Public Const CH_PARAM_SETTIG_PALL As Integer = 114                          '�����p�^�[�� ���� �o�͐ݒ�
    Public Const CH_PARAM_SETTIG_LOG As Integer = 115                           '�V�X���O�e�[�u��
    Public Const CH_PARAM_SETTIG_STATUS As Integer = 116                        '�@���ԃe�[�u��
    Public Const CH_PARAM_CAMERA_LINKSWITCH As Integer = 117                    '�J���������N�X�C�b�`�ݒ�
    Public Const CH_PARAM_DEMAND As Integer = 118                               '���v�Ɛݒ�

    '-------------------------------------------------------
    '�v���Z�X���̈ꗗ
    '-------------------------------------------------------
    '�x���@�\
    Public Const KS_MASTER As String = "KS_MASTER"                              'PC�}�X�^�[�Ǘ�
    Public Const KS_SLAVE As String = "KS_SLAVE"                                'PC�X���[�u�Ǘ�
    Public Const KS_START As String = "KS_START"                                '�X�^�[�g�A�b�v�Ǘ�
    Public Const KS_WAKEUP As String = "KS_WAKEUP"                              '�C�j�V�������ҋ@���
    Public Const KS_EVTLOG As String = "KS_EVTLOG"                              '�C�x���g�r���[�A�[���Ǘ�
    Public Const KS_SYSLOG As String = "KS_SYSLOG"                              'SYSLOG�o�^�^�X�N
    Public Const KS_UPSMGR As String = "KS_UPSMGR"                              'UPS�ُ�I���^�X�N
    Public Const KS_BACKUP As String = "KS_BACKUP"                              '�o�b�N�A�b�v�ؑ֎x���^�X�N
    Public Const KS_EQALDB As String = "KS_EQALDB"                              'DB�����Ǘ��^�X�N

    Public Const KS_HDDCHK As String = "KS_HDDCHK"                              'HDD��ԊĎ��^�X�N
    Public Const KS_HISTRY As String = "KS_HISTRY"                              '�@���ԗ����Ǘ��^�X�N
    Public Const KS_APCHECK As String = "KS_APCHECK"                            'AP��ԊĎ��^�X�N
    Public Const KS_MAILTR As String = "KS_MAILTR"                              '���[���X���b�g�]���^�X�N

    '���ʃ^�X�N
    '�����ɖ���`��KT�^�X�N�́ALineCommon.h�ɋN���v������`���Ă���̂�����̂Œ���
    Public Const KT_CHILER As String = "KT_CHILER"                              '�΃`���[�ʐM
    Public Const KT_LEDMGR As String = "KT_LEDMGR"                              '��LED�Ɩ��ʐM
    Public Const KT_TEMPMT As String = "KT_TEMPMT"                              '�Ή��x�p�l�����[�^
    Public Const KT_RPICHK As String = "KT_RPICHK"                              'RPI�t�@�C���Ǘ�

    '�~�ϋ@�\
    Public Const TI_DELETE As String = "TI_DELETE"                              'DB�폜�Ǘ��^�X�N	

    '-------------------------------------------------------
    '�f�[�^�x�[�X
    '-------------------------------------------------------
    '�}�X�^�[
    Public Const DB_STATUS_MASTER As String = "M_STATUS_MASTER"                     '[DB]�@��}�X�^�[�e�[�u��
    Public Const DB_STATUS_INF As String = "M_STATUS_INF"                           '[DB]�@���ԃe�[�u��
    Public Const DB_LOG_MASTER As String = "M_LOG_MASTER"                           '[DB]���O�}�X�^�[�e�[�u��
    Public Const DB_LOG_INF As String = "T_LOG_INF"                                 '[DB]���O���e�[�u��
    Public Const DB_PC_MASTER As String = "M_PC_MASTER"                             '[DB]PC�}�X�^�[
    Public Const DB_PC_INF As String = "M_PC_INF"                                   '[DB]PC���
    Public Const DB_ST_HISTORY_MASTER As String = "M_ST_HISTORY_MASTER"             '[DB]�@�헚���}�X�^�[�e�[�u��
    Public Const DB_ST_HISTORY_INF As String = "T_ST_HISTORY_INF"                   '[DB]�@�헚���e�[�u��
    Public Const DB_STATUS_LOG As String = "T_STATUS_LOG"                           '[DB]�@���ԃ��O�e�[�u��
    Public Const DB_BASE_ACTION_MASTER As String = "M_BASE_ACTION_MASTER"           '[DB]��{����ꗗ�ݒ�

    Public Const DB_COMMON_MASTER As String = "M_COMMON_MASTER"                     '[DB]�����萔�}�X�^�[
    Public Const DB_TYPE_NAME As String = "TYPE_NAME"                               '[DB]�r��}�X�^�[
    Public Const DB_GRADE_NAME As String = "GRADE_NAME"                             '[DB]�O���[�h�}�X�^�[
    Public Const DB_SCODE_MASTER As String = "M_SCODE_MASTER"                       '[DB]�����\�ʏ�ԃ}�X�^�[
    Public Const DB_SCODE_JUDGE As String = "M_SCODE_JUDGE"                         '[DB]�����\�ʏ�ԏ����ݒ�
    Public Const DB_SCODE_OUTPUT As String = "M_SCODE_OUTPUT"                       '[DB]�����\�ʏ�ԕʏo�͐ݒ�
    Public Const DB_PCODE_MASTER As String = "M_PCODE_MASTER"                       '[DB]�����p�^�[���}�X�^�[
    Public Const DB_PCODE_JUDGE As String = "M_PCODE_JUDGE"                         '[DB]�����p�^�[�������ݒ�
    Public Const DB_PCODE_OUTPUT As String = "M_PCODE_OUTPUT"                       '[DB]�����p�^�[���ʏo�͐ݒ�
    Public Const DB_PALL_OUTPUT As String = "M_PALL_OUTPUT"                         '[DB]�����p�^�[�����ʏo�͐ݒ�
    Public Const DB_PCOMMON_OUTPUT As String = "M_PCOMMON_OUTPUT"                   '[DB]�����p�^�[���ŗL�o�͐ݒ�

    Public Const DB_CAMERA_LINKSWITCH As String = "M_CAMERA_LINKSWITCH"             '[DB]�J���������N�X�C�b�`�ݒ�
    Public Const DB_PAR_GRID_SETTING As String = "M_PAR_GRID_SETTING"               '[DB]�f�[�^�O���b�h�\���ݒ�
    Public Const DB_PAR_LOV_ITEM As String = "M_PAR_LOV_ITEM"                       '[DB]��ʑI�����ڃA�C�e���}�X�^


    '�p�����[�^
    Public Const DB_ATTR_NAME As String = "ATTR_NAME"                               '[DB]�����ʖ��̃}�X�^�[
    Public Const DB_PARAM_COMMON As String = "PARAM_COMMON"                         '[DB]���ʃp�����[�^
    Public Const DB_PARAM_SCODE As String = "PARAM_SCODE"                           '[DB]�\�ʏ�ԕʃp�����[�^
    Public Const DB_PARAM_CAMSET As String = "PARAM_CAMSET"                         '[DB]�J�����Z�b�g�ʃp�����[�^
    Public Const DB_PARAM_CAMSET_SCODE As String = "PARAM_CAMSET_SCODE"             '[DB]�J�����Z�b�g�ʁ~�\�ʏ�ԕʃp�����[�^
    Public Const DB_PARAM_CAMSET_ANGLE As String = "PARAM_CAMSET_ANGLE"             '[DB]�J�����Z�b�g�ʁ~�J�����p�x�ʃp�����[�^
    Public Const DB_PARAM_CAMANGLE_SCODE As String = "PARAM_CAMANGLE_SCODE"         '[DB]�J�����p�x�ʁ~�\�ʏ�ԕʃp�����[�^
    Public Const DB_PARAM_CAMSET_ANGLE_SCODE As String = "PARAM_CAMSET_ANGLE_SCODE" '[DB]�J�����Z�b�g�ʁ~�J�����p�x�ʁ~�\�ʏ�ԕʃp�����[�^
    Public Const DB_ROLL_INFO As String = "ROLL_INFO"                               '[DB]���[�����
    Public Const DB_PARAM_PCINFO_DESC As String = "PCINFO_DESC"                     '[DB]��ʏ����e

    Public Const DB_PARAM_REDUCTION As String = "PARAM_REDUCTION"                   '[DB]�k����
    Public Const DB_PARAM_FILTER As String = "PARAM_FILTER"                         '[DB]�t�B���^ 
    Public Const DB_PARAM_THRESHOLD As String = "PARAM_THRESHOLD"                   '[DB]���d�p臒l
    'Public Const DB_PARAM_CYCLE_THRESHOLD As String = "PD_PARAM_THRESHOLD"          '[DB]�����p臒l
    Public Const DB_PARAM_CROP_SHRINK As String = "PARAM_CROP_SHRINK"               '[DB]�Ԉ���

    ' <20221117> <����> <�ŏI�H�����@�\������1> --->>>
    Public Const DB_PARAM_LABELIMAGE As String = "PARAM_LABELIMAGE"                 '[DB]
    Public Const DB_PARAM_SIMPLEBIN As String = "PARAM_SIMPLEBIN"                   '[DB]
    Public Const DB_PARAM_FILTERBIN As String = "PARAM_FILTERBIN"                   '[DB]
    ' <20221117> <����> <�ŏI�H�����@�\������1> <<<---

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
	Public Const DB_PARAM_CYCLE As String = "PARAM_CYCLE"                            '[DB] �ȈՎ����r����p�����[�^�e�[�u��
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

    '-------------------------------------------------------
    '�N���v��
    '-------------------------------------------------------
    '+++++++++++++++++++++++++++++++++++
    '�x���@�\
    '+++++++++++++++++++++++++++++++++++
    'KS_MASTER
    Public Const FACT_KS_MASTER_01 As Integer = 9101        '�����ύX�v��                  (��TO_DEFECT,   SO_GAMENN)
    Public Const FACT_KS_MASTER_02 As Integer = 9102        '�n�[�g�r�[�g                  (��KS_SLAVE)
    Public Const FACT_KS_MASTER_03 As Integer = 9103        'PC�}�X�^�[�Ǘ���Ԗ₢���킹  (��KS_SLAVE)
    Public Const FACT_KS_MASTER_04 As Integer = 9104        '�����グ������              (��KS_SLAVE,    KS_MASTER)
    Public Const FACT_KS_MASTER_05 As Integer = 9105        '��������������              (��KS_SLAVE,    KS_MASTER)
    Public Const FACT_KS_MASTER_06 As Integer = 9106        '�V�X�e���I���v��              (��SO_GAMENN,   KS_UPSMGR)
    Public Const FACT_KS_MASTER_07 As Integer = 9107        'PC�ċN���v��                  (��SO_GAMENN)
    Public Const FACT_KS_MASTER_08 As Integer = 9108        'PC���O�I�t�v��                (��SO_GAMENN)
    Public Const FACT_KS_MASTER_09 As Integer = 9109        'PC�풓�^�X�N�I���v��          (��KS_MASTER)

    'KS_SLAVE
    Public Const FACT_KS_SLAVE_01 As Integer = 9121         '�����ύX�v��                  (��KS_MASTER)
    Public Const FACT_KS_SLAVE_02 As Integer = 9122         '�����n�[�g�r�[�g              (��KS_MASTER)
    Public Const FACT_KS_SLAVE_03 As Integer = 9123         'PC�}�X�^�[�Ǘ��N�������ʒm    (��KS_MASTER)
    Public Const FACT_KS_SLAVE_04 As Integer = 9124         '�V���b�g�_�E���v��            (��KS_MASTER)
    Public Const FACT_KS_SLAVE_05 As Integer = 9125         'PC�ċN���v��                  (��KS_MASTER)
    Public Const FACT_KS_SLAVE_06 As Integer = 9126         'PC���O�I�t�v��                (��KS_MASTER)
    Public Const FACT_KS_SLAVE_07 As Integer = 9127         'PC�풓�^�X�N�I���v��          (��KS_MASTER)

    'KS_START
    '9131

    'KS_SYSLOG
    Public Const FACT_KS_SYSLOG_01 As Integer = 9141        '�V�X���O�o��
    Public Const FACT_KS_SYSLOG_02 As Integer = 9142        '�@���ԃ��O

    'KS_EVTLOG
    '9151

    'KS_WAKEUP
    Public Const FACT_KS_WAKEUP_01 As Integer = 9161        '���������グ������          (��KS_MASTER,   KS_SLAVE)
    Public Const FACT_KS_WAKEUP_02 As Integer = 9162        '�~�ϗ����グ������          (��KS_MASTER,   KS_SLAVE)

    'KS_UPSMGR
    '9171

    'KS_BACKUP
    '9181

    'KS_EQALDB
    '9191
    Public Const FACT_KS_EQALDB_01 As Integer = 9191        '�������s�ʒm                  (��KS_BACKUP)
    'Public Const FACT_KS_EQALDB_02 As Integer = 9192        '���шړ��ʒm                 (��KS_BACKUP)

    'KS_MAILTR
    '9231
    Public Const FACT_KS_MAILTR_01 As Integer = 9231        'UDP���M�˗�(mailudp_que)

    '+++++++++++++++++++++++++++++++++++
    '�~�ϋ@�\
    '+++++++++++++++++++++++++++++++++++
    'TI_DELETE
    Public Const FACT_TI_DELETE_01 As Integer = 9411        '�폜������D�惂�[�h�ʒm       (��TO_DEFECT)

    '+++++++++++++++++++++++++++++++++++
    '�c�[��
    '+++++++++++++++++++++++++++++++++++
    '9800�`9899 �̓c�[�����Ŏg�p�̈ח\��

    '+++++++++++++++++++++++++++++++++++
    '�z���g�ɋ��ʂŎg�p
    '+++++++++++++++++++++++++++++++++++
    Public Const FACT_KS_TASK_STATEUS As Integer = 9901     '�^�X�N��Ԏ擾�v��(�`9920�܂ŗ\��)
    Public Const FACT_KS_DB_STATEUS As Integer = 9997       'DB��ԕύX�ʒm(fl.data[0]= 0:���� 1:�ُ�)	
    Public Const FACT_KS_PCNAME_CHANGE As Integer = 9998    'PC���̕ύX�ʒm�N���v��
    Public Const FACT_KS_END As Integer = 9999              '�^�X�N�I���N���v��

    '-------------------------------------------------------
    '��ʕ\���F
    '-------------------------------------------------------
    '����
    Public g_ColorControl As Color = Color.FromKnownColor(KnownColor.Control)   '�V�X�e���w�i�F

    '�ғ����
    Public g_ColorKadouInit As Color = Color.Red                                '��������
    Public g_ColorKadouStop As Color = Color.Red                                '��~��
    Public g_ColorKadouStart As Color = Color.Blue                              '�ғ���
    Public g_ColorKadouRetry As Color = Color.DarkOrange                        '��ײ��

    '�������
    Public g_ColorKensaNon As Color = Color.FromKnownColor(KnownColor.Control)  '��~
    Public g_ColorKensaOk As Color = Color.Blue                                 '����
    Public g_ColorKensaRed As Color = Color.Red                                 '�k��(�R�C����񗓁@�ԐF)
    Public g_ColorKensaNg As Color = Color.Violet                               '�k��(���F)
    Public g_ColorKensaEdgeNg As Color = Color.Magenta                          '�G�b�W�s��
    Public g_ColorKensaDbNon As Color = Color.MediumOrchid                      '���ۑ�
    Public g_ColorKensaSkip As Color = Color.MediumPurple                       '������
    Public g_ColorKensaStop As Color = Color.Black                              '������~
    Public g_ColorMente As Color = Color.DarkOrange                             '��Ò�

    Public g_ColorOkure As Color = Color.Violet                                 '�������\���ԂŎ�M�x��(�\��PC�ł����g��Ȃ�)

    '���Ə��
    Public g_ColorSogyoStop As Color = Color.Red                                '��~��
    Public g_ColorSogyoSlow As Color = Color.Blue                               '���C���ꎞ��~or�ᑬ����
    Public g_ColorSogyoRun As Color = Color.Blue                                '�ʔ�
    Public g_ColorSogyoNon As Color = Color.Red                                 '�s��

    '�@����
    Public g_ColorKikiOk As Color = Color.Blue                                  '����
    Public g_ColorKikiKei As Color = Color.Orange                               '�y�̏�
    Public g_ColorKikiJyu As Color = Color.Red                                  '�d�̏�

    '�^�]���[�h
    Public g_ColorUntenStop As Color = Color.Red                                '��~
    Public g_ColorUntenNomal As Color = Color.Blue                              '�ʏ�^�]
    Public g_ColorUntenSamp As Color = Color.Magenta                            '�e�X�g�^�]
    Public g_ColorUntenCamera As Color = Color.Orange                           '�Z�����[�h
    Public g_ColorUntenSmyu As Color = Color.Orange                             '�V�~�����[�V����

    'PLG�敪
    Public g_ColorPlgLine As Color = Color.Blue                                 '���C��
    Public g_ColorPlgTop As Color = Color.Magenta                               '�\�Z��
    Public g_ColorPlgBot As Color = Color.Magenta                               '���Z��
    Public g_ColorPlgTest As Color = Color.Orange                               '�e�X�g

    '-------------------------------------------------------
    'TEMP�t�@�C���֘A
    '-------------------------------------------------------
    Public Const IMG_PATH As String = "..\TEMP\JPEG"                            '�n�[�h�R�s�[�i�[�ꏊ
    Public Const CSV_PATH As String = "..\TEMP\CSV"                             'CSV�i�[�ꏊ
    Public Const EXCEL_PATH As String = "..\TEMP\EXCEL"                         'EXCEL�i�[�ꏊ

    '-------------------------------------------------------
    '�V�X���O��
    '-------------------------------------------------------
    '-----------------------------------------------------------------------------------------------------------------------------
    '�S�� 0
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_NON As Integer = 0                   '								        SyslogNo�����蓖��

    '-----------------------------------------------------------------------------------------------------------------------------
    '���[���֘A 1�`4
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_MAIL_SNED_ERR As Integer = 1         '[�ʒm�� err_code = %d]		            ���[���X���b�g���M���s
    Public Const SYSNO_MAIL_RECV_ERR As Integer = 2         '[�ʒm�� err_code = %d]		            ���[���X���b�g��M���s
    Public Const SYSNO_MAILNO_ERR As Integer = 3            '[event = %d]				            �C�x���g�R�[�h�s��

    '-----------------------------------------------------------------------------------------------------------------------------
    '�蓮�R�����g�֘A 5�`7
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_COMMENT_ALL As Integer = 5           '								        �����(�S��)
    Public Const SYSNO_COMMENT_SOFT As Integer = 6          '								        �����(�\�t�g)
    Public Const SYSNO_COMMENT_HARD As Integer = 7          '								        �����(�n�[�h)

    '-----------------------------------------------------------------------------------------------------------------------------
    '�\�P�b�g 10�`24
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_SOCK_SEND_TIMEOUT As Integer = 10    '[�ʐM��]					            [SOCKET]���M�^�C���A�E�g
    Public Const SYSNO_SOCK_NEW_ERR As Integer = 11         '[err_code = %d]				        [SOCKET]�������s
    Public Const SYSNO_SOCK_SEND_ERR As Integer = 12        '[err_code = %d]				        [SOCKET]���M�G���[
    Public Const SYSNO_SOCK_CONNECT As Integer = 13         '[�ʐM��]					            [SOCKET]�ڑ�����
    Public Const SYSNO_SOCK_CLOSE As Integer = 14           '[�ʐM��]					            [SOCKET]�ؒf
    Public Const SYSNO_SOCK_RECV_ERR As Integer = 15        '[section=%s err_code=%d]	            [SOCKET]��M�G���[
    Public Const SYSNO_SOCK_LISTEN_ERR As Integer = 16      '[section=%s err_code=%d]	            [SOCKET]Listen�G���[
    Public Const SYSNO_SOCK_CLIENT_MAX As Integer = 17      '								        [SOCKET]�N���C�A���g���I�[�o�[
    Public Const SYSNO_SOCK_INITIALIZ As Integer = 18       '[name=%s]					            [SOCKET]�C�j�V�����`����M
    Public Const SYSNO_SOCK_TC_NG As Integer = 19           '[section=%s code=%4.4s]		        [SOCKET]�g�����U�N�V�����R�[�h�ُ�
    Public Const SYSNO_SOCK_RECV_TIMEOUT As Integer = 20    '								        [SOCKET]��M�^�C���A�E�g
    Public Const SYSNO_SOCK_CONNECT_ERR As Integer = 21     '[err_code = %d]				        [SOCKET]Connect���s
    Public Const SYSNO_SOCK_HELCY_TIMEOUT As Integer = 22   '[�ʐM��]                               [SOCKET]�w���V�[�Ď��^�C���A�E�g

    '-----------------------------------------------------------------------------------------------------------------------------
    '���L������ 25�`29
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_MEM_OPEN As Integer = 25             '[���L��������]				            [���L������]�I�[�v�����s
    Public Const SYSNO_MEM_READ As Integer = 26             '[���L��������]				            [���L������]�ǂݍ��ݎ��s
    Public Const SYSNO_MEM_WRITE As Integer = 27            '[���L��������]				            [���L������]�������ݎ��s

    '-----------------------------------------------------------------------------------------------------------------------------
    'DB 30�`39
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_DB_EXECUTEDIRECT_ERR As Integer = 30 '[�e�[�u����]				            [DB]SqlExecuteDirect�G���[
    Public Const SYSNO_DB_FETCH_ERR As Integer = 31         '[�e�[�u����]				            [DB]SqlFetch�G���[
    Public Const SYSNO_DB_CONNECT_ERR As Integer = 32       '[�e�[�u����]				            [DB]SqlConnect�G���[
    Public Const SYSNO_DB_DATA_ERR As Integer = 33          '[�e�[�u����]				            [DB]�f�[�^�s��
    Public Const SYSNO_DB_BINALYWRITE_ERR As Integer = 34   '[�e�[�u����]				            [DB]�o�C�i�������݃G���[
    Public Const SYSNO_DB_ODBC_SET_OK As Integer = 35       '[ODBC��,�A�h���X]		                [DB]ODBC�ݒ菑������
    Public Const SYSNO_DB_ODBC_SET_ERR As Integer = 36      '[ODBC��,�A�h���X]		                [DB]ODBC�ݒ菑�����s

    '-----------------------------------------------------------------------------------------------------------------------------
    'RS232C 40�`45
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_232C_NEW_ERR As Integer = 40         '[err_code=%d]				            [232C]�������s
    Public Const SYSNO_232C_SEND_ERR As Integer = 41        '[err_code=%d]				            [232C]���M�G���[
    Public Const SYSNO_232C_RECV_ERR As Integer = 42        '[err_code=%d]				            [232C]��M�G���[
    Public Const SYSNO_232C_OPEN As Integer = 43            '[�ʐM��]					            [232C]�ڑ�����
    Public Const SYSNO_232C_CLOSE As Integer = 44           '[�ʐM��]					            [232C]�ؒf

    '-----------------------------------------------------------------------------------------------------------------------------
    'PIPE 46�`49
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_PIPE_OPEN As Integer = 45            '							            [PIPE]�ڑ�����
    Public Const SYSNO_PIPE_CLOSE As Integer = 46           '							            [PIPE]�ؒf����
    Public Const SYSNO_PIPE_CONNECT_ERR As Integer = 47     '[err_code=%d]			                [PIPE]�ڑ����s
    Public Const SYSNO_PIPE_EXEC As Integer = 48            '							            [PIPE]���쐳�튮��
    Public Const SYSNO_PIPE_EXEC_ERR As Integer = 49        '[err_code=%d]			                [PIPE]����ُ�

    '-----------------------------------------------------------------------------------------------------------------------------
    '50�`69 ���̑��O���[�v
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_CNT_ERR As Integer = 50              '[err_code=%d sub_code=%d]	            [�J�E���^�[�{�[�h]�ُ픭��
    Public Const SYSNO_PIO_ERR As Integer = 51              '[err_code=%d sub_code=%d]	            [PIO�{�[�h]�ُ픭��
    Public Const SYSNO_AIO_ERR As Integer = 52              '[err_code=%d sub_code=%d]	            [�A�i���O�{�[�h]�ُ픭��
    Public Const SYSNO_PLG_ERR As Integer = 53              '[�����p=%0.3f ��r�p=%0.3f ��=%d]      [�J�E���^�[�{�[�h]PLG�������ُ�

    Public Const SYSNO_STATUS_OK As Integer = 55            '[�@�햼]					            [�@���ԊĎ�]�@���Q����
    Public Const SYSNO_STATUS_NG As Integer = 56            '[�@�햼]					            [�@���ԊĎ�]�@���Q���o
    Public Const SYSNO_EVTLOG_MSG_NG As Integer = 57        '[���b�Z�[�W��]				            [�C�x���g���O�Ď�]�ُ팟�o
    Public Const SYSNO_EVTLOG_AP_NG As Integer = 58         '							            [�C�x���g���O�Ď�]�A�v���P�[�V�����ُ팟�o
    Public Const SYSNO_STATUS_WAR As Integer = 59           '[�@�햼]                               [�@���ԊĎ�] �@��x�����o (�y�̏�)


    '-----------------------------------------------------------------------------------------------------------------------------
    '70�`99 ��ʂ������
    '[��]                                                   '[�T�u���b�Z�[�W�`��]                   [���b�Z�[�W]
    '-----------------------------------------------------------------------------------------------------------------------------
    Public Const SYSNO_TASK_INIT_ERR As Integer = 70        '[�^�X�N��]					            �^�X�N�����N���������s
    Public Const SYSNO_INI_READ_ERR As Integer = 71         '[�Z�b�V����,�L�[]			            [INI]�擾�G���[
    Public Const SYSNO_PCNAME_GET_ERR As Integer = 72       '								        PC���̎擾���s
    Public Const SYSNO_QUEFULL_ERR As Integer = 73          '[�敪����]				                [LIB]�����o�b�t�@�L���[�t���I�I
    Public Const SYSNO_QUEFULL_WAR As Integer = 74          '[�敪����]				                [LIB]�����o�b�t�@�L���[�t��

    Public Const SYSNO_UPS_NG As Integer = 80               '								        �d���ُ�
    Public Const SYSNO_TIMESET_NG As Integer = 81           '[err_code = %d]				        �����ݒ莸�s	
    Public Const SYSNO_RESTAET_ERR As Integer = 82          '							            �ċN�����s	
    Public Const SYSNO_SHUTDOWN_ERR As Integer = 83         '								        �V���b�g�_�E�����s	
    Public Const SYSNO_TIMESET_OK As Integer = 84           '								        �����ݒ芮��	
    Public Const SYSNO_DELETE_ERR As Integer = 85           '							            DB�폜���s
    Public Const SYSNO_COMPRESS_ERR As Integer = 86         '								        DB���k���s
    Public Const SYSNO_RESTAET_OK As Integer = 87           '[PC��]					                PC�ċN���v����t
    Public Const SYSNO_SHUTDOWN_OK As Integer = 88          '                                       �V�X�e���I���v����t
    Public Const SYSNO_SYSTEM_START As Integer = 89         '[�J�n����]				                �����J�n�v��
    Public Const SYSNO_SYSTEM_STOP As Integer = 90          '							            ������~�v��
    Public Const SYSNO_LOGOFF_ERR As Integer = 91           '						                ���O�I�t���s
    Public Const SYSNO_LOGOFF_OK As Integer = 92            '[PC��]					                PC���O�I�t�v����t	
    Public Const SYSNO_DB_CHECK_RETRY As Integer = 93       '[���g���C��]			                �f�[�^�x�[�X�ڑ��`�F�b�N�Őڑ����s

    '///////////////////////////////////////////////////////////////////////////
    '�񋓑�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �g��BOOL
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_BOOL
        DIV_BOOL_TRUE = 1               '����   ��
        DIV_BOOL_FALSE = 0              '�ُ�   �~
        DIV_BOOL_NON = -1               '����   �|
        DIV_BOOL_NULL = -9              '�ΏۊO ��
    End Enum

    ''' <summary>
    ''' PLG�敪
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_PLG
        DIV_PLG_LINE = 1                '���C��
        DIV_PLG_TOP                     '�\�Z��
        DIV_PLG_BOT                     '���Z��
        DIV_PLG_TEST                    '�e�X�g
    End Enum

    ''' <summary>
    ''' �^�]���[�h
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_UNTEN
        DIV_UNTEN_STOP = 0              '��~���[�h
        DIV_UNTEN_NOMAL                 '�ʏ�^�]���[�h
        DIV_UNTEN_SAMP                  '�e�X�g�^�]���[�h(�T���v���B�e)
        DIV_UNTEN_CAMERA                '�Z�����[�h(�J��������)
        DIV_UNTEN_SMYU                  '�V�~�����[�V�������[�h
    End Enum

    ''' <summary>
    ''' ���Ə��
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_SOGYO
        DIV_SOGYO_NON = -1              '�s��
        DIV_SOGYO_STOP = 0              '���C���x�~��
        DIV_SOGYO_SLOW                  '���C���ꎞ��~or�ᑬ����
        DIV_SOGYO_RUN                   '���C�����ƒ�
    End Enum

    ''' <summary>
    ''' �ғ����
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_KADOU
        DIV_KADOU_INIT = 1              '��������
        DIV_KADOU_STOP                  '��~��
        DIV_KADOU_START                 '�ғ���&�e�X�g��
        DIV_KADOU_RETRY                 '���g���C��
    End Enum

    ''' <summary>
    ''' �������
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_KENSA
        DIV_KENSA_NON = -9              '��~(�ΏۊO)

        DIV_KENSA_OK = 0                '����
        DIV_KENSA_NG = 1                '�k��
        DIV_KENSA_MENTE = 2             '�����e��(�v���r�W����)

        DIV_KENSA_EDGENG = 4            '�G�b�W�s��(�G�b�W�����o�A�G�b�W�������ُ�)
        DIV_KENSA_DBNON = 5             'DB���і��ۑ�(�L�QGr���ђx��A�ۑ�����������B)
        DIV_KENSA_SKIP = 6              '���������(�摜�����G���W�����X�L�b�v)

        DIV_KENSA_STOP = 9              '��~(�Жʈُ�̏ꍇ�A������~��)
    End Enum

    ''' <summary>
    ''' �@����
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_KIKI
        DIV_KIKI_OK = 0                 '����
        DIV_KIKI_KEI = 1                '�y�̏�
        DIV_KIKI_TYU = 2                '���̏�(�v���r�W����)
        DIV_KIKI_JYU = 3                '�d�̏�
    End Enum

    ''' <summary>
    ''' �J�����|�W�V����
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_POS
#If cNumCamPos = "NUM_CAM_POS_1" Then
        DIV_POS_C = 0                   'C
#End If
#If cNumCamPos = "NUM_CAM_POS_2" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_3" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_C                       'C
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_4" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_5" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_C                       'C
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_6" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_7" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_C                       'C
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_8" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_9" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_C                       'C
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_10" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_11" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_C                       'C
		DIV_POS_W4                      'W4
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_12" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_W5                      'W5
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_13" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_C                       'C
		DIV_POS_W5                      'W5
		DIV_POS_W4                      'W4
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_14" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_D6                      'D6
        DIV_POS_W6                      'W6
        DIV_POS_W5                      'W5
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
 #If cNumCamPos = "NUM_CAM_POS_15" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_D6                      'D6
        DIV_POS_C                       'C
		DIV_POS_W6                      'W6
		DIV_POS_W5                      'W5
		DIV_POS_W4                      'W4
		DIV_POS_W3                      'W3
		DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
#If cNumCamPos = "NUM_CAM_POS_16" Then
        DIV_POS_DS = 0                  'DS
        DIV_POS_D1                      'D1
        DIV_POS_D2                      'D2
        DIV_POS_D3                      'D3
        DIV_POS_D4                      'D4
        DIV_POS_D5                      'D5
        DIV_POS_D6                      'D6
        DIV_POS_D7                      'D7
        DIV_POS_W7                      'W7
        DIV_POS_W6                      'W6
        DIV_POS_W5                      'W5
        DIV_POS_W4                      'W4
        DIV_POS_W3                      'W3
        DIV_POS_W2                      'W2
        DIV_POS_W1                      'W1
        DIV_POS_WS                      'WS
#End If
   End Enum

    ''' <summary>
    ''' �J�����Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DIV_CAMSET
        DIV_CAMSET_BACK = 0             '[�J�����y�ANo]�o�b�N�A�b�v
#If cNumCamPos = "NUM_CAM_POS_1" Then
        DIV_CAMSET_T_C                  '�\
        
        DIV_CAMSET_B_C                  '��
#End If
#If cNumCamPos = "NUM_CAM_POS_2" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_3" Then
        DIV_CAMSET_T_DS                 '�\
		DIV_CAMSET_T_C
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
		DIV_CAMSET_B_C
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_4" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_5" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_C
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_C
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_6" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_7" Then
	    DIV_CAMSET_T_DS                 '�\
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '��
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_8" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_9" Then
	    DIV_CAMSET_T_DS                 '�\
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '��
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_10" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_11" Then
	    DIV_CAMSET_T_DS                 '�\
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
	    DIV_CAMSET_T_D4
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W4
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '��
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
	    DIV_CAMSET_B_D4
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W4
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_12" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_D5
        DIV_CAMSET_T_W5
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_D5
        DIV_CAMSET_B_W5
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_13" Then
	    DIV_CAMSET_T_DS                 '�\
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
	    DIV_CAMSET_T_D4
	    DIV_CAMSET_T_D5
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W5
	    DIV_CAMSET_T_W4
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '��
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
	    DIV_CAMSET_B_D4
	    DIV_CAMSET_B_D5
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W5
	    DIV_CAMSET_B_W4
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_14" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_D5
        DIV_CAMSET_T_D6
        DIV_CAMSET_T_W6
        DIV_CAMSET_T_W5
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_D5
        DIV_CAMSET_B_D6
        DIV_CAMSET_B_W6
        DIV_CAMSET_B_W5
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_15" Then
	    DIV_CAMSET_T_DS                 '�\
	    DIV_CAMSET_T_D1
	    DIV_CAMSET_T_D2
	    DIV_CAMSET_T_D3
	    DIV_CAMSET_T_D4
	    DIV_CAMSET_T_D5
	    DIV_CAMSET_T_D6
		DIV_CAMSET_T_C
	    DIV_CAMSET_T_W6
	    DIV_CAMSET_T_W5
	    DIV_CAMSET_T_W4
	    DIV_CAMSET_T_W3
	    DIV_CAMSET_T_W2
	    DIV_CAMSET_T_W1
	    DIV_CAMSET_T_WS
	    
	    DIV_CAMSET_B_DS                 '��
	    DIV_CAMSET_B_D1
	    DIV_CAMSET_B_D2
	    DIV_CAMSET_B_D3
	    DIV_CAMSET_B_D4
	    DIV_CAMSET_B_D5
	    DIV_CAMSET_B_D6
		DIV_CAMSET_B_C
	    DIV_CAMSET_B_W6
	    DIV_CAMSET_B_W5
	    DIV_CAMSET_B_W4
	    DIV_CAMSET_B_W3
	    DIV_CAMSET_B_W2
	    DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
#If cNumCamPos = "NUM_CAM_POS_16" Then
        DIV_CAMSET_T_DS                 '�\
        DIV_CAMSET_T_D1
        DIV_CAMSET_T_D2
        DIV_CAMSET_T_D3
        DIV_CAMSET_T_D4
        DIV_CAMSET_T_D5
        DIV_CAMSET_T_D6
        DIV_CAMSET_T_D7
        DIV_CAMSET_T_W7
        DIV_CAMSET_T_W6
        DIV_CAMSET_T_W5
        DIV_CAMSET_T_W4
        DIV_CAMSET_T_W3
        DIV_CAMSET_T_W2
        DIV_CAMSET_T_W1
        DIV_CAMSET_T_WS
        
        DIV_CAMSET_B_DS                 '��
        DIV_CAMSET_B_D1
        DIV_CAMSET_B_D2
        DIV_CAMSET_B_D3
        DIV_CAMSET_B_D4
        DIV_CAMSET_B_D5
        DIV_CAMSET_B_D6
        DIV_CAMSET_B_D7
        DIV_CAMSET_B_W7
        DIV_CAMSET_B_W6
        DIV_CAMSET_B_W5
        DIV_CAMSET_B_W4
        DIV_CAMSET_B_W3
        DIV_CAMSET_B_W2
        DIV_CAMSET_B_W1
        DIV_CAMSET_B_WS
#End If
    End Enum

    ''' <summary>
    ''' ���׃O���[�h
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DETECT_GR
        DETECT_GR_NON = 0               '���Q
        DETECT_GR_KEI                   '�y����
        DETECT_GR_TYU                   '������
        DETECT_GR_JU                    '�d����
    End Enum

    ''' <summary>
    ''' ���׃O���[�h
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum EM_DETECT_GR2
        DETECT_GR_NON = 0               '���Q
        DETECT_GR_KEI                   '�y����
        DETECT_GR_TYU                   '������
        DETECT_GR_JU                    '�d����
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '�@����[TBL_HY_STATUS]
    '-------------------------------------------------------
    Public Const SIZE_TYP_STATUS_INF As Integer = 288
    ''' <summary>
    ''' �@����[TBL_HY_STATUS]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TYP_STATUS_INF, pack:=1)> _
    Public Structure TYP_STATUS_INF
        Dim id As Integer                                   '�@��ID
        Dim pcid As Integer                                 'PCID
        Dim area As Integer                                 '�ُ�͈�
        Dim Kensa As EM_DIV_KENSA                           '�����ُ�x
        Dim Kiki As EM_DIV_KIKI                             '�@��ُ�x
        Dim yusen As Integer                                '�D��x
        <VBFixedString(256), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=256)> _
        Dim msg As String                                   '�ُ펞�̃��b�Z�[�W
        Dim pcmode As Integer                               'PC�敪
        Dim stat As Integer                                 '������
    End Structure

    '-------------------------------------------------------
    '�X�e�[�^�X[TBL_HY_STATUS]
    '-------------------------------------------------------
    Public Const SIZE_TYP_STATUS_NOWSYSTEM As Integer = SIZE_TYP_STATUS_INF * 5 + 20
    ''' <summary>
    ''' �X�e�[�^�X[TBL_HY_STATUS]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TYP_STATUS_NOWSYSTEM, pack:=1)> _
    Public Structure TYP_STATUS_NOWSYSTEM
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, sizeconst:=2)> _
        Dim st_kensa() As EM_DIV_KENSA                      '�����ُ�x
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, sizeconst:=2)> _
        Dim st_kiki() As EM_DIV_KIKI                        '�@��ُ�x
        Dim nCnt As Integer                                 '�ُ�Z�b�g����
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, sizeconst:=5)> _
        Dim typErr() As TYP_STATUS_INF                      '�@����[TBL_HY_STATUS]
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim st_kensa(2 - 1)
            ReDim st_kiki(2 - 1)
            ReDim typErr(5 - 1)
        End Sub
    End Structure
End Module
