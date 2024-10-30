Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyDefMap
    '///////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////
    Private m_typHyDefMap As TBL_HY_DEFMAP

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
        InitDgv(dgvMapInf, 8)

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
        str = String.Format("TBL_HY_DEFMAP Size = {0}", Marshal.SizeOf(GetType(TBL_HY_DEFMAP))) & vbCrLf
        str &= String.Format("DSP_MAP_HEADER Size = {0}", Marshal.SizeOf(GetType(DSP_MAP_HEADER))) & vbCrLf
        str &= String.Format("DSP_MAP_DATA Size = {0}", Marshal.SizeOf(GetType(DSP_MAP_DATA))) & vbCrLf
        str &= String.Format("DSP_MAP_INF Size = {0}", Marshal.SizeOf(GetType(DSP_MAP_INF))) & vbCrLf

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
        Dim tmp As DSP_MAP_HEADER = CType(ksMemRead(TBL_HY_DEFMAP_NAME, 0, GetType(DSP_MAP_HEADER), nRetc), DSP_MAP_HEADER)
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
    ''' <remarks></remarks>
    Private Function ReadFromShmem() As Boolean
        Dim bRet As Boolean = False

        Try
            m_typHyDefMap = CType(ksMemRead(TBL_HY_DEFMAP_NAME, GetType(TBL_HY_DEFMAP)), TBL_HY_DEFMAP)

            If m_typHyDefMap.rec.data.defect Is Nothing Then
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
    ''' <remarks></remarks>
    Private Function WriteToShmem() As Boolean
        Dim bRet As Boolean = False

        Try
            If Not ksMemWrite(TBL_HY_DEFMAP_NAME, m_typHyDefMap) Then
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
    ''' �r�}�b�v����\���E�ǂݍ���
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:�\�� false:�Ǎ�</param>	
    ''' <remarks></remarks>
    Private Sub DspMapInf(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)
        ' �Ώۂ�dgv���Z�b�g
        Dim dgv As tCnt_DataGridView = dgvMapInf

        With m_typHyDefMap.rec.data.defect(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0
                            If init Then dgv(0, ii).Value = "�Ǘ�No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cKanriNo) _
                            Else tMod.StringToByte(.cKanriNo, CStr(dgv(1, ii).Value), .cKanriNo.Length)
                        Case 1
                            If init Then dgv(0, ii).Value = "�v���R�����ѓ`��ID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cID) _
                            Else tMod.StringToByte(.cID, CStr(dgv(1, ii).Value), .cID.Length)
                        Case 2
                            If init Then dgv(0, ii).Value = "�o���J�b�g����" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCutNo.ToString _
                            Else .nCutNo = CInt(dgv(1, ii).Value)
                        Case 3
                            If init Then dgv(0, ii).Value = "�̐��J�n�ʒu[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCut_start.ToString _
                            Else .nCut_start = CInt(dgv(1, ii).Value)
                        Case 4
                            If init Then dgv(0, ii).Value = "�̐��I���ʒu[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nCut_end.ToString _
                            Else .nCut_end = CInt(dgv(1, ii).Value)
                        Case 5
                            If init Then dgv(0, ii).Value = "�␳��[10^2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dHosei.ToString _
                            Else .dHosei = CSng(dgv(1, ii).Value)
                        Case 6
                            If init Then dgv(0, ii).Value = "�␳��_�o��[10^2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dHosei_out.ToString _
                            Else .dHosei_out = CSng(dgv(1, ii).Value)
                        Case 7
                            If init Then dgv(0, ii).Value = "�␳��_����[10^2%]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dHosei_ana.ToString _
                            Else .dHosei_ana = CSng(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�r�}�b�v���{0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
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
        txtRec.Text = ""

        DspMapInf(0, True, True)
    End Sub

    ''' <summary>
    ''' dgv����f�[�^��ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
        Dim rec As Integer = cmbRecord.SelectedIndex

        If IsNumeric(txtRec.Text) Then
            m_typHyDefMap.rec.head.nPos = CInt(txtRec.Text)
        End If

        DspMapInf(rec, False, False)
    End Sub

    ''' <summary>
    ''' ���L�������̓��e��\������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispShmem()
        Dim rec As Integer = cmbRecord.SelectedIndex

        txtRec.Text = m_typHyDefMap.rec.head.nPos.ToString

        DspMapInf(rec, False, True)
    End Sub

End Class