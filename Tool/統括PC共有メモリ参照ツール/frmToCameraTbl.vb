Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

''' <summary>
''' �J�����R���g���[�����
''' </summary>
''' <remarks></remarks>
Public Class frmToCameraTbl
    '///////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////
    Private m_typToCameraTbl As TO_CAMERA_TBL
    Private WithEvents m_timer1 As New Timer
    Private WithEvents m_timer2 As New Timer    ' ���L�������A�N�Z�X�E�s�m�F�^�C�}�[

    '///////////////////////////////////////////////////////
    '�����o�[�֐�
    '///////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�O���b�h������
    ''' </summary>
    ''' <param name="dgvCtrl"></param>
    ''' <remarks></remarks>
    Private Sub InitdgvCamInf(ByVal dgvCtrl As tCnt_DataGridView)
        '��{�ݒ�
        dgvCtrl.dgvInitialize(dgvCtrl)

        With dgvCtrl
            '�X�N���[���o�[�̕\��
            .ScrollBars = ScrollBars.Both

            '�I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '�s�ݒ�
            .RowCount = NUM_CAMERA
            .RowTemplate.Height = 20

            '��ݒ�
            .ColumnCount = 3
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 40
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0, 1, 2
                        .Columns(ii).Width = 120
                    Case Else
                        .Columns(ii).Width = 80
                End Select
            Next ii
            .Columns(0).ReadOnly = True
            .Columns(0).Frozen = True

            .Columns(0).HeaderText = "��׾��/�p�x��"
            .Columns(1).HeaderText = "���ݘI������"
            .Columns(2).HeaderText = "���ݏ��"
        End With
    End Sub

    ''' <summary>
    ''' �J�����R���g���[���ݒ���ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadToCameraTbl()
        Dim strTmp As String

        Try
            Dim nRetc As ValueType = 0                          '���^�[���R�[�h

            '�ڍ׏��ǂݍ���
            m_typToCameraTbl = CType(ksMemRead(TBL_TO_CAMERA_NAME, 0, GetType(TO_CAMERA_TBL), nRetc), TO_CAMERA_TBL)
        Catch ex As Exception
            strTmp = String.Format("��׺��۰אݒ���ǂݍ��ُ݈�[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' �J�����R���g���[���ݒ��񏑂�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub WriteCamInf()
        Dim strTmp As String

        Try
            '���L��������������
            If Not ksMemWrite(TBL_TO_CAMERA_NAME, m_typToCameraTbl) Then
                strTmp = String.Format("���L�������������ُ݈�[{0}]", TBL_TO_CAMERA_NAME)
                MsgBox(strTmp)
            End If
        Catch ex As Exception
            strTmp = String.Format("��׺��۰אݒ��񏑂����ُ݈�[{0}]", ex.Message)
            MsgBox(strTmp)
        End Try
    End Sub

    ''' <summary>
    ''' �J�����ݒ���\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspCamInf()
        Try

            For nRow As Integer = 0 To NUM_CAMERA - 1

                For nCol As Integer = 0 To dgvCamInf.ColumnCount - 1
                    dgvCamInf(nCol, nRow).Style.BackColor = Color.White
                Next
                With m_typToCameraTbl.caminf(nRow)
                    dgvCamInf(0, nRow).Value = "�J����" & CStr(nRow + 1)
                    dgvCamInf(1, nRow).Value = CInt(.nExpTime)
                    dgvCamInf(2, nRow).Value = CInt(.nMode)
                End With
            Next

        Catch ex As Exception
            MsgBox("�J�����ݒ���\���G���[")
        End Try


    End Sub


    '///////////////////////////////////////////////////////////////////////////
    '�C�x���g�֐�
    '///////////////////////////////////////////////////////////////////////////
    Private Sub frmToCameraTbl_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me.Cursor = Cursors.WaitCursor

        ' ���L�������ɃA�N�Z�X�\���`�F�b�N�^�C�}�[
        m_timer2.Interval = 100         ' 100msec
        m_timer2.Start()

        '������
        InitdgvCamInf(dgvCamInf)

        '�J�����R���g���[���ݒ�����ǂݍ���
        ReadToCameraTbl()

        '�J�����ݒ���\��
        DspCamInf()

        ' �����X�V��L���ɂ���E�E�E�K�v�Ȃ��ꍇ�̓R�����g�ɂ���
        chkUpdate.Checked = True

        ' ���L�������T�C�Y���\��
        Dim str As String = ""
        str &= String.Format("TO_CAMERA_TBL Size = {0}", Marshal.SizeOf(GetType(TO_CAMERA_TBL))) & vbCrLf
        txtInfo.Text = str

        Me.Cursor = Cursors.Default
        Me.CenterToScreen()
    End Sub


    ''' <summary>
    ''' �}�b�v�ʒu�ǂݍ��݃{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        'Me.Cursor = Cursors.WaitCursor
        ReadToCameraTbl()
        '�\��
        DspCamInf()
        'Me.Cursor = Cursors.Default
    End Sub

    ''' <summary>
    ''' �������݃{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click
        Me.Cursor = Cursors.WaitCursor
        '��������
        WriteCamInf()
        Me.Cursor = Cursors.Default
    End Sub


    Private Sub dgvCamInf_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvCamInf.CellValueChanged
        Dim nCol As Integer = e.ColumnIndex
        Dim nRow As Integer = e.RowIndex

        If nRow < 0 OrElse nCol < 0 Then
            Exit Sub
        End If

        If dgvCamInf(nCol, nRow).Value Is Nothing Then
            dgvCamInf(nCol, nRow).Value = 0
        End If

        With m_typToCameraTbl.caminf(nRow)
            Select Case e.ColumnIndex
                Case 1
                    .nExpTime = CInt(dgvCamInf(nCol, nRow).Value)
                Case 2
                    .nMode = CInt(dgvCamInf(nCol, nRow).Value)
            End Select
        End With

        DspCamInf()
    End Sub

    Private Sub chkUpdate_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkUpdate.CheckedChanged
        Dim chk As CheckBox = CType(sender, CheckBox)

        If chk.Checked Then
            ' �`�F�b�N���ꂽ��
            chk.BackColor = Color.Cyan
            m_timer1.Interval = 500
            m_timer1.Start()
        Else
            ' �`�F�b�N���������ꂽ��
            chk.BackColor = SystemColors.Control
            m_timer1.Stop()
        End If
    End Sub

    ''' <summary>
    ''' �����X�V����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_timer1_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer1.Tick
        btnRead.PerformClick()
    End Sub


    Private Sub frmToCameraTbl_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub

    ''' <summary>
    ''' ���L�������A�N�Z�X�\���`�F�b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_timer2_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer2.Tick
        ' �Ώۂ̋��L�����������I�ɓǂݍ��݂��Ă݂āA�A�N�Z�X�ł��邩�ǂ����m�F����B
        ' ���܂�傫�ȋ��L��������ǂݍ��ނƕ��ׂɂȂ�̂Œ���
        Dim nRetc As ValueType = 0
        Dim tmp As TO_CAMERA_TBL = CType(ksMemRead(TBL_TO_CAMERA_NAME, 0, GetType(TO_CAMERA_TBL), nRetc), TO_CAMERA_TBL)
        If CBool(nRetc) Then
            ' ���L�������ɃA�N�Z�X�\
            DspShmemStatus(True)
            ' �����X�V��OFF�ɂȂ��Ă���ꍇ�� �ǂݍ��݁E�������݃{�^����L���ɂ���
            If Not chkUpdate.Checked Then
                btnRead.Enabled = True
                btnWrite.Enabled = True
            End If
        Else
            ' ���L�������ɃA�N�Z�X�ł��Ȃ�
            DspShmemStatus(False)
            ' �ǂݍ��݁E�������݃{�^���𖳌��ɂ���
            btnRead.Enabled = False
            btnWrite.Enabled = False
        End If
    End Sub

    ''' <summary>
    ''' ���L���������ǂݍ��߂�E�ǂݍ��߂Ȃ����^�C�g���̔w�i�J���[�ŕ\��
    ''' </summary>
    ''' <param name="flg">true:�Ǎ��� False:�Ǎ��s��</param>
    ''' <remarks>�Ǎ���:LimeGreen �Ǎ��s��:Red</remarks>
    Private Sub DspShmemStatus(ByVal flg As Boolean)
        If flg Then
            ' �ǂݍ��݉\
            txtTitle.BackColor = Color.LimeGreen
        Else
            txtTitle.BackColor = Color.Red
        End If
    End Sub
End Class
