'*******************************************************************************
'   共有メモリ定義
'
'  [Ver]
'       Ver.01  2013/12/12  初版
'
'  [メモ]
'
'*******************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Public Module ToTbl
	'///////////////////////////////////////////////////////////////////////////
	'スイッチ
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'動作環境
	'-------------------------------------------------------
	'False:本番環境、True:テスト環境
#Const cLOCAL = False

	'-------------------------------------------------------
	'横展開用ライン区分
	'-------------------------------------------------------
	'LINEスイッチ(必ず1コのみコントアウトする事)VBの場合モジュール毎に必要
#Const cLINE = "LINE_5CGL_TINY"

	'///////////////////////////////////////////////////////////////////////////
	'定数
	'///////////////////////////////////////////////////////////////////////////
	'-------------------------------------------------------
	'共有メモリ名称
	'-------------------------------------------------------
	'''<summary>コイル情報受け渡しテーブル </summary>
	Public Const TBL_TO_COIL_NAME As String = "TBL_TO_COIL"

	'''<summary>シーケンサ情報テーブル（データメモリ） </summary>
	Public Const TBL_TO_SEQMEM_NAME As String = "TBL_TO_SEQMEM"

	'''<summary>カメラコントローラ設定情報テーブル（データメモリ） </summary>
	Public Const TBL_TO_CAMERA_NAME As String = "TBL_TO_CAMERA"

    '-------------------------------------------------------
    '固定サイズ
    '-------------------------------------------------------
    '''<summary>シーケンサーデータ数 </summary>
    Public Const DM_DATA_MAX As Integer = 256

    '''<summary>シーケンサーデータAREA数 </summary>
    Public Const DM_AREA_MAX As Integer = 2

	'''<summary>受信エリア (用は、シーケンサ→統括) </summary>
	Public Const DM_RECV_REC As Integer = 0

	'''<summary>送信エリア (用は、シーケンサ←統括) </summary>
    Public Const DM_SEND_REC As Integer = 1



	'''<summary>コイル情報テーブル</summary>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
	Public Structure TO_COIL_TBL

		<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
		Public Structure TO_COIL_REC
			'''<summary>基本設定 </summary>
			Dim setting As COIL_SETTING
			'''<summary>基本情報 </summary>
			Dim basedata As COIL_BASEDATA
            '''<summary>コイル情報受け渡しバッファ </summary>
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

	'''<summary>シーケンサ情報テーブル</summary>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
	Public Structure TO_SEQMEM_TBL

		<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
		Public Structure TO_SEQMEM_TBL_FB_DM
			'''<summary>データ  </summary>
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

	'''<summary>カメラコントローラ設定情報テーブル</summary>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
	Public Structure TO_CAMERA_TBL

		<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
		Public Structure TO_CAMERA_TBL_CAM_INF
			'''<summary>現在露光時間 [clk] </summary>
			Dim nExpTime As Integer
			'''<summary>現在状態 (0:正常範囲 -1:下限到達 1:上限到達) </summary>
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
	''' サイズ表示
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

