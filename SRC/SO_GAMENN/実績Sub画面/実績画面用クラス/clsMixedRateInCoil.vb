'*********************************************************************************
'�@�����R�C���P�ʁ@�r�������Ǘ��N���X
'	[Ver]
'		Ver.01    2018/12/12  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict Off
Imports tClassLibrary
Imports System.Runtime.InteropServices

Public Class clsMixedRateInCoil
    Implements IDisposable                          ' �f�X�g���N�^��`

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxCol_MRare As Integer = 2           '�f�[�^��MAX

    '�R�C�����
    Public Structure RATE_CONTROL_DATA
        Dim dgvMixedRateIn As DataGridView       '�����R�C���P�� �������f�[�^�O���b�h
        Dim lblNgMsg As Label                    '�ُ�K�C�_���X�\���p
        Dim bCorrect As Boolean                  '�␳���g�p(True:�g�p�AFalse:�s�g�p)
    End Structure

    '�����R�C���P�ʁ@�r���\����
    Private Structure IN_COIL_KIZU_DATA
        Dim nLen As Integer                     '����������
        Dim dCorrect As Double                  '�␳��
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nKizuCnt() As Integer               '�r��(0:�\�A1:��)
        Public Sub initialize()
            nLen = 0
            ReDim nKizuCnt(NUM_MEN - 1)
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    ' ���O�Ǘ��N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False                ' �d������Ăяo�������o����ɂ�
    Private m_typRate As RATE_CONTROL_DATA                  ' �������N���X�R���g���[��

    ' �O���b�h�@��ݒ�
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '���ڌ��o��
        iRate                   '������
    End Enum

    ' �����R�C���P�� �������O���b�h �s�ݒ�
    Private Enum EM_MRATE_ROW_ID_IN
        iOmote = 0          '�\
        iUra                '��
    End Enum


    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="typRatedata">�R�C�����</param>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typRatedata As RATE_CONTROL_DATA, ByRef clsLog As tClass.tClass_LogManager)

        '�T�����
        m_typRate = typRatedata

        '���O�N���X
        mcls_Log = clsLog

    End Sub

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
            End If

        End If
        Me.disposedValue = True
    End Sub
#Region " IDisposable Support "
    ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' ����������������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()

        '�f�[�^�O���b�h�����ݒ�
        Call dgvInitialize(m_typRate.dgvMixedRateIn)

        '�r�����������\��
        Call dgvClear(m_typRate.dgvMixedRateIn)

    End Sub

    ''' <summary>
    ''' �r�������\���i�����R�C���P�ʁj�O���b�h�̏�����
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)

        Dim ii As Integer = 0
        With dgv
            .RowCount = NUM_MEN

            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.None

            .SelectionMode = DataGridViewSelectionMode.CellSelect        ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = False                                   ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = DgvMaxCol_MRare           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y

            '��P (�A�h���X) ���ڌ��o��
            '.Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 60
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' �����ړ��֎~

            '��Q (�A�h���X) �O���[�h�P
            '.Columns(EM_MRATE_COL_ID.iRate).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iRate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MRATE_COL_ID.iRate).Width = 116
            .Columns(EM_MRATE_COL_ID.iRate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iRate).Frozen = True
        End With

        '���ڌ��o���ݒ�
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID_IN.iOmote).Value = "�\"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID_IN.iUra).Value = "��"

        '�e�s�Z�������ݒ�
        For ii = EM_MRATE_ROW_ID_IN.iOmote To EM_MRATE_ROW_ID_IN.iUra
            dgv.Rows(ii).Height = 15
        Next ii

    End Sub

    ''' <summary>
    ''' �O���b�h�̃N���A����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvClear(ByVal dgv As DataGridView)
        Dim ii As Integer = 0

        '�����m�F�p
        For ii = 0 To dgv.RowCount - 1
            dgv(EM_MRATE_COL_ID.iRate, ii).Value = ""
        Next ii

    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <param name="blnSyslog">syslog�o�͋敪(True:�o��, False:�o�͂Ȃ�)</param>
    ''' <remarks></remarks>
    Public Function DspData(ByVal strKizuken As String, Optional ByVal blnSyslog As Boolean = False) As Boolean

        Dim inCoil As IN_COIL_KIZU_DATA = Nothing   '�����R�C���P�ʁ@�r���

        '�f�[�^�擾
        inCoil.initialize()
        If Not GetData(strKizuken, inCoil, blnSyslog) Then
            Call DspInit()
            Return False
        End If

        '�f�[�^�Z�b�g
        SetData(inCoil)

        '�f�[�^�O���b�h�����t���b�V���C�x���g�ŕ\��
        m_typRate.dgvMixedRateIn.Refresh()

        Return True

    End Function

    ''' <summary>
    ''' �����R�C���P�ʁ@�r�������擾
    ''' </summary>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <param name="inCoil">�r���</param>
    ''' <param name="blnSyslog">syslog�o�͋敪(True:�o��, False:�o�͂Ȃ�)</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strKizuken As String, ByRef inCoil As IN_COIL_KIZU_DATA, ByVal blnSyslog As Boolean) As Boolean
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String                                    'SQL��
        Dim bRet As Boolean = False

        '-------------------------------------------------------------------
        ' SELECT��
        '-------------------------------------------------------------------
        strSQL = "SELECT"
        strSQL &= "  ISNULL(RES.����������,0) as ����������"
        strSQL &= " ,ISNULL(RES.�␳��,1) as �␳��"
        strSQL &= " ,ISNULL(CNT_T.��, 0) as �\���r���\"
        strSQL &= " ,ISNULL(CNT_B.��, 0) as �\���r����"

        '-------------------------------------------------------------------
        ' FROM�� �o�������R�C�����уe�[�u��
        '-------------------------------------------------------------------
        strSQL &= " FROM " & DB_COIL_RESULT & " RES "

        '-------------------------------------------------------------------
        ' �O������  �\ �\���r��
        '-------------------------------------------------------------------
        strSQL &= " LEFT OUTER JOIN (SELECT count(*) as ��, d.�Ǘ�No from "
        strSQL &= DB_DEFECT_INF_T & " d "
        strSQL &= " WHERE d.�d������ <> 1 "
        strSQL &= " Group by d.�Ǘ�No ) CNT_T "
        strSQL &= " ON RES.�Ǘ�No = CNT_T.�Ǘ�No "

        '-------------------------------------------------------------------
        ' �O������  �� �\���r��
        '-------------------------------------------------------------------
        strSQL &= " LEFT OUTER JOIN (SELECT count(*) as ��, d.�Ǘ�No from "
        strSQL &= DB_DEFECT_INF_B & " d "
        strSQL &= " WHERE d.�d������ <> 1 "
        strSQL &= " Group by d.�Ǘ�No ) CNT_B "
        strSQL &= " ON RES.�Ǘ�No = CNT_B.�Ǘ�No "

        '-------------------------------------------------------------------
        ' ������
        '-------------------------------------------------------------------
        strSQL &= " WHERE 1 = 1"
        strSQL = strSQL & " and RES.�Ǘ�No = '" & strKizuken & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_RESULT & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "DB�ڑ��G���[ [" & DB_COIL_RESULT & "]"
            'syslog�o��
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_CONNECT_ERR, String.Format("[{0}]", "[" & DB_COIL_RESULT & "]"))
            End If
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_RESULT & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "DB�ǂݍ��݃G���[ [" & DB_COIL_RESULT & "]"
            'syslog�o��
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_EXECUTEDIRECT_ERR, String.Format("[{0}]", "[" & DB_COIL_RESULT & "]"))
            End If
            Return False
        End If

        Try

            ' �f�[�^�擾
            sqlRead.Read()
            If sqlRead.HasRows Then

                inCoil.nLen = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("����������")))
                inCoil.dCorrect = CDbl(sqlRead.GetDouble(sqlRead.GetOrdinal("�␳��")))
                inCoil.nKizuCnt(EM_MRATE_ROW_ID_IN.iOmote) = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("�\���r���\")))
                inCoil.nKizuCnt(EM_MRATE_ROW_ID_IN.iUra) = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("�\���r����")))

                '����I��
                bRet = True
            Else
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�����R�C���P�ʁ@�r�������擾�G���[ [�Ǘ�No = " & strKizuken & "]")
                '�ُ�K�C�_���X�\��
                m_typRate.lblNgMsg.Text = "�����R�C���P�ʁ@�r���������擾�Ɏ��s���܂����B"
                ''syslog�o��
                'If blnSyslog Then
                '    KizuLibCLI.KizuMod.SysLog(SYSNO_ZISEKI_NG, String.Format("[{0}]", "[�Ǘ�No = " & strKizuken & "]"))
                'End If
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�����R�C���P�ʁ@�r�������擾�G���[ [" & ex.Message & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "�r���������擾�Ɏ��s���܂����B"
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    '''<param name="inCoil">�����R�C���P�ʁ@�r���f�[�^</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal inCoil As IN_COIL_KIZU_DATA)

        Dim ii As Integer   '���[�v�J�E���^
        Dim nLen As Integer

        If m_typRate.bCorrect Then
            '�␳���g�p
            nLen = CInt(inCoil.nLen * inCoil.dCorrect)
        Else
            nLen = inCoil.nLen
        End If

        '�r�������ݒ�
        For ii = 0 To m_typRate.dgvMixedRateIn.RowCount - 1
            m_typRate.dgvMixedRateIn(EM_MRATE_COL_ID.iRate, ii).Value = Format(inCoil.nKizuCnt(ii) / CSng(nLen / 1000) * 100, "#0.00")
        Next ii

    End Sub

    ''' <summary>
    ''' �r�������T�C�Y����
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ResizeMixedRate()
        Dim nColumnHeaderHeight As Integer = 0
        Dim nColumnHeight As Integer = 0
        Dim nColumnHeaderFontSize As Integer = 0
        Dim nColumnFontSize As Integer = 0

        ' �R���g���[���T�C�Y����s�̍������Z�o����B
        nColumnHeight = CInt(Math.Floor(m_typRate.dgvMixedRateIn.Height / NUM_MEN))

        ' �s�̍�������t�H���g�T�C�Y�𒲐�����B
        nColumnFontSize = CInt(GetGridFontScaleSize(m_typRate.dgvMixedRateIn, "99.99", 20, 110, nColumnHeight))

        ' ����
        With m_typRate.dgvMixedRateIn

            For ii As Integer = 0 To .RowCount - 1
                .Rows(ii).Height = nColumnHeight
            Next ii
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", nColumnFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
        End With
    End Sub

End Class
