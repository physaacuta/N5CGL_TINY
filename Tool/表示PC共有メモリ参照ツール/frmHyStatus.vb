Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tCnt

''' <summary>
''' �X�e�[�^�X���
''' </summary>
''' <remarks></remarks>
Public Class frmHyStatus
    '///////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////
    Private m_typHyStatus As TBL_HY_STATUS

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
    Private Sub InitdgvNgInf(ByVal dgvCtrl As tCnt_DataGridView)
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
            .RowCount = 5
            .RowTemplate.Height = 20
            '+++++++++++++++++++++++++++
            '��ݒ�
            '+++++++++++++++++++++++++++
            .AllowUserToResizeColumns = True
            .ColumnCount = 9
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            '�����ݒ�
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).HeaderText = ""
				.Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				Select Case ii
					Case 2
						.Columns(ii).Width = 65
					Case 3, 4
						.Columns(ii).Width = 90
					Case 6
						.Columns(ii).Width = 140
					Case Else
						.Columns(ii).Width = 55
				End Select
				.Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Next
            '�ʐݒ�
            .Columns(0).HeaderText = "�@��ID"
            .Columns(1).HeaderText = "PCID"
            .Columns(2).HeaderText = "�ُ�͈�"
            .Columns(3).HeaderText = "�����ُ�x"
            .Columns(4).HeaderText = "�@��ُ�x"
            .Columns(5).HeaderText = "�D��x"
            .Columns(6).HeaderText = "�ُ펞�̃��b�Z�[�W"
            .Columns(6).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(7).HeaderText = "PC�敪"
            .Columns(8).HeaderText = "�����"
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
		Dim tmp As TBL_HY_STATUS = CType(ksMemRead(TBL_HY_STATUS_NAME, 0, GetType(TBL_HY_STATUS), nRetc), TBL_HY_STATUS)
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
	''' �^�]��Ԃ̃e�L�X�g��Ԃ�
	''' </summary>
	''' <param name="emUnten"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetUntenText(ByVal emUnten As EM_DIV_UNTEN) As String
		Dim strRet As String = ""
		Select Case emUnten
			Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA
				strRet = "�Z�����[�h(�J��������)"
			Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL
				strRet = "�ʏ�^�]���[�h"
			Case EM_DIV_UNTEN.DIV_UNTEN_SAMP
				strRet = "�e�X�g�^�]���[�h(�T���v���B�e)"
			Case EM_DIV_UNTEN.DIV_UNTEN_SMYU
				strRet = "�V�~�����[�V�������[�h"
			Case EM_DIV_UNTEN.DIV_UNTEN_STOP
				strRet = "��~���[�h"
		End Select
		Return strRet
	End Function

	''' <summary>
	''' �ғ���Ԃ̃e�L�X�g��Ԃ�
	''' </summary>
	''' <param name="emKadou"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetKadouText(ByVal emKadou As EM_DIV_KADOU) As String
		Dim strRet As String = ""
		Select Case emKadou
			Case EM_DIV_KADOU.DIV_KADOU_INIT
				strRet = "��������"
			Case EM_DIV_KADOU.DIV_KADOU_RETRY
				strRet = "���g���C��"
			Case EM_DIV_KADOU.DIV_KADOU_START
				strRet = "�ғ���&�e�X�g��"
			Case EM_DIV_KADOU.DIV_KADOU_STOP
				strRet = "��~��"
		End Select
		Return strRet
	End Function

	''' <summary>
	''' ������Ԃ̃e�L�X�g��Ԃ�
	''' </summary>
	''' <param name="emKensa"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetKensaText(ByVal emKensa As EM_DIV_KENSA) As String
		Dim strRet As String = ""
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				strRet = "��~(�ΏۊO)"
			Case EM_DIV_KENSA.DIV_KENSA_OK
				strRet = "����"
			Case EM_DIV_KENSA.DIV_KENSA_NG
				strRet = "�k��"
			Case EM_DIV_KENSA.DIV_KENSA_MENTE
				strRet = "�����e��(�v���r�W����)"
			Case EM_DIV_KENSA.DIV_KENSA_EDGENG
				strRet = "�G�b�W�s��"
			Case EM_DIV_KENSA.DIV_KENSA_DBNON
				strRet = "DB���і��ۑ�"
			Case EM_DIV_KENSA.DIV_KENSA_SKIP
				strRet = "���������"
			Case EM_DIV_KENSA.DIV_KENSA_STOP
				strRet = "��~(�Жʈُ�̏ꍇ�A������~��)"
		End Select
		Return strRet

	End Function

    ''' <summary>
    ''' �@��ُ�x�̃e�L�X�g��Ԃ�
    ''' </summary>
    ''' <param name="emKiki"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetKikiText(ByVal emKiki As EM_DIV_KIKI) As String
        Dim strRet As String = ""
        Select Case emKiki
            Case EM_DIV_KIKI.DIV_KIKI_OK
                strRet = "����"
            Case EM_DIV_KIKI.DIV_KIKI_KEI
                strRet = "�y�̏�"
            Case EM_DIV_KIKI.DIV_KIKI_TYU
                strRet = "���̏�(�v���r�W����)"
            Case EM_DIV_KIKI.DIV_KIKI_JYU
                strRet = "�d�̏�"
        End Select
        Return strRet
    End Function

    ''' <summary>
    ''' �����Ԃ̃e�L�X�g��Ԃ�
    ''' </summary>
    ''' <param name="nSt"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetConnectText(ByVal nSt As Integer) As String
        Dim strRet As String = ""
        Select Case nSt
            Case 0
                strRet = "����"
            Case 1
                strRet = "�ُ�"
        End Select
        Return strRet
    End Function


    ''' <summary>
    ''' �f�[�^�ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadData()
        Dim strTmp As String

        Try
            '���L�������ǂݍ���
            m_typHyStatus = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_STATUS_NAME, GetType(TBL_HY_STATUS)), TBL_HY_STATUS)

            '�f�[�^�\��
            With m_typHyStatus
                '�X�e�[�^�X
                txtUnten.Text = CStr(.emUnten)
                txtUntenText.Text = GetUntenText(.emUnten)

                txtKadou.Text = CStr(.emKadou)
                txtKadouText.Text = GetKadouText(.emKadou)

                ' �X�e�[�^�X �����ُ�
                txtKensa0.Text = CStr(.status.st_kensa(0))
                txtKensa0Text.Text = GetKensaText(.status.st_kensa(0))
                txtKensa1.Text = CStr(.status.st_kensa(1))
                txtKensa1Text.Text = GetKensaText(.status.st_kensa(1))

                ' �@��ُ�
                txtKiki0.Text = CStr(.status.st_kiki(0))
                txtKiki0Text.Text = GetKikiText(.status.st_kiki(0))
                txtKiki1.Text = CStr(.status.st_kiki(1))
                txtKiki1Text.Text = GetKikiText(.status.st_kiki(1))

                ' �ُ�Z�b�g����
                txtCnt.Text = CStr(.status.nCnt)

                '�@����
                For ii As Integer = 0 To dgvNgInf.RowCount - 1
                    dgvNgInf(0, ii).Value = .status.typErr(ii).id
                    dgvNgInf(1, ii).Value = .status.typErr(ii).pcid
                    dgvNgInf(2, ii).Value = .status.typErr(ii).area
                    dgvNgInf(3, ii).Value = CInt(.status.typErr(ii).Kensa)
                    dgvNgInf(4, ii).Value = CInt(.status.typErr(ii).Kiki)
                    dgvNgInf(5, ii).Value = .status.typErr(ii).yusen
                    dgvNgInf(6, ii).Value = .status.typErr(ii).msg
                    dgvNgInf(7, ii).Value = .status.typErr(ii).pcmode
                    dgvNgInf(8, ii).Value = .status.typErr(ii).stat
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
            With m_typHyStatus
                '�X�e�[�^�X
                .emUnten = CType(txtUnten.Text, EM_DIV_UNTEN)
                .emKadou = CType(txtKadou.Text, EM_DIV_KADOU)

                .status.st_kensa(0) = CType(txtKensa0.Text, EM_DIV_KENSA)
                .status.st_kensa(1) = CType(txtKensa1.Text, EM_DIV_KENSA)
                .status.st_kiki(0) = CType(txtKiki0.Text, EM_DIV_KIKI)
                .status.st_kiki(1) = CType(txtKiki1.Text, EM_DIV_KIKI)
                .status.nCnt = CInt(txtCnt.Text)

                '�@����
                For ii As Integer = 0 To dgvNgInf.RowCount - 1
                    .status.typErr(ii).id = CInt(dgvNgInf(0, ii).Value)
                    .status.typErr(ii).pcid = CInt(dgvNgInf(1, ii).Value)
                    .status.typErr(ii).area = CInt(dgvNgInf(2, ii).Value)
                    .status.typErr(ii).Kensa = CType(dgvNgInf(3, ii).Value, EM_DIV_KENSA)
                    .status.typErr(ii).Kiki = CType(dgvNgInf(4, ii).Value, EM_DIV_KIKI)
                    .status.typErr(ii).yusen = CInt(dgvNgInf(5, ii).Value)
                    .status.typErr(ii).msg = CStr(dgvNgInf(6, ii).Value)
                    .status.typErr(ii).pcmode = CInt(dgvNgInf(7, ii).Value)
                    .status.typErr(ii).stat = CInt(dgvNgInf(8, ii).Value)
                Next
            End With

            '���L��������������
            If Not KizuLibCLI.KizuMod.ksMemWrite(TBL_HY_STATUS_NAME, m_typHyStatus) Then
                strTmp = String.Format("���L�������������ُ݈�[{0}]", TBL_HY_STATUS_NAME)
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
        InitdgvNgInf(dgvNgInf)

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
    ''' �^�]��ԃe�L�X�g�ύX�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtUnten_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtUnten.TextChanged
        If IsNumeric(txtUnten.Text) Then
            Dim emUnten As EM_DIV_UNTEN = CType(txtUnten.Text, EM_DIV_UNTEN)
            txtUntenText.Text = GetUntenText(emUnten)
        Else
            txtUntenText.Text = ""
        End If
    End Sub

    ''' <summary>
    ''' �ғ���ԃe�L�X�g�ύX�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtKadou_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtKadou.TextChanged
        If IsNumeric(txtKadou.Text) Then
            Dim emKadou As EM_DIV_KADOU = CType(txtKadou.Text, EM_DIV_KADOU)
            txtKadouText.Text = GetKadouText(emKadou)
        Else
            txtKadouText.Text = ""
        End If
    End Sub





    ''' <summary>
    ''' �����ُ�x�e�L�X�g�ύX�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtKensa0_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtKensa0.TextChanged, txtKensa1.TextChanged
        Dim txtBox As TextBox

        If sender Is txtKensa0 Then
            txtBox = txtKensa0Text
        ElseIf sender Is txtKensa1 Then
            txtBox = txtKensa1Text
        Else
            Exit Sub
        End If

        If IsNumeric(CType(sender, TextBox).Text) Then
            Dim emKensa As EM_DIV_KENSA = CType(CType(sender, TextBox).Text, EM_DIV_KENSA)
            txtBox.Text = GetKensaText(emKensa)
        Else
            txtBox.Text = ""
        End If

    End Sub

    ''' <summary>
    ''' �@��ُ�x�e�L�X�g�ύX�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtKiki0_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtKiki0.TextChanged, txtKiki1.TextChanged
        Dim txtBox As TextBox

        If sender Is txtKiki0 Then
            txtBox = txtKiki0Text
        ElseIf sender Is txtKiki1 Then
            txtBox = txtKiki1Text
        Else
            Exit Sub
        End If

        If IsNumeric(CType(sender, TextBox).Text) Then
            Dim emKiki As EM_DIV_KIKI = CType(CType(sender, TextBox).Text, EM_DIV_KIKI)
            txtBox.Text = GetKikiText(emKiki)
        Else
            txtBox.Text = ""
        End If
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

    Private Sub frmHyStatus_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_tmAuto.Stop()
        m_tmAuto.Dispose()
    End Sub
End Class
