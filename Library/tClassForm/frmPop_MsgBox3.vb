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

Public Class frmPop_MsgBox3
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^�������C�x���g (�x�[�X�̒u������)
	''' </summary>
	''' <param name="FncNo">0�I���W��</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)


	''' �����o�ϐ�
	Private m_bBtnMode(2) As Boolean				' �����{�^�����


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
	 Optional ByVal btnL As Boolean = True, Optional ByVal btnC As Boolean = True, Optional ByVal btnR As Boolean = True, _
	 Optional ByVal retL As DialogResult = DialogResult.None, _
	 Optional ByVal retC As DialogResult = DialogResult.None, _
	 Optional ByVal retR As DialogResult = DialogResult.None)

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
		lblMsg.Text = strMsg
		lblMsg.ForeColor = Color.FromArgb(msgColor)
		Me.Text = strTaitle

		m_bBtnMode(0) = btnL
		m_bBtnMode(1) = btnC
		m_bBtnMode(2) = btnR
		btnF01.DialogResult = retL
		btnF02.DialogResult = retC
		btnF03.DialogResult = retR
	End Sub
	''' <summary>
	''' ���b�Z�[�W�ύX
	''' </summary>
	''' <param name="strMsg">���b�Z�[�W�ύX</param>
	''' <remarks></remarks>
	Public Sub ChangeMsg(ByVal strMsg As String, Optional ByVal msgColor As Integer = 0, Optional ByVal btnL As Boolean = True, Optional ByVal btnC As Boolean = True, Optional ByVal btnR As Boolean = True)
		lblMsg.Text = strMsg
		lblMsg.ForeColor = Color.FromArgb(msgColor)
		m_bBtnMode(0) = btnL
		m_bBtnMode(1) = btnC
		m_bBtnMode(2) = btnR
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

		Me.SetButtonEnable(0, m_bBtnMode(0))
		Me.SetButtonEnable(1, m_bBtnMode(1))
		Me.SetButtonEnable(2, m_bBtnMode(2))

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
			Case 4
				Me.DialogResult = btnF02.DialogResult
			Case 11
				Me.DialogResult = btnF03.DialogResult
		End Select

		' ��ʂ̏I�� (�C���X�^���X�͂��̂܂܂������)
		Me.Close()

		' �Ăь��֒ʒm
		RaiseEvent tEv_FunctionButton(FncNo, onKey)
	End Sub

End Class
