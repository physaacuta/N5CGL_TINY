'*********************************************************************************
' ���b�Z�[�W�{�b�N�X�̑���ƂȂ�N���X
'	[Ver]
'		Ver.01    2007/07/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_MsgBox
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^�������C�x���g (�x�[�X�̒u������)
	''' </summary>
	''' <param name="FncNo">0�I���W��</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <param name="strMsg"></param>
	''' <param name="strTaitle"></param>
	''' <remarks></remarks>
	Public Sub New(ByVal strMsg As String, Optional ByVal strTaitle As String = "", Optional ByVal msgColor As Integer = 0, _
		Optional ByVal btnL As Boolean = True, Optional ByVal btnR As Boolean = False, _
		Optional ByVal retL As DialogResult = DialogResult.None, _
		Optional ByVal retR As DialogResult = DialogResult.None)

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
		lblMsg.Text = strMsg
		lblMsg.ForeColor = Color.FromArgb(msgColor)
		Me.Text = strTaitle
		btnF01.Enabled = btnL
		btnF02.Enabled = btnR
		btnF01.DialogResult = retL
		btnF02.DialogResult = retR
	End Sub

	''' <summary>
	''' ���b�Z�[�W�ύX
	''' </summary>
	''' <param name="strMsg">���b�Z�[�W�ύX</param>
	''' <remarks></remarks>
	Public Sub ChangeMsg(ByVal strMsg As String, Optional ByVal msgColor As Integer = 0, Optional ByVal btnL As Boolean = True, Optional ByVal btnR As Boolean = False)
		lblMsg.Text = strMsg
		lblMsg.ForeColor = Color.FromArgb(msgColor)
		btnF01.Enabled = btnL
		btnF02.Enabled = btnR
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_MsgBox_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' ������
		Me.TopLevel = True						 ' �őO��
	End Sub
	''' <summary>
	''' �t�@���N�V�����{�^���N���b�N
	''' </summary>
	''' <param name="FncNo"></param>
	''' <param name="onKey"></param>
	''' <remarks></remarks>
	Private Sub frmPop_MsgBox_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

		Select Case FncNo

			Case 0
				Me.DialogResult = btnF01.DialogResult
			Case 11
				Me.DialogResult = btnF02.DialogResult
		End Select

		' ��ʂ̏I�� (�C���X�^���X�͂��̂܂܂������)
		Me.Close()

		' �Ăь��֒ʒm
		RaiseEvent tEv_FunctionButton(FncNo, onKey)
	End Sub

End Class
