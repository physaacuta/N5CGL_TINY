'*********************************************************************************
'ScrollBarの独自クラス
'	[Ver]
'		Ver.01    2009/03/08  vs2005 対応
'
'	[メモ]
'		スクロールバーでプログラム変更によるValueChangedで判断できるようにした。
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Windows.Forms						' コントロール

Namespace tCnt
    Public Class tCnt_HScrollBar
        Inherits HScrollBar

        ''' <summary>
        ''' 継承したValueChangedイベント
        ''' </summary>
        ''' <param name="sender">通常と同じ</param>
        ''' <param name="e">ValueProgramableで変更時:True  通常:false</param>
        ''' <remarks></remarks>
        Public Shadows Event ValueChanged(ByVal sender As Object, ByVal e As Boolean)


        ''' <summary>
        ''' 内部判定用
        ''' </summary>
        ''' <remarks></remarks>
        Protected IsValueChangedProg As Boolean = False


        ''' <summary>
        ''' オーバーライド用。元のOnValueChangedを横取りして、イベントを発生させる
        ''' </summary>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Protected Overrides Sub OnValueChanged(ByVal e As EventArgs)
            If IsValueChangedProg Then
                IsValueChangedProg = False
                RaiseEvent ValueChanged(Me, True)
            Else
                RaiseEvent ValueChanged(Me, False)
            End If
        End Sub


        ''' <summary>
        ''' プログラム中で値を変更。ValueChangedのeでTrueとなる。
        ''' </summary>
        ''' <value></value>
        ''' <remarks></remarks>
        Public WriteOnly Property ValueProgramable() As Integer
            Set(ByVal value As Integer)
                If value < Me.Minimum Then value = Me.Minimum
                If value > Me.Maximum Then value = Me.Maximum

                IsValueChangedProg = True
                Me.Value = value
                IsValueChangedProg = False	' 念の為
            End Set
        End Property


        ''' <summary>
        ''' スクロールした時の最大値 (GUIから操作したらこと値以上は帰ってこない)
        ''' </summary>
        Public ReadOnly Property MaximumVirtual() As Integer
            Get
                Return Me.Maximum - Me.LargeChange + 1
            End Get
        End Property

    End Class
End Namespace