'*********************************************************************************
' �K�C�_���X���O��ʂ̐e�N���X
'	[Ver]
'		Ver.01    2013/09/19  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' �K�C�_���X���O��ʂ̐e�N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_StatusLog
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��`
	'/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���b�h�i�K�C�_���X���O�j�ݒ荀��
    ''' </summary>
    ''' <remarks></remarks>
	Private Enum em_LogListColm
        ''' <summary>�ύX��</summary>
        idayHenTime = 0
        ''' <summary>�Ǘ�No</summary>
        iKizuNo
        ''' <summary>�R�C��No</summary>
        iCoilNo
        ''' <summary>�ғ����</summary>
        iKado
        ''' <summary>�\_�@����</summary>
        iKiki_O
        ''' <summary>�\_�������</summary>
        iKensa_O
        ''' <summary>��_�@����</summary>
        iKiki_U
        ''' <summary>��_�������</summary>
        iKensa_U
        ''' <summary>�K�C�_���X���b�Z�[�W</summary>
        iMsg


        iend
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_lstChkKadKind As New List(Of CheckBox)        ' �敪(�ғ����)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
    Private m_lstChkKikiKind_O As New List(Of CheckBox)       ' �敪(�@���ԕ\)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
    Private m_lstChkKenKind_O As New List(Of CheckBox)        ' �敪(������ԕ\)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
    Private m_lstChkKikiKind_U As New List(Of CheckBox)       ' �敪(�@���ԗ�)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
    Private m_lstChkKenKind_U As New List(Of CheckBox)        ' �敪(������ԗ�)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g

    ' DB�֘A
    Private m_sDB_STATUS_LOG As String = DB_STATUS_LOG                        ' DB�e�[�u�� �@���ԃ��O�e�[�u��
    Private m_sDbSession As String = INI_DB                                 ' DB�Z�b�V����

    ' ���O�֘A
    Private m_bLog_ExecCancel As Boolean = False                            ' �L�����Z���� True



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���v���p�e�B
    '/ /////////////////////////////////////////////////////////////////////////////////



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ �O���A�N�Z�X ���\�b�h
    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���O�\���ő�s��
    ''' </summary>
    ''' <param name="nMaxRow">���O�\���ő�s��</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetLogMaxRow(ByVal nMaxRow As Integer)
        'm_nLogMaxRow = nMaxRow
        nudRowMaxCount.Value = nMaxRow
    End Sub

    ''' <summary>
    ''' �敪�i�ғ���ԁj�`�F�b�N�{�b�N�X�R���g���[�� ��o�^
    ''' </summary>
    ''' <param name="chk">�敪�i�ғ���ԁj�`�F�b�N�{�b�N�X�R���g���[��</param>
    ''' <param name="pcName">DB��������PCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKadKind(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tag�ɕ������ǉ�
        m_lstChkKadKind.Add(chk)     ' ���X�g�ɓo�^
        '' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' �敪�i�@���ԕ\�j�`�F�b�N�{�b�N�X�R���g���[�� ��o�^
    ''' </summary>
    ''' <param name="chk">�敪�i�ғ���ԁj�`�F�b�N�{�b�N�X�R���g���[��</param>
    ''' <param name="pcName">DB��������PCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKikiKind_O(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tag�ɕ������ǉ�
        m_lstChkKikiKind_O.Add(chk)     ' ���X�g�ɓo�^
        '' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' �敪�i�@���ԗ��j�`�F�b�N�{�b�N�X�R���g���[�� ��o�^
    ''' </summary>
    ''' <param name="chk">�敪�i�ғ���ԁj�`�F�b�N�{�b�N�X�R���g���[��</param>
    ''' <param name="pcName">DB��������PCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKikiKind_U(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tag�ɕ������ǉ�
        m_lstChkKikiKind_U.Add(chk)     ' ���X�g�ɓo�^
        '' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub


    ''' <summary>
    ''' �敪�i������ԕ\�j�`�F�b�N�{�b�N�X�R���g���[�� ��o�^
    ''' </summary>
    ''' <param name="chk">�敪�i������ԁj�`�F�b�N�{�b�N�X�R���g���[��</param>
    ''' <param name="pcName">DB��������PCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKenKind_O(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tag�ɕ������ǉ�
        m_lstChkKenKind_O.Add(chk)     ' ���X�g�ɓo�^
        '' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' �敪�i������ԗ��j�`�F�b�N�{�b�N�X�R���g���[�� ��o�^
    ''' </summary>
    ''' <param name="chk">�敪�i������ԁj�`�F�b�N�{�b�N�X�R���g���[��</param>
    ''' <param name="pcName">DB��������PCKind</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddCheckBoxKenKind_U(ByVal chk As CheckBox, ByVal pcName As String)
        chk.Tag = pcName        ' Tag�ɕ������ǉ�
        m_lstChkKenKind_U.Add(chk)     ' ���X�g�ɓo�^
        '' ���łɃN���b�N�C�x���g�n���h���ɒǉ�
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub


    ''' <summary>
    ''' �`�F�b�N�{�b�N�X �w�i�F�ύX
    ''' </summary>
    ''' <param name="chk">�`�F�b�N�{�b�N�X</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox)
        If chk.Checked Then                 ' ON
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub
    ''' <summary>
    ''' �`�F�b�N�{�b�N�X �w�i�F�ύX
    ''' </summary>
    ''' <param name="chk">�`�F�b�N�{�b�N�X</param>
    ''' <param name="checked"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox, ByVal checked As Boolean)
        If checked Then                 ' ON
            chk.Checked = checked
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.Checked = checked
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' ���X�g�z��ɓo�^����Ă���R���g���[���̃`�F�b�N�ς݌����Z�o
    ''' </summary>
    ''' <param name="lstChk"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Protected Friend Function GetLstCheckCount(ByVal lstChk As List(Of CheckBox)) As Integer
        Dim iCnt As Integer = 0
        For ii As Integer = 0 To lstChk.Count - 1
            If lstChk.Item(ii).Checked Then iCnt += 1
        Next ii
        Return iCnt
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' ����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBase_Log_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '' �O���b�h��`
        dgvInitialize(dgvLog)


        '' �����Z�b�g
        dtpStartDay.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
        dtpStartTime.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
        dtpEndDay.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))
        dtpEndTime.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))

    End Sub
    ''' <summary>
    ''' �J��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBase_Log_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        m_lstChkKadKind.Clear()       ' �敪(�ғ����)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
        m_lstChkKikiKind_O.Clear()    ' �敪(�@���ԕ\)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
        m_lstChkKenKind_O.Clear()     ' �敪(������ԕ\)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
        m_lstChkKikiKind_U.Clear()    ' �敪(�@���ԗ�)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
        m_lstChkKenKind_U.Clear()     ' �敪(������ԗ�)�`�F�b�N�{�b�N�X�R���g���[�� ���X�g
    End Sub
    ''' <summary>
    ''' �`�F�b�N�{�b�N�X �`�F�b�N��ԕύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chk_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ' �`�F�b�N�{�b�N�X �w�i�ύX
        SetChkBackColor_Change(CType(sender, CheckBox))
    End Sub
    ''' <summary>
    ''' �����\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timClock_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timClock.Tick
        lblClock.Text = Now.ToString("yyyy/MM/dd HH:mm:ss")
    End Sub
    ''' <summary>
    ''' �����͈͐ݒ�`�F�b�N�{�b�N�X
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkStartEndDay_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkStartDay.CheckedChanged, chkEndDay.CheckedChanged
        If chkStartDay Is sender Then
            dtpStartDay.Enabled = chkStartDay.Checked
            dtpStartTime.Enabled = chkStartDay.Checked
        Else
            dtpEndDay.Enabled = chkEndDay.Checked
            dtpEndTime.Enabled = chkEndDay.Checked
        End If
    End Sub
    ''' <summary>
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel, dtpEndDay.MouseWheel, dtpEndTime.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv
            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �Z���I��
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
            .DefaultCellStyle().SelectionBackColor = Color.DarkBlue         ' �I��
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue 'Color.LightSteelBlue         ' 1�s�����ɐF�ւ�


            '' ���`
            .ColumnCount = CInt(em_LogListColm.iend)


            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowCount = 0
            .ColumnHeadersHeight = 25                                       ' ��w�b�_�T�C�Y

            '��0 (����)
            .Columns(em_LogListColm.idayHenTime).HeaderText = "�ύX��"
            .Columns(em_LogListColm.idayHenTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.idayHenTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
            .Columns(em_LogListColm.idayHenTime).Width = 160
            .Columns(em_LogListColm.idayHenTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
            '��1 (�Ǘ�No)
            .Columns(em_LogListColm.iKizuNo).HeaderText = "�Ǘ�No"
            .Columns(em_LogListColm.iKizuNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iKizuNo).Width = 160
            .Columns(em_LogListColm.iKizuNo).SortMode = DataGridViewColumnSortMode.NotSortable
            '��2 (�R�C��No)
            .Columns(em_LogListColm.iCoilNo).HeaderText = "�R�C��No"
            .Columns(em_LogListColm.iCoilNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iCoilNo).Width = 110
            .Columns(em_LogListColm.iCoilNo).SortMode = DataGridViewColumnSortMode.NotSortable
            '��3 (�ғ����)
            .Columns(em_LogListColm.iKado).HeaderText = "�ғ����"
            .Columns(em_LogListColm.iKado).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKado).Width = 100
            .Columns(em_LogListColm.iKado).SortMode = DataGridViewColumnSortMode.NotSortable
            '��4 (�\_�@����)
            .Columns(em_LogListColm.iKiki_O).HeaderText = "�\_�@����"
            .Columns(em_LogListColm.iKiki_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKiki_O).Width = 100
            .Columns(em_LogListColm.iKiki_O).SortMode = DataGridViewColumnSortMode.NotSortable
            '��5 (�\_�������)
            .Columns(em_LogListColm.iKensa_O).HeaderText = "�\_�������"
            .Columns(em_LogListColm.iKensa_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKensa_O).Width = 100
            .Columns(em_LogListColm.iKensa_O).SortMode = DataGridViewColumnSortMode.NotSortable

            '��6 (��_�@����)
            .Columns(em_LogListColm.iKiki_U).HeaderText = "��_�@����"
            .Columns(em_LogListColm.iKiki_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKiki_U).Width = 100
            .Columns(em_LogListColm.iKiki_U).SortMode = DataGridViewColumnSortMode.NotSortable
            '��7 (��_�������)
            .Columns(em_LogListColm.iKensa_U).HeaderText = "��_�������"
            .Columns(em_LogListColm.iKensa_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iKensa_U).Width = 100
            .Columns(em_LogListColm.iKensa_U).SortMode = DataGridViewColumnSortMode.NotSortable
            '��8 (���e)
            .Columns(em_LogListColm.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).HeaderText = "�K�C�_���X���b�Z�[�W"
            .Columns(em_LogListColm.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).Width = 545
            .Columns(em_LogListColm.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable


        End With
    End Sub


    ''' <summary>
    ''' �I���s�������΍� (�I���s�F�ւ��𖳌�������ꍇ�́A�K�{�B�����ɂ���ƃf�[�^�Z�b�g����1�s�ڂ��ςȎ��ɂȂ�)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvLog_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvLog.SelectionChanged
        ' �w�i�F�������̏ꍇ�̂݃L�����Z��
        If dgvLog.DefaultCellStyle().SelectionBackColor = Color.Transparent Then CType(sender, DataGridView).ClearSelection()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL�������p
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' SQL������ (SELECT��,FROM��,WHERE��,END�� �̏��ԂɃR�[��)
    ''' </summary>
    ''' <param name="sql">����SQL��</param>
    ''' <param name="bMode">true:�ʏ� false:Count(*)</param>
    ''' <param name="nSelectCnt">0�ȊO�̏ꍇ top 10 �Ƃ��ɂ���</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
        '' SELECT��
        subSQL_Create_Select(sql, bMode, nSelectCnt)
        '' FROM��
        subSQL_Create_From(sql)
        '' WHERE��
        subSQL_Create_Where(sql)
        '' END��
        subSQL_Create_End(sql, bMode)

    End Sub
    ''' <summary>
    ''' SQL������ (SELECT��)
    ''' </summary>
    ''' <param name="sql">����SQL��</param>
    ''' <param name="bMode">true:�ʏ� false:Count(*)</param>
    ''' <param name="nSelectCnt">0�ȊO�̏ꍇ top 10 �Ƃ��ɂ���</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_Select(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
        If bMode Then
            sql = "SELECT "
            If 0 <> nSelectCnt Then sql &= "TOP " & nSelectCnt & " "
            sql &= "ISNULL(D.�o�^���t,''),"
            sql &= "ISNULL(D.�ғ����,'0'),"
            sql &= "ISNULL(D.�������_�\,'0'),"
            sql &= "ISNULL(D.�������_��,'0'),"
            sql &= "ISNULL(D.�@����_�\,'0'),"
            sql &= "ISNULL(D.�@����_��,'0'),"
            sql &= "ISNULL(D.�K�C�_���X,''),"
            sql &= "ISNULL(D.�Ǘ�No,''),"
            sql &= "ISNULL(D.�R�C��No,''),"
            sql &= "ISNULL(D.AutoNo,'0')"
        Else
            sql = "SELECT "
            sql &= "COUNT(*) "
        End If
    End Sub
    ''' <summary>
    ''' SQL������ (FROM��)
    ''' </summary>
    ''' <param name="sql">����SQL��</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_From(ByRef sql As String)
        sql &= "FROM " & m_sDB_STATUS_LOG & " D "
    End Sub
    ''' <summary>
    ''' SQL������ (WHERE��)
    ''' </summary>
    ''' <param name="sql">����SQL��</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_Where(ByRef sql As String)
        Dim strWhere As String = ""             ' Where�像�[�N
        Dim ii As Integer
        Dim iWk As Integer
        Dim bWk As Boolean

        Dim iWk1 As Integer
        Dim iWk2 As Integer
        Dim iWk3 As Integer
        Dim iWk4 As Integer
        Dim iWk5 As Integer

        Dim bKubun As Boolean = False

        '''' �����t�B���^
        '' �J�n
        If chkStartDay.Checked Then
            strWhere &= "AND D.�o�^���t >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
        End If
        '' �I��
        If chkEndDay.Checked Then
            strWhere &= "AND D.�o�^���t <= '" & dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss") & "' "
        End If


        iWk1 = GetLstCheckCount(m_lstChkKadKind)                   ' �敪�i�ғ���ԁj�t�B���^
        iWk2 = GetLstCheckCount(m_lstChkKikiKind_O)                ' �敪�i�@���ԕ\�j�t�B���^
        iWk3 = GetLstCheckCount(m_lstChkKikiKind_U)                ' �敪�i�@���ԗ��j�t�B���^
        iWk4 = GetLstCheckCount(m_lstChkKenKind_O)                 ' �敪�i������ԕ\�j�t�B���^
        iWk5 = GetLstCheckCount(m_lstChkKenKind_U)                 ' �敪�i������ԗ��j�t�B���^
        iWk = iWk1 + iWk2 + iWk3 + iWk4 + iWk5

        If iWk = 0 Then                                     ' 0���I��
            ' ��������
        Else

            strWhere &= "AND ("

            '''' �敪�i�ғ���ԁj�t�B���^
            If iWk1 = 0 Then      ' 0���I��
                ' ��������
            Else

                bWk = False                                         ' �敪�t�B���^��1���ł��L�莞 true
                For ii = 0 To m_lstChkKadKind.Count - 1
                    If m_lstChkKadKind.Item(ii).Checked Then                 ' PC�t�B���^ �`�F�b�N
                        If Not bWk Then
                            If Not bKubun Then      '�ŏ��Ɍ������f�[�^
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= "("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' �ғ���ԃZ�b�g
                        strWhere &= " D.�ғ���� = '" & CStr(m_lstChkKadKind.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If


            '''' �敪�i�@���ԕ\�j�t�B���^
            If iWk2 = 0 Then      ' 0���I��
                ' ��������
            Else
                bWk = False                                         ' �敪�t�B���^��1���ł��L�莞 true
                For ii = 0 To m_lstChkKikiKind_O.Count - 1
                    If m_lstChkKikiKind_O.Item(ii).Checked Then                 ' PC�t�B���^ �`�F�b�N
                        If Not bWk Then
                            If Not bKubun Then      '�ŏ��Ɍ������f�[�^
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' �ғ���ԃZ�b�g
                        strWhere &= " D.�@����_�\ = '" & CStr(m_lstChkKikiKind_O.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If

            '''' �敪�i�@���ԗ��j�t�B���^
            If iWk3 = 0 Then      ' 0���I��
                ' ��������
            Else
                bWk = False                                         ' �敪�t�B���^��1���ł��L�莞 true
                For ii = 0 To m_lstChkKikiKind_U.Count - 1
                    If m_lstChkKikiKind_U.Item(ii).Checked Then                 ' PC�t�B���^ �`�F�b�N
                        If Not bWk Then
                            If Not bKubun Then      '�ŏ��Ɍ������f�[�^
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' �ғ���ԃZ�b�g
                        strWhere &= " D.�@����_�� = '" & CStr(m_lstChkKikiKind_U.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If

            '''' �敪�i������ԕ\�j�t�B���^
            If iWk4 = 0 Then      ' 0���I��
                ' ��������
            Else
                bWk = False                                         ' �敪�t�B���^��1���ł��L�莞 true
                For ii = 0 To m_lstChkKenKind_O.Count - 1
                    If m_lstChkKenKind_O.Item(ii).Checked Then                 ' PC�t�B���^ �`�F�b�N
                        If Not bWk Then
                            If Not bKubun Then      '�ŏ��Ɍ������f�[�^
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' �ғ���ԃZ�b�g
                        strWhere &= " D.�������_�\ = '" & CStr(m_lstChkKenKind_O.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If


            '''' �敪�i������ԗ��j�t�B���^
            If iWk5 = 0 Then      ' 0���I�� ���� �S���I��
                ' ��������
            Else
                bWk = False                                         ' �敪�t�B���^��1���ł��L�莞 true
                For ii = 0 To m_lstChkKenKind_U.Count - 1
                    If m_lstChkKenKind_U.Item(ii).Checked Then                 ' PC�t�B���^ �`�F�b�N
                        If Not bWk Then
                            If Not bKubun Then      '�ŏ��Ɍ������f�[�^
                                strWhere &= "("
                                bKubun = True
                            Else
                                strWhere &= " OR ("
                            End If
                            bWk = True
                        Else
                            strWhere &= " OR "
                        End If

                        ' �ғ���ԃZ�b�g
                        strWhere &= " D.�������_�� = '" & CStr(m_lstChkKenKind_U.Item(ii).Tag) & "'"
                    End If
                Next ii
                If bWk Then
                    strWhere &= ") "
                End If
            End If

            strWhere &= ") "

        End If



        '''' WHERE�吶��
        If 0 <> strWhere.Length Then
            sql &= "WHERE 1=1 " & strWhere
        End If
    End Sub
    ''' <summary>
    ''' SQL������ (END��)
    ''' </summary>
    ''' <param name="sql">����SQL��</param>
    ''' <param name="bMode">true:�ʏ� false:Count(*)</param>	
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_End(ByRef sql As String, ByVal bMode As Boolean)
        If bMode Then
            sql &= "ORDER BY D.�o�^���t DESC, D.AutoNo  DESC"
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �������s
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �������s���f
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecCancel_Log()
        m_bLog_ExecCancel = True
    End Sub
    ''' <summary>
    ''' ���O�\���J�n��
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecStart()
        my_btnFunction(3).Enabled = True
        my_btnFunction(4).Enabled = False
        my_btnFunction(11).Enabled = False
        m_bLog_ExecCancel = False
    End Sub
    ''' <summary>
    ''' ���O�\���I����
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecEnd()
        my_btnFunction(3).Enabled = False
        my_btnFunction(4).Enabled = True
        my_btnFunction(11).Enabled = True
        m_bLog_ExecCancel = False
    End Sub



    ''' <summary>
    ''' �������s
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExecSelect_Log()
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim ii As Integer = 0
        Dim rowCnt As Integer = 0
        Dim rowMaxCnt As Integer = 0

        Try
            '''' ������
            Call ExecStart()
            dgvLog.RowCount = 0
            If Not tcls_DB.Open() Then Exit Sub
            Application.DoEvents()

            '''' ���X�g�����擾
            Call subSQL_Create(strSQL, False)
            If Not tcls_DB.GetSelectKey(strSQL, rowMaxCnt) Then Exit Sub
            ' �p�t�H�[�}���X�ׂ̈ɁA�f�[�^�O���b�h�r���[�ɍs���Z�b�g
            If nudRowMaxCount.Value < rowMaxCnt Then        ' �������
                rowMaxCnt = CInt(nudRowMaxCount.Value)
                lblRowCount.BackColor = Color.Pink
            Else
                rowMaxCnt = rowMaxCnt
                lblRowCount.BackColor = SystemColors.Control
            End If
            dgvLog.RowCount = rowMaxCnt
            lblRowCount.Text = rowMaxCnt.ToString("#,##0")
            prbReadRow.Minimum = 0
            prbReadRow.Value = 0
            prbReadRow.Maximum = rowMaxCnt
            prbReadRow.Visible = True
            Application.DoEvents()

            '''' �f�[�^���擾
            Call subSQL_Create(strSQL, True, rowMaxCnt)
            sqlRead = tcls_DB.SelectExecute(strSQL) ' Select�����s
            If sqlRead Is Nothing Then Exit Sub
            ' �f�[�^���o��
            While sqlRead.Read()

                ' �f�[�^���o��
                dgvLog(em_LogListColm.idayHenTime, rowCnt).Value = sqlRead.GetDateTime(0).ToString("yyyy/MM/dd HH:mm:ss")
                dgvLog(em_LogListColm.iKizuNo, rowCnt).Value = sqlRead.GetString(7)
                dgvLog(em_LogListColm.iCoilNo, rowCnt).Value = sqlRead.GetString(8)
                dgvLog(em_LogListColm.iKado, rowCnt).Value = sqlRead.GetInt32(1)
                dgvLog(em_LogListColm.iKensa_O, rowCnt).Value = sqlRead.GetInt32(2)
                dgvLog(em_LogListColm.iKiki_O, rowCnt).Value = sqlRead.GetInt32(4)
                dgvLog(em_LogListColm.iKensa_U, rowCnt).Value = sqlRead.GetInt32(3)
                dgvLog(em_LogListColm.iKiki_U, rowCnt).Value = sqlRead.GetInt32(5)
                dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(6)

                ' �v���O���X�o�[
                prbReadRow.PerformStep()


                If 0 = (rowCnt Mod 100) Then
                    Application.DoEvents()
                End If

                '''' ���s����
                rowCnt += 1
                If rowCnt >= rowMaxCnt Then Exit Sub
                If m_bLog_ExecCancel Then                   ' �L�����Z��
                    ' �L�����Z���������� �\�����Ă���s�������ɂ���
                    dgvLog.RowCount = rowCnt
                    lblRowCount.Text = rowCnt.ToString
                    Exit Sub
                End If
            End While

            '''' DB�ؒf
            tcls_DB.Close()
        Catch ex As Exception
        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

            ' �R���g���[��������
            prbReadRow.Visible = False
            Call ExecEnd()
        End Try
    End Sub

    ''' <summary>
    ''' �\���G���A�݂̂̕`��w��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvLog_CellFormatting(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs) Handles dgvLog.CellFormatting
        ' �ғ���Ԃ��當���ɕύX
        If e.ColumnIndex = em_LogListColm.iKado Then
            Dim strVal As String = ""
            strVal = GetKadouStatusName(CType(e.Value, EM_DIV_KADOU))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        '�f�t�H���g�F�̏ꍇ�̂ݐݒ�
                Dim color As Color = Drawing.Color.Black
                ChangeColor_Kado(e)
            End If
        End If

        ' �@���ԁi�\�j���當���ɕύX
        If e.ColumnIndex = em_LogListColm.iKiki_O Then
            Dim strVal As String = ""
            strVal = GetKiKiStatusName(CType(e.Value, EM_DIV_KIKI))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        '�f�t�H���g�F�̏ꍇ�̂ݐݒ�
                Dim color As Color = Drawing.Color.Black

                ChangeColor_Kiki(e)
            End If
        End If

        ' �@���ԁi���j���當���ɕύX
        If e.ColumnIndex = em_LogListColm.iKiki_U Then
            Dim strVal As String = ""
            strVal = GetKiKiStatusName(CType(e.Value, EM_DIV_KIKI))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        '�f�t�H���g�F�̏ꍇ�̂ݐݒ�
                Dim color As Color = Drawing.Color.Black
                ChangeColor_Kiki(e)
            End If
        End If

        ' ������ԁi�\�j���當���ɕύX
        If e.ColumnIndex = em_LogListColm.iKensa_O Then
            Dim strVal As String = ""
            strVal = GetKensaStatusName(CType(e.Value, EM_DIV_KENSA))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        '�f�t�H���g�F�̏ꍇ�̂ݐݒ�
                Dim color As Color = Drawing.Color.Black

                ChangeColor_Kensa(e)
            End If
        End If

        ' ������ԁi�\�j���當���ɕύX
        If e.ColumnIndex = em_LogListColm.iKensa_U Then
            Dim strVal As String = ""
            strVal = GetKensaStatusName(CType(e.Value, EM_DIV_KENSA))
            e.Value = strVal
            If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then        '�f�t�H���g�F�̏ꍇ�̂ݐݒ�
                Dim color As Color = Drawing.Color.Black
                ChangeColor_Kensa(e)
            End If
        End If

    End Sub



    ''' <summary>
    ''' �ғ���Ԗ��擾
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function GetKadouStatusName(ByVal nVal As EM_DIV_KADOU) As String
        'frmStatusLog���ɂāA�e���C���ʂ̖��̎擾

        Return ""

    End Function


    ''' <summary>
    ''' ������Ԗ��擾
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function GetKensaStatusName(ByVal nVal As EM_DIV_KENSA) As String
        'frmStatusLog���ɂāA�e���C���ʂ̖��̎擾

        Return ""

    End Function

    ''' <summary>
    ''' �@���Ԗ��擾
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overridable Function GetKiKiStatusName(ByVal nVal As EM_DIV_KIKI) As String
        'frmStatusLog���ɂāA�e���C���ʂ̖��̎擾

        Return ""
    End Function

    ''' <summary>
    ''' �F�ς�(�ғ����)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub ChangeColor_Kado(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        'frmStatusLog���ɂāA�e���C���ʂ̐F�w��

    End Sub

    ''' <summary>
    ''' �F�ς�(�@����)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub ChangeColor_Kiki(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        'frmStatusLog���ɂāA�e���C���ʂ̐F�w��

    End Sub

    ''' <summary>
    ''' �F�ς�(�������)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub ChangeColor_Kensa(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        'frmStatusLog���ɂāA�e���C���ʂ̐F�w��

    End Sub

    ''' <summary>
    ''' CSV�o��
    ''' </summary>
    ''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
    ''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""                      '1�s�̃f�[�^

        '' �t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        '' �t�H���_�̗L���`�F�b�N(�����������낤)
        tMod.CheckFolder(strFolder)

        '' CSV�o��
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' �I�[�v��
                cls_Stram.Open()

                strData = ""
                '�w�b�_�[�o��
                For ii As Integer = 0 To dgvLog.Columns.Count - 1
                    strData &= dgvLog.Columns(ii).HeaderText & ","
                Next ii

                '�Ō��","��r��
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)


                '�f�[�^�o��
                For ii As Integer = 0 To dgvLog.Rows.Count - 1
                    strData = ""
                    For jj As Integer = 0 To dgvLog.ColumnCount - 1
                        If jj = 3 Then      '�ғ����
                            strData &= GetKadouStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KADOU)) & ","
                        ElseIf jj = 4 Then  '�@���ԁi�\�j
                            strData &= GetKiKiStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KIKI)) & ","
                        ElseIf jj = 5 Then  '������ԁi�\�j
                            strData &= GetKensaStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KENSA)) & ","
                        ElseIf jj = 6 Then  '�@���ԁi���j
                            strData &= GetKiKiStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KIKI)) & ","
                        ElseIf jj = 7 Then  '������ԁi���j
                            strData &= GetKensaStatusName(CType(dgvLog(jj, ii).Value, EM_DIV_KENSA)) & ","
                        Else
                            strData &= dgvLog(jj, ii).Value.ToString & ","
                        End If
                    Next jj
                    '�Ō��","��r��
                    strData = strData.Remove(strData.Length - 1, 1)
                    cls_Stram.WriteLine(strData)
                Next ii

                ' �N���[�Y
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

            Catch ex As Exception
                strMsg = "CSV�o�͂Ɏ��s���܂����B"
            End Try
        End Using

        '''' �K�C�_���X�\��
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using
    End Sub


End Class
