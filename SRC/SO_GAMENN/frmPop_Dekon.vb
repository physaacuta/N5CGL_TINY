'*********************************************************************************
' 出側単位疵混入率CSV出力画面
'	[Ver]
'		Ver.01    2007/08/29  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary

Public Class frmPop_Dekon

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////

    Private Const INI_TYPE_KEY As String = "Dekon_Type"
    Private Const INI_GRADE_KEY As String = "Dekon_Grade"

    'コイル検査結果列No
    Private Enum EM_RESULT_COL
        nDATE = 0       ' 通板日付
        nCoilNo         ' 生産No
        nOrderNo        ' 命令No
        nKoban          ' 分割子番
        nCutPos_F       ' カット範囲(Ｆ)
        nCutPos_T       ' カット範囲(Ｔ)
        nCoilLen        ' コイル長
        nDspKizuCnt_O   ' 表示疵個数表
        nDspKizuCnt_U   ' 表示疵個数裏
        nMixedRate_O    ' 混入率 (表)
        nMixedRate_U    ' 混入率 (裏)
        nKizKanri       ' 疵管理番号

    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部プロパティ
    '/ /////////////////////////////////////////////////////////////////////////////////



    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()


        '初期化
        txtCoil.Text = ""               ' 生産No.
        txtSaveName.Text = ""           ' 管理No.
        lblGuide.Text = ""              ' n件見つかりました

        'デフォルトで日付と、検索結果表示順を降順にする
        chkDateSet.Checked = True       ' 日付フィルタ
        rdoSortDesc.Checked = True      ' 検索結果...降順
        chkNagate.Checked = False       ' 長手方向反転
        chkHaba.Checked = False         ' 幅方向反転


        'データグリッド初期化(共通項目)
        Call dgvInitialize(dgvResult)

        'データグリッド初期化(ライン固有項目)
        Call dgvInitCGL(dgvResult)


        '疵種・グレードフィルターの情報を基底クラスにセット
        MyBase.tProp_IniSection = SO_GAMENN
        MyBase.tProp_TypeKey = INI_TYPE_KEY
        MyBase.tProp_GradeKey = INI_GRADE_KEY
        MyBase.tProp_IniName = LOCALINI_NAME

        '疵種フィルター表示
        MyBase.SetFilterData(dgvDispType, DB_TYPE_NAME, SO_GAMENN, INI_TYPE_KEY)

        'グレードフィルター表示
        MyBase.SetFilterData(dgvDispGrade, DB_GRADE_NAME, SO_GAMENN, INI_GRADE_KEY)

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnSearch, 5 - 1)
        AddFunctionButton(btnEnd, 12 - 1)

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

    End Sub


    ''' <summary>
    ''' フォーム読み込み完了処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Dekon_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Me.Cursor = Cursors.WaitCursor

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        'コイル検索
        Call SearchCoil()
        Me.Cursor = Cursors.Arrow
    End Sub


    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frmSearch_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 5 - 1                                                      ' コイル検索
                    btnSearch_Click(Nothing, Nothing)
                    '-----------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("実績検索画面ファンクションイベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 検索ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        If Not CheckSearch() Then
            lblGuide.Text = "検索キーを入力してください"
            Exit Sub
        End If

        Me.Cursor = Cursors.WaitCursor
        'コイル検索
        Call SearchCoil()
        Me.Cursor = Cursors.Arrow
    End Sub

    ''' <summary>
    ''' 閉じるボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.Hide()
    End Sub


    ''' <summary>
    ''' 検索条件チェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckSearch() As Boolean

        '検査条件が指定されていなければ、エラー
        If txtCoil.Text.Trim = "" And txtSaveName.Text.Trim = "" And
              Not chkDateSet.Checked And Not chkTimeSet.Checked Then
            Return False
        End If

        Return True

    End Function



    ''' <summary>
    ''' コイル検索
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SearchCoil()

        Dim nCnt As Integer = 0             ' カウンタ(件数)
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""
        Dim strSqlKoban As String = ""

        Try
            dgvResult.RowCount = 0

            ' 検索条件 DBオープン
            If Not tcls_DB.Open() Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_CUT_POS & "]")
                Exit Sub
            End If


            ' -----------------------------------------------------------
            ' すべての条件で検索
            ' -----------------------------------------------------------
            Call CreateSql(strSql)                      ' SQL文作成

            ' Select文実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー(1) [" & DB_COIL_CUT_POS & "] SQL = " & strSql)
                Exit Sub
            End If


            ' データ表示
            While sqlRead.Read()
                dgvResult.RowCount += 1

                ' --------------------------------
                ' コイル情報表示(子番以外)
                ' --------------------------------
                dgvResult(EM_RESULT_COL.nDATE, nCnt).Value = sqlRead.GetDateTime(sqlRead.GetOrdinal("登録日付")).ToString("yyyy/MM/dd HH:mm:ss")
                dgvResult(EM_RESULT_COL.nCoilNo, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("生産No")).ToString.Trim
                dgvResult(EM_RESULT_COL.nOrderNo, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("命令No"))

                Dim CoilL As Integer = CInt(sqlRead.GetSqlValue(sqlRead.GetOrdinal("コイル長")).ToString)
                Dim CoilF As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("カット範囲Ｆ"))).ToString)
                Dim CoilT As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("カット範囲Ｔ"))).ToString)
                Dim Kizu_o As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("表示疵個数表"))).ToString)
                Dim Kizu_u As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("表示疵個数裏"))).ToString)

                dgvResult(EM_RESULT_COL.nCoilLen, nCnt).Value = (CoilL / 1000).ToString("###0.0")       ' コイル長さ
                dgvResult(EM_RESULT_COL.nCutPos_F, nCnt).Value = (CoilF / 1000).ToString("###0.0")      ' カット範囲Ｆ
                dgvResult(EM_RESULT_COL.nCutPos_T, nCnt).Value = (CoilT / 1000).ToString("###0.0")      ' カット範囲Ｔ
                dgvResult(EM_RESULT_COL.nDspKizuCnt_O, nCnt).Value = Kizu_o             ' 表示疵個数表
                dgvResult(EM_RESULT_COL.nDspKizuCnt_U, nCnt).Value = Kizu_u             ' 表示疵個数裏

                ' 混入率
                dgvResult(EM_RESULT_COL.nMixedRate_O, nCnt).Value = ""
                dgvResult(EM_RESULT_COL.nMixedRate_U, nCnt).Value = ""
                If CoilL > 0 Then
                    dgvResult(EM_RESULT_COL.nMixedRate_O, nCnt).Value = ((Kizu_o * 1000 / CoilL) * 100).ToString("##0.00")
                    dgvResult(EM_RESULT_COL.nMixedRate_U, nCnt).Value = ((Kizu_u * 1000 / CoilL) * 100).ToString("##0.00")
                End If


                ' 疵管理番号
                dgvResult(EM_RESULT_COL.nKizKanri, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("管理No"))

                ' 入側子番
                dgvResult(EM_RESULT_COL.nKoban, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("入側子番"))

                nCnt += 1
            End While
            sqlRead.Close()
            sqlRead = Nothing


            If nCnt = 0 Then
                Exit Sub                             ' コイルがないときは、終了
            End If


        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "コイル情報検索エラー [" & ex.Message & "] SQL = " & strSql)
        Finally
            If Not sqlRead Is Nothing Then
                sqlRead.Close()
                sqlRead = Nothing
            End If

            If Not tcls_DB Is Nothing Then
                tcls_DB.Cancel()
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

        If 0 <> nCnt Then
            lblGuide.Text = CStr(nCnt) & "件見つかりました"
        Else
            lblGuide.Text = "データが見つかりませんでした。"
        End If

    End Sub



    ''' <summary>
    ''' コイル検索用SQL文作成
    ''' </summary>
    ''' <param name="strSql"></param>
    ''' <remarks></remarks>
    Private Sub CreateSql(ByRef strSql As String)

        Dim ii As Integer
        Dim strSqlFilter As String


        '疵個数フィルター用SQL取得
        strSqlFilter = ""

        '欠陥情報フィルター
        With dgvDispType
            For ii = 0 To .RowCount - 1
                If Not CType(dgvDispType(EM_FILTER_COL.nCheck, ii).Value, Boolean) Then
                    strSqlFilter &= " AND NOT 判定疵種 = " & dgvDispType(EM_FILTER_COL.nEdasysID, ii).Value.ToString
                End If
            Next ii
        End With

        'グレードフィルター
        With dgvDispGrade
            For ii = 0 To .RowCount - 1
                If Not CType(dgvDispGrade(EM_FILTER_COL.nCheck, ii).Value, Boolean) Then
                    strSqlFilter &= " AND NOT 判定グレード = " & dgvDispGrade(EM_FILTER_COL.nEdasysID, ii).Value.ToString
                End If
            Next ii
        End With

        strSql = ""
        strSql &= " SELECT A.登録日付 as 登録日付 "
        strSql &= "      , ISNULL(B.生産No,'') as 生産No "
        strSql &= "      , ISNULL(B.命令No,'') as 命令No "
        strSql &= "      , ISNULL(A.先頭位置, 0) as カット範囲Ｆ "
        strSql &= "      , ISNULL(A.保有長さ, 0) as コイル長 "
        strSql &= "      , ISNULL(A.先頭位置, 0) + ISNULL(A.保有長さ, 0) as カット範囲Ｔ "
        strSql &= "      , ISNULL(C.個数, 0) as 表示疵個数表 "
        strSql &= "      , ISNULL(D.個数, 0) as 表示疵個数裏 "
        strSql &= "      , ISNULL(A.管理No,'') as 管理No "
        strSql &= "      , ISNULL(A.ID,0) as CUTPOS_ID    "
        strSql &= "      , ISNULL(A.No,0) as CUTPOS_No    "
        strSql &= "      , ISNULL(A.入側子番,0) as 入側子番    "

        strSql &= " FROM  " & DB_COIL_CUT_POS & " A "
        strSql &= " LEFT OUTER JOIN " & DB_COIL_INF & " B "
        strSql &= " ON A.管理No = B.管理No "

        strSql &= " LEFT OUTER JOIN ( SELECT CA.ID, CA.No, count(*) as 個数 from "
        strSql &= "     " & DB_COIL_CUT_POS & " AS CA RIGHT OUTER JOIN " & DB_DEFECT_INF_T & " AS CB "
        strSql &= "     ON CA.管理No = CB.管理No "
        strSql &= "     AND ( CB.長手位置 >=  ISNULL(CA.先頭位置, 0) )"
        strSql &= "     AND ( CB.長手位置 <= (CAST(ISNULL(CA.先頭位置, 0) as bigint) + CAST(ISNULL(CA.保有長さ, 0) as bigint))  ) "

        strSql &= "     LEFT OUTER JOIN " & DB_COIL_RESULT & " AS CC "
        strSql &= "     ON CB.管理No = CC.管理No "

        strSql &= "     where CB.重複結果 <> 1 " & strSqlFilter
        strSql &= "     Group by CA.ID, CA.No ) C "
        strSql &= " ON A.ID = C.ID and A.No = C.No "

        strSql &= " LEFT OUTER JOIN ( SELECT DA.ID, DA.No, count(*) as 個数 from "
        strSql &= "     " & DB_COIL_CUT_POS & " AS DA RIGHT OUTER JOIN " & DB_DEFECT_INF_B & " AS DB "
        strSql &= "     ON DA.管理No = DB.管理No "
        strSql &= "     AND ( DB.長手位置 >=  ISNULL(DA.先頭位置, 0) )"
        strSql &= "     AND ( DB.長手位置 <= (CAST(ISNULL(DA.先頭位置, 0) as bigint) + CAST(ISNULL(DA.保有長さ, 0) as bigint))  ) "

        strSql &= "     LEFT OUTER JOIN " & DB_COIL_RESULT & " AS DC "
        strSql &= "     ON DB.管理No = DC.管理No "

        strSql &= "     where DB.重複結果 <> 1 " & strSqlFilter
        strSql &= "     Group by DA.ID, DA.No ) D "
        strSql &= " ON A.ID = D.ID and A.No = D.No "

        strSql &= " LEFT OUTER JOIN " & DB_COIL_RESULT & " E "
        strSql &= " ON A.管理No = E.管理No "

        strSql &= " WHERE 1 = 1"


        'コイルNo
        If "" <> txtCoil.Text.Trim Then
            strSql = strSql & " and B.生産No like '" & txtCoil.Text.Trim & "' "
        End If

        '保存ファイル名
        If "" <> txtSaveName.Text.Trim Then
            strSql = strSql & " and A.管理No like '" & txtSaveName.Text.Trim & "' "
        End If

        '日時（開始）
        If chkDateSet.Checked Then
            '日指定
            strSql = strSql & " and A.登録日付 >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
            '時指定
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeStart.Value, "HH:mm:ss") & "' ",
             strSql & "00:00:00' "))
        End If

        '日時（終了）
        If chkDateSet.Checked Then
            '日指定
            strSql = strSql & " and A.登録日付 <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
            '時指定
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' ",
             strSql & "23:59:59' "))
        End If

        '' オーダー句
        strSql = CStr(IIf(rdoSortAsc.Checked,
         strSql & " ORDER BY A.ID ASC, A.No ASC",
         strSql & " ORDER BY A.ID DESC, A.No DESC"))


    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検索結果 グリッド初期化
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検索結果 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)

        With dgv

            .ColumnCount = 0

            '' 動作定義
            .AutoGenerateColumns = False            ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False             ' ユーザー 行追加
            .AllowUserToDeleteRows = False          ' ユーザー 行削除
            .AllowUserToOrderColumns = False        ' ユーザー 列移動
            .AllowUserToResizeColumns = False       ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False          ' ユーザー 行サイズ変更
            .ShowCellErrors = False                 ' セルエラーを表示しない
            .ShowCellToolTips = False               ' ツールヒントを表示しない
            .ShowEditingIcon = False                ' 
            .ShowRowErrors = False                  '

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect    ' セル選択
            .MultiSelect = False                                        ' 複数行選択
            .ReadOnly = False                                           ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                ' セル書き換え

            .ScrollBars = ScrollBars.Both                                                           'スクロールバー
            .ColumnHeadersVisible = True                                                            ' 列ヘッダ表示
            .ColumnHeadersHeight = 45         ' 列ヘッダ高さ調整
            .RowHeadersVisible = False                                                              ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None                             ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None                                   ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .RowCount = 0

        End With

    End Sub



    ''' <summary>
    ''' 検索結果グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitCGL(ByVal dgv As DataGridView)

        With dgv
            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 通板日時
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 生産NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 命令No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 分割子番
            .Columns.Add(New DataGridViewTextBoxColumn)   ' カット範囲(Ｆ)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' カット範囲(Ｔ)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' コイル長
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 表示疵個数(表)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 表示疵個数(裏)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 混入率(表)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 混入率(裏)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ダミー


            '''' 列定義
            '通板日時
            .Columns(EM_RESULT_COL.nDATE).HeaderText = "通板日時" & vbCrLf
            .Columns(EM_RESULT_COL.nDATE).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDATE).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDATE).Width = 170
            .Columns(EM_RESULT_COL.nDATE).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nDATE).ReadOnly = True

            '生産NO
            .Columns(EM_RESULT_COL.nCoilNo).HeaderText = "生産No" & vbCrLf
            .Columns(EM_RESULT_COL.nCoilNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_COL.nCoilNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCoilNo).Width = 95
            .Columns(EM_RESULT_COL.nCoilNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCoilNo).ReadOnly = True
            .Columns(EM_RESULT_COL.nCoilNo).Frozen = True                                   ' 横スクロール時、表示列固定

            '命令No
            .Columns(EM_RESULT_COL.nOrderNo).HeaderText = "命令No" & vbCrLf
            .Columns(EM_RESULT_COL.nOrderNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nOrderNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nOrderNo).Width = 65
            .Columns(EM_RESULT_COL.nOrderNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nOrderNo).ReadOnly = True

            '分割子番
            .Columns(EM_RESULT_COL.nKoban).HeaderText = "分割" & vbCrLf & "子番"
            .Columns(EM_RESULT_COL.nKoban).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKoban).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKoban).Width = 50
            .Columns(EM_RESULT_COL.nKoban).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nKoban).ReadOnly = True

            'カット範囲(Ｆ)
            .Columns(EM_RESULT_COL.nCutPos_F).HeaderText = "カット" & vbCrLf & "範囲(Ｆ)"
            .Columns(EM_RESULT_COL.nCutPos_F).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nCutPos_F).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCutPos_F).Width = 80
            .Columns(EM_RESULT_COL.nCutPos_F).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCutPos_F).ReadOnly = True

            'カット範囲(Ｔ)
            .Columns(EM_RESULT_COL.nCutPos_T).HeaderText = "カット" & vbCrLf & "範囲(Ｔ)"
            .Columns(EM_RESULT_COL.nCutPos_T).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nCutPos_T).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCutPos_T).Width = 80
            .Columns(EM_RESULT_COL.nCutPos_T).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCutPos_T).ReadOnly = True

            'コイル長
            .Columns(EM_RESULT_COL.nCoilLen).HeaderText = "コイル長" & vbCrLf & "[m]"
            .Columns(EM_RESULT_COL.nCoilLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nCoilLen).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCoilLen).Width = 80
            .Columns(EM_RESULT_COL.nCoilLen).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCoilLen).ReadOnly = True

            '表示疵個数
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).HeaderText = "表示疵" & vbCrLf & "個数(表)"
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).Width = 80
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).ReadOnly = True

            '表示疵個数
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).HeaderText = "表示疵" & vbCrLf & "個数(裏)"
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).Width = 80
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).ReadOnly = True

            '混入率(表)
            .Columns(EM_RESULT_COL.nMixedRate_O).HeaderText = "混入率" & vbCrLf & "表[%]"
            .Columns(EM_RESULT_COL.nMixedRate_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nMixedRate_O).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nMixedRate_O).Width = 75
            .Columns(EM_RESULT_COL.nMixedRate_O).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nMixedRate_O).ReadOnly = True

            '混入率(裏)
            .Columns(EM_RESULT_COL.nMixedRate_U).HeaderText = "混入率" & vbCrLf & "裏[%]"
            .Columns(EM_RESULT_COL.nMixedRate_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nMixedRate_U).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nMixedRate_U).Width = 75
            .Columns(EM_RESULT_COL.nMixedRate_U).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nMixedRate_U).ReadOnly = True

            '管理No
            .Columns(EM_RESULT_COL.nKizKanri).HeaderText = "管理No"
            .Columns(EM_RESULT_COL.nKizKanri).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKizKanri).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKizKanri).Width = 160
            .Columns(EM_RESULT_COL.nKizKanri).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nKizKanri).ReadOnly = True

        End With

    End Sub



    ''' <summary>
    ''' CSVボタン押下処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCsv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsv.Click

        'CSV出力
        Call CsvOut(CSV_PATH, "出側混入率表示画面.csv")

    End Sub



    ''' <summary>
    ''' 検索中画面 更新
    ''' </summary>
    ''' <param name="nVal">進捗値</param>
    ''' <remarks></remarks>
    Private Sub DspWaitProg(ByVal nVal As Integer)

        '進捗値セット
        prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))

    End Sub


    ''' <summary>
    ''' 選択ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSelect.Click

    End Sub


    ''' <summary>
    ''' キャンセルボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCancel.Click

    End Sub

End Class

