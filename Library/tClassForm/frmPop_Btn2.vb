'*********************************************************************************
' �|�b�v�A�b�v�Q�{�^����ʃN���X
'	[Ver]
'		Ver.01    2008/02/01  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_Btn2

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_Btn2_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' ��`
		MyBase.AddFunctionButton(btnF01, 0)
		MyBase.AddFunctionButton(btnF02, 11)
	End Sub
End Class
