'*********************************************************************************
'ソートを定義する基本インターフェース
'	[Ver]
'		Ver.01    2006/01/28  vs2005 対応
'		Ver.02    2006/01/28  vs2003 対応
'
'	[メモ]
'
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tIface
    '///////////////////////////////////////////////////////////////////////////////
    '[Generic対応]
    '///////////////////////////////////////////////////////////////////////////////
#If VBC_VER >= 8.0 Then                         ' Generic に対応しているバージョン時
    ''' <summary>
    ''' 検索情報 呼び出し用 インターフェース [Generic対応]
    ''' </summary>
    ''' <typeparam name="KEY">検索対象のオブジェクト</typeparam>
    ''' <typeparam name="SERCH">実際に比較する値</typeparam>
    ''' <remarks></remarks>
    Public Interface tIface_Comparer(Of KEY, SERCH)
        ''' <summary>
        ''' 要素から指定キーで検索 (ソートを実行したらこれが要素数分コールされる)
        ''' </summary>
        ''' <param name="x">検索オブジェクト</param>
        ''' <param name="y">検索オブジェクト</param>
        ''' <param name="KeyIndex">検索キー配列位置</param>
        ''' <param name="setX">検索キーセット結果</param>
        ''' <param name="setY">検索キーセット結果</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Function Comparer_InfSet(ByVal x As KEY, ByVal y As KEY, ByVal KeyIndex As Integer, ByRef setX As SERCH, ByRef setY As SERCH) As Integer

    End Interface
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic非対応]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検索情報 呼び出し用 インターフェース
    ''' </summary>
    ''' <remarks></remarks>
    Public Interface tIface_Comparer
        ''' <summary>
        ''' 要素から指定キーで検索 (ソートを実行したらこれが要素数分コールされる)
        ''' </summary>
        ''' <param name="x">検索オブジェクト</param>
        ''' <param name="y">検索オブジェクト</param>
        ''' <param name="KeyIndex">検索キー配列位置</param>
        ''' <param name="setX">検索キーセット結果</param>
        ''' <param name="setY">検索キーセット結果</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Function Comparer_InfSet(ByVal x As Object, ByVal y As Object, ByVal KeyIndex As Integer, ByRef setX As Object, ByRef setY As Object) As Integer

    End Interface
End Namespace