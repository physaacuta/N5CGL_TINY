Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tCnt

''' <summary>
''' �X�e�[�^�X���
''' </summary>
''' <remarks></remarks>
Public Class frmHyLongPd
	'///////////////////////////////////////////////////////
	'�����o�[�ϐ�
	'///////////////////////////////////////////////////////
	Private m_typHyLongPd As TBL_HY_LONGPD

	Private WithEvents m_tmAuto As New Timer
	Private WithEvents m_timer2 As New Timer
	'///////////////////////////////////////////////////////
	'�����o�[�֐�
	'///////////////////////////////////////////////////////
	''' <summary>
	''' �f�[�^�O���b�h������
	''' </summary>
	''' <param name="dgvCtrl"></param>
	''' <remarks></remarks>
	Private Sub InitdgvRec(ByVal dgvCtrl As tCnt_DataGridView)
		'�����ݒ�
		dgvCtrl.dgvInitialize(dgvCtrl)

		'��{�ݒ�
		With dgvCtrl
			'+++++++++++++++++++++++++++
			'�Z���ݒ�
			'+++++++++++++++++++++++++++
			.DefaultCellStyle().SelectionBackColor = Color.Transparent
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			'+++++++++++++++++++++++++++
			'�s�ݒ�
			'+++++++++++++++++++++++++++
			.RowCount = TBL_HY_COIL_MAX
			.RowTemplate.Height = 20
			'+++++++++++++++++++++++++++
			'��ݒ�
			'+++++++++++++++++++++++++++
			.AllowUserToResizeColumns = True
			.ColumnCount = 2
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.ColumnHeadersHeight = 20
			'�����ݒ�
			For ii As Integer = 0 To .ColumnCount - 1
				.Columns(ii).HeaderText = ""
				.Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				Select Case ii
					Case 0
						.Columns(ii).Width = 30
					Case 1
						.Columns(ii).Width = 150
					Case Else
						.Columns(ii).Width = 50
				End Select
				.Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			Next

			'�ʐݒ�
			.Columns(0).HeaderText = "REC"
			.Columns(1).HeaderText = "�����������t���O"
		End With
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
	''' ���L�������A�N�Z�X�\���`�F�b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_timer2_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer2.Tick
		' �Ώۂ̋��L�����������I�ɓǂݍ��݂��Ă݂āA�A�N�Z�X�ł��邩�ǂ����m�F����B
		' ���܂�傫�ȋ��L��������ǂݍ��ނƕ��ׂɂȂ�̂Œ���
		Dim nRetc As ValueType = 0
		Dim tmp As TBL_HY_LONGPD = CType(ksMemRead(TBL_HY_LONGPD_NAME, 0, GetType(TBL_HY_LONGPD), nRetc), TBL_HY_LONGPD)
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
	''' �f�[�^�ǂݍ���
	''' </summary>
	''' <remarks></remarks>
	Private Sub ReadData()
        Dim strTmp As String

        Try
			'���L�������ǂݍ���
			m_typHyLongPd = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_LONGPD_NAME, GetType(TBL_HY_LONGPD)), TBL_HY_LONGPD)

			'�f�[�^�\��
			With m_typHyLongPd
				'�X�e�[�^�X
				txtGuidanceFlag.Text = CStr(.nGuidanceFlag)

				'�R�C���P��
				For ii As Integer = 0 To dgvRec.RowCount - 1
					dgvRec(0, ii).Value = (ii + 1).ToString
					dgvRec(1, ii).Value = m_typHyLongPd.rec(ii).nLongPdOccurrenceFlag
				Next
			End With
		Catch ex As Exception
			strTmp = String.Format("�f�[�^�ǂݍ��ُ݈�[{0}]", ex.Message)
			'MsgBox(strTmp)
		End Try
	End Sub

	''' <summary>
	''' �f�[�^��������
	''' </summary>
	''' <remarks></remarks>
	Private Sub WriteData()
		Dim strTmp As String

		Try
			'�f�[�^�擾
			With m_typHyLongPd
				'�X�e�[�^�X
				.nGuidanceFlag = CInt(txtGuidanceFlag.Text)

				For ii As Integer = 0 To dgvRec.RowCount - 1
					.rec(ii).nLongPdOccurrenceFlag = CInt(dgvRec(1, ii).Value)
				Next
			End With

			'���L��������������
			If Not KizuLibCLI.KizuMod.ksMemWrite(TBL_HY_LONGPD_NAME, m_typHyLongPd) Then
				strTmp = String.Format("���L�������������ُ݈�[{0}]", TBL_HY_LONGPD_NAME)
				MsgBox(strTmp)
			End If
		Catch ex As Exception
			strTmp = String.Format("�f�[�^�������ُ݈�[{0}]", ex.Message)
			MsgBox(strTmp)
		End Try
	End Sub

	'///////////////////////////////////////////////////////
	'�C�x���g�֐�
	'///////////////////////////////////////////////////////
	''' <summary>
	''' ��ʃ��[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmStatus_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		'������
		InitdgvRec(dgvRec)

		' ���L�������ɃA�N�Z�X�\���`�F�b�N�^�C�}�[
		m_timer2.Interval = 100         ' 100msec
		m_timer2.Start()

		' �����X�V��L���ɂ���E�E�E�K�v�Ȃ��ꍇ�̓R�����g�ɂ���
		chkUpdate.Checked = True

		'�f�[�^�ǂݍ���
		ReadData()
	End Sub

	''' <summary>
	''' �ǂݍ��݃{�^��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRead_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnRead.Click
		'�f�[�^�ǂݍ���
		ReadData()
	End Sub

	''' <summary>
	''' �������݃{�^��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click
		'�f�[�^��������
		WriteData()
		'�f�[�^�ǂݍ���
		ReadData()
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
			m_tmAuto.Start()

			' �ǂݍ��݁E�������݃{�^���𖳌��ɂ���
			btnRead.Enabled = False
			btnWrite.Enabled = False

		Else
			' �`�F�b�N���������ꂽ��
			chk.BackColor = SystemColors.Control
			m_tmAuto.Stop()

			If txtTitle.BackColor <> Color.Red Then
				' ���L�������A�N�Z�X�\�ȏꍇ
				btnRead.Enabled = True
				btnWrite.Enabled = True
			End If
		End If
	End Sub

	''' <summary>
	''' �����X�V����
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_tmAuto_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles m_tmAuto.Tick
		'�f�[�^�ǂݍ���
		ReadData()
	End Sub

    Private Sub frmHyLongPd_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_tmAuto.Stop()
        m_tmAuto.Dispose()
    End Sub
End Class
