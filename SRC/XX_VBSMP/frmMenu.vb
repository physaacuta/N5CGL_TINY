'*********************************************************************************
' この画面は、完全にサンプル集てきにはもの
'	[Ver]
'		Ver.01    2012/10/02  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict Off					' 共有メモリを使用する場合は、例外的にOFFにする

Imports KizuLibCLI						' 共有メモリアクセス用
Imports System.Runtime.InteropServices	' アンマネージドコード定義用

Public Class frmMenu



	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMenu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' 共有メモリ サンプル
		Call MemAccess()

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 共有メモリ アクセス

	Private TBL_HY_NAME_TEST As String = "TBL_HY_TEST"		 ' 共有メモリ テーブル名称


	' テスト構造体
	Public Const SIZE_DSP_TYP_T As Integer = 256
	<StructLayout(LayoutKind.Sequential, Size:=SIZE_DSP_TYP_T, pack:=1)> _
	 Public Structure DSP_TYP_T

		'// VBで2次元配列のマーシャルの方法が分からん為 ヤメ
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_s As String									'// 画像のパス[正](画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_r As String									'// 画像のパス[乱](画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_r2 As String									'// 画像のパス[乱2](画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)
		<VBFixedString(32), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim image_path_r3 As String									'// 画像のパス[乱3](画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)

		<VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
		Dim pcode() As Integer									  '// 検査パターン (表/裏)(A=1 B=2…となる)


		<VBFixedArray((SIZE_DSP_TYP_T - 32 * 4 - 4 * 2) - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_DSP_TYP_T - 32 * 4 - 4 * 2)> _
		 Dim reserve() As Byte											  '// 予備
		Public Sub initialize()
			ReDim pcode(2 - 1)
			ReDim reserve((SIZE_DSP_TYP_T - 32 * 4 - 4 * 2) - 1)

		End Sub
	End Structure


	''' <summary>
	''' 共有メモリ アクセスサンプル
	''' </summary>
	''' <remarks></remarks>
	Private Sub MemAccess()
		Dim nReadAddr As Integer = 0		' 読み込み先頭位置
		Dim typValF As DSP_TYP_T = Nothing
		Dim typValT As DSP_TYP_T = Nothing

		'''' 共有メモリ生成 (通常は、KS_SLAVEで生成するため、不必要)
		Call KizuMod.ksMemCreate(TBL_HY_NAME_TEST, SIZE_DSP_TYP_T)


		'''' 書き込み
		' テストデータセット。書き込み無しの場合は、必要なし
		typValF.initialize()
		typValF.image_path_r = "aaa"
		typValF.pcode(1) = 9

		If Not KizuMod.ksMemWrite(TBL_HY_NAME_TEST, typValF) Then Exit Sub


		'''' 読み込み
        'If Not KizuMod.ksMemRead(TBL_HY_NAME_TEST, nReadAddr, typValT) Then Exit Sub


        ' 共有メモリ開放 (通常は、KS_SLAVEで生成するため、不必要)
        Call KizuMod.ksMemClose(TBL_HY_NAME_TEST)
    End Sub




End Class
