'*********************************************************************************
' �r���p�|�b�v�A�b�v��ʂ̐e�t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' �r���p�|�b�v�A�b�v��ʂ̐e�t�H�[��
''' </summary>
''' <remarks></remarks>
Public Class frmBase_Popup
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Protected Friend my_lstFunction As New List(Of Button)						' �{�^���R���g���[���z��
	Protected Friend my_nSelectedFNo As Integer = -1								' �����I���t�@���N�V�����{�^��No (0�I���W���B-1�͑I�𖳂�)

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^�������C�x���g
	''' </summary>
	''' <param name="FncNo">0�I���W��</param>
	''' <param name="onKey">�{�^���N���b�N���������</param>
	''' <remarks></remarks>
	Public Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���A�N�Z�X ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^���p�R���g���[�� ��o�^
	''' </summary>
	''' <param name="btn">�t�@���N�V�����{�^���R���g���[��</param>
	''' <param name="FncNo">FncNo (0�I���W��)</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddFunctionButton(ByVal btn As Button, ByVal FncNo As Integer)
		btn.Tag = FncNo			'Tag�ɃZ�b�g
		my_lstFunction.Add(btn)

		'' ���łɃC�x���g�n���h���ɒǉ�
		AddHandler btn.Click, AddressOf FunctionButton_Click
	End Sub

	''' <summary>
	''' �C�x���g�𔭐�������
	''' </summary>
	''' <param name="FncNo">�ǂ̃t�@���N�V�����{�^�� (0�I���W��) </param>
	''' <param name="onKey">�ꏏ�ɉ�����Ă���Modifiers�L�[</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetEvFunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)
		RaiseEvent tEv_FunctionButton(FncNo, onKey)
	End Sub

	''' <summary>
	''' �����I���t�@���N�V�����{�^��No���w��
	''' </summary>
	''' <param name="FncNo">�I����Ԃɂ���t�@���N�V�����{�^��No(0�I���W���B-1�͑I�𖳂�)</param>
	''' <remarks></remarks>
	Public Sub SetSelectedFNo(ByVal FncNo As Integer)
		my_nSelectedFNo = FncNo
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ����
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Popup_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

		'' �f�o�b�N����΍�
#If DEBUG Then
		' �f�o�b�N���͍őO�ʂɂ��Ȃ�
		Me.TopMost = False
#End If


	End Sub

	''' <summary>
	''' �\������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Popup_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
		If -1 = my_nSelectedFNo Then Return
		'''' �Y������t�@���N�V�����{�^����ǉ�
		For Each btn As Button In my_lstFunction
			If Not btn.Tag Is Nothing AndAlso CInt(btn.Tag) = my_nSelectedFNo Then
				btn.Select()
				Exit For
			End If
		Next
	End Sub
	''' <summary>
	''' �J��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Popup_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		'' �J��
		my_lstFunction.Clear()
	End Sub
	''' <summary>
	''' �t�@���N�V�����{�^���N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub FunctionButton_Click(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim btn As Button = CType(sender, Button)
		Dim FncNo As Integer = CInt(btn.Tag)		' �{�^��No���o��

		' �t�@���N�V�����{�^�������C�x���g����
		Call SetEvFunctionButton(FncNo, Control.ModifierKeys())
	End Sub

	''' <summary>
	''' �L�[�_�E�� (�t�@���N�V�����L�[)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBasePopup_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
		Select Case e.KeyValue
			Case Keys.F1 To Keys.F12
				Dim FncNo As Integer = e.KeyValue - Keys.F1
				For ii As Integer = 0 To my_lstFunction.Count - 1
					If FncNo = CInt(my_lstFunction(ii).Tag) Then
						' �t�@���N�V�����{�^���������̏ꍇ�́A�C�x���g���������Ȃ�
						If Not my_lstFunction(ii).Enabled Then Exit Sub
						' ����ȍ~�́A���\�b�h�ɃL�[�C�x���g�𔭐������Ȃ��悤�ɂ���
						e.Handled = True

						' �t�@���N�V�����{�^�������C�x���g����
						Call SetEvFunctionButton(FncNo, e.Modifiers)
						Exit Sub
					End If
				Next ii
		End Select
	End Sub


End Class
