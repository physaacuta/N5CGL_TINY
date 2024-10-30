'*********************************************************************************
'ORACLEと接続を行う基本クラス
'	[Ver]
'		Ver.01    2006/01/28  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Data
Imports tClassLibrary.tMod.tMod_IniFile			' iniファイルの読み込み

#Const tDEBUG = False

Namespace tClass
    ''' <summary>
    ''' ORACLEと接続を行う基本クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Oracle
        '********************************************************************
        'メンバー 定義
        '********************************************************************
        Protected m_Connect As OleDb.OleDbConnection			'データベースに接続を行うインスタンス
        Protected m_Command As OleDb.OleDbCommand				'コマンド
        Protected m_SQLTimeOut As Integer						'SQL実行時のタイムアウト [秒]
        Protected m_Transact As OleDb.OleDbTransaction			'トランザクション処理

        '********************************************************************
        'プロパティー定義
        '********************************************************************
        ''' <summary>
        ''' タイムアウト設定 (書き込み専用)
        ''' </summary>
        ''' <value>タイムアウト秒</value>
        ''' <remarks></remarks>
        Public WriteOnly Property SQLTimeOut() As Integer
            Set(ByVal Value As Integer)
                If m_Command Is Nothing Then Exit Property
                '' SQL実行時のタイムアウト秒セット
                m_Command.CommandTimeout = Value
            End Set
        End Property

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="strDataSource">データベース接続アドレス</param>
        ''' <param name="strUser">ログインユーザー</param>
        ''' <param name="strPass">ログインパスワード</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal strDataSource As String, ByVal strUser As String, ByVal strPass As String)
            '' 接続設定
            Dim strLogin As String
            strLogin = "Persist Security Info=True;" & _
             "server=oralce;" & _
             "Connection Timeout=60;" & _
             "Provider=MSDAORA;" & _
             "Data Source=" & strDataSource & ";" & _
             "User ID=" & strUser & ";" & _
             "Password=" & strPass & ";"

            Connection(strLogin)
        End Sub

        ''' <param name="iniFPath">iniファイルフルパス</param>
        ''' <param name="iniSession">セッション</param>
        Public Sub New(ByVal iniFPath As String, ByVal iniSession As String)
            Me.new( _
              ReadIniStr(iniSession, "DBADDLESS", iniFPath), _
              ReadIniStr(iniSession, "USERID", iniFPath), _
              ReadIniStr(iniSession, "PASSWORD", iniFPath) _
            )
        End Sub
        ''' <param name="strLogin">接続文字列指定</param>
        Public Sub New(ByVal strLogin As String)
            Connection(strLogin)
        End Sub
        ''' <summary>
        ''' 接続準備
        ''' </summary>
        ''' <param name="strLogin"></param>
        ''' <remarks></remarks>
        Private Sub Connection(ByVal strLogin As String)
            '' 各メンバ変数のインスタンス生成
            m_Connect = New OleDb.OleDbConnection(strLogin)
            m_Command = New OleDb.OleDbCommand
            m_Command.Connection = m_Connect
        End Sub

        ''' <summary>
        ''' DBと接続を開始する
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function Open() As Boolean
            '' 接続状態確認
            If m_Connect.State <> ConnectionState.Closed Then Return True

            Try
                '' DBと接続を行う
                m_Connect.Open()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' DBと接続を終了する
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function Close() As Boolean
            Try
                '' 接続状態確認
                If m_Connect.State <> ConnectionState.Open Then Return True

                '' 切断
                m_Connect.Close()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' パラメータをクリアーする
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ParamClear()
            '' 実行後パラメータ削除
            m_Command.Parameters.Clear()
        End Sub

        ''' <summary>
        ''' SELECT文を発行する
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文</param>
        ''' <returns>データ ソースからデータ行の前方向ストリームを読みクラス</returns>
        ''' <remarks></remarks>
        Public Function SelectExecute(ByVal strSQL As String) As OleDb.OleDbDataReader
            '' 発行するSQL文セット
            m_Command.CommandText = strSQL

            Try
                '' SQL文発行
                Return m_Command.ExecuteReader()
            Catch ex As Exception
                If m_Command.Parameters.Count <> 0 Then
                    Dim strVal As String = ""
                    For ii As Integer = 0 To m_Command.Parameters.Count - 1
                        strVal &= m_Command.Parameters.Item(ii).ToString & "=" & CStr(m_Command.Parameters.Item(ii).Value) & ", "
                    Next
                End If
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' SELECT文を発行する (バインド対応)
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文 (@*** のパラメータ付きのSQL文)</param>
        ''' <param name="para">パラメータ</param>
        ''' <returns>影響を受けた行数を返す   -1:DML文失敗</returns>
        ''' <remarks>"SELECT CustomerID, CompanyName FROM Customers WHERE Country = @Country AND City = @City"</remarks>
        Public Overridable Function SelectExecute(ByVal strSQL As String, ByVal para() As OleDb.OleDbParameter) As OleDb.OleDbDataReader
            Try
                '' パラメータ追加
                Dim p As OleDb.OleDbParameter
                For Each p In para
                    m_Command.Parameters.Add(p)
                Next p

                '' SQL文発行
                Return SelectExecute(strSQL)
            Catch ex As Exception
                m_Command.Parameters.Clear()
                Return Nothing
            Finally
            End Try
        End Function

        ''' <summary>
        ''' DML文を実行する
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文</param>
        ''' <returns>影響を受けた行数を返す   -1:DML文失敗</returns>
        ''' <remarks></remarks>
        Public Function DMLExecute(ByVal strSQL As String) As Integer
            '' 発行するSQL文セット
            m_Command.CommandText = strSQL

            Try
                '' DML文実行
                Return m_Command.ExecuteNonQuery()
            Catch ex As Exception
                If m_Command.Parameters.Count <> 0 Then
                    Dim strVal As String = ""
                    For ii As Integer = 0 To m_Command.Parameters.Count - 1
                        strVal &= m_Command.Parameters.Item(ii).ToString & "=" & CStr(m_Command.Parameters.Item(ii).Value) & ", "
                    Next
                End If
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' DML文を実行する (バインド対応)
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文 (@*** のパラメータ付きのSQL文)</param>
        ''' <param name="para">パラメータ</param>
        ''' <returns>影響を受けた行数を返す   -1:DML文失敗</returns>
        ''' <remarks>"SELECT CustomerID, CompanyName FROM Customers WHERE Country = @Country AND City = @City"</remarks>
        Public Overridable Function DMLExecute(ByVal strSQL As String, ByVal para() As OleDb.OleDbParameter) As Integer
            Try
                '' パラメータ追加
                Dim p As OleDb.OleDbParameter
                For Each p In para
                    m_Command.Parameters.Add(p)
                Next p

                Return DMLExecute(strSQL)
            Catch ex As Exception
                Return Nothing
            Finally
            End Try
        End Function

        ''' <summary>
        ''' Transaction を開始する処理 Commit,Rollbackを行い場合は実行させましょう
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function TransactStart() As Boolean
            Try
                '' トランザクション処理を開始
                m_Transact = m_Connect.BeginTransaction

                m_Command.Transaction = m_Transact
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' Commit を行う
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function TransactCommint() As Boolean
            Try
                '' コミットを行う
                m_Transact.Commit()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' Rollbackを行い場合は実行させましょう
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function TransactRollback() As Boolean
            Try
                '' ロールバックを行う
                m_Transact.Rollback()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function
    End Class
End Namespace