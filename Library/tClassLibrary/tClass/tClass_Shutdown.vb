'*********************************************************************************
'パソコンの終了・再起動等を制御するクラス
'	[Ver]
'		Ver.01    2005/12/30  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices          ' DllImport

Namespace tClass
    ''' <summary>
    ''' パソコンの終了・再起動等を制御するクラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Shutdown
        '********************************************************************
        '公開定数定義
        '********************************************************************
        ''' <summary>
        ''' パソコンの動作定義
        ''' </summary>
        ''' <remarks></remarks>
        <Flags()> _
        Public Enum ShutdownFlag As Integer
            ''' <summary>ログオフ</summary> 
            LogOff = &H0
            ''' <summary>シャットダウン</summary> 
            Shutdown = &H1
            ''' <summary>再起動</summary> 
            Reboot = &H2
            ''' <summary>強制的に終了</summary> 
            Force = &H4
            ''' <summary>電源オフ</summary> 
            PowerOff = &H8
            ''' <summary>応答無しのアプリも強制的に終了 (ORで他フラグと同時指定可能)(NT系以外は無効)</summary> 
            ForceIfHung = &H10
        End Enum

        '********************************************************************
        'API定義
        '********************************************************************
        <StructLayout(LayoutKind.Sequential, Pack:=4)> _
        Private Structure LUID_AND_ATTRIBUTES
            Dim Luid As Long
            Dim Attributes As Integer
        End Structure

        <StructLayout(LayoutKind.Sequential, Pack:=4)> _
        Private Structure TOKEN_PRIVILEGES
            Dim PrivilegeCount As Integer
            Dim Privilege As LUID_AND_ATTRIBUTES
        End Structure

        <DllImport("user32.dll")> _
        Private Shared Function ExitWindowsEx _
            (ByVal flag As Integer, _
            ByVal reserved As Integer) _
            As Boolean
        End Function

        <DllImport("advapi32.dll")> _
        Private Shared Function OpenProcessToken _
            (ByVal ProcessHandle As IntPtr, _
            ByVal DesiredAccess As Integer, _
            ByRef TokenHandle As IntPtr) _
            As Boolean
        End Function

        <DllImport("advapi32.dll")> _
        Private Shared Function LookupPrivilegeValue _
            (ByVal lpSystemName As String, _
            ByVal lpName As String, _
            ByRef lpLuid As Long) _
            As Boolean
        End Function

        <DllImport("advapi32.dll")> _
        Private Shared Function AdjustTokenPrivileges _
            (ByVal TokenHandle As IntPtr, _
            ByVal DisableAllPrivileges As Boolean, _
            ByRef NewState As TOKEN_PRIVILEGES, _
            ByVal BufferLength As Integer, _
            ByVal PreviousState As IntPtr, _
            ByVal ReturnLength As IntPtr) _
            As Boolean
        End Function

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' 指定したモードでパソコンの操作を行う
        ''' </summary>
        ''' <param name="flags">パソコンの動作定義</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Shared Function ExitWindows(ByVal flags As ShutdownFlag) As Boolean
            Dim blnRetc As Boolean
            Dim setFlg As ShutdownFlag = flags  '実行フラグ

            Try
                '''' システムがNT系かチェック
                If System.Environment.OSVersion.Platform = System.PlatformID.Win32NT Then
                    setFlg = setFlg And Not ShutdownFlag.Force
                    setFlg = setFlg And Not ShutdownFlag.ForceIfHung
                    '''' ログオフのときは特権を割り当てる必要はない
                    If setFlg <> ShutdownFlag.LogOff Then
                        '' シャットダウン特権を割り当てる
                        SetShutdownPrivilege()
                    End If
                Else
                    '' NT系以外ではForceIfHungは無効
                    setFlg = setFlg And Not ShutdownFlag.ForceIfHung
                End If
                '''' ExitWindowsEx関数を呼び出す
                blnRetc = ExitWindowsEx(CInt(setFlg), 0)
            Catch ex As Exception
                blnRetc = False
            End Try
            Return blnRetc
        End Function

        ''' <summary>
        ''' 現在のプロセスにシャットダウン特権を割り当てる
        ''' </summary>
        ''' <remarks></remarks>
        Private Shared Sub SetShutdownPrivilege()
            Const TOKEN_QUERY As Integer = &H8
            Const TOKEN_ADJUST_PRIVILEGES As Integer = &H20
            Const SE_SHUTDOWN_NAME As String = "SeShutdownPrivilege"
            Const SE_PRIVILEGE_ENABLED As Integer = &H2

            '''' プロセスハンドルを取得する
            Dim hProc As IntPtr
            hProc = Process.GetCurrentProcess().Handle

            '''' トークンを取得する
            Dim hToken As IntPtr = IntPtr.Zero
            If Not OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES Or TOKEN_QUERY, hToken) Then
                Throw New Exception("トークンを取得できませんでした。")
            End If

            '''' LUIDを取得する
            Dim luid As Long = 0
            If Not LookupPrivilegeValue(Nothing, SE_SHUTDOWN_NAME, luid) Then
                Throw New Exception("LUIDを取得できませんでした。")
            End If

            '''' SE_SHUTDOWN_NAME特権を設定する
            Dim tp As New TOKEN_PRIVILEGES
            tp.PrivilegeCount = 1
            tp.Privilege = New LUID_AND_ATTRIBUTES
            tp.Privilege.Luid = luid
            tp.Privilege.Attributes = SE_PRIVILEGE_ENABLED
            If Not AdjustTokenPrivileges(hToken, False, tp, 0, IntPtr.Zero, IntPtr.Zero) Then
                Throw New Exception("SE_SHUTDOWN_NAME特権を設定できませんでした。")
            End If
        End Sub
    End Class
End Namespace
