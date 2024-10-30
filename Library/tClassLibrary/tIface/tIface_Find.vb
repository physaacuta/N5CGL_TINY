'*********************************************************************************
'検索用インターフェース
'	[Ver]
'		Ver.01    2005/12/24  vs2005 対応
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
    ''' Findのインターフェース [Generic対応]
    ''' </summary>
    ''' <typeparam name="KEY">検索キー</typeparam>
    ''' <remarks>tClass_Array の呼び元クラスで Implements tIface_Find を定義する事</remarks>
    Public Interface tIface_Find(Of KEY)
        ''' <summary>
        ''' 要素リストから指定キーで検索を行い、検索結果の配列位置を戻す 処理を実装する
        ''' </summary>
        ''' <param name="gnrKey">検索キー</param>
        ''' <param name="aryList">要素リスト</param>
        ''' <returns>検索結果を返す 0オリジンの配列位置 -1:該当無し</returns>
        ''' <remarks></remarks>
        Function Find(ByVal gnrKey As KEY, ByVal aryList As ArrayList) As Integer

    End Interface
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic非対応]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' Findのインターフェース
    ''' </summary>
    ''' <remarks>tClass_Array の呼び元クラスで Implements tIface_Find を定義する事</remarks>
    Public Interface tIface_Find
        ''' <summary>
        ''' 要素リストから指定キーで検索を行い、検索結果の配列位置を戻す 処理を実装する
        ''' </summary>
        ''' <param name="gnrKey">検索キー</param>
        ''' <param name="aryList">要素リスト</param>
        ''' <returns>検索結果を返す 0オリジンの配列位置 -1:該当無し</returns>
        ''' <remarks></remarks>
        Function Find(ByVal gnrKey As Object, ByVal aryList As ArrayList) As Integer

    End Interface
End Namespace