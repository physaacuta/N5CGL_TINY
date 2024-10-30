'*********************************************************************************
'�@�������FF�N���X
'	[Ver]
'		Ver.01    2015/03/04  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Imports tClassLibrary.tCnt
Public Class frmKensaStatus

    '---------------------------------------------------------------------------
    ' �萔��`
    '---------------------------------------------------------------------------
    Private Const STATUS_MAP_COL As Integer = 20                                    ' �f�[�^�\����(���o���͏���)
    Private Const STATUS_MAP_ROW As Integer = CInt(MAX_FF_STS_CNT / STATUS_MAP_COL) ' �f�[�^�\���s��

    '---------------------------------------------------------------------------
    ' �\����
    '---------------------------------------------------------------------------
    '�\���p������ԍ\����
    Private Structure DSP_CONTROL_MEMBER
        Dim lblHeader_Code As Label         '�`���R�[�h �\:95 ��:96
        Dim lblRec_Code As Label            '�A��(00)
        Dim lblDate As Label                '�`���������� �`�� YYYYMMDHHNNSS
        Dim lblLine_Code As Label           '�r�����C���̍H���R�[�h['81','85','54','82(53)']�ݒ�
        Dim lblMeino As Label               '���ߔԍ�
        Dim lblCoilno As Label              '�≄�R�C���ԍ�
        Dim lblId As Label                  '�r���Ǘ�No
        Dim lbldataSize As Label            '������ԃZ�b�g��
        Dim dgv As tCnt_DataGridView        '�������
    End Structure

    '---------------------------------------------------------------------------
    ' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private m_typ(NUM_MEN - 1) As DSP_CONTROL_MEMBER
    Private m_mode As Integer                           '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strID As String                           'ID

    '***************************************************************************
    ' �t�H�[���C�x���g
    '***************************************************************************
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="mode"></param>
    ''' <param name="strID"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strID As String = "")
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        '�����o�ϐ��Z�b�g
        m_mode = mode                   '�p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
        m_strID = strID                 'ID

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    '***************************************************************************
    ' �t�H�[���C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKensaStatus_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        '�R���g���[���Z�b�g
        Call Set_Control()

        '��ʏ�����
        Call InitDsp()

        'ID���X�g�쐬
        Call MakeIDList()

        If 1 = m_mode Then                      '�v�����[�h(1:�I�����C���p�j
            btnRead.Visible = False
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count > 0 Then
                cmbID.SelectedIndex = 0         '�擪�f�[�^�I��
            End If
        Else                                ' �c�[�����[�h
            btnRead.Visible = True          ' �Ǎ��{�^���u�\���v
        End If

    End Sub

    ''' <summary>
    ''' �R���g���[���Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Set_Control()
        ' �\�p�R���g���[���Z�b�g
        m_typ(0).lblHeader_Code = lblHeader_Code1
        m_typ(0).lblRec_Code = lblRec_Code1
        m_typ(0).lblDate = lblDate1
        m_typ(0).lblLine_Code = lblLine_Code1
        m_typ(0).lblMeino = lblMeino1
        m_typ(0).lblCoilno = lblCoilno1
        m_typ(0).lblId = lblId1
        m_typ(0).lbldataSize = lbldataSize1
        m_typ(0).dgv = dgvStatus1

        ' ���p�R���g���[���Z�b�g
        m_typ(1).lblHeader_Code = lblHeader_Code2
        m_typ(1).lblRec_Code = lblRec_Code2
        m_typ(1).lblDate = lblDate2
        m_typ(1).lblLine_Code = lblLine_Code2
        m_typ(1).lblMeino = lblMeino2
        m_typ(1).lblCoilno = lblCoilno2
        m_typ(1).lblId = lblId2
        m_typ(1).lbldataSize = lbldataSize2
        m_typ(1).dgv = dgvStatus2
    End Sub

    ''' <summary>
    ''' ID���X�g�쐬
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        If 1 = m_mode Then                      '�v�����[�h(1:�I�����C���p�j
            strSQL = ""
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
            strSQL &= " WHERE ID=" & "'" & m_strID & "'"
            strSQL &= " ORDER BY �o�^���t DESC"
        Else                                    '�v�����[�h(0:�c�[���p�j
            strSQL = ""
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
            strSQL &= " ORDER BY �o�^���t DESC"
        End If


        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' �f�[�^�擾
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            If tcls_DB Is Nothing = False Then
                tcls_DB.Cancel()
                If sqlRead Is Nothing = False Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' ��ʏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()
        For Each typ As DSP_CONTROL_MEMBER In m_typ
            typ.lblHeader_Code.Text = ""
            typ.lblRec_Code.Text = ""
            typ.lblDate.Text = ""
            typ.lblLine_Code.Text = ""
            typ.lblMeino.Text = ""
            typ.lblCoilno.Text = ""
            typ.lblId.Text = ""
            typ.lbldataSize.Text = ""
            dgvInitialize(typ.dgv)
        Next

    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' ��ʏI��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '��ʏI��
        Me.Close()

    End Sub

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="typStatus1">������ԃf�[�^</param>
    ''' <param name="typStatus2">������ԃf�[�^</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typStatus1 As FF_STS_JISSEKI, ByRef typStatus2 As FF_STS_JISSEKI) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData1() As Byte  '���`���o�C�i���f�[�^
        Dim bytData2() As Byte  '���`���o�C�i���f�[�^


        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " SELECT "
        strSQL &= "        ���`��3,"
        strSQL &= "        ���`��4 "
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY �o�^���t DESC"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' �f�[�^�擾
            sqlRead.Read()

            '���`��
            bytData1 = CType(sqlRead.GetValue(0), Byte())

            '���`��
            bytData2 = CType(sqlRead.GetValue(1), Byte())

            tMod.MemSet(bytData1, 0, typStatus1, tMod.SizeOf(typStatus1))
            tMod.MemSet(bytData2, 0, typStatus2, tMod.SizeOf(typStatus2))


        Catch ex As Exception
            Return False
        Finally
            If tcls_DB Is Nothing = False Then
                tcls_DB.Cancel()
                If sqlRead Is Nothing = False Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try

        Return True

    End Function


    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '' ��{��ݒ�
        'Call dgv.dgvInitialize(dgv)
        'Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                                         ' �Z���G���[�̕\��
            .ShowCellToolTips = False                                       ' �c�[���q���g�̕\��
            .ShowEditingIcon = False                                        ' �ҏW���̃Z���̖���
            .ShowRowErrors = False                                          ' �G���[�s�̖���
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��
            '.ColumnHeadersHeight = 42										' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            .RowCount = STATUS_MAP_ROW


            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = STATUS_MAP_COL + 1           '
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' �w�b�_�[�̍���

            '��0 (�A�h���X) ����ʒu
            .Columns(0).HeaderText = "�ʒu[m]"
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(0).Width = 58
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '��1�` (�f�[�^)
            For ii As Integer = 1 To STATUS_MAP_COL
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).Width = 27
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).HeaderText = CStr(ii)
            Next ii
        End With
    End Sub

    ''' <summary>
    ''' ID���X�g�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub

    ''' <summary>
    ''' �ǂݍ��݃{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typStatus(NUM_MEN - 1) As FF_STS_JISSEKI
        Try
            '�f�[�^�\��
            If GetData(cmbID.Text, typStatus(0), typStatus(1)) Then
                For ii As Integer = 0 To NUM_MEN - 1
                    If Not SetData(m_typ(ii), typStatus(ii)) Then
                        Call InitDsp()
                        Exit For
                    End If
                Next
            Else
                Call InitDsp()
                MsgBox("�f�[�^�̎擾�Ɏ��s���܂����B")
            End If
        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    ''' <param name="typ">�R���g���[��</param>
    ''' <param name="typStatus">������ԃf�[�^</param>
    ''' <remarks></remarks>
    Private Function SetData(ByVal typ As DSP_CONTROL_MEMBER, ByVal typStatus As FF_STS_JISSEKI) As Boolean

        Dim ii As Integer   '���[�v�J�E���^
        Dim jj As Integer   '���[�v�J�E���^
        Dim nPos As Integer
        Try
            typ.lblHeader_Code.Text = tMod.ByteToString(typStatus.Hd.Header_Code)
            typ.lblRec_Code.Text = tMod.ByteToString(typStatus.Hd.Rec_Code)
            typ.lblDate.Text = tMod.ByteToString(typStatus.Hd.bytDate)
            typ.lblLine_Code.Text = tMod.ByteToString(typStatus.Hd.Line_Code)
            typ.lblRec_Code.Text = tMod.ByteToString(typStatus.Hd.Rec_Code)
            typ.lblMeino.Text = tMod.ByteToString(typStatus.Hd.Meino)
            typ.lblCoilno.Text = tMod.ByteToString(typStatus.Hd.Coilno)
            typ.lblId.Text = tMod.ByteToString(typStatus.Dt.Id)
            typ.lbldataSize.Text = tMod.ByteToString(typStatus.Dt.dataSize)

            ' �������
            For ii = 0 To STATUS_MAP_ROW - 1
                'typ.dgv(0, ii).Value = CStr(ii * 20) & "m"
                typ.dgv(0, ii).Value = CStr(ii * STATUS_MAP_COL)
                For jj = 0 To STATUS_MAP_COL - 1
                    nPos = ii * STATUS_MAP_COL + jj
                    If nPos > MAX_FF_STS_CNT - 1 Then Exit For
                    typ.dgv(jj + 1, ii).Value = tMod.ByteToString(typStatus.Dt.data(nPos).status)
                Next
            Next
            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function

End Class
