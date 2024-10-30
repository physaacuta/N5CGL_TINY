Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Runtime.InteropServices

Public Class AllUserLogOff

	'********************************************************************
	' API定数 定義

	' C++で使用していた関数を使用する為、定義する
	' 該当セッションをログオフする関数
	<DllImport("wtsapi32.dll", SetLastError:=True)> _
	Private Shared Function WTSLogoffSession(ByVal hServer As IntPtr, ByVal SessionId As Integer, ByVal bWait As Boolean) As Integer
	End Function

    ' ユーザー切り替え
    <DllImport("wtsapi32.dll", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
    Private Shared Function WTSConnectSession(ByVal LogonId As Int32, ByVal TargetLogonID As Int32, <MarshalAs(UnmanagedType.LPStr)> ByVal pPassword As String, ByVal bWait As Boolean) As Integer
    End Function


	' セッション情報を取得する関数
	<DllImport("wtsapi32.dll", bestfitmapping:=True, CallingConvention:=CallingConvention.StdCall, CharSet:=CharSet.Auto, EntryPoint:="WTSEnumerateSessions", setlasterror:=True, ThrowOnUnmappableChar:=True)> _
	Private Shared Function WTSEnumerateSessions(ByVal hServer As IntPtr, <MarshalAs(UnmanagedType.U4)> ByVal Reserved As Int32, <MarshalAs(UnmanagedType.U4)> ByVal Version As Int32, ByRef ppSessionInfo As IntPtr, <MarshalAs(UnmanagedType.U4)> ByRef pCount As Int32) As Int32
	End Function


	' メモリを開放する関数
	<DllImport("wtsapi32.dll")> _
	 Private Shared Sub WTSFreeMemory(ByVal pMemory As IntPtr)
	End Sub



	' セッション状態のEnum定義
    Private Enum WTS_CONNECTSTATE_CLASS
        WTSActive
        WTSConnected
        WTSConnectQuery
        WTSShadow
        WTSDisconnected
        WTSIdle
        WTSListen
        WTSReset
        WTSDown
        WTSInit
    End Enum

	' セッション情報を格納する構造体定義
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Auto)> _
	Private Structure WTS_SESSION_INFO
		Dim nSessionID As Int32
		Dim pWinStationName As String
		Dim eState As WTS_CONNECTSTATE_CLASS
	End Structure


	'********************************************************************
	' メイン処理

	''' <summary>
	''' 自分以外のセッションをログオフする(Listenセッションとサービス用セッションは除外)
	''' </summary>
	''' <remarks></remarks>
	Public Shared Sub Exec()
		' 変数定義
		' 変数の宣言_初期化
		Dim nMySessionID As Integer = Process.GetCurrentProcess.SessionId	' 自セッションID取得
		Dim pSessions As IntPtr = IntPtr.Zero	' 取得セッションポインタ
		Dim nScount As Int32 = 0				' セッション数


		' 接続されている全セッション数を取得する
		Dim nRetVal As Int32 = WTSEnumerateSessions(IntPtr.Zero, 0, 1, pSessions, nScount)
		If 0 = nRetVal Then Return


		' セッション数分情報を生成する
		Dim pSessionInfo() As WTS_SESSION_INFO = New WTS_SESSION_INFO(nScount) {}
		Dim nOffset As Int64 = pSessions.ToInt64

		' セッション数分処理を行う
		For ii As Integer = 0 To nScount - 1
			' セッション情報を取得する
			pSessionInfo(ii) = CType(Marshal.PtrToStructure(New IntPtr(nOffset), GetType(WTS_SESSION_INFO)), WTS_SESSION_INFO)
			nOffset += Marshal.SizeOf(New WTS_SESSION_INFO)	 ' 次へ


			' 自分のセッションID以外とサービス用セッション(ID:0)以外とリッスンセッション(65536)のユーザーを除外する
			If ((nMySessionID <> pSessionInfo(ii).nSessionID) And _
			   ((0 <> pSessionInfo(ii).nSessionID) And ("Services" <> pSessionInfo(ii).pWinStationName) And _
			   (WTS_CONNECTSTATE_CLASS.WTSListen <> pSessionInfo(ii).eState))) Then

				' 自分のユーザ名以外とサービス用セッション以外とリッスンセッション(65536)のユーザーをログオフする
				Dim nRet As Integer = WTSLogoffSession(IntPtr.Zero, pSessionInfo(ii).nSessionID, True)
				''gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, String.Format(" ログオフセッション名：{0} ログオフセッションID: {1} ログオフ関数戻り値：{2} ", pSessionInfo(ii).pWinStationName, pSessionInfo(ii).nSessionID, nRet))
				Continue For
			End If
			''gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, String.Format(" [ログオンのまま] ログオフセッション名：{0} セッションID: {1} ", pSessionInfo(ii).pWinStationName, pSessionInfo(ii).nSessionID))
		Next

		' 開放する
		WTSFreeMemory(pSessions)
		''gcls_Log.Dispose()
		''gcls_Log = Nothing
    End Sub

    ''' <summary>
    ''' コンソール側が 表でない場合 コンソールを表に変更する
    ''' </summary>
    ''' <remarks></remarks>
    Public Shared Sub Change()
        Dim nMySessionID As Integer = Process.GetCurrentProcess.SessionId   ' 自セッションID取得
        Dim pSessions As IntPtr = IntPtr.Zero   ' 取得セッションポインタ
        Dim nScount As Int32 = 0                ' セッション数

        Dim nRetVal As Int32 = WTSEnumerateSessions(IntPtr.Zero, 0, 1, pSessions, nScount)
        If 0 = nRetVal Then Return

        ' コンソールセッションIDを特定
        Dim nConsoleId As Int32 = 0    ' コンソールID
        Dim pSessionInfo() As WTS_SESSION_INFO = New WTS_SESSION_INFO(nScount) {}
        Dim nOffset As Int64 = pSessions.ToInt64
        For ii As Integer = 0 To nScount - 1
            pSessionInfo(ii) = CType(Marshal.PtrToStructure(New IntPtr(nOffset), GetType(WTS_SESSION_INFO)), WTS_SESSION_INFO)
            nOffset += Marshal.SizeOf(New WTS_SESSION_INFO)  ' 次へ


            If "Console" = pSessionInfo(ii).pWinStationName Then
                nConsoleId = pSessionInfo(ii).nSessionID
            End If
        Next

        ' 切り替え
        If 0 <> nConsoleId AndAlso nMySessionID <> nConsoleId Then
            ' 第三引数は、パスワード。同一であれば、空白でも良いと思う
            WTSConnectSession(nMySessionID, nConsoleId, "", True)
        End If

        ' 開放する
        WTSFreeMemory(pSessions)

    End Sub


    ''' <summary>
    ''' アクティブなセッションIDを返す
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function GetActiveSessionId() As Int32
        Dim nMySessionID As Integer = Process.GetCurrentProcess.SessionId   ' 自セッションID取得
        Dim pSessions As IntPtr = IntPtr.Zero   ' 取得セッションポインタ
        Dim nScount As Int32 = 0                ' セッション数

        Dim nRetVal As Int32 = WTSEnumerateSessions(IntPtr.Zero, 0, 1, pSessions, nScount)
        If 0 = nRetVal Then Return -1

        ' コンソールセッションIDを特定
        Dim nActiveId As Int32 = -1    ' アクティブID
        Dim pSessionInfo() As WTS_SESSION_INFO = New WTS_SESSION_INFO(nScount) {}
        Dim nOffset As Int64 = pSessions.ToInt64
        For ii As Integer = 0 To nScount - 1
            pSessionInfo(ii) = CType(Marshal.PtrToStructure(New IntPtr(nOffset), GetType(WTS_SESSION_INFO)), WTS_SESSION_INFO)
            nOffset += Marshal.SizeOf(New WTS_SESSION_INFO)  ' 次へ


            If WTS_CONNECTSTATE_CLASS.WTSActive = pSessionInfo(ii).eState Then
                nActiveId = pSessionInfo(ii).nSessionID
            End If
        Next
        ' 開放する
        WTSFreeMemory(pSessions)

        Return nActiveId
    End Function
End Class
