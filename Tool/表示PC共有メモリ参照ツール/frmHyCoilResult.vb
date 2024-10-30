Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyCoilResult
    '///////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////
    Private m_typHyCoilResult As TBL_HY_COILRESULT
    Private WithEvents m_timer1 As New Timer    ' �\�������X�V�p�^�C�}�[
    Private WithEvents m_timer2 As New Timer    ' ���L�������A�N�Z�X�E�s�m�F�^�C�}�[

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmHyCoilOutInf_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' �\�������X�V�^�C�}�[�����ݒ�
        m_timer1.Interval = 100         ' 100msec

        ' ���L�������ɃA�N�Z�X�\���`�F�b�N�^�C�}�[
        m_timer2.Interval = 100         ' 100msec
        m_timer2.Start()

        '���R�[�h�R���{�{�b�N�X������
        cmbRecord.Items.Clear()
        For ii As Integer = 0 To TBL_HY_COIL_MAX - 1
            cmbRecord.Items.Add(CStr(ii + 1))
        Next
        cmbRecord.SelectedIndex = 0

        ' dgv�̏�����
        InitDgv(dgvCoilResult, 4)                       ' �R�C������
        InitDgv_CutInf(dgvCutInf, MAX_SHACUT_COUNT)     ' �J�b�g���

        ' �\�����e�N���A
        DispInit()

        ' ���L�������̑S�̂�ǂݍ���
        If ReadFromShmem() Then
            ' ���L�������̓��e��\��
            DispShmem()
        End If

        ' �����X�V��L���ɂ���E�E�E�K�v�Ȃ��ꍇ�̓R�����g�ɂ���
        chkUpdate.Checked = True

        ' ���L�������T�C�Y���\��
        Dim str As String = ""
        str = String.Format("TO_SO_DATA_CUT Size = {0}", Marshal.SizeOf(GetType(TO_SO_DATA_CUT))) & vbCrLf
        str &= String.Format("COILRESULT_REC Size = {0}", Marshal.SizeOf(GetType(COILRESULT_REC))) & vbCrLf
        str &= String.Format("TBL_HY_COILRESULT Size = {0}", Marshal.SizeOf(GetType(TBL_HY_COILRESULT))) & vbCrLf

        txtInfo.Text = str

        txtTitle.Select()

    End Sub

    ''' <summary>
    ''' �����X�V����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_timer1_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer1.Tick
        ' ���L�������̑S�̂�ǂݍ���
        If ReadFromShmem() Then
            ' ���L�������̓��e��\��
            DispShmem()
        End If
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
        Dim tmp As COINFO_BASE = CType(ksMemRead(TBL_HY_COILINF_NAME, 0, GetType(COINFO_BASE), nRetc), COINFO_BASE)
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
    ''' DataGridView������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgv(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

        '��{�ݒ�
        dgv.dgvInitialize(dgv)

        With dgv
            '�X�N���[���o�[�̕\��
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect

            '�I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '�s�ݒ�
            .RowCount = RowCount
            .RowTemplate.Height = 20

            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue         ' �I��
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�

            '��ݒ�
            .ColumnCount = 3
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0
                        .Columns(ii).Width = 160
                    Case 1
                        .Columns(ii).Width = 140
                    Case 2
                        .Columns(ii).Width = 140
                End Select
            Next ii
            .Columns(0).ReadOnly = True
            .Columns(0).Frozen = True
            .Columns(2).ReadOnly = True
            .Columns(2).Visible = False

            .Columns(0).HeaderText = "����"
            .Columns(1).HeaderText = "�l"
            .Columns(2).HeaderText = "�ϐ���"

        End With

    End Sub

    ''' <summary>
    ''' DataGridView������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgv_CutInf(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

        '��{�ݒ�
        dgv.dgvInitialize(dgv)

        With dgv
            '�X�N���[���o�[�̕\��
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect

            '�I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '�s�ݒ�
            .RowCount = RowCount
            .RowTemplate.Height = 20

            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue         ' �I��
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�

            '��ݒ�
            .ColumnCount = 2
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case 0
                        .Columns(ii).Width = 50
                    Case 1
                        .Columns(ii).Width = 280
                End Select
            Next ii

            .Columns(0).HeaderText = "�敪"
            .Columns(1).HeaderText = "�ʒu"

        End With

    End Sub

    ''' <summary>
    ''' �����X�V�̃`�F�b�N�{�b�N�X���ω��������̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkUpdate_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkUpdate.CheckedChanged
        Dim chk As CheckBox = CType(sender, CheckBox)

        If chk.Checked Then
            ' �`�F�b�N���ꂽ��
            chk.BackColor = Color.Cyan
            m_timer1.Start()

            ' �ǂݍ��݁E�������݃{�^���𖳌��ɂ���
            btnRead.Enabled = False
            btnWrite.Enabled = False

        Else
            ' �`�F�b�N���������ꂽ��
            chk.BackColor = SystemColors.Control
            m_timer1.Stop()

            If txtTitle.BackColor <> Color.Red Then
                ' ���L�������A�N�Z�X�\�ȏꍇ
                btnRead.Enabled = True
                btnWrite.Enabled = True
            End If
        End If
    End Sub

    ''' <summary>
    ''' ���R�[�h �R���{�{�b�N�X�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbRecord_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbRecord.SelectedIndexChanged
        ' �ǂݍ��ݏ���
        btnRead.PerformClick()
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

    ''' <summary>
    ''' �ǂݍ��݃{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        ' ���L�������̑S�̂�ǂݍ���
        If ReadFromShmem() Then
            ' ���L�������̓��e��\��
            DispShmem()
        End If

    End Sub

    ''' <summary>
    ''' �������݃{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click

        ' ���L�������̑S�̂�ǂݍ���
        If ReadFromShmem() Then
            ' dgv����f�[�^��ǂݍ���
            ReadFromDgv()

            ' ���L�������ɏ�������
            WriteToShmem()

            ' ���L�������̑S�̂�ǂݍ���
            If ReadFromShmem() Then
                ' ���L�������̓��e��\��
                DispShmem()
            Else
                ' ������Ԃ�\��
                ' DispInit()
            End If

        End If

    End Sub

    ''' <summary>
    ''' ���L����������ǂݍ���
    ''' </summary>
    ''' <returns>true:�ǂݍ��ݐ��� False:�ǂݍ��ݎ��s</returns>
    ''' <remarks>���L��������m_typHyCoilInf</remarks>
    Private Function ReadFromShmem() As Boolean
        Dim bRet As Boolean = False

        Try
            m_typHyCoilResult = CType(ksMemRead(TBL_HY_COILRESULT_NAME, GetType(TBL_HY_COILRESULT)), TBL_HY_COILRESULT)

            If m_typHyCoilResult.rec Is Nothing Then
                ' ���L�������̓ǂݍ��݂Ɏ��s���Ă���ꍇ�� rec �� nothing �ɂȂ�
                bRet = False
                DspShmemStatus(False)
            Else
                bRet = True
                DspShmemStatus(True)
            End If
        Catch ex As Exception
            ' ���L�������ǂݍ��ݎ��s
            DspShmemStatus(False)
            Dim str As String = txtInfo.Text
            str &= String.Format("ReadFromShmem Error [{0}]", ex.Message)
            txtInfo.Text = str
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' ���L�������ɏ�������
    ''' </summary>
    ''' <returns>true:�������ݐ��� false:�������ݎ��s</returns>
    ''' <remarks>m_typHyCoilInf�ˋ��L������</remarks>
    Private Function WriteToShmem() As Boolean
        Dim bRet As Boolean = False

        Try
            If Not ksMemWrite(TBL_HY_COILRESULT_NAME, m_typHyCoilResult) Then
                bRet = False
                DspShmemStatus(False)
            Else
                bRet = True
                DspShmemStatus(True)
            End If
        Catch ex As Exception
            bRet = False
            DspShmemStatus(False)
            Dim str As String = txtInfo.Text
            str &= String.Format("WriteToShmem Error [{0}]", ex.Message)
            txtInfo.Text = str
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �R�C�����т�\���E�ǂݍ���
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:�\�� false:�Ǎ�</param>	
    ''' <remarks></remarks>
    Private Sub CoilBaseData(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

        ' �Ώۂ�dgv���Z�b�g
        Dim dgv As tCnt_DataGridView = dgvCoilResult

        With m_typHyCoilResult.rec(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' �Ӄ֎�����
                            If init Then dgv(0, ii).Value = "�Ӄ֎�����" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nLen _
                            Else .nLen = CInt(dgv(1, ii).Value)
                        Case 1  ' �J�b�g���Z�b�g��
                            If init Then dgv(0, ii).Value = "�J�b�g���Z�b�g��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutCnt _
                            Else .nCutCnt = CInt(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�R�C������{0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
        End With
    End Sub

    ''' <summary>
    ''' �J�b�g����\���E�ǂݍ���
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:�\�� false:�Ǎ�</param>	
    ''' <remarks></remarks>
    Private Sub CountInf(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        If m_typHyCoilResult.rec Is Nothing Then
            Exit Sub
        End If

        ' �Ώۂ�dgv���Z�b�g
        Dim dgv As tCnt_DataGridView = dgvCutInf

        With m_typHyCoilResult.rec(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    If init Then
                        dgv(0, ii).Value = ""
                        dgv(1, ii).Value = ""
                    ElseIf flg Then
                        dgv(0, ii).Value = .cut(ii).nMode
                        dgv(1, ii).Value = .cut(ii).nPos
                    Else
                        .cut(ii).nMode = CInt(dgv(0, ii).Value)
                        .cut(ii).nPos = CInt(dgv(1, ii).Value)
                    End If

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�J�b�g���{0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
        End With
    End Sub


    ''' <summary>
    ''' �\��������������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispInit()
        ' �V�X�e�����ʂ̃R�C�����
        CoilBaseData(0, True, True)
        ' �J�E���g���
        CountInf(0, True, True)
    End Sub

    ''' <summary>
    ''' dgv����f�[�^��ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
        Dim rec As Integer = cmbRecord.SelectedIndex

        ' �V�X�e�����ʂ̃R�C�����
        CoilBaseData(rec, False, False)
        ' �J�E���g���
        CountInf(rec, False, False)

    End Sub

    ''' <summary>
    ''' ���L�������̓��e��\������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispShmem()
        Dim rec As Integer = cmbRecord.SelectedIndex

        ' �V�X�e�����ʂ̃R�C�����
        CoilBaseData(rec, False, True)
        ' �J�E���g���
        CountInf(rec, False, True)

    End Sub

    Private Sub frmHyCoilResult_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class