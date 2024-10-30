'*********************************************************************************
' ＥＰＣ周期疵設定画面クラス
'	[Ver]
'		Ver.01    2012/10/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcCycle
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列（周期疵設定有無)
    Private Enum EM_DATACOLUMN_GRID_ID
        emTxt_No = 0                ' テキストボックス  No
        emTxt_Name                  ' コンボボックス    疵種
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		'emChk_Grade1                ' チェックボックス  グレード１
		'emChk_Grade2                ' チェックボックス  グレード２
		'emChk_Grade3                ' チェックボックス  グレード３
		'emChk_Grade4                ' チェックボックス  グレード４
		'emChk_Grade5                ' チェックボックス  グレード５
		'emChk_Grade6                ' チェックボックス  グレード６
		'emChk_Grade7                ' チェックボックス  グレード７
		'emChk_Grade8                ' チェックボックス  グレード８

		emChk_Grade                 ' チェックボックス  グレード８
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
    End Enum

    ' データグリッドビューの列（周期疵設値)
    Private Enum EM_PERIOD_GRID_COL_ID
        emTxt_Title = 0             ' テキストボックス  タイトル
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		'emTxt_Grade1                ' テキストボックス  グレード1
		'emTxt_Grade2                ' テキストボックス  グレード2
		'emTxt_Grade3                ' テキストボックス  グレード3
		'emTxt_Grade4                ' テキストボックス  グレード4
		'emTxt_Grade5                ' テキストボックス  グレード5
		'emTxt_Grade6                ' テキストボックス  グレード6
		'emTxt_Grade7                ' テキストボックス  グレード7
		'emTxt_Grade8                ' テキストボックス  グレード8 

		emTxt_Grade                 ' テキストボックス  グレード
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
    End Enum

    ' データグリッドビューの列（周期疵設値)
    Private Enum EM_PERIOD_GRID_ROW_ID
        emTxt_HistThr = 0           ' テキストボックス  疵間隔ヒストグラム閾値
        emTxt_Lpitch                ' テキストボックス  疵間隔計算ピッチ
        emTxt_Nthr                  ' テキストボックス  長手個数閾値
        emTxt_Wpitch                ' テキストボックス  幅分割ピッチ
        emTxt_Min                   ' テキストボックス  最小周期
        emTxt_Max                   ' テキストボックス  最大周期
        emTxt_KeepLen               ' テキストボックス  保持長さ
        emTxt_Weight                ' テキストボックス  歯抜け補正係数
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		emTxt_FrontEnableLen       'テキストボックス  F部有効区間
		emTxt_TailEnableLen        'テキストボックス  T部有効区間
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
    End Enum

	'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
	'Private Const mc_TypeMsc_ColMax As Integer = 8 + 2                ' 周期疵有無設定 最大列数 (8bit)
	'Private Const mc_Period_RowMax As Integer = 8                     ' 周期疵設定最大行数
	'Private Const mc_Period_ColMax As Integer = 8                     ' グレード最大位数 (8列固定)

	Private Const mc_TypeMsc_ColMax As Integer = MAX_CYCLE + 2        ' 周期疵有無設定 最大列数 (16bit)
	Private Const mc_Period_RowMax As Integer = 10                     ' 周期疵設定最大行数
	Private Const mc_Period_ColMax As Integer = MAX_CYCLE             ' グレード最大位数 (16列固定)
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

    ' 選択ボタンの定数
    Private Enum EM_SELECT_MODE
        emSelect = 0                     ' 全選択
        emCancel                         ' 全解除
    End Enum

    '周期dgv用の行タイトル設定、 表示が2行になるように改行してます
'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
    'Private m_strPeriodRowHead() As String = {"疵間隔ヒストグラム閾値" & vbNewLine & "(0 - 100) [%]",
    '   "疵間隔計算ピッチ" & vbNewLine & "(5 - 1000) [mm]",
    '   "長手個数閾値" & vbNewLine & "(0 - 1000) [個]",
    '   "幅分割ピッチ" & vbNewLine & "(1 - 500) [pixel]",
    '   "最小周期" & vbNewLine & "(0 - 2000) [mm]",
    '   "最大周期" & vbNewLine & "(0 - 10000000) [mm]",
    '   "保持長さ" & vbNewLine & "(0 - 10000) [m]",
    '   "歯抜け補正係数" & vbNewLine & "(0 - 1.0)"}

	Private m_strPeriodRowHead() As String = {"疵間隔ヒストグラム閾値" & vbNewLine & "(0 - 100) [%]",
	   "疵間隔計算ピッチ" & vbNewLine & "(5 - 1000) [mm]",
	   "長手個数閾値" & vbNewLine & "(0 - 1000) [個]",
	   "幅分割ピッチ" & vbNewLine & "(1 - 500) [pixel]",
	   "最小周期" & vbNewLine & "(0 - 10000000) [mm]",
	   "最大周期" & vbNewLine & "(0 - 10000000) [mm]",
	   "保持長さ" & vbNewLine & "(0 - 10000) [m]",
	   "歯抜け補正係数" & vbNewLine & "(0 - 1.0)",
	   "F部有効区間" & vbNewLine & "(0 - 20000)[m]",
	   "T部有効区間" & vbNewLine & "(0 - 20000)[m]"}
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

    ''ボタンdgv用の定数設定ボタン用のdgvは疵種dgvと列を揃える
    Private m_strBtnName() As String = {"全選択", "全解除"}

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter_TypeMsc As SqlClient.SqlDataAdapter = Nothing        ' アダプター
    Private m_Adapter_Period As SqlClient.SqlDataAdapter = Nothing         ' アダプター

    Private m_DS_TypeMsc As New DataSet                                    ' 非接続型テーブル
    Private m_DS_Period As New DataSet                                     ' 非接続型テーブル

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メニュー関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmEpcCycle_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
			Case 6								' F07 画面印刷
				Call PrintOut()
			Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "周期疵設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
				modMain.ChgProject()			' F09 KA_PARSETへ切替処理
            Case 10
                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' 設定
            Case 11
                modMain.TaskEnd()               ' F12 終了
        End Select

    End Sub

    ''' <summary>
    ''' 画面変更要求されたときの処理 (継承元クラスよりコールされる)
    ''' </summary>
    ''' <param name="frmID">選択画面ID</param>
    ''' <param name="cancel">選択を取りやめる場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmEpcCycle_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DS_TypeMsc.Dispose()
        m_DS_Period.Dispose()

        If Not m_Adapter_TypeMsc Is Nothing Then
            m_Adapter_TypeMsc.Dispose()
            m_Adapter_TypeMsc = Nothing
        End If

        If Not m_Adapter_Period Is Nothing Then
            m_Adapter_Period.Dispose()
            m_Adapter_Period = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcCycle_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcCycle              ' 周期疵設定画面呼出

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' 周期疵設定有無一覧 設定
        dgvInitialize_TypeMsc(dgvTypeMscData)

        ' 周期疵設定値一覧 設定
        dgvInitialize_Period(dgvPeriodData)

        ' 全選択ボタン
        dgvInitialize_SelectBtn(dgvTypeMscBtnAll, EM_SELECT_MODE.emSelect)

        ' 全選択解除
        dgvInitialize_SelectBtn(dgvTypeMscBtnClear, EM_SELECT_MODE.emCancel)

        DoubleBuffered = True

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        Else
            ' データ表示処理
            Data_Disp()
        End If

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvTypeMscData.DataGridViewSelectionRowColorChange(g_ColorSelection)
        dgvTypeMscData.DataGridViewSelectionRowColorChange(g_ColorSelection)

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

        frmMsg.Dispose()
		frmMsg = Nothing

		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>        
		' 縦スクロールバー表示有りならば、dgvの幅を広げる
		ChangeDgvWidth()
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
    End Sub

    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcCycle_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' このフォームで必要なデータを取得する
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' 周期疵設定有無用データ取得
        ' SQL文生成
        strSQL = "SELECT EdasysID"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", 優先度"
		strSQL &= ", PeriodProcMask"
		'strSQL &= ", ISNULL(PeriodProcMask,0) as PeriodProcMask"
		strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_TypeMsc, m_Adapter_TypeMsc, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' 周期疵設定値データ取得
        '// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		'strSQL = "SELECT [Key]"
		'      strSQL &= ", PeriodHistThr1, PeriodHistThr2"
		'      strSQL &= ", PeriodHistThr3, PeriodHistThr4"
		'      strSQL &= ", PeriodHistThr5, PeriodHistThr6"
		'      strSQL &= ", PeriodHistThr7, PeriodHistThr8"
		'      strSQL &= ", PeriodLpitch1, PeriodLpitch2"
		'      strSQL &= ", PeriodLpitch3, PeriodLpitch4"
		'      strSQL &= ", PeriodLpitch5, PeriodLpitch6"
		'      strSQL &= ", PeriodLpitch7, PeriodLpitch8"
		'      strSQL &= ", PeriodNthr1, PeriodNthr2"
		'      strSQL &= ", PeriodNthr3, PeriodNthr4"
		'      strSQL &= ", PeriodNthr5, PeriodNthr6"
		'      strSQL &= ", PeriodNthr7, PeriodNthr8"
		'      strSQL &= ", PeriodWpitch1, PeriodWpitch2"
		'      strSQL &= ", PeriodWpitch3, PeriodWpitch4"
		'      strSQL &= ", PeriodWpitch5, PeriodWpitch6"
		'      strSQL &= ", PeriodWpitch7, PeriodWpitch8"
		'      strSQL &= ", PeriodMin1, PeriodMin2"
		'      strSQL &= ", PeriodMin3, PeriodMin4"
		'      strSQL &= ", PeriodMin5, PeriodMin6"
		'      strSQL &= ", PeriodMin7, PeriodMin8"
		'      strSQL &= ", PeriodMax1, PeriodMax2"
		'      strSQL &= ", PeriodMax3, PeriodMax4"
		'      strSQL &= ", PeriodMax5, PeriodMax6"
		'      strSQL &= ", PeriodMax7, PeriodMax8"
		'      strSQL &= ", PeriodKeepLen1, PeriodKeepLen2"
		'      strSQL &= ", PeriodKeepLen3, PeriodKeepLen4"
		'      strSQL &= ", PeriodKeepLen5, PeriodKeepLen6"
		'      strSQL &= ", PeriodKeepLen7, PeriodKeepLen8"
		'      strSQL &= ", PeriodWeight1, PeriodWeight2"
		'      strSQL &= ", PeriodWeight3, PeriodWeight4"
		'      strSQL &= ", PeriodWeight5, PeriodWeight6"
		'      strSQL &= ", PeriodWeight7, PeriodWeight8"
		'      strSQL &= " FROM " & DB_PARAM_COMMON
		'      strSQL &= " WHERE [Key] = " & CStr(PARAM_COMMON_KEY)

		strSQL = " SELECT "
		strSQL &= "     id "
		strSQL &= "    ,PeriodHistThr "
		strSQL &= "    ,PeriodLpitch "
		strSQL &= "    ,PeriodNthr "
		strSQL &= "    ,PeriodWpitch "
		strSQL &= "    ,PeriodMin "
		strSQL &= "    ,PeriodMax "
		strSQL &= "    ,PeriodKeepLen "
		strSQL &= "    ,PeriodWeight "
		strSQL &= "    ,PeriodFrontEnableLen "
		strSQL &= "    ,PeriodTailEnableLen "
		strSQL &= " FROM " & DB_PARAM_CYCLE
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Period, m_Adapter_Period, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_COMMON & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' 取得データを画面に表示する。
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        ' 全選択、全解除ボタン設定
        Call Disp_SelectBtn(dgvTypeMscBtnAll)
        Call Disp_SelectBtn(dgvTypeMscBtnClear)

        ' 周期疵設定有無用データ表示
        Call Db_Disp_TypeMsc()

        ' 周期疵設定値データ表示
        Call Db_Disp_Period()

    End Sub

    ''' <summary>
    ''' 疵種データdgvのチェックボックスのチェックを変更した時　色を変える
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTypeMscData.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        'タイトルより右の列 ヘッダー以外
        If e.ColumnIndex > EM_DATACOLUMN_GRID_ID.emTxt_Name And e.RowIndex >= 0 Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                'チェックを付けた時
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = g_ColorSetting
            Else
                'チェックを外した時
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Empty
            End If
        End If

    End Sub

    ''' <summary>
    ''' 全選択dgvボタン押下時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscBtnAll_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTypeMscBtnAll.CellContentClick

        'ボタン列でなければなにもしない
        If e.ColumnIndex <= 0 Then Return

        '列のチェック状態を全選択にする
        ChangeTypeMscCheckBox(dgvTypeMscData, e.ColumnIndex, True)

    End Sub

    ''' <summary>
    ''' 全解除dgvボタン押下時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscBtnClear_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTypeMscBtnClear.CellContentClick

        'ボタン列でなければなにもしない
        If e.ColumnIndex <= 0 Then Return

        '列のチェック状態を全解除にする
        ChangeTypeMscCheckBox(dgvTypeMscData, e.ColumnIndex, False)

    End Sub

    ''' <summary>
    ''' 対象dgvの対象列のチェックボックスのチェック状態を変更する
    ''' </summary>
    ''' <param name="dgv">対象のdgv</param>
    ''' <param name="nIndex">列インデックス</param>
    ''' <param name="bChecked">チェック状態</param>
    ''' <remarks></remarks>
    Private Sub ChangeTypeMscCheckBox(ByVal dgv As DataGridView, ByVal nIndex As Integer, ByVal bChecked As Boolean)
        Dim ii As Integer   'カウンタ

        '全行のチェック状態を変更
        For ii = 0 To dgv.RowCount - 1
            dgv(nIndex, ii).Value = bChecked
        Next ii
    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check() Then Exit Sub

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '画面設定値を読み込む
            Data_Get()

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
                'DBデータ再読込
                Data_Disp()
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 設定ボタン押下後の 入力チェック、DB更新処理関連 DB更新部分を記入すること
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String
        Dim iColIx As Integer                                   ' 列INDEX

        ' 周期疵有無設定
        ' チェックボックスのみなので特にチェックはしない

        ' 周期疵設定値
        ' 列数分ループ
        For ii As Integer = 1 To dgvPeriodData.ColumnCount - 1

            iColIx = ii

            ' 疵間隔ヒストグラム閾値
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 100, False, "疵間隔ヒストグラム閾値") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value = strWk

            ' 疵間隔計算ピッチ
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 5, 1000, False, 10, "疵間隔計算ピッチ", True) <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value = strWk

            ' 長手個数閾値
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 1000, False, "長手個数閾値") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value = strWk

            ' 幅分割ピッチ
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 500, False, "幅分割ピッチ") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value = strWk

            ' 最小周期
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value)
            '// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 2000, False, 10, "最小周期", True) <> 0 Then Return False
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 10000000, False, 10, "最小周期", True) <> 0 Then Return False
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value = strWk

            ' 最大周期
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value)
            'If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 5000, False, 10, "最大周期", True) <> 0 Then Return False
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 10000000, False, 10, "最大周期", True) <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value = strWk

            ' 保持長さ
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 10000, False, "保持長さ") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value = strWk

            ' 歯抜け補正係数
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 1.0, False, 10, "歯抜け補正係数", True) <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value = strWk

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			' F部有効区間
			strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 20000, False, "F部有効区間") <> 0 Then Return False
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value = strWk

			' T部有効区間
			strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 20000, False, "T部有効区間") <> 0 Then Return False
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value = strWk
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        Next ii

        Return True
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 画面に表示中のデータ取得
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()

        Data_Get_TypeMsc()                  '周期疵設定有無 データ設定
        Data_Get_Period()                   '周期疵設定値   データ設定

    End Sub

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット 周期疵設定有無
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get_TypeMsc()
        Dim strWhere As String                          ' WHERE句に相当の条件生成
        Dim drow As DataRow
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>> 
		'Dim BitFlag(0 To 7) As Integer                  ' BIT変換後の配列
		'Dim bFlag As Byte

		Dim BitFlag(0 To MAX_CYCLE - 1) As Integer                  ' BIT変換後の配列
		Dim nFlag As Integer
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        Dim iStartIx As Integer

        ' 条件No分ループ
        For ii As Integer = 0 To dgvTypeMscData.Rows.Count - 1
            strWhere = String.Format("ID='{0}' ", ii)                     ' WHERE句に相当の条件生成
            drow = m_DS_TypeMsc.Tables(0).Select(strWhere)(0)             ' IDから更新用データ取得

            '''' 条件チェック後セット

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'bFlag = 0
			'         iStartIx = EM_DATACOLUMN_GRID_ID.emChk_Grade1
			'         For jj As Integer = 0 To 7
			nFlag = 0
			iStartIx = EM_DATACOLUMN_GRID_ID.emChk_Grade
			For jj As Integer = 0 To MAX_CYCLE - 1
				'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

                If CInt(dgvTypeMscData(iStartIx, ii).Value) = 0 Then
                    BitFlag(jj) = 0      '無効
                Else
                    BitFlag(jj) = 1      '有効
                End If
                iStartIx = iStartIx + 1
            Next jj

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>> 
			'' 設定フラグを確認して、ビット変換を行う。
			'IntArreyToBit(bFlag, UBound(BitFlag), BitFlag)
			'
			'' セット
			'drow.Item("PeriodProcMask") = bFlag

            ' 設定フラグを確認して、ビット変換を行う。
			IntArrayToBit(nFlag, UBound(BitFlag), BitFlag)

            ' セット
			drow.Item("PeriodProcMask") = nFlag
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        Next ii
    End Sub

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット 周期疵設定値
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get_Period()
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>> 
		'Dim drow As DataRow
		'Dim strWhere As String                              ' WHERE句に相当の条件生成
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        Dim sDbItemName As String                           ' DB項目名称
        Dim iColIx As Integer                               ' 列INDEX

		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		'strWhere = String.Format("[Key]='{0}' ", 0)         ' WHERE句に相当の条件生成
		'drow = m_DS_Period.Tables(0).Select(strWhere)(0)    ' KEYから更新用データ取得
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

        For ii As Integer = 1 To mc_Period_ColMax
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			Dim strWhere As String = String.Format("[ID]='{0}' ", ii)       ' WHERE句に相当の条件生成
			Dim drow As DataRow = m_DS_Period.Tables(0).Select(strWhere)(0) ' PCommonテーブルを参照するので1行しかないはず
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

            iColIx = ii

            ' 設定値 疵間隔ヒストグラム閾値
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodHistThr" & iColIx
			sDbItemName = "PeriodHistThr"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------            
			drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value)           'PeriodHistThr

            ' 設定値 疵間隔計算ピッチ
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodLpitch" & iColIx
			sDbItemName = "PeriodLpitch"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value)           'PeriodLpitch

            ' 設定値 長手個数閾値
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodNthr" & iColIx
			sDbItemName = "PeriodNthr"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value)              'PeriodNthr

            ' 設定値 幅分割ピッチ
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodWpitch" & iColIx
			sDbItemName = "PeriodWpitch"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value)            'PeriodWpitch

            ' 設定値 最小周期
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodMin" & iColIx
			sDbItemName = "PeriodMin"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value)              'PeriodMin

            ' 設定値 最大周期
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodMax" & iColIx
			sDbItemName = "PeriodMax"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value)               'PeriodMax

            ' 設定値 保持長さ
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodKeepLen" & iColIx
			sDbItemName = "PeriodKeepLen"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value)           'PeriodKeepLen

            ' 設定値 歯抜け補正係数
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodWeight" & iColIx
			sDbItemName = "PeriodWeight"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value)            'PeriodWeight

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			' 設定値 F部有効区間
			sDbItemName = "PeriodFrontEnableLen"
			drow.Item(sDbItemName) = dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value    'PeriodFrontEnableLen

			' 設定値 T部有効区間
			sDbItemName = "PeriodTailEnableLen"
			drow.Item(sDbItemName) = dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value     'PeriodTailEnableLen
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        Next ii
    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transactTypeMsc As SqlClient.SqlTransaction = Nothing   'トランザクション処理
        Dim transactPeriod As SqlClient.SqlTransaction = Nothing    'トランザクション処理

        Try
            ' DB接続
            m_Adapter_TypeMsc.SelectCommand.Connection.Open()
            m_Adapter_Period.SelectCommand.Connection.Open()

            'トランザクション開始
            transactTypeMsc = m_Adapter_TypeMsc.SelectCommand.Connection.BeginTransaction()
            m_Adapter_TypeMsc.SelectCommand.Transaction = transactTypeMsc
            transactPeriod = m_Adapter_Period.SelectCommand.Connection.BeginTransaction()
            m_Adapter_Period.SelectCommand.Transaction = transactPeriod

            Using cbWk As New SqlClient.SqlCommandBuilder()

                'TypeNameのアダプタをセットして更新
                cbWk.DataAdapter = m_Adapter_TypeMsc
                m_Adapter_TypeMsc.Update(m_DS_TypeMsc)

                'ParamCommonのアダプタをセットして更新
                cbWk.DataAdapter = m_Adapter_Period
                m_Adapter_Period.Update(m_DS_Period)

            End Using

            'コミット
            transactTypeMsc.Commit()
            transactPeriod.Commit()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_CYCLE)                             '統括への通知を変更する事
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)

            '' エラーならロールバックして終了
            If Not transactTypeMsc Is Nothing Then
                transactTypeMsc.Rollback()
            End If

            If Not transactPeriod Is Nothing Then
                transactPeriod.Rollback()
            End If

            Return False
        Finally
            ' 後始末
            If m_Adapter_TypeMsc.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter_TypeMsc.SelectCommand.Connection.Close()
            End If

            If m_Adapter_Period.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter_Period.SelectCommand.Connection.Close()
            End If

            If Not transactTypeMsc Is Nothing Then
                transactTypeMsc.Dispose()
            End If

            If Not transactPeriod Is Nothing Then
                transactPeriod.Dispose()
            End If
        End Try
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他の関数
    '/ /////////////////////////////////////////////////////////////////////////////////
	'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
	'''' <summary>
	'''' BIT変換処理 BIT -> INT型の配列
	'''' </summary>
	'''' <param name="bFlag">BITデータ</param>
	'''' <param name="IntFlag">INTの配列データ</param>
	'''' <remarks></remarks>
	'Private Sub BitToIntArrey(ByVal bFlag As Byte, ByVal IntFlag() As Integer)
	'
	'
	'       If (bFlag And &H1) <> &H0 Then IntFlag(0) = 1
	'       If (bFlag And &H2) <> &H0 Then IntFlag(1) = 1
	'       If (bFlag And &H4) <> &H0 Then IntFlag(2) = 1
	'       If (bFlag And &H8) <> &H0 Then IntFlag(3) = 1
	'
	'       If (bFlag And &H10) <> &H0 Then IntFlag(4) = 1
	'       If (bFlag And &H20) <> &H0 Then IntFlag(5) = 1
	'       If (bFlag And &H40) <> &H0 Then IntFlag(6) = 1
	'       If (bFlag And &H80) <> &H0 Then IntFlag(7) = 1
	'
	'   End Sub
	'
	'   ''' <summary>
	'   ''' BIT変換処理 INT型の配列 -> BIT
	'   ''' </summary>
	'   ''' <param name="bFlag">BITデータ</param>
	'   ''' <param name="iCnt">配列数</param>
	'   ''' <param name="IntFlag">INTの配列データ</param>
	'   ''' <remarks></remarks>
	'   Private Sub IntArreyToBit(ByRef bFlag As Byte, ByVal iCnt As Integer, ByVal IntFlag() As Integer)
	'
	'       Dim bWork As UInteger
	'
	'       bWork = 0
	'       For ii As Integer = 0 To iCnt
	'           If (IntFlag(ii) = 1) Then bWork = bWork + CUInt(2 ^ ii) '
	'       Next ii
	'
	'       bFlag = CByte(bWork)
	'
	'   End Sub

    ''' <summary>
    ''' BIT変換処理 BIT -> INT型の配列
    ''' </summary>
	''' <param name="nFlag">BITデータ(32bit)</param>
	''' <param name="nIntFlag">INTの配列データ</param>
    ''' <remarks></remarks>
	Private Sub BitToIntArray(ByVal nFlag As Int32, ByVal nIntFlag() As Integer)
		If (nFlag And &H1) <> &H0 Then nIntFlag(0) = 1
		If (nFlag And &H2) <> &H0 Then nIntFlag(1) = 1
		If (nFlag And &H4) <> &H0 Then nIntFlag(2) = 1
		If (nFlag And &H8) <> &H0 Then nIntFlag(3) = 1

		If (nFlag And &H10) <> &H0 Then nIntFlag(4) = 1
		If (nFlag And &H20) <> &H0 Then nIntFlag(5) = 1
		If (nFlag And &H40) <> &H0 Then nIntFlag(6) = 1
		If (nFlag And &H80) <> &H0 Then nIntFlag(7) = 1

		If (nFlag And &H100) <> &H0 Then nIntFlag(8) = 1
		If (nFlag And &H200) <> &H0 Then nIntFlag(9) = 1
		If (nFlag And &H400) <> &H0 Then nIntFlag(10) = 1
		If (nFlag And &H800) <> &H0 Then nIntFlag(11) = 1

		If (nFlag And &H1000) <> &H0 Then nIntFlag(12) = 1
		If (nFlag And &H2000) <> &H0 Then nIntFlag(13) = 1
		If (nFlag And &H4000) <> &H0 Then nIntFlag(14) = 1
		If (nFlag And &H8000) <> &H0 Then nIntFlag(15) = 1
    End Sub

    ''' <summary>
    ''' BIT変換処理 INT型の配列 -> BIT
    ''' </summary>
	''' <param name="nFlag">BITデータ(32bit)</param>
	''' <param name="nCnt">配列数</param>
	''' <param name="nIntFlag">INTの配列データ</param>
    ''' <remarks></remarks>
	Private Sub IntArrayToBit(ByRef nFlag As Int32, ByVal nCnt As Integer, ByVal nIntFlag() As Integer)
		nFlag = 0
		For ii As Integer = 0 To nCnt
			If (nIntFlag(ii) = 1) Then nFlag = nFlag + CInt(2 ^ ii) '
        Next ii
    End Sub
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 画面表示処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' DBデータを表示する 周期疵設定有無
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Db_Disp_TypeMsc()

        Dim drow() As DataRow = m_DS_TypeMsc.Tables(0).Select()
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
		'Dim iMarkFlag(0 To 7) As Integer                       '周期疵設定 BIT変換後の値
		'Dim bFlags As Byte
		Dim iMarkFlag(0 To MAX_CYCLE - 1) As Integer                 '周期疵設定 BIT変換後の値        
		Dim nFlags As Integer
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

        ' 画面に表示
        dgvTypeMscData.RowCount = drow.Length                  ' 基本的にマスターには正常なレコードを登録済みのはずだが、汎用的にする為
        For ii As Integer = 0 To drow.Length - 1
            dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emTxt_No, ii).Value = CInt(drow(ii).Item("ID")) + 1    ' IDは0からなので1加算する
            dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emTxt_Name, ii).Value = drow(ii).Item("Name")          ' 疵種

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'For jj As Integer = 0 To 7
			For jj As Integer = 0 To MAX_CYCLE - 1
				'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

                iMarkFlag(jj) = 0
            Next jj
            ' 周期疵設定 （BIT）で設定されているので変換してやること
			' BIT変換処理を作成すること
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'If drow(ii).Item("PeriodProcMask") Is DBNull.Value Then
			'	bFlags = 0		'疵種
			'Else
			'	bFlags = CByte(drow(ii).Item("PeriodProcMask"))		'疵種
			'End If
			'
			'BitToIntArrey(bFlags, iMarkFlag)

			If drow(ii).Item("PeriodProcMask") Is DBNull.Value Then
				nFlags = 0      '疵種
			Else
				nFlags = CInt(drow(ii).Item("PeriodProcMask"))     '疵種
			End If

			BitToIntArray(nFlags, iMarkFlag)
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

			'チェックボックス設定
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade1, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade1 - 2)			' チェックボックス  グレード１
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade2, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade2 - 2)			' チェックボックス  グレード２
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade3, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade3 - 2)			' チェックボックス  グレード３
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade4, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade4 - 2)			' チェックボックス  グレード４
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade5, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade5 - 2)			' チェックボックス  グレード５
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade6, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade6 - 2)			' チェックボックス  グレード６
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade7, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade7 - 2)			' チェックボックス  グレード７
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade8, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade8 - 2)            ' チェックボックス  グレード８

			For kk As Integer = 0 To MAX_CYCLE - 1
				Dim nColIndex As Integer = EM_DATACOLUMN_GRID_ID.emChk_Grade + kk
				Dim nFlagIndex As Integer = EM_DATACOLUMN_GRID_ID.emChk_Grade + kk - 2

				dgvTypeMscData(nColIndex, ii).Value = iMarkFlag(nFlagIndex)           ' チェックボックス  グレード
			Next
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
		Next ii
    End Sub

    ''' <summary>
    ''' DBデータを表示する 周期疵設定値
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Db_Disp_Period()
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		'Dim strWhere As String = String.Format("[Key]='{0}' ", 0)           ' WHERE句に相当の条件生成
		'Dim drow() As DataRow = m_DS_Period.Tables(0).Select(strWhere)      ' PCommonテーブルを参照するので1行しかないはず
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        Dim sDbItemName As String                                           ' DB項目名称
        Dim iColIx As Integer                                               ' 列INDEX

        ' 画面に表示
        dgvPeriodData.RowCount = mc_Period_RowMax                           ' 行数設定

        ' 行タイトル設定
        For ii As Integer = 0 To mc_Period_RowMax - 1
            dgvPeriodData(EM_PERIOD_GRID_COL_ID.emTxt_Title, ii).Value = m_strPeriodRowHead(ii)
        Next ii

        For ii As Integer = 0 To mc_Period_ColMax - 1
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			Dim strWhere As String = String.Format("[ID]='{0}' ", ii + 1)      ' WHERE句に相当の条件生成
			Dim drow() As DataRow = m_DS_Period.Tables(0).Select(strWhere)  ' PCommonテーブルを参照するので1行しかないはず

			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

            iColIx = ii + 1
            ' 設定値 疵間隔ヒストグラム閾値
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodHistThr" & iColIx
			sDbItemName = "PeriodHistThr"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value = drow(0).Item(sDbItemName)            'PeriodHistThr

            ' 設定値 疵間隔計算ピッチ
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodLpitch" & iColIx
			sDbItemName = "PeriodLpitch"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value = drow(0).Item(sDbItemName)             'PeriodLpitch


            ' 設定値 長手個数閾値
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodNthr" & iColIx
			sDbItemName = "PeriodNthr"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value = drow(0).Item(sDbItemName)               'PeriodNthr

            ' 設定値 幅分割ピッチ
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodWpitch" & iColIx
			sDbItemName = "PeriodWpitch"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value = drow(0).Item(sDbItemName)             'PeriodWpitch

            ' 設定値 最小周期
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodMin" & iColIx
			sDbItemName = "PeriodMin"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value = drow(0).Item(sDbItemName)                'PeriodMin

            ' 設定値 最大周期
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodMax" & iColIx
			sDbItemName = "PeriodMax"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value = drow(0).Item(sDbItemName)                'PeriodMax


            ' 設定値 保持長さ
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodKeepLen" & iColIx
			sDbItemName = "PeriodKeepLen"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value = drow(0).Item(sDbItemName)            'PeriodKeepLen

            ' 設定値 歯抜け補正係数
			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'sDbItemName = "PeriodWeight" & iColIx
			sDbItemName = "PeriodWeight"
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value = drow(0).Item(sDbItemName)             'PeriodWeight

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			sDbItemName = "PeriodFrontEnableLen"
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value = drow(0).Item(sDbItemName)   'PeriodFrontEnableLen

			sDbItemName = "PeriodTailEnableLen"
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value = drow(0).Item(sDbItemName)   'PeriodTailEnableLen
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

        Next ii

    End Sub
    ''' <summary>
    ''' 全選択　全解除ボタン設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub Disp_SelectBtn(ByVal dgv As DataGridView)

        '行を初期化
        dgv.Rows.Clear()

        ' 画面に表示
        dgv.RowCount = 1                  ' ボタン設定のレコード数は"1"で固定

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連            
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データグリッドビュー設定 周期疵設定有無 
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize_TypeMsc(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

        End With

        ' NO
        dgvColSetText(dgv, "NO", 45, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 疵種
        dgvColSetText(dgv, "疵種", 145, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        For ii As Integer = 2 To mc_TypeMsc_ColMax - 1
            ' グレード設定
            dgvColSetCheck(dgv, CStr(ii - 1), 100, False, False, _
                           DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        Next ii
    End Sub

    ''' <summary>
    ''' タイトル列のセルが選択されたとき選択解除を行う 周期疵有無設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvTypeMscData.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        'タイトルが書かれているセルなら、選択解除する
        If dgv.CurrentCell.ColumnIndex <= EM_DATACOLUMN_GRID_ID.emTxt_Name Then
            dgv.CurrentCell.Selected = False        ' 選択解除
        End If
    End Sub

    ''' <summary>
    ''' セルの内容が変化した場合
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridViewのチェックボックス等押された場合 通常 セルを移動させないと確定にならない為の処置</remarks>
    Private Sub dgvTypeMscData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvTypeMscData.CurrentCellDirtyStateChanged
        ' セルがコミットされていない変更を含む場合 コミットする
        CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
    End Sub

    ''' <summary>
    ''' データグリッドビュー設定 周期疵設定値 
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize_Period(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                                   ' ヘッダーの高さ
            .RowTemplate.Height = 38                                                    ' テンプレートの行高さを設定

        End With

        ' 行タイトル
        dgvColSetText(dgv, "", 190, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
        dgv.Columns(EM_PERIOD_GRID_COL_ID.emTxt_Title).DefaultCellStyle.WrapMode = DataGridViewTriState.True

        For ii As Integer = 1 To mc_Period_ColMax
            ' グレード設定
            dgvColSetText(dgv, CStr(ii), 100, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        Next ii
    End Sub

    ''' <summary>
    ''' タイトル列のセルが選択されたとき選択解除を行う 周期疵設定値
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPeriodData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPeriodData.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        'タイトルが書かれているセルなら、選択解除する
        If dgv.CurrentCell.ColumnIndex <= EM_PERIOD_GRID_COL_ID.emTxt_Title Then
            dgv.CurrentCell.Selected = False        ' 選択解除
        End If
    End Sub

    ''' <summary>
    ''' セルの内容が変化した場合 周期疵設定値
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridViewのチェックボックス等押された場合 通常 セルを移動させないと確定にならない為の処置</remarks>
    Private Sub dgvPeriodData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPeriodData.CurrentCellDirtyStateChanged
        ' セルがコミットされていない変更を含む場合 コミットする
        CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
    End Sub

    ''' <summary>
    ''' データグリッドビュー設定 全選択 Or 全解除
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <param name="iMode">表示設定 0:全選択 1:全解除</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize_SelectBtn(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal iMode As Integer)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                            ' 選択色無し

        With dgv
            .RowCount = 0

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			'.ScrollBars = ScrollBars.Both                                    ' スクロールバーの表示			            
			.ScrollBars = ScrollBars.None                                   ' スクロールバーの表示
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue   ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                        ' ヘッダーの高さ
            .ColumnHeadersVisible = False                                    ' 列ヘッダ表示

        End With

        ' データ部と列数をあわせるので、先頭の２列にダミー列を追加する。
        ' ダミー1
        dgvColSetText(dgv, "Dummy1", 40, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

        ' ダミー2
        dgvColSetText(dgv, "Dummy2", 40, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

        'ここから本来のボタン設定
        For ii As Integer = 2 To mc_TypeMsc_ColMax - 1
            If EM_SELECT_MODE.emSelect = iMode Then                                         ' 全選択
                ' 全選択ボタン
                dgvColSetButton(dgv, m_strBtnName(EM_SELECT_MODE.emSelect), 100, False, False, _
                                DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                                New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, m_strBtnName(EM_SELECT_MODE.emSelect))
                dgv.Columns(ii).DefaultCellStyle().BackColor = Color.White                  ' 背景
                dgv.Columns(ii).DefaultCellStyle().SelectionBackColor = Color.White         ' 選択

            Else                                                                            ' 全解除
                ' 全解除ボタン
                dgvColSetButton(dgv, m_strBtnName(EM_SELECT_MODE.emCancel), 100, False, False, _
                                DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                                New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, m_strBtnName(EM_SELECT_MODE.emCancel))
                dgv.Columns(ii).DefaultCellStyle().BackColor = Color.AliceBlue              ' 背景
                dgv.Columns(ii).DefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 選択

            End If

        Next ii
    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub

	'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>    
	''' <summary>
	''' dgvTypeMscDataの横スクロールが変更されたときに その他dgvをスクロールさせる
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks>項目が増えたとき用</remarks>
	Private Sub dgvTypeMscData_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvTypeMscData.Scroll, dgvPeriodData.Scroll
		' スクロールイベントが水平スクロールなら
		If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
			' スクロール位置を合わせる
			Dim nScollingOffset As Integer = 0
			If sender Is dgvTypeMscData Then
				nScollingOffset = dgvTypeMscData.HorizontalScrollingOffset
				dgvPeriodData.HorizontalScrollingOffset = nScollingOffset
			Else
				nScollingOffset = dgvPeriodData.HorizontalScrollingOffset
				dgvTypeMscData.HorizontalScrollingOffset = nScollingOffset
			End If
			dgvTypeMscBtnAll.HorizontalScrollingOffset = nScollingOffset
			dgvTypeMscBtnClear.HorizontalScrollingOffset = nScollingOffset
		End If
	End Sub

	''' <summary>
	''' '縦スクロールバー表示有りならば、縦スクロールバー分、dgvの幅を広げる
	''' </summary>
	''' <remarks></remarks>
	Private Sub ChangeDgvWidth()
		Dim vsBar As VScrollBar

		For Each c As Control In dgvTypeMscData.Controls
			If TypeOf c Is VScrollBar Then
				vsBar = DirectCast(c, VScrollBar)

				If True = vsBar.Visible Then
					dgvTypeMscData.Width = dgvTypeMscData.Width + vsBar.Width
				End If
			End If
		Next

		For Each c As Control In dgvPeriodData.Controls
			If TypeOf c Is VScrollBar Then
				vsBar = DirectCast(c, VScrollBar)

				If True = vsBar.Visible Then
					dgvPeriodData.Width = dgvPeriodData.Width + vsBar.Width
				End If
			End If
		Next
	End Sub
	'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------

End Class
