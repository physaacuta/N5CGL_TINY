Imports tClassLibrary


Public Class frmMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グラフプロットデータ
    ''' </summary>
    Private Structure TYP_PROTDATA
        Dim sCoilNo As String                       ' コイルNo
        Dim nCoilLen As Integer                     ' 検査実測長 [m] (\ 1000) する

        Dim nCntO() As Integer                      ' 表対象疵件数
        Dim nCntU() As Integer                      ' 裏対象疵件数

        'イニシャライズ
        Public Sub initialize()
            ReDim nCntO(GC_MAX_SELECT_TYPE - 1)
            ReDim nCntU(GC_MAX_SELECT_TYPE - 1)
        End Sub
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mfrm_Setting As New frmDspSetting                ' 表示条件設定画面
    Private WithEvents mcls_Map_O As New tClass.tClass_MapBase
    Private WithEvents mcls_Map_U As New tClass.tClass_MapBase
    Private mcls_pMap(1) As tClass.tClass_MapBase                       ' マップ描画基底クラス (使いやすいように配列化)


    '' 保持ワークデータ
    Private m_nProtData As Integer                                      ' 有効プロットデータ件数
    Private m_typProtData(GC_MAX_PROTDATA - 1) As TYP_PROTDATA          ' グラフプロットデータ (保持ワーク領域)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '''' タイトル設定
        Call SetTitleName(Me)

        '''' 設定画面を自フォームの子供に追加
        Me.AddOwnedForm(mfrm_Setting)

        '''' メンバー変数準備
        ' 使いやすいように配列化
        mcls_pMap(0) = mcls_Map_O
        mcls_pMap(1) = mcls_Map_U


        '''' その他 初期準備
        Call dgvInitialize(dgvCount)

        '''' マップ初期定義
        Call MapInitialize()

        '''' 画面初期化
        Call DspInit()

        ' 疵点マップの表示ボタンのサイズを調整
        btnDspDefMap.Visible = True

        With lblF1
            .Location = New Point(1157, 9)
            .Size = New Size(40, 20)
        End With

        With lblF5
            .Location = New Point(1157, 39)
            .Size = New Size(40, 20)
            .Visible = True
        End With

        With btnSetting
            .Location = New Point(1180, 4)
            .Size = New Size(78, 30)
            .Font = New Font("ＭＳ 明朝", 9.75, FontStyle.Bold)
            .Text = "表示設定"
        End With

        With btnDspDefMap
            .Location = New Point(1180, 34)
            .Size = New Size(78, 30)
            .Visible = True
        End With

    End Sub

    ''' <summary>
    ''' フォームロード完了 (frmBaseのProtected Overrides Sub LoadEnding()からのオーバーライドはヤメ)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmMenu_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        '''' 画面表示要求
        Call gfrm_Setting_EvDsp()
    End Sub

    ''' <summary>
    ''' 画面 閉じようとした
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' メインスレッド終了通知
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' フォームキーダウン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTansho_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.F1 Then                ' 表示条件設定画面
            Call btnSetting_Click(Nothing, Nothing)

        ElseIf e.KeyCode = Keys.F5 Then            ' 疵点マップ画面
            Call btnDspDefMap_Click(Nothing, Nothing)

        ElseIf True = e.Shift And e.KeyCode = Keys.F12 Then         ' 終了

            Call modMain.SendMailDefMap(FACT_KS_END)

            'タスク終了
            Me.Close()
        End If
    End Sub

    ''' <summary>
    ''' 表示条件設定画面ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSetting_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSetting.Click
        ' 表示条件設定画面
        Call mfrm_Setting.Disp()
        Call mfrm_Setting.Show()
    End Sub

    ''' <summary>
    ''' 疵点マップボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnDspDefMap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDspDefMap.Click
        Dim strExecPath As String = String.Format(".\\{0}.exe", SO_DEFMAP)

        Try
            Call Diagnostics.Process.Start(strExecPath)
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵点マップ起動エラー")
        End Try
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 画面初期表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting

        '''' ヘッダー部
        lblOrderNo.Text = ""
        lblCoilNo.Text = ""
        lblLength.Text = ""
        lblSizeZai.Text = ""
        lblSizeSei.Text = ""

        '''' グリッド部分
        ' 1行目
        For ii As Integer = 0 To dgvCount.Columns.Count - 1
            dgvCount(ii, 0).Value = typ.sDspType(ii)
            dgvCount(ii, 0).Style.ForeColor = typ.colorLine(ii)
        Next ii
        ' 2行目
        For ii As Integer = 0 To dgvCount.Columns.Count - 1
            dgvCount(ii, 1).Value = "0/0"
        Next ii
    End Sub



    ''' <summary>
    ''' 画面データ描画 (設定変更時にイベント実行) (画面表示時にも)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub gfrm_Setting_EvDsp() Handles mfrm_Setting.EvDsp
        '''' 画面初期化
        Call DspInit()

        '''' マップ領域再定義
        Call MapReSetring()

        '''' プロットデータをセット
        Call GetProtData()

        '''' 疵個数情報欄表示領域再設定
        Call dgvWidthResize(dgvCount)

        '''' マップ再生成依頼
        For ii As Integer = 0 To mcls_pMap.Length - 1
            Call mcls_pMap(ii).DrawPaint()
        Next

        '''' マップ再描画
        Call picMap1.Refresh()
        Call picMap2.Refresh()
    End Sub

    ''' <summary>
    ''' 表裏のプロットデータを取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetProtData()
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting
        Dim ii As Integer
        Dim strSQL As String

        '''' グラフデータ初期化
        m_nProtData = 0
        For ii = 0 To m_typProtData.Length - 1
            m_typProtData(ii) = New TYP_PROTDATA
            m_typProtData(ii).initialize()
        Next ii


        '''' SQL接続準備
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_RESULT & "]")
            Exit Sub
        End If


        ''''' 期間指定の場合、表示件数を調整
        'If 1 = typ.nSelectMode Then
        '	' 期間内件数取得
        '	strSQL = "SELECT COUNT(*) "
        '	strSQL &= " FROM " & DB_COIL_RESULT & " r "
        '	strSQL &= " WHERE r.登録日付 >= '" & Format(typ.daySelectS, "yyyy/MM/dd ") & "00:00:00'"
        '	strSQL &= " AND r.登録日付 <= '" & Format(typ.daySelectE, "yyyy/MM/dd ") & "23:59:59'"
        '	' 期間内件数取得
        '	Dim nMaxCnt As Integer
        '	If Not tcls_DB.GetSelectKey(strSQL, nMaxCnt) Then
        '		Exit Sub
        '	End If
        '	' 件数調整
        '	If typ.nXmax > nMaxCnt Then
        '		typ.nXmax = nMaxCnt
        '	End If
        'End If

        '''' 検索状態を表示
        If 1 = typ.nSelectMode Then     ' 期間指定の場合
            lblSearch.Text = "期間指定"
            lblSearch.BackColor = Color.Yellow
            lblSearch.Visible = True
        Else                            ' 直近コイル指定の場合
            lblSearch.Visible = False
        End If

        ''''' 疵種別対応
        '''' 疵個数特定フィルタ文
        Dim strSqlFilter As String
        strSqlFilter = "重複結果 <> 1  AND 判定グレード IN("
        For ii = 0 To typ.nDspGrade.Length - 1
            If typ.nDspGrade.Length = 0 Then Continue For

            strSqlFilter &= typ.nDspGrade(ii)
            strSqlFilter &= ","
        Next
        strSqlFilter = strSqlFilter.Substring(0, strSqlFilter.Length - 1)   ' 最後の,を削除
        strSqlFilter &= ")"

        '''' SQL文生成
        strSQL = "SELECT TOP " & typ.nXmax
        strSQL &= " r.管理No, ISNULL(c.命令No,'') as 命令No, ISNULL(c.生産No,'') as 生産No, r.検査実測長, "
        strSQL &= "ISNULL(c.材料板厚,0) as 材料板厚, ISNULL(c.材料板幅,0) as 材料板幅, ISNULL(c.注文板厚,0) as 注文板厚, ISNULL(c.注文板幅,0) as 注文板幅, "

        strSQL &= "ISNULL(o1.個数, 0) as 表疵個数1, ISNULL(o2.個数, 0) as 表疵個数2, ISNULL(o3.個数, 0) as 表疵個数3, ISNULL(o4.個数, 0) as 表疵個数4, ISNULL(o5.個数, 0) as 表疵個数5, "
        strSQL &= "ISNULL(u1.個数, 0) as 裏疵個数1, ISNULL(u2.個数, 0) as 裏疵個数2, ISNULL(u3.個数, 0) as 裏疵個数3, ISNULL(u4.個数, 0) as 裏疵個数4, ISNULL(u5.個数, 0) as 裏疵個数5 "
        strSQL &= " FROM " & DB_COIL_RESULT & " r "
        strSQL &= " LEFT OUTER JOIN " & DB_COIL_INF & " c "
        strSQL &= " ON r.管理No = c.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(0)
        strSQL &= " GROUP BY 管理No ) o1 "
        strSQL &= " ON r.管理No = o1.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(1)
        strSQL &= " GROUP BY 管理No ) o2 "
        strSQL &= " ON r.管理No = o2.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(2)
        strSQL &= " GROUP BY 管理No ) o3 "
        strSQL &= " ON r.管理No = o3.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(3)
        strSQL &= " GROUP BY 管理No ) o4 "
        strSQL &= " ON r.管理No = o4.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(4)
        strSQL &= " GROUP BY 管理No ) o5 "
        strSQL &= " ON r.管理No = o5.管理No "


        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(0)
        strSQL &= " GROUP BY 管理No ) u1 "
        strSQL &= " ON r.管理No = u1.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(1)
        strSQL &= " GROUP BY 管理No ) u2 "
        strSQL &= " ON r.管理No = u2.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(2)
        strSQL &= " GROUP BY 管理No ) u3 "
        strSQL &= " ON r.管理No = u3.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(3)
        strSQL &= " GROUP BY 管理No ) u4 "
        strSQL &= " ON r.管理No = u4.管理No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as 個数, 管理No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND 判定疵種=" & typ.nDspType(4)
        strSQL &= " GROUP BY 管理No ) u5 "
        strSQL &= " ON r.管理No = u5.管理No "


        ' 期間指定の場合のみ
        If 1 = typ.nSelectMode Then
            strSQL &= " WHERE r.登録日付 >= '" & Format(typ.daySelectS, "yyyy/MM/dd ") & "00:00:00'"
            strSQL &= " AND r.登録日付 <= '" & Format(typ.daySelectE, "yyyy/MM/dd ") & "23:59:59'"
        End If

        strSQL &= " ORDER BY r.登録日付 desc "


        '''' データ取得
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_RESULT & "]")
            Exit Sub
        End If

        ' 取り出し
        Try
            While sqlRead.Read()
                '' 通常のデータ取得
                m_typProtData(m_nProtData).sCoilNo = sqlRead.GetString(sqlRead.GetOrdinal("生産No"))
                m_typProtData(m_nProtData).nCoilLen = sqlRead.GetInt32(sqlRead.GetOrdinal("検査実測長")) \ 1000
                m_typProtData(m_nProtData).nCntO(0) = sqlRead.GetInt32(sqlRead.GetOrdinal("表疵個数1"))
                m_typProtData(m_nProtData).nCntO(1) = sqlRead.GetInt32(sqlRead.GetOrdinal("表疵個数2"))
                m_typProtData(m_nProtData).nCntO(2) = sqlRead.GetInt32(sqlRead.GetOrdinal("表疵個数3"))
                m_typProtData(m_nProtData).nCntO(3) = sqlRead.GetInt32(sqlRead.GetOrdinal("表疵個数4"))
                m_typProtData(m_nProtData).nCntO(4) = sqlRead.GetInt32(sqlRead.GetOrdinal("表疵個数5"))
                m_typProtData(m_nProtData).nCntU(0) = sqlRead.GetInt32(sqlRead.GetOrdinal("裏疵個数1"))
                m_typProtData(m_nProtData).nCntU(1) = sqlRead.GetInt32(sqlRead.GetOrdinal("裏疵個数2"))
                m_typProtData(m_nProtData).nCntU(2) = sqlRead.GetInt32(sqlRead.GetOrdinal("裏疵個数3"))
                m_typProtData(m_nProtData).nCntU(3) = sqlRead.GetInt32(sqlRead.GetOrdinal("裏疵個数4"))
                m_typProtData(m_nProtData).nCntU(4) = sqlRead.GetInt32(sqlRead.GetOrdinal("裏疵個数5"))


                '' 最初の１件目のみの特殊処理
                If m_nProtData = 0 Then
                    lblOrderNo.Text = sqlRead.GetString(sqlRead.GetOrdinal("命令No"))
                    lblCoilNo.Text = m_typProtData(m_nProtData).sCoilNo
                    lblLength.Text = m_typProtData(m_nProtData).nCoilLen.ToString
                    lblSizeZai.Text = String.Format("{0:#0.000}×{1}", sqlRead.GetInt32(sqlRead.GetOrdinal("材料板厚")) / 1000, sqlRead.GetInt32(sqlRead.GetOrdinal("材料板幅")))
                    lblSizeSei.Text = String.Format("{0:#0.000}×{1}", sqlRead.GetInt32(sqlRead.GetOrdinal("注文板厚")) / 1000, sqlRead.GetInt32(sqlRead.GetOrdinal("注文板幅")))


                    ' 件数セット
                    For ii = 0 To dgvCount.Columns.Count - 1
                        dgvCount(ii, 1).Value = String.Format("{0}/{1}", m_typProtData(m_nProtData).nCntO(ii), m_typProtData(m_nProtData).nCntU(ii))
                    Next ii
                End If

                m_nProtData += 1
            End While

        Catch ex As Exception
            m_nProtData = 0
        Finally
            tcls_DB.Cancel()
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


        '''' 期間指定の場合、マップ領域のY軸調整
        If 1 = typ.nSelectMode Then
            For ii = 0 To mcls_pMap.Length - 1
                With mcls_pMap(ii)
                    Dim nProntData As Integer = m_nProtData
                    If m_nProtData = .tProp_MapInf_LimitXmin Then
                        nProntData = 0
                    End If

                    '' マップ領域
                    .tProp_MapInf_LimitXmax = nProntData
                End With
            Next ii
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ フィルターグリッド 動作定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵種フィルター グリッド初期化
    ''' </summary>
    ''' <param name="dgv">フィルター用のデータグリッド</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        '''' 独自の項目を設定
        With dgv
            .RowCount = 2

            '' 定義
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 行選択
            .ScrollBars = ScrollBars.Horizontal
            .ReadOnly = True
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (DataGridViewSelectionCancelが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 14.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .DefaultCellStyle.WrapMode = DataGridViewTriState.False


            '' 列定義
            .ColumnCount = GC_MAX_SELECT_TYPE                           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersVisible = False
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 13.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' 列ヘッダサイズ


            '行定義
            For ii As Integer = 0 To .Rows.Count - 1
                .Rows(ii).Height = 28   '行の幅をセット
            Next ii

            '' 列定義
            For ii As Integer = 0 To .Columns.Count - 1
                .Columns(ii).Width = 96
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
            Next ii

            '' 列幅変更イベント設定
            AddHandler .ColumnWidthChanged, AddressOf dgvColumnWidthChanged

        End With
    End Sub

    ''' <summary>
    ''' 列幅のサイズ変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvColumnWidthChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewColumnEventArgs)

        Dim dgv As tCnt.tCnt_DataGridView = CType(sender, tCnt.tCnt_DataGridView)

        ' 疵個数情報欄表示領域再設定
        dgvWidthResize(dgv)

    End Sub

    ''' <summary>
    ''' 疵個数情報欄表示領域再設定
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvWidthResize(ByVal dgv As tCnt.tCnt_DataGridView)
        Dim hsc As HScrollBar
        Dim nHeight As Integer

        ' 行幅を取得
        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            ' 水平スクロールバー表示中
            nHeight = CInt((dgv.ClientSize.Height - hsc.Height) / dgv.Rows.Count)
        Else
            ' 水平スクロールバー未表示
            nHeight = CInt(dgv.ClientSize.Height / dgv.Rows.Count)
        End If

        ' 行数分の設定
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv.Rows(ii).Height = nHeight   ' 行幅を設定

            ' 列数分の文字サイズを設定
            For jj As Integer = 0 To dgv.ColumnCount - 1
                If hsc.Visible Then
                    ' 水平スクロールバー表示中
                    dgv.Item(jj, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Else
                    ' 水平スクロールバー未表示
                    Dim emSize As Single = 14.0
                    ' 表と裏の疵個数値をセル領域内に全て表示できない場合（9文字以上）
                    If ii = 1 AndAlso Not dgv(jj, ii).Value Is Nothing AndAlso dgv(jj, ii).Value.ToString.Count > 8 Then
                        ' セル領域内に文字を全て表示できるように文字サイズを変更
                        emSize = 10.0
                    End If

                    dgv(jj, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", emSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End If
            Next
        Next

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ マップ関係
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' マップ基本情報定義
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInitialize()
        '''' グラフ描画に必要な情報を設定
        For ii As Integer = 0 To mcls_pMap.Length - 1
            With mcls_pMap(ii)
                '' 表示領域情報
                .tProp_AriaInf_BackGroundLineColor = Pens.Black
                .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
                .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
                .tProp_AriaInf_AriaSize = picMap1.Size
                .tProp_AriaInf_Margin = New Padding(30, 10, 30, 40)

                '' マップ領域
                .tProp_MapInf_IsCenterX = False
                .tProp_MapInf_IsCenterY = False
                .tProp_MapInf_IsBaseX_L = True
                .tProp_MapInf_IsBaseY_T = False
                .tProp_MapInf_IsStrechX = True
                .tProp_MapInf_IsStrechY = True
                .tProp_MapInf_LimitXmin = 1
                .tProp_MapInf_LimitXmax = 1000
                .tProp_MapInf_LimitYmin = 0
                .tProp_MapInf_LimitYmax = 100
                .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

                '' マップ対象外領域設定
                .tProp_LimitInf_LimitDraw = False
                '''' コイルを設定する
                '.tProp_LimitInf_LimitXmin = .tProp_MapInf_LimitXmin
                '.tProp_LimitInf_LimitXmax = .tProp_MapInf_LimitXmax
                '.tProp_LimitInf_LimitYmin = .tProp_MapInf_LimitYmin
                '.tProp_LimitInf_LimitYmax = .tProp_MapInf_LimitYmax

                '' メモリ設定
                Dim p As Pen = New Pen(Color.LightBlue, 1)
                p.DashStyle = Drawing2D.DashStyle.DashDot
                .tProp_MemoriInf_LinePen = p
                .tProp_MemoriInf_StringFont = New Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
                .tProp_MemoriInf_PosStringX = 10
                .tProp_MemoriInf_PosStringY = 1
                .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(100, 10)
                .tProp_MemoriInf_ScaleStringFormatX = "0本前"
                .tProp_MemoriInf_ScaleStringFormatY = "0'%'"
                .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1)
                .tProp_MemoriInf_StringDisp_T = False
                .tProp_MemoriInf_StringDisp_B = True
                .tProp_MemoriInf_StringDisp_L = True
                .tProp_MemoriInf_StringDisp_R = False
                .tProp_MemoriInf_LineDsp_X = True
                .tProp_MemoriInf_LineDsp_Y = True
            End With
        Next
    End Sub

    ''' <summary>
    ''' マップ再定義
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapReSetring()
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting

        '''' グラフ描画に必要な情報を設定
        For ii As Integer = 0 To mcls_pMap.Length - 1
            With mcls_pMap(ii)
                '' マップ領域
                .tProp_MapInf_LimitXmax = typ.nXmax
                .tProp_MapInf_LimitYmax = typ.nYmax

                '' メモリ設定
                .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(typ.nXinterval, typ.nYinterval)
            End With
        Next
    End Sub

    ''' <summary>
    ''' 描画
    ''' </summary>
    Private Sub PictureBox1_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles picMap1.Paint, picMap2.Paint
        Dim ou As Integer = CInt(IIf(sender Is picMap1, 0, 1))   ' 表裏

        '''' ビットマップ取得
        Dim bmp As Bitmap = mcls_pMap(ou).tProp_Bmp
        If bmp Is Nothing Then Return

        '''' 背景をセット
        e.Graphics.DrawImage(bmp, 0, 0)
    End Sub




    ''' <summary>
    ''' 画面データ描画
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics) Handles mcls_Map_O.tEv_DrawPaint_End, mcls_Map_U.tEv_DrawPaint_End
        If m_nProtData = 0 Then Return

        Dim ou As Integer = CInt(IIf(cls Is mcls_Map_O, 0, 1))   ' 表裏
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting


        '''' 描画
        Dim dVal As Single
        Dim x1, y1, x2, y2 As Integer
        For ii As Integer = typ.nDspType.Length - 1 To 0 Step -1
            If typ.nDspType(ii) = 0 Then Continue For

            ' 始点算出 (先頭の座標算出)
            x1 = cls.GetVtoP_X(1)
            dVal = CSng(CInt(IIf(ou = 0, m_typProtData(0).nCntO(ii), m_typProtData(0).nCntU(ii))) / m_typProtData(0).nCoilLen) * 100
            If dVal > typ.nYmax Then dVal = typ.nYmax
            y1 = cls.GetVtoP_Y(dVal)

            Using p As New Pen(typ.colorLine(ii), 1)
                ' 始点のみプロットの指定
                Select Case ii
                    Case 0
                        g.FillEllipse(New SolidBrush(p.Color), x1 - 4, y1 - 4, 8, 8)       ' ●
                    Case 1
                        g.FillRectangle(New SolidBrush(p.Color), x1 - 3, y1 - 3, 6, 6)      ' ■
                    Case 2
                        g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x1, y1 + 4), New Point(x1 + 4, y1 - 4), New Point(x1 - 4, y1 - 4)})    ' ▼
                    Case 3
                        g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x1, y1 - 4), New Point(x1 + 4, y1 + 4), New Point(x1 - 4, y1 + 4)})   ' ▲
                    Case Else
                        g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x1, y1 - 4), New Point(x1 + 4, y1), New Point(x1, y1 + 4), New Point(x1 - 4, y1)}) ' ◆
                End Select


                For jj As Integer = 1 To m_nProtData - 1
                    ' 終点算出
                    x2 = cls.GetVtoP_X(jj + 1)
                    dVal = CSng(CInt(IIf(ou = 0, m_typProtData(jj).nCntO(ii), m_typProtData(jj).nCntU(ii))) / m_typProtData(jj).nCoilLen) * 100
                    If dVal > typ.nYmax Then dVal = typ.nYmax
                    y2 = cls.GetVtoP_Y(dVal)

                    ' 描画
                    g.DrawLine(p, x1, y1, x2, y2)

                    ' プロットの指定
                    Select Case ii
                        Case 0
                            g.FillEllipse(New SolidBrush(p.Color), x2 - 4, y2 - 4, 8, 8)       ' ●
                        Case 1
                            g.FillRectangle(New SolidBrush(p.Color), x2 - 3, y2 - 3, 6, 6)      ' ■
                        Case 2
                            g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x2, y2 + 4), New Point(x2 + 4, y2 - 4), New Point(x2 - 4, y2 - 4)})   ' ▼
                        Case 3
                            g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x2, y2 - 4), New Point(x2 + 4, y2 + 4), New Point(x2 - 4, y2 + 4)})   ' ▲
                        Case Else
                            g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x2, y2 - 4), New Point(x2 + 4, y2), New Point(x2, y2 + 4), New Point(x2 - 4, y2)}) ' ◆
                    End Select


                    ' 次の準備
                    x1 = x2
                    y1 = y2
                Next jj
            End Using

        Next ii


        '''' メモリ軸のコイルNoを表示
        Dim b As SolidBrush = New SolidBrush(Color.Black)
        Dim f As Font = New Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        ' 原点
        x1 = cls.GetVtoP_X(1)
        y1 = CInt(IIf(ou = 0, picMap1.Height, picMap2.Height))
        cls.DrawString(g, x1, y1, 0, -1, m_typProtData(0).sCoilNo, ContentAlignment.TopCenter, f, b)
        ' メモリ部
        For ii As Integer = typ.nXinterval To m_nProtData Step typ.nXinterval
            ' 表示するデータは、メモリが100の場合は、配列の99。メモリ１が配列の0を表示している為
            x1 = cls.GetVtoP_X(ii)
            cls.DrawString(g, x1, y1, 0, -1, m_typProtData(ii - 1).sCoilNo, ContentAlignment.TopCenter, f, b)
        Next ii
        b.Dispose()
        f.Dispose()
    End Sub


End Class
