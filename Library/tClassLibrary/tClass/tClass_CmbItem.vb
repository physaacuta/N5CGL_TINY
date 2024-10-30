'*********************************************************************************
'コンボボックスに表示する為のToStringを実装したクラス
'	[Ver]
'		Ver.01    2009/01/26  vs2005 対応
'
'	[メモ]
'		今まで使う所で勝手に宣言していたが、思ったより良く使用する為、クラスに格上げ
'*********************************************************************************
Option Strict On

''' <summary>
''' コンボボックスに表示するデータ (いろんなデータに対応できるように構成しておく)
''' </summary>
Public Class tClass_CmbItem
    ''' <summary>Int主キー</summary>
    Public g_nKey As Integer							' Int主キー
    ''' <summary>String主キー</summary>
    Public g_sKey As String								' String主キー

    ''' <summary>紐付く名称(ToStringで返却される文字列)</summary>
    Public g_sName As String							' 紐付く名称

    ''' <summary>Intアイテム</summary>
    Public g_nItem As Integer							' Intアイテム
    ''' <summary>Stringアイテム</summary>
    Public g_sItem As String							' Stringアイテム


    ''' <summary>
    ''' コンストラクタ (Int主キー)
    ''' </summary>
    ''' <param name="nKey">主キー</param>
    ''' <param name="strName">紐付く名称</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal nKey As Integer, ByVal strName As String)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = 0
        g_sItem = ""
    End Sub
    Public Sub New(ByVal nKey As Integer, ByVal strName As String, ByVal nItem As Integer)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = nItem
        g_sItem = ""
    End Sub
    Public Sub New(ByVal nKey As Integer, ByVal strName As String, ByVal sItem As String)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = 0
        g_sItem = sItem
    End Sub
    Public Sub New(ByVal nKey As Integer, ByVal strName As String, ByVal nItem As Integer, ByVal sItem As String)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = nItem
        g_sItem = sItem
    End Sub


    ''' <summary>
    ''' コンストラクタ (String主キー)
    ''' </summary>
    ''' <param name="sKey">主キー</param>
    ''' <param name="strName">紐付く名称</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal sKey As String, ByVal strName As String)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = 0
        g_sItem = ""
    End Sub
    Public Sub New(ByVal sKey As String, ByVal strName As String, ByVal nItem As Integer)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = nItem
        g_sItem = ""
    End Sub
    Public Sub New(ByVal sKey As String, ByVal strName As String, ByVal sItem As String)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = 0
        g_sItem = sItem
    End Sub
    Public Sub New(ByVal sKey As String, ByVal strName As String, ByVal nItem As Integer, ByVal sItem As String)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = nItem
        g_sItem = sItem
    End Sub


    ''' <summary>
    ''' 構造体の表示名称
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function ToString() As String
        Return g_sName
    End Function
End Class
