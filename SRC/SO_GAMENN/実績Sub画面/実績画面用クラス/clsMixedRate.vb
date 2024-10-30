'*********************************************************************************
'�@�r�������Ǘ��N���X
'	[Ver]
'		Ver.01    2008/10/16  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict Off
Imports tClassLibrary
Public Class clsMixedRate
    Implements IDisposable                          ' �f�X�g���N�^��`

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SYSLOG�o��
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const SYSNO_ZISEKI_NG As Integer = 425                     '// �v���R���������я����擾NG

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxCol_MRare As Integer = 2           '�f�[�^��MAX

    '�R�C�����
    Public Structure RATE_CONTROL_DATA
        Dim lblID As Label                       '�J�b�g���т�ID(1�`)
        Dim dgvMixedRate As DataGridView         '�������f�[�^�O���b�h
        Dim lblNgMsg As Label                    '�ُ�K�C�_���X�\���p
        Dim btnPre As Button                     '�O���{�^���i���щ�ʂ̂ݎ����j
        Dim btnNext As Button                    '�����{�^���i���щ�ʂ̂ݎ����j
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
    Private m_dctID As New Dictionary(Of Integer, String)   ' �Y���r���Ǘ�No�̃J�b�g���т�ID
    Private m_nIDCnt As Integer                             ' �J�b�g���т�ID�̃J�E���^

    ' �O���b�h�@��ݒ�
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '���ڌ��o��
        iRate                   '������
    End Enum

    ' �O���b�h �s�ݒ�
    Private Enum EM_MRATE_ROW_ID
        iGrade_No01 = 0         'Grade1
        iGrade_No02             'Grade2
        iGrade_No03             'Grade3
        iGrade_No04             'Grade4
        iGrade_No05             'Grade5
        iGrade_No06             'Grade6
    End Enum

    ''' <summary>
    ''' �J�b�g����ID�J�E���^���Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("���r�������N���X��p�v���p�e�B") _
     , System.ComponentModel.Description("�J�b�g����ID�J�E���^���Z�b�g")>
    Public Property tProp_IDCnt() As Integer
        Get
            Return m_nIDCnt
        End Get
        Set(ByVal Value As Integer)
            m_nIDCnt = Value
        End Set
    End Property

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

            'ID�擾�̈�J��
            m_dctID = Nothing

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
        m_typRate.lblID.Text = ""

        '�r�������f�[�^�O���b�h�����ݒ�
        Call dgvInitialize(m_typRate.dgvMixedRate)

        '�r�����������\��
        Call dgvClear(m_typRate.dgvMixedRate)

    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv
            .RowCount = MAX_PROGRP_GRADE

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

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = DgvMaxCol_MRare           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 16.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 60                                      ' ��w�b�_�T�C�Y

            '��P (�A�h���X) ���ڌ��o��
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 60
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' �����ړ��֎~

            '��Q (�A�h���X) �O���[�h�P
            .Columns(EM_MRATE_COL_ID.iRate).HeaderText = "������[%]"
            .Columns(EM_MRATE_COL_ID.iRate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MRATE_COL_ID.iRate).Width = 118
            .Columns(EM_MRATE_COL_ID.iRate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iRate).Frozen = True                                       ' �����ړ��֎~

        End With

        '���ڌ��o���ݒ�
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No01).Value = "Gr1"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No02).Value = "Gr2"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No03).Value = "Gr3"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No04).Value = "Gr4"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No05).Value = "Gr5"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No06).Value = "Gr6"

        '�e�s�Z�������ݒ�
        For ii = EM_MRATE_ROW_ID.iGrade_No01 To EM_MRATE_ROW_ID.iGrade_No06
            m_typRate.dgvMixedRate.Rows(ii).Height = 40
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
        For ii = EM_MRATE_ROW_ID.iGrade_No01 To MAX_PROGRP_GRADE - 1
            dgv(EM_MRATE_COL_ID.iRate, ii).Value = ""
        Next ii

    End Sub

    ''' <summary>
    ''' �Y���Ǘ�No�̃J�b�g���т�ID��S�Ď擾�i���щ�ʗp�F�f�[�^�\���O�ɕK���擾�j
    ''' </summary>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <param name="blnSyslog">syslog�o�͋敪(True:�o��, False:�o�͂Ȃ�)</param>
    ''' <remarks></remarks>
    Public Function GetCutID(ByVal strKizuken As String, Optional ByVal blnSyslog As Boolean = False) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nCnt As Integer = 0    '�J�E���^
        Dim bRet As Boolean = True

        strSQL = ""
        strSQL &= " SELECT ID"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE �Ǘ�No = '" & strKizuken & "'"
        strSQL &= " ORDER BY �o�^���t ASC"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog�o��
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_CONNECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "DB�ǂݍ��݃G���[ [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog�o��
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_EXECUTEDIRECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        Try

            '�J�b�gID�ۑ��̈�N���A
            m_dctID.Clear()

            ' �f�[�^�擾
            While sqlRead.Read()
                m_dctID.Add(nCnt, sqlRead.GetString(0))
                nCnt += 1
            End While

            '���̐擪��\�����R�[�h�ɐݒ�
            m_nIDCnt = 0

            '�f�[�^0���͎��ُ͈��Ԃ��B
            If nCnt = 0 Then
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�J�b�g����ID�擾�G���[ [0��]")
                '�ُ�K�C�_���X�\��
                m_typRate.lblNgMsg.Text = "�r���������擾�Ɏ��s���܂����B"
                'syslog�o��
                If blnSyslog Then
                    KizuLibCLI.KizuMod.SysLog(SYSNO_ZISEKI_NG, String.Format("[{0}]", "[�Ǘ�No = " & strKizuken & " ,ID = ]"))
                End If
                bRet = False
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�J�b�g����ID�擾�G���[[" & ex.Message & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "�r���������擾�Ɏ��s���܂����B"
            bRet = False
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
    ''' �f�[�^�\���i���щ�ʗp�F���O�Ɏ擾�������ѓ`��ID��I�����ĕ\���j
    ''' </summary>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <param name="blnSyslog">syslog�o�͋敪(True:�o��, False:�o�͂Ȃ�)</param>
    ''' <remarks></remarks>
    Public Function DspRateData(ByVal strKizuken As String, Optional ByVal blnSyslog As Boolean = False) As Boolean

        Dim strCutID As String = ""      '�J�b�g����ID
        Dim bRet As Boolean = True       '��������NG����

        'ID�擾
        strCutID = m_dctID(m_nIDCnt)     '�v���p�e�B����t�H�[����ʂ�m_nIDCnt���{�|���䂷��i�y�[�W�ړ��j

        m_typRate.lblID.Text = CUT_RESULT_ID_NO(m_nIDCnt)
        '�f�[�^�\��
        If Not DspData(strKizuken, strCutID, blnSyslog) Then
            bRet = False
        End If

        '�{�^�L���A����
        Call SetPreNextBotton()

        Return bRet

    End Function

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <param name="strCutID">���ѓ`��ID</param>
    ''' <param name="blnSyslog">syslog�o�͋敪(True:�o��, False:�o�͂Ȃ�)</param>
    ''' <remarks></remarks>
    Public Function DspData(ByVal strKizuken As String, ByVal strCutID As String,
                            ByVal blnSyslog As Boolean) As Boolean

        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '�r�������уf�[�^

        '�f�[�^�擾
        If Not GetData(strKizuken, typZiseki, strCutID, blnSyslog) Then
            Call DspInit()
            Return False
        End If

        '�f�[�^�Z�b�g
        SetData(typZiseki)

        '�f�[�^�O���b�h�����t���b�V���C�x���g�ŕ\��
        m_typRate.dgvMixedRate.Refresh()

        Return True

    End Function

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <param name="typZiseki">�r�������уf�[�^</param>
    ''' <param name="strCutID">���ѓ`��ID</param>
    ''' <param name="blnSyslog">syslog�o�͋敪(True:�o��, False:�o�͂Ȃ�)</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strKizuken As String, ByRef typZiseki As PRO_TYPE_ZISEKI, ByVal strCutID As String,
                            ByVal blnSyslog As Boolean) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData() As Byte        '���`���o�C�i���f�[�^
        Dim bRet As Boolean = False

        strSQL = ""
        strSQL &= " SELECT ���`��"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE �Ǘ�No = '" & strKizuken & "'"
        strSQL &= " AND   ID = '" & strCutID & "'"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog�o��
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_CONNECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
            '�ُ�K�C�_���X�\��
            m_typRate.lblNgMsg.Text = "DB�ǂݍ��݃G���[ [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog�o��
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_EXECUTEDIRECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        Try

            ' �f�[�^�擾
            sqlRead.Read()

            If sqlRead.HasRows Then

                '���`��
                bytData = CType(sqlRead.GetValue(0), Byte())

                tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

                '����I��
                bRet = True

            Else
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�������擾�G���[ [ID = " & strCutID & "]")
                '�ُ�K�C�_���X�\��
                m_typRate.lblNgMsg.Text = "�r���������擾�Ɏ��s���܂����B"
                'syslog�o��
                If blnSyslog Then
                    KizuLibCLI.KizuMod.SysLog(SYSNO_ZISEKI_NG, String.Format("[{0}]", "[�Ǘ�No = " & strKizuken & " ,ID = " & strCutID & "]"))
                End If
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�������擾�G���[ [" & ex.Message & "]")
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
    '''<param name="typZiseki">�v���R�����уf�[�^</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_ZISEKI)

        Dim ii As Integer   '���[�v�J�E���^

        '�����m�F�p
        For ii = EM_MRATE_ROW_ID.iGrade_No01 To MAX_PROGRP_GRADE - 1
            m_typRate.dgvMixedRate(EM_MRATE_COL_ID.iRate, ii).Value = Format(typZiseki.data.grade_inf(ii).nMixedRate / 100, "#0.00")
        Next ii

    End Sub

    ''' <summary>
    ''' �O���A�����{�^���̗L���A������ݒ�i���щ�ʗp�j
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetPreNextBotton()

        'ID�擾��0���̏ꍇ�͑O���^�����{�^���𖳌������A�I���B
        If m_dctID.Count = 0 Then
            m_typRate.btnPre.Enabled = False
            m_typRate.btnNext.Enabled = False
            Exit Sub
        End If

        'ID�擾���킩��ID�擪���R�[�h�̏ꍇ�͑O���{�^���̂ݖ���������B
        If m_nIDCnt = 0 Then
            m_typRate.btnPre.Enabled = False
        Else
            m_typRate.btnPre.Enabled = True
        End If

        'ID�擾���킩��ID�ŏI���R�[�h�̏ꍇ�͎����{�^���̂ݖ���������B
        If m_nIDCnt = m_dctID.Count - 1 Then
            m_typRate.btnNext.Enabled = False
        Else
            m_typRate.btnNext.Enabled = True
        End If

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

        ' �R���g���[���T�C�Y����w�b�_�[�A�s�̍������Z�o����B
        nColumnHeight = CInt(Math.Floor(m_typRate.dgvMixedRate.Height * 0.8 / 6))
        nColumnHeaderHeight = m_typRate.dgvMixedRate.Height - nColumnHeight * 6 - SystemInformation.Border3DSize.Height

        ' �w�b�_�[�A�s�̍�������t�H���g�T�C�Y�𒲐�����B
        nColumnHeaderFontSize = CInt(GetGridFontScaleSize(m_typRate.dgvMixedRate, "������[%]", 16, 110, nColumnHeaderHeight))
        nColumnFontSize = CInt(GetGridFontScaleSize(m_typRate.dgvMixedRate, "99.99", 20, 110, nColumnHeight))

        ' ����
        With m_typRate.dgvMixedRate
            .ColumnHeadersHeight = nColumnHeaderHeight
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", nColumnHeaderFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            For ii As Integer = 0 To .RowCount - 1
                .Rows(ii).Height = nColumnHeight
            Next ii
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", nColumnFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
        End With
    End Sub

End Class
