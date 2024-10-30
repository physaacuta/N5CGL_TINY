Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Explicit On
Imports System.Runtime.InteropServices          'MarshalAsクラス等々
Imports tClassLibrary

Namespace tNinsho
    Public Module UserCheck
        ' ===========================================================
        ' 定数
        ' ===========================================================
        ' DLL名
        Const Dll32bit As String = "UserCheck32.dll"
        Const Dll64bit As String = "UserCheck64.dll"
        ' システム名
        Const SYSTEM_NAME As String = "ALL"
        ' DB接続先区分
        Enum EM_CONNECT_DB_TYPE
            emLocal = 0             ' ローカル
            emShare                 ' 統合
            emTikuseki              ' 蓄積

            emEnd
        End Enum

        ' ===========================================================
        ' DLLアクセスクラス
        ' ===========================================================
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

        ' ===========================================================
        ' x86,x64を自動で判断
        ' ===========================================================
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


        ' ===========================================================
        ' メイン認証処理
        ' ===========================================================
        ''' <summary>
        ''' 認証処理
        ''' </summary>
        ''' <param name="sTaskName">自タスク名（認証用ソフト名）</param>
        ''' <returns>true：認証成功,false：認証失敗</returns>
        ''' <remarks>認証失敗時、エラーポップアップ表示</remarks>
        Public Function IsUserCheck(ByVal sTaskName As String) As Boolean
            Dim strDbIp As String = ""                                      ' 接続先DBのIPアドレス
            Dim bConnectedDbType(EM_CONNECT_DB_TYPE.emEnd - 1) As Boolean   ' 接続先区分(true:認証済み、false:認証未済、(0)=統合,(1)=蓄積)
            Dim nRet As Integer                                             ' 認証結果
            ' 初期化
            Array.Clear(bConnectedDbType, 0, EM_CONNECT_DB_TYPE.emEnd)
            
            For ii As Integer = 1 To MC_LINE_CNT
                Dim strKey As String        ' 取得キー
                Dim strWk As String

                '''' コイル検索用ライン情報取得
                ' L1～MC_LINE_CNTの文字列（"表示用ライン名", "ラインセクション名"）取得
                strKey = String.Format("L_{0}", ii.ToString)
                strWk = tMod.tMod_IniFile.ReadIniStr("LINE", strKey, TASKINI_NAME)
                ' 空白をトリム
                strWk.Trim()
                ' 取得文字列の長さが0ならContinue
                If 0 >= strWk.Length Then Continue For
                ' カンマ区切りに値分割
                Dim strAry() As String = strWk.Split(CChar(","))
                ' 2つの要素が取得出来たかチェック
                If 2 <> strAry.Length Then Continue For
                ' 各要素をトリム
                strAry(0).Trim()
                strAry(1).Trim()
                ' 各要素の文字列の長さが0ならContinue
                If 0 >= strAry(0).Length Then Continue For
                If 0 >= strAry(1).Length Then Continue For

                '''' 接続先区分チェック（全ラインの接続先がローカル（or統合or現地蓄積）に統一されているか？）
                ' DBアクセスが有効（=1）以外ならContinue
                If 1 <> tMod.tMod_IniFile.ReadIniInt(strAry(1), "ENABLE_DB", TASKINI_NAME) Then Continue For
                ' 接続先DBのアドレスを取得
                strDbIp = tMod.tMod_IniFile.ReadIniStr(strAry(1), "DBADDLESS", TASKINI_NAME)
                ' 接続先DBのアドレスチェック
                If 0 >= strDbIp.Length Then Continue For

                Dim nNewDbType As Integer  ' 新しい接続先区分
                ' 接続先区分を取得（ローカル=0,統合=1,現地蓄積=2）
                nNewDbType = tNinsho.GetRemoteDBType(strDbIp)

                ' 取得した接続先区分についてまだ認証していない場合は認証処理を行う
                If False = bConnectedDbType(nNewDbType) Then
                    ' 認証開始
                    nRet = tNinsho.CheckAccess(strDbIp, SYSTEM_NAME, sTaskName)
                    bConnectedDbType(nNewDbType) = True
                End If

                ' 認証失敗ならループを抜ける
                If 0 > nRet Then Exit For
                ' 既に統合と蓄積で認証済みならループを抜ける
                If bConnectedDbType(EM_CONNECT_DB_TYPE.emShare) And _
                bConnectedDbType(EM_CONNECT_DB_TYPE.emTikuseki) Then Exit For
            Next

            ''''' 認証処理
            ' 認証失敗の場合
            If 0 > nRet Then
                ' エラーメッセージ取得
                Dim strErrMsg As String = tNinsho.GetErrorString + String.Format("(ErrMsg={0})", nRet)
                ' ポップアップ表示し、遠隔ソフトを終了する
                MsgBox(strErrMsg, MsgBoxStyle.OkOnly, "認証エラー")                       'メッセージボックス

                Return False
            End If
            Return True
        End Function

        ''' <summary>
        ''' DB名取得
        ''' </summary>
        ''' <param name="nDbType">接続先区分</param>
        ''' <returns>"ローカル"or"統合"or"現地蓄積"の文字列</returns>
        ''' <remarks></remarks>
        Public Function GetDbName(ByVal nDbType As Integer) As String
            If 0 = nDbType Then
                Return "ローカル"
            ElseIf 1 = nDbType Then
                Return "統合サーバー"
            ElseIf 2 = nDbType Then
                Return "現地蓄積サーバー"
            Else
                Return ""
            End If
        End Function

    End Module

End Namespace

