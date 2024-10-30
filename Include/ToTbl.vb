'*******************************************************************************
'   ���L��������`
'
'  [Ver]
'       Ver.01  2013/12/12  ����
'
'  [����]
'
'*******************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Public Module ToTbl
	'///////////////////////////////////////////////////////////////////////////
	'�X�C�b�`
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'�����
	'-------------------------------------------------------
	'False:�{�Ԋ��ATrue:�e�X�g��
#Const cLOCAL = False

	'-------------------------------------------------------
	'���W�J�p���C���敪
	'-------------------------------------------------------
	'LINE�X�C�b�`(�K��1�R�̂݃R���g�A�E�g���鎖)VB�̏ꍇ���W���[�����ɕK�v
#Const cLINE = "LINE_5CGL_TINY"

	'///////////////////////////////////////////////////////////////////////////
	'�萔
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'���L����������
	'-------------------------------------------------------
	'''<summary>�R�C�����󂯓n���e�[�u�� </summary>
	Public Const TBL_TO_COIL_NAME As String = "TBL_TO_COIL"

	'''<summary>�V�[�P���T���e�[�u���i�f�[�^�������j </summary>
	Public Const TBL_TO_SEQMEM_NAME As String = "TBL_TO_SEQMEM"

	'''<summary>�J�����R���g���[���ݒ���e�[�u���i�f�[�^�������j </summary>
	Public Const TBL_TO_CAMERA_NAME As String = "TBL_TO_CAMERA"

    '-------------------------------------------------------
    '�Œ�T�C�Y
    '-------------------------------------------------------
    '''<summary>�V�[�P���T�[�f�[�^�� </summary>
    Public Const DM_DATA_MAX As Integer = 256

    '''<summary>�V�[�P���T�[�f�[�^AREA�� </summary>
    Public Const DM_AREA_MAX As Integer = 2

	'''<summary>��M�G���A (�p�́A�V�[�P���T������) </summary>
	Public Const DM_RECV_REC As Integer = 0

	'''<summary>���M�G���A (�p�́A�V�[�P���T������) </summary>
    Public Const DM_SEND_REC As Integer = 1



	'''<summary>�R�C�����e�[�u��</summary>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
	Public Structure TO_COIL_TBL

		<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
		Public Structure TO_COIL_REC
			'''<summary>��{�ݒ� </summary>
			Dim setting As COIL_SETTING
			'''<summary>��{��� </summary>
			Dim basedata As COIL_BASEDATA
            '''<summary>�R�C�����󂯓n���o�b�t�@ </summary>
            <MarshalAs(UnmanagedType.ByValArray, SizeConst:=1888)>
            Dim val() As Byte

            Public Sub initialize()
                ReDim val(1888 - 1)
            End Sub
        End Structure

        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim rec() As TO_COIL_REC

        Public Sub initialize()
			ReDim rec(2 - 1)
			For ii As Integer = 0 To 1
				rec(ii).initialize()
			Next
		End Sub
	End Structure

	'''<summary>�V�[�P���T���e�[�u��</summary>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
	Public Structure TO_SEQMEM_TBL

		<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
		Public Structure TO_SEQMEM_TBL_FB_DM
			'''<summary>�f�[�^  </summary>
			<MarshalAs(UnmanagedType.ByValArray, SizeConst:=DM_DATA_MAX)> _
			Dim Data() As Byte

			Public Sub initialize()
				ReDim Data(DM_DATA_MAX - 1)
			End Sub
		End Structure

		<MarshalAs(UnmanagedType.ByValArray, SizeConst:=DM_AREA_MAX)> _
		Dim fb_dm() As TO_SEQMEM_TBL_FB_DM

		Public Sub initialize()
			ReDim fb_dm(DM_AREA_MAX - 1)
		End Sub
	End Structure

	'''<summary>�J�����R���g���[���ݒ���e�[�u��</summary>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
	Public Structure TO_CAMERA_TBL

		<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
		Public Structure TO_CAMERA_TBL_CAM_INF
			'''<summary>���ݘI������ [clk] </summary>
			Dim nExpTime As Integer
			'''<summary>���ݏ�� (0:����͈� -1:�������B 1:������B) </summary>
			Dim nMode As Integer

			Public Sub initialize()
			End Sub
		End Structure

		<MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_CAMERA)> _
		Dim caminf() As TO_CAMERA_TBL_CAM_INF

		Public Sub initialize()
			ReDim caminf(NUM_CAMERA - 1)
		End Sub
	End Structure

	''' <summary>
	''' �T�C�Y�\��
	''' </summary>
	''' <remarks></remarks>
	Public Sub PutStructureSizes()
		Console.WriteLine("COIL_SETTING Size = {0}", Marshal.SizeOf(GetType(COIL_SETTING)))
		Console.WriteLine("COIL_BASEDATA Size = {0}", Marshal.SizeOf(GetType(COIL_BASEDATA)))
		Console.WriteLine("TO_COIL_REC Size = {0}", Marshal.SizeOf(GetType(TO_COIL_TBL.TO_COIL_REC)))
		Console.WriteLine("TO_COIL_TBL SIZE Size = {0}", Marshal.SizeOf(GetType(TO_COIL_TBL)))
		Console.WriteLine("TO_SEQMEM_TBL SIZE Size = {0}", Marshal.SizeOf(GetType(TO_SEQMEM_TBL)))
		Console.WriteLine("TO_CAMERA_TBL SIZE Size = {0}", Marshal.SizeOf(GetType(TO_CAMERA_TBL)))
	End Sub
End Module

