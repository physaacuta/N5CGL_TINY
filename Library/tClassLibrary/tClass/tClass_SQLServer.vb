'*********************************************************************************
'SQLサーバーと接続を行う基本クラス
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
    ''' SQLサーバーと接続を行う基本クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_SQLServer
        Implements IDisposable                          ' デストラクタ定義

        '********************************************************************
        'メンバー 定義
        '********************************************************************
        Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには
        Protected m_bValid As Boolean = False
        Protected m_Connect As SqlClient.SqlConnection          'データベースに接続を行うインスタンス
        Protected m_Command As SqlClient.SqlCommand             'コマンド
        Protected m_SQLTimeOut As Integer                       'SQL実行時のタイムアウト [秒]
        Protected m_Transact As SqlClient.SqlTransaction        'トランザクション処理

        Private Const mc_BinalyFileOneReadSize As Integer = 2 * 1024 * 1024 ' バイナリファイル1回の最大読み込みサイズ

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
        'スタティック
        '********************************************************************
        ''' <summary>
        ''' DBと接続できるか確認する
        ''' </summary>
        ''' <param name="iniFPath">iniファイルフルパス</param>
        ''' <param name="iniSession">セッション</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function OpenCheck(ByVal iniFPath As String, ByVal iniSession As String) As Boolean
            Dim db As tClass_SQLServer = New tClass_SQLServer(iniFPath, iniSession)

            Try
                '' 接続してみる
                If db.Open() Then
                    db.Close()
                    Return True
                End If

            Catch ex As Exception
            Finally
                db.Dispose()
            End Try
            Return False
        End Function

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="strDataSource">データベース接続アドレス</param>
        ''' <param name="strDatabase">データベース名称</param>
        ''' <param name="strUser">ログインユーザー</param>
        ''' <param name="strPass">ログインパスワード</param>
        ''' <param name="nConnectTimeOut">ｺﾈｸｼｮﾝﾀｲﾑｱｳﾄ(ﾃﾞﾌｫﾙﾄ5秒)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal strDataSource As String, ByVal strDatabase As String, ByVal strUser As String, ByVal strPass As String, Optional ByVal nConnectTimeOut As Integer = 5)
            '''' 接続設定
            Dim strLogin As String
            strLogin = "Persist Security Info=True;" & _
              "Integrated Security=False;" & _
              "server=mySQLServer;" & _
              "Connection Timeout=" & nConnectTimeOut & ";" & _
              "Data Source=" & strDataSource & ";" & _
              "database=" & strDatabase & ";" & _
              "User ID=" & strUser & ";" & _
              "Password=" & strPass & ";"

            Connection(strLogin)

            If String.IsNullOrEmpty(strDatabase) Or String.IsNullOrEmpty(strDataSource) Then
                m_bValid = False
            End If
        End Sub
        ''' <param name="iniFPath">iniファイルフルパス</param>
        ''' <param name="iniSession">セッション</param>
        ''' <param name="nConnectTimeOut">ｺﾈｸｼｮﾝﾀｲﾑｱｳﾄ(ﾃﾞﾌｫﾙﾄ5秒)</param>
        Public Sub New(ByVal iniFPath As String, ByVal iniSession As String, Optional ByVal nConnectTimeOut As Integer = 5)
            Me.new( _
              ReadIniStr(iniSession, "DBADDLESS", iniFPath), _
              ReadIniStr(iniSession, "DSNAME", iniFPath), _
              ReadIniStr(iniSession, "USERID", iniFPath), _
              ReadIniStr(iniSession, "PASSWORD", iniFPath), _
              nConnectTimeOut _
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
            m_Connect = New SqlClient.SqlConnection(strLogin)
            m_Command = New SqlClient.SqlCommand
            m_Command.Connection = m_Connect
            m_bValid = True
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                Call Close()
                m_Command = Nothing
                m_Connect = Nothing
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' DBと接続を開始する
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function Open() As Boolean
            If m_bValid = False Then
                Return False
            End If

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
                ''トランザクション処理 開放
                If Not m_Transact Is Nothing Then
                    m_Transact.Dispose()
                End If
                '' 切断
                m_Connect.Close()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' DBの接続を中断する
        ''' </summary>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Function Cancel() As Boolean

            Try

                '' 接続状態確認
                If m_Connect.State <> ConnectionState.Open Then Return True

                'キャンセル
                m_Command.Cancel()
                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function
        ''' <summary>
        ''' コマンド実行タイムアウト時間
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub SetCommandTimeOut(ByVal nInterval As Integer)
            m_Command.CommandTimeout = nInterval
        End Sub

        ''' <summary>
        ''' SELECT文を発行する
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文</param>
        ''' <returns>非接続</returns>
        ''' <remarks>例外時はNothingをかえす</remarks>
        Public Function SelectAdapter(ByVal strSQL As String) As SqlClient.SqlDataAdapter
            '' 発行するSQL文セット
            m_Command.CommandText = strSQL

            Try
                '' SQL文発行
                Return New SqlClient.SqlDataAdapter(m_Command)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' SELECT文を発行する。例外が飛びうる。
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文</param>
        ''' <returns>テーブルアダプタ</returns>
        Public Function SelectAdapterEx(ByVal strSQL As String) As SqlClient.SqlDataAdapter
            '' 発行するSQL文セット
            m_Command.CommandText = strSQL
            '' SQL文発行
            Return New SqlClient.SqlDataAdapter(m_Command)
        End Function

        ''' <summary>
        ''' SELECT文を発行する
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文</param>
        ''' <returns>データ ソースからデータ行の前方向ストリームを読みクラス</returns>
        ''' <remarks></remarks>
        Public Function SelectExecute(ByVal strSQL As String, Optional ByVal CommandBehavior As Data.CommandBehavior = Data.CommandBehavior.Default) As SqlClient.SqlDataReader
            '' 発行するSQL文セット
            m_Command.CommandText = strSQL

            Try
                '' SQL文発行
                Return m_Command.ExecuteReader(CommandBehavior)
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
        Public Overridable Function SelectExecute(ByVal strSQL As String, ByVal para() As SqlClient.SqlParameter) As SqlClient.SqlDataReader
            Try
                '' パラメータ追加
                m_Command.Parameters.Clear()

                For Each p As SqlClient.SqlParameter In para
                    m_Command.Parameters.Add(p)
                Next p

                '' SQL文発行
                Return SelectExecute(strSQL)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' SELECT文を発行する。異常時は例外発生
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文</param>
        ''' <returns>データ ソースからデータ行の前方向ストリームを読みクラス</returns>
        ''' <remarks></remarks>
        Public Function SelectExecuteEx(ByVal strSQL As String, Optional ByVal CommandBehavior As Data.CommandBehavior = Data.CommandBehavior.Default) As SqlClient.SqlDataReader
            '' 発行するSQL文セット
            m_Command.CommandText = strSQL

            '' SQL文発行
            Return m_Command.ExecuteReader(CommandBehavior)
        End Function


        ''' <summary>
        ''' SELECT文を発行する (バインド対応)　異常時は例外発生
        ''' </summary>
        ''' <param name="strSQL">発行するSQL文 (@*** のパラメータ付きのSQL文)</param>
        ''' <param name="para">パラメータ</param>
        ''' <returns>影響を受けた行数を返す   -1:DML文失敗</returns>
        ''' <remarks>"SELECT CustomerID, CompanyName FROM Customers WHERE Country = @Country AND City = @City"</remarks>
        Public Overridable Function SelectExecuteEx(ByVal strSQL As String, ByVal para() As SqlClient.SqlParameter) As SqlClient.SqlDataReader
            '' パラメータ追加
            m_Command.Parameters.Clear()

            m_Command.CommandText = strSQL
            Dim p As SqlClient.SqlParameter
            For Each p In para
                m_Command.Parameters.Add(p)
            Next p

            '' SQL文発行
            Return m_Command.ExecuteReader(Data.CommandBehavior.Default)
        End Function


        ''' <summary>
        ''' 巨大なバイナリデータの順次読込を実行し、そのまま指定ファイルに書き込む (バイナリデータのみの取得)
        ''' </summary>
        ''' <param name="bCancelFlg">中断用フラグ (Trueで中断される) (別スレッドからTrueにする)</param>
        ''' <param name="strSQL">1行,1項目のみバイナリデータを返すSQL文に限定する事</param>
        ''' <param name="strFPath">バイナリデータを格納するフルパス</param>
        ''' <param name="nBaynarySize">バイナリデータのサイズ</param>
        ''' <returns>-99:何らかの失敗 -98:SQL発行失敗  -4:取得異常 -3:NULLデータ -2:0件 -1:途中中断 0:正常</returns>
        ''' <remarks></remarks>
        Public Overridable Function SelectBigBaynaryExecute(ByVal strSQL As String, ByRef bCancelFlg As Boolean, ByVal strFPath As String, ByVal nBaynarySize As Integer, Optional ByVal nReadSize As Integer = mc_BinalyFileOneReadSize) As Integer
            '' 事前準備

            Dim readByte(nReadSize - 1) As Byte         ' バイナリデータ読み込み領域
            Dim nReadIndex As Long                      ' 読み込み位置

            '' 発行するSQL文セット
            m_Command.CommandText = strSQL
            Try
                '' SQL文発行 (部分読み出し可能オプション指定) ＝ 通常の場合は、発行したタイミングで全データを取ってくるから、巨大なデータには向かない
                Using reader As SqlClient.SqlDataReader = m_Command.ExecuteReader(CommandBehavior.SequentialAccess)
                    If reader Is Nothing Then Return -98

                    '' 1行のみの読み込み
                    If Not reader.Read() Then Return -2       ' 行無し
                    If reader.IsDBNull(0) Then Return -3      ' NULLデータ？

                    ' 読み込んだバイナリデータをファイル出力する為のストリームクラス
                    Using stream As New IO.FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.Write)         ' ファイルストリーム
                        '' ちょっとづつデータを読み込み
                        nReadIndex = 0
                        While (True)
                            ' テーブルからバイナリデータの部分読み出し
                            Dim nOneReadByte As Long = reader.GetBytes(0, nReadIndex, readByte, 0, nReadSize)
                            If nOneReadByte <= 0 Then
                                ' データ無し
                                Return -4
                            End If

                            ' ファイルに書き込み
                            stream.Write(readByte, 0, CInt(nOneReadByte))

                            ' 全部 読み込んだ？
                            nReadIndex += nOneReadByte
                            If nBaynarySize <= nReadIndex Then
                                Exit While
                            End If

                            ' キャンセルチェック
                            If bCancelFlg Then
                                Return -1
                            End If
                        End While
                    End Using
                End Using
                Return 0
            Catch ex As Exception
                Return -99
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
                    '''' 20140514  バイト配列時に CStr(m_Command.Parameters.Item(ii).Value) でThrow発生。対策として、コメント化。もともとsyslogは止めていたので
                    'Dim strVal As String = ""
                    'For ii As Integer = 0 To m_Command.Parameters.Count - 1
                    '    strVal &= m_Command.Parameters.Item(ii).ToString & "=" & CStr(m_Command.Parameters.Item(ii).Value) & ", "
                    'Next
                    ''tClass_SysLog.Syslog("▲ [tClass_SQLServer::DMLExecute] (param)" & strVal)
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
        Public Overridable Function DMLExecute(ByVal strSQL As String, ByVal para() As SqlClient.SqlParameter) As Integer
            Try
                '' パラメータ追加
                m_Command.Parameters.Clear()
                Dim p As SqlClient.SqlParameter
                For Each p In para
                    m_Command.Parameters.Add(p)
                Next p

                Return DMLExecute(strSQL)
            Catch ex As Exception
                Return Nothing
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

        '********************************************************************
        'SQL文生成 補助
        '********************************************************************
        '作りかけ
        'Public Shared Sub sqlColumnInsert(ByVal strSql As String, ByVal val As String, ByVal size As Integer)
        '    If val.Length = 0 Then
        '        strSql &= "NULL,"
        '    Else
        '        strSql &= "'" & val.Substring(0, size) & "'"
        '    End If
        'End Sub
        'Public Shared Sub sqlColumnInsert(ByVal strSql As String, ByVal val As Integer)
        '    strSql &= val.ToString
        'End Sub

        'Public Shared Sub sqlColumnUpdate(ByVal strSql As String, ByVal name As String, ByVal val As String, ByVal size As Integer)
        '    If val.Length = 0 Then
        '        strSql &= name & "=NULL,"
        '    Else
        '        strSql &= name & "='" & val.Substring(0, size) & "'"
        '    End If
        'End Sub

        ''' <summary>
        ''' インサートの補助
        ''' </summary>
        Public Shared Function sqlAddInsert(ByVal val As Integer) As String
            Return String.Format("{0},", val)
        End Function
        Public Shared Function sqlAddInsert(ByVal val As Double) As String
            Return String.Format("{0:0.000000},", val)
        End Function
        Public Shared Function sqlAddInsert(ByVal val As String) As String
            Return String.Format("'{0}',", Val)
        End Function
        Public Shared Function sqlAddInsert_Text(ByVal val As String) As String
            Return String.Format("{0},", val)
        End Function
        ''' <summary>
        ''' アップデートの補助
        ''' </summary>
        Public Shared Function sqlAddUpdate(ByVal name As String, ByVal val As Integer) As String
            Return String.Format("{0}={1},", name, val)
        End Function
        Public Shared Function sqlAddUpdate(ByVal name As String, ByVal val As String) As String
            Return String.Format("{0}='{1}',", name, val)
        End Function
        Public Shared Function sqlAddUpdate_Text(ByVal name As String, ByVal val As String) As String
            Return String.Format("{0}={1},", name, val)
        End Function
        ''' <summary>
        ''' 時刻追加の補助
        ''' </summary>
        Public Shared Function sqlColumnAddTime() As String
            Return String.Format("'{0:yyyy/MM/dd HH:mm:ss}'", Now)
        End Function



        ''' <summary>
        ''' UpDateの列コピー時のサポート
        ''' </summary>
        ''' <param name="name">列名</param>
        ''' <param name="AsName">コピー元の列名のAs</param>
        ''' <returns>生成文字列</returns>
        ''' <remarks></remarks>
        Public Shared Function sqlColumnUpdate(ByVal name As String, ByVal AsName As String) As String
            Return String.Format("[{0}]={1}.[{0}],", name, AsName)
        End Function

        ''' <summary>
        ''' 最後の "," を取り除く
        ''' </summary>
        ''' <param name="strSQL">SQL文</param>
        ''' <remarks></remarks>
        Public Shared Sub sqlColumnTrim(ByRef strSQL As String)
            If strSQL.EndsWith(",") Then
                strSQL = strSQL.Remove(strSQL.Length - 1, 1)
            End If
        End Sub




        '********************************************************************
        '簡易実行
        '********************************************************************
        ''' <summary>
        ''' 単一行数Select文の実行
        ''' </summary>
        ''' <param name="strSQL"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overridable Function GetSelectKey(ByVal strSQL As String, ByRef Ans As Integer) As Boolean
            Ans = 0
            Dim sqlRead As SqlClient.SqlDataReader = SelectExecute(strSQL)
            If sqlRead Is Nothing Then Return False
            Try
                sqlRead.Read()
                Ans = sqlRead.GetInt32(0)
                Return True
            Catch ex As Exception
                Return False
            Finally
                sqlRead.Close()
            End Try
        End Function
        Public Overridable Function GetSelectKey(ByVal strSQL As String, ByRef Ans As String) As Boolean
            Ans = ""
            Dim sqlRead As SqlClient.SqlDataReader = SelectExecute(strSQL)
            If sqlRead Is Nothing Then Return False
            Try
                sqlRead.Read()
                Ans = sqlRead.GetString(0)
                Return True
            Catch ex As Exception
                Return False
            Finally
                sqlRead.Close()
            End Try
        End Function
        Public Overridable Function GetSelectKey(ByVal strSQL As String) As Object
            Dim sqlRead As SqlClient.SqlDataReader = SelectExecute(strSQL)
            If sqlRead Is Nothing Then Return False
            Try
                sqlRead.Read()
                Return sqlRead.GetValue(0)
            Catch ex As Exception
                Return Nothing
            Finally
                sqlRead.Close()
            End Try
        End Function


        '********************************************************************
        ' NULLチェック込みリーダー読み込み
        '********************************************************************

        Public Overloads Shared Function GetDbValT(Of T)(ByVal sqlRead As System.Data.Common.DbDataReader, ByVal nIndex As Integer, ByVal DefVal As T) As T
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return CType(sqlRead.GetValue(nIndex), T)
        End Function

        Public Overloads Shared Function GetDbValT(Of T)(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As T) As T
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbValT(sqlRead, nIndex, DefVal)
        End Function


        ''' <summary>
        ''' リーダーからNULLをチェックして値を読み込んで結果を返す
        ''' </summary>
        ''' <param name="sqlRead">リーダー</param>
        ''' <param name="nIndex">列No(0オリジン)</param>
        ''' <param name="DefVal">デフォルト値</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As String) As String
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetString(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Integer) As Integer
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetInt32(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Short) As Short
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetInt16(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Double) As Double
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetDouble(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Single) As Single
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetFloat(nIndex)
        End Function
        Public Overloads Shared Function GetDbNameToIndex(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String) As Integer
            Try
                Return sqlRead.GetOrdinal(sName)
            Catch ex As Exception
                Return -1
            End Try
        End Function
        ''' <summary>
        ''' リーダーからNULLをチェックして値を読み込んで結果を返す
        ''' </summary>
        ''' <param name="sqlRead">リーダー</param>
        ''' <param name="sName">列名</param>
        ''' <param name="DefVal">デフォルト値</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As String) As String
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Integer) As Integer
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Short) As Short
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Double) As Double
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Single) As Single
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function


    End Class
End Namespace