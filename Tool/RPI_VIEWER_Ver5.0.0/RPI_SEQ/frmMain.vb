'Option Strict On	'共有メモリアクセスがあるため、Off化
Option Strict Off

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    Private WithEvents mcls_RecvMail As clsRecvMail = Nothing						' メールスロット受信
    Private m_memTrkPtr As IntPtr = IntPtr.Zero							' 共有メモリポインタ(他工程間連携)
    Private m_cntItem(TBL_RPI_REC_MAX - 1) As cntItem					' 各アイテムコントロール


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '''' メールスロット
        mcls_RecvMail = New clsRecvMail(Me)


        '''' 共有メモリ確保 (未生成なら自動で生成。生成済みなら接続だけ)
        m_memTrkPtr = tClass_GlovalMemory.AllocMapped(SIZE_TYPE_TRK, TBL_RPI_TRK_NAME)

        ' 初期設定 (読込失敗対策)
        Dim tMem As New TYPE_TRK        ' 共有メモリ全データ
        tMem.initialize()
        tClass_GlovalMemory.SetMappedT(m_memTrkPtr, 0, tMem)




        '''' コントロール生成
        '' ループが使えるように
        m_cntItem(0) = cntItem1
        m_cntItem(1) = cntItem2
        m_cntItem(2) = cntItem3
        m_cntItem(3) = cntItem4
        m_cntItem(4) = cntItem5
        For ii As Integer = 0 To m_cntItem.Length - 1
            m_cntItem(ii).Prop_No = ii
        Next


        '' モニタ情報取得
        Call DspSelectInit()



        '''' 処理開始
        timCycle.Start()

        '''' デフォルト状態を確定
        ckbSeq.Checked = True
    End Sub

    ''' <summary>
    ''' 終了
    ''' </summary>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        '' 処理停止
        timCycle.Stop()


        '' コントロール開放
        For ii As Integer = 0 To m_cntItem.Length - 1
            m_cntItem(ii).Dispose()
        Next

        ' 共有メモリ開放
        If m_memTrkPtr <> IntPtr.Zero Then
            tClass_GlovalMemory.FreeMapped(m_memTrkPtr, TBL_RPI_TRK_NAME)
        End If

        ' メールスロット終了
        If Not mcls_RecvMail Is Nothing Then
            mcls_RecvMail.Dispose()
            mcls_RecvMail = Nothing
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メールスロット受信

    ''' <summary>
    ''' メールスロット受信
    ''' </summary>
    ''' <param name="nEvtNo"></param>
    ''' <param name="que"></param>
    ''' <remarks></remarks>
    Private Sub mcls_RecvMail_tEv_RecvMail(ByVal nEvtNo As Integer, ByVal que As COMMON_QUE_STRMIX) Handles mcls_RecvMail.tEv_RecvMail
        Dim nLineId As Integer = que.nLineNo			   ' 対象の配列へ (0:全部 1～:対象IDのみ)

        ' ループ用
        Dim iiS As Integer
        Dim iiE As Integer
        If 0 = nLineId Then
            iiS = 0
            iiE = TBL_RPI_REC_MAX - 1
        Else
            iiS = nLineId - 1
            iiE = nLineId - 1
        End If


        ' 自分に対して関係 -------------------------------------------------
        ' 終了
        If FACT_RPIV_END = nEvtNo Then
            Me.Close()
            Return
        End If

        ' 連動設定
        If FACT_RPIS_SETTING = nEvtNo Then
            For ii As Integer = iiS To iiE
                m_cntItem(ii).OutParamExec(que)
            Next
            Return
        End If









        ' チェーン関係 -------------------------------------------------
        If FACT_RPIV_DSP_MOVE = nEvtNo Then
            ' 移動の場合、複数連動時はアクティブしか有効にならない対策
            Dim quewk As COMMON_QUE_STRMIX = que
            quewk.nEventNo = FACT_RPIV_ACTIVE

            For ii As Integer = iiS To iiE
                m_cntItem(ii).Mail_Send_Chain(quewk)
                m_cntItem(ii).Mail_Send_Chain(que)
            Next


        ElseIf FACT_RPIV_DSPING = nEvtNo Then		' 起動
            For ii As Integer = iiS To iiE
                m_cntItem(ii).ExecRpiV()
                m_cntItem(ii).Mail_Send_Chain(que)
            Next

        ElseIf FACT_RPIS_DSP_POS = nEvtNo Then		' 表示位置
            For ii As Integer = iiS To iiE
                ' Window ｻｲｽﾞを変更
                Dim nSize As New Size
                Dim nPoint As New Point

                Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
                Dim MainDsp As Rectangle = monitor(que.idata(0) - 1).Bounds

                If 1 <> que.idata(5) Then
                    ' 指定ｻｲｽﾞにｻｲｽﾞ変更
                    nPoint.X = MainDsp.Location.X + que.idata(1)	' 表示位置X
                    nPoint.Y = MainDsp.Location.Y + que.idata(2)	' 表示位置Y
                    nSize.Width = que.idata(3)						' 表示ｻｲｽﾞX
                    nSize.Height = que.idata(4)						' 表示ｻｲｽﾞY
                Else
                    ' 最大ﾓﾆﾀｰｻｲｽﾞにｻｲｽﾞ変更
                    nPoint.X = MainDsp.Location.X					' 表示位置X
                    nPoint.Y = MainDsp.Location.Y					' 表示位置Y
                    nSize.Width = MainDsp.Width						' 表示ｻｲｽﾞX
                    nSize.Height = MainDsp.Height					' 表示ｻｲｽﾞY
                End If
                m_cntItem(ii).DspMove(nSize, nPoint)
            Next

        ElseIf FACT_RPIS_RENDO = nEvtNo Then		' 連動開始／停止
            If que.idata(0) Then
                ckbSeq.Checked = True
            Else
                ckbSeq.Checked = False
            End If

        ElseIf FACT_RPIS_OFFSET = nEvtNo Then		' オフセット値反映
            btnSetOffset.PerformClick()

        ElseIf FACT_RPIS_CLOSE = nEvtNo Then		' ＲＰＩビューアクローズ
            For ii As Integer = iiS To iiE
                m_cntItem(ii).OutMail_RpiEnd()
            Next

        ElseIf FACT_RPIV_FIRST_DSPING = nEvtNo Then
            For ii As Integer = iiS To iiE
                m_cntItem(ii).ExecRpiV()
            Next
        Else

            ' 単純転送
            For ii As Integer = iiS To iiE
                m_cntItem(ii).Mail_Send_Chain(que)
            Next
        End If
    End Sub








    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 連動処理

    ''' <summary>
    ''' 定周期実行
    ''' </summary>
    Private Sub timCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timCycle.Tick
        Dim tMem As New TYPE_TRK        ' 共有メモリ全データ
        tMem.initialize()
        Dim nId As Integer			' メイン (0オリジン)

        '===============================================

        '''' 共有メモリ参照
        tClass_GlovalMemory.GetMapped(m_memTrkPtr, 0, tMem.GetType(), tMem)
        nId = tMem.head.nActiveID


        '===============================================
        ' 連動処理
        If ckbSeq.Checked Then
            '''' 今回の主の補正前生距離を把握
            Dim nBaseY As Integer = (tMem.rec(nId).nY - m_cntItem(nId).Prop_Offset(1)) / m_cntItem(nId).Prop_Rate()
            Dim nBaseX As Integer = (tMem.rec(nId).nX - m_cntItem(nId).Prop_Offset(0))


            '''' 主の距離を 従に反映
            For ii As Integer = 0 To m_cntItem.Length - 1
                If nId = ii Then Continue For

                tMem.rec(ii).nY = CInt(nBaseY * m_cntItem(ii).Prop_Rate() + m_cntItem(ii).Prop_Offset(1))
                tMem.rec(ii).nX = CInt(nBaseX + m_cntItem(ii).Prop_Offset(0))
            Next
        End If

        '===============================================
        ' 結果表示

        '''' 現状セット
        For ii As Integer = 0 To m_cntItem.Length - 1
            ' 現在の距離
            m_cntItem(ii).SetNowData(tMem.rec(ii))
            ' アクティブコントロール
            If nId = ii Then
                m_cntItem(ii).SetActive(True)
            Else
                m_cntItem(ii).SetActive(False)
            End If
        Next

        '''' 結果書込
        tClass_GlovalMemory.SetMappedT(m_memTrkPtr, 0, tMem)
    End Sub


    ''' <summary>
    ''' オフセット調整
    ''' </summary>
    Private Sub btnSetOffset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSetOffset.Click
        Dim tMem As TYPE_TRK = Nothing		  ' 共有メモリ全データ
        tMem.initialize()

        '''' 共有メモリ参照
        tClass_GlovalMemory.GetMapped(m_memTrkPtr, 0, tMem.GetType(), tMem)


        '===============================================
        ' 1倍基準 

        '''' 全部1倍基準で考えること！
        Dim nBaseY(TBL_RPI_REC_MAX - 1) As Integer	  ' 1倍基準の長手[m]
        Dim nBaseX(TBL_RPI_REC_MAX - 1) As Integer	  ' 1倍基準の長手[m]
        For ii As Integer = 0 To TBL_RPI_REC_MAX - 1
            nBaseY(ii) = tMem.rec(ii).nY / m_cntItem(ii).Prop_Rate()
            nBaseX(ii) = tMem.rec(ii).nX
        Next

        '''' 最小位置のラインを見つける
        Dim nMinY As Integer = nBaseY(0)	' 1倍基準の最小誤差
        Dim nMinX As Integer = nBaseX(0)
        For ii As Integer = 0 To m_cntItem.Length - 1
            If Not m_cntItem(ii).CheckRpiRun() Then Continue For

            nMinY = Math.Min(nMinY, nBaseY(ii))
            nMinX = Math.Min(nMinX, nBaseX(ii))
        Next


        '===============================================
        ' 元に戻す 

        '''' 最小からのズレ量
        For ii As Integer = 0 To m_cntItem.Length - 1
            m_cntItem(ii).Prop_Offset(1) = tMem.rec(ii).nY - (nMinY * m_cntItem(ii).Prop_Rate())
            m_cntItem(ii).Prop_Offset(0) = tMem.rec(ii).nX - nMinX
        Next
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 小物

    ''' <summary>
    ''' 連動状態
    ''' </summary>
    Private Sub ckbSeq_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbSeq.CheckedChanged
        '''' 状態変更
        If ckbSeq.Checked Then
            ckbSeq.Text = "連動中"
            ckbSeq.BackColor = Color.Orange
        Else
            ckbSeq.Text = "連動解除"
            ckbSeq.BackColor = SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' RPIVの起動済みIndexのリスト (0オリジン)
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ExecRpiVList() As List(Of Integer)
        Dim l As New List(Of Integer)

        For ii As Integer = 0 To m_cntItem.Length - 1
            If m_cntItem(ii).CheckRpiRun() Then l.Add(ii)
        Next
        Return l
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ モニタ関係

    ''' <summary>
    ''' モニタ取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspSelectInit()
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens()
        cmbDspSelect.Items.Clear()
        For ii As Integer = 0 To monitor.Length - 1
            cmbDspSelect.Items.Add(ii + 1)
        Next
        cmbDspSelect.SelectedIndex = 0


        cmb11_Rpi.Items.Clear()
        For ii As Integer = 0 To m_cntItem.Length - 1
            cmb11_Rpi.Items.Add(ii)
        Next
        cmb11_Rpi.SelectedIndex = 0
    End Sub



    ''' <summary>
    ''' ２×２ に RpiVを整列させる
    ''' </summary>
    Private Sub tlp22_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlp22.DoubleClick, lbl22_1.DoubleClick, lbl22_2.DoubleClick, lbl22_3.DoubleClick, lbl22_4.DoubleClick
        Dim pIndex As List(Of Integer) = ExecRpiVList()		' RPIVの起動済みIndexのリスト (0オリジン)

        Dim nDspId As Integer = cmbDspSelect.SelectedIndex
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
        Dim MainDsp As Rectangle = monitor(nDspId).Bounds

        Dim s As Size
        s.Width = MainDsp.Size.Width / 2
        s.Height = MainDsp.Size.Height / 2
        Dim p As Point = MainDsp.Location


        '' 動作
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To 2 - 1
            p.Y = MainDsp.Location.Y + (s.Height * ii)
            For jj As Integer = 0 To 2 - 1
                If nCnt >= pIndex.Count Then Exit For

                p.X = MainDsp.Location.X + (s.Width * jj)

                Call m_cntItem(pIndex(nCnt)).DspMove(s, p)

                nCnt += 1
            Next
        Next
    End Sub

    ''' <summary>
    ''' １×X に RpiVを整列させる
    ''' </summary>
    Private Sub tlp13_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlp13.DoubleClick, lbl13_1.DoubleClick, lbl13_2.DoubleClick, lbl13_3.DoubleClick
        Dim pIndex As List(Of Integer) = ExecRpiVList()		' RPIVの起動済みIndexのリスト (0オリジン)

        Dim nDspId As Integer = cmbDspSelect.SelectedIndex
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
        Dim MainDsp As Rectangle = monitor(nDspId).Bounds

        Dim s As Size
        s.Width = MainDsp.Size.Width / pIndex.Count
        s.Height = MainDsp.Size.Height
        Dim p As Point = MainDsp.Location


        '' 動作
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To pIndex.Count - 1
            If nCnt >= pIndex.Count Then Exit For

            p.X = MainDsp.Location.X + (s.Width * ii)
            Call m_cntItem(pIndex(nCnt)).DspMove(s, p)
            nCnt += 1
        Next
    End Sub

    ''' <summary>
    ''' １×1 に RpiVを整列させる
    ''' </summary>
    Private Sub tlp11_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlp11.DoubleClick
        Dim nDspId As Integer = cmbDspSelect.SelectedIndex
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
        Dim MainDsp As Rectangle = monitor(nDspId).Bounds

        Dim s As Size = MainDsp.Size
        Dim p As Point = MainDsp.Location


        Dim nIndex As Integer = cmb11_Rpi.SelectedIndex
        If Not m_cntItem(nIndex).CheckRpiRun() Then Return
        Call m_cntItem(nIndex).DspMove(s, p)
    End Sub
End Class
