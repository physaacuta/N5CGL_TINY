
'*********************************************************************************
' 疵個数発生トレンド画面
'	[Ver]
'		Ver.01    2012/10/03  疵個数発生トレンド画面追加対応(新規作成)
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports SO_GAMENN.frmDefIncidenceGraph

Public Class frmDefTrand

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const INI_TYPE_KEY As String = "KizuCountTrandType"      '疵種フィルターチェック保存用 KizuLocal.iniキー
    Private Const INI_GRADE_KEY As String = "KizuCountTrandGrade"    'グレードフィルターチェック保存用 KizuLocal.iniキー

    Private Const CSV_DEFCOUNT_FILENAME As String = "疵個数発生トレンド画面.csv" 'CSV保存ファイル名

    '疵種・グレードフィルター列名
    Public Enum EM_FILTER_COL                       ' 疵種
        emCheck = 0                                 ' チェックボックス
        emName                                      ' 名称
        emEdasysID                                  ' EdasysID (主キー)
        emMapName                                   ' 略称（疵種のみ）
    End Enum

    '検索結果 ヘッダ部
    Private Enum EM_RESULT_HEAD_COL
        nTsuNit = 0         '通販日時
        nCoilNo             'コイルNo
        nLen                '検査実測長
        nKizuKanNo          '管理No(非表示項目)
        nOrderNo            '命令No(非表示項目)
        nHinCode            '品種(非表示項目)
        nSyuZai             '主材質(非表示項目)
        nNextCode           '計画次工程(非表示項目)
        nKenKijun           '検査基準(表)(非表示項目)
        nCoilAtu            '材料板厚(非表示項目)
        nCoilWidth          '材料板幅(非表示項目)
        nCoilAtuSei         '注文板厚(非表示項目)
        nCoilWidSei         '注文板幅(非表示項目)
        nTotalO				'全体個数(表)
		nTotalU				'全体個数(裏)

		nDetalStart			'各疵個数開始
	End Enum

    ''' <summary>
    ''' 疵個数トレンド検索画面の戻り値
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum KIZU_TRAND_RETURN
        nShowKizuTrand = 0          '疵個数トレンド画面表示
        nCancel                     'キャンセル
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    '/ /////////////////////////////////////////////////////////////////////////////////

    '検索結果 ヘッダ部データ構造体
    Private Structure TINIY_DEFCOUNT_INF
		Dim strTourokuNit As String			'登録日時
		Dim strKanriNO As String			'管理No
		Dim strCoilNO As String				'コイルNo
		Dim dDefOCount As Double			'表疵個数
		Dim dDefUCount As Double			'裏疵個数
		Dim dDefACount As Double			'合計疵個数

		'イニシャライズ
		Public Sub initialize()
			strKanriNO = ""
			strCoilNO = ""
			dDefOCount = 0					'表疵個数
			dDefUCount = 0					'裏疵個数
			dDefACount = 0					'合計疵個数
		End Sub

	End Structure

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_strIniName As String = LOCALINI_NAME	'チェック情報を保存するINIファイル名
	Private m_proID As System.Diagnostics.Process = Nothing	'EXCEL用プロセスID
	Private m_dtSearchDateStr As Date	   '開始日
	Private m_dtSearchDateEnd As Date	   '終了日
	Private m_dtSearchTimeStr As Date	   '開始時間
	Private m_dtSearchTimeEnd As Date      '終了時間

    Private m_bSortFlg As Boolean          '表示順 True:昇順 False:降順

    Private m_typKizuTrandInf As TYP_KIZU_TRAND_INF '疵個数トレンドグラフ表示用データ
    Private m_nRetCode As KIZU_TRAND_RETURN '
    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
#Region "プロパティ"
    ''' <summary>
    ''' 疵個数トレンドグラフ表示用データ
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizuTrandInf() As TYP_KIZU_TRAND_INF
        Get
            Return m_typKizuTrandInf
        End Get
    End Property


    ''' <summary>
    ''' リターンコード
    ''' </summary>
    ''' <returns>操作結果</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ReturnCode() As KIZU_TRAND_RETURN
        Get
            Return m_nRetCode
        End Get
    End Property
#End Region


#Region "イベント"
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        '遠隔時
        If NOA_ONLINE <> g_NOA Then
            Me.tProp_Moveable = True                                                ' 画面移動許可
        End If

        '疵種・グレードフィルター初期化
        Call dgvInitialize_FilterGrid(dgvDispType)
        Call dgvInitColumn_FilterGrid(dgvDispType, "疵種名称")
        Call dgvFilter_AddHandler(dgvDispType)

        Call dgvInitialize_FilterGrid(dgvDispGrade)
        Call dgvInitColumn_FilterGrid(dgvDispGrade, "グレード名称")
        Call dgvFilter_AddHandler(dgvDispGrade)

        '疵種フィルター表示
        SetFilterData(dgvDispType, DB_TYPE_NAME, SO_GAMENN, INI_TYPE_KEY)

        'グレードフィルター表示
        SetFilterData(dgvDispGrade, DB_GRADE_NAME, SO_GAMENN, INI_GRADE_KEY)

        '検索結果初期化
        Call dgvInitialize_ResultGrid(dgvResult)
        Call dgvInitColumn_ResultGrid(dgvResult)

        '疵種全選択、全解除に該当グリッド情報をセット
        btnTypeAllSelect.Tag = dgvDispType
        btnTypeAllCancel.Tag = dgvDispType

        'グレード全選択、全解除に該当グリッド情報をセット
        btnGradeAllSelect.Tag = dgvDispGrade
        btnGradeAllCancel.Tag = dgvDispGrade

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnSearch, 5 - 1)
        AddFunctionButton(btnCsv, 6 - 1)
        AddFunctionButton(btnGraph, 7 - 1)
        AddFunctionButton(btnEnd, 12 - 1)

    End Sub

    ''' <summary>
    ''' フォームクローズイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefTrand_FormClosed(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles Me.FormClosed

        'フィルターグリッドのチェックをiniファイルに保存する
        Call SaveCheck()
    End Sub

    ''' <summary>
    ''' 閉じるボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        m_nRetCode = KIZU_TRAND_RETURN.nCancel
        Me.Hide()
    End Sub

    ''' <summary>
    ''' 検索ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '検索入力チェック
        If Not CheckSearch() Then
            '検索入力がされておらず、日付のチェックも入っていないときメッセージ
            lblGuide.Text = "検索キーを入力してください"
            Exit Sub
        End If

        'カーソルを処理中に変更
        Me.Cursor = Cursors.WaitCursor

        'コイル検索
        Call SearchCoil()

        'グリッド列表示切り替え
        Call ChangeDspCol()

        '開始日・終了日取得(EXCEL 範囲用)
        If chkDateSet.Checked = True Then
            m_dtSearchDateStr = dtpDateStart.Value
            m_dtSearchDateEnd = dtpDateEnd.Value
        Else
            m_dtSearchDateStr = Nothing
            m_dtSearchDateEnd = Nothing
        End If

        '開始時刻・終了時刻取得(EXCEL 範囲用)
        If chkTimeSet.Checked = True Then
            m_dtSearchTimeStr = dtpTimeStart.Value
            m_dtSearchTimeEnd = dtpTimeEnd.Value
        Else
            m_dtSearchTimeStr = Nothing
            m_dtSearchTimeEnd = Nothing
        End If

        '表示順
        If rdoSortAsc.Checked = True Then
            m_bSortFlg = True
        Else
            m_bSortFlg = False
        End If

        'カーソルを通常に戻す
        Me.Cursor = Cursors.Arrow
    End Sub

    ''' <summary>
    ''' グラフ出力ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnGraph_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim bRet As Boolean

        '疵個数トレンドグラフ画面用データ取得
        bRet = SetTrandGraphData()
        If bRet Then
            m_nRetCode = KIZU_TRAND_RETURN.nShowKizuTrand
            Me.Hide()
        End If

    End Sub

    ''' <summary>
    ''' CSV出力ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCsv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        'CSV出力
        Call CsvOut(CSV_PATH, CSV_DEFCOUNT_FILENAME)
    End Sub

    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' 日付フィルター 有効／無効設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkDateSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkDateSet.CheckedChanged

        ' コントロールの有効判定
        If chkDateSet.Checked = True Then
            dtpDateStart.Enabled = True
            dtpDateEnd.Enabled = True
        Else
            dtpDateStart.Enabled = False
            dtpDateEnd.Enabled = False
        End If

        '日付フィルターが無効なら、時刻フィルターも無効にする
        If Not chkDateSet.Checked Then If chkTimeSet.Checked = True Then chkTimeSet.Checked = False

    End Sub
    ''' <summary>
    ''' 時刻フィルター 有効／無効設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkTimeSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkTimeSet.CheckedChanged

        ' コントロールの有効判定
        If chkTimeSet.Checked = True Then
            dtpTimeStart.Enabled = True
            dtpTimeEnd.Enabled = True
        Else
            dtpTimeStart.Enabled = False
            dtpTimeEnd.Enabled = False
        End If

        '日付フィルターが有効なら、日付フィルターも有効にする
        If chkTimeSet.Checked Then If chkDateSet.Checked = False Then chkDateSet.Checked = True

    End Sub

    ''' <summary>
    ''' 全選択ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllSelect.Click, btnGradeAllSelect.Click

        'チェックボックスを全てON
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), True)

    End Sub

    ''' <summary>
    ''' 全解除ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

        'チェックボックスを全てOFF
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), False)

    End Sub
#End Region

    ''' <summary>
    ''' 検索条件チェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckSearch() As Boolean

        '検査条件が指定されていなければ、エラー
        If txtCoil.Text.Trim = "" And txtKanriNo.Text.Trim = "" And
           Not chkDateSet.Checked And Not chkTimeSet.Checked Then
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' 検索結果グリッド 列表示切替
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeDspCol()

        Dim nTypeCol As Integer     '疵種行カウント
        Dim nTypeLoop As Integer    'フィルターグリッドループ用

        nTypeCol = EM_RESULT_HEAD_COL.nDetalStart

        '表疵
        For nTypeLoop = 0 To dgvDispType.RowCount - 1
            'チェックボックスのチェック有無で表示切替
            If Not CType(dgvDispType(EM_FILTER_COL.emCheck, nTypeLoop).Value, Boolean) Then
                dgvResult.Columns(nTypeCol).Visible = False
            Else
                dgvResult.Columns(nTypeCol).Visible = True
            End If

            nTypeCol += 1
        Next

        '裏疵
        For nTypeLoop = 0 To dgvDispType.RowCount - 1
            'チェックボックスのチェック有無で表示切替
            If Not CType(dgvDispType(EM_FILTER_COL.emCheck, nTypeLoop).Value, Boolean) Then
                dgvResult.Columns(nTypeCol).Visible = False
            Else
                dgvResult.Columns(nTypeCol).Visible = True
            End If

            nTypeCol += 1
        Next

    End Sub

    ''' <summary>
    ''' コイル検索
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SearchCoil()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim Adapter As New SqlClient.SqlDataAdapter      ' アダプター

        Dim strSql As String = ""       '疵単位取得SQL文作成用
        Dim strGroupSql As String = ""  'グループ(コイル単位) 取得SQL作成用
        Dim nRowNo As Integer           'グループ行数

        Dim strKensaO As String = ""    '検査基準_表
        Dim strKensaU As String = ""    '検査基準_裏
        Dim strDspKensa As String = ""  '表示する検査基準

        Dim nGridCol As Integer         'グリッド列数
        Dim nTypeLoop As Integer        '疵種ループ用

        Dim dsData As New DataSet       '疵種単位データ保存用
        Dim dsGroupData As New DataSet  'コイル単位データ保存用

        Dim nWork As Integer            '文字列加工用 一時ワーク

        Dim drowList() As DataRow       'データロウ

        Dim nKizuCount As Integer = dgvDispType.RowCount        '疵種数

        'DBオープン
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_INF & "]")
            Exit Sub
        End If

        'グループSQL文作成
        Call CreGroupSql(strGroupSql)

        '疵種SQL文作成
        Call CreateSql(strSql)

        Try
            'グリッドを一旦初期化
            dgvResult.RowCount = 0

            'グループ用SQL実行
            Adapter = tcls_DB.SelectAdapter(strGroupSql)

            'グループ用データをデータセットに登録
            Adapter.Fill(dsGroupData)

            'SQL実行
            Adapter = tcls_DB.SelectAdapter(strSql)

            'データをデータセットに登録
            Adapter.Fill(dsData)

            'コイル単位でループ
            For nRowNo = 0 To dsGroupData.Tables(0).Rows.Count - 1
                Dim nCoilLen As Integer = CInt(dsGroupData.Tables(0).Rows(nRowNo).Item("検査実測長"))

                'グリッドを＋1行
                dgvResult.RowCount += 1

                '通販日時
                dgvResult(EM_RESULT_HEAD_COL.nTsuNit, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("登録日付").ToString

                'コイルNo
                dgvResult(EM_RESULT_HEAD_COL.nCoilNo, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("生産No").ToString

                '検査実測長
                dgvResult(EM_RESULT_HEAD_COL.nLen, nRowNo).Value = Format(CSng(nCoilLen / 1000), "#0.0")

                '管理No
                dgvResult(EM_RESULT_HEAD_COL.nKizuKanNo, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("管理No").ToString

                '命令No
                dgvResult(EM_RESULT_HEAD_COL.nOrderNo, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("命令No").ToString

                '品種
                dgvResult(EM_RESULT_HEAD_COL.nHinCode, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("品種").ToString

                '主材質
                dgvResult(EM_RESULT_HEAD_COL.nSyuZai, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("主材質").ToString

                '計画次工程
                dgvResult(EM_RESULT_HEAD_COL.nNextCode, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("計画次工程コード").ToString

                '検査基準(表)
                dgvResult(EM_RESULT_HEAD_COL.nKenKijun, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("検査基準_表").ToString

                '材料板厚
                dgvResult(EM_RESULT_HEAD_COL.nCoilAtu, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("材料板厚").ToString

                '材料板幅
                dgvResult(EM_RESULT_HEAD_COL.nCoilWidth, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("材料板幅").ToString

                '注文板厚
                dgvResult(EM_RESULT_HEAD_COL.nCoilAtuSei, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("注文板厚").ToString

                '注文板幅
                dgvResult(EM_RESULT_HEAD_COL.nCoilWidSei, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("注文板幅").ToString

                dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowNo).Value = "0"
                dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowNo).Value = "0"

                nGridCol = EM_RESULT_HEAD_COL.nDetalStart

                For nTypeLoop = 0 To dgvDispType.RowCount - 1

                    '疵種フィルターにチェックが入っている疵種のみ出力
                    If CType(dgvDispType(EM_FILTER_COL.emCheck, nTypeLoop).Value, Boolean) Then
                        drowList = dsData.Tables(0).Select(" 管理No = '" & dsGroupData.Tables(0).Rows(nRowNo).Item("管理No").ToString & "' " &
                            " AND 判定疵種 = '" & dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value.ToString & "' ")

                        '0で初期化
                        dgvResult(nGridCol, nRowNo).Value = "0"
                        dgvResult(nKizuCount + nGridCol, nRowNo).Value = "0"

                        '表裏区分分 ループ（最大2ループ）
                        For nListLoop As Integer = 0 To UBound(drowList)
                            '表裏区分の値でセットする行を変える
                            If drowList(nListLoop).Item("表裏区分").ToString = "0" Then
                                '表

                                '疵件数
                                nWork = CInt(drowList(nListLoop).Item("疵件数").ToString)
                                dgvResult(nGridCol, nRowNo).Value = Format(nWork, "#,##0")

                                '全体個数
                                nWork = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowNo).Value) +
                                  CInt(drowList(nListLoop).Item("疵件数").ToString)
                                dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowNo).Value = Format(nWork, "#,##0")

                            Else
                                '裏

                                '疵件数
                                nWork = CInt(drowList(nListLoop).Item("疵件数").ToString)
                                dgvResult(nKizuCount + nGridCol, nRowNo).Value = Format(nWork, "#,##0")

                                '全体個数
                                nWork = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowNo).Value) +
                                  CInt(drowList(nListLoop).Item("疵件数").ToString)
                                dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowNo).Value = Format(nWork, "#,##0")

                            End If
                        Next

                    End If

                    '列加算
                    nGridCol += 1
                Next

                nGridCol += nKizuCount

                '表保存疵個数
                nWork = CInt(dsGroupData.Tables(0).Rows(nRowNo).Item("表保存疵個数").ToString)
                dgvResult(nGridCol, nRowNo).Value = Format(nWork, "#,##0")
                nGridCol += 1

                '裏保存疵個数
                nWork = CInt(dsGroupData.Tables(0).Rows(nRowNo).Item("裏保存疵個数").ToString)
                dgvResult(nGridCol, nRowNo).Value = Format(nWork, "#,##0")
                nGridCol += 1

            Next

            '件数判定
            If 0 <> nRowNo Then
                '取得有り
                lblGuide.Text = CStr(nRowNo) & "件見つかりました"
            Else
                '取得無し
                lblGuide.Text = "データが見つかりませんでした。"
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "コイル情報検索エラー [" & ex.Message & "]")
        Finally

            'DBクラス 後処理
            If Not tcls_DB Is Nothing Then
                tcls_DB.Cancel()
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

    End Sub

    ''' <summary>
    ''' コイル検索用SQL文作成
    ''' </summary>
    ''' <param name="strSql"></param>
    ''' <remarks></remarks>
    Private Sub CreateSql(ByRef strSql As String)

        Dim nCnt As Integer         'グレードフィルターグリッド 行カウント
        Dim strWhereAnd As String   '条件文 接続文字列

        'SQL取得

        strSql = ""
        strSql &= " SELECT "
        strSql &= "    R.登録日付, "
        strSql &= "    C.生産No, "
        strSql &= "    D.管理No, "
        strSql &= "    D.判定疵種, "
        strSql &= "    D.表裏区分, "
        strSql &= "    D.疵件数 "
        strSql &= " FROM "

        '表疵
        strSql &= " ("
        strSql &= "   SELECT 管理No, "
        strSql &= "          判定疵種, "
        strSql &= "          0 AS 表裏区分, "
        strSql &= "          COUNT(1) 疵件数 "
        strSql &= "   FROM " & DB_DEFECT_INF_T
        strSql &= "   WHERE  重複結果 = 0 "

        'グレードフィルター
        With dgvDispGrade
            For nCnt = 0 To .RowCount - 1
                'グレードグリッドのチェックが入っていないグレードを除外
                If Not CType(dgvDispGrade(EM_FILTER_COL.emCheck, nCnt).Value, Boolean) Then
                    strSql &= " AND NOT 判定グレード = " & dgvDispGrade(EM_FILTER_COL.emEdasysID, nCnt).Value.ToString
                End If
            Next nCnt
        End With
        strSql &= "   GROUP BY 管理No, 判定疵種"
        strSql &= "   UNION ALL "

        '裏疵
        strSql &= "   SELECT 管理No, "
        strSql &= "          判定疵種, "
        strSql &= "          1 AS OU_KBN, "
        strSql &= "          COUNT(1) 疵件数 "
        strSql &= "   FROM " & DB_DEFECT_INF_B
        strSql &= "   WHERE  重複結果 = 0 "

        'グレードフィルター
        With dgvDispGrade
            For nCnt = 0 To .RowCount - 1
                'グレードグリッドのチェックが入っていないグレードを除外
                If Not CType(dgvDispGrade(EM_FILTER_COL.emCheck, nCnt).Value, Boolean) Then
                    strSql &= " AND NOT 判定グレード = " & dgvDispGrade(EM_FILTER_COL.emEdasysID, nCnt).Value.ToString
                End If
            Next nCnt
        End With

        strSql &= "   GROUP BY 管理No, 判定疵種"
        strSql &= " ) AS D "

        'コイル情報、コイル実績と結合
        strSql &= " INNER JOIN " & DB_COIL_INF & " AS C ON D.管理No = C.管理No "
        strSql &= " INNER JOIN " & DB_COIL_RESULT & " AS R ON D.管理No = R.管理No "

        strWhereAnd = " WHERE "

        '' 取得条件
        'コイルNo(板No)
        If "" <> txtCoil.Text.Trim Then
            strSql &= strWhereAnd & " C.生産No like '" & txtCoil.Text.Trim & "' "
            strWhereAnd = " AND "
        End If

        '管理No
        If "" <> txtKanriNo.Text.Trim Then
            strSql &= strWhereAnd & " D.管理No like '" & txtKanriNo.Text.Trim & "' "
            strWhereAnd = " AND "
        End If


        '日時（開始）
        If chkDateSet.Checked Then
            '日指定
            strSql &= strWhereAnd & " R.登録日付 >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '時指定
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeStart.Value, "HH:mm:ss") & "' ",
             strSql & "00:00:00' "))
        End If

        '日時（終了）
        If chkDateSet.Checked Then
            '日指定
            strSql &= strWhereAnd & " R.登録日付 <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '時指定
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' ",
             strSql & "23:59:59' "))
        End If

        '' オーダー句
        strSql = CStr(IIf(rdoSortAsc.Checked,
         strSql & " ORDER BY R.登録日付 ASC",
         strSql & " ORDER BY R.登録日付 DESC"))

        strSql &= " ,D.管理No ASC"

    End Sub

    ''' <summary>
    ''' コイル検索グループ(コイル単位)用SQL文作成
    ''' </summary>
    ''' <param name="strSql">SQL文</param>
    ''' <remarks></remarks>
    Private Sub CreGroupSql(ByRef strSql As String)

        Dim strWhereAnd As String

        'SQL作成

        strSql = ""
        strSql &= " SELECT "
        strSql &= "    ISNULL(R.登録日付,'') AS 登録日付 "
        strSql &= "    ,ISNULL(C.生産No, '') AS 生産No "
        strSql &= "    ,ISNULL(C.命令No, '') AS 命令No  "
        strSql &= "    ,ISNULL(R.管理No, '') AS 管理No  "
        strSql &= "    ,ISNULL(R.表保存疵個数, 0) AS 表保存疵個数  "
        strSql &= "    ,ISNULL(R.裏保存疵個数, 0) AS 裏保存疵個数  "
        strSql &= "    ,ISNULL(R.検査実測長,0) as 検査実測長 "
        strSql &= "    ,ISNULL(C.材料板厚,0) as 材料板厚 "
        strSql &= "    ,ISNULL(C.材料板幅,0) as 材料板幅 "
        strSql &= "    ,ISNULL(C.注文板厚,0) as 注文板厚 "
        strSql &= "    ,ISNULL(C.注文板幅,0) as 注文板幅 "
        strSql &= "    ,ISNULL(C.品種,'') as 品種 "
        strSql &= "    ,ISNULL(C.主材質,'') as 主材質 "
        strSql &= "    ,ISNULL(C.計画次工程コード,'') as 計画次工程コード "
        strSql &= "    ,ISNULL(C.検査基準_表,'') as 検査基準_表 "
        strSql &= " FROM " & DB_COIL_RESULT & " AS R "
        strSql &= " INNER JOIN " & DB_COIL_INF & " AS C ON R.管理No = C.管理No "

        strWhereAnd = " WHERE "

        'コイルNo(板No)
        If "" <> txtCoil.Text.Trim Then
            strSql &= strWhereAnd & " C.生産No like '" & txtCoil.Text.Trim & "' "
            strWhereAnd = " AND "
        End If

        '管理No
        If "" <> txtKanriNo.Text.Trim Then
            strSql &= strWhereAnd & " R.管理No like '" & txtKanriNo.Text.Trim & "' "
            strWhereAnd = " AND "
        End If


        '日時（開始）
        If chkDateSet.Checked Then
            '日指定
            strSql &= strWhereAnd & " R.登録日付 >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '時指定
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeStart.Value, "HH:mm:ss") & "' ",
             strSql & "00:00:00' "))
        End If

        '日時（終了）
        If chkDateSet.Checked Then
            '日指定
            strSql &= strWhereAnd & " R.登録日付 <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '時指定
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' ",
             strSql & "23:59:59' "))
        End If

        '' オーダー句
        strSql = CStr(IIf(rdoSortAsc.Checked,
         strSql & " ORDER BY R.登録日付 ASC",
         strSql & " ORDER BY R.登録日付 DESC"))

    End Sub

    ''' <summary>
    ''' 疵種・グレードフィルター初期表示
    ''' </summary>
    ''' <param name="dgv">データグリッド</param>
    ''' <param name="strTable">テーブル名</param>
    ''' <param name="strSection">セクション名</param>
    ''' <param name="strKey">キー</param>
    ''' <remarks></remarks>
    Private Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String,
                                ByVal strSection As String, ByVal strKey As String)

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)    'DBクラス
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""               'Sql文作成用
        Dim nCnt As Integer = 0                 'グリッド行
        Dim strSetString As String = ""

        'INIファイルの設定を取得
        strSetString = tClassLibrary.tMod.ReadIniStr(strSection, strKey, m_strIniName)

        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT Name, "
        strSQL &= "        EdasysID "
        '疵種のときはマップ名称を取得
        If INI_TYPE_KEY = strKey Then
            strSQL &= "    ,マップ表示名称 "
        End If
        strSQL &= " FROM " & strTable
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()

                dgv.RowCount += 1   '表示行数を設定

                ''設定不可 指定
                dgv(EM_FILTER_COL.emName, nCnt).ReadOnly = True

                'データセット
                dgv(EM_FILTER_COL.emName, nCnt).Value = sqlRead.GetString(0)
                dgv(EM_FILTER_COL.emName, nCnt).Style.WrapMode = DataGridViewTriState.False
                dgv(EM_FILTER_COL.emEdasysID, nCnt).Value = sqlRead.GetInt32(1)
                If INI_TYPE_KEY = strKey Then
                    dgv(EM_FILTER_COL.emMapName, nCnt).Value = sqlRead.GetString(2)
                End If

                '未登録または、1が設定されている場合は、チェックを入れる
                If "" = strSetString OrElse "1" = Mid(strSetString, nCnt + 1, 1) Then
                    dgv(EM_FILTER_COL.emCheck, nCnt).Value = True
                Else
                    dgv(EM_FILTER_COL.emCheck, nCnt).Value = False
                End If

                nCnt += 1

            End While

        Catch ex As Exception

        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()

            If Not tcls_DB Is Nothing Then
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

    End Sub

#Region "グリッド系"
    ''' <summary>
    ''' 疵種フィルター グリッド初期化
    ''' </summary>
    ''' <param name="dgv">フィルター用のデータグリッド</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize_FilterGrid(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)

        '''' 独自の項目を設定
        With dgv
            ' '' 定義
            '.ColumnHeadersHeight = 25										' ヘッダーの高さ
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 行選択
            .ScrollBars = ScrollBars.Vertical                               ' スクロールバーの表示

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 0
        End With
    End Sub

    ''' <summary>
    ''' 疵種フィルター グリッド列定義
    ''' </summary>
    ''' <param name="dgv">フィルター用のデータグリッド</param>
    ''' <param name="strName">2列目の項目名</param>
    ''' <remarks></remarks>
    Private Sub dgvInitColumn_FilterGrid(ByVal dgv As DataGridView, ByVal strName As String)
        With dgv
            ''''' 列追加
            .Columns.Add(New DataGridViewCheckBoxColumn)  ' チェックボックス
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' EdasysID
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 略称

            '''' 列定義
            '1列目:チェックボックス
            .Columns(EM_FILTER_COL.emCheck).HeaderText = ""
            .Columns(EM_FILTER_COL.emCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).Width = 20
            .Columns(EM_FILTER_COL.emCheck).SortMode = DataGridViewColumnSortMode.NotSortable

            '2列目:疵種名称
            .Columns(EM_FILTER_COL.emName).HeaderText = strName & vbCrLf
            .Columns(EM_FILTER_COL.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).Width = .ClientSize.Width - 20 - 3
            .Columns(EM_FILTER_COL.emName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:EdasysID
            .Columns(EM_FILTER_COL.emEdasysID).Visible = False  '非表示

            '4列目:略称
            .Columns(EM_FILTER_COL.emMapName).Visible = False  '非表示
        End With
    End Sub

    ''' <summary>
    ''' 検索結果グリッド 初期化
    ''' </summary>
    ''' <param name="dgv">検索結果グリッド</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize_ResultGrid(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)

        '''' 独自の項目を設定
        With dgv
            ' '' 定義
            .ColumnHeadersHeight = 40                                       ' ヘッダーの高さ
            '列ヘッダ中央寄せ
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 行選択
            .ScrollBars = ScrollBars.Both
            .ReadOnly = True

            '' 選択セル色定義                                                      'スクロールバー
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 0
        End With
    End Sub

    ''' <summary>
    ''' 検索結果グリッド 列定義
    ''' </summary>
    ''' <param name="dgv">検索結果グリッド</param>
    ''' <remarks></remarks>
    Private Sub dgvInitColumn_ResultGrid(ByVal dgv As DataGridView)

        Dim nTypeLoop As Integer = 0
        Dim nColNum As Integer

        With dgv

            '''' 列定義
            '1列目:通販日時
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 通板日時
            '.Columns(EM_RESULT_HEAD_COL.nTsuNit).HeaderText = "通販日時"
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).HeaderText = "通板日時"
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).Width = 180
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).Frozen = True

            '2列目:コイルNo
            .Columns.Add(New DataGridViewTextBoxColumn)   ' コイルNo
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).HeaderText = "生産No"
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).Width = 120
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).Frozen = True

            '3列目:検査実測長
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検査実測長
            .Columns(EM_RESULT_HEAD_COL.nLen).HeaderText = "コイル長" & vbCrLf & "[m]"
            .Columns(EM_RESULT_HEAD_COL.nLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nLen).Width = 80
            .Columns(EM_RESULT_HEAD_COL.nLen).SortMode = DataGridViewColumnSortMode.NotSortable

            '4列目:管理No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 管理No
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).Visible = False

            '5列目:命令No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 命令No
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).Visible = False

            '6列目:品種
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 品種
            .Columns(EM_RESULT_HEAD_COL.nHinCode).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nHinCode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nHinCode).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nHinCode).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nHinCode).Visible = False

            '7列目:主材質
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 主材質
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).Visible = False

            '8列目:計画次工程
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 計画次工程
            .Columns(EM_RESULT_HEAD_COL.nNextCode).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nNextCode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nNextCode).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nNextCode).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nNextCode).Visible = False

            '9列目:検査基準(表)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検査基準(表)
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).Visible = False

            '10列目:材料板厚
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 材料板厚
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).Visible = False

            '11列目:材料板幅
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 材料板幅
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).Visible = False

            '12列目:注文板厚
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 注文板厚
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).Visible = False

            '13列目:注文板幅
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 注文板幅
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).Visible = False

            '14列目:全体件数(表)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 全体件数(表)
            .Columns(EM_RESULT_HEAD_COL.nTotalO).HeaderText = "全体件数" & vbCrLf & "(表)"
            .Columns(EM_RESULT_HEAD_COL.nTotalO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nTotalO).Width = 80
            .Columns(EM_RESULT_HEAD_COL.nTotalO).SortMode = DataGridViewColumnSortMode.NotSortable

            '15列目:全体件数(裏)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 全体件数(裏)
            .Columns(EM_RESULT_HEAD_COL.nTotalU).HeaderText = "全体件数" & vbCrLf & "(裏)"
            .Columns(EM_RESULT_HEAD_COL.nTotalU).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nTotalU).Width = 80
            .Columns(EM_RESULT_HEAD_COL.nTotalU).SortMode = DataGridViewColumnSortMode.NotSortable

            nColNum += EM_RESULT_HEAD_COL.nDetalStart

            For nTypeLoop = 0 To dgvDispType.RowCount - 1
                '16列目以降～:疵種(表)
                .Columns.Add(New DataGridViewTextBoxColumn)
                .Columns(nColNum).HeaderText = dgvDispType(EM_FILTER_COL.emMapName, nTypeLoop).Value.ToString & vbCrLf &
                                               "(表)" & dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value.ToString
                .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(nColNum).Width = 78

                .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable

                nColNum += 1
            Next


            For nTypeLoop = 0 To dgvDispType.RowCount - 1
                '表＋16列目以降～:疵種(裏)
                .Columns.Add(New DataGridViewTextBoxColumn)
                .Columns(nColNum).HeaderText = dgvDispType(EM_FILTER_COL.emMapName, nTypeLoop).Value.ToString & vbCrLf &
                                               "(裏)" & dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value.ToString
                .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(nColNum).Width = 78
                .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable

                nColNum += 1
            Next

            '保存個数(表)
            .Columns.Add(New DataGridViewTextBoxColumn)
            .Columns(nColNum).HeaderText = "保存個数" & vbCrLf & "(表)"
            .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(nColNum).Width = 80
            .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable
            nColNum += 1

            '保存個数(裏)
            .Columns.Add(New DataGridViewTextBoxColumn)
            .Columns(nColNum).HeaderText = "保存個数" & vbCrLf & "(裏)"
            .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(nColNum).Width = 80
            .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable
            nColNum += 1

        End With
    End Sub
#End Region

#Region "疵個数トレンドグラフ画面表示系"
    ''' <summary>
    ''' 疵個数トレンドグラフ画面用データ設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Function SetTrandGraphData() As Boolean

        Dim typKizuInf As New TYP_KIZU_TRAND_INF

        ' 検索結果が無い
        If dgvResult.ColumnCount = 0 Then
            Return False
        End If

        'コイル未選択
        If 1 > dgvResult.SelectedRows.Count Then
            txtMsg.Text = "コイルを選択してください。"
            Return False
        End If

        'データ初期化
        typKizuInf.initialize()

        'データ取得
        If Not GetKizuTrandData(typKizuInf) Then
            txtMsg.Text = "疵個数トレンド情報の取得に失敗しました。"
            Return False
        End If

        '疵個数トレンド情報の設定
        m_typKizuTrandInf = typKizuInf

        Return True

    End Function

    ''' <summary>
    ''' 疵個数トレンド画面用データ取得
    ''' </summary>
    ''' <param name="typKizuInf">疵個数トレンド情報</param>
    ''' <remarks></remarks>
    Private Function GetKizuTrandData(ByRef typKizuInf As TYP_KIZU_TRAND_INF) As Boolean

        Try
            '検索条件取得
            typKizuInf.typSearch.nSelCoilIdx = dgvResult.SelectedRows.Item(0).Index

            '検索実施時の開始日付・終了日付取得
            typKizuInf.typSearch.strStartDate = IIf(Nothing = m_dtSearchDateStr, String.Empty, m_dtSearchDateStr.ToString("yyyy/MM/dd"))
            typKizuInf.typSearch.strEndDate = IIf(Nothing = m_dtSearchDateEnd, String.Empty, m_dtSearchDateEnd.ToString("yyyy/MM/dd"))
            typKizuInf.typSearch.strStartTime = IIf(Nothing = m_dtSearchTimeStr, String.Empty, m_dtSearchTimeStr.ToString("HH:mm:ss"))
            typKizuInf.typSearch.strEndTime = IIf(Nothing = m_dtSearchTimeEnd, String.Empty, m_dtSearchTimeEnd.ToString("HH:mm:ss"))

            'コイル情報取得
            Dim typData As TYP_KIZU_TRAND_COILINF = Nothing  '疵個数トレンド情報

            For nRowCnt As Integer = 0 To dgvResult.Rows.Count - 1
                '構造体初期化
                typData.initialize()

                ' コイル情報
                typData.strOrderNo = dgvResult(EM_RESULT_HEAD_COL.nOrderNo, nRowCnt).Value.ToString                 ' 命令No
                typData.strCoilNo = dgvResult(EM_RESULT_HEAD_COL.nCoilNo, nRowCnt).Value.ToString                   ' 生産No
                typData.strHinCode = dgvResult(EM_RESULT_HEAD_COL.nHinCode, nRowCnt).Value.ToString                 ' 品種
                typData.strSyuZai = dgvResult(EM_RESULT_HEAD_COL.nSyuZai, nRowCnt).Value.ToString                   ' 主材質
                typData.strNextCode = dgvResult(EM_RESULT_HEAD_COL.nNextCode, nRowCnt).Value.ToString               ' 計画次工程
                typData.strKenKijun = dgvResult(EM_RESULT_HEAD_COL.nKenKijun, nRowCnt).Value.ToString               ' 検査基準（表）
                typData.nCoilAtu = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilAtu, nRowCnt).Value)                      ' 材料板厚
                typData.nCoilWidth = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilWidth, nRowCnt).Value)                  ' 材料板幅
                typData.nCoilAtuSei = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilAtuSei, nRowCnt).Value)                ' 注文板厚
                typData.nCoilWidSei = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilWidSei, nRowCnt).Value)                ' 注文板幅
                typData.nOKizuCnt = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowCnt).Value)                      ' 表疵件数
                typData.nUKizuCnt = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowCnt).Value)                      ' 裏疵件数
                typData.nAKizuCnt = typData.nOKizuCnt + typData.nUKizuCnt                                           ' 合計疵件数

                ' 疵情報
                Dim dLen As Single = CSng(dgvResult(EM_RESULT_HEAD_COL.nLen, nRowCnt).Value.ToString)              ' 検査実測長
                Dim nCnt As Integer = EM_RESULT_HEAD_COL.nDetalStart
                Dim typKizu As TYP_KIZU_TRAND_KIZUINF = Nothing  '疵情報

                For nTypeLoop As Integer = 0 To dgvDispType.RowCount - 1
                    ' 疵情報の値を設定
                    typKizu.nEdasysID = CInt(dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value)
                    typKizu.nOKizuCnt = CInt(dgvResult(nCnt, nRowCnt).Value)
                    typKizu.nUKizuCnt = CInt(dgvResult(nCnt + dgvDispType.RowCount, nRowCnt).Value)
                    typKizu.nAKizuCnt = typKizu.nOKizuCnt + typKizu.nUKizuCnt

                    If 0 <> dLen Then
                        typKizu.dOKizuRate = (typKizu.nOKizuCnt / dLen) * 100
                        typKizu.dUKizuRate = (typKizu.nUKizuCnt / dLen) * 100
                        typKizu.dAKizuRate = (typKizu.nAKizuCnt / dLen) * 100
                    Else
                        typKizu.dOKizuRate = 0
                        typKizu.dUKizuRate = 0
                        typKizu.dAKizuRate = 0
                    End If

                    ' 未取得疵か
                    If False = typKizuInf.lstAllKizu.Contains(typKizu.nEdasysID) AndAlso 0 < typKizu.nAKizuCnt Then
                        typKizuInf.lstAllKizu.Add(typKizu.nEdasysID)    ' 疵一覧に疵を追加
                    End If

                    ' 疵個数最大値の更新
                    If typKizu.nOKizuCnt > typKizuInf.typSearch.nOKizuCntMax Then
                        typKizuInf.typSearch.nOKizuCntMax = typKizu.nOKizuCnt
                    End If
                    If typKizu.nUKizuCnt > typKizuInf.typSearch.nUKizuCntMax Then
                        typKizuInf.typSearch.nUKizuCntMax = typKizu.nUKizuCnt
                    End If
                    If typKizu.nAKizuCnt > typKizuInf.typSearch.nAKizuCntMax Then
                        typKizuInf.typSearch.nAKizuCntMax = typKizu.nAKizuCnt
                    End If

                    ' 疵混入率最大値の更新
                    If typKizu.dOKizuRate > typKizuInf.typSearch.dOKizuRateMax Then
                        typKizuInf.typSearch.dOKizuRateMax = typKizu.dOKizuRate
                    End If
                    If typKizu.dUKizuRate > typKizuInf.typSearch.dUKizuRateMax Then
                        typKizuInf.typSearch.dUKizuRateMax = typKizu.dUKizuRate
                    End If
                    If typKizu.dAKizuRate > typKizuInf.typSearch.dAKizuRateMax Then
                        typKizuInf.typSearch.dAKizuRateMax = typKizu.dAKizuRate
                    End If


                    ' 疵情報リスト追加
                    typData.lstKizuInf.Add(typKizu)

                    nCnt += 1
                Next

                ' コイル情報リスト追加
                typKizuInf.lstCoil.Add(typData)
            Next

            typKizuInf.lstAllKizu.Sort()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵個数トレンド情報取得エラー [" & ex.Message & "]")
            Return False
        End Try

    End Function

#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ フィルターグリッド 動作定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド動作 ハンドル割り当て
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvFilter_AddHandler(ByVal dgv As tCnt.tCnt_DataGridView)
        AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
        AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv.dgvData_CurrentCellDirtyStateChanged
    End Sub

    ''' <summary>
    ''' セルチェンジ処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, tCnt.tCnt_DataGridView)
        'ヘッダは処理しない
        If e.RowIndex = -1 Then Exit Sub

        If e.ColumnIndex = EM_FILTER_COL.emCheck Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '' チェックを付けた時
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
            Else
                '' チェックを外した時
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If

    End Sub

    ''' <summary>
    ''' グリッドのチェックボックスセット
    ''' </summary>
    ''' <param name="dgvData">データグリッド</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Private Sub CheckGrid(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' グリッドの全てのチェックボックスを設定
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_FILTER_COL.emCheck, ii).Value = bValue
        Next ii

    End Sub

    ''' <summary>
    ''' CSV出力
    ''' </summary>
    ''' <param name="strFolder">保存ファイルのフォルダパス</param>
    ''' <param name="strName">保存ファイルのファイル名</param>
    ''' <remarks></remarks>
    Private Sub CsvOut(ByVal strFolder As String, ByVal strName As String)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""                      '1行のデータ
        Dim strWk() As String
        Dim strWkData As String

        '' フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        '' フォルダの有無チェック(無かったら作る)
        tMod.CheckFolder(strFolder)

        '' CSV出力
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' オープン
                cls_Stram.Open()

                Dim ii As Integer
                Dim jj As Integer

                'ヘッダを出力
                strData = ""
                For ii = 0 To dgvResult.ColumnCount - 1

                    '隠し列は出力しないようにする
                    If dgvResult.Columns(ii).Visible = True Then
                        '改行コードをすべて取り除く
                        strWk = dgvResult.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
                        strWkData = ""
                        For jj = 0 To strWk.Length - 1
                            strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
                        Next jj

                        strData &= Trim(strWkData) & ","
                    End If

                Next ii

                '最後のタブを排除
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)

                'データ出力
                For ii = 0 To dgvResult.Rows.Count - 1
                    strData = ""
                    For jj = 0 To dgvResult.ColumnCount - 1

                        '隠し列は出力しないようにする
                        If dgvResult.Columns(jj).Visible = True Then

                            strWkData = ""
                            '数値のカンマをすべて取り除く
                            strWk = dgvResult(jj, ii).Value.ToString.Split(","c)
                            For nLoop As Integer = 0 To strWk.Length - 1
                                strWkData &= strWk(nLoop)
                            Next
                            '項目をカンマで連結する
                            strData &= strWkData & ","
                        End If

                    Next jj

                    '最後のカンマを排除
                    strData = strData.Remove(strData.Length - 1, 1)

                    cls_Stram.WriteLine(strData)
                Next ii

                ' クローズ
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "にCSVファイルを保存しました。"

            Catch ex As Exception
                strMsg = "CSV出力に失敗しました。"
            End Try
        End Using

        '''' ガイダンス表示
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using
    End Sub

    ''' <summary>
    ''' 疵種、グレードフィルターのチェック保存
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SaveCheck()

        Dim strSetString As String
        Dim ii As Integer

        '疵種,グレードフィルターの条件をINIファイルへ登録
        With dgvDispType
            strSetString = ""
            For ii = 0 To dgvDispType.RowCount - 1
                strSetString = CStr(IIf(CType(dgvDispType(EM_FILTER_COL.emCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
            Next ii
            tClassLibrary.tMod.WriteIni(SO_GAMENN, INI_TYPE_KEY, m_strIniName, strSetString)
        End With


        With dgvDispGrade
            strSetString = ""
            For ii = 0 To dgvDispGrade.RowCount - 1
                strSetString = CStr(IIf(CType(dgvDispGrade(EM_FILTER_COL.emCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
            Next ii
            tClassLibrary.tMod.WriteIni(SO_GAMENN, INI_GRADE_KEY, m_strIniName, strSetString)
        End With

    End Sub

    ''' <summary>
    ''' 文字バイト数取得関数
    ''' </summary>
    ''' <param name="strText">対象文字列</param>
    ''' <returns>文字列バイト数</returns>
    ''' <remarks></remarks>
    Private Function LenB(ByVal strText As String) As Integer
        Dim ByteArray As Byte() = System.Text.Encoding.GetEncoding("SJIS").GetBytes(strText)
        Return ByteArray.Length
    End Function

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
                Case 5 - 1                                                      ' 検索
                    btnSearch_Click(Nothing, Nothing)
                    '---------------------------------------------------------------
                Case 6 - 1                                                      ' CSV出力
                    btnCsv_Click(Nothing, Nothing)
                    '---------------------------------------------------------------
                Case 7 - 1                                                      ' グラフ出力
                    btnGraph_Click(Nothing, Nothing)
                    '---------------------------------------------------------------
                Case 12 - 1														' 終了
					btnEnd_Click(Nothing, Nothing)
			End Select
		Catch ex As Exception
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("ファンクションイベント異常 [{0}]", ex.Message))
		End Try
	End Sub

    '***************************************************************************
    ' マウスイベント
    '***************************************************************************
    ''' <summary>
    ''' 検索結果ダブルクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvResult_CellDoubleClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvResult.CellDoubleClick
        Try
            If e.RowIndex < 0 Then                                              ' 選択行が不正なら終わる
                Exit Sub
            End If

            btnGraph_Click(Nothing, Nothing)                                    ' グラフ出力ボタン押下時と同じ動作
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("検索結果ダブルクリックイベント異常 [{0}]", ex.Message))
        End Try
    End Sub
End Class
