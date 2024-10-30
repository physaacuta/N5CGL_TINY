'*********************************************************************************
' �̏Ⴭ�O���
'	[Ver]
'		Ver.01    2012/07/13  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuForm
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tClass.tClass_SQLServer

''' <summary>
''' �̏Ⴭ�O���
''' </summary>
''' <remarks></remarks>
Public Class frmLog
    Inherits SO_GAMENN.frmMain_Log2

    Private Const mc_PC_FILTER_ROW As Integer = 3                                 ' PC�t�B���^�[�s��

    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �f�o�b�O���O�\��
        If 1 - 1 = FncNo AndAlso onKey = Keys.Shift Then
            ShowDebugLog()
            Exit Sub
        End If

        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                            ' ��0�I���W��
            Case 0                                   ' F01 RAS��ʒ��ڕ\��
                Dim frm As New frmRas
                frm.Owner = Me.my_frmParent
                frm.SetParentForm(Me.my_frmParent)
                Me.SetParentForm(Nothing)
                frm.Show()
                ExecFromClose()
            Case 1                                  ' F01 �S�I��
                btnAllOn_Click()
            Case 2                                  ' F02 �S����
                btnAllOff_Click()
            Case 3                                   ' F04 �L�����Z��
                Call ExecCancel_Log()
            Case 4                                   ' F05 ���O����
                Call ExecSelect_Log()
            Case 5
            Case 7                                   ' F08 �R�����g�ҏW
                Call ExecComment()
            Case 8                                   ' F09 CSV�o��
                Call CsvOut(CSV_PATH, "�̏Ⴭ�O���.csv")
            Case 9                                   ' F10 ��ʈ��
                Call PrintOut()
            Case 10                                  ' F11 ʰ�޺�߰
                Call HardCopy(IMG_PATH, "�̏Ⴭ�O���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 11                                  ' F12 �I��
                MyBase.ExecFromClose()
        End Select
    End Sub

    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub

    ''' <summary>
    ''' ���O���X�g�_�u���N���b�N
    ''' </summary>
    Private Sub dgvLog_CellDoubleClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvLog.CellDoubleClick
        Call ExecComment()
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmLog_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '���u��
        If NOA_ONLINE <> g_NOA Then
            '�t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
        End If

        '''' ����������
        Call GetDbInfToSetData()
        Call Get_PCInfo()
        Call InitdgvMap(dgvPcKind, GetMaxPcCnt(False))
        Call InitdgvMap(dgvPcName, GetMaxPcCnt(True))
        Call SetPCFillterType()
        Call SetPCFillterName()

        '''' �����I�����
        Call MyBase.SetChkBackColor_Change(chkKubun_A_Jyu, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_S_Jyu, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_H_Jyu, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_A_Kei, True)
        Call MyBase.SetChkBackColor_Change(chkKubun_H_Kei, True)

        '''' �R�����g�@�\��ON/OFF
        If MyBase.tProp_IsComment Then
            Call MyBase.SetChkBackColor_Change(chkKubun_A_Cmt, True)
            Call MyBase.SetChkBackColor_Change(chkKubun_S_Cmt, True)
            Call MyBase.SetChkBackColor_Change(chkKubun_H_Cmt, True)
        End If

    End Sub

    ''' <summary>
    ''' �\��/�ő�\�������擾/�f�[�^�Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetDbInfToSetData()
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String

        Dim rowMaxCnt As Integer = 0        ' �s��
        Dim clrEvt As Integer = 0           ' �C�x���g�F
        Dim clrJyu As Integer = 0           ' �d�̏�F
        Dim clrKei As Integer = 0           ' �y�̏�F

        strSQL = "SELECT LOG�\������, LOG�C�x���g�F, LOG�d�̏�F, LOG�y�̏�F FROM " & DB_COMMON_MASTER
        tcls_DB.Open()

        Dim sqlRead As SqlClient.SqlDataReader = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return

        Try
            sqlRead.Read()
            rowMaxCnt = sqlRead.GetInt32(0)
            clrEvt = sqlRead.GetInt32(1)
            clrJyu = sqlRead.GetInt32(2)
            clrKei = sqlRead.GetInt32(3)

        Catch ex As Exception
            Return
        Finally
            sqlRead.Close()
        End Try

        sqlRead.Close()
        tcls_DB.Close()
        tcls_DB.Dispose()

        '''' �f�[�^�Z�b�g
        MyBase.SetLogKindColor(clrEvt, clrJyu, clrKei)
        MyBase.SetLogMaxRow(rowMaxCnt)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �I�[�o�[���C�h
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' Me.Activated�ł͎g�����ɂȂ�Ȃ����߁ALoad�̏I��1�񂾂��񓯊��Ń��\�b�h
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        '���X�̃��\�b�h���s
        MyBase.LoadEnding()

        '''' �g����
        '' ��ʂ��J���I������^�C�~���O�ň�񌟍����s��
        Call ExecSelect_Log()
    End Sub

    ''' <summary>
    ''' ���O�����J�n
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecStart()
        my_btnFunction(0).Enabled = False
        my_btnFunction(5).Enabled = False
        my_btnFunction(7).Enabled = False
        my_btnFunction(8).Enabled = False
        my_btnFunction(9).Enabled = False
        my_btnFunction(10).Enabled = False

        '' �x�[�X���s
        MyBase.ExecStart()
    End Sub

    ''' <summary>
    ''' ���O��������
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecEnd()
        my_btnFunction(0).Enabled = True
        my_btnFunction(5).Enabled = True
        my_btnFunction(7).Enabled = True
        my_btnFunction(8).Enabled = True
        my_btnFunction(10).Enabled = True

        '�v�����^���Ȃ���΁A��ʈ������
        If Not tClass.tClass_HeadCopy.IsCheckPrinter Then
            my_btnFunction(9).Enabled = False
        Else
            my_btnFunction(9).Enabled = True
        End If

        '' �x�[�X���s
        MyBase.ExecEnd()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgvCtrl">�r�}�b�v�f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub InitdgvMap(ByVal dgvCtrl As tCnt.tCnt_DataGridView, ByVal nMaxCnt As Integer)
        '��{�ݒ�
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl
            '�s��`
            .RowTemplate.Height = 16                                        ' �s����

            .RowCount = 0

            .ColumnHeadersVisible = False                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            '�����`
            '.ScrollBars = ScrollBars.Horizontal
            .DoubleBuffered = True
            .EditMode = DataGridViewEditMode.EditProgrammatically                       ' �Z����������

            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCellsExceptHeaders      ' ���s�T�C�Y��������

            '�Z����`
            '.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            '.DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)


            For ii As Integer = 0 To CInt(Math.Ceiling(nMaxCnt / mc_PC_FILTER_ROW)) - 1
                Using clm As New DataGridViewCheckButtonColumn
                    clm.HeaderText = ""

                    clm.Width = 60
                    clm.Frozen = False
                    clm.ReadOnly = False
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                    clm.DefaultCellStyle.BackColor = Drawing.SystemColors.Control

                    .Columns.Add(clm)
                End Using
            Next

            If nMaxCnt < mc_PC_FILTER_ROW Then
                .RowCount = nMaxCnt
            Else
                .RowCount = mc_PC_FILTER_ROW
            End If

        End With
    End Sub

    ''' <summary>
    ''' SQL���擾
    ''' </summary>
    ''' <param name="bMode">���ڎ擾�敪(True:�ʏ�,False:��(count(*)))</param>
    ''' <returns>�쐬����SQL��</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_PC(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '�s���擾
            strSQL &= " SELECT COUNT(*) "

        Else

            '���ڎ擾
            strSQL = " SELECT "
            strSQL &= " M.PCID as PCID,"
            strSQL &= " I.PCKIND as PCKIND,"
            strSQL &= " M.�p�\�R������ as �p�\�R������,"
            strSQL &= " M.PC�� as PC��,"
            strSQL &= " I.PC��� as PC���,"
            strSQL &= " I.PC�敪 as PC�敪"
        End If

        '�Ō��","���폜
        sqlColumnTrim(strSQL)

        '// �ُ�̂ݕ\���͌���̂܂�
        '// ���ׂĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        strSQL &= " FROM " & DB_PC_MASTER & " M "
        strSQL &= " INNER JOIN " & DB_PC_INF & " I ON I.PCID = M.PCID"
        If bMode Then
            strSQL &= " ORDER BY PCID ASC "
        End If

        '�쐬����SQL��Ԃ�
        Return strSQL

    End Function

    Public Structure TYP_PC_INFO_DATA
        Dim nPcId As Integer                    '
        Dim nPcKind As Integer                  '
        Dim cPcName1 As String                  '
        Dim cPcName2 As String                  '
        Dim cPcType As String                   '
        Dim nPcKbn As Integer                   '
        '�C�j�V�����C�Y
        Public Sub initialize()
            nPcId = 0
            nPcKind = 0
            cPcName1 = String.Empty
            cPcName2 = String.Empty
            cPcType = String.Empty
            nPcKbn = 0
        End Sub
    End Structure

    Private m_lstPcInfo As New List(Of TYP_PC_INFO_DATA)

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Get_PCInfo()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB�ڑ��G���[(PC) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '�\���f�[�^�̍��ڎ擾
        strSQL = GetSQL_PC(True)

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            m_lstPcInfo.Clear()

            ' �f�[�^�\��
            While sqlRead.Read()

                Dim typData As TYP_PC_INFO_DATA = Nothing
                typData.initialize()

                ' �f�[�^���o��
                typData.nPcId = sqlRead.GetInt32(sqlRead.GetOrdinal("PCID"))              'PCID
                typData.nPcKind = sqlRead.GetInt32(sqlRead.GetOrdinal("PCKIND"))          'PCKIND
                typData.cPcType = sqlRead.GetString(sqlRead.GetOrdinal("PC���"))         'PC���
                typData.cPcName2 = sqlRead.GetString(sqlRead.GetOrdinal("�p�\�R������"))  '�p�\�R������
                typData.cPcName1 = sqlRead.GetString(sqlRead.GetOrdinal("PC��"))          'PC��
                typData.nPcKbn = sqlRead.GetInt32(sqlRead.GetOrdinal("PC�敪"))           'PC�敪

                m_lstPcInfo.Add(typData)

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("PC�f�[�^�\�����ɗ�O[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' PC���t�B���^�[�Z�b�g
    ''' </summary>
    Private Sub SetPCFillterName()
        For ii As Integer = 0 To m_lstPcInfo.Count - 1
            Dim nX As Integer = 0
            Dim nY As Integer = 0
            nX = ii Mod mc_PC_FILTER_ROW
            nY = CInt(Math.Floor(ii / mc_PC_FILTER_ROW))
            dgvPcName(nY, nX).Value = m_lstPcInfo(ii).cPcName1
            CType(dgvPcName(nY, nX), DataGridViewCheckButtonCell).Enable = True
            AddCheckBoxPCName(CType(dgvPcName(nY, nX), DataGridViewCheckButtonCell), m_lstPcInfo(ii).cPcName1)
            dgvPcName.AutoResizeColumn(nY)
        Next
    End Sub

    ''' <summary>
    ''' PC�@�\�t�B���^�[�Z�b�g
    ''' </summary>
    Private Sub SetPCFillterType()
        Dim nCnt As Integer = 0

        For ii As Integer = 0 To m_lstPcInfo.Count - 1
            Dim nX As Integer = 0
            Dim nY As Integer = 0

            If m_lstPcInfo(ii).nPcKbn > 0 Then Continue For

            nX = nCnt Mod mc_PC_FILTER_ROW
            nY = CInt(Math.Floor(nCnt / mc_PC_FILTER_ROW))
            dgvPcKind(nY, nX).Value = m_lstPcInfo(ii).cPcType
            CType(dgvPcKind(nY, nX), DataGridViewCheckButtonCell).Enable = True
            AddCheckBoxPCKind(CType(dgvPcKind(nY, nX), DataGridViewCheckButtonCell), CStr(m_lstPcInfo(ii).nPcKind))
            dgvPcKind.AutoResizeColumn(nY)
            nCnt = nCnt + 1
        Next
    End Sub

    ''' <summary>
    ''' PC���擾
    ''' </summary>
    ''' <param name="bMode">True:�SPC�AFalse:�I�����C��PC</param>
    ''' <returns></returns>
    Private Function GetMaxPcCnt(ByVal bMode As Boolean) As Integer
        If bMode Then Return m_lstPcInfo.Count
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To m_lstPcInfo.Count - 1
            If m_lstPcInfo(ii).nPcKbn > 0 Then Continue For
            nCnt = nCnt + 1
        Next
        Return nCnt
    End Function

    ''' <summary>
    ''' �{�^���N���b�N���̐F�ւ�����(PC�@�\)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvPcKind_CellClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPcKind.CellClick
        If e.RowIndex < 0 Then Exit Sub
        If e.ColumnIndex < 0 Then Exit Sub
        If CType(dgvPcKind(e.ColumnIndex, e.RowIndex), DataGridViewCheckButtonCell).Checked Then
            dgvPcKind.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Cyan
        Else                                ' OFF
            dgvPcKind.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Drawing.SystemColors.Control
        End If
        dgvPcKind.Refresh()
    End Sub

    ''' <summary>
    ''' �{�^���N���b�N���̐F�ւ�����(PC��)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvPcName_CellClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPcName.CellClick
        If e.RowIndex < 0 Then Exit Sub
        If e.ColumnIndex < 0 Then Exit Sub
        If CType(dgvPcName(e.ColumnIndex, e.RowIndex), DataGridViewCheckButtonCell).Checked Then
            dgvPcName.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Cyan
        Else                                ' OFF
            dgvPcName.Item(e.ColumnIndex, e.RowIndex).Style.BackColor = Drawing.SystemColors.Control
        End If
        dgvPcName.Refresh()
    End Sub

End Class
