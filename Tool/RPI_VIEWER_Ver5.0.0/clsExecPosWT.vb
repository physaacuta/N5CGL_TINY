'*********************************************************************************
'通板距離関係クラス (フォームタイマーを使用なので、スレッドでは無いので注意)
'	[Ver]
'		Ver.01    2009/02/20  vs2005 対応
'
'	[メモ]
'		・通板距離を加算する
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Threading						' スレッド

Public Class clsExecPosWT
    '===================================================
    '' イベント
    Public Event tEv_AutoRunning(ByVal nSpeed As Integer, ByVal nDp As Integer)	   ' 自動更新通知

    '===================================================
    '' 内部定数
    Private Const mc_nCycleTime As Integer = 60							' 定周期実行時間 [ms]

    '===================================================
    '' メンバークラス
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)
    Private mfrm_pPopCont As frmPopControl								' コントロール画面 (ポインタ)
    Private mcnt_VBar As tCnt.tCnt_VScrollBar							' スクロールバー (ポインタ)

    '''' 必須管理情報
    Private WithEvents m_tmDspInterval As New Windows.Forms.Timer()		' 定周期タイマー
    Private m_tmCycleTime As New Stopwatch								' ライン速度決定用時間計測



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 初期設定
    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub InitCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub
    Public Sub InitFrm_Control(ByRef frm As frmPopControl)
        mfrm_pPopCont = frm
    End Sub
    Public Sub InitCnt_Bar(ByRef cnt As tCnt.tCnt_VScrollBar)
        mcnt_VBar = cnt
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' スレッドスタート
    ''' </summary>
    Public Sub ThreadStart()
        m_tmDspInterval.Interval = 60
        m_tmDspInterval.Start()
        m_tmCycleTime.Start()
    End Sub

    ''' <summary>
    ''' スレッドストップ
    ''' </summary>
    Public Sub ThreadStop()
        m_tmDspInterval.Stop()
        m_tmCycleTime.Stop()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ スレッドメイン

    ''' <summary>
    ''' タイマー
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_trDspInterval_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_tmDspInterval.Tick
        If Not mcls_pRpiView.IsRpi() Then Return

        '''' 今回の位置決定
        Dim nSpeed As Integer = mfrm_pPopCont.LineSpeed
        Dim nDp As Integer = mcnt_VBar.Value	   ' 表示位置 [表示pixel]
        If 0 <> nSpeed Then
            nDp = ExecPos(nSpeed, nDp)
        End If

        '''' イベント通知
        RaiseEvent tEv_AutoRunning(nSpeed, nDp)

        ' 今回位置
    End Sub

    ''' <summary>
    ''' 今回の位置決定
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ExecPos(ByVal nSpeed As Integer, ByVal nDP As Integer) As Integer

        '''' 今回の時間を算出
        Dim tms As TimeSpan = m_tmCycleTime.Elapsed
        m_tmCycleTime.Reset()
        m_tmCycleTime.Start()


        '''' 今回の通板距離を算出
        Dim dwk As Double = nSpeed / 60.0		' [m/msec]
        dwk = dwk * tms.Milliseconds			' タイマー時間辺り これだけ進む距離 [mm]
        Dim nWk As Integer = mcls_pRpiView.GetPos_LenToDp_Y(dwk)	' 表示座標系のpixel位置
        ' 最低1pixelは必ず移動させる
        If 0 < nSpeed AndAlso nWk <= 0 Then nWk = 1
        If 0 > nSpeed AndAlso nWk >= 0 Then nWk = -1

        ' 今回の表示位置
        nDP += nWk
        'If nDP > mcnt_VBar.MaximumVirtual Then nDP = mcnt_VBar.Minimum
        If nDP > mcnt_VBar.MaximumVirtual Then nDP = mcnt_VBar.MaximumVirtual
        If nDP < mcnt_VBar.Minimum Then nDP = mcnt_VBar.Minimum


        'm_Output.WriteLine("{0},{1},{2}", m_nSpeed, tms.Milliseconds, nWk)

        Return nDP
    End Function
End Class
