'*********************************************************************************
'ソートを定義する基本クラス
'	[Ver]
'		Ver.01    2006/01/28  vs2005 対応
'		Ver.02    2006/01/28  vs2003 対応
'
'	[メモ]
'		ちょっと パフォーマンスが悪い けど 汎用性重視に作った(vs2005では問題解消)
'*********************************************************************************
'Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Strict Off
Imports System.Windows.Forms

#Region "サンプル"
'''''必ずインターフェースを実装させる事
''''Public Class frmMain
''''    Inherits System.Windows.Forms.Form
''''    Implements tClass.tIface_Comparer

''''    ''' コール元
''''    Private Sub ListViewCoilSrach_ColumnClick(ByVal sender As Object, ByVal e As System.Windows.Forms.ColumnClickEventArgs) Handles ListViewCoilSrach.ColumnClick
''''        ''' ソート
''''        ListViewCoilSrach.ListViewItemSorter = New tClass.tClass_Comparer(Me, e.Column, tClass.tClass_Comparer.ComparerType.str, oldOrder)
''''    End Sub
''''    '''検索条件指定 (ソートを実行したらこれが要素数分コールされる)
''''    Public Function Comparer_InfSet(ByVal x As Object, ByVal y As Object, ByVal KeyIndex As Integer, ByRef setX As Object, ByRef setY As Object) As Integer Implements tClass.tIface_Comparer.Comparer_InfSet
''''        setX = CType(x, ListViewItem).SubItems(KeyIndex).Text
''''        setY = CType(y, ListViewItem).SubItems(KeyIndex).Text
''''    End Function
''''End Class
#End Region

Namespace tClass

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic対応]
    '///////////////////////////////////////////////////////////////////////////////
#If VBC_VER >= 8.0 Then                         ' Generic に対応しているバージョン時

    ''' <summary>
    ''' 検索本体 (検索値を検索する)
    ''' </summary>
    ''' <typeparam name="KEY">検索対象のオブジェクト</typeparam>
    ''' <typeparam name="SERCH">実際に比較する値</typeparam>
    ''' <remarks></remarks>
    Public Class tClass_Comparer(Of KEY, SERCH)
        Implements System.Collections.Generic.IComparer(Of KEY)        'IComparerインターフェイスを実装する

        '********************************************************************
        '公開変数定義
        '********************************************************************
        ''' <summary>
        ''' 比較型定義
        ''' </summary>
        ''' <remarks></remarks>
        Public Enum iComparerType
            ''' <summary>数値</summary>
            int = 0
            ''' <summary>文字列</summary>
            str
            ''' <summary>日付</summary>
            day
            ''' <summary>ファイルの日付</summary>
            fileDay
        End Enum

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private m_comparerMethod As tIface.tIface_Comparer(Of KEY, SERCH)      'コール先メソッド インターフェース

        Private m_Order As System.Windows.Forms.SortOrder       '並び替える方法 (昇順、降順)
        Private m_KeyIndex As Integer                           '並び替える項目 (0オリジン)
        Private m_Type As iComparerType							'比較型

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="comparerMethod">コール先メソッド インターフェース</param>
        ''' <param name="keyIndex">並び替える項目(0オリジン)</param>
        ''' <param name="type">比較型</param>
        ''' <param name="order">並び替える方法(昇順、降順)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal comparerMethod As tIface.tIface_Comparer(Of KEY, SERCH), ByVal keyIndex As Integer, ByVal type As iComparerType, Optional ByVal order As SortOrder = SortOrder.Ascending)
            m_comparerMethod = comparerMethod
            m_KeyIndex = keyIndex
            m_Type = Type
            m_Order = order
        End Sub

        ''' <summary>
        ''' 比較方法 (sortにより実装)
        ''' </summary>
        ''' <param name="x">比較対照</param>
        ''' <param name="y">比較対照</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As KEY, ByVal y As KEY) As Integer Implements System.Collections.Generic.IComparer(Of KEY).Compare
            Dim retc As Double              '検索結果 xがyより小さいときはマイナスの数、大きいときはプラスの数、同じときは0を返す
            Dim setX As SERCH = Nothing
            Dim setY As SERCH = Nothing
            '' インターフェース 定義元 メソッド呼び出し
            m_comparerMethod.Comparer_InfSet(x, y, m_KeyIndex, setX, setY)

            '' xがyより小さいときはマイナスの数、大きいときはプラスの数、同じときは0を返す
            '''''''' 良いやり方が不明 とりあえず、一度Objectに変換する (ボックス化、ボックス化解除が多発するので辞めたい)
            Select Case m_Type
                Case iComparerType.int
                    If Not IsNumeric(setX) Then setX = CObj(Double.MinValue)
                    If Not IsNumeric(setY) Then setY = CObj(Double.MinValue)
                    retc = CDbl(CObj(setX)) - CDbl(CObj(setY))

                Case iComparerType.str
                    retc = String.Compare(CStr(CObj(setX)), CStr(CObj(setY)))
                Case iComparerType.day
                    retc = Date.Compare(CDate(CObj(setX)), CDate(CObj(setY)))
                Case iComparerType.fileDay
                    retc = DateTime.Compare(CObj(setX), CObj(setY))
            End Select

            '' 降順のときは+-を逆転させる
            If m_Order = SortOrder.Descending Then retc = -retc

            '' オーバーフロー対策
            Dim iretc As Integer
            If retc < Integer.MinValue Then
                iretc = Integer.MinValue
            ElseIf retc > Integer.MaxValue Then
                iretc = Integer.MaxValue
            Else
                iretc = CInt(retc)
            End If
            Return iretc

        End Function

    End Class
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic非対応]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検索本体 (検索値を検索する)
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Comparer
        Implements System.Collections.IComparer        'IComparerインターフェイスを実装する

        '********************************************************************
        '公開変数定義
        '********************************************************************
        ''' <summary>
        ''' 比較型定義
        ''' </summary>
        ''' <remarks></remarks>
        Public Enum iComparerType
            ''' <summary>数値</summary>
            int = 0
            ''' <summary>文字列</summary>
            str
            ''' <summary>日付</summary>
            day
            ''' <summary>ファイルの日付</summary>
            fileDay
        End Enum

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private m_comparerMethod As tIface.tIface_Comparer      'コール先メソッド インターフェース

        Private m_Order As System.Windows.Forms.SortOrder       '並び替える方法 (昇順、降順)
        Private m_KeyIndex As Integer                           '並び替える項目 (0オリジン)
        Private m_Type As iComparerType                         '比較型

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="comparerMethod">コール先メソッド インターフェース</param>
        ''' <param name="keyIndex">並び替える項目(0オリジン)</param>
        ''' <param name="type">比較型</param>
        ''' <param name="order">並び替える方法(昇順、降順)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal comparerMethod As tIface.tIface_Comparer, ByVal keyIndex As Integer, ByVal type As iComparerType, Optional ByVal order As SortOrder = SortOrder.Ascending)
            m_comparerMethod = comparerMethod
            m_KeyIndex = keyIndex
            m_Type = type
            m_Order = order
        End Sub


        ''' <summary>
        ''' 比較方法 (sortにより実装)
        ''' </summary>
        ''' <param name="x">比較対照</param>
        ''' <param name="y">比較対照</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As Object, ByVal y As Object) As Integer Implements IComparer.Compare
            Dim retc As Double              '検索結果 xがyより小さいときはマイナスの数、大きいときはプラスの数、同じときは0を返す
            Dim setX As Object = Nothing
            Dim setY As Object = Nothing
            '' インターフェース 定義元 メソッド呼び出し
            m_comparerMethod.Comparer_InfSet(x, y, m_KeyIndex, setX, setY)

            '' xがyより小さいときはマイナスの数、大きいときはプラスの数、同じときは0を返す
            Select Case m_Type
                Case iComparerType.int
                    If Not IsNumeric(setX) Then setX = Double.MinValue
                    If Not IsNumeric(setY) Then setY = Double.MinValue
                    retc = CDbl(setX) - CDbl(setY)
                Case iComparerType.str
                    retc = String.Compare(CStr(setX), CStr(setY))
                Case iComparerType.day
                    retc = Date.Compare(CDate(setX), CDate(setY))
                Case iComparerType.fileDay
                    retc = DateTime.Compare(CType(setX, DateTime), CType(setY, DateTime))
            End Select

            '' 降順のときは+-を逆転させる
            If m_Order = SortOrder.Descending Then retc = -retc

            '' オーバーフロー対策
            Dim iretc As Integer
            If retc < Integer.MinValue Then
                iretc = Integer.MinValue
            ElseIf retc > Integer.MaxValue Then
                iretc = Integer.MaxValue
            Else
                iretc = CInt(retc)
            End If
            Return iretc
        End Function
    End Class
End Namespace
