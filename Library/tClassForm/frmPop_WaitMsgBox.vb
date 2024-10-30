'*********************************************************************************
' �������\����ʃN���X
'	[Ver]
'		Ver.01    2007/07/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On


Public Class frmPop_WaitMsgBox
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^�������C�x���g (�x�[�X�̒u������)
	''' </summary>
	''' <param name="dbMode">DB�̏��</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal dbMode As Boolean)



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_bPrgBarAuto As Boolean							' �v���O���X�o�[ �X�V (true:���� false:���[�U�[)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���b�Z�[�W�P
	''' </summary>
	Public WriteOnly Property tProp_Msg1(Optional ByVal index As Integer = 0) As String
		Set(ByVal Value As String)
			If index = 0 Then
				lblMsg1.Text = Value
			Else
				lblMsg2.Text = Value
			End If
		End Set
	End Property


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �v���u���X�o�[ �����X�V �����ݒ�
	''' </summary>
	''' <param name="nPrgBarMaxVal">�v���O���X�o�[ �ő�l</param>
	''' <param name="nPrgBarTimer">�v���O���X�o�[ �X�V�^�C�}�[�l [s] bPrgBarAuto��true���̂ݎg�p</param>
	''' <remarks></remarks>
	Public Sub SetPrgBarAutoStart(ByVal nPrgBarMaxVal As Integer, Optional ByVal nPrgBarTimer As Integer = 1000)
		m_bPrgBarAuto = True

		If ProgressBar1.Value > nPrgBarMaxVal Then
			ProgressBar1.Value = nPrgBarMaxVal
		End If
		ProgressBar1.Maximum = nPrgBarMaxVal


		' �^�C�}�[�X�^�[�g
		trmCycle.Interval = nPrgBarTimer * 1000
		trmCycle.Start()
	End Sub

	''' <summary>
	''' �v���u���X�o�[ �蓮�X�V �����ݒ�
	''' </summary>
	''' <param name="nPrgBarMaxVal">�v���O���X�o�[ �ő�l</param>
	''' <remarks></remarks>
	Public Sub SetPrgBarUserStart(ByVal nPrgBarMaxVal As Integer)
		m_bPrgBarAuto = False

		ProgressBar1.Minimum = 0
		ProgressBar1.Value = 0
		ProgressBar1.Maximum = nPrgBarMaxVal
	End Sub

	''' <summary>
	''' �v���u���X�o�[ �X�V
	''' </summary>
	''' <param name="nValue">���ݒl</param>
	''' <remarks></remarks>
	Public Sub SetPrgBarUserSet(ByVal nValue As Integer)
		If ProgressBar1.Maximum < nValue Then
			ProgressBar1.Value = ProgressBar1.Maximum
		ElseIf ProgressBar1.Minimum > nValue Then
			ProgressBar1.Value = ProgressBar1.Minimum
		Else
			ProgressBar1.Value = nValue
		End If
	End Sub

	''' <summary>
	''' ��ʍX�V
	''' </summary>
	''' <remarks></remarks>
	Public Shadows Sub Refresh()
		Application.DoEvents()
		MyBase.Refresh()
	End Sub

	''' <summary>
	''' �^�C�}�[
	''' </summary>
	Private Sub trmCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trmCycle.Tick
		' �v���O���X�o�[�Z�b�g
		If ProgressBar1.Value = ProgressBar1.Maximum Then
			ProgressBar1.Value = 0
		End If
		ProgressBar1.Increment(1)
	End Sub

	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_WaitMsgBox_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		Me.TopMost = True
	End Sub
End Class
