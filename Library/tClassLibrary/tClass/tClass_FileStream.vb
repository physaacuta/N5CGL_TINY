'*********************************************************************************
'テキストファイル読み書き関係の処理を行う基本クラス
'	[Ver]
'		Ver.01    2003/09/15  vs2002 対応
'		Ver.02    2007/03/06  vs2005 対応
'
'	[メモ]
'		かなり初期に作ったから、あんまり信用しないで下さい。
'*********************************************************************************
Option Strict On				'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass
    Public Class tClass_FileStream
        Implements IDisposable							'デストラクタ定義

        '********************************************************************
        'デリゲート定義
        '********************************************************************
        Private Delegate Sub tDele_OpenDelegate()		'オープンデリゲート
        Private m_OpenMethod As tDele_OpenDelegate		'オープン時の起動メッソド

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private m_Writer As System.IO.StreamWriter		'ファイル書込みクラス
        Private m_Reader As System.IO.StreamReader		'ファイル読込みクラス
        Private m_Stream As System.IO.FileStream		'ファイルストリームクラス

        Private m_FPath As String						'ファイルフルパス
        Private m_Mode As System.IO.FileMode			'ファイルオープンモード
        Private m_Access As System.IO.FileAccess		'ファイルオープン時のアクセス権
        Private m_Share As System.IO.FileShare			'ファイルオープン時の他プロセスからの共有状態

        Private m_Encoding As System.Text.Encoding

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="FPath">ファイルフルパス</param>
        ''' <param name="fMode">ファイルオープンモード</param>
        ''' <param name="fAccess">ファイルオープン時のアクセス権</param>
        ''' <param name="fShare">ファイルオープン時の他プロセスからの共有状態</param>
        ''' <param name="encoding">エンコード</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal FPath As String, _
                        Optional ByVal fMode As System.IO.FileMode = System.IO.FileMode.OpenOrCreate, _
                        Optional ByVal fAccess As System.IO.FileAccess = System.IO.FileAccess.Read, _
                        Optional ByVal fShare As System.IO.FileShare = System.IO.FileShare.Read, _
                        Optional ByVal encoding As System.text.Encoding = Nothing)

            '' プライベート変数にセット
            m_FPath = FPath
            m_Mode = fMode
            m_Access = fAccess
            m_Share = fShare
            If encoding Is Nothing Then
                m_Encoding = System.Text.Encoding.GetEncoding("shift-jis")
            Else
                m_Encoding = encoding
            End If

            '' オープン時のメッソド割り当て
            Select Case fAccess
                Case IO.FileAccess.Read
                    m_OpenMethod = New tDele_OpenDelegate(AddressOf OpenRead)
                Case IO.FileAccess.Write
                    m_OpenMethod = New tDele_OpenDelegate(AddressOf OpenWrite)
                Case IO.FileAccess.ReadWrite
                    m_OpenMethod = New tDele_OpenDelegate(AddressOf OpenReadWrite)
            End Select
        End Sub

        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Dispose() Implements IDisposable.Dispose
            m_OpenMethod = Nothing
            '' ファイルクローズ
            Call Close()
            '' GCにFinalizeを行わないように通知
            GC.SuppressFinalize(Me)
        End Sub

        ''' <summary>
        ''' Finalizeデストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overrides Sub Finalize()
            m_OpenMethod = Nothing
            '' ファイルクローズ
            Call Close()
        End Sub

        ''' <summary>
        ''' 外部公開 ファイルオープン
        ''' </summary>
        ''' <remarks>コンストラクタで指定したアクセスモードにより起動メソッド変更</remarks>
        Public Sub Open()
            Try
                '' コンストラクタで指定したアクセスモードにより起動メソッド変更
                m_OpenMethod.Invoke()
            Catch ex As Exception
                m_Writer = Nothing
                m_Reader = Nothing
                m_Stream = Nothing
                Throw New System.Exception("Stream コンストラクタ異常", ex)
            End Try
        End Sub

        ''' <summary>
        ''' ファイルオープン用インスタンス生成
        ''' </summary>
        ''' <remarks></remarks>
        Protected Sub OpenRead()
            '' ファイルストリームセット
            m_Stream = New System.IO.FileStream(m_FPath, m_Mode, m_Access, m_Share)
            '' 読み書き用クラスセット
            m_Reader = New System.IO.StreamReader(m_Stream, m_Encoding)
        End Sub
        Protected Sub OpenWrite()
            '' ファイルストリームセット
            m_Stream = New System.IO.FileStream(m_FPath, m_Mode, m_Access, m_Share)
            '' 読み書き用クラスセット
            m_Writer = New System.IO.StreamWriter(m_Stream, m_Encoding)
            m_Writer.AutoFlush = True		 'バッファに蓄えず直に出力
        End Sub
        Protected Sub OpenReadWrite()
            '' ファイルストリームセット
            m_Stream = New System.IO.FileStream(m_FPath, m_Mode, m_Access, m_Share)
            '' 読み書き用クラスセット
            m_Reader = New System.IO.StreamReader(m_Stream, m_Encoding)
            m_Writer = New System.IO.StreamWriter(m_Stream, m_Encoding)
            m_Writer.AutoFlush = True		 'バッファに蓄えず直に出力
        End Sub

        ''' <summary>
        ''' ファイルクローズ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Close()
            Try
                '' オープンファイルクローズ
                If Not Nothing Is m_Writer Then
                    m_Writer.Close()
                    m_Writer = Nothing
                End If
                If Not Nothing Is m_Reader Then
                    m_Reader.Close()
                    m_Reader = Nothing
                End If
                If Not Nothing Is m_Stream Then
                    m_Stream.Close()
                    m_Stream = Nothing
                End If
            Catch ex As Exception
                Throw New System.Exception("Stream Close 異常", ex)
            End Try
        End Sub

        ''' <summary>
        ''' 1行ファイル読込み
        ''' </summary>
        ''' <returns>1行分の文字列</returns>
        ''' <remarks> 取得行無し時、または読込み異常は Nothingが帰る
        ''' 		  1行読込むと、次の行にフェッチする ∴WhileでNothingが帰るまで読込んでもOK</remarks>
        Public Overridable Overloads Function ReadLine() As String
            Try
                '' 1行読込み (読込み行無し時 Nothing)
                Return m_Reader.ReadLine()
            Catch
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' 全ファイル読込み
        ''' </summary>
        ''' <returns>行数分の文字列</returns>
        ''' <remarks></remarks>
        Public Overridable Overloads Function ReadAll() As ArrayList
            Try
                '' 全行をリストに追加
                Dim ary As New ArrayList
                Do While m_Reader.Peek() >= 0
                    ary.Add(m_Reader.ReadLine())
                Loop
                Return ary
            Catch
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' 1行ファイル書込み
        ''' </summary>
        ''' <param name="strVal">ファイルフルパス</param>
        ''' <returns>True:正常 False:異常</returns>
        ''' <remarks></remarks>
        Public Overridable Overloads Function WriteLine(ByVal strVal As String) As Boolean
            Try
                '' ファイル書込み
                m_Writer.WriteLine(strVal)
                Return True
            Catch
                Return False
            End Try
        End Function
        Public Overridable Overloads Function WriteLine(ByVal strVal As String()) As Boolean
            Try
                '' ファイル書込み
                For ii As Integer = 0 To strVal.Length - 1
                    m_Writer.WriteLine(strVal(ii))
                Next ii
                Return True
            Catch
                Return False
            End Try
        End Function
        Public Overridable Overloads Function WriteLine(ByVal strVal As ArrayList) As Boolean
            Try
                '' ファイル書込み
                For ii As Integer = 0 To strVal.Count - 1
                    m_Writer.WriteLine(strVal(ii))
                Next ii
                Return True
            Catch
                Return False
            End Try
        End Function
    End Class
End Namespace

