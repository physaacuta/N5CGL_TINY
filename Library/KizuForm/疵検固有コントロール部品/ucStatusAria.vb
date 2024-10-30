'*********************************************************************************
'�@�T����� ��p�H �X�e�[�^�X���\���R���g���[��
'	[Ver]
'		Ver.01    2008/10/09  ����
'
'	[����]
'		
'*********************************************************************************

Option Strict On
Imports tClassLibrary

Public Class ucStatusAria

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_bIsDbErr As Boolean = False										' DB�ُ�t���O (true:�ُ�)
	Private Shared WithEvents m_tmGuidance As Timer = New Timer()				' �K�C�_���X�N���A�^�C�}�[ 
	Private Const m_nGuidanceTime As Integer = 10000							' �K�C�_���X����(10�b)
	Private Shared bGuidance As Boolean = False									' �K�C�_���X�t���O


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J�v���p�e�B�[
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' DB�ُ�t���O (true:�ُ�)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property tProp_IsDbErr() As Boolean
		Get
			Return m_bIsDbErr
		End Get
		Set(ByVal Value As Boolean)
			m_bIsDbErr = Value
		End Set
	End Property



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////


	''' <summary>
	''' �S��������
	''' </summary>
	''' <remarks></remarks>
	Public Sub DspInit()
		' �K�C�_���X���\��
		InitLabel(lblGaidans)
		' �ғ���ԕ\��
		InitLabel(lblKadou)
		' ������ԕ\��
		InitLabel(lblKensa)
		' �@���ԕ\��
		InitLabel(lblKiki)
	End Sub


	''' <summary>
	''' �S�� �X�V
	''' </summary>
	''' <param name="ou">�\���敪 (0:�\ 1:��)</param>
	''' <param name="emUnten">�^�]���</param>
	''' <param name="emKadou">�ғ����</param>
	''' <param name="nowstatus">�X�e�[�^�X���e�[�u��</param>
	''' <param name="bDbErr">DB�ُ���(�����t���O���g��Ȃ�)</param>
	''' <remarks></remarks>
	Public Overloads Sub DspData(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU, ByVal nowstatus As TYP_STATUS_NOWSYSTEM, ByVal bDbErr As Boolean)
		If bDbErr Then
			' DB�ُ�
			Call DspDBErr(ou, emUnten, emKadou)
		Else
			' DB����
			Call DspNomal(ou, emUnten, emKadou, nowstatus)
		End If
	End Sub
	Public Overloads Sub DspData(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU, ByVal nowstatus As TYP_STATUS_NOWSYSTEM)
		If m_bIsDbErr Then
			' DB�ُ�
			Call DspDBErr(ou, emUnten, emKadou)
		Else
			' DB����
			Call DspNomal(ou, emUnten, emKadou, nowstatus)
		End If
	End Sub

	''' <summary>
	''' DB���펞 �̍X�V
	''' </summary>
	''' <param name="ou">�\���敪 (0:�\ 1:��)</param>
	''' <param name="emUnten">�^�]���</param>
	''' <param name="emKadou">�ғ����</param>
	''' <param name="nowstatus">�X�e�[�^�X���e�[�u��</param>
	''' <remarks></remarks>
	Public Sub DspNomal(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU, ByVal nowstatus As TYP_STATUS_NOWSYSTEM)
		' �K�C�_���X���\��
		DspGaidans(lblGaidans, ou, nowstatus)
		' �ғ���ԕ\��
		DspKadou(lblKadou, emKadou, emUnten)
		' ������ԕ\��
		DspKensa(lblKensa, CType(nowstatus.st_kensa(ou), EM_DIV_KENSA))
		' �@���ԕ\��
		DspKiki(lblKiki, CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))
	End Sub


	''' <summary>
	''' DB�ُ펞 �̍X�V
	''' </summary>
	''' <param name="ou">�\���敪 (0:�\ 1:��)</param>
	''' <param name="emUnten">�^�]���</param>
	''' <param name="emKadou">�ғ����</param>
	''' <remarks></remarks>
	Public Sub DspDBErr(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU)
		' �����Ő���
		Dim nowstatus As TYP_STATUS_NOWSYSTEM = Nothing
		nowstatus.initialize()
		nowstatus.nCnt = 1
		nowstatus.typErr(0).msg = "�f�[�^�x�[�X�ڑ��ُ킪�����������߁A�����ł��܂���B"
		nowstatus.st_kensa(ou) = EM_DIV_KENSA.DIV_KENSA_NG
		nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_JYU

		' �S���\��
		DspNomal(ou, emUnten, emKadou, nowstatus)
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Sub ucStatusAria_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' �S��������
		DspInit()
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �ÓI ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ���x��������������
	''' </summary>
	''' <param name="lbl"></param>
	''' <remarks></remarks>
	Public Shared Sub InitLabel(ByVal lbl As Label)
		lbl.Text = ""
		lbl.BackColor = g_ColorControl
	End Sub


	''' <summary>
	''' �K�C�_���X���\��
	''' </summary>
	''' <param name="lbl">�K�C�_���X���x��</param>
	''' <param name="ou">�\���敪 (0:�\ 1:��)</param>
	''' <param name="nowstatus">�X�e�[�^�X���e�[�u��</param>
	''' <remarks></remarks>
	Public Shared Sub DspGaidans(ByVal lbl As Label, ByVal ou As Integer, ByVal nowstatus As TYP_STATUS_NOWSYSTEM)
		If 0 = nowstatus.nCnt Then
			If m_tmGuidance Is Nothing Then
				lbl.Text = ""
				lbl.BackColor = g_ColorControl
				bGuidance = False
			End If
		Else

			If lbl.Text <> nowstatus.typErr(0).msg Then bGuidance = False ' ���b�Z�[�W���Ⴄ�ꍇ�́A�K�C�_���X�t���O��FALSE

			If Not bGuidance Then								' �K�C�_���X�t���O��TRUE�̏ꍇ�͏������Ȃ�
				lbl.Text = nowstatus.typErr(0).msg				' ���b�Z�[�W�Z�b�g
				If m_tmGuidance Is Nothing Then
					m_tmGuidance = New Timer()					' �^�C�}�I�u�W�F�N�g����
				Else
					m_tmGuidance.Stop()							' �^�C�}�X�g�b�v
				End If
				m_tmGuidance.Enabled = True
				m_tmGuidance.Interval = m_nGuidanceTime			' �^�C�}�[���ԃZ�b�g
				m_tmGuidance.Start()							' �^�C�}�[�N��
				bGuidance = True								' �K�C�_���X�t���O��TRUE
			End If

			'If nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_OK Then
			'	' ���ʂ�����ł��A�S�̂Ƃ��Ĉُ킪�o�Ă���̂ŁA�Ƃ肠�����y�̏�F���o��
			'	lbl.BackColor = DspKikiBackColor(EM_DIV_KIKI.DIV_KIKI_KEI)
			'Else
			'	lbl.BackColor = DspKikiBackColor(CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))
			'End If

			If nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_JYU Then
				lbl.BackColor = DspKikiBackColor(CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))
			ElseIf nowstatus.st_kensa(ou) = 6 Or _
			nowstatus.st_kensa(ou) = 5 Or _
			nowstatus.st_kensa(ou) = 4 Or _
			nowstatus.st_kensa(ou) = EM_DIV_KENSA.DIV_KENSA_NG Then
				lbl.BackColor = DspKensaBackColor(CType(nowstatus.st_kensa(ou), EM_DIV_KENSA))

			ElseIf nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_KEI Then
				lbl.BackColor = DspKikiBackColor(CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))

			ElseIf nowstatus.st_kiki(ou) <> EM_DIV_KIKI.DIV_KIKI_OK Or nowstatus.st_kensa(ou) <> EM_DIV_KENSA.DIV_KENSA_OK Then
				' �����̖ʂŉ����ُ̈킪����
				lbl.BackColor = DspKikiBackColor(EM_DIV_KIKI.DIV_KIKI_KEI)
			Else
				' ���ʂ�����ł��A�S�̂Ƃ��Ĉُ킪�o�Ă���̂ŁA�Ƃ肠�����y�̏�F���o��
				lbl.BackColor = DspKikiBackColor(EM_DIV_KIKI.DIV_KIKI_KEI)
			End If

		End If
	End Sub

	''' <summary>
	''' �K�C�_���X���\���ێ��^�C�}
	''' </summary>
	''' <param name="sender">�C�x���g������</param>
	''' <param name="e">�C�x���g�p�����[�^</param>
	''' <remarks></remarks>
	Private Shared Sub m_tmGuidance_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_tmGuidance.Tick
		m_tmGuidance.Stop()			' �^�C�}�[�X�g�b�v
		m_tmGuidance.Dispose()		' �^�C�}�[
		m_tmGuidance = Nothing		' �^�C�}�[���
	End Sub

	''' <summary>
	''' �^�]��ԕ\��
	''' </summary>
	''' <param name="emUnten">�^�]���</param>
	''' <remarks></remarks>
	Public Shared Function DspUnten(ByVal emUnten As EM_DIV_UNTEN) As String
		Select Case emUnten
			Case EM_DIV_UNTEN.DIV_UNTEN_STOP
				Return "��~"
			Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL
				Return "�ʏ�^�]"
			Case EM_DIV_UNTEN.DIV_UNTEN_SAMP
				Return "����َB�e"
			Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA
				Return "��ג���"
			Case EM_DIV_UNTEN.DIV_UNTEN_SMYU
				Return "�Эڰ���"
			Case Else
				Return ""
		End Select
	End Function

	''' <summary>
	''' �ғ���ԕ\��
	''' </summary>
	''' <param name="lbl">�ғ����x��</param>
	''' <param name="emKadou">�ғ����</param>
	''' <param name="emUnten">�^�]���</param>
	''' <remarks></remarks>
	Public Shared Sub DspKadou(ByVal lbl As Label, ByVal emKadou As EM_DIV_KADOU, ByVal emUnten As EM_DIV_UNTEN)

		' �ғ����
		Select Case emKadou
			Case EM_DIV_KADOU.DIV_KADOU_INIT
				lbl.Text = "��������"
				lbl.BackColor = g_ColorKadouInit
			Case EM_DIV_KADOU.DIV_KADOU_STOP
				lbl.Text = "��~��"
				lbl.BackColor = g_ColorKadouStop
			Case EM_DIV_KADOU.DIV_KADOU_START

				' �^�]��Ԃ����Ĕ��f����
				If emUnten = EM_DIV_UNTEN.DIV_UNTEN_NOMAL Then
					lbl.Text = "�ғ���"
					lbl.BackColor = g_ColorKadouStart
				Else
					lbl.Text = "�e�X�g��"
					lbl.BackColor = g_ColorKadouStart
				End If

			Case EM_DIV_KADOU.DIV_KADOU_RETRY
				lbl.Text = "��ײ��"
				lbl.BackColor = g_ColorKadouRetry
			Case Else
				lbl.Text = "�s��"
				lbl.BackColor = g_ColorControl
		End Select
	End Sub


	''' <summary>
	''' ������ԕ\��
	''' </summary>
	''' <param name="lbl">�������x��</param>
	''' <param name="emKensa">�������</param>
	''' <remarks></remarks>
	Public Shared Sub DspKensa(ByVal lbl As Label, ByVal emKensa As EM_DIV_KENSA)
		lbl.Text = DspKensa(emKensa)
		lbl.BackColor = DspKensaBackColor(emKensa)
		lbl.ForeColor = DspKensaForeColor(emKensa)
	End Sub


	''' <summary>
	''' �@���ԕ\��
	''' </summary>
	''' <param name="lbl">�@�탉�x��</param>
	''' <param name="emKiki">�@����</param> 
	''' <remarks></remarks>
	Public Shared Sub DspKiki(ByVal lbl As Label, ByVal emKiki As EM_DIV_KIKI)
		lbl.Text = DspKiki(emKiki)
		lbl.BackColor = DspKikiBackColor(emKiki)
		lbl.ForeColor = DspKikiForeColor(emKiki)
	End Sub




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���̑�������� �ÓI ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �\���敪�̕\��
	''' </summary>
	''' <param name="nOU">�\���敪 (0:�\ 1:��)</param>
	''' <param name="lbl">�Ώۃ��x��</param>
	''' <remarks></remarks>
	Public Shared Sub DspTorB(ByVal nOU As Integer, ByVal lbl As Label)
		If nOU = 0 Then
			lbl.Text = "�\��"
		Else
			lbl.Text = "����"
		End If
		lbl.BackColor = Color.Blue
		lbl.ForeColor = Color.Yellow
	End Sub

	''' <summary>
	''' �ꎞ��~�^�����X�V�敪�̕\��
	''' </summary>
	''' <param name="bIsStop">�ꎞ��~ (true:�ꎞ��~ false:�����X�V)</param>
	''' <param name="lbl">�Ώۃ��x��</param>
	''' <param name="btn">�Ώۃ{�^��</param>
	''' <remarks></remarks>
	Public Shared Sub DspStopMode(ByVal bIsStop As Boolean, ByVal lbl As Label, ByVal btn As Button)
		If bIsStop Then
			lbl.Text = "�ꎞ��~��"
			lbl.BackColor = Color.Red
			lbl.ForeColor = Color.White
			btn.Text = "�����X�V"
		Else
			lbl.Text = "�����X�V"
			lbl.BackColor = Color.Blue
			lbl.ForeColor = Color.Yellow
			btn.Text = "�ꎞ��~��"
		End If
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��{
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ������Ԗ���
	''' </summary>
	''' <param name="emKensa">�������</param>
	Public Shared Function DspKensa(ByVal emKensa As EM_DIV_KENSA) As String
		' �������
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return ""
			Case CType(-1, EM_DIV_KENSA)
				Return "��~"
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return "����"
			Case EM_DIV_KENSA.DIV_KENSA_NG
				Return "�k��"
			Case EM_DIV_KENSA.DIV_KENSA_MENTE
				Return "��Ò�"
			Case CType(4, EM_DIV_KENSA)
				Return "���ޕs��"
			Case CType(5, EM_DIV_KENSA)
				Return "���ۑ�"
			Case CType(6, EM_DIV_KENSA)
				Return "������"
			Case EM_DIV_KENSA.DIV_KENSA_STOP
				Return "��~"
			Case Else
				Return "�s��"
		End Select
	End Function


	''' <summary>
	''' ������� �w�i�F
	''' </summary>
	''' <param name="emKensa">�������</param>
	Public Shared Function DspKensaBackColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' ������� �w�i�F
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return g_ColorKensaNon
			Case CType(-1, EM_DIV_KENSA)
				Return g_ColorKensaNon
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return g_ColorKensaOk
			Case EM_DIV_KENSA.DIV_KENSA_NG
				Return g_ColorKensaNg
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' ���݃v���r�W����
				Return g_ColorMente
			Case CType(4, EM_DIV_KENSA)
				Return g_ColorKensaEdgeNg
			Case CType(5, EM_DIV_KENSA)
				Return g_ColorKensaDbNon
			Case CType(6, EM_DIV_KENSA)
				Return g_ColorKensaSkip
			Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return g_ColorKensaNon
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' ������� �����F
	''' </summary>
	''' <param name="emKensa">�������</param>
	Public Shared Function DspKensaForeColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' ������� �w�i�F
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return Color.Black
			Case CType(-1, EM_DIV_KENSA)
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return Color.Yellow
			Case EM_DIV_KENSA.DIV_KENSA_NG
                Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' ���݃v���r�W����
                Return Color.Black
			Case CType(4, EM_DIV_KENSA)
				Return Color.Black
			Case CType(5, EM_DIV_KENSA)
				Return Color.Black
			Case CType(6, EM_DIV_KENSA)
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return Color.Black
			Case Else
				Return Color.Black
		End Select
	End Function





	''' <summary>
	''' �@���Ԗ���
	''' </summary>
	''' <param name="emKiki">�@����</param>
	Public Shared Function DspKiki(ByVal emKiki As EM_DIV_KIKI) As String
		' �������
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return "����"
			Case EM_DIV_KIKI.DIV_KIKI_KEI
				Return "�y�̏�"
			Case EM_DIV_KIKI.DIV_KIKI_JYU
				Return "�d�̏�"
			Case Else
				Return "�s��"
		End Select
	End Function

	''' <summary>
	''' �@���Ԕw�i�F
	''' </summary>
	''' <param name="emKiki">�@����</param>
	Public Shared Function DspKikiBackColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' �������
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return g_ColorKikiOk
			Case EM_DIV_KIKI.DIV_KIKI_KEI
				Return g_ColorKikiKei
			Case EM_DIV_KIKI.DIV_KIKI_JYU
				Return g_ColorKikiJyu
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' �@���ԕ����F
	''' </summary>
	''' <param name="emKiki">�@����</param>
	Public Shared Function DspKikiForeColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' �������
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return Color.Yellow
			Case EM_DIV_KIKI.DIV_KIKI_KEI
                Return Color.Black
			Case EM_DIV_KIKI.DIV_KIKI_JYU
                Return Color.Black
			Case Else
                Return Color.Black
		End Select
	End Function


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �r�}�b�v
	'/ /////////////////////////////////////////////////////////////////////////////////


	''' <summary>
	''' ������� �w�i�F
	''' </summary>
	''' <param name="emKensa">�������</param>
	Public Shared Function MapKensaBackColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' ������� �w�i�F
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return g_ColorKensaNon
			Case CType(-1, EM_DIV_KENSA)
				Return g_ColorKensaStop
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return g_ColorKensaOk
			Case EM_DIV_KENSA.DIV_KENSA_NG
				Return g_ColorKensaNg
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' ���݃v���r�W����
				Return g_ColorMente
			Case CType(4, EM_DIV_KENSA)
				Return g_ColorKensaEdgeNg
			Case CType(5, EM_DIV_KENSA)
				Return g_ColorKensaDbNon
			Case CType(6, EM_DIV_KENSA)
				Return g_ColorKensaSkip
			Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return g_ColorKensaStop
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' ������� �����F
	''' </summary>
	''' <param name="emKensa">�������</param>
	Public Shared Function MapKensaForeColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' ������� �w�i�F
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return Color.Black
			Case CType(-1, EM_DIV_KENSA)
				Return Color.White
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_NG
                Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' ���݃v���r�W����
                Return Color.Black
			Case CType(4, EM_DIV_KENSA)
				Return Color.Black
			Case CType(5, EM_DIV_KENSA)
				Return Color.Black
			Case CType(6, EM_DIV_KENSA)
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_STOP
				Return Color.White
			Case Else
				Return Color.Black
		End Select
	End Function
	''' <summary>
	''' �@���Ԕw�i�F
	''' </summary>
	''' <param name="emKiki">�@����</param>
	Public Shared Function MapKikiBackColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' �������
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return g_ColorKikiOk
			Case EM_DIV_KIKI.DIV_KIKI_KEI
				Return g_ColorKikiKei
			Case EM_DIV_KIKI.DIV_KIKI_JYU
				Return g_ColorKikiJyu
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' �@���ԕ����F
	''' </summary>
	''' <param name="emKiki">�@����</param>
	Public Shared Function MapKikiForeColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' �������
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
                Return Color.Black
			Case EM_DIV_KIKI.DIV_KIKI_KEI
                Return Color.Black
			Case EM_DIV_KIKI.DIV_KIKI_JYU
                Return Color.Black
			Case Else
                Return Color.Black
		End Select
	End Function


End Class
