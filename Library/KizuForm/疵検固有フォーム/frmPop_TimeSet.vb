'*********************************************************************************
' 時刻設定画面の親クラス
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports System.Runtime.InteropServices

''' <summary>
''' 時刻設定画面の親クラス
''' </summary>
''' <remarks></remarks>
Public Class frmPop_TimeSet

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 時刻設定用構造体
    '/ /////////////////////////////////////////////////////////////////////////////////
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MSL, pack:=1)> _
    Structure COMMON_QUE_TIME
        Dim nEventNo As Integer
        Dim nLineNo As Integer

        Dim wYear As Short
        Dim wMonth As Short
        Dim wDayOfWeek As Short
        Dim wDay As Short
        Dim wHour As Short
        Dim wMinute As Short
        Dim wSecond As Short
        Dim wMilliseconds As Short

        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=CInt(SIZE_MSL_DATA / 2) - 16)> _
        Dim dataL() As Short

        Public Sub Inisialize()
            ReDim dataL(CInt(SIZE_MSL_DATA / 2) - 16 - 1)
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクションボタン押下イベント
    ''' </summary>
    ''' <param name="FncNo">0オリジン</param>
    ''' <param name="selectDay">設定時刻</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal selectDay As Date)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_TimeSet_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' 初期表示
        Call timClock_Tick(Nothing, Nothing)
    End Sub
    ''' <summary>
    ''' 時刻表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timClock_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timClock.Tick
        lblClock.Text = Now.ToString("yyyy/MM/dd HH:mm:ss")
    End Sub
    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTime.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub
    ''' <summary>
    ''' カレンダー日付を設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub mclMonth_DateChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DateRangeEventArgs) Handles mclMonth.DateChanged
        '' カレンダーの年を変更すると 変な選択状態になる為 (どうやらコントロールがバクっているらしい･･･)
        mclMonth.SelectionStart = e.Start
        mclMonth.SelectionEnd = e.Start
    End Sub
    ''' <summary>
    ''' ファンクションボタンクリック
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_TimeSet_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
        Dim day As Date = CDate(mclMonth.SelectionStart.ToString("yyyy/MM/dd ") & dtpTime.Value.ToString("HH:mm:ss"))

        ' 選択時刻を渡す
        RaiseEvent tEv_FunctionButton(FncNo, day)
    End Sub

End Class
