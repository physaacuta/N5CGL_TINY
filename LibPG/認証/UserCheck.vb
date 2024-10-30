Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Explicit On
Imports System.Runtime.InteropServices          'MarshalAsクラス等々
Imports tClassLibrary

Namespace tNinsho
    Public Module UserCheck
        'DLL名
        Const Dll32bit As String = "UserCheck32.dll"
        Const Dll64bit As String = "UserCheck64.dll"

        ''' <summary>
        ''' 32BitDLL用アクセスクラス
        ''' </summary>
        ''' <remarks></remarks>
        Private Class Auth32
            Const Dll As String = Dll32bit
            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccess(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccessODBC(<MarshalAs(UnmanagedType.LPStr)> ByVal DSN As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetRemoteDBType(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAccessibleKensaNames(<MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As <MarshalAs(UnmanagedType.SafeArray)> String()
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAuthID() As <MarshalAs(UnmanagedType.BStr)> String
            End Function


            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetErrorString() As <MarshalAs(UnmanagedType.BStr)> String
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Sub Debug_SetAuthID(<MarshalAs(UnmanagedType.LPStr)> ByVal AuthID As String)
            End Sub
        End Class

        ''' <summary>
        ''' 64BitDLL用アクセスクラス
        ''' </summary>
        ''' <remarks></remarks>
        Private Class Auth64
            Const Dll As String = Dll64bit

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccess(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccessODBC(<MarshalAs(UnmanagedType.LPStr)> ByVal DSN As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetRemoteDBType(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAuthID() As <MarshalAs(UnmanagedType.BStr)> String
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAccessibleKensaNames(<MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As <MarshalAs(UnmanagedType.SafeArray)> String()
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetErrorString() As <MarshalAs(UnmanagedType.BStr)> String
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Sub Debug_SetAuthID(<MarshalAs(UnmanagedType.LPStr)> ByVal AuthID As String)
            End Sub
        End Class

        ''' <summary>
        ''' 引数で指定された検査装置・ソフトについて、認証チェックを実施する
        ''' </summary>
        ''' <param name="DBIP">接続しようとするデータベースのIPアドレス</param>
        ''' <param name="SysID">検査装置名称 (NHOT_MAKIなど)</param>
        ''' <param name="SoftID">ソフト名称 ( SO_GAMENなど)</param>
        ''' <returns>正数=アクセス可</returns>
        Public Function CheckAccess(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String, ByVal SysID As String, Optional ByVal SoftID As String = "") As Integer
            If IntPtr.Size = 4 Then
                Return Auth32.CheckAccess(DBIP, SysID, SoftID)
            Else
                Return Auth64.CheckAccess(DBIP, SysID, SoftID)
            End If
        End Function

        ''' <summary>
        ''' 引数で指定された検査装置・ソフトについて、認証チェックを実施する
        ''' </summary>
        ''' <param name="DSN">接続しようとするデータベースのODBC名</param>
        ''' <param name="SysID">検査装置名称 (NHOT_MAKIなど)</param>
        ''' <param name="SoftID">ソフト名称 ( SO_GAMENなど)</param>
        ''' <returns>正数=アクセス可</returns>
        Public Function CheckAccessODBC(<MarshalAs(UnmanagedType.LPStr)> ByVal DSN As String, ByVal SysID As String, Optional ByVal SoftID As String = "") As Integer
            If IntPtr.Size = 4 Then
                Return Auth32.CheckAccessODBC(DSN, SysID, SoftID)
            Else
                Return Auth64.CheckAccessODBC(DSN, SysID, SoftID)
            End If
        End Function


        ''' <summary>
        ''' 引数で指定されたIPアドレスの区分（ローカルor統合or現地）を返す
        ''' </summary>
        ''' <param name="DBIP">IPアドレス</param>
        ''' <returns>0:ローカル、1:統合、2:現地</returns>
        Public Function GetRemoteDBType(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String) As Integer
            If IntPtr.Size = 4 Then
                Return Auth32.GetRemoteDBType(DBIP)
            Else
                Return Auth64.GetRemoteDBType(DBIP)
            End If
        End Function


        ''' <summary>
        ''' 引数で指定されたソフトのアクセスが許可される検査装置の一覧を配列として返す。
        ''' </summary>
        ''' <param name="SoftID">ソフト名称 (SO_GAMENなど)</param>
        ''' <returns>検査装置の一覧</returns>
        Public Function GetAccessibleKensaNames(Optional ByVal SoftID As String = "") As String()
            If IntPtr.Size = 4 Then
                Return Auth32.GetAccessibleKensaNames(SoftID)
            Else
                Return Auth64.GetAccessibleKensaNames(SoftID)
            End If
        End Function

        ''' <summary>
        ''' 認証時などエラーが発生した直後で呼ぶと、その原因を文字列で返す。
        ''' </summary>
        ''' <returns>エラー文字列</returns>
        ''' <remarks>呼び出しが成功した場合は空文字列を返す</remarks>
        Public Function GetErrorString() As String
            If IntPtr.Size = 4 Then
                Return Auth32.GetErrorString()
            Else
                Return Auth64.GetErrorString()
            End If
        End Function

        ''' <summary>
        ''' PC環境の認証ID(認証ID選択、ADユーザ名、IPアドレス)を取得する
        ''' </summary>
        ''' <returns>“0, ad\User5678, 10.1.1.1”</returns>
        ''' <remarks>CSV形式</remarks>
        Public Function GetAuthID() As String
            If IntPtr.Size = 4 Then
                Return Auth32.GetAuthID()
            Else
                Return Auth64.GetAuthID()
            End If
        End Function

        ''' <summary>
        ''' 認証IDを、指定した引数のもので上書きする
        ''' </summary>
        ''' <param name="AuthID">強制的に設定する認証ID</param>
        ''' <remarks>リリース版のDLLではダミー関数で呼んでも効かない。</remarks>
        Public Sub Debug_SetAuthID(ByVal AuthID As String)
            If IntPtr.Size = 4 Then
                Auth32.Debug_SetAuthID(AuthID)
            Else
                Auth64.Debug_SetAuthID(AuthID)
            End If
        End Sub


        ''' <summary>
        ''' 共通認証処理
        ''' </summary>
        ''' <param name="sTaskName">自タスク名（認証用ソフト名）</param>
        ''' <returns>true：認証成功,false：認証失敗</returns>
        ''' <remarks>認証失敗時、エラーポップアップ表示</remarks>
        Public Function IsUserCheck(ByVal sTaskName As String) As Boolean
            Dim strDbIp As String = tMod.tMod_IniFile.ReadIniStr(INI_DB, "DBADDLESS", TASKINI_NAME)
            ' 認証結果
            Dim nRet As Integer = tNinsho.CheckAccess(strDbIp, SYSTEM_NAME, sTaskName)
            ' 認証失敗の場合
            If 0 > nRet Then
                ' エラーメッセージ取得
                Dim strErrMsg As String = tNinsho.GetErrorString + String.Format("(ErrMsg={0})", nRet)
                ' ポップアップ表示し、遠隔ソフトを終了する
                Dim frm_Msg As New tClassForm.frmPop_MsgBox(strErrMsg, "認証エラー", Color.Red.ToArgb, , , Windows.Forms.DialogResult.OK)                       'メッセージボックス
                frm_Msg.ShowDialog()
                Return False
            End If
            Return True
        End Function


    End Module


    'Public Class tAuth



    '    Public Shared Sub Main()

    '        System.Console.WriteLine(AuthUser.GetIDKey())

    '        For Each s As String In AuthUser.GetPermittedLines(1)
    '            System.Console.WriteLine(s)
    '        Next

    '    End Sub

    'End Class

End Namespace

