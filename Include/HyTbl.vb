'*******************************************************************************
'   ���L��������`
'
'  [Ver]
'       Ver.01  2013/12/12  ����
'
'  [����]
'
'*******************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Public Module HyTbl
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
    '���L����������
    '-------------------------------------------------------
    Public Const TBL_HY_STATUS_NAME As String = "TBL_HY_STATUS"					'�X�e�[�^�X���e�[�u��
	Public Const TBL_HY_POINT_NAME As String = "TBL_HY_POINT"					'�|�C���^���e�[�u��
	Public Const TBL_HY_COILINF_NAME As String = "TBL_HY_COILINF"				'�R�C�����e�[�u��
	Public Const TBL_HY_COILRESULT_NAME As String = "TBL_HY_COILRESULT"			'�R�C�����уe�[�u��
	'Public Const TBL_HY_MAP_COLROW_NAME As String = "TBL_HY_MAP_COLROW"			'�}�b�v���e�[�u��
	Public Const TBL_HY_MAP_ROW_NAME As String = "TBL_HY_MAP_ROW"				'�}�b�v�ʒu�e�[�u��
	Public Const TBL_HY_DEFECTINF_NAME As String = "TBL_HY_DEFECTINF"           '�r���e�[�u��

    Public Const TBL_HY_DEFLIST_NAME As String = "TBL_HY_DEFLIST"               '�d���׈ꗗ�\���|�C���^���
    'Public Const TBL_HY_DSPLENGTH_NAME As String = "TBL_HY_DSPLENGTH"			'�\���ʒu���e�[�u��
    Public Const TBL_HY_DEFMAP_NAME As String = "TBL_HY_DEFMAP"                 '�r�}�b�v�\�����e�[�u��

    'Public Const TBL_HY_MAE_MAP_NAME As String = "TBL_HY_MAE_MAP"				'�O�H���}�b�v���
    Public Const TBL_HY_MAE_INF_NAME As String = "TBL_HY_MAE_INF"               '�O�H���r���

    'Public Const TBL_HY_CYCLE_MAP_NAME As String = "TBL_HY_CYCLE_MAP"			'�����}�b�v���
    'Public Const TBL_HY_CYCLE_INF_NAME As String = "TBL_HY_CYCLE_INF"           '�����r���

    Public Const TBL_HY_ALARM_NAME As String = "TBL_HY_ALARM"                   '�x����

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	Public Const TBL_HY_LONGPD_NAME As String = "TBL_HY_LONGPD"                 '���������e�[�u��
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'-------------------------------------------------------
	'�Œ�T�C�Y
	'-------------------------------------------------------
#If cLOCAL = False Then                                        '�{�Ԋ�
	Public Const TBL_HY_REC_MAX As Integer = 6                                  '�\��PC���L���������R�[�h��
	Public Const TBL_HY_MAE_INF_MAX As Integer = 2000                           '�O�H���r���
    Public Const HY_DEFLIST_REC_MAX As Integer = 25                             '�d���׈ꗗ�\���|�C���^��� �ő僌�R�[�h��
    Public Const HY_DEFMAP_REC_MAX As Integer = 6                               '�r�}�b�v�\�����@�ő匏��
    Public Const DSP_IDX_OLD_MAX As Integer = 1                                 '�Q�ƃ��R�[�h�� (���R�C���܂߂�)
#Else                                                     '�e�X�g��
    Public Const TBL_HY_REC_MAX As Integer = 6                                  '�\��PC���L���������R�[�h��
    Public Const TBL_HY_MAE_INF_MAX As Integer = 2000                           '�O�H���r���
    Public Const HY_DEFLIST_REC_MAX As Integer = 25                             '�d���׈ꗗ�\���|�C���^��� �ő僌�R�[�h��
    Public Const HY_DEFMAP_REC_MAX As Integer = 6                               '�r�}�b�v�\�����@�ő匏��
    Public Const DSP_IDX_OLD_MAX As Integer = 1                                 '�Q�ƃ��R�[�h�� (���R�C���܂߂�)
#End If

    Public Const TBL_HY_COIL_MAX As Integer = TBL_HY_REC_MAX '(TBL_HY_REC_MAX * MAX_V_COIL_NUM)	'�\��PC���L������ �R�C����



	'///////////////////////////////////////////////////////////////////////////
	'�񋓑�
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ��ʕ\�����
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_DIV_DSP_STATUS
		DIV_DSP_NG = -1					'��ʕ\���s�� (�\���ʒm�����ŁA���̃R�C���ɂ����������)
		DIV_DSP_INITIALIZE				'�f�[�^�Ȃ�   (�����l)
		DIV_DSP_STANDBY					'��ʕ\���p�� (�����������A�Ώۍނ��܂��A�\����_��ʉ߂��Ă��Ȃ�)
		DIV_DSP_OK						'��ʕ\����
	End Enum

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '�X�e�[�^�X���e�[�u��[TBL_HY_STATUS]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_STATUS As Integer = 4 + 4 + SIZE_TYP_STATUS_NOWSYSTEM
    ''' <summary>
    ''' �X�e�[�^�X���e�[�u��[TBL_HY_STATUS]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_STATUS, Pack:=1)>
    Public Structure TBL_HY_STATUS
        Dim emUnten As EM_DIV_UNTEN                             '�^�]���
        Dim emKadou As EM_DIV_KADOU                             '�ғ����
        Dim status As TYP_STATUS_NOWSYSTEM                      '�X�e�[�^�X
    End Structure

    '���������|�C���^���e�[�u��[TBL_HY_POINT]��������
    ''-------------------------------------------------------
    ''�S�̕t�����[TBL_HY_POINT]
    ''-------------------------------------------------------
    'Public Const SIZE_POINT_INF_NOW As Integer = 4
    '''' <summary>
    '''' �S�̏��
    '''' </summary>
    '''' <remarks></remarks>
    '<StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_INF_NOW, pack:=1)> _
    'Public Structure POINT_INF_NOW
    '	Dim nLenZan As Integer									'�n�ړ_�c��
    'End Structure

    '-------------------------------------------------------
    '�ʒu��� �ʔ���[TBL_HY_POINT]
    '-------------------------------------------------------
    'Public Const SIZE_POINT_LENGTH As Integer = 4 * NUM_MEN
    Public Const SIZE_POINT_LENGTH As Integer = 4
    ''' <summary>
    ''' �ʔ���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_LENGTH, Pack:=1)>
    Public Structure POINT_LENGTH
        Dim nLen As Integer                                   '�ʔ���
        '�C�j�V�����C�Y
        Public Sub initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    '�ʒu��� �}�b�v�ؑ�[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_POINT_STAGE As Integer = 4 * MAP_STAGE_NUM
    ''' <summary>
    ''' �}�b�v�ؑ�
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_STAGE, Pack:=1)>
    Public Structure POINT_STAGE
        <VBFixedArray(MAP_STAGE_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_STAGE_NUM)>
        Dim nStage() As Integer                                 '�}�b�v�ؑ�
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nStage(MAP_STAGE_NUM - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�e������ʒu���[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_ROW_INFO As Integer = SIZE_POINT_LENGTH * MAX_DISP_POS + SIZE_POINT_STAGE * MAX_DISP_POS _
    + SIZE_POINT_STAGE * NUM_MEN + SIZE_POINT_STAGE + 8
    ''' <summary>
    ''' �e������ʒu���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_ROW_INFO, Pack:=1)>
    Public Structure POINT_ROW_INFO
        <VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)>
        Dim nLenDisp() As POINT_LENGTH                          '�ʔ���[m]
        <VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)>
        Dim nRowDisp() As POINT_STAGE                           '�ʔs
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nRowWrite() As POINT_STAGE                          '���ݏ������ݍs
        <VBFixedArray(MAP_STAGE_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_STAGE_NUM)>
        Dim nRowLast() As Integer                               '���������s
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi() As Integer                                   '�\��
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nLenDisp(MAX_DISP_POS - 1)
            For ii As Integer = 0 To nLenDisp.Length - 1
                nLenDisp(ii).initialize()
            Next
            ReDim nRowDisp(MAX_DISP_POS - 1)
            For ii As Integer = 0 To nRowDisp.Length - 1
                nRowDisp(ii).initialize()
            Next
            ReDim nRowWrite(NUM_MEN - 1)
            For ii As Integer = 0 To nRowWrite.Length - 1
                nRowWrite(ii).initialize()
            Next
            ReDim nRowLast(MAP_STAGE_NUM - 1)
            ReDim yobi(2 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�e���R�[�h�P�ʂ̃|�C���^���[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_POINT_TBL As Integer = SIZE_KIZUKEN_NO + 4 + SIZE_ROW_INFO
    ''' <summary>
    ''' �e���R�[�h�P�ʂ̃|�C���^���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_POINT_TBL, Pack:=1)>
    Public Structure POINT_TBL
        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)>
        Dim cKizukenNo() As Byte                                '�Ǘ�No
        Dim emDispStatus As EM_DIV_DSP_STATUS                   '��ʕ\�����
        Dim typ As POINT_ROW_INFO                               '�ʒu���
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cKizukenNo(SIZE_KIZUKEN_NO - 1)

            typ.initialize()
        End Sub
    End Structure

    '-------------------------------------------------------
    '�|�C���^���e�[�u��[TBL_HY_POINT]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_POINT As Integer = 4 + 4 + 4 * NUM_MEN + 4 + 4 * MAX_DISP_POS _
    + 2 * 4 + SIZE_POINT_TBL * TBL_HY_REC_MAX
    ''' <summary>
    ''' �|�C���^���e�[�u��[TBL_HY_POINT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_POINT, Pack:=1)>
    Public Structure TBL_HY_POINT
        Dim nRefresh As Integer                                 '�X�V���t���O  ��TRUE�̎��́A[nRecDisp]�Q�Ƌ֎~
        Dim nRecWriteBase As Integer                            '�����݃��R�[�h
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nRecWrite() As Integer                              '�e�������u�P�ʂł̌��݃��R�[�h
        Dim nRecDispBase As Integer                             '�\����_�ʒu�ł̌��ݕ\�����R�[�h
        <VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)>
        Dim nRecDisp() As Integer                               '�e�ʔ|�C���g�ł� ���݃��R�[�h
        '<VBFixedArray(MAX_DISP_POS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_DISP_POS)> _
        'Dim inf() As POINT_INF_NOW								'�S�̕t�����
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi() As Integer                                   '�\��
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim pnt() As POINT_TBL                                  '�e���R�[�h�P�ʂ̃|�C���^���
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nRecWrite(NUM_MEN - 1)
            ReDim nRecDisp(MAX_DISP_POS - 1)
            'ReDim inf(MAX_DISP_POS - 1)
            ReDim yobi(2 - 1)
            ReDim pnt(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To pnt.Length - 1
                pnt(ii).initialize()
            Next
        End Sub
    End Structure

    '���������R�C�����e�[�u��[TBL_HY_COILINF]��������
    '-------------------------------------------------------
    '���R�[�h[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_COILINF_REC As Integer = SIZE_COINFO_BASE
    ''' <summary>
    ''' ���R�[�h[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COILINF_REC, Pack:=1)>
    Public Structure COILINF_REC
        Dim coilinf As COINFO_BASE                              '�R�C����񋤒�
    End Structure

    '-------------------------------------------------------
    '�R�C�����e�[�u��[TBL_HY_COILINF]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_COILINF As Integer = SIZE_COILINF_REC * TBL_HY_COIL_MAX
    ''' <summary>
    ''' �R�C�����e�[�u��[TBL_HY_COILINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_COILINF, Pack:=1)>
    Public Structure TBL_HY_COILINF
        <VBFixedArray(TBL_HY_COIL_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_COIL_MAX)>
        Dim rec() As COILINF_REC                                '���R�[�h
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim rec(TBL_HY_COIL_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).coilinf.initialize()
            Next
        End Sub
    End Structure

    '���������R�C�����уe�[�u��[TBL_HY_COILRESULT]��������
    '-------------------------------------------------------
    '�J�b�g���[TBL_HY_COILRESULT]
    '-------------------------------------------------------
    Public Const SIZE_TO_SO_DATA_CUT As Integer = 4 + 4
    ''' <summary>
    ''' �J�b�g���[TBL_HY_COILRESULT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TO_SO_DATA_CUT, Pack:=1)>
    Public Structure TO_SO_DATA_CUT
        Dim nMode As Integer                                    '���(1:�t�@�[�X�g�J�b�g 2:���X�g�J�b�g 3:���� 4:�َ�_)
        Dim nPos As Integer                                     '�ʒu[mm]
        '�C�j�V�����C�Y
    End Structure

    '-------------------------------------------------------
    '���R�[�h[TBL_HY_COILRESULT]
    '-------------------------------------------------------
    Public Const SIZE_COILRESULT_REC As Integer = 4 + 4 + SIZE_TO_SO_DATA_CUT * MAX_SHACUT_COUNT
    ''' <summary>
    ''' ���R�[�h[TBL_HY_COILRESULT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_COILRESULT_REC, Pack:=1)>
    Public Structure COILRESULT_REC
        Dim nLen As Integer                                     '����������[mm]
        Dim nCutCnt As Integer                                  '�J�b�g���Z�b�g��
        <VBFixedArray(MAX_SHACUT_COUNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_SHACUT_COUNT)>
        Dim cut() As TO_SO_DATA_CUT                             '�J�b�g���
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cut(MAX_SHACUT_COUNT - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�R�C�����уe�[�u��[TBL_HY_COILRESULT]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_COILRESULT As Integer = SIZE_COILRESULT_REC * TBL_HY_COIL_MAX
    ''' <summary>
    ''' �R�C�����уe�[�u��[TBL_HY_COILRESULT]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_COILRESULT, Pack:=1)>
    Public Structure TBL_HY_COILRESULT
        <VBFixedArray(TBL_HY_COIL_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_COIL_MAX)>
        Dim rec() As COILRESULT_REC                             '���R�[�h
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim rec(TBL_HY_COIL_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure

    '���������}�b�v���e�[�u��[TBL_HY_MAP_COLROW]��������
    '-------------------------------------------------------
    '�}�b�v���e�[�u��[TBL_HY_MAP_COLROW]
    '-------------------------------------------------------
    'Public Const SIZE_TBL_HY_MAP_COLROW As Integer = SIZE_DSP_MAP_COLROW * TBL_HY_REC_MAX
    '''' <summary>
    '''' �}�b�v���e�[�u��[TBL_HY_MAP_COLROW]
    '''' </summary>
    '''' <remarks></remarks>
    '<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_MAP_COLROW, pack:=1)> _
    'Public Structure TBL_HY_MAP_COLROW
    '	<VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)> _
    '	Dim rec() As DSP_MAP_COLROW								'���R�[�h
    '	'�C�j�V�����C�Y
    '	Public Sub initialize()
    '		ReDim rec(TBL_HY_REC_MAX - 1)
    '		For ii As Integer = 0 To rec.Length - 1
    '			rec(ii).initialize()
    '		Next
    '	End Sub
    'End Structure

    '���������}�b�v�ʒu�e�[�u��[TBL_HY_MAP_ROW]��������
    '-------------------------------------------------------
    '�}�b�v�ʒu�e�[�u��[TBL_HY_MAP_ROW]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_MAP_ROW As Integer = SIZE_DSP_MAP_ROW * TBL_HY_REC_MAX
    ''' <summary>
    ''' �}�b�v�ʒu�e�[�u��[TBL_HY_MAP_ROW]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_MAP_ROW, Pack:=1)>
    Public Structure TBL_HY_MAP_ROW
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim rec() As DSP_MAP_ROW                                '���R�[�h
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim rec(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure

    '���������r���e�[�u��[TBL_HY_DEFECTINF]��������
    '-------------------------------------------------------
    '�r���e�[�u��[TBL_HY_DEFECTINF]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_DEFECTINF As Integer = SIZE_DSP_MAP_DEFECTINF * TBL_HY_REC_MAX
    ''' <summary>
    ''' �r���e�[�u��[TBL_HY_DEFECTINF]
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_DEFECTINF, Pack:=1)>
    Public Structure TBL_HY_DEFECTINF
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim rec() As DSP_MAP_DEFECTINF                          '���R�[�h
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim rec(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure

    '���������d���׈ꗗ�\���|�C���^���e�[�u��[TBL_HY_DEFECTINF]��������
    '-------------------------------------------------------
    '�r���[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST_DEFECT As Integer = SIZE_KIZUKEN_NO + SIZE_COIL_NO + 1 + SIZE_MEI_NO + 1 + 2 + SIZE_DSP_TYP_DEFECTINF
    ''' <summary>
    ''' �r���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_LIST_DEFECT, Pack:=1)>
    Public Structure DSP_LIST_DEFECT 'DSP_DATA_DEF_LIST
        <VBFixedString(SIZE_KIZUKEN_NO), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=SIZE_KIZUKEN_NO)>
        Dim cKanriNo As String                                  ' �Ǘ�NO
        <VBFixedArray(SIZE_COIL_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_COIL_NO + 1)>
        Dim cCoilNo() As Byte                                   ' ���Y�ԍ�
        <VBFixedArray(SIZE_MEI_NO - 1 + 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_MEI_NO + 1)>
        Dim cMeiNo() As Byte                                    ' ����No
        <VBFixedString(2 - 1), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=2)>
        Dim cYobi1 As String                                    ' �\��
        Dim typ As DSP_TYP_DEFECTINF                            ' �\���r���[TBL_HY_DEFECTINF]

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim cCoilNo(SIZE_COIL_NO - 1 + 1)
            ReDim cMeiNo(SIZE_COIL_NO - 1 + 1)
            typ.initialize()
        End Sub

    End Structure

    '-------------------------------------------------------
    '�f�[�^��[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST_DATA As Integer = SIZE_DSP_LIST_DEFECT * HY_DEFLIST_REC_MAX
    ''' <summary>
    ''' �f�[�^��
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_LIST_DATA, Pack:=1)>
    Public Structure DSP_LIST_DATA

        <VBFixedArray(HY_DEFLIST_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=HY_DEFLIST_REC_MAX)>
        Dim DfData() As DSP_LIST_DEFECT                         '�d���׈ꗗ�\���r���

        Public Sub initialize()
            ReDim DfData(HY_DEFLIST_REC_MAX - 1)
        End Sub
    End Structure


    '-------------------------------------------------------
    '�w�b�_��[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST_HEADER As Integer = 4 * 2 + 24
    ''' <summary>
    ''' �w�b�_��
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DSP_LIST_HEADER
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nPos() As Integer                                   '�ŐV���R�[�h�ʒu
        <VBFixedArray(24 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=24)>
        Dim cYobi() As Byte                                     '�\��

        Public Sub initialize()
            ReDim nPos(NUM_MEN - 1)
            ReDim cYobi(24 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '���R�[�h[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_DSP_LIST As Integer = SIZE_DSP_LIST_HEADER + SIZE_DSP_LIST_DATA * NUM_MEN
    ''' <summary>
    ''' ���R�[�h
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_LIST, Pack:=1)>
    Public Structure DSP_LIST
        Dim head As DSP_LIST_HEADER                             '�w�b�_��
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim data() As DSP_LIST_DATA                             '�f�[�^��
        Public Sub initialize()
            ReDim data(NUM_MEN - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�d���׈ꗗ�\���|�C���^���e�[�u��[TBL_HY_DEFLIST]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_DEFLIST As Integer = SIZE_DSP_LIST
    ''' <summary>
    ''' �d���׈ꗗ�\���|�C���^���e�[�u��
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_DEFLIST, Pack:=1)>
    Public Structure TBL_HY_DEFLIST
        Dim rec As DSP_LIST                                     '���R�[�h
    End Structure


    '���������O�H���r���e�[�u��[TBL_HY_MAE_INF]��������
    '-------------------------------------------------------
    '�O�H���������u���[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_MAE_DEFECTINF As Integer = 52
    ''' <summary>
    ''' �O�H���������u���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_MAE_DEFECTINF, Pack:=1)>
    Public Structure DSP_TYP_MAE_DEFECTINF

        Dim nImageFlg As Short                                  ' �摜�`�� (0:Jpeg 1:BMP)
        Dim nDspMode As Short                                   ' ���O�ҏW�t���O (-1:��\��  0:�ʏ�\��  1:�����\��) ���v���r�W����

        Dim men As UShort                                       ' �\���敪  (0:�\ 1:��)
        Dim cycle_div As UShort                                 ' �����r�敪(0:����r, 1:�ȈՎ����r,�A���r 2:��������)
        Dim dsws As UShort                                      ' DSWS�敪  (0:DS 1:WS)
        Dim yobi As UShort

        '// ����A���ʒu�́AFF�����g�p
        Dim speed As Integer                                    ' ���x[mpm]
        Dim pitch As Integer                                    ' �s�b�`[mm]
        Dim roolkei As Single                                   ' ���[���a[mm]
        Dim box_wid As Short                                    ' �r�� [mm]
        Dim box_len As Short                                    ' �r���� [mm]

        '// �摜�\���ɕK�v
        Dim h_res As Single                                     ' �c����\[mm/pixel]
        Dim w_res As Single                                     ' ������\[mm/pixel]
        Dim box_x_min As Short                                  ' �O��Xmin[pixel]		(�؂�o���摜�̍��ォ��̈ʒu) (�}�C�i�X�l���肦��)
        Dim box_x_max As Short                                  ' �O��Xmax[pixel]
        Dim box_y_min As Short                                  ' �O��Ymin[pixel]
        Dim box_y_max As Short                                  ' �O��Ymax[pixel]
        Dim center_x As Short                                   ' �r���S�ʒuX [pixel]  (�؂�o���摜�̍��ォ��̈ʒu)
        Dim center_y As Short                                   ' �r���S�ʒuY [pixel]
        Dim h_size As UShort                                    ' �c��f��[pixel]
        Dim w_size As UShort                                    ' ����f��[pixel]

    End Structure


    '-------------------------------------------------------
    '�O�H���r�摜[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_MAE_KIZUINF As Integer = 256
    ''' <summary>
    ''' �O�H���r�摜
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_MAE_KIZUINF, Pack:=1)>
    Public Structure DSP_TYP_MAE_KIZUINF
        Dim data As DSP_TYP_MAE_DEFECTINF                       ' �O�H���r���

        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_1 As String                              ' �摜�̃p�X1 (�摜��������ALL NULL)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_2 As String                              ' �摜�̃p�X2 (�摜��������ALL NULL)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_3 As String                              ' �摜�̃p�X3 (�摜��������ALL NULL)
        <VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)>
        Dim image_path_4 As String                              ' �摜�̃p�X4 (�摜��������ALL NULL)

        <VBFixedArray(76 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=76)>
        Dim reserve() As Byte

        Public Sub initialize()
            ReDim reserve(76 - 1)
        End Sub
    End Structure

	'-------------------------------------------------------
	'FF���[TBL_HY_MAE_INF]
	'-------------------------------------------------------
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	' 20221021 2TCM���@���ɔ������H������ ���� --->>>
	'Public Const SIZE_DSP_TYP_MAE_FFINF As Integer = SIZE_KIZUKEN_NO + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 2 + 2
	'Public Const SIZE_DSP_TYP_MAE_FFINF As Integer = SIZE_KIZUKEN_NO + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4
	'' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	Public Const SIZE_DSP_TYP_MAE_FFINF As Integer = SIZE_KIZUKEN_NO + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
    ''' <summary>
    ''' FF���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_MAE_FFINF, Pack:=1)>
    Public Structure DSP_TYP_MAE_FFINF

        <VBFixedArray(SIZE_KIZUKEN_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_KIZUKEN_NO)>
        Dim cMaeKizukenNo() As Byte                             ' FF�Ǘ�No (�O�H���Ӄւł̊Ǘ�No)

        Dim nMaeKizuNo As Integer                               ' FF�rNo	   (�O�H���r���ł��rNo)

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim cT() As Byte                                        ' FF�r��

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim cG() As Byte                                        ' FF�O���[�h

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim cProc() As Byte                                     ' FF�r���o�H��

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi1() As Byte

        Dim nDiv As Integer                                     ' FF�r�敪
        Dim nPos As Integer                                     ' FF�r���������ʒu [mm]	������͎g��Ȃ�
        Dim nWidth As Integer                                   ' FF�r�Е����ʒu [mm]		(WS����̈ʒu)
        Dim nLen As Integer                                     ' FF�r���� [mm]

        '// �g��
        Dim nY As Integer                                       ' �ŏI����ʒu [mm]
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
		Dim nY_Out As Integer                                   ' �ŏI�o������ʒu [mm]
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
		
        Dim nTid As Short                                       ' MAE_TYPE_NAME  �� ID  (0:�s��   1�`�F�O�H���̑Ώ��r) ���s���̏ꍇ�́AFF���ŕ\���B�}�X�^�[����`��
        Dim nGid As Short                                       ' MAE_GRADE_NAME �� ID  (0:�s��)

        ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
        Dim nKouteiID As Integer                                ' �H�����
        ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

        Public Sub initialize()
            ReDim cMaeKizukenNo(SIZE_KIZUKEN_NO - 1)
            ReDim cT(2 - 1)
            ReDim cG(2 - 1)
            ReDim cProc(2 - 1)
            ReDim yobi1(2 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '�O�H���r���[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_MAE_INF As Integer = 4 + SIZE_DSP_TYP_MAE_FFINF + SIZE_DSP_TYP_MAE_KIZUINF + 4
    ''' <summary>
    ''' �O�H���r���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MAE_INF, Pack:=1)>
    Public Structure MAE_INF
        Dim nMaeDsp As Integer                                  '�O�H���r�̕\�����
        Dim ff As DSP_TYP_MAE_FFINF                             'FF���
        Dim inf As DSP_TYP_MAE_KIZUINF                          '�O�H���r���
        Dim nKizuNo As Integer                                  '���H��DB �rNo
        Public Sub initialize()
            ff.initialize()
            inf.initialize()
       End Sub
    End Structure

    '-------------------------------------------------------
    '�ʏ��[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_MEN As Integer = 4 + SIZE_MAE_INF * TBL_HY_MAE_INF_MAX
    ''' <summary>
    ''' �ʏ��
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MEN, Pack:=1)>
    Public Structure MEN
        Dim num As Integer                                      '���݂̊i�[����
        <VBFixedArray(TBL_HY_MAE_INF_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_MAE_INF_MAX)>
        Dim maeinf() As MAE_INF                                 '�O�H���r���
        Public Sub initialize()
            ReDim maeinf(TBL_HY_MAE_INF_MAX - 1)
            For ii As Integer = 0 To maeinf.Length - 1
                maeinf(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '���R�[�h[TBL_HY_MAE_INF]
    '-------------------------------------------------------
    Public Const SIZE_REC As Integer = SIZE_MEN * NUM_MEN
    ''' <summary>
    ''' ���R�[�h
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_REC, Pack:=1)>
    Public Structure REC
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim men() As MEN                                    '�ʏ��
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim men(NUM_MEN - 1)
            For ii As Integer = 0 To men.Length - 1
                men(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '�O�H���r���[TBL_HY_MAE_INF]
    '-------------------------------------------------------
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>	
	'Public Const SIZE_TBL_HY_MAE_INF As Integer = SIZE_MEN * TBL_HY_REC_MAX
	Public Const SIZE_TBL_HY_MAE_INF As Integer = SIZE_REC * TBL_HY_REC_MAX
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
    ''' <summary>
    ''' �O�H���r���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_MAE_INF, Pack:=1)>
    Public Structure TBL_HY_MAE_INF
        <VBFixedArray(TBL_HY_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_REC_MAX)>
        Dim rec() As REC                                          '���R�[�h
        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim rec(TBL_HY_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
        End Sub
    End Structure


    '���������x����e�[�u��[TBL_HY_ALARM]��������
    '-------------------------------------------------------
    '��P��[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_DSP_ALARM_DATA As Integer = 4 * MAP_COL_NUM
    ''' <summary>
    ''' ��P��
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_ALARM_DATA, Pack:=1)>
    Public Structure DSP_ALARM_DATA
        <VBFixedArray(MAP_COL_NUM - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAP_COL_NUM)>
        Dim row() As Integer                '��

        Public Sub initialize()
            ReDim row(MAP_COL_NUM - 1)
        End Sub

    End Structure

    '-------------------------------------------------------
    '�\���P��[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_ALARM_TORB As Integer = SIZE_DSP_ALARM_DATA * NUM_MEN
    ''' <summary>
    ''' �\���P��
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_ALARM_TORB, Pack:=1)>
    Public Structure DSP_TYP_ALARM_TORB
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim torb() As DSP_ALARM_DATA        '�\��
        Public Sub initialize()
            ReDim torb(NUM_MEN - 1)
            For ii As Integer = 0 To torb.Length - 1
                torb(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '������P��[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_DSP_TYP_ALARM_KENSADAI As Integer = SIZE_DSP_TYP_ALARM_TORB * 2
    ''' <summary>
    ''' ������P��
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_ALARM, Pack:=1)>
    Public Structure DSP_TYP_ALARM_KENSADAI
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim kendai() As DSP_TYP_ALARM_TORB       '�������
        Public Sub initialize()
            ReDim kendai(2 - 1)
            For ii As Integer = 0 To kendai.Length - 1
                kendai(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    '�x����e�[�u��[TBL_HY_ALARM]
    '-------------------------------------------------------
    Public Const SIZE_TBL_HY_ALARM As Integer = SIZE_DSP_TYP_ALARM_KENSADAI
	''' <summary>
	''' �x����e�[�u��
	''' </summary>
	''' <remarks></remarks>
	<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_ALARM, Pack:=1)>
	Public Structure TBL_HY_ALARM
		Dim a_mode As DSP_TYP_ALARM_KENSADAI       '�x��t���O
		Public Sub initialize()
			a_mode.initialize()
		End Sub
	End Structure

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	'-------------------------------------------------------
	'���������e�[�u��[TBL_HY_LONGPD]
	'-------------------------------------------------------
	Public Const SIZE_TBL_HY_LONGPD_REC As Integer = 4
	Public Const SIZE_TBL_HY_LONGPD As Integer = 4 + SIZE_TBL_HY_LONGPD_REC * TBL_HY_COIL_MAX

	<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_LONGPD_REC, Pack:=1)>
	Public Structure TBL_HY_LONGPD_REC
		Dim nLongPdOccurrenceFlag As Integer
	End Structure

	<StructLayout(LayoutKind.Sequential, Size:=SIZE_TBL_HY_LONGPD, Pack:=1)>
	Public Structure TBL_HY_LONGPD

		'// �S�̃X�e�[�^�X
		Dim nGuidanceFlag As Integer                '// �K�C�_���X�\���t���O(0:�\���Ȃ� 1:�\������) �����R�C���A�O�R�C���Ŕ���

		'// �R�C���P��
		<VBFixedArray(TBL_HY_COIL_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_HY_COIL_MAX)>
		Dim rec() As TBL_HY_LONGPD_REC

		Public Sub initialize()
			ReDim rec(TBL_HY_COIL_MAX - 1)
		End Sub
	End Structure
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

End Module