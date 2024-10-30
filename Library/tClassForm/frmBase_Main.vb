'*********************************************************************************
' ���j���[�L���ʂ̐e�t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' ���j���[�L���ʂ̐e�t�H�[��
''' </summary>
''' <remarks></remarks>
Public Class frmBase_Main

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Protected Friend my_btnFunction(11) As Button						' �{�^���R���g���[���z�� (New�ŃC���X�^���X�Z�b�g�ς�)
	Protected Friend my_frmParent As Form = Nothing						' �ʏ�̓��j���[���

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^�������C�x���g
	''' </summary>
	''' <param name="FncNo">0�I���W��</param>
	''' <param name="enable">�{�^���R���g���[���L���� True</param>
	''' <param name="onKey">�{�^���N���b�N���������</param>
	''' <remarks></remarks>
	Public Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <remarks></remarks>
	Public Sub New()
		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
		'''' �t�@���N�V�����{�^���̃R���g���[���z��𐶐�����
		my_btnFunction(0) = btnF01
		my_btnFunction(1) = btnF02
		my_btnFunction(2) = btnF03
		my_btnFunction(3) = btnF04
		my_btnFunction(4) = btnF05
		my_btnFunction(5) = btnF06
		my_btnFunction(6) = btnF07
		my_btnFunction(7) = btnF08
		my_btnFunction(8) = btnF09
		my_btnFunction(9) = btnF10
		my_btnFunction(10) = btnF11
		my_btnFunction(11) = btnF12
		'' �{�^���ԍ��Z�b�g
		For ii As Integer = 0 To my_btnFunction.Length - 1
			my_btnFunction(ii).Tag = ii
		Next
	End Sub

	''' <summary>
	''' �t�@���N�V�����{�^���N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub FunctionButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnF01.Click, btnF02.Click, btnF03.Click, btnF04.Click, btnF05.Click, btnF06.Click, btnF07.Click, btnF08.Click, btnF09.Click, btnF10.Click, btnF11.Click, btnF12.Click
		Dim btn As Button = CType(sender, Button)
		Dim FncNo As Integer = CInt(btn.Tag)		' �{�^��No���o��

		' �t�@���N�V�����{�^�������C�x���g����
		Call SetEvFunctionButton(FncNo, btn.Enabled, Control.ModifierKeys())
	End Sub

	''' <summary>
	''' �L�[�_�E�� (�t�@���N�V�����L�[)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBaseMain_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
		Select Case e.KeyValue
			Case Keys.F1 To Keys.F12
				Dim FncNo As Integer = e.KeyValue - Keys.F1

				' �����̏����~�߁B  �{�^��������ԂŁAShift+F12 �Ƃ��̃C�x���g�����Ȃ���
				'' '' �t�@���N�V�����{�^���������̏ꍇ�́A�C�x���g���������Ȃ�
				' ''If Not my_btnFunction(FncNo).Enabled Then Exit Sub

				' ����ȍ~�́A���\�b�h�ɃL�[�C�x���g�𔭐������Ȃ��悤�ɂ���
				e.Handled = True

				' �t�@���N�V�����{�^�������C�x���g����
				Call SetEvFunctionButton(FncNo, my_btnFunction(FncNo).Enabled, e.Modifiers)
		End Select
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���A�N�Z�X ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �C�x���g�𔭐�������
	''' </summary>
	''' <param name="FncNo">�ǂ̃t�@���N�V�����{�^�� (0�I���W��) </param>
	''' <param name="enable">�{�^���R���g���[���L���� True</param>
	''' <param name="onKey">�ꏏ�ɉ�����Ă���Modifiers�L�[</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetEvFunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys)
		RaiseEvent tEv_FunctionButton(FncNo, enable, onKey)
	End Sub
	''' <summary>
	''' �Ăяo�����t�H�[�����Z�b�g
	''' </summary>
	''' <param name="frm"></param>
	''' <remarks></remarks>
	Public Overridable Sub SetParentForm(ByRef frm As Form)
		my_frmParent = frm
	End Sub
	''' <summary>
	''' �t�H�[���I������ �e�t�H�[����\��
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecFromClose()
		If Not my_frmParent Is Nothing Then my_frmParent.Show() ' �e�t�H�[����\��
		Me.Close()
	End Sub
End Class
