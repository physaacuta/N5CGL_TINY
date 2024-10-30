'*********************************************************************************
' �r�ꗗ��ʕ\���ݒ�N���X
'	[Ver]
'		Ver.01    2008/06/17  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmAllCodeOutput

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    'DataGridView�^�ONO
    Private Const MC_SHOUSAIDATA As Integer = 0    '�ڍ�dgv �f�[�^�\���G���A
    Private Const MC_SHOUSAIETC As Integer = 1     '�ڍ�dgv �u��L�ȊO�v

    Private Enum em_dgvShousaiColum
        iText_No = 0          'NO
        iCombo_Kizushu        '�r��
        iCombo_Grade          '�ڍ׏o��
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterAll As SqlClient.SqlDataAdapter = Nothing      '�A�_�v�^�[ ���ʏo�͐ݒ�e�[�u��
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing     '�A�_�v�^�[ �r��}�X�^�[
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing    '�A�_�v�^�[ �O���[�h�}�X�^�[

    Private m_DsAll As New DataSet                                  '��ڑ��^�e�[�u�� ���ʏo�͐ݒ�e�[�u��
    Private m_DsShousaiType As New DataSet                          '��ڑ��^�e�[�u�� �ڍ�dgv �r��}�X�^�[ �R���{�{�b�N�X�p �f�[�^�擪�Ɂu�����v��ǉ�����
    Private m_DsShousaiGrade As New DataSet                         '��ڑ��^�e�[�u�� �ڍ�dgv �O���[�h�}�X�^�[ �R���{�{�b�N�X�p �f�[�^�擪�Ɂu�|�v��ǉ�����
    Private m_DsType As New DataSet                                 '��ڑ��^�e�[�u�� �Ȉ�dgv �r��}�X�^�[�p
    Private m_DsGrade As New DataSet                                '��ڑ��^�e�[�u�� �Ȉ�dgv �O���[�h�}�X�^�[�p

    ''' <summary>
    ''' �R���{�{�b�N�X�ɕ\������O���[�h
    ''' </summary>
    ''' <remarks>�R���{�{�b�N�X�\������</remarks>
    Private Class mcls_CmbItem

        ''' <summary>EdasysID</summary>
        Public id As Integer                             'EdasysID
        ''' <summary>�\����</summary>
        Public name As String                            '�\����

        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="nid">EdasysID</param>
        ''' <param name="strName">�\����</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nid As Integer, ByVal strName As String)
            id = nid
            name = strName
        End Sub

        ''' <summary>
        ''' �\���̂̕\������
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���j���[�{�^��
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����{�^������������
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="enable"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmSetPat_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�d�匇�ו\���ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLOR�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select
    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmSetPat_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '�������L���b�V���̊J��
        m_DsAll.Dispose()                       '���ʏo�͐ݒ�e�[�u��
        m_DsShousaiType.Dispose()               '�r���萔�}�X�^�[�e�[�u��
        m_DsShousaiGrade.Dispose()              '�ڍ�dgv �r��}�X�^�[ �R���{�{�b�N�X�p
        m_DsType.Dispose()                      '�ڍ�dgv �O���[�h�}�X�^�[ �R���{�{�b�N�X�p
        m_DsGrade.Dispose()                     '�Ȉ�dgv �r��}�X�^�[�p

        '���ʏo�͐ݒ�e�[�u�� �A�_�v�^�[�J��
        If Not m_AdapterAll Is Nothing Then
            m_AdapterAll.Dispose()
            m_AdapterAll = Nothing
        End If

        '�r��}�X�^�[ �A�_�v�^�[�J��
        If Not m_AdapterType Is Nothing Then
            m_AdapterType.Dispose()
            m_AdapterType = Nothing
        End If

        '�O���[�h�}�X�^�[ �A�_�v�^�[�J��
        If Not m_AdapterGrade Is Nothing Then
            m_AdapterGrade.Dispose()
            m_AdapterGrade = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmAllCodeOutput_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmAllCodeOutput         ' �r�ꗗ��ʕ\���ݒ�Ăяo��

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾���� �R���{�{�b�N�X�̒l���Ƃ��Ă���̂ŁAdgv�ݒ����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '�R���{�{�b�N�X�̕\���f�[�^��������
        Data_Init()

        'DataGridView�^�ONO�ݒ�
        dgvShousaiData.Tag = MC_SHOUSAIDATA
        dgvShousaiEtc.Tag = MC_SHOUSAIETC

        '�ڍ�dgv�ݒ�
        dgvInitialize(dgvShousaiData)
        dgvShousaiColunmDefData(dgvShousaiData, True)
        MyBase.DataGridViewSelectionCancel(dgvShousaiData)

        '�ڍ�dgv�u��L�ȊO�v
        dgvInitialize(dgvShousaiEtc)
        dgvShousaiColunmDefData(dgvShousaiEtc, False)
        MyBase.DataGridViewSelectionCancel(dgvShousaiEtc)

        '�f�[�^�\��
        Data_Disp()

        '�I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        MyBase.DataGridViewSelectionRowColorChange(dgvShousaiData)

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

        '�v�����^���Ȃ���΁A��ʈ������
        If Not tClass_HeadCopy.IsCheckPrinter Then
            btnF07.Enabled = False
        Else
            btnF07.Enabled = True
        End If

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmAllCodeOutput_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        
        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv

            .ColumnCount = 0

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
            '.ColumnHeadersHeight = 38                                      ' �w�b�_�[�̍���
            .RowTemplate.Height = 19                                        ' ���s����Ă镶���������Ă��܂��̂ŁA�f�t�H���g�̍s������ύX

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '
            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '�I���Z���F��`()
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

        End With
    End Sub

    ''' <summary>
    ''' dgv�̑I����Ԃ��ύX���ꂽ��A�Z���̑I�����L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvShousaiData.SelectionChanged, dgvShousaiEtc.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        dgv.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' �ڍאݒ�̃f�[�^dgv���`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <param name="nkbn">dgv�̃^�C�v True = �ڍ� �f�[�^, False = �ڍׁu��L�ȊO�v</param>
    ''' <remarks></remarks>
    Private Sub dgvShousaiColunmDefData(ByVal dgv As DataGridView, ByVal nkbn As Boolean)
        With dgv
            '�ڍ�dgv-----------------------------------------------------------------------------
            'NO
            Using clm As New DataGridViewTextBoxColumn
                '�ڍ�dgv�f�[�^�\���G���A
                clm.HeaderText = "NO"
                clm.Width = 25
                clm.Frozen = True
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

            '�r��
            If nkbn = True Then
                '�ڍ�dgv �r��
                Using clm As New DataGridViewComboBoxColumn
                    clm.HeaderText = "�r��"
                    clm.Width = 239
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = True
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DataSource = m_DsShousaiType.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
                    clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
                    clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
                    clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
                    clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
                    .Columns.Add(clm)
                End Using
            Else
                '�ڍ�dgv �u��L�ȊO�v
                Using clm As New DataGridViewTextBoxColumn
                    clm.Width = 239
                    clm.Frozen = True
                    clm.ReadOnly = True
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            '�ڍ׏o��
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "�ڍ׏o��"
                clm.Width = 200
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Frozen = True
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DataSource = m_DsShousaiGrade.Tables(0)     ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
                clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
                clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
                clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
                clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
                .Columns.Add(clm)

            End Using

            If nkbn = False Then
                '�ڍ�dgv�u��L�ȊO�v�͗�w�b�_�[��\�����Ȃ�
                .ColumnHeadersVisible = False        ' ��w�b�_�[��\��
            End If
        End With
    End Sub

    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvShousaiData.CellEnter, dgvShousaiEtc.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    ''' <summary>
    ''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
    ''' </summary>
    ''' <remarks>�O���[�h���̂Ɉȏ������A�����̍s��ǉ��A</remarks>
    Private Sub Data_Init()
        Dim drowNewType As DataRow      ' �r��}�X�^�[�̐V�K�s
        Dim drowNewGrade As DataRow     ' �r��O���[�h�̐V�K�s
        Dim ii As Integer               ' ���[�v�J�E���^

        '�r���ڑ��e�[�u���̐ݒ�
        ' �r��̐V�K�s��ݒ� EdasysID�A�r���� 
        drowNewType = m_DsShousaiType.Tables(0).NewRow()
        drowNewType.Item("EdasysID") = 0
        drowNewType.Item("Name") = "����"
        ' �e�[�u���̈�ԏ�ɑ}��
        m_DsShousaiType.Tables(0).Rows.InsertAt(drowNewType, 0)

        '�O���[�h��ڑ��e�[�u���̐ݒ�
        ' �S�s���[�v �O���[�h�̖��̂Ɉȏ������
        For ii = 0 To m_DsShousaiGrade.Tables(0).Rows.Count - 1
            m_DsShousaiGrade.Tables(0).Rows(ii).Item("Name") = m_DsShousaiGrade.Tables(0).Rows(ii).Item("Name").ToString & "�ȏ�"
            m_DsGrade.Tables(0).Rows(ii).Item("Name") = m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString & "�ȏ�"
        Next
        ' �O���[�h�̐V�K�s��ݒ� EdasysID�A�O���[�h���́A�}�b�v�\���F
        drowNewGrade = m_DsShousaiGrade.Tables(0).NewRow()
        drowNewGrade.Item("EdasysID") = 0
        drowNewGrade.Item("Name") = "-"
        drowNewGrade.Item("�}�b�v�\���F") = -1      ' �}�b�v�\���F�A�g��Ȃ����ǈꉞ�ݒ�
        ' �e�[�u���̈�ԏ�ɑ}��
        m_DsShousaiGrade.Tables(0).Rows.InsertAt(drowNewGrade, 0)

    End Sub

    ''' <summary>
    ''' �f�[�^�x�[�X����l���擾����
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""         'SQL���i�[�G���A
        Dim strErrMsg As String = ""      '�G���[���e�i�[�G���A

        '�e�[�u��"PALL_OUTPUT"�l�擾
        strSQL = "SELECT �r��EdasysID, �d���׈ꗗ��ʕ\��_�ڍ׏���, �d���׈ꗗ��ʕ\��_�ȈՏ���, �d���׈ꗗ��ʕ\��_�ȈՍ���"
        strSQL &= " FROM " & DB_PALL_OUTPUT
        strSQL &= " ORDER BY �r��EdasysID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsAll, m_AdapterAll, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '�e�[�u��"TYPE_NAME"�l�擾
        strSQL = "SELECT EdasysID, Name "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '�e�[�u��"GRADE_NAME"�l�擾
        strSQL = "SELECT EdasysID, Name, �}�b�v�\���F "
        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY �D��x ASC"

        If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        m_DsShousaiType = m_DsType.Copy()      '�擾�����r����ڍ�dgv�p�e�[�u���ɃR�s�[
        m_DsShousaiGrade = m_DsGrade.Copy()    '�擾�����O���[�h���ڍ�dgv�p�e�[�u���ɃR�s�[
        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim drowAllData() As DataRow = m_DsAll.Tables(0).Select("�r��EdasysID>=0")
        Dim drowAllEtc() As DataRow = m_DsAll.Tables(0).Select("�r��EdasysID=-1")
        Dim drowType() As DataRow = m_DsType.Tables(0).Select()
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select()

        Dim ii As Integer                                                       '���[�v�J�E���^
        Dim jj As Integer                                                       '���[�v�J�E���^
        Dim nRowCount As Integer = 0                                            '�R���{�\���p�J�E���^
        Dim strWhere As String = ""

        '�\�����R�[�h���Z�b�g
        dgvShousaiData.RowCount = MAX_TYPE                                      '�ڍ�dgv �ő�\�����R�[�h���Z�b�g(=64)

        '�ڍ� �r��A�O���[�h�\��----------------------------------------------------------------------------------------------
        For ii = 0 To dgvShousaiData.RowCount - 1
            dgvShousaiData(em_dgvShousaiColum.iText_No, ii).Value = ii + 1      'No
            If ii < drowAllData.Length Then
                For jj = nRowCount To m_DsAll.Tables(0).Rows.Count - 1
                    If CInt(m_DsAll.Tables(0).Rows(jj).Item("�r��EdasysID").ToString) >= 0 Then
                        dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value = m_DsAll.Tables(0).Rows(jj).Item("�r��EdasysID")              '�ڍ�dgv �r��R���{�{�b�N�X�����\��
                        dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value = m_DsAll.Tables(0).Rows(jj).Item("�d���׈ꗗ��ʕ\��_�ڍ׏���")     '�ڍ�dgv �O���[�h�R���{�{�b�N�X�����\��
                        nRowCount = jj + 1
                        Exit For
                    End If
                Next jj
            Else
                dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value = 0     '�ڍ�dgv �r��R���{�{�b�N�X�����\��
                dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value = 0       '�ڍ�dgv �O���[�h�R���{�{�b�N�X�����\��
            End If
        Next ii

        '�ڍ� �u��ʈȊO�v�\��
        '���̑�dgv�̐ݒ�
        dgvShousaiEtc.RowCount = 1                                              '�ڍ�dgv�u��L�ȊO�v�\����1�s�Œ�
        dgvShousaiEtc(em_dgvShousaiColum.iText_No, 0).Value = ""                '�ڍ�dgv�u��L�ȊO�vNo�͋�
        dgvShousaiEtc(em_dgvShousaiColum.iCombo_Kizushu, 0).Value = "��L�ȊO"  '�ڍ�dgv�u��L�ȊO�v�r��i"��L�ȊO"�ƕ\������j

        '�f�[�^�����݂��邩
        If drowAllEtc.Length > 0 Then
            dgvShousaiEtc(em_dgvShousaiColum.iCombo_Grade, 0).Value = drowAllEtc(0).Item("�d���׈ꗗ��ʕ\��_�ڍ׏���")            '�ڍ�dgv�u��L�ȊO�v�O���[�h�R���{�{�b�N�X�����\��
        Else
            dgvShousaiEtc(em_dgvShousaiColum.iCombo_Grade, 0).Value = 0
        End If

        '----------------------------------------------------------------------------------------------------------------------

    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strMsg As String       ' �G���[���b�Z�[�W�p
        Dim ii As Integer          ' ���[�v�J�E���^
        Dim jj As Integer          ' ���[�v�J�E���^

        Dim bKizuUmu As Boolean     '�r�킪�P���I������ĂȂ��Ƃ�False 1�ł��r�킪�I������Ă�����True

        bKizuUmu = False

        ' �s�������[�v �R���{�{�b�N�X�����Ȃ��̂ŁA�r��EdasysID�̏d���`�F�b�N�̂ݍs��
        For ii = 0 To dgvShousaiData.Rows.Count - 1
            ' �r�킪�����̍s�͖���
            If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) = 0 Then Continue For

            '�r�킪�I������Ă�����t���O��True�ɂ���
            bKizuUmu = True

            ' �r��̏d���`�F�b�N
            For jj = ii + 1 To dgvShousaiData.Rows.Count - 1
                ' ��r�悪�����̍s�͖���
                If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, jj).Value) = 0 Then Continue For

                ' �r�킪�����Ȃ烁�b�Z�[�W���o���ďI��
                If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) = _
                                    CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, jj).Value) Then
                    strMsg = "�y �r�� �z" & vbNewLine
                    strMsg &= "�����r��͓o�^�ł��܂���B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                    MsgBox(strMsg, MsgBoxStyle.OkOnly Or MsgBoxStyle.Information, "�f�[�^�o�^�ُ�")
                    Return False
                End If
            Next
        Next ii

        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <returns>True:���� False:�ُ�</returns>
    ''' <remarks></remarks>
    Private Function Data_Get() As Boolean
        Dim drow() As DataRow = m_DsAll.Tables(0).Select("")        '�f�[�^�s ���ʏo�͐ݒ�e�[�u���̃f�[�^�S�Ċ��蓖��
        Dim drowNew As DataRow                                      '�V�K�s
        Dim nRowCount As Integer                                    '�폜�Ώۂ̃f�[�^��

        Dim ii As Integer
        Dim mm As Integer

        '�ڍאݒ莞-------------------------------------------------------------------------------------------------------
        '���݉�ʂɕ\�����Ă���pcode�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        nRowCount = drow.Length
        For ii = 1 To nRowCount
            m_DsAll.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next
        ' ���\�����Ă���f�[�^��S�Ċi�[
        For ii = 0 To dgvShousaiData.Rows.Count - 1
            drowNew = m_DsAll.Tables(0).NewRow()                             ' �V�K�s
            '' �f�[�^���Z�b�g
            drowNew.Item("�r��EdasysID") = dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value
            drowNew.Item("�d���׈ꗗ��ʕ\��_�ڍ׏���") = dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value
            drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՏ���") = 0
            drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՍ���") = 0
            ' �e�[�u���ɒǉ�
            m_DsAll.Tables(0).Rows.InsertAt(drowNew, m_DsAll.Tables(0).Rows.Count)
        Next ii

        ' ��L�ȊOdgv�̃f�[�^��S�Ċi�[ �ڍאݒ�̏�L�ȊO�̐ݒ��1�s�����Ȃ�
        For mm = 0 To dgvShousaiEtc.Rows.Count - 1
            drowNew = m_DsAll.Tables(0).NewRow()                             ' �V�K�s
            ' �f�[�^���Z�b�g
            drowNew.Item("�r��EdasysID") = -1              '�u��L�ȊO�v�̐ݒ���r��EdasysID��-1�Œ�
            drowNew.Item("�d���׈ꗗ��ʕ\��_�ڍ׏���") = dgvShousaiEtc(em_dgvShousaiColum.iCombo_Grade, mm).Value
            drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՏ���") = 0
            drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՍ���") = 0
            ' �e�[�u���ɒǉ�
            m_DsAll.Tables(0).Rows.InsertAt(drowNew, m_DsAll.Tables(0).Rows.Count)
        Next mm

        Return True

    End Function

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()
        '���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '��ʂ̃f�[�^���擾
            If Not Data_Get() Then Exit Sub

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' �f�[�^�̍Ď擾
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If

            ' �\���f�[�^���������A�u�����v�s��ǉ��A�u�|�v�s��ǉ�
            Data_Init()

            '�f�[�^�ĕ\��
            Data_Disp()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql��
        Dim ii As Integer                                   '���[�v�J�E���^

        Try
            '�f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            '�f���[�g�C���T�[�g����̂Ō����p�^�[���o�͐ݒ�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_PALL_OUTPUT
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�����p�^�[���o�͐ݒ�e�[�u���փf�[�^��o�^ 
            '�f�[�^�e�[�u���͕����e�[�u�����Q�Ƃ��Ă���ׁA�A�b�v�f�[�g�ł��Ȃ��̂ŁA�C���T�[�g����
            For ii = 0 To m_DsAll.Tables(0).Rows.Count - 1
                With m_DsAll.Tables(0).Rows(ii)
                    If CInt(.Item("�r��EdasysID").ToString) <> 0 Then
                        'TYPEOUT�o�^SQL���쐬
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_PALL_OUTPUT & " VALUES ("
                        strSql &= .Item("�r��EdasysID").ToString & ","                  ' �r��EdasysID
                        strSql &= .Item("�d���׈ꗗ��ʕ\��_�ڍ׏���").ToString & ","       ' �r�ꗗ��ʕ\��_�ڍ׏���
                        strSql &= .Item("�d���׈ꗗ��ʕ\��_�ȈՏ���").ToString & ","       ' �r�ꗗ��ʕ\��_�ȈՏ���
                        strSql &= .Item("�d���׈ꗗ��ʕ\��_�ȈՍ���").ToString & ")"       ' �r�ꗗ��ʕ\��_�ȈՍ���

                        'SQL���s
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' �G���[�Ȃ烍�[���o�b�N���ďI��
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_DEFLIST)
			modMain.HardCopy(Me)
            Return True

        Catch ex As Exception
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' ��n��
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    ''' <summary>
    ''' CurrentCellDirtyStateChanged�C�x���g�n���h��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvShousaiEtc.CurrentCellDirtyStateChanged, dgvShousaiData.CurrentCellDirtyStateChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ�R�~�b�g����
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub

    ''' <summary>
    ''' ����Z���̒l���ς������F��ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvShousaiData.CellValueChanged, dgvShousaiEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim drow() As DataRow = m_DsType.Tables(0).Select()
        Dim check_cnt As Integer = 0    ' �`�F�b�N��
        Dim msg As String = ""          ' ���b�Z�[�W�{�b�N�X���e�i�[�p
        Dim strEdasysID As String

        ' �s�ȊO�̓X���[
        If e.RowIndex < 0 Then Exit Sub

        '�ڍאݒ莞-------------------------------------------------------------------------------------------------
        If CInt(dgv.Tag) = MC_SHOUSAIDATA Or CInt(dgv.Tag) = MC_SHOUSAIETC Then
            ' �O���[�h�R���{�{�b�N�X�̐F�ύX
            If e.ColumnIndex = em_dgvShousaiColum.iCombo_Grade Then
                ' �Z���̒l���擾
                strEdasysID = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

                ' �Z���̒l�iEdasysID�j�ŐF���擾���A�w�i�F��ύX���� 
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
            End If
        End If

    End Sub

    ''' <summary>
    ''' �F�擾
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbColor(ByVal key As String) As Color
        'EdasysID�̒l�����o��()
        Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE��ɑ����̏�������
        Dim drowGrade() As DataRow = m_DsShousaiGrade.Tables(0).Select(strWhere)       ' 1�s����͂�

        Dim retColor As Color = Color.Empty   ' �߂�l �����l��NULL�F

        ' Key��0�Ȃ�NULL�F��Ԃ�
        If 0 = CInt(key) Then Return retColor

        ' 1�s�����Ȃ��Ǝv������
        For ii As Integer = 0 To drowGrade.Length - 1
            ' �J���[���擾
            retColor = Color.FromArgb(CInt(drowGrade(0).Item("�}�b�v�\���F").ToString))
        Next

        Return retColor
    End Function

    ''' <summary>
    ''' �\�����@�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radButton_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        '���W�I�{�^���w�i�F�F�ς�
        If rdoData.Checked Then
            rdoData.BackColor = Color.Lime
        Else
            rdoData.BackColor = g_ColorControl
        End If

    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

End Class
