
Imports tClassLibrary

Public Class cntItem

    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ API
    Private Declare Function MoveWindow Lib "user32" Alias "MoveWindow" _
     (ByVal hwnd As IntPtr, ByVal x As Integer, ByVal y As Integer, _
      ByVal nWidth As Integer, ByVal nHeight As Integer, _
      ByVal nRepaint As Integer) As Integer



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ 定数
    Private m_nNo As Integer = -1										' No (0オリジン)
    Private m_sMailName As String = ""									' 該当RpiViewerのメールスロット名

    Private m_pRpi As System.Diagnostics.Process						' RpiViewer プロセス

    Dim mcls_Mail As New tClassLibrary.tClass.tClass_MailSlot_Base		' メールスロット(送信専用)

    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ 初期設定

    ''' <summary>
    ''' No
    ''' </summary>
    Public Property Prop_No() As Integer
        Get
            Return m_nNo
        End Get
        Set(ByVal value As Integer)
            m_nNo = value

            ' 関連項目設定
            lblNo.Text = CStr(m_nNo + 1)
            m_sMailName = String.Format("{0}{1}", RPIV_NAME, m_nNo)
        End Set
    End Property

    ''' <summary>
    ''' 伸び率
    ''' </summary>
    Public Property Prop_Rate() As Double
        Get
            Return nudRateY.Value
        End Get
        Set(ByVal value As Double)
            If nudRateY.Minimum > value Then value = nudRateY.Minimum
            If nudRateY.Maximum < value Then value = nudRateY.Maximum

            nudRateY.Value = CDec(value)
        End Set
    End Property

    ''' <summary>
    ''' オフセット
    ''' </summary>
    ''' <param name="nIndex">0:X  1:Y</param>
    Public Property Prop_Offset(ByVal nIndex As Integer) As Integer
        Get
            Dim nudVal As NumericUpDown = CType(IIf(0 = nIndex, nudOffsetX, nudOffsetY), NumericUpDown)

            Return CInt(nudVal.Value)

        End Get
        Set(ByVal value As Integer)
            Dim nudVal As NumericUpDown = CType(IIf(0 = nIndex, nudOffsetX, nudOffsetY), NumericUpDown)
            nudVal.Value = CDec(value)
        End Set
    End Property



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ RPI 初期表示関係

    ''' <summary>
    ''' 終了時
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub cntItem_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Call Mail_RpiEnd()
    End Sub

    ''' <summary>
    ''' RPIファイルをドラッグで指定する (ドラッグ)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragEnter
        ' ドラッグで有効にしておかないとドロップイベントが走らない
        ' フォームプロパティで AllowDrop を有効にしておかないとダメ
        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            e.Effect = DragDropEffects.Copy
        Else
            e.Effect = DragDropEffects.None
        End If
    End Sub

    ''' <summary>
    ''' RPIファイルをドラッグで指定する (ドロップ)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragDrop(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragDrop
        If Not e.Data.GetDataPresent(DataFormats.FileDrop) Then Return

        '' ドラッグされたデータ形式を調べ、ファイルのときはコピーとする
        Dim fileName As String() = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
        Dim strFPath As String = fileName(0) ' 複数指定時を考慮して、最初の１コを使う


        '===============================================
        ' 事前準備
        txtRpiPath.Text = strFPath

        ' RPIファイル実行
        'Call SelectRpiFile(strFPath, True)
    End Sub

    ''' <summary>
    ''' 実行
    ''' </summary>
    Private Sub btnExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExec.Click
        '===============================================
        ' RPI Viewerが起動
        Call ExecRpiV()

        '===============================================
        ' RPI Viewer に情報通知
        If 0 < txtRpiPath.Text.Length Then
            Call Mail_RpiDsping(txtRpiPath.Text)
        End If
    End Sub

    ''' <summary>
    ''' RPI Viewer起動
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ExecRpiV()
        '===============================================
        ' RPI Viewerが起動しているかチェック
        Dim bIsRun As Boolean = CheckRpiRun()								  ' RPIViewer起動時 true


        '===============================================
        ' RPI Viewerが起動
        If Not bIsRun Then
            Dim sCmd As String		' 起動コマンド
            sCmd = String.Format("{0}{1} {2}{3}", New Object() {START_CMD_TASKNAME, m_sMailName, START_CMD_ID, m_nNo})
            Try
                m_pRpi = Process.Start("RPI_VIEWER.exe", sCmd)
                'm_pRpi= Process.Start("C:\★13_開発案件\RPI連動\RPI_VIEWER Ver440\bin\Release\RPI_VIEWER.exe", sCmd)
                m_pRpi.WaitForInputIdle(1500)	' 起動するまで待ち
                Threading.Thread.Sleep(1500)	' メール受信可能になるまで待ち

            Catch ex As Exception
            End Try
        End If
    End Sub




    ''' <summary>
    ''' 画面サイズ、位置変更
    ''' </summary>
    ''' <param name="s">サイズ</param>
    ''' <param name="p">位置</param>
    ''' <remarks></remarks>
    Public Sub DspMove(ByVal s As Size, ByVal p As Point)

        MoveWindow(m_pRpi.MainWindowHandle, p.X, p.Y, s.Width, s.Height, 1)


    End Sub


    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ メールスロット関係

    ''' <summary>
    ''' 外部から来た通知をそのまま転送
    ''' </summary>
    ''' <param name="que">そのもの</param>
    ''' <remarks></remarks>
    Public Sub Mail_Send_Chain(ByRef que As COMMON_QUE_STRMIX)
        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' RPI Viewerが起動しているかチェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function CheckRpiRun() As Boolean
        Using clsMail As New tClassLibrary.tClass.tClass_MailSlot_Base	' 重複起動確認用メールスロット
            If clsMail.Mail_Open(m_sMailName) Then
                clsMail.Mail_Close()
                Return False
            Else
                Return True
            End If
        End Using
    End Function

    ''' <summary>
    ''' RPIファイル指定
    ''' </summary>
    ''' <param name="sFPath"></param>
    ''' <remarks></remarks>
    Private Sub Mail_RpiDsping(ByVal sFPath As String)
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_DSPING

        que.cdata1 = IO.Path.GetDirectoryName(sFPath) & "\"	' パス
        Dim sName As String = IO.Path.GetFileName(sFPath)		' ファイル名
        Dim nCnt As Integer = tClassLibrary.tMod.LenB(sName)
        If 32 > nCnt Then
            que.cdata2 = sName.Substring(0, nCnt)
        Else
            que.cdata2 = sName.Substring(0, 31)
            que.cdata3 = sName.Substring(31, nCnt - 31)
        End If

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' 終了通知
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Mail_RpiEnd()
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_END

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' 左右反転
    ''' </summary>
    Private Sub ckbTurn_Left_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbTurn_Left.CheckedChanged
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_DSP_TURN

        que.idata(0) = CInt(IIf(CType(sender, CheckBox).Checked, 1, -1))

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub

    ''' <summary>
    ''' 上下反転
    ''' </summary>
    Private Sub ckbTurn_Top_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbTurn_Top.CheckedChanged
        Dim que As COMMON_QUE_STRMIX = Nothing
        que.inisialize()
        que.nEventNo = FACT_RPIV_DSP_TURN

        que.idata(1) = CInt(IIf(CType(sender, CheckBox).Checked, 1, -1))

        mcls_Mail.Mail_Send(".", m_sMailName, que)
    End Sub


    ''' <summary>
    ''' 外部から設定反映
    ''' </summary>
    Public Sub OutParamExec(ByRef que As COMMON_QUE_STRMIX)
        Dim nMode As Integer = que.idata(0)

        ' 最下位1バイト
        If 0 <> (nMode And &H1) Then		' 伸び率
            Prop_Rate = que.idata(1) / 1000000.0
        End If
        If 0 <> (nMode And &H2) Then		' オフセットY
            Prop_Offset(1) = que.idata(2)
        End If
        If 0 <> (nMode And &H4) Then		' オフセットX
            Prop_Offset(0) = que.idata(3)
        End If

        If 0 <> (nMode And &H8) Then
        End If

        If 0 <> (nMode And &H10) Then
        End If
        If 0 <> (nMode And &H20) Then
        End If
        If 0 <> (nMode And &H40) Then
        End If
        If 0 <> (nMode And &H80) Then
        End If

    End Sub

    ''' <summary>
    ''' 外部からＲＰＩビューアクローズ要求
    ''' </summary>
    Public Sub OutMail_RpiEnd()
        Mail_RpiEnd()
    End Sub

    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ TRK操作

    ''' <summary>
    ''' 現在情報
    ''' </summary>
    ''' <param name="typItem"></param>
    ''' <remarks></remarks>
    Public Sub SetNowData(ByVal typItem As TYPE_TRK_ITEM)

        txtValY.Text = String.Format("{0:#,##0}", typItem.nY)
        txtValX.Text = String.Format("{0:#,##0}", typItem.nX)

    End Sub

    ''' <summary>
    ''' 主選択状態
    ''' </summary>
    ''' <param name="bMode">true:選択中</param>
    ''' <remarks></remarks>
    Public Sub SetActive(ByVal bMode As Boolean)
        If bMode Then
            lblNo.BackColor = Color.Orange
        Else
            lblNo.BackColor = SystemColors.Control
        End If
    End Sub




End Class
