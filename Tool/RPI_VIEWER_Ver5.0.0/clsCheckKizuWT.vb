'*********************************************************************************
'通板距離関係クラス (並列処理でチェックを行う)
'	[Ver]
'		Ver.01    2009/02/20  vs2005 対応
'
'	[メモ]
'		・減速有効範囲	正転のみ
'						ONイベント：画面下端＋xm 到達時 (前回～今回チェック距離で範囲内に、対象疵がある場合）
'						フラグON中：画面下端＋xm～画面上端まで。 自分で
'		・停止			正転のみ
'						ONイベント：画面中央に来たとき。(前回～今回チェック距離が画面中央位置。そのときに、表示は画面中央にする)
'						フラグON中：一定時間経過で自動OFF。		ライン速度を上げた
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Threading						' スレッド

Public Class clsCheckKizuWT

    '===================================================
    '/ 公開イベント
    Public Event tEv_DspPosChange(ByVal nLen As Integer)				' 画面中央表示位置変更要求 [mm]


    '===================================================
    '' 内部定数
    Private Const mc_nCycleTime As Integer = 100						' 定周期実行時間 [ms]


    '===================================================
    '' メンバークラス
    Private mcls_pParam As clsParamManager								' パラメータ管理クラス (ポインタ)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)
    Private mfrm_pPopCont As frmPopControl								' コントロール画面 (ポインタ)


    Private mcls_WThread As tClass_WorkThread = Nothing					' ワーカースレッド本体
    Private m_evGo As New Threading.AutoResetEvent(False)				' 即時実行イベント

    '' 内部イベント
    Private m_trAutoStop As New Threading.Timer(AddressOf AutoStopEnd)	'ライン停止中タイマー



    '''' 必須管理情報
    Private m_bEnable As Boolean										' 自動減速有効無効 (true:有効)
    Private m_nPosOld As Integer										' 前回の通板距離 (表示先端) [mm]
    Private m_nPerSpeed As Integer										' 減速、停止前のライン速度


    '''' コイル単位で管理が必要な情報
    Private m_nMen As Integer											' 面

    ' データ
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' 疵情報リスト


    ' 結果
    Private m_bAutoDown As Boolean										' 減速区間時 true。※最終判断は、frmMainで判定
    Private m_bAutoStop As Boolean										' 停止時間中 true。※最終判断は、frmMainで判定
    Private m_bAutoChancel As Boolean									' 減速区間中にSTOP釦押下 true。※最終判断は、frmMainで判定
    Private m_nRanpGId(MC_RANP_CNT - 1) As Integer						' 疵接近ランプのGr
    Private m_nRanpGYu(MC_RANP_CNT - 1) As Integer						' 疵接近ランプの優先度




    '===================================================
    '' 外部アクセス
    Public ReadOnly Property IsAutoDown() As Boolean
        Get
            Return m_bAutoDown
        End Get
    End Property

    Public ReadOnly Property IsAutoStop() As Boolean
        Get
            Return m_bAutoStop
        End Get
    End Property

    Public Function RanpStat() As Integer()
        Return m_nRanpGId
    End Function


    ''' <summary>
    ''' 自動減速有効無効
    ''' </summary>
    Public Property Enable() As Boolean
        Get
            Return m_bEnable
        End Get
        Set(ByVal value As Boolean)
            m_bEnable = value

            '解除時は、自動減速等がかかっていたら強制ヤメ
            If Not value Then
                Call AutoStopEnd(Nothing)
                Call AutoDownEnd()
            End If

        End Set
    End Property



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 初期設定
    ''' <summary>
    ''' パラメータクラスセット
    ''' </summary>
    ''' <param name="cls">計算クラス</param>
    Public Sub InitCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

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

    ''' <summary>
    ''' 疵情報リスト
    ''' </summary>
    ''' <param name="lst">疵情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub InitData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub

    ''' <summary>
    ''' コイル切替
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub CoilChange()
        m_nPosOld = 0
        m_nPerSpeed = 0
        '解除時は、自動減速等がかかっていたら強制ヤメ
        Call AutoStopEnd(Nothing)
        Call AutoDownEnd()
    End Sub

    ''' <summary>
    ''' 自動減速キャンセル
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub AutoStopCancel()
        m_nPosOld = 0
        m_nPerSpeed = 0
        m_bAutoChancel = True		' 自動減速キャンセル
        '解除時は、自動減速等がかかっていたら強制ヤメ
        Call AutoStopEnd(Nothing)
        Call AutoDownEnd()
    End Sub

    ''' <summary>
    ''' 面変更
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MenChange(ByVal nMen As Integer)
        m_nMen = nMen
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' スレッドスタート
    ''' </summary>
    Public Function ThreadStart(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
        If mcls_WThread Is Nothing Then
            mcls_WThread = New tClass_WorkThread

            '''' メインスレッド実行
            mcls_WThread.ThreadStart(AddressOf mainThread, priority, "CheckKizuWT", True)
            Return True
        End If
        Return False
    End Function


    ''' <summary>
    ''' スレッドストップ
    ''' </summary>
    Public Sub ThreadStop()
        '' インスタンス確認
        If Not mcls_WThread Is Nothing Then
            '' メインスレッド終了
            mcls_WThread.ThreadStop(2000)
            mcls_WThread.Dispose()
            mcls_WThread = Nothing
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ スレッドメイン

    Private Sub mainThread()
        Dim EventAry(2 - 1) As Threading.WaitHandle		' イベント
        Dim nEvtNo As Integer							' WaitAny 復帰情報

        '''' シグナルセット
        EventAry(0) = mcls_WThread.g_evStop				' スレッド終了
        EventAry(1) = m_evGo

        '''' とりあえず即時実行
        m_evGo.Set()

        '''' シグナル待ち
        Try
            While (True)
                '' シグナル待ち
                nEvtNo = Threading.WaitHandle.WaitAny(EventAry, mc_nCycleTime, False)
                Select Case nEvtNo
                    Case 0								' 終了確認
                        Exit While
                    Case Else							' 即時実行,タイムアウト等
                        Call ExecMain()
                End Select
            End While
        Catch ex As Threading.ThreadAbortException
        Catch ex As Exception
        End Try
    End Sub


    ''' <summary>
    ''' 定周期実行 ※スレッド内なので、コントロールへのアクセスは禁止
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ExecMain()
        If Not mcls_pRpiView.IsRpi() Then Return
        If m_lstDetect Is Nothing Then Return

        '''' 必要な情報取得
        Dim nSpeed As Integer = mfrm_pPopCont.LineSpeed			' ライン速度
        Dim nDpsAria As Integer = CInt(mcls_pRpiView.GetPos_DpToLen_Y(mcls_pRpiView.GetImg_AriaInf.nAriaHeight))	' 画面エリアの距離
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_pRpiView.GetDspAriaInf
        Dim nPosF As Integer = CInt(mcls_pRpiView.GetPos_DpToLen_Y(DspInf.DspPixelTotalOffset.Y))


        ' ==========================================
        ' 各範囲 算出
        ' 停止 範囲
        Dim nStop(2 - 1) As Integer		' 開始,終了 [mm]
        nStop(0) = m_nPosOld + CInt(nDpsAria / 2)
        nStop(1) = nPosF + CInt(nDpsAria / 2)

        ' 減速 範囲
        Dim nDown(2 - 1) As Integer		' 開始,終了
        nDown(0) = nPosF - mfrm_pPopCont.AutoE	' 終了
        nDown(1) = nPosF + nDpsAria + mfrm_pPopCont.AutoS	 ' 開始

        ' 疵接近ランプ
        Dim nRanpGId(MC_RANP_CNT - 1) As Integer						' 疵接近ランプのGr
        Dim nRanpGYu(MC_RANP_CNT - 1) As Integer						' 疵接近ランプの優先度
        Dim dWid As Single = mcls_pRpiView.GetPos_DpToLen_X(mcls_pRpiView.GetImg_AriaInf.nAriaWidth) / MC_RANP_CNT '1区間の幅
        Dim ranp(2 - 1) As Integer		' 開始,終了
        ranp(0) = nPosF - nDpsAria - mfrm_pPopCont.RanpE
        ranp(1) = nPosF + nDpsAria + mfrm_pPopCont.RanpS


        ' ==========================================
        ' 疵チェック
        Dim bStop As Boolean = False
        Dim bDown As Boolean = False

        ' 別スレッドでリスト配列にアクセスするので、疵変更とかで読み直し時にエラーになる可能性大。だから、tryしておく
        Try
            For Each data As DATA_DETECT_INF In m_lstDetect
                If Not data.bDsp Then Continue For ' そもそも表示フィルターで消えている疵は対象外

                ' ランプ
                ' 今回の表示対象？
                If data.nY >= ranp(0) AndAlso data.nY <= ranp(1) Then

                    Dim nRpW As Integer = CInt((data.nFrameXmin + data.nFrameXmax) / 2.0) + mcls_pRpiView.GetRpi_THeader.caminfo(data.nCamPos).nBaseLeft	' 疵中心位置 [RP]
                    Dim nIndex As Integer = CInt(Int(mcls_pRpiView.GetPos_RpToLen_X(nRpW) / dWid))
                    If nIndex < 0 Then nIndex = 0
                    If nIndex >= MC_RANP_CNT Then nIndex = MC_RANP_CNT - 1
                    ' 左右反転対策
                    If data.nBaseX <> data.nX Then
                        nIndex = MC_RANP_CNT - nIndex - 1
                    End If

                    If 0 = nRanpGYu(nIndex) OrElse nRanpGYu(nIndex) > data.gYu Then
                        nRanpGId(nIndex) = data.gId
                        nRanpGYu(nIndex) = data.gYu
                    End If
                End If



                '''' 以降は、正転のみ
                If Not m_bEnable Or 0 >= nSpeed Then Continue For

                ' 停止 Onのみ
                If 0 <> mfrm_pPopCont.AutoStopTime Then
                    If Not bStop AndAlso data.nY > nStop(0) AndAlso data.nY <= nStop(1) Then
                        If data.gYu <= mfrm_pPopCont.AutoStopGr() Then
                            bStop = True
                        End If
                    End If
                End If

                ' 減速 範囲内
                If Not bDown AndAlso data.nBaseY > nDown(0) And data.nBaseY <= nDown(1) Then
                    If data.gYu <= mfrm_pPopCont.AutoDownGr() Then
                        bDown = True
                    End If

                    If data.nY <= nStop(1) AndAlso m_bAutoChancel Then
                        bDown = False			' 自動減速キャンセル時は、通常に戻す
                    End If
                End If
            Next
        Catch ex As Exception

        End Try



        ' ==========================================
        '''' 結果反映
        ' 自動停止
        If bStop Then Call AutoStopStart()
        ' 自動減速
        If bDown And Not m_bAutoDown Then AutoDownStart() ' 通常→自動減速
        If Not bDown And m_bAutoDown Then AutoDownEnd() ' 自動減速→通常

        ' 疵接近ランプ
        m_nRanpGId = nRanpGId
        m_nRanpGYu = nRanpGYu


        '''' 最後チェック位置
        m_nPosOld = nPosF
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 自動停止

    ''' <summary>
    ''' 自動停止開始
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoStopStart()

        If Not m_bAutoDown Then m_nPerSpeed = mfrm_pPopCont.LineSpeed ' AutoDownであっている。自動減速前の値
        mfrm_pPopCont.LineSpeed = 0
        m_bAutoStop = True

        ' タイマー実行
        Dim nStopTime As Integer = mfrm_pPopCont.AutoStopTime	   '[ms]
        m_trAutoStop.Change(nStopTime, 0)	' 起動
    End Sub

    ''' <summary>
    ''' 自動停止解除 (タイマー実行)
    ''' </summary>
    ''' <param name="state"></param>
    ''' <remarks></remarks>
    Private Sub AutoStopEnd(ByVal state As Object)
        If Not m_bAutoStop Then Return ' 自動停止中は解除。それ以外は解除しない。 2014.06.24 J.Wakazono
        m_bAutoStop = False

        If m_bAutoDown Then
            mfrm_pPopCont.LineSpeed = mfrm_pPopCont.AutoDownMpm
        Else
            mfrm_pPopCont.LineSpeed = m_nPerSpeed
        End If
    End Sub



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ 自動減速
    ''' <summary>
    ''' 自動減速開始
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoDownStart()
        If m_bAutoStop Then Return ' 自動停止中は無視。通常ありえない

        If Not m_bAutoDown Then m_nPerSpeed = mfrm_pPopCont.LineSpeed
        mfrm_pPopCont.LineSpeed = mfrm_pPopCont.AutoDownMpm
        m_bAutoDown = True
        m_bAutoChancel = False

    End Sub

    ''' <summary>
    ''' 自動減速解除
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoDownEnd()
        If m_bAutoStop Then Return ' 自動停止中は無視。通常ありえない
        If Not m_bAutoDown Then Return ' 自動減速中は解除。それ以外は解除しない。 2014.06.24 J.Wakazono

        m_bAutoDown = False
        mfrm_pPopCont.LineSpeed = m_nPerSpeed
    End Sub

End Class


'ちゃんと動くよー
'''' <summary>
'''' C++のCreateWaitableTimer, SetWaitableTimer に相当
'''' </summary>
'''' <remarks></remarks>
'Public Class tClass_WaitableTimer
'	Public g_evHandler As New Threading.AutoResetEvent(False)
'	Private m_evTimer As New Threading.Timer(AddressOf TimerProc)


'	''' <summary>
'	''' タイマー実行開始
'	''' </summary>
'	''' <param name="nFastTime">初回時間[ms]</param>
'	''' <param name="nCycleTime">繰り返し時間[ms]</param>
'	''' <remarks></remarks>
'	Public Sub SetWaitableTimer(ByVal nFastTime As Integer, Optional ByVal nCycleTime As Integer = 0)
'		m_evTimer.Change(nFastTime, nCycleTime)
'	End Sub

'	''' <summary>
'	''' タイムアップ
'	''' </summary>
'	''' <param name="state"></param>
'	''' <remarks></remarks>
'	Private Sub TimerProc(ByVal state As Object)
'		g_evHandler.Set()
'	End Sub
'End Class
