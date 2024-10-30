'*********************************************************************************
' 異常状態表示画面
'	[Ver]
'		Ver.01    2010/11/15 初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuForm
Imports tClassLibrary.tClass.tClass_SQLServer

Public Class frmStart
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    'ラジオボタン区分
    Private Enum EM_RAD_KUBUN
        nErr = 0      '異常のみ表示
        nDefult = 1   '正常＋異常
        nAll = 2      '全て
    End Enum
    '// データグリッド定数
    Public Enum en_KikiList                 ' 縮退状態表示
        iNo = 0                             ' NO
        iKiKiName                           ' 機器名称
        iIjouHani                           ' 異常範囲
        iKensa                              ' 検査状態
        iKiki                               ' 機器状態
        iData                               ' 生値
        iMsg                                ' 異常メッセージ
        iBikou                              ' 備考
    End Enum
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nRadKubun As EM_RAD_KUBUN            'ラジオボタン選択(0:異常のみ表示,1:正常＋異常,2:全て)

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub

    ''' <summary>
    ''' フォームアクティブ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSyukutai_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Activated

        '移動可能
        Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.tProp_EndButtonEnable = False

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)
    End Sub

    ''' <summary>
    ''' 画面ロード時 初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmSyukutai_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        'タイトル設定
        modMain.SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        ' データグリッド 初期設定
        ' 機器状態表示
        dgvInitialize(dgvKiki)

        '初期設定
        Call InitDsp()

        'デフォルト異常情報のみ表示
        m_nRadKubun = EM_RAD_KUBUN.nErr
        radErr.Checked = True

        'タイマー周期設定
        tmrDsp.Enabled = True
        tmrDsp.Interval = tMod.tMod_IniFile.ReadIniInt(SO_GAMENN, "SYUKUTAI_TIMER", TASKINI_NAME, 10000)

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 機器状態表示 グリッド初期化
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 機器状態表示 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)

        With dgv
            .RowCount = 0
            .ColumnCount = 0

            '' 動作定義
            .AutoGenerateColumns = False            ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False             ' ユーザー 行追加
            .AllowUserToDeleteRows = False          ' ユーザー 行削除
            .AllowUserToOrderColumns = False        ' ユーザー 列移動
            .AllowUserToResizeColumns = True        ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False          ' ユーザー 行サイズ変更
            .ShowCellErrors = False                 ' セルエラーを表示しない
            .ShowCellToolTips = False               ' ツールヒントを表示しない
            .ShowEditingIcon = False                ' 
            .ShowRowErrors = False                  '

            .SelectionMode = DataGridViewSelectionMode.CellSelect       ' セル選択
            .MultiSelect = False                                         ' 複数行選択
            .ReadOnly = True                                             ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditOnEnter                 ' セル書き換え


            .ScrollBars = ScrollBars.Both                                                           'スクロールバー有効
            .ColumnHeadersVisible = True                                                            ' 列ヘッダ表示
            .RowHeadersVisible = False                                                              ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None                             ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None                                   ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1行おきに色替え

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 機器名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検査状態
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検査状態
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 機器状態
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 生値
            .Columns.Add(New DataGridViewTextBoxColumn)   ' メッセージ

            '''' 列定義
            '1列目:NO
            .Columns(en_KikiList.iNo).HeaderText = "NO" & vbCrLf
            .Columns(en_KikiList.iNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).Width = 50
            .Columns(en_KikiList.iNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_KikiList.iNo).Frozen = True


            '2列目:機器名称
            .Columns(en_KikiList.iKiKiName).HeaderText = "機器名称" & vbCrLf
            .Columns(en_KikiList.iKiKiName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiKiName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiKiName).Width = 200
            .Columns(en_KikiList.iKiKiName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:異常範囲
            .Columns(en_KikiList.iIjouHani).HeaderText = "異常範囲" & vbCrLf
            .Columns(en_KikiList.iIjouHani).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iIjouHani).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iIjouHani).Width = 90
            .Columns(en_KikiList.iIjouHani).SortMode = DataGridViewColumnSortMode.NotSortable


            '4列目:検査状態
            .Columns(en_KikiList.iKensa).HeaderText = "検査状態" & vbCrLf
            .Columns(en_KikiList.iKensa).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKensa).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKensa).Width = 90
            .Columns(en_KikiList.iKensa).SortMode = DataGridViewColumnSortMode.NotSortable


            '5列目:機器状態
            .Columns(en_KikiList.iKiki).HeaderText = "機器状態" & vbCrLf
            .Columns(en_KikiList.iKiki).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKiki).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiki).Width = 90
            .Columns(en_KikiList.iKiki).SortMode = DataGridViewColumnSortMode.NotSortable


            '6列目:生値
            .Columns(en_KikiList.iData).HeaderText = "生値" & vbCrLf
            .Columns(en_KikiList.iData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).Width = 50
            .Columns(en_KikiList.iData).SortMode = DataGridViewColumnSortMode.NotSortable


            '7列目:異常MSG
            .Columns(en_KikiList.iMsg).HeaderText = "異常MSG" & vbCrLf
            .Columns(en_KikiList.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).Width = 1000
            .Columns(en_KikiList.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    cmdEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 処理終了
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Me.Close()

    End Sub

    ''' <summary>
    ''' 定周期タイマー
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrDsp.Tick

        'DB接続チェック
        If Not OpenCheck(TASKINI_NAME, INI_DB) Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        'データ表示
        Call DspData()


    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nMaxRowCnt As Integer  '最大行数
        Dim nRowCnt As Integer = 0

        'DBオープン
        If Not tcls_DB.Open() Then
            'gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '行数取得SQL作成
        strSQL = GetSQL(False)

        'レコード読み込み
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            'gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "データ取得エラー [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '最大行数セット
        dgvKiki.RowCount = nMaxRowCnt

        '表示データの項目取得
        strSQL = ""
        strSQL = GetSQL(True)

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' データ表示
            While sqlRead.Read()

                ' データ取り出し
                dgvKiki(en_KikiList.iNo, nRowCnt).Value = sqlRead.GetInt32(0)           '機器ID
                dgvKiki(en_KikiList.iKiKiName, nRowCnt).Value = sqlRead.GetString(1)    '機器名

                ' 異常範囲
                If 0 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "全体"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 1 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "表面"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 2 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "裏面"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                ElseIf -1 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "ｵﾌﾗｲﾝのみ"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                Else
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = ""
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty

                End If

                '検査状態表示
                If 0 = sqlRead.GetInt32(3) Then
                    '検査異常度なし
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then

                    '機器状態が異常の場合は検査異常度を見て表示する
                    Select Case sqlRead.GetInt32(3)
						Case EM_DIV_KENSA.DIV_KENSA_NG
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "縮退"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
						Case EM_DIV_KENSA.DIV_KENSA_DBNON
							dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "未保存"
							dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
						Case EM_DIV_KENSA.DIV_KENSA_SKIP
							dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "未検査"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KENSA.DIV_KENSA_STOP
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "停止"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case CType(-1, EM_DIV_KENSA)
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "停止"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KENSA.DIV_KENSA_MENTE
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "ﾒﾝﾃ中"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KENSA.DIV_KENSA_EDGENG
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "ｴｯｼﾞ不正"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case Else
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "不明"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                    End Select

                Else

                    '検査異常度あり、機器状態正常
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "正常"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                End If

                '機器状態表示
                If 0 = sqlRead.GetInt32(4) Then

                    '機器異常度なし
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then

                    '機器状態が異常の場合は、機器異常度を見て表示する
                    Select Case sqlRead.GetInt32(4)
                        Case EM_DIV_KIKI.DIV_KIKI_KEI
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "軽故障"
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KIKI.DIV_KIKI_JYU
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "重故障"
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Pink
                        Case Else
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "不明"
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Pink
                    End Select

                Else

                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "正常"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                End If

                '生値として機器状態を表示する。
                dgvKiki(en_KikiList.iData, nRowCnt).Value = CStr(sqlRead.GetInt32(6))    '生値

                dgvKiki(en_KikiList.iMsg, nRowCnt).Value = sqlRead.GetString(5)          '異常MSG

                '''' 次行準備
                nRowCnt += 1

            End While

        Catch ex As Exception
            'gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "縮退状態表示エラー")
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' SQL文取得
    ''' </summary>
    ''' <param name="bMode">項目取得区分(True:通常,False:個数(count(*)))</param>
    ''' <returns>作成したSQL文</returns>
    ''' <remarks></remarks>
    Private Function GetSQL(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '行数取得
            strSQL &= " SELECT COUNT(*) "

        Else

            '項目取得
            strSQL &= " SELECT a.機器ID,"
            strSQL &= "        a.機器名,"
            strSQL &= "        a.異常範囲,"
            strSQL &= "        a.検査異常度,"
            strSQL &= "        a.機器異常度,"
            strSQL &= "        ISNULL(a.異常MSG,''),"
            strSQL &= "        b.機器状態,"

        End If

        '最後の","を削除
        sqlColumnTrim(strSQL)

        '// 異常のみ表示は現状のまま
        '// すべて表示 ･･･ 機器IDが設定されているデータすべて表示
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.機器ID = b.機器ID "

        ' 選択ボタンによる分岐
        If m_nRadKubun = EM_RAD_KUBUN.nErr Then
            ' 異常情報のみ表示
            strSQL &= " WHERE (a.検査異常度 <> 0 or a.機器異常度 <> 0) "
            strSQL &= " AND b.機器状態 = 1"
            'strSQL &= " AND b.機器状態 <> 0"
        ElseIf m_nRadKubun = EM_RAD_KUBUN.nDefult Then
            ' 正常＋異常 情報表示
            strSQL &= " WHERE (a.検査異常度 <> 0 or a.機器異常度 <> 0) "
        Else
            ' 全て表示 ･･･ 機器IDが設定されているデータすべて表示
        End If

        '作成したSQLを返す
        Return strSQL

    End Function
    ''' <summary>
    ''' 選択行チェンジ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvKiki.SelectionChanged

        '選択を解除
        dgvKiki.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' 行選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowEnter

        '行の背景を変更
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    End Sub
    ''' <summary>
    ''' 行移動
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowLeave

        '行の背景をデフォルト色に戻す
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' ラジオボタンチェックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radbtn_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim radData As RadioButton = CType(sender, RadioButton)

        'チェックが外れた場合は、初期化
        If Not radData.Checked Then
            'radData.BackColor = Color.FromKnownColor(KnownColor.Control)
            radData.BackColor = Color.Empty
            Exit Sub
        End If

        Select Case CInt(radData.Tag)
            Case EM_RAD_KUBUN.nErr
                radData.BackColor = Color.Cyan
            Case EM_RAD_KUBUN.nDefult
                radData.BackColor = Color.Cyan
            Case EM_RAD_KUBUN.nAll
                radData.BackColor = Color.Cyan
        End Select

        'ステータスセット
        m_nRadKubun = CType(radData.Tag, EM_RAD_KUBUN)

        '再表示
        Call DspData()

    End Sub

    ''' <summary>
    ''' 初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        'ラジオボタン初期設定
        radErr.Tag = EM_RAD_KUBUN.nErr
        radDefult.Tag = EM_RAD_KUBUN.nDefult
        radAll.Tag = EM_RAD_KUBUN.nAll

        'イベント追加
        AddHandler radErr.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radDefult.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radAll.CheckedChanged, AddressOf radbtn_CheckedChanged

    End Sub


End Class
