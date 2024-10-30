'*********************************************************************************
'バイナリファイル読み書き関係の処理を行う基本クラス (基本的にVB6と同じように作っている)
'	[Ver]
'		Ver.01    2003/06/18  vs2002 対応
'		Ver.02    2007/03/06  vs2005 対応
'
'	[メモ]
'		かなり初期に作ったから、あんまり信用しないで下さい。
'
'		構造体を使用する場合は、以下のサンプルの様に構造体を指定する
'
    ' '' 構造体サンプル
    '<StructLayout(LayoutKind.Sequential, Size:=1200, pack:=1)> _
    'Structure SAMPLE
    '	<VBFixedArray(99), MarshalAs(UnmanagedType.ByValArray, SizeConst:=100)> _
    '	Dim dataL() As Integer

    '	<VBFixedString(400), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=400)> _
    '	Dim dataS As String

    '	<VBFixedArray(399), MarshalAs(UnmanagedType.ByValArray, SizeConst:=400)> _
    '	Dim dataB() As Byte

    '	Public Sub Inisialize()
    '		ReDim dataL(99)
    '		ReDim dataB(399)
    '	End Sub
    'End Structure
'*********************************************************************************
Option Strict On				'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass
    Public Class tClass_FileBinary

        Private m_nFreeNo As Integer				' フリーファイルＮｏ

        ''' <summary>
        ''' フリーファイルＮｏを取得する
        ''' </summary>
        ''' <returns>0以上:フリーファイルＮｏ   -1:異常</returns>
        ''' <remarks></remarks>
        Public Shared Function intFreeFileNo() As Integer
            'フリーファイルＮｏを取得する
            Try
                Return FreeFile()
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' バイナリファイルを開く
        ''' </summary>
        ''' <param name="strFullPath">フルパス</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileOpen(ByVal strFullPath As String) As Integer

            'ランダムアクセスモードでファイルを開く
            Try
                m_nFreeNo = intFreeFileNo()
                If m_nFreeNo = -1 Then Return -9

                FileOpen(m_nFreeNo, strFullPath, OpenMode.Binary, OpenAccess.ReadWrite, OpenShare.Shared)
                Return 0
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' ファイルを閉じる
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileClose() As Integer
            'ファイルを閉じる
            Try
                FileClose(m_nFreeNo)
                Return 0
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' ファイルデータを読込む
        ''' </summary>
        ''' <param name="objData">値型の基本クラス</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileRead(ByRef objData As System.ValueType) As Integer
            'ランダムアクセスモードでファイルを読込む
            Try
                FileGet(m_nFreeNo, objData)
                Return 0
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' ランダムアクセスモードでファイルデータを書込む
        ''' </summary>
        ''' <param name="objData">値型の基本クラス</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileWrite(ByVal objData As System.ValueType) As Integer
            'ランダムアクセスモードでファイルを書込む
            Try
                FilePut(m_nFreeNo, objData)
                Return 0
            Catch
                Return -1
            End Try
        End Function

    End Class
End Namespace