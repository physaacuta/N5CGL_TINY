'*********************************************************************************
' FFサーバー実績送信履歴画面
'	[Ver]
'		Ver.01    2017/06/08  vs2015 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

Public Class frmFFSearch

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド設定項目
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_LIST_COL
        ''' <summary>ID</summary>
        nID = 0
        ''' <summary>キー情報</summary>
        nKEY
        ''' <summary>登録日付</summary>
        nDATE
        ''' <summary>送信日付</summary>
        nSEND
        ''' <summary>管理No</summary>
        nKANRI
        ''' <summary>伝文区分</summary>
        nKIND
        ''' <summary>最大列数</summary>
        MAX_LIST_COL
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass_LogManager                                   ' ログ管理
    Private m_lstChkKind As New List(Of CheckBox)                           ' 伝文区分チェックボックスコントロール リスト

    ' DB関連
    'Private m_sDB_STATUS_LOG As String = DB_BUF_FF_S_ZISEKI                 ' DBテーブル FF検査実績送信バッファテーブル
    Private m_sDbSession As String = INI_DB                                 ' DBセッション

    ' ログ関連
    Private m_bLog_ExecCancel As Boolean = False                            ' キャンセル時 True


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Sub New(ByVal log As tClass_LogManager)

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        '-------------------------------------------------------------------
        ' パラメータの初期化
        '-------------------------------------------------------------------
        mcls_Log = log

    End Sub

    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmFFSearch_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン
            Case 1 - 1                          ' 伝文出力
            Case 5 - 1                          ' 最新表示
                Call ExecSelect_Log()
            Case 10 - 1                         ' 画面印刷
                Call PrintOut()
            Case 11 - 1                         ' 画面保存
                Call HardCopy(IMG_PATH, "FF実績送信履歴_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 12 - 1                         ' 終了
                MyBase.ExecFromClose()
        End Select
    End Sub

    ''' <summary>
    ''' 生成
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmFFSearch_Load(sender As Object, e As EventArgs) Handles Me.Load

        '' グリッド定義
        dgvInitialize(dgvList)

        ' 時刻セット
        dtpStartDay.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
        dtpStartTime.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
        dtpEndDay.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))
        dtpEndTime.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))

        AddCheckBoxKind(chkKind_Normal, 0)
        AddCheckBoxKind(chkKind_NoSend, 1)
        AddCheckBoxKind(chkKind_Failed, -1)
        AddCheckBoxKind(chkKind_Cancel, -9)

    End Sub

    ''' <summary>
    ''' 開放
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmFFSearch_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        m_lstChkKind.Clear()       ' 区分(稼動状態)チェックボックスコントロール リスト
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ オーバーライド
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' Me.Activated では使い物にならない為、Loadの終了一回だけ非同期でメソッドをコールしてあげるイベントを作ったよ。
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' 元々のメソッド実行
        MyBase.LoadEnding()

        ''''' 拡張分
        '' 画面が開き終わったタイミングで 一回検索を行う
        Call ExecSelect_Log()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL文生成用
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' SQL文生成 (SELECT句,FROM句,WHERE句,END句 の順番にコール)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <param name="bMode">true:通常 false:Count(*)</param>
    ''' <param name="nSelectCnt">0以外の場合 top 10 とかにする</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
        '' SELECT句
        subSQL_Create_Select(sql, bMode, nSelectCnt)
        '' FROM句
        subSQL_Create_From(sql)
        '' WHERE句
        subSQL_Create_Where(sql)
        '' END句
        subSQL_Create_End(sql, bMode)

    End Sub
    ''' <summary>
    ''' SQL文生成 (SELECT句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <param name="bMode">true:通常 false:Count(*)</param>
    ''' <param name="nSelectCnt">0以外の場合 top 10 とかにする</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_Select(ByRef sql As String, ByVal bMode As Boolean, Optional ByVal nSelectCnt As Integer = 0)
        If bMode Then
            sql = "SELECT "
            If 0 <> nSelectCnt Then sql &= "TOP " & nSelectCnt & " "
            sql &= "ISNULL(D.ID,''),"
            sql &= "ISNULL(D.伝文区分,0),"
            sql &= "ISNULL(D.キー情報,''),"
            sql &= "ISNULL(D.管理No,''),"
            sql &= "ISNULL(D.送信日付,''),"
            sql &= "ISNULL(D.送信日時,'')"
        Else
            sql = "SELECT "
            sql &= "COUNT(*) "
        End If
    End Sub
    ''' <summary>
    ''' SQL文生成 (FROM句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_From(ByRef sql As String)
        'sql &= "FROM " & DB_BUF_FF_S_ZISEKI & " D "
    End Sub
    ''' <summary>
    ''' SQL文生成 (WHERE句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_Where(ByRef sql As String)
        Dim strWhere As String = ""             ' Where句ワーク
        Dim ii As Integer
        Dim bWk As Boolean
        Dim bKubun As Boolean = False


        '''' 時刻フィルタ
        '' 開始
        If chkStartDay.Checked Then
            strWhere &= "AND D.登録日付 >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
        End If
        '' 終了
        If chkEndDay.Checked Then
            strWhere &= "AND D.登録日付 <= '" & dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss") & "' "
        End If

        If 0 < GetLstCheckCount(m_lstChkKind) Then        ' 区分（伝文区分）フィルタが選択あり
            bWk = False                                         ' 区分フィルタが1件でも有り時 true
            For ii = 0 To m_lstChkKind.Count - 1
                If Not m_lstChkKind.Item(ii).Checked Then       ' 伝文区分 チェックなし
                    Continue For
                End If

                If Not bWk Then
                    strWhere &= "AND ("
                Else
                    strWhere &= " OR "
                End If
                ' 稼動状態セット
                strWhere &= " D.伝文区分 = " & CInt(m_lstChkKind.Item(ii).Tag).ToString
            Next ii
            If bWk Then
                strWhere &= ") "
            End If

            strWhere &= ") "
        End If

        '''' WHERE句生成
        If 0 <> strWhere.Length Then
            sql &= "WHERE 1=1 " & strWhere
        End If
    End Sub

    ''' <summary>
    ''' SQL文生成 (END句)
    ''' </summary>
    ''' <param name="sql">生成SQL文</param>
    ''' <param name="bMode">true:通常 false:Count(*)</param>	
    ''' <remarks></remarks>
    Protected Friend Overridable Sub subSQL_Create_End(ByRef sql As String, ByVal bMode As Boolean)
        If bMode Then
            sql &= "ORDER BY D.登録日付 DESC, D.ID  DESC"
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検索実行
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検索実行中断
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ExecCancel_Log()
        m_bLog_ExecCancel = True
    End Sub
    ''' <summary>
    ''' ログ表示開始時
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ExecStart()
        my_btnFunction(0).Enabled = False
        my_btnFunction(4).Enabled = False
        my_btnFunction(9).Enabled = False
        my_btnFunction(10).Enabled = False
        my_btnFunction(11).Enabled = False
        m_bLog_ExecCancel = False
    End Sub
    ''' <summary>
    ''' ログ表示終了時
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ExecEnd()
        my_btnFunction(0).Enabled = True
        my_btnFunction(4).Enabled = True
        'プリンタがなければ、画面印刷無効
        If Not tClass_HeadCopy.IsCheckPrinter Then
            my_btnFunction(9).Enabled = False
        Else
            my_btnFunction(9).Enabled = True
        End If
        my_btnFunction(10).Enabled = True
        my_btnFunction(11).Enabled = True
        m_bLog_ExecCancel = False
    End Sub

    ''' <summary>
    ''' 検索実行
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ExecSelect_Log()
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim ii As Integer = 0
        Dim rowCnt As Integer = 0
        Dim rowMaxCnt As Integer = 0

        Try
            ' 初期化
            Call ExecStart()
            dgvList.RowCount = 0
            If Not tcls_DB.Open() Then Exit Sub
            Application.DoEvents()

            ' リスト件数取得
            Call subSQL_Create(strSQL, False)
            If Not tcls_DB.GetSelectKey(strSQL, rowMaxCnt) Then Exit Sub
            dgvList.RowCount = rowMaxCnt
            'lblRowCount.Text = rowMaxCnt.ToString("#,##0")
            prbReadRow.Minimum = 0
            prbReadRow.Value = 0
            prbReadRow.Maximum = rowMaxCnt
            prbReadRow.Visible = True
            Application.DoEvents()

            '''' データを取得
            Call subSQL_Create(strSQL, True, rowMaxCnt)
            sqlRead = tcls_DB.SelectExecute(strSQL) ' Select文実行
            If sqlRead Is Nothing Then Exit Sub
            ' データ取り出し
            While sqlRead.Read()

                '' データ取り出し
                'dgvLog(em_LogListColm.idayHenTime, rowCnt).Value = sqlRead.GetDateTime(0).ToString("yyyy/MM/dd HH:mm:ss")
                'dgvLog(em_LogListColm.iKizuNo, rowCnt).Value = sqlRead.GetString(7)
                'dgvLog(em_LogListColm.iCoilNo, rowCnt).Value = sqlRead.GetString(8)
                'dgvLog(em_LogListColm.iKado, rowCnt).Value = sqlRead.GetInt32(1)
                'dgvLog(em_LogListColm.iKensa_O, rowCnt).Value = sqlRead.GetInt32(2)
                'dgvLog(em_LogListColm.iKiki_O, rowCnt).Value = sqlRead.GetInt32(4)
                'dgvLog(em_LogListColm.iKensa_U, rowCnt).Value = sqlRead.GetInt32(3)
                'dgvLog(em_LogListColm.iKiki_U, rowCnt).Value = sqlRead.GetInt32(5)
                'dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(6)

                ' プログレスバー
                prbReadRow.PerformStep()


                If 0 = (rowCnt Mod 100) Then
                    Application.DoEvents()
                End If

                '''' 次行準備
                rowCnt += 1
                If rowCnt >= rowMaxCnt Then Exit Sub
                If m_bLog_ExecCancel Then                   ' キャンセル
                    ' キャンセルだったら 表示している行数だけにする
                    dgvList.RowCount = rowCnt
                    'lblRowCount.Text = rowCnt.ToString
                    Exit Sub
                End If
            End While

            ' DB切断
            tcls_DB.Close()
        Catch ex As Exception
        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

            ' コントロール初期化
            prbReadRow.Visible = False
            Call ExecEnd()
        End Try
    End Sub


    ''' <summary>
    ''' 時刻範囲設定チェックボックス
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkStartEndDay_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkStartDay.CheckedChanged, chkEndDay.CheckedChanged
        If chkStartDay Is sender Then
            dtpStartDay.Enabled = chkStartDay.Checked
            dtpStartTime.Enabled = chkStartDay.Checked
        Else
            dtpEndDay.Enabled = chkEndDay.Checked
            dtpEndTime.Enabled = chkEndDay.Checked
        End If
    End Sub
    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel, dtpEndDay.MouseWheel, dtpEndTime.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    '***************************************************************************
    ' DGV関連関数
    '***************************************************************************
    ''' <summary>
    ''' グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt_DataGridView_turn)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)

        Try
            With dgv
                .ScrollBars = ScrollBars.Both                                   ' スクロールバー
                .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' セル選択
                .ColumnHeadersHeight = 24                                       ' 列ヘッダ高さ調整
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
                .DefaultCellStyle().SelectionForeColor = Color.Black            '
                .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
                .DefaultCellStyle().ForeColor = Color.Black
                .RowCount = 0

                For col As EM_LIST_COL = EM_LIST_COL.nID To EM_LIST_COL.MAX_LIST_COL
                    Select Case col
                        Case EM_LIST_COL.nID
                            ' ID
                            dgvColSetText(dgv, "ID", 160, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_LIST_COL.nKEY
                            ' キー情報
                            dgvColSetText(dgv, "キー情報", 180, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_LIST_COL.nDATE
                            ' 登録日時
                            dgvColSetText(dgv, "登録日時", 240, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_LIST_COL.nSEND
                            ' 送信日時
                            dgvColSetText(dgv, "送信日時", 240, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_LIST_COL.nKANRI
                            ' 管理No
                            dgvColSetText(dgv, "管理No", 180, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_LIST_COL.nKIND
                            ' 区分
                            dgvColSetText(dgv, "区分", 240, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    End Select
                Next
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("FF実績送信履歴画面結果欄初期化異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' データグリッドビューテキストボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Private Sub dgvColSetText(ByRef dgv As tCnt_DataGridView_turn, ByVal sHeader As String, ByVal nWidth As Integer,
     ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode,
     ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
     Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    '***************************************************************************
    ' その他 内部関数
    '***************************************************************************
    ''' <summary>
    ''' チェックボックス チェック状態変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chk_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ' チェックボックス 背景変更
        SetChkBackColor_Change(CType(sender, CheckBox))
    End Sub

    ''' <summary>
    ''' 伝文区分チェックボックス登録
    ''' </summary>
    ''' <param name="chk"></param>
    ''' <param name="value"></param>
    Private Sub AddCheckBoxKind(ByVal chk As CheckBox, ByVal value As Integer)
        chk.Tag = value             ' Tagに区分値を追加
        m_lstChkKind.Add(chk)       ' リストに登録
        '' ついでにクリックイベントハンドラに追加
        AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
    End Sub

    ''' <summary>
    ''' チェックボックス 背景色変更
    ''' </summary>
    ''' <param name="chk">チェックボックス</param>
    ''' <remarks></remarks>
    Private Sub SetChkBackColor_Change(ByVal chk As CheckBox)
        If chk.Checked Then                 ' ON
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' リスト配列に登録されているコントロールのチェック済み個数を算出
    ''' </summary>
    ''' <param name="lstChk"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Protected Friend Function GetLstCheckCount(ByVal lstChk As List(Of CheckBox)) As Integer
        Dim iCnt As Integer = 0
        For ii As Integer = 0 To lstChk.Count - 1
            If lstChk.Item(ii).Checked Then iCnt += 1
        Next ii
        Return iCnt
    End Function



End Class
