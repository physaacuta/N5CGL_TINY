Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyDefList
	'///////////////////////////////////////////////////////
	'�����o�[�ϐ�
	'///////////////////////////////////////////////////////
	Private m_typHyDeflist As TBL_HY_DEFLIST

	Private WithEvents m_timer1 As New Timer	' �\�������X�V�p�^�C�}�[
	Private WithEvents m_timer2 As New Timer	' ���L�������A�N�Z�X�E�s�m�F�^�C�}�[

	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmHyDefList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' �\�������X�V�^�C�}�[�����ݒ�
		m_timer1.Interval = 100			' 100msec

		' ���L�������ɃA�N�Z�X�\���`�F�b�N�^�C�}�[
		m_timer2.Interval = 100			' 100msec
		m_timer2.Start()

		' �|�C���^�[ nud �����l�Z�b�g
		nudRec_O.Maximum = HY_DEFLIST_REC_MAX
		nudRec_O.Minimum = 1
		nudRec_O.Value = 1

		nudRec_U.Maximum = HY_DEFLIST_REC_MAX
		nudRec_U.Minimum = 1
        nudRec_U.Value = 1

        ' dgv�̏�����
        InitDgv(dgvDeflist_o, 10)	   ' �r�ꗗ���\��
		InitDgv(dgvDeflist_u, 10)      ' �r�ꗗ��񗠖�
        InitDgvdefect(dgvDefect_o, 61)      ' �r�ꗗ���\��
        InitDgvdefect(dgvDefect_u, 61)      ' �r�ꗗ���\��
        InitDgvPath(dgvPath_o, 4)
        InitDgvPath(dgvPath_u, 4)

        ' �\�����e�N���A
        'DispInit()

        ' ���L�������̑S�̂�ǂݍ���
        If ReadFromShmem() Then
            DispInit()
            ' ���L�������̓��e��\��
            DispShmem()
        End If

        ' �����X�V��L���ɂ���E�E�E�K�v�Ȃ��ꍇ�̓R�����g�ɂ���
        chkUpdate.Checked = True

		' ���L�������T�C�Y���\��
		Dim str As String = ""
		str = String.Format("TBL_HY_DEFLIST Size = {0}", Marshal.SizeOf(GetType(TBL_HY_DEFLIST))) & vbCrLf
		str &= String.Format("DSP_LIST_HEADER Size = {0}", Marshal.SizeOf(GetType(DSP_LIST_HEADER))) & vbCrLf
        str &= String.Format("DSP_LIST_DEFECT Size = {0}", Marshal.SizeOf(GetType(DSP_LIST_DEFECT))) & vbCrLf
        str &= String.Format("DSP_LIST_DATA Size = {0}", Marshal.SizeOf(GetType(DSP_LIST_DATA))) & vbCrLf
		str &= String.Format("HY_DEFLIST_REC_MAX = {0}", HY_DEFLIST_REC_MAX) & vbCrLf
		txtInfo.Text = str
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
            Dim nRec_o As Integer = m_typHyDeflist.rec.head.nPos(0)
            Dim nRec_u As Integer = m_typHyDeflist.rec.head.nPos(1)

            If nRec_o <> 0 Then nudRec_O.Value = nRec_o Else nudRec_O.Value = 1
			If nRec_u <> 0 Then nudRec_U.Value = nRec_u Else nudRec_U.Value = 1


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
		Dim tmp As TBL_HY_DEFLIST = CType(ksMemRead(TBL_HY_DEFLIST_NAME, 0, GetType(TBL_HY_DEFLIST), nRetc), TBL_HY_DEFLIST)
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
            .RowTemplate.Height = 16
            .RowCount = RowCount

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
                        .Columns(ii).Width = 140
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
    ''' �|�C���^�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudPoint_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudRec_O.ValueChanged, nudRec_U.ValueChanged
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
		If ReadFromShmemAll() Then
			' dgv����f�[�^��ǂݍ��� dgv->m_typHyDeflist
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
		Dim nRetc As ValueType = 0

		Try
			m_typHyDeflist = CType(ksMemRead(TBL_HY_DEFLIST_NAME, 0, GetType(TBL_HY_DEFLIST), nRetc), TBL_HY_DEFLIST)
			If Not CBool(nRetc) Then
				' �ǂݍ��ݎ��s
				DspShmemStatus(False)
				Exit Try
			End If

			bRet = True
			DspShmemStatus(True)

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
	''' ���L�������S�̂�ǂݍ���
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function ReadFromShmemAll() As Boolean
		Dim bRet As Boolean = False
		Dim nRetc As ValueType = 0

		Try
			m_typHyDeflist = CType(ksMemRead(TBL_HY_DEFLIST_NAME, 0, GetType(TBL_HY_DEFLIST), nRetc), TBL_HY_DEFLIST)
			If Not CBool(nRetc) Then
				' �ǂݍ��ݎ��s
				DspShmemStatus(False)
				Exit Try
			End If

			bRet = True
			DspShmemStatus(True)

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
			If Not ksMemWrite(TBL_HY_DEFLIST_NAME, m_typHyDeflist) Then
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
	''' �r�ꗗ��� �\�ʂ�\���E�ǂݍ���
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:�\�� false:�Ǎ�</param>	
	''' <remarks></remarks>
	Private Sub Deflist_o(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

		' �Ώۂ�dgv���Z�b�g
		Dim dgv As tCnt_DataGridView = dgvDeflist_o

        With m_typHyDeflist.rec.data(0).DfData(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' �Ǘ�No
                            If init Then dgv(0, ii).Value = "�Ǘ�No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cKanriNo _
                            Else .cKanriNo = CStr(dgv(1, ii).Value)
                        Case 1  ' �R�C��No
                            If init Then dgv(0, ii).Value = "�R�C��No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cCoilNo) _
                            Else tMod.StringToByte(.cCoilNo, CStr(dgv(1, ii).Value), .cCoilNo.Length)
                        Case 2  ' ����No
                            If init Then dgv(0, ii).Value = "����No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cMeiNo) _
                            Else tMod.StringToByte(.cMeiNo, CStr(dgv(1, ii).Value), .cMeiNo.Length)
                        Case 3  ' �\��
                            If init Then dgv(0, ii).Value = "�\��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cYobi1 _
                            Else .cYobi1 = CStr(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�r�ꗗ��� �\��{0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
                    txtInfo.Text = str
                End Try
            Next
        End With
    End Sub

	''' <summary>
	''' �r�ꗗ��� �\�ʂ�\���E�ǂݍ���
	''' </summary>
	''' <param name="rec"></param>
	''' <param name="flg">true:�\�� false:�Ǎ�</param>	
	''' <remarks></remarks>
	Private Sub Deflist_u(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

		' �Ώۂ�dgv���Z�b�g
		Dim dgv As tCnt_DataGridView = dgvDeflist_u

        With m_typHyDeflist.rec.data(1).DfData(rec)
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' �Ǘ�No
                            If init Then dgv(0, ii).Value = "�Ǘ�No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cKanriNo _
                            Else .cKanriNo = CStr(dgv(1, ii).Value)
                        Case 1  ' �R�C��No
                            If init Then dgv(0, ii).Value = "�R�C��No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cCoilNo) _
                            Else tMod.StringToByte(.cCoilNo, CStr(dgv(1, ii).Value), .cCoilNo.Length)
                        Case 2  ' ����No
                            If init Then dgv(0, ii).Value = "����No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = tMod.ByteToString(.cMeiNo) _
                            Else tMod.StringToByte(.cMeiNo, CStr(dgv(1, ii).Value), .cMeiNo.Length)
                        Case 3  ' �\��
                            If init Then dgv(0, ii).Value = "�\��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cYobi1 _
                            Else .cYobi1 = CStr(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�r�ꗗ��� �\��{0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
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
		' �ŐV���R�[�h�ʒu
		txtRec_o.Text = ""
		txtRec_u.Text = ""
		' �r�ꗗ���\��
		Deflist_o(0, True, True)
        ' �r�ꗗ��񗠖�
        Deflist_u(0, True, True)

        DefectInf(dgvDefect_o, 0, 0, 0, True, True)
        DefectInf(dgvDefect_u, 1, 0, 0, True, True)

        path(dgvPath_o, 0, 0, True, True)
        path(dgvPath_u, 0, 0, True, True)
    End Sub

    ''' <summary>
    ''' dgv����f�[�^��ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
		Dim rec_o As Integer = CInt(nudRec_O.Value) - 1
		Dim rec_u As Integer = CInt(nudRec_U.Value) - 1

        ' �ŐV���R�[�h�ʒu
        m_typHyDeflist.rec.head.nPos(0) = CInt(txtRec_o.Text)
        m_typHyDeflist.rec.head.nPos(1) = CInt(txtRec_u.Text)

        ' �r�ꗗ���\��
        Deflist_o(rec_o, False, False)
		' �r�ꗗ��񗠖�
		Deflist_u(rec_u, False, False)

        DefectInf(dgvDefect_o, 0, 0, rec_o, False, False)
        DefectInf(dgvDefect_u, 1, 0, rec_u, False, False)

        path(dgvPath_o, 0, rec_o, False, False)
        path(dgvPath_u, 1, rec_u, False, False)

    End Sub

	''' <summary>
	''' ���L�������̓��e��\������
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispShmem()
		Dim rec_o As Integer = CInt(nudRec_O.Value) - 1
		Dim rec_u As Integer = CInt(nudRec_U.Value) - 1

        If rec_o < 0 Then rec_o = 0
        If rec_u < 0 Then rec_u = 0

        ' �ŐV���R�[�h�ʒu
        txtRec_o.Text = m_typHyDeflist.rec.head.nPos(0).ToString
        txtRec_u.Text = m_typHyDeflist.rec.head.nPos(1).ToString

        ' �r�ꗗ���\��
        Deflist_o(rec_o, False, True)
		' �r�ꗗ��񗠖�
		Deflist_u(rec_u, False, True)


        DefectInf(dgvDefect_o, 0, 0, rec_o, False, True)
        DefectInf(dgvDefect_u, 1, 0, rec_u, False, True)

        path(dgvPath_o, 0, rec_o, False, True)
        path(dgvPath_u, 1, rec_u, False, True)

    End Sub

    ''' <summary>
    ''' DataGridView������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgvdefect(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)
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
            .RowTemplate.Height = 16
            .RowCount = RowCount

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
                        .Columns(ii).Width = 180
                    Case 1
                        .Columns(ii).Width = 80
                    Case 2
                        .Columns(ii).Width = 120
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
    ''' Path�pDataGridView������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgvPath(ByVal dgv As tCnt_DataGridView, ByVal RowCount As Integer)
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
            .RowTemplate.Height = 16
            .RowCount = RowCount

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
                        .Columns(ii).Width = 100
                    Case 1
                        .Columns(ii).Width = 180
                    Case 2
                        .Columns(ii).Width = 120
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
    ''' �r����\���E�ǂݍ���
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:�\�� false:�Ǎ�</param>	
    ''' <remarks></remarks>
    Private Sub DefectInf(ByVal dgv As tCnt_DataGridView, ByVal nMen As Integer, ByVal nSpm As Integer, ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        '' �Ώۂ�dgv���Z�b�g
        'Dim dgv As tCnt_DataGridView = dgvDefect1

        With m_typHyDeflist.rec.data(nMen).DfData(rec).typ.data
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii
                        Case 0  ' �rNo
                            If init Then dgv(0, ii).Value = "�rNo" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nKizuNo.ToString _
                            Else .nKizuNo = CInt(dgv(1, ii).Value)
                        Case 1  ' �\���敪
                            If init Then dgv(0, ii).Value = "�\���敪" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .TorB.ToString _
                            Else .TorB = CUShort(dgv(1, ii).Value)
                        Case 2  ' �r��EdasysID
                            If init Then dgv(0, ii).Value = "�r��EdasysID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nTid.ToString _
                            Else .nTid = CUShort(dgv(1, ii).Value)
                        Case 3  ' �O���[�hEdasysID
                            If init Then dgv(0, ii).Value = "�O���[�hEdasysID" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nGid.ToString _
                            Else .nGid = CUShort(dgv(1, ii).Value)
                        Case 4  ' �����r�敪
                            If init Then dgv(0, ii).Value = "�����r�敪" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cycle_div.ToString _
                            Else .cycle_div = CUShort(dgv(1, ii).Value)
                        Case 5  ' DSWS�敪
                            If init Then dgv(0, ii).Value = "DSWS�敪" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dsws.ToString _
                            Else .dsws = CUShort(dgv(1, ii).Value)
                        Case 6  ' �J�����Z�b�g
                            If init Then dgv(0, ii).Value = "�J�����Z�b�g" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .camset.ToString _
                            Else .camset = CUShort(dgv(1, ii).Value)
                        Case 7  ' F����̈ʒu[mm]
                            If init Then dgv(0, ii).Value = "F����̈ʒu[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .y.ToString _
                            Else .y = CSng(dgv(1, ii).Value)
                        Case 8  ' �o������̈ʒu[mm]
                            If init Then dgv(0, ii).Value = "�o������̈ʒu[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .y_out.ToString _
                            Else .y_out = CSng(dgv(1, ii).Value)
                        Case 9  ' DS���G�b�W����̈ʒu[mm]
                            If init Then dgv(0, ii).Value = "DS���G�b�W����̈ʒu[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .ds.ToString _
                            Else .ds = CSng(dgv(1, ii).Value)
                        Case 10 ' WS���G�b�W����̈ʒu[mm]
                            If init Then dgv(0, ii).Value = "WS���G�b�W����̈ʒu[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .ws.ToString _
                            Else .ws = CSng(dgv(1, ii).Value)
                        Case 11 ' DS���Z���^�[����̋���[mm]
                            If init Then dgv(0, ii).Value = "DS���Z���^�[����̋���[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .dsc.ToString _
                            Else .dsc = CSng(dgv(1, ii).Value)
                        Case 12 ' WS���Z���^�[����̋���[mm]
                            If init Then dgv(0, ii).Value = "WS���Z���^�[����̋���[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .wsc.ToString _
                            Else .wsc = CSng(dgv(1, ii).Value)
                        Case 13 ' ���x[mpm]
                            If init Then dgv(0, ii).Value = "���x[mpm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .speed.ToString _
                            Else .speed = CInt(dgv(1, ii).Value)
                        Case 14 ' �s�b�`[mm]
                            If init Then dgv(0, ii).Value = "�s�b�`[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .pitch.ToString _
                            Else .pitch = CInt(dgv(1, ii).Value)
                        Case 15 ' ���[���a[mm]
                            If init Then dgv(0, ii).Value = "���[���a[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .roolkei.ToString _
                            Else .roolkei = CInt(dgv(1, ii).Value)
                        Case 16 ' �ʐ�[mm^2]
                            If init Then dgv(0, ii).Value = "�ʐ�[mm^2]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .aria.ToString _
                             Else .aria = CSng(dgv(1, ii).Value)
                        Case 17 ' �r��[mm]
                            If init Then dgv(0, ii).Value = "�r��[mm]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .kizu_wid.ToString _
                             Else .kizu_wid = CInt(dgv(1, ii).Value)
                        Case 18 ' �r����[mm]
                            If init Then dgv(0, ii).Value = "�r����[mm]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .kizu_len.ToString _
                            Else .kizu_len = CInt(dgv(1, ii).Value)
                        Case 19 ' �c����\[mm/pixel]
                            If init Then dgv(0, ii).Value = "�c����\[mm/pixel]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .h_res.ToString _
                             Else .h_res = CSng(dgv(1, ii).Value)
                        Case 20 ' ������\[mm/pixel]
                            If init Then dgv(0, ii).Value = "������\[mm/pixel]" : dgv(1, ii).Value = "" _
                             Else If flg Then dgv(1, ii).Value = .w_res.ToString _
                             Else .w_res = CSng(dgv(1, ii).Value)
                        Case 21  ' �t���[��No
                            If init Then dgv(0, ii).Value = "�t���[��No" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nFrameNo.ToString _
                            Else .nFrameNo = CInt(dgv(1, ii).Value)
                        Case 22  ' �t���[���O��Xmin[pixel]
                            If init Then dgv(0, ii).Value = "�t���[���O��Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_x_min.ToString _
                            Else .frm_x_min = CShort(dgv(1, ii).Value)
                        Case 23  ' �t���[���O��Xmax[pixel]
                            If init Then dgv(0, ii).Value = "�t���[���O��Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_x_max.ToString _
                            Else .frm_x_max = CShort(dgv(1, ii).Value)
                        Case 24  ' �t���[���O��Ymin[pixel]
                            If init Then dgv(0, ii).Value = "�t���[���O��Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_y_min.ToString _
                            Else .frm_y_min = CShort(dgv(1, ii).Value)
                        Case 25  ' �t���[���O��Ymax[pixel]
                            If init Then dgv(0, ii).Value = "�t���[���O��Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .frm_y_max.ToString _
                            Else .frm_y_max = CShort(dgv(1, ii).Value)
                        Case 26  ' �Ԉ����t���[���O��Xmin[pixel]
                            If init Then dgv(0, ii).Value = "�Ԉ����t���[���O��Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_x_min.ToString _
                            Else .cull_frm_x_min = CShort(dgv(1, ii).Value)
                        Case 27  ' �Ԉ����t���[���O��Xmax[pixel]
                            If init Then dgv(0, ii).Value = "�Ԉ����t���[���O��Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_x_max.ToString _
                            Else .cull_frm_x_max = CShort(dgv(1, ii).Value)
                        Case 28  ' �Ԉ����t���[���O��Ymin[pixel]
                            If init Then dgv(0, ii).Value = "�Ԉ����t���[���O��Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_y_min.ToString _
                            Else .cull_frm_y_min = CShort(dgv(1, ii).Value)
                        Case 29  ' �Ԉ����t���[���O��Ymax[pixel]
                            If init Then dgv(0, ii).Value = "�Ԉ����t���[���O��Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .cull_frm_y_max.ToString _
                            Else .cull_frm_y_max = CShort(dgv(1, ii).Value)
                        Case 30  ' �O��Xmin[pixel]
                            If init Then dgv(0, ii).Value = "�O��Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_x_min.ToString _
                            Else .box_x_min = CShort(dgv(1, ii).Value)
                        Case 31  ' �O��Xmax[pixel]
                            If init Then dgv(0, ii).Value = "�O��Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_x_max.ToString _
                            Else .box_x_max = CShort(dgv(1, ii).Value)
                        Case 32  ' �O��Ymin[pixel]
                            If init Then dgv(0, ii).Value = "�O��Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_y_min.ToString _
                            Else .box_y_min = CShort(dgv(1, ii).Value)
                        Case 33  '�O��Ymax[pixel]
                            If init Then dgv(0, ii).Value = "�O��Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .box_y_max.ToString _
                            Else .box_y_max = CShort(dgv(1, ii).Value)
                        Case 34  '�r���S�ʒuX[pixel]
                            If init Then dgv(0, ii).Value = "�r���S�ʒuX[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .center_x.ToString _
                            Else .center_x = CShort(dgv(1, ii).Value)
                        Case 35  '�r���S�ʒuY[pixel]
                            If init Then dgv(0, ii).Value = "�r���S�ʒuY[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .center_y.ToString _
                            Else .center_y = CShort(dgv(1, ii).Value)
                        Case 36 '�c��f��[pixel]
                            If init Then dgv(0, ii).Value = "�c��f��[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .h_size.ToString _
                            Else .h_size = CUShort(dgv(1, ii).Value)
                        Case 37 '����f��[pixel]
                            If init Then dgv(0, ii).Value = "����f��[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .w_size.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 38  '���摜_�O��Xmin[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�O��Xmin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_x_min.ToString _
                            Else .base_box_x_min = CShort(dgv(1, ii).Value)
                        Case 39  '���摜_�O��Xmax[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�O��Xmax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_x_max.ToString _
                            Else .base_box_x_max = CShort(dgv(1, ii).Value)
                        Case 40  '���摜_�O��Ymin[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�O��Ymin[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_y_min.ToString _
                            Else .base_box_y_min = CShort(dgv(1, ii).Value)
                        Case 41  '���摜_�O��Ymax[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�O��Ymax[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_box_y_max.ToString _
                            Else .base_box_y_max = CShort(dgv(1, ii).Value)
                        Case 42  '���摜_�r���S�ʒuX[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�r���S�ʒuX[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_center_x.ToString _
                            Else .base_center_x = CShort(dgv(1, ii).Value)
                        Case 43  '���摜_�r���S�ʒuY[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�r���S�ʒuY[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_center_y.ToString _
                            Else .base_center_y = CShort(dgv(1, ii).Value)
                        Case 44 ' ���摜_�c��f��[pixel]
                            If init Then dgv(0, ii).Value = "���摜_�c��f��[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_h_size.ToString _
                            Else .h_size = CUShort(dgv(1, ii).Value)
                        Case 45 ' ���摜_����f��[pixel]
                            If init Then dgv(0, ii).Value = "���摜_����f��[pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_w_size.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 46 ' ���摜_�c����\[mm/pixel]
                            If init Then dgv(0, ii).Value = "���摜_�c����\[mm/pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_h_res.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 47 ' ���摜_������\[mm/pixel]
                            If init Then dgv(0, ii).Value = "���摜_������\[mm/pixel]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .base_w_res.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 48 ' �k����
                            If init Then dgv(0, ii).Value = "�k����" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nShrinkRatio.ToString _
                            Else .w_size = CUShort(dgv(1, ii).Value)
                        Case 49 '�r��D��x
                            If init Then dgv(0, ii).Value = "�r��D��x" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nTyuu.ToString _
                            Else .nTyuu = CUShort(dgv(1, ii).Value)
                        Case 50 '�O���[�h�D��x
                            If init Then dgv(0, ii).Value = "�O���[�h�D��x" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nGyuu.ToString _
                            Else .nGyuu = CUShort(dgv(1, ii).Value)
                        Case 51 '��������Έʒu
                            If init Then dgv(0, ii).Value = "��������Έʒu" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWidAbsPos.ToString _
                            Else .nWidAbsPos = CShort(dgv(1, ii).Value)
                        Case 52 '�������敪
                            If init Then dgv(0, ii).Value = "�������敪" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nWidDiv.ToString _
                            Else .nWidDiv = CShort(dgv(1, ii).Value)
                        Case 53 '�T����ʑΏ��r
                            If init Then dgv(0, ii).Value = "�T����ʑΏ��r" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nObjTansho.ToString _
                            Else .nObjTansho = CShort(dgv(1, ii).Value)
                        Case 54 '�d���׈ꗗ��ʑΏ��r
                            If init Then dgv(0, ii).Value = "�d���׈ꗗ��ʑΏ��r" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nObjDeflist.ToString _
                            Else .nObjDeflist = CShort(dgv(1, ii).Value)
                        Case 55 '�d���׈ꗗ�����\���Ώ��r
                            If init Then dgv(0, ii).Value = "�d���׈ꗗ�����\���Ώ��r" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .nModeDeflist.ToString _
                            Else .nModeDeflist = CShort(dgv(1, ii).Value)
                        Case 56 '�r�������� �N
                            If init Then dgv(0, ii).Value = "�r�������� �N" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(0).ToString _
                            Else .time(0) = CShort(dgv(1, ii).Value)
                        Case 57 '�r�������� ��
                            If init Then dgv(0, ii).Value = "�r�������� ��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(1).ToString _
                            Else .time(1) = CShort(dgv(1, ii).Value)
                        Case 58 '�r�������� ��
                            If init Then dgv(0, ii).Value = "�r�������� ��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(2).ToString _
                            Else .time(2) = CShort(dgv(1, ii).Value)
                        Case 59 '�r�������� ��
                            If init Then dgv(0, ii).Value = "�r�������� ��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(3).ToString _
                            Else .time(3) = CShort(dgv(1, ii).Value)
                        Case 60 '�r�������� ��
                            If init Then dgv(0, ii).Value = "�r�������� ��" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(4).ToString _
                            Else .time(4) = CShort(dgv(1, ii).Value)
                        Case 61 '�r�������� �b
                            If init Then dgv(0, ii).Value = "�r�������� �b" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .time(5).ToString _
                            Else .time(5) = CShort(dgv(1, ii).Value)
                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�r���(SPM������){0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
                    txtInfo.Text = str
                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub
    ''' <summary>
    ''' �摜�p�X��\���E�ǂݍ���
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="flg">true:�\�� false:�Ǎ�</param>	
    ''' <remarks></remarks>
    Private Sub path(ByVal dgv As tCnt_DataGridView, ByVal nMen As Integer, ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        With m_typHyDeflist.rec.data(nMen).DfData(rec).typ
            For ii As Integer = 0 To dgv.RowCount - 1
                Try
                    Select Case ii

                        Case 0  ' �摜�̃p�X[��]
                            If init Then dgv(0, ii).Value = "�摜�̃p�X[��]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_s _
                            Else .image_path_s = CStr(dgv(1, ii).Value)
                        Case 1  ' �摜�̃p�X[��]
                            If init Then dgv(0, ii).Value = "�摜�̃p�X[��]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_r _
                            Else .image_path_r = CStr(dgv(1, ii).Value)
                        Case 2  ' �摜�̃p�X[��2]
                            If init Then dgv(0, ii).Value = "�摜�̃p�X[��2]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_r2 _
                            Else .image_path_r2 = CStr(dgv(1, ii).Value)
                        Case 3  ' �摜�̃p�X[��3]
                            If init Then dgv(0, ii).Value = "�摜�̃p�X[��3]" : dgv(1, ii).Value = "" _
                            Else If flg Then dgv(1, ii).Value = .image_path_r3 _
                            Else .image_path_r3 = CStr(dgv(1, ii).Value)

                    End Select

                Catch ex As Exception
                    Dim str As String = txtInfo.Text
                    str &= String.Format("�V�X�e�����ʂ̃R�C�����{0} �G���[ idx[{1}] Error [{2}]", CStr(IIf(flg, "�\��", "�Ǎ�")), ii, ex.Message)
                    txtInfo.Text = str

                End Try
            Next
            dgv.ClearSelection()
        End With
    End Sub

    ''' <summary>
    ''' ���R�[�h �R���{�{�b�N�X�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSpm_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ' �ǂݍ��ݏ���
        btnRead.PerformClick()
    End Sub

    Private Sub frmHyDefList_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class