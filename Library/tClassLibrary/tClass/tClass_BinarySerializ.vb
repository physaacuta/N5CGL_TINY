'*********************************************************************************
'クラスの状態をそのままバイナリファイルで保存/読み込みを行う基底クラス
'	[Ver]
'		Ver.01    2005/12/24  vs2005 対応
'		Ver.02    2006/03/04  バッファにバイナリセット
'
'	[メモ]
'		シリアル化を行うクラスは<System.Serializable()>属性に設定する必要がある
'*********************************************************************************
Option Strict On                'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass
    ''' <summary>
    ''' クラスの状態をそのままバイナリファイルで保存/読み込みを行う基底クラス
    ''' </summary>
    ''' <remarks>シリアル化を行うクラスは"System.Serializable()"属性に設定する必要がある</remarks>
    Public Class tClass_BinarySerializ
        '********************************************************************
        'メソッド定義
        '********************************************************************

        ''' <summary>
        ''' シリアル化する
        ''' </summary>
        ''' <param name="objVal">シリアル化するインスタンス</param>
        ''' <param name="strFPath">保存フルパス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>拡張子は基本的に.binにしておこう</remarks>
        Public Overloads Shared Function Save(ByRef objVal As Object, ByVal strFPath As String) As Boolean
            Dim strFolder As String = IO.Path.GetDirectoryName(strFPath)
            Dim stream As IO.FileStream                                         'ストリーム

            '''' 指定フォルダチェック
            Call tMod.CheckFolder(strFolder)
            '''' シリアル化
            Try
                '' ストリーム オープン
                stream = New IO.FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.Write)
            Catch ex As Exception
                Return False
            End Try
            Try
                '' バイナリフォーマッタ 作成
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                '' シリアル化
                formatter.Serialize(stream, objVal)
                Return True
            Catch ex As Exception
                Return False
            Finally
                '' クローズ
                stream.Close()
            End Try
        End Function

        ''' <param name="objVal">シリアル化するインスタンス</param>
        ''' <param name="strFolder">保存フォルダパス</param>
        ''' <param name="strName">保存ファイル名</param>
        ''' <returns>復帰情報</returns>
        Public Overloads Shared Function Save(ByRef objVal As Object, ByVal strFolder As String, ByVal strName As String) As Boolean
            Dim strFPath As String = IO.Path.Combine(strFolder, strName)        'フルパスに変換
            Return Save(objVal, strFPath)
        End Function

        ''' <summary>
        ''' 逆シリアル化する
        ''' </summary>
        ''' <param name="objVal">逆シリアル化したインスタンスを格納するオブジェクト</param>
        ''' <param name="strFPath">保存フルパス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function Load(ByRef objVal As Object, ByVal strFPath As String) As Boolean
            Dim stream As IO.FileStream                                         'ストリーム

            '''' ファイルチェック
            objVal = Nothing
            If Not IO.File.Exists(strFPath) Then Return False

            '''' 逆シリアル化
            Try
                '' ストリーム オープン
                stream = New IO.FileStream(strFPath, IO.FileMode.Open, IO.FileAccess.Read)
            Catch ex As Exception
                Return False
            End Try
            Try
                '' バイナリフォーマッタ 作成
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                objVal = formatter.Deserialize(stream)
                Return True
            Catch ex As Exception
                Return False
            Finally
                '' クローズ
                stream.Close()
            End Try
        End Function

        ''' <param name="objVal">逆シリアル化したインスタンスを格納するオブジェクト</param>
        ''' <param name="strFolder">保存フォルダパス</param>
        ''' <param name="strName">保存ファイル名</param>
        ''' <returns>復帰情報</returns>
        Public Overloads Shared Function Load(ByRef objVal As Object, ByVal strFolder As String, ByVal strName As String) As Boolean
            Dim strFPath As String = IO.Path.Combine(strFolder, strName)        'フルパスに変換
            Return Load(objVal, strFPath)
        End Function

        ''' <summary>
        ''' MemoryStreamにシリアル化した情報をセットする
        ''' </summary>
        ''' <param name="objVal">シリアル化する情報</param>
        ''' <param name="memS">MemoryStreamオブジェクト</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>MemoryStreamは、呼び元でインスタンスを生成しておく事</remarks>
        Public Overloads Shared Function SaveBuffer(ByRef objVal As Object, ByVal memS As IO.MemoryStream) As Boolean
            '' ストリーム生成済み？
            If memS Is Nothing Then Return False

            Try
                '' バイナリフォーマッタ 作成
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                '' シリアル化
                formatter.Serialize(memS, objVal)
                memS.Position = 0
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' MemoryStreamの情報を逆シリアル化する
        ''' </summary>
        ''' <param name="objVal">逆シリアル化するインスタンス</param>
        ''' <param name="memS">MemoryStreamオブジェクト</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>使用後は、memSをクローズする事。MemoryStreamのPositionを正確な位置にしておく事</remarks>
        Public Overloads Shared Function LoadBuffer(ByRef objVal As Object, ByVal memS As IO.MemoryStream) As Boolean
            Try
                '' バイナリフォーマッタ 作成
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                objVal = formatter.Deserialize(memS)
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function
    End Class
End Namespace
