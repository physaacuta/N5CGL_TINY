
'*********************************************************************************
'�@�t�B���^�[�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2008/07/25  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmSetFilter

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    'DataGridView�^�ONO
    Private Const MC_SHOUSAIDATA As Integer = 0    '�ڍ�dgv �f�[�^�\���G���A
    Private Const MC_SHOUSAIETC As Integer = 1     '�ڍ�dgv �u��L�ȊO�v

    '�ڍ׃O���b�h��`
    Private Enum em_dgvShousaiColum
        iText_No = 0          'NO
        iCombo_Kizushu        '�r��
        iCombo_Grade          '�ڍ׏o��
        iCombo_KaniUmu        '�ȈՏo��
    End Enum

    ' ���ʂ̐F�R�[�h
    Public m_ColorSelection As Color = Color.LightGreen                 '�I���Z����s���̐F�ς�


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                     '���O�Ǘ��N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterAll As SqlClient.SqlDataAdapter = Nothing      '�A�_�v�^�[ ���ʏo�͐ݒ�e�[�u��
    Private m_AdapterCommon As SqlClient.SqlDataAdapter = Nothing   '�A�_�v�^�[ �r���萔�}�X�^�[�e�[�u��
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing     '�A�_�v�^�[ �r��}�X�^�[
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing    '�A�_�v�^�[ �O���[�h�}�X�^�[

    Private m_DsAll As New DataSet                                  '��ڑ��^�e�[�u�� ���ʏo�͐ݒ�e�[�u��
    Private m_DsCommon As New DataSet                               '��ڑ��^�e�[�u�� �r���萔�}�X�^�[�e�[�u��
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

    ''' <summary>
    ''' �C�j�V�����C�Y
    ''' </summary>
    ''' <param name="clsLog"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClassLibrary.tClass.tClass_LogManager)

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        '���O�N���X�Z�b�g
        mcls_Log = clsLog

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
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

        End With
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSetFilter_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '�f�[�^�擾����
        If Not DB_Select() Then
            Exit Sub
        End If

        '�R���{�{�b�N�X�̕\���f�[�^��������
        Data_Init()

        'DataGridView�^�ONO�ݒ�
        dgvShousaiData.Tag = MC_SHOUSAIDATA
        dgvShousaiEtc.Tag = MC_SHOUSAIETC

        '�ڍ�dgv�ݒ�
        dgvInitialize(dgvShousaiData)
        dgvShousaiColunmDefData(dgvShousaiData, True)
        SetCommonEvent(dgvShousaiData)

        '�ڍ�dgv�u��L�ȊO�v
        dgvInitialize(dgvShousaiEtc)
        dgvShousaiColunmDefData(dgvShousaiEtc, False)
        SetCommonEvent(dgvShousaiEtc)

        '�f�[�^�\��
        Data_Disp()

        '�I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        DataGridViewSelectionRowColorChange(dgvShousaiData)
    
        '�ڍ�dgv�u��L�ȊO��CellEnter�̂�
        AddHandler dgvShousaiEtc.CellEnter, AddressOf dgvData_CellEnter

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�̋��ʂ̃C�x���g�n���h���[�̊��t
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub SetCommonEvent(ByVal dgv As DataGridView)

        AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
        AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv_CurrentCellDirtyStateChanged
        AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged

    End Sub

    ''' <summary>
    ''' �I����s�F�ς�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
    ''' <remarks></remarks>
    Private Sub DataGridViewSelectionRowColorChange(ByVal dgv As DataGridView)
        ' ���̊֐��ɃC�x���g�n���h�����t
        AddHandler dgv.RowEnter, AddressOf dgvData_RowEnter
        AddHandler dgv.RowLeave, AddressOf dgvData_RowLeave
        AddHandler dgv.CellEnter, AddressOf dgvData_CellEnter

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView ����֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�I���𖳌���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)

        '�I���𖳌��ɂ���
        CType(sender, DataGridView).CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�s�I��
    ''' </summary>
    Private Sub dgvData_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = m_ColorSelection      '�s�̔w�i��ύX

    End Sub
    
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�s��I��
    ''' </summary>
    Private Sub dgvData_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty           '�s�̔w�i���f�t�H���g�F�ɖ߂�

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�Z���I��
    ''' </summary>
    Private Sub dgvData_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
        End If

    End Sub

    ''' <summary>
    ''' CurrentCellDirtyStateChanged�C�x���g�n���h��
    ''' </summary>
    Private Sub dgv_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ�R�~�b�g����
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub

    ''' <summary>
    ''' ����Z���̒l���ς������F��ύX
    ''' </summary>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) 
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim check_cnt As Integer = 0    ' �`�F�b�N��
        Dim strEdasysID As String

        ' �s�ȊO�̓X���[
        If e.RowIndex < 0 Then Exit Sub

        ' �O���[�h�R���{�{�b�N�X�̐F�ύX
        If CInt(dgv.Tag) = MC_SHOUSAIDATA Or CInt(dgv.Tag) = MC_SHOUSAIETC Then

            '�ڍאݒ莞-------------------------------------------------------------------------------------------------
            If e.ColumnIndex = em_dgvShousaiColum.iCombo_Grade Then
                '�O���[�h�R���{�{�b�N�X�ݒ�ύX
                ' �Z���̒l���擾
                strEdasysID = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

                ' �Z���̒l�iEdasysID�j�ŐF���擾���A�w�i�F��ύX���� 
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
            End If

            If e.ColumnIndex = em_dgvShousaiColum.iCombo_KaniUmu Then
                ' �ȈՏo�͐ݒ�ύX
                If dgv(e.ColumnIndex, e.RowIndex).Value.ToString = "��" Then
                    dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.PaleTurquoise

                Else
                    dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Empty
                End If

            End If

        End If

    End Sub


    ''' <summary>
    ''' �t�@���N�V�����{�^������
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmSetFilter_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

        Select Case FncNo

            Case 0  '�ݒ�

                '�ݒ菈��
                If Not subData_UpDate() Then
                    Exit Sub
                End If

                ''�����@�\�փp�����[�^�ύX�ʒm
                modMain.Send_Mail()

                Me.Close()

            Case 11 '�L�����Z��
                Me.Close()
        End Select

    End Sub

    ''' <summary>
    ''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
    ''' </summary>
    ''' <remarks>�O���[�h���̂Ɉȏ������A�����̍s��ǉ��A</remarks>
    Private Sub Data_Init()
        Dim drowNewType As DataRow      ' �r��}�X�^�[�̐V�K�s
        Dim drowNewGrade As DataRow     ' �r��O���[�h�̐V�K�s
        Dim ii As Integer               ' ���[�v�J�E���^

        Dim nKaniKomoku As Integer      ' ALLCODE_OUTPUT�̊ȈՍ��ڂ��Z�b�g����

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

        '�d���׈ꗗ��ʕ\��_�ȈՍ��ڂ��擾����
        nKaniKomoku = 0
        For ii = 0 To m_DsAll.Tables(0).Rows.Count - 1
            If CInt(m_DsAll.Tables(0).Rows(ii).Item("�d���׈ꗗ��ʕ\��_�ȈՍ���").ToString.Trim) <> 0 Then
                nKaniKomoku = CInt(m_DsAll.Tables(0).Rows(ii).Item("�d���׈ꗗ��ʕ\��_�ȈՍ���").ToString.Trim)
                Exit For
            End If
        Next ii
        ' �O���[�h�ݒ�R���{�{�b�N�X�ɃA�C�e�����Z�b�g
        For ii = 0 To m_DsGrade.Tables(0).Rows.Count - 1
            cmbKaniGrade.Items.Add(New mcls_CmbItem(CInt(m_DsGrade.Tables(0).Rows(ii).Item("EdasysID").ToString),
                                                    m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString))
            '�O���[�h�ݒ�R���{�{�b�N�X�̏����\��
            If CType(cmbKaniGrade.Items(ii), mcls_CmbItem).id = nKaniKomoku Then
                cmbKaniGrade.SelectedIndex = ii

            End If
        Next ii

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
                    clm.Width = 160
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
                    clm.Width = 160
                    clm.Frozen = True
                    clm.ReadOnly = True
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            '�O���[�h
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "�ڍ׏o��"
                clm.Width = 93
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

            '�ȈՏo��
            If nkbn = True Then
                '�ڍ�dgv 
                Using clm As New DataGridViewComboBoxColumn
                    clm.HeaderText = "�ȈՏo��"
                    clm.Width = 93
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = True
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.Items.AddRange("��", "-")
                    clm.MaxDropDownItems = clm.Items.Count
                    .Columns.Add(clm)

                End Using
            Else
                '�ڍ�dgv �u��L�ȊO�v
                Using clm As New DataGridViewTextBoxColumn
                    clm.Width = 93
                    clm.Frozen = True
                    clm.ReadOnly = True
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            If nkbn = False Then
                '�ڍ�dgv�u��L�ȊO�v�͗�w�b�_�[��\�����Ȃ�
                .ColumnHeadersVisible = False        ' ��w�b�_�[��\��
            End If
        End With
    End Sub


    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim drowAllData() As DataRow = m_DsAll.Tables(0).Select("�r��EdasysID>=0")
        Dim drowAllEtc() As DataRow = m_DsAll.Tables(0).Select("�r��EdasysID=-1")
        Dim drowType() As DataRow = m_DsType.Tables(0).Select()
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select()
        Dim drowCommon() As DataRow = m_DsCommon.Tables(0).Select()             '�r���萔�}�X�^�[�e�[�u��

        Dim ii As Integer                                                       '���[�v�J�E���^
        Dim jj As Integer                                                       '���[�v�J�E���^
        Dim nRowCount As Integer = 0                                            '�R���{�\���p�J�E���^

        Dim ndisp_flg As Integer = CInt(drowCommon(0).Item("�d���׈ꗗ���_�t�B���^"))   '�ڍ�<>�ȈՍ��ڕ\���ؑփt���O
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
                        '�ڍ�dgv �ȈՏo�͕\��
                        If m_DsAll.Tables(0).Rows(jj).Item("�d���׈ꗗ��ʕ\��_�ȈՍ���").ToString.Trim <> "0" Then
                            dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value = "��"
                        Else
                            dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value = "-"
                        End If
                        nRowCount = jj + 1
                        Exit For
                    End If
                Next jj
            Else
                dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value = 0 '�ڍ�dgv �r��R���{�{�b�N�X�����\��
                dgvShousaiData(em_dgvShousaiColum.iCombo_Grade, ii).Value = 0   '�ڍ�dgv �O���[�h�R���{�{�b�N�X�����\��
                dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value = "-"  '�ڍ�dgv �Ȉ՗L���\��
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

        dgvShousaiEtc(em_dgvShousaiColum.iCombo_KaniUmu, 0).Value = "-"         '�ڍ�dgv�u��L�ȊO�v�ȈՏo�́i"-"�Œ�j
        '----------------------------------------------------------------------------------------------------------------------

        '�ڍׁA�ȈՐݒ�ؑ�        
        If MC_SHOUSAI = ndisp_flg Then
            '�ڍאݒ�
            radShousai.Checked = True
        Else
            '�ȈՐݒ�
            radKani.Checked = True
        End If

    End Sub


    ''' <summary>
    ''' �f�[�^�x�[�X����l���擾����
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""         'SQL���i�[�G���A
        Dim strErrMsg As String = ""      '�G���[���e�i�[�G���A

        '�e�[�u��"ALLCODE_OUTPUT"�l�擾
        strSQL = "SELECT �r��EdasysID, �d���׈ꗗ��ʕ\��_�ڍ׏���, �d���׈ꗗ��ʕ\��_�ȈՏ���, �d���׈ꗗ��ʕ\��_�ȈՍ���"
        strSQL &= " FROM " & DB_PALL_OUTPUT
        strSQL &= " ORDER BY �r��EdasysID ASC"

        'SQL�����s
        If 0 > Get_SelectData(strSQL, m_DsAll, m_AdapterAll, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_PALL_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '�e�[�u��"COMMON_MASTER"�l�擾
        strSQL = "SELECT LINEID, �d���׈ꗗ���_�t�B���^ "
        strSQL &= " FROM " & DB_COMMON_MASTER

        'SQL�����s
        If 0 > Get_SelectData(strSQL, m_DsCommon, m_AdapterCommon, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '�e�[�u��"TYPE_NAME"�l�擾
        strSQL = "SELECT EdasysID, Name "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        If 0 > Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '�e�[�u��"GRADE_NAME"�l�擾
        strSQL = "SELECT EdasysID, Name, �}�b�v�\���F "
        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY �D��x ASC"

        If 0 > Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        m_DsShousaiType = m_DsType.Copy()      '�擾�����r����ڍ�dgv�p�e�[�u���ɃR�s�[
        m_DsShousaiGrade = m_DsGrade.Copy()    '�擾�����O���[�h���ڍ�dgv�p�e�[�u���ɃR�s�[
        Return True

    End Function

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="strSQL">���s����SQL��</param>
    ''' <param name="DsDataSet">�f�[�^�Z�b�g�I�u�W�F�N�g</param>
    ''' <param name="AdapterData">�f�[�^�A�_�v�^�I�u�W�F�N�g</param>
    ''' <param name="strErrMsg">�G���[���b�Z�[�W</param>
    ''' <returns>�e�����󂯂��s����Ԃ��@-1:���s�@-9:DB�ڑ����s</returns>
    ''' <remarks></remarks>
    Private Function Get_SelectData(ByVal strSQL As String, ByRef DsDataSet As System.Data.DataSet,
                                    ByRef AdapterData As SqlClient.SqlDataAdapter, ByRef strErrMsg As String) As Integer

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        '���b�Z�[�W������
        strErrMsg = ""

        Try

            ' SQL���s
            If Not tcls_DB.Open() Then
                strErrMsg = "DB�ڑ����s"
                Return -9
            End If

            ' �f�[�^�擾
            DsDataSet.Clear()
            AdapterData = tcls_DB.SelectAdapter(strSQL)

            Return AdapterData.Fill(DsDataSet)


        Catch ex As Exception
            strErrMsg = ex.Message
            Return -1

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function


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
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strMsg As String       ' �G���[���b�Z�[�W�p
        Dim ii As Integer          ' ���[�v�J�E���^
        Dim jj As Integer          ' ���[�v�J�E���^

        Dim check_cnt As Integer    '���I��
        Dim bKizuUmu As Boolean     '�r�킪�P���I������ĂȂ��Ƃ�False 1�ł��r�킪�I������Ă�����True
        check_cnt = 0
        bKizuUmu = False

        '�ȈՏo�͂�"��"�I�����Ă鍀�ڐ����`�F�b�N
        For ii = 0 To dgvShousaiData.Rows.Count - 1
            If CStr(dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value) = "��" And
               CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) <> 0 Then
                '�r�킪���I���A�ȈՏo�͂����̍��ڂ���������J�E���g�A�b�v
                check_cnt = check_cnt + 1
            End If
        Next ii

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
                If CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, ii).Value) =
                                    CInt(dgvShousaiData(em_dgvShousaiColum.iCombo_Kizushu, jj).Value) Then
                    strMsg = "�y �r�� �z" & vbNewLine
                    strMsg &= "�����r��͓o�^�ł��܂���B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)

                    '���b�Z�[�W�\��
                    Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�f�[�^�o�^�ُ�")
                        frmMsg.ShowDialog()
                    End Using
                    Return False
                End If
            Next

        Next ii

        '�ȈՏo�͐��𔻒� ������
        If check_cnt > MC_KIZUSHU_NUM Then
            strMsg = "�y �ȈՏo�� �z" & vbNewLine
            strMsg &= "�ȈՏo�͂�" & MC_KIZUSHU_NUM & "�ȏ�ݒ�ł��܂���B" & vbNewLine

            '���b�Z�[�W�\��
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�f�[�^�o�^�ُ�")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        '�ȈՏo�͐��𔻒� ���I��
        If check_cnt <= 0 Or bKizuUmu = False Then
            strMsg = "�y �r�� �z" & vbNewLine
            strMsg &= "�ȈՏo�͂����ݒ�ɂȂ邽�߁A�폜�ł��܂���B" & vbNewLine

            '���b�Z�[�W�\��
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�f�[�^�o�^�ُ�")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        '�ȈՏo�͐��𔻒� ���I��
        If check_cnt <= 0 Then
            strMsg = "�y �ȈՏo�� �z" & vbNewLine
            strMsg &= "�ȈՏo�͂����ݒ�ł��B" & vbNewLine

            '���b�Z�[�W�\��
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�f�[�^�o�^�ُ�")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

        '�ȈՃO���[�h�ݒ肪�I������ĂȂ��Ƃ�(�Ȃ��n�Y)
        If cmbKaniGrade.Text.Trim = "" Then
            strMsg = "�y �ȈՃO���[�h�ݒ� �z" & vbNewLine
            strMsg &= "�ȈՃO���[�h�ݒ肪���ݒ�ł��B" & vbNewLine

            '���b�Z�[�W�\��
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�f�[�^�o�^�ُ�")
                frmMsg.ShowDialog()
            End Using
            Return False
        End If

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

        Dim strWhere2 As String = ""                                '�ȈՃO���[�h���o����

        Dim bShousai_flg As Boolean = False  '�ڍאݒ�Ƀf�[�^���P�ł��I������Ă��邩�ۂ� True=�ݒ肠�� False=�ݒ�Ȃ�
        Dim bKizu_flg As Boolean = False     '�ȈՐݒ���r�킪�P�ł��I������Ă��邩�ۂ� True=�`�F�b�N���� False=�`�F�b�N�Ȃ�
        Dim bGrade_flg As Boolean = False    '�ȈՐݒ�̃O���[�h�P�ł����I������Ă��邩�ۂ� True=�`�F�b�N���� False=�`�F�b�N�Ȃ�

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
            If CStr(dgvShousaiData(em_dgvShousaiColum.iCombo_KaniUmu, ii).Value) = "��" Then
                drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՏ���") = CStr(CType(cmbKaniGrade.Items(cmbKaniGrade.SelectedIndex), mcls_CmbItem).id)
                drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՍ���") = CStr(CType(cmbKaniGrade.Items(cmbKaniGrade.SelectedIndex), mcls_CmbItem).id)
            Else
                drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՏ���") = 0
                drowNew.Item("�d���׈ꗗ��ʕ\��_�ȈՍ���") = 0
            End If
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
    Private Function subData_UpDate() As Boolean

        Dim strMsg As String = ""       ' �G���[���b�Z�[�W�p

        '���͒l�`�F�b�N
        If Not Data_Check() Then Return False

        '��ʂ̃f�[�^���擾
        If Not Data_Get() Then Return False

        'DB������
        If Not DB_UpDate() Then
            strMsg = "�o�^�Ɏ��s"
            '���b�Z�[�W�\��
            Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�菈��")
                frmMsg.ShowDialog()
            End Using
        End If

        Return True

    End Function

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql��
        Dim ndisp_flg As Integer                            '�ڍ�<>�ȈՍ��ڕ\���ؑփt���O
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
                        strSql &= .Item("�d���׈ꗗ��ʕ\��_�ڍ׏���").ToString & ","       ' �d���׈ꗗ��ʕ\��_�ڍ׏���
                        strSql &= .Item("�d���׈ꗗ��ʕ\��_�ȈՏ���").ToString & ","       ' �d���׈ꗗ��ʕ\��_�ȈՏ���
                        strSql &= .Item("�d���׈ꗗ��ʕ\��_�ȈՍ���").ToString & ")"       ' �d���׈ꗗ��ʕ\��_�ȈՍ���

                        'SQL���s
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' �G���[�Ȃ烍�[���o�b�N���ďI��
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next

            '�r���萔�}�X�^�[�e�[�u���̏d���׈ꗗ���_�t�B���^�X�V-----------------------------------------------------------------
            '�g�p�����ʕ\���ݒ�̔���
            If radShousai.Checked = True Then
                '�ڍאݒ��I��
                ndisp_flg = MC_SHOUSAI
            Else
                '�ȈՐݒ��I��
                ndisp_flg = MC_KANI
            End If

            strSql = ""
            strSql &= "UPDATE " & DB_COMMON_MASTER & " SET "
            strSql &= "�d���׈ꗗ���_�t�B���^ = " & ndisp_flg
            'strSql &= " WHERE LINEID=" & KizuLibCLI.KizuMod.GetLineID()
            tClass_SQLServer.sqlColumnTrim(strSql)                  '���܂��Ȃ� �K�������Ă�
            'SQL���s
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�R�~�b�g
            tcls_DB.TransactCommint()

            Return True

        Catch ex As Exception
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            mcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' ��n��
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function


    ''' <summary>
    ''' �\�����@�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radButton_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radShousai.CheckedChanged, radKani.CheckedChanged

        Dim rdoData As RadioButton = CType(sender, RadioButton)

        '���W�I�{�^���w�i�F�F�ς�
        If rdoData.Checked Then
            rdoData.BackColor = Color.Lime
        Else
            rdoData.BackColor = g_ColorControl
        End If

    End Sub
    ''' <summary>
    ''' �ȈՃO���[�h�ݒ�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbKaniGrade_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbKaniGrade.SelectedIndexChanged
        cmbKaniGrade.BackColor = GetCmbColor(CStr(CType(cmbKaniGrade.Items(cmbKaniGrade.SelectedIndex), mcls_CmbItem).id))
    End Sub

End Class
