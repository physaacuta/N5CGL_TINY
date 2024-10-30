'' 共有メモリアクセス サンプル

Option Strict On							' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports system.Runtime.InteropServices					' 属性指定

Imports KizuLibCLI



Public Class frmTestSem


	<StructLayout(LayoutKind.Sequential, Size:=16, Pack:=1)> _
	Structure FL
		<VBFixedArray(4), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
		Dim data() As Integer

		Public Sub inisialize()
			ReDim data(3)
		End Sub
	End Structure

	<StructLayout(LayoutKind.Sequential, Size:=2160100, Pack:=1)> _
	Structure BIG
		<VBFixedArray(540025), MarshalAs(UnmanagedType.ByValArray, SizeConst:=540025)> _
		Dim data() As Integer

		Public Sub inisialize()
			ReDim data(540025 - 1)
		End Sub
	End Structure

	<StructLayout(LayoutKind.Sequential, Size:=640, Pack:=1)> _
	Structure COIL
		<VBFixedString(20), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=20)> _
		Dim dataS As String

		<VBFixedArray(640 - 5), MarshalAs(UnmanagedType.ByValArray, SizeConst:=640 - 5)> _
		Dim data() As Integer

		Public Sub inisialize()
			ReDim data(640 - 5 - 1)
		End Sub
	End Structure






	Private Sub Timer1_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles Timer1.Tick
		Dim tbl As FL = Nothing
		Dim tblbig As BIG = Nothing	'2,160,100 byte
		Dim tblcoil As COIL = Nothing
		tbl = CType(KizuMod.ksMemRead("SO_STATUSTBL", GetType(FL)), FL)		' 共有メモリ 読込
		tblbig = CType(KizuMod.ksMemRead("SO_MAPTBL", GetType(BIG)), BIG)
		tblcoil = CType(KizuMod.ksMemRead("SO_COILTBL", GetType(COIL)), COIL)

		TextBox1.Text = tbl.data(0).ToString


		tbl.data(0) = -11
		KizuMod.ksMemWrite("SO_STATUSTBL", tbl)

		tblcoil.dataS = "test1234"
		KizuMod.ksMemWrite("SO_COILTBL", tblcoil)
	End Sub
End Class