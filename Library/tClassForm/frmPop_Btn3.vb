'*********************************************************************************
' �|�b�v�A�b�v�R�{�^����ʃN���X
'	[Ver]
'		Ver.01    2008/02/01  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_Btn3

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O�����\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �{�^���L�������ؑ�
	''' </summary>
	''' <param name="Index">�C���f�b�N�X (0�I���W��)</param>
	''' <param name="bEnable">�L������</param>
	''' <remarks></remarks>
	Public Sub SetButtonEnable(ByVal Index As Integer, ByVal bEnable As Boolean)
		Select Case Index
			Case 0
				btnF01.Enabled = bEnable
			Case 1
				btnF02.Enabled = bEnable
			Case 2
				btnF03.Enabled = bEnable
		End Select
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
	Private Sub frmPop_Btn3_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		btnF01.Enabled = False
		btnF02.Enabled = False
		btnF03.Enabled = False

		' ��`
		MyBase.AddFunctionButton(btnF01, 0)
		MyBase.AddFunctionButton(btnF02, 4)
		MyBase.AddFunctionButton(btnF03, 11)
	End Sub


End Class