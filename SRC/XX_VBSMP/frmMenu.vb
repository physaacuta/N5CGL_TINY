'*********************************************************************************
' ���̉�ʂ́A���S�ɃT���v���W�Ă��ɂ͂���
'	[Ver]
'		Ver.01    2012/10/02  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict Off					' ���L���������g�p����ꍇ�́A��O�I��OFF�ɂ���

Imports KizuLibCLI						' ���L�������A�N�Z�X�p
Imports System.Runtime.InteropServices	' �A���}�l�[�W�h�R�[�h��`�p

Public Class frmMenu



	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMenu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' ���L������ �T���v��
		Call MemAccess()

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���L������ �A�N�Z�X

	Private TBL_HY_NAME_TEST As String = "TBL_HY_TEST"		 ' ���L������ �e�[�u������


	' �e�X�g�\����
	Public Const SIZE_DSP_TYP_T As Integer = 256
	<StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_T, pack:=1)> _
	 Public Structure DSP_TYP_T

		'// VB��2�����z��̃}�[�V�����̕��@���������� ����
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_s As String									'// �摜�̃p�X[��](�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_r As String									'// �摜�̃p�X[��](�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_r2 As String									'// �摜�̃p�X[��2](�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_r3 As String									'// �摜�̃p�X[��3](�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)

		<VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
		Dim pcode() As Integer									  '// �����p�^�[�� (�\/��)(A=1 B=2�c�ƂȂ�)


		<VBFixedArray((SIZE_DSP_TYP_T - 32 * 4 - 4 * 2) - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_DSP_TYP_T - 32 * 4 - 4 * 2)> _
		 Dim reserve() As Byte											  '// �\��
		Public Sub initialize()
			ReDim pcode(2 - 1)
			ReDim reserve((SIZE_DSP_TYP_T - 32 * 4 - 4 * 2) - 1)

		End Sub
	End Structure


	''' <summary>
	''' ���L������ �A�N�Z�X�T���v��
	''' </summary>
	''' <remarks></remarks>
	Private Sub MemAccess()
		Dim nReadAddr As Integer = 0		' �ǂݍ��ݐ擪�ʒu
		Dim typValF As DSP_TYP_T = Nothing
		Dim typValT As DSP_TYP_T = Nothing

		'''' ���L���������� (�ʏ�́AKS_SLAVE�Ő������邽�߁A�s�K�v)
		Call KizuMod.ksMemCreate(TBL_HY_NAME_TEST, SIZE_DSP_TYP_T)


		'''' ��������
		' �e�X�g�f�[�^�Z�b�g�B�������ݖ����̏ꍇ�́A�K�v�Ȃ�
		typValF.initialize()
		typValF.image_path_r = "aaa"
		typValF.pcode(1) = 9

		If Not KizuMod.ksMemWrite(TBL_HY_NAME_TEST, typValF) Then Exit Sub


		'''' �ǂݍ���
        'If Not KizuMod.ksMemRead(TBL_HY_NAME_TEST, nReadAddr, typValT) Then Exit Sub


        ' ���L�������J�� (�ʏ�́AKS_SLAVE�Ő������邽�߁A�s�K�v)
        Call KizuMod.ksMemClose(TBL_HY_NAME_TEST)
    End Sub




End Class
