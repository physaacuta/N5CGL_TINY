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

Public Class frmLongPd
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定数
	'/ /////////////////////////////////////////////////////////////////////////////////

	' データグリッドビューの「列」（長周期欠陥検出マスター設定)
	Private Enum EM_COMMON_GRID_COL_ID
		emTxt_Name = 0               ' 名称
		emTxt_Filter_Start           ' フィルター1～16の設定
	End Enum

	' データグリッドビューの「行」（長周期欠陥検出マスター設定)
	Private Enum EM_COMMON_GRID_ROW_ID
		emTxt_ExecInterval = 0                        ' 定周期実行間隔 int
		emTxt_InitialExec_OldLimit                    ' 定周期実行実績対象範囲 int
		emTxt_MaxDefectLen                            ' 【FFT】最大疵長さ int
		emTxt_FFTFreq                                 ' 【FFT】サンプリング周期 int
		emTxt_FFTPeakThr                              ' 【FFT】ピーク検出閾値 float 
		emTxt_HoughConvertAccumulatorRasio            ' 【ハフ変換】FFT周期倍率 float
		emTxt_HoughConvertMinLineLengthRasio          ' 【ハフ変換】最小線分長倍率 float
		emTxt_HoughConvertMaxLineGap                  ' 【ハフ変換】ラインギャップ int
		emTxt_HoughConvertOnlineThr                   ' 【ハフ変換】直線状判断余裕代 float
		emTxt_HoughConvertTangentThr                  ' 【ハフ変換】基準Tangent倍率 float
		emCmb_ExpandImage                             ' 【ハフ変換】画像連結処理有無(0:「連結なし」 1:「連結あり」)
		emTxt_CsvDir                                  ' CSV出力モード出力先フォルダ varchar(max)
		emTxt_CsvSaveDate                             ' CSV出力モード出力先ファイル保存日数 int
		emTxt_PeriodicExecInterval                    ' 定周期処理ピッチ int

		ROW_MAX
	End Enum

	' データグリッドビューの「列」（長周期欠陥疵種設定)
	Private Enum EM_OUTPUT_GRID_COL_ID
		emTxt_No = 0
		emTxt_Name                  ' 疵種
		emTxt_EdasysId              ' 疵種EdasysId (非表示)
		emCmb_Grade_Start           ' フィルター1～16のグレード設定開始列
	End Enum


	' データグリッドビューの「行」（長周期欠陥検出_検査パターン×フィルター設定)
	Private Enum EM_FILTER_GRID_ROW_ID
		emTxt_Name = 0              ' フィルター名称
		emCmb_Enable                ' 有効／無効
		emTxt_MaskFront             ' コイル先端マスク量
		emTxt_MaskTail              ' コイル尾端マスク量
		emTxt_MaskLeft              ' 左エッジマスク量
		emTxt_MaskRight             ' 右エッジマスク量
		emTxt_DetectPdMin           ' 検出周期下限
		emTxt_DetectPdMax           ' 検出周期上限
		emTxt_DefectNumMin          ' 検出個数下限
		emTxt_DefectNumMax          ' 検出個数上限
		emTxt_WidthDiv              ' 周期検出分割単位巾

		ROW_MAX
	End Enum

	' データグリッドビューの「列」（長周期欠陥検出_検査パターン×フィルター設定)
	Private Enum EM_FILTER_GRID_COL_ID
		emTxt_Name = 0               ' 名称
		emTxt_Filter_Start           ' フィルター1～16の設定
	End Enum

	'MAX_LONGPD_FILTER
	Private Const MX_COMMON_COLMAX As Integer = MAX_LONGPD_FILTER + 1        ' 長周期欠陥検出マスター設定 最大列数
	Private Const MX_FILTER_COLMAX As Integer = MAX_LONGPD_FILTER + 1        ' 長周期欠陥検出_検査パターン×フィルター設定 最大列数
	Private Const MX_OUTPUT_COLMAX As Integer = MAX_LONGPD_FILTER + 2        ' 長周期疵有無設定 最大列数
	Private Const MC_OUTPUT_ROWMAX As Integer = MAX_TYPE                     ' 長周期疵設定最大行数

	' 選択ボタンの定数
	Private Enum EM_SELECT_MODE
		emCancel                         ' 全解除
	End Enum

	'長周期欠陥検出マスター設定 DGV用の行タイトル設定
	Private m_strCommonRowHead() As String = {
	   "定周期実行間隔" & vbNewLine & "(0 - ) [秒]",
	   "定周期実行実績対象範囲" & vbNewLine & "(0 - ) [分]",
	   "【FFT】最大疵長さ" & vbNewLine & "[mm]",
	   "【FFT】サンプリング周期" & vbNewLine & "(1 - ) [mm]",
	   "【FFT】ピーク検出閾値" & vbNewLine & "",
	   "【ハフ変換】FFT周期倍率" & vbNewLine & "",
	   "【ハフ変換】最小線分長倍率" & vbNewLine & "",
	   "【ハフ変換】ラインギャップ" & vbNewLine & "",
	   "【ハフ変換】直線状判断余裕代" & vbNewLine & "",
	   "【ハフ変換】基準Tangent倍率" & vbNewLine & "",
	   "【ハフ変換】画像連結処理有無" & vbNewLine & "",
	   "CSV出力モード出力先フォルダ" & vbNewLine & "",
	   "CSV出力モード出力先ファイル" & vbNewLine & "保存日数[日]",
	   "定周期処理ピッチ" & vbNewLine & "(0 - ) [mm]"
		}

	'長周期欠陥検出_検査パターン×フィルター設定 DGV用の行タイトル設定
	Private m_strFilterRowHead() As String = {
	   "フィルター名称" & vbNewLine & "(全角32文字)",
	   "有効／無効" & vbNewLine & "",
	   "コイル先端マスク量" & vbNewLine & "(0 - ) [mm]",
	   "コイル尾端マスク量" & vbNewLine & "(0 - ) [mm]",
	   "左エッジマスク量" & vbNewLine & "(0 - ) [mm]",
	   "右エッジマスク量" & vbNewLine & "(0 - ) [mm]",
	   "検出周期下限" & vbNewLine & "(-1 - ) [mm]",
	   "検出周期上限" & vbNewLine & "(-1 - ) [mm]",
	   "検出個数下限" & vbNewLine & "(-1 - ) [個]",
	   "検出個数上限" & vbNewLine & "(-1 - ) [個]",
	   "周期検出分割単位巾" & vbNewLine & "[mm]"
		}

	''ボタンdgv用の定数設定ボタン用のdgvは疵種dgvと列を揃える
	Private m_strBtnName() As String = {"全解除"}

	' 周期判定処理有無 コンボ用
	Private m_strCmbLongPcodeEnable() As String = {"なし", "あり"}

	' 【ハフ変換】画像連結処理有無　コンボ用
	Private m_strCmbExpandImageValue() As String = {"連結なし", "連結あり"}

	' 有効／無効 コンボ用
	Private m_strCmbFilterEnableValue() As String = {"無効", "有効"}

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_Adapter_LongPd_Common As SqlClient.SqlDataAdapter = Nothing               ' アダプター
	Private m_Adapter_LongPd_PCode As SqlClient.SqlDataAdapter = Nothing                ' アダプター
	Private m_Adapter_LongPd_PCodeFilter As SqlClient.SqlDataAdapter = Nothing          ' アダプター
	Private m_Adapter_LongPd_PCodeFilterOutput As SqlClient.SqlDataAdapter = Nothing    ' アダプター
	Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing                         ' アダプター
	Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing                        ' アダプター

	Private m_DS_LongPd_Common As New DataSet                                           ' 非接続型テーブル
	Private m_DS_LongPd_PCode As New DataSet                                            ' 非接続型テーブル
	Private m_DS_LongPd_PCodeFilter As New DataSet                                      ' 非接続型テーブル
	Private m_DS_LongPd_PCodeFilterOutput As New DataSet                                ' 非接続型テーブル
	Private m_DsType As New DataSet                                                     ' 非接続形テーブル
	Private m_DsGrade As New DataSet                                                    ' 非接続型テーブル

	Private m_KeyCode As String = ""                                                    ' 主キー(PCode)

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
	Private Sub frmLongPd_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' ファンクションボタン有効項目のみ
		If Not enable Then Exit Sub

		'' ファンクション切替
		Select Case FncNo                       ' ※0オリジン
			Case 6                              ' F07 画面印刷
				Call PrintOut()
			Case 7                              ' F08 画面保存
				Call HardCopy(IMG_PATH, "長周期欠陥検出設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
			Case 8
				modMain.ChgProject()            ' F09 KA_PARSETへ切替処理
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
	Private Sub frmLongPd_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' いろいろ開放処理
		m_DS_LongPd_Common.Dispose()
		m_DS_LongPd_PCode.Dispose()
		m_DS_LongPd_PCodeFilter.Dispose()
		m_DS_LongPd_PCodeFilterOutput.Dispose()
		m_DsType.Dispose()
		m_DsGrade.Dispose()

		If Not m_DS_LongPd_Common Is Nothing Then
			m_DS_LongPd_Common.Dispose()
			m_DS_LongPd_Common = Nothing
		End If

		If Not m_Adapter_LongPd_PCode Is Nothing Then
			m_Adapter_LongPd_PCode.Dispose()
			m_Adapter_LongPd_PCode = Nothing
		End If

		If Not m_Adapter_LongPd_PCodeFilter Is Nothing Then
			m_Adapter_LongPd_PCodeFilter.Dispose()
			m_Adapter_LongPd_PCodeFilter = Nothing
		End If

		If Not m_Adapter_LongPd_PCodeFilterOutput Is Nothing Then
			m_Adapter_LongPd_PCodeFilterOutput.Dispose()
			m_Adapter_LongPd_PCodeFilterOutput = Nothing
		End If

		If Not m_AdapterType Is Nothing Then
			m_AdapterType.Dispose()
			m_AdapterType = Nothing
		End If

		If Not m_AdapterGrade Is Nothing Then
			m_AdapterGrade.Dispose()
			m_AdapterGrade = Nothing
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
	Private Sub frmLongPd_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

		'タイトル設定
		Call SetTitleName(Me)

		modMain.SetListMenu(Me)                                     ' メニューリスト生成
		modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
		MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmLongPd                ' 長周期欠陥検出設定画面呼出

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		' アップコードのコンボボックス
		MyBase.SetComboBox_Pcode(cmbUpCodeItem)
		MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

		' 周期判定処理コンボ			
		cmbEnable.Items.Clear()
		cmbEnable.Items.AddRange(m_strCmbLongPcodeEnable)

		' このフォームで必要なデータを取得する コンボボックスの値もとってくるので、dgv設定より先
		If Not DB_Select() Then
			' DB 失敗なので、画面強制終了
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "異常"
			frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '終了
		End If

		' コンボボックスの表示データを初期化
		Data_Init()

		' （共通）長周期欠陥検出_マスター設定
		dgvInitialize_Common(dgvCommon)

		' （疵種設定）長周期欠陥_疵種設定
		dgvInitialize_Output(dgvOutput, True)
		dgvInitialize_Output(dgvOutputEtc, False)

		' （検査パターン別）長周期欠陥検出_検査パターン×フィルター設定
		dgvInitialize_Filter(dgvFilter)

		' 全選択解除
		dgvInitialize_SelectBtn(dgvOutputBtnClear)

		' コンボボックスに検査パターンをセット
		MyBase.SetComboBox_Pcode(cmbPCode)

		DoubleBuffered = True

		If 0 < cmbPCode.Items.Count Then
			'前の画面で選択した状態をセット
			If cmbPCode.Items.Count - 1 < g_nPaternIndex Then
				'該当項目がない場合は、先頭を表示
				cmbPCode.SelectedIndex = 0
			Else
				cmbPCode.SelectedIndex = g_nPaternIndex
			End If
		Else
			' 画面表示失敗なので、強制終了
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "異常"
			frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '終了
		End If

		' 画面にパターン別の値を表示		
		m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
		Data_Disp()

		' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
		dgvCommon.DataGridViewSelectionRowColorChange(g_ColorSelection)
		dgvOutput.DataGridViewSelectionRowColorChange(g_ColorSelection)
		dgvOutputEtc.DataGridViewSelectionRowColorChange(g_ColorSelection)
		dgvFilter.DataGridViewSelectionRowColorChange(g_ColorSelection)

		' 更新ボタンの有効/無効
		Call UpdateEnabled(g_bPassFlg)

		'プリンタがなければ、画面印刷無効
		If Not tClass_HeadCopy.IsCheckPrinter Then
			btnF07.Enabled = False
		Else
			btnF07.Enabled = True
		End If

		frmMsg.Dispose()
		frmMsg = Nothing

		' 縦スクロールバー表示有りならば、dgvの幅を広げる
		'ChangeDgvWidth()
	End Sub

	''' <summary>
	''' ディスポーズ
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmLongPd_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

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
		Dim sbSQL As New System.Text.StringBuilder

		'--------------------------------------------------------------
		' 長周期欠陥検出_マスター設定
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     ID")
		sbSQL.AppendLine("    ,ExecInterval")
		sbSQL.AppendLine("    ,InitialExec_OldLimit")
		sbSQL.AppendLine("    ,MaxDefectLen")
		sbSQL.AppendLine("    ,FFTFreq")
		sbSQL.AppendLine("    ,FFTPeakThr")
		sbSQL.AppendLine("    ,HoughConvertAccumulatorRasio")
		sbSQL.AppendLine("    ,HoughConvertMinLineLengthRasio")
		sbSQL.AppendLine("    ,HoughConvertMaxLineGap")
		sbSQL.AppendLine("    ,HoughConvertOnlineThr")
		sbSQL.AppendLine("    ,HoughConvertTangentThr")
		sbSQL.AppendLine("    ,ExpandImage")
		sbSQL.AppendLine("    ,CsvDir")
		sbSQL.AppendLine("    ,CsvSaveDate")
		sbSQL.AppendLine("    ,PeriodicExecInterval")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0}", DB_LONGPD_COMMON)
		sbSQL.AppendLine()
		sbSQL.AppendLine("ORDER BY ID ASC")

		strSQL = sbSQL.ToString()

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_Common, m_Adapter_LongPd_Common, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_COMMON & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' 長周期欠陥検出_検査パターンコード別設定
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     Pcode")
		sbSQL.AppendLine("    ,Enable")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0}", DB_LONGPD_PCODE)
		sbSQL.AppendLine()
		sbSQL.AppendLine("ORDER BY Pcode ASC")

		strSQL = sbSQL.ToString()

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCode, m_Adapter_LongPd_PCode, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' 長周期欠陥検出_検査パターン×フィルター設定
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     Pcode")
		sbSQL.AppendLine("    ,Filter")
		sbSQL.AppendLine("    ,Name")
		sbSQL.AppendLine("    ,Enable")
		sbSQL.AppendLine("    ,MaskFront")
		sbSQL.AppendLine("    ,MaskTail")
		sbSQL.AppendLine("    ,MaskLeft")
		sbSQL.AppendLine("    ,MaskRight")
		sbSQL.AppendLine("    ,DetectPdMin")
		sbSQL.AppendLine("    ,DetectPdMax")
		sbSQL.AppendLine("    ,DefectNumMin")
		sbSQL.AppendLine("    ,DefectNumMax")
		sbSQL.AppendLine("    ,WidthDiv")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0}", DB_LONGPD_PCODE_FILTER)
		sbSQL.AppendLine()
		sbSQL.AppendLine("ORDER BY")
		sbSQL.AppendLine("    Pcode ASC,")
		sbSQL.AppendLine("    Filter ASC")

		strSQL = sbSQL.ToString()

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCodeFilter, m_Adapter_LongPd_PCodeFilter, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' 長周期欠陥_疵種設定
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     Pcode")
		sbSQL.AppendLine("    ,Filter")
		sbSQL.AppendLine("    ,TypeEdasysID")
		sbSQL.AppendLine("    ,GrEdasysID")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0} A ", DB_LONGPD_PCODE_FILTER_OUTPUT)
		sbSQL.AppendLine()
		sbSQL.AppendFormat("    LEFT OUTER JOIN {0} B ", DB_TYPE_NAME)
		sbSQL.AppendLine()
		sbSQL.AppendLine(" ON A.TypeEdasysID = B.EdasysID ")
		sbSQL.AppendLine("ORDER BY B.ID ")

		strSQL = sbSQL.ToString()

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCodeFilterOutput, m_Adapter_LongPd_PCodeFilterOutput, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' 疵種 ※コンボボックス用
		'--------------------------------------------------------------
		' SQL文生成 疵種はIDで昇順
		strSQL = "SELECT EdasysID"
		strSQL &= ", Name"
		strSQL &= " FROM " & DB_TYPE_NAME
		strSQL &= " ORDER BY ID ASC"

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' グレード ※コンボボックス用
		'--------------------------------------------------------------
		' SQL文生成 グレードは優先度で昇順
		strSQL = "SELECT EdasysID"
		strSQL &= ", Name"
		strSQL &= ", マップ表示色"
		strSQL &= ", 優先度"
		strSQL &= " FROM " & DB_GRADE_NAME
		strSQL &= " ORDER BY 優先度 ASC"

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		Return True
	End Function

	''' <summary>
	''' DBからとってきたコンボボックス用の値を表示用に加工
	''' </summary>
	''' <remarks>グレード名称に以上をつける、無しの行を追加、</remarks>
	Private Sub Data_Init()
		Dim drowNewType As DataRow      ' 疵種マスターの新規行
		Dim drowNewGrade As DataRow     ' 疵種グレードの新規行

		Try
			'グレード非接続テーブルの設定
			' 全行ループ グレードの名称に以上をつける
			For ii As Integer = 0 To m_DsGrade.Tables(0).Rows.Count - 1
				m_DsGrade.Tables(0).Rows(ii).Item("Name") = m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString & "以上"
			Next ii
			' グレードの新規行を設定 EdasysID、グレード名称、マップ表示色
			drowNewGrade = m_DsGrade.Tables(0).NewRow()
			drowNewGrade.Item("EdasysID") = 0
			drowNewGrade.Item("Name") = "-"
			drowNewGrade.Item("マップ表示色") = -1      ' マップ表示色、使わないけど一応設定
			' テーブルの一番上に挿入
			m_DsGrade.Tables(0).Rows.InsertAt(drowNewGrade, 0)

		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス用データ 設定エラー [" & ex.Message & "] " & Me.Text)
		End Try

	End Sub

	''' <summary>
	''' 取得データを画面に表示する。
	''' </summary>
	''' <param name="bDispOnlyPcode">True:コンボで選択されたPCodeのデータだけ表示 False:全表示</param>
	''' <remarks></remarks>
	Private Sub Data_Disp(Optional ByVal bDispOnlyPcode As Boolean = False)

		If False = bDispOnlyPcode Then
			' 全選択、全解除ボタン設定
			Call Disp_SelectBtn(dgvOutputBtnClear)

			' 長周期欠陥検出_マスター設定
			Call Db_Disp_Common()
		End If

		' 長周期欠陥_疵種設定
		Call Db_Disp_PcodeFilterOutput()

		' 長周期欠陥検出_検査パターンコード別設定
		Call Db_Disp_Pcode()

		' 長周期欠陥検出_検査パターン×フィルター設定
		Call Db_Disp_PcodeFilter()

	End Sub

	''' <summary>
	''' 疵種データdgvのチェックボックスのチェックを変更した時　色を変える
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvOutput_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvOutput.CellValueChanged, dgvOutputEtc.CellValueChanged
		Dim dgv As DataGridView = CType(sender, DataGridView)

		' 行以外はスルー
		If e.RowIndex < 0 Then Exit Sub

		If EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start <= e.ColumnIndex And
		   EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + MAX_LONGPD_FILTER >= e.ColumnIndex Then

			' セルの値を取得
			Dim strEdasysID As String = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

			' セルの値（EdasysID）で色を取得し、背景色を変更する 
			dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
		End If
	End Sub

	''' <summary>
	''' 色取得
	''' </summary>
	''' <param name="key">GrEdasysID</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetCmbColor(ByVal key As String) As Color
		' EdasysIDの値を取り出し
		Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE句に相当の条件生成
		Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select(strWhere)       ' 1行あるはず

		Dim retColor As Color = Color.Empty   ' 戻り値 初期値はNULL色

		' Keyが0ならNULL色を返す
		If 0 = CInt(key) Then Return retColor

		' 1行しかないと思うけど
		For ii As Integer = 0 To drowGrade.Length - 1
			' カラーを取得
			retColor = Color.FromArgb(CInt(drowGrade(0).Item("マップ表示色").ToString))
		Next ii

		Return retColor
	End Function

	''' <summary>
	''' 全解除dgvボタン押下時
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvTypeMscBtnClear_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvOutputBtnClear.CellContentClick

		'ボタン列でなければなにもしない
		If e.ColumnIndex <= 0 Then Return

		'列のコンボを未選択にする。
		ClearOutputComboBox(dgvOutput, e.ColumnIndex)
		ClearOutputComboBox(dgvOutputEtc, e.ColumnIndex)
	End Sub

	''' <summary>
	''' 対象dgvの対象列のコンボボックスを未選択にする。
	''' </summary>
	''' <param name="dgv">対象のdgv</param>
	''' <param name="nIndex">列インデックス</param>
	''' <remarks></remarks>
	Private Sub ClearOutputComboBox(ByVal dgv As DataGridView, ByVal nIndex As Integer)
		Dim ii As Integer   'カウンタ

		'全行のコンボボックスを未選択に。
		For ii = 0 To dgv.RowCount - 1
			dgv(nIndex, ii).Value = 0
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
		Dim nColIx As Integer                                   ' 列INDEX


		' 共通(長周期欠陥検出_マスター設定)
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_COMMON_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strFilterMsg As String = String.Format("フィルター{0}の", ii + 1)

			' 定周期実行間隔
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "定周期実行間隔") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value = strWk

			' 定周期実行実績対象範囲
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "定周期実行実績対象範囲") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value = strWk

			' 【FFT】最大疵長さ
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "【FFT】最大疵長さ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value = strWk

			' 【FFT】サンプリング周期
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, Int32.MaxValue, False, strFilterMsg & "【FFT】サンプリング周期") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value = strWk

			' 【FFT】ピーク検出閾値
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "【FFT】ピーク検出閾値") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value = strWk

			' 【ハフ変換】FFT周期倍率
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "【ハフ変換】FFT周期倍率") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value = strWk

			' 【ハフ変換】最小線分長倍率
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "【ハフ変換】最小線分長倍率") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value = strWk

			' 【ハフ変換】ラインギャップ
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "【ハフ変換】ラインギャップ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value = strWk

			' 【ハフ変換】直線状判断余裕代
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "【ハフ変換】直線状判断余裕代") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value = strWk

			' 【ハフ変換】基準Tangent倍率 
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "【ハフ変換】基準Tangent倍率 ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value = strWk

			' 【ハフ変換】画像連結処理有無
			' コンボなのでチェックなし

			' CSV出力モード出力先フォルダ
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, , , strFilterMsg & "CSV出力モード出力先フォルダ ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value = strWk

			' CSV出力モード出力先ファイル保存日数
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "CSV出力モード出力先ファイル保存日数") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value = strWk

			' 定周期処理ピッチ
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "定周期処理ピッチ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value = strWk
		Next ii

		' 長周期欠陥検出_検査パターンコード別設定
		' コンボなのでチェックなし

		' 長周期欠陥検出_検査パターン×フィルター設定
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_FILTER_GRID_COL_ID.emTxt_Filter_Start + ii
			Dim strFilterMsg As String = String.Format("フィルター{0}の", ii + 1)

			' フィルター名称
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, , 64, strFilterMsg & "フィルター名称") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value = strWk

			' 有効／無効
			' コンボなのでチェックなし

			' コイル先端マスク量
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "コイル先端マスク量") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value = strWk

			' コイル尾端マスク量
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "コイル尾端マスク量") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value = strWk

			' 左エッジマスク量
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "左エッジマスク量") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value = strWk

			' 右エッジマスク量
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "右エッジマスク量") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value = strWk

			' 検出周期下限
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "検出周期下限") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value = strWk

			' 検出周期上限
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "検出周期上限") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value = strWk

			' 検出個数下限
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, -1, Int32.MaxValue, False, strFilterMsg & "検出個数下限") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value = strWk

			' 検出個数上限
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, -1, Int32.MaxValue, False, strFilterMsg & "検出個数上限") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value = strWk

			' 周期検出分割単位巾
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, "周期検出分割単位巾") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value = strWk
		Next ii

		' 長周期欠陥_疵種設定
		' コンボなのでチェックなし

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
		Data_Get_Common()
		Data_Get_Output()
		Data_Get_Pcode()
		Data_Get_PcodeFilter()
	End Sub

	''' <summary>
	'''  画面に表示中のデータを非接続型テーブルにセット (長周期欠陥検出_マスター設定)
	''' </summary>
	Private Sub Data_Get_Common()
		Dim nColIx As Integer                                               ' 列INDEX

		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_COMMON_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("ID='{0}' ", ii + 1)            ' WHERE句に相当の条件生成
			Dim drow() As DataRow = m_DS_LongPd_Common.Tables(0).Select(strWhere)

			'' データが無ければ追加
			'If drow Is Nothing OrElse drow.Length <= 0 Then

			'	Dim temp As DataRow = m_DS_LongPd_Common.Tables(0).NewRow
			'	temp.Item("ID") = (ii + 1).ToString()
			'	m_DS_LongPd_Common.Tables(0).Rows.Add(temp)

			'	drow = m_DS_LongPd_Common.Tables(0).Select(strWhere)
			'End If

			If drow Is Nothing OrElse drow.Length <= 0 Then
				' 全件有る筈
				Continue For
			End If

			' 定周期実行間隔
			drow(0).Item("ExecInterval") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value

			' 定周期実行実績対象範囲
			drow(0).Item("InitialExec_OldLimit") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value

			' 【FFT】最大疵長さ
			drow(0).Item("MaxDefectLen") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value

			' 【FFT】サンプリング周期
			drow(0).Item("FFTFreq") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value

			' 【FFT】ピーク検出閾値
			drow(0).Item("FFTPeakThr") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value

			' 【ハフ変換】FFT周期倍率
			drow(0).Item("HoughConvertAccumulatorRasio") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value

			' 【ハフ変換】最小線分長倍率
			drow(0).Item("HoughConvertMinLineLengthRasio") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value

			' 【ハフ変換】ラインギャップ
			drow(0).Item("HoughConvertMaxLineGap") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value

			' 【ハフ変換】直線状判断余裕代
			drow(0).Item("HoughConvertOnlineThr") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value

			' 【ハフ変換】基準Tangent倍率 
			drow(0).Item("HoughConvertTangentThr") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value

			' 【ハフ変換】画像連結処理有無
			For nIdx As Integer = 0 To m_strCmbExpandImageValue.Length - 1
				If dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage).Value.ToString = m_strCmbExpandImageValue(nIdx) Then
					drow(0).Item("ExpandImage") = nIdx
				End If
			Next nIdx

			' CSV出力モード出力先フォルダ
			drow(0).Item("CsvDir") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value

			' CSV出力モード出力先ファイル保存日数
			drow(0).Item("CsvSaveDate") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value

			' 定周期処理ピッチ
			drow(0).Item("PeriodicExecInterval") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value
		Next ii

	End Sub

	''' <summary>
	''' 画面に表示中のデータを非接続型テーブルにセット (長周期欠陥_疵種設定)
	''' </summary>
	Private Sub Data_Get_Output()

		Try
			' データを取得
			For ii As Integer = 0 To dgvOutput.RowCount - 1
				Dim nTypeEdasysId As Integer = CInt(dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_EdasysId, ii).Value)

				For nFilter As Integer = 1 To MAX_LONGPD_FILTER
					Dim strWhere As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID={2}", m_KeyCode, nFilter, nTypeEdasysId)
					Dim drow() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhere)                ' EdasysIDが1以上のデータを    0～64行

					'現在画面に表示しているpcodeの非接続テーブルのデータを削除(後ろから削除)
					Dim nRowCount As Integer = drow.Length
					For nRowIndex As Integer = 1 To nRowCount
						m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Remove(drow(nRowCount - nRowIndex))
					Next

					' 今表示しているデータを格納
					Dim drowNew As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).NewRow()
					drowNew.Item("pcode") = m_KeyCode
					drowNew.Item("filter") = nFilter
					drowNew.Item("TypeEdasysID") = nTypeEdasysId
					drowNew.Item("GrEdasysID") = dgvOutput(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, ii).Value

					' テーブルに追加
					m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.InsertAt(drowNew, m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count)
				Next nFilter
			Next ii

			' その他
			For nFilter As Integer = 1 To MAX_LONGPD_FILTER
				Dim strWhereEtc As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID=-1", m_KeyCode, nFilter)
				Dim drowEtc() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhereEtc)

				'現在画面に表示しているpcodeの非接続テーブルのデータを削除(後ろから削除)
				Dim nRowCount As Integer = drowEtc.Length
				For nRowIndex As Integer = 1 To nRowCount
					m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Remove(drowEtc(nRowCount - nRowIndex))
				Next

				' 今表示しているデータを格納
				Dim drowNew As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).NewRow()
				drowNew.Item("pcode") = m_KeyCode
				drowNew.Item("filter") = nFilter
				drowNew.Item("TypeEdasysID") = -1
				drowNew.Item("GrEdasysID") = dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, 0).Value


				' テーブルに追加
				m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.InsertAt(drowNew, m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count)
			Next nFilter

		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " 取得エラー [" & ex.Message & "] " & Me.Text)
			Return
		End Try
	End Sub

	''' <summary>
	'''  画面に表示中のデータを非接続型テーブルにセット (長周期欠陥検出_検査パターンコード別設定)
	''' </summary>
	Private Sub Data_Get_Pcode()
		Dim strWhere As String = String.Format("Pcode='{0}' ", m_KeyCode)            ' WHERE句に相当の条件生成
		Dim drow() As DataRow = m_DS_LongPd_PCode.Tables(0).Select(strWhere)

		If drow Is Nothing OrElse drow.Length <= 0 Then
			' データは有る筈
		Else
			drow(0).Item("Enable") = cmbEnable.SelectedIndex
		End If
	End Sub

	''' <summary>
	'''  画面に表示中のデータを非接続型テーブルにセット(長周期欠陥検出_検査パターン×フィルター設定)
	''' </summary>
	Private Sub Data_Get_PcodeFilter()
		Dim nColIx As Integer                                               ' 列INDEX

		' データ表示
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_FILTER_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("Pcode='{0}' and Filter={1} ", m_KeyCode, ii + 1)            ' WHERE句に相当の条件生成
			Dim drow() As DataRow = m_DS_LongPd_PCodeFilter.Tables(0).Select(strWhere)

			' データが無ければスキップ
			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If

			' フィルター名称
			If dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value Is Nothing OrElse
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value.ToString = "" Then

				drow(0).Item("Name") = DBNull.Value
			Else
				drow(0).Item("Name") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value
			End If

			' 有効／無効
			For nIdx As Integer = 0 To m_strCmbFilterEnableValue.Length - 1
				If dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable).Value.ToString = m_strCmbFilterEnableValue(nIdx) Then
					drow(0).Item("Enable") = nIdx
				End If
			Next nIdx

			' コイル先端マスク量
			drow(0).Item("MaskFront") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value

			' コイル尾端マスク量
			drow(0).Item("MaskTail") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value

			' 左エッジマスク量
			drow(0).Item("MaskLeft") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value

			' 右エッジマスク量
			drow(0).Item("MaskRight") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value

			' 検出周期下限
			drow(0).Item("DetectPdMin") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value

			' 検出周期上限
			drow(0).Item("DetectPdMax") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value

			' 検出個数下限
			drow(0).Item("DefectNumMin") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value

			' 検出個数上限
			drow(0).Item("DefectNumMax") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value

			' 周期検出分割単位巾
			drow(0).Item("WidthDiv") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value

		Next ii
	End Sub


	''' <summary>
	''' アップデート
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Dim transactCommon As SqlClient.SqlTransaction = Nothing   'トランザクション処理
		Dim transactPCode As SqlClient.SqlTransaction = Nothing    'トランザクション処理
		Dim transactFilter As SqlClient.SqlTransaction = Nothing    'トランザクション処理
		Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
		tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim sbSql As New Text.StringBuilder
		Dim bProcState As Boolean = False

		Try
			' DB接続
			'''' データベースオープン
			If Not tcls_DB.Open() Then
				bProcState = False
				Exit Try
			End If

			m_Adapter_LongPd_Common.SelectCommand.Connection.Open()
			m_Adapter_LongPd_PCode.SelectCommand.Connection.Open()
			m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.Open()

			'トランザクション開始
			transactCommon = m_Adapter_LongPd_Common.SelectCommand.Connection.BeginTransaction()
			m_Adapter_LongPd_Common.SelectCommand.Transaction = transactCommon

			transactPCode = m_Adapter_LongPd_PCode.SelectCommand.Connection.BeginTransaction()
			m_Adapter_LongPd_PCode.SelectCommand.Transaction = transactPCode

			transactFilter = m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.BeginTransaction()
			m_Adapter_LongPd_PCodeFilter.SelectCommand.Transaction = transactFilter

			tcls_DB.TransactStart()

			' 更新処理
			Using cbWk As New SqlClient.SqlCommandBuilder()
				' アダプタをセットして更新
				cbWk.DataAdapter = m_Adapter_LongPd_Common
				m_Adapter_LongPd_Common.Update(m_DS_LongPd_Common)

				cbWk.DataAdapter = m_Adapter_LongPd_PCode
				m_Adapter_LongPd_PCode.Update(m_DS_LongPd_PCode)

				cbWk.DataAdapter = m_Adapter_LongPd_PCodeFilter
				m_Adapter_LongPd_PCodeFilter.Update(m_DS_LongPd_PCodeFilter)
			End Using

			' 長周期欠陥_疵種設定はDELETE INSERTするので削除
			sbSql.Clear()
			sbSql.AppendLine("DELETE FROM " & DB_LONGPD_PCODE_FILTER_OUTPUT)
			If 0 > tcls_DB.DMLExecute(sbSql.ToString) Then
				' エラーならロールバックして終了
				tcls_DB.TransactRollback()
				bProcState = False
				Exit Try
			End If

			'周期欠陥_疵種設定定テーブルへデータを登録 
			For ii As Integer = 0 To m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count - 1
				With m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows(ii)
					If CInt(.Item("TypeEdasysID").ToString) = -1 Or
					   (CInt(.Item("TypeEdasysID").ToString) > 0 And CInt(.Item("GrEdasysID").ToString) > 0) Then

						'TYPEOUT登録SQL文作成						
						sbSql.Clear()
						sbSql.AppendFormat("INSERT INTO {0} VALUES (", DB_LONGPD_PCODE_FILTER_OUTPUT)
						sbSql.AppendLine()
						sbSql.AppendLine(String.Format(" {0}", .Item("pcode")))
						sbSql.AppendLine(String.Format(",{0}", .Item("filter")))
						sbSql.AppendLine(String.Format(",{0}", .Item("TypeEdasysID")))
						sbSql.AppendLine(String.Format(",{0}", .Item("GrEdasysID")))
						sbSql.AppendLine(")")

						'SQL実行
						If 0 > tcls_DB.DMLExecute(sbSql.ToString) Then
							' エラーならロールバックして終了
							tcls_DB.TransactRollback()
							Return False
						End If
					End If
				End With
			Next ii

			'コミット
			transactCommon.Commit()
			transactPCode.Commit()
			transactFilter.Commit()
			tcls_DB.TransactCommint()

			'統括機能へパラメータ変更通知
			modMain.Send_Main(CH_PARAM_LONGPD)                             '統括への通知を変更する事
			modMain.HardCopy(Me)

			bProcState = True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)

			'' エラーならロールバックして終了
			If Not transactCommon Is Nothing Then
				transactCommon.Rollback()
			End If

			If Not transactPCode Is Nothing Then
				transactPCode.Rollback()
			End If

			If Not transactFilter Is Nothing Then
				transactFilter.Rollback()
			End If

			If Not tcls_DB Is Nothing Then
				tcls_DB.TransactRollback()
			End If


			bProcState = False
		Finally
			'' 後始末
			If m_Adapter_LongPd_Common.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter_LongPd_Common.SelectCommand.Connection.Close()
			End If

			If m_Adapter_LongPd_PCode.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter_LongPd_PCode.SelectCommand.Connection.Close()
			End If

			If m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.Close()
			End If

			If Not transactCommon Is Nothing Then
				transactCommon.Dispose()
			End If

			If Not transactPCode Is Nothing Then
				transactPCode.Dispose()
			End If

			If Not transactFilter Is Nothing Then
				transactFilter.Dispose()
			End If

			If Not tcls_DB Is Nothing Then
				tcls_DB.Close()
				tcls_DB.Dispose()
			End If
		End Try

		Return bProcState
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ その他の関数
	'/ /////////////////////////////////////////////////////////////////////////////////

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 画面表示処理
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' DBデータを表示する (長周期欠陥検出_マスター設定)
	''' </summary>
	''' <remarks></remarks>
	Private Sub Db_Disp_Common()
		Dim nColIx As Integer                                               ' 列INDEX

		' 行表示
		dgvCommon.RowCount = EM_COMMON_GRID_ROW_ID.ROW_MAX                  ' 行数設定

		' 行タイトル設定
		For ii As Integer = 0 To EM_COMMON_GRID_ROW_ID.ROW_MAX - 1
			dgvCommon(EM_COMMON_GRID_COL_ID.emTxt_Name, ii).Value = m_strCommonRowHead(ii)
			dgvCommon(EM_COMMON_GRID_COL_ID.emTxt_Name, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
		Next ii

		' データ表示
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_COMMON_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("ID='{0}' ", ii + 1)            ' WHERE句に相当の条件生成
			Dim drow() As DataRow = m_DS_LongPd_Common.Tables(0).Select(strWhere)

			' 行毎の書式設定
			' 【ハフ変換】画像連結処理有無
			Using clm As New DataGridViewComboBoxCell
				Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
				listItem.Items.AddRange(m_strCmbExpandImageValue)
				listItem.MaxDropDownItems = listItem.Items.Count
				dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage) = clm
			End Using

			' CSV出力モード出力先フォルダ
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Style.Alignment = DataGridViewContentAlignment.MiddleLeft

			' データが無ければスキップ
			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If


			' 定周期実行間隔
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value = drow(0).Item("ExecInterval")

			' 定周期実行実績対象範囲
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value = drow(0).Item("InitialExec_OldLimit")

			' 【FFT】最大疵長さ
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value = drow(0).Item("MaxDefectLen")

			' 【FFT】サンプリング周期
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value = drow(0).Item("FFTFreq")

			' 【FFT】ピーク検出閾値
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value = drow(0).Item("FFTPeakThr")

			' 【ハフ変換】FFT周期倍率
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value = drow(0).Item("HoughConvertAccumulatorRasio")

			' 【ハフ変換】最小線分長倍率
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value = drow(0).Item("HoughConvertMinLineLengthRasio")

			' 【ハフ変換】ラインギャップ
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value = drow(0).Item("HoughConvertMaxLineGap")

			' 【ハフ変換】直線状判断余裕代
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value = drow(0).Item("HoughConvertOnlineThr")

			' 【ハフ変換】基準Tangent倍率 
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value = drow(0).Item("HoughConvertTangentThr")

			' 【ハフ変換】画像連結処理有無
			If m_strCmbExpandImageValue.Length - 1 < CInt(drow(0).Item("ExpandImage")) OrElse 0 > CInt(drow(0).Item("ExpandImage")) Then
				' 範囲外は0の値
				dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage).Value = m_strCmbExpandImageValue(0)
			Else
				dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage).Value = m_strCmbExpandImageValue(CInt(drow(0).Item("ExpandImage")))
			End If

			' CSV出力モード出力先フォルダ
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value = drow(0).Item("CsvDir")

			' CSV出力モード出力先ファイル保存日数
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value = drow(0).Item("CsvSaveDate")

			' 定周期処理ピッチ
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value = drow(0).Item("PeriodicExecInterval")
		Next ii

	End Sub


	''' <summary>
	''' DBデータを表示する (長周期欠陥検出_検査パターンコード別設定)
	''' </summary>
	''' <remarks></remarks>
	Private Sub Db_Disp_Pcode()
		Dim strWhere As String = String.Format("Pcode='{0}' ", m_KeyCode)            ' WHERE句に相当の条件生成
		Dim drow() As DataRow = m_DS_LongPd_PCode.Tables(0).Select(strWhere)

		If drow Is Nothing OrElse drow.Length <= 0 Then
			cmbEnable.SelectedIndex = 0
		Else
			Dim nEnable As Integer = CInt(drow(0).Item("Enable"))
			If 0 > nEnable Or m_strCmbLongPcodeEnable.Length - 1 < nEnable Then
				cmbEnable.SelectedIndex = 0
			Else
				cmbEnable.SelectedIndex = nEnable
			End If
		End If
	End Sub

	''' <summary>
	''' DBデータを表示する (長周期欠陥_疵種設定)
	''' </summary>
	Private Sub Db_Disp_PcodeFilterOutput()
		' 一括のテーブルなので、検査パターン毎の値のみ取り出し
		Try
			' 画面に表示
			dgvOutput.RowCount = m_DsType.Tables(0).Rows.Count

			' データをセット
			For ii As Integer = 0 To dgvOutput.RowCount - 1
				dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_No, ii).Value = ii + 1                              ' No
				dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_Name, ii).Value = m_DsType.Tables(0).Rows(ii).Item("Name").ToString
				Dim nTypeEdasysId As Integer = CInt(m_DsType.Tables(0).Rows(ii).Item("EdasysID"))
				dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_EdasysId, ii).Value = nTypeEdasysId

				For nFilter As Integer = 1 To MAX_LONGPD_FILTER
					Dim strWhere As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID={2}", m_KeyCode, nFilter, nTypeEdasysId)
					Dim drow() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhere)                ' EdasysIDが1以上のデータを    0～64行

					If 0 < drow.Length Then
						dgvOutput(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, ii).Value = CInt(drow(0).Item("GrEdasysID"))
					Else
						' データ初期化						
						dgvOutput(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, ii).Value = 0
					End If
				Next nFilter
			Next ii

			'その他dgvの設定
			dgvOutputEtc.RowCount = 1                                                                             ' 1行固定
			dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emTxt_No, 0).Value = ""                                            ' No 空白
			dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emTxt_Name, 0).Value = "上記以外"                                  ' 疵種

			For nFilter As Integer = 1 To MAX_LONGPD_FILTER
				Dim strWhereEtc As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID=-1", m_KeyCode, nFilter)
				Dim drowEtc() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhereEtc)

				' その他行の表示データがあるか
				If drowEtc.Length > 0 Then
					dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, 0).Value = CInt(drowEtc(0).Item("GrEdasysID"))
				Else
					' 表示データが無い行は初期化する
					dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, 0).Value = 0
				End If
			Next nFilter

			' カーソルスタート位置は入力エリアの先頭
			dgvOutput(1, 0).Selected = True
			dgvOutputEtc(1, 0).Selected = True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " 表示エラー [" & ex.Message & "] " & Me.Text)
			Return
		End Try
	End Sub

	''' <summary>
	''' DBデータを表示する (長周期欠陥検出_検査パターン×フィルター設定)
	''' </summary>
	''' <remarks></remarks>
	Private Sub Db_Disp_PcodeFilter()
		Dim nColIx As Integer                                               ' 列INDEX

		' 行表示
		dgvFilter.RowCount = EM_FILTER_GRID_ROW_ID.ROW_MAX                  ' 行数設定

		' 行タイトル設定
		For ii As Integer = 0 To EM_FILTER_GRID_ROW_ID.ROW_MAX - 1
			dgvFilter(EM_FILTER_GRID_COL_ID.emTxt_Name, ii).Value = m_strFilterRowHead(ii)
			'dgvFilter(EM_FILTER_GRID_COL_ID.emTxt_Name, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
		Next ii

		' データ表示
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_FILTER_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("Pcode='{0}' and Filter={1} ", m_KeyCode, ii + 1)            ' WHERE句に相当の条件生成
			Dim drow() As DataRow = m_DS_LongPd_PCodeFilter.Tables(0).Select(strWhere)

			' 行毎の書式設定

			' フィルター名称
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Style.Alignment = DataGridViewContentAlignment.MiddleLeft

			' 有効／無効
			Using clm As New DataGridViewComboBoxCell
				Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
				listItem.Items.AddRange(m_strCmbFilterEnableValue)
				listItem.MaxDropDownItems = listItem.Items.Count
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable) = clm
			End Using

			' データが無ければスキップ
			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If

			' フィルター名称
			If drow(0).Item("Name") Is DBNull.Value Then
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value = ""
			Else
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value = drow(0).Item("Name")
			End If


			' 有効／無効
			If m_strCmbFilterEnableValue.Length - 1 < CInt(drow(0).Item("Enable")) OrElse 0 > CInt(drow(0).Item("Enable")) Then
				' 範囲外は0の値
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable).Value = m_strCmbFilterEnableValue(0)
			Else
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable).Value = m_strCmbFilterEnableValue(CInt(drow(0).Item("Enable")))
			End If

			' コイル先端マスク量
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value = drow(0).Item("MaskFront")

			' コイル尾端マスク量
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value = drow(0).Item("MaskTail")

			' 左エッジマスク量
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value = drow(0).Item("MaskLeft")

			' 右エッジマスク量
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value = drow(0).Item("MaskRight")

			' 検出周期下限
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value = drow(0).Item("DetectPdMin")

			' 検出周期上限
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value = drow(0).Item("DetectPdMax")

			' 検出個数下限
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value = drow(0).Item("DefectNumMin")

			' 検出個数上限
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value = drow(0).Item("DefectNumMax")

			' 周期検出分割単位巾
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value = drow(0).Item("WidthDiv")

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
	''' タイトル列のセルが選択されたとき選択解除を行う 周期疵有無設定
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvTypeMscData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvOutput.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
		''タイトルが書かれているセルなら、選択解除する
		'If dgv.CurrentCell.ColumnIndex <= EM_DATACOLUMN_GRID_ID.emTxt_Name Then
		'    dgv.CurrentCell.Selected = False        ' 選択解除
		'End If
	End Sub

    ''' <summary>
    ''' セルの内容が変化した場合
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridViewのチェックボックス等押された場合 通常 セルを移動させないと確定にならない為の処置</remarks>
    Private Sub dgvTypeMscData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvOutput.CurrentCellDirtyStateChanged
        ' セルがコミットされていない変更を含む場合 コミットする
        CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
    End Sub

	''' <summary>
	''' データグリッドビュー設定 長周期欠陥検出マスター設定
	''' </summary>
	''' <param name="dgv">データグリッドビュー</param>
	''' <remarks></remarks>
	Private Overloads Sub dgvInitialize_Common(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 11, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			' 列定義
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'.ColumnHeadersHeight = 42                                                   ' ヘッダーの高さ
			.RowTemplate.Height = 38                                                    ' テンプレートの行高さを設定

		End With

		' 列タイトル
		dgvColSetText(dgv, "", 190, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
		dgv.Columns(EM_COMMON_GRID_COL_ID.emTxt_Name).DefaultCellStyle.WrapMode = DataGridViewTriState.True

		For ii As Integer = 1 To MX_COMMON_COLMAX - 1
			dgvColSetText(dgv, CStr(ii), 100, False, False,
						  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		Next ii
	End Sub

	''' <summary>
	''' データグリッドビュー設定 長周期欠陥検出_検査パターン×フィルター設定設定
	''' </summary>
	''' <param name="dgv">データグリッドビュー</param>
	''' <remarks></remarks>
	Private Overloads Sub dgvInitialize_Filter(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 11, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			' 列定義
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'.ColumnHeadersHeight = 42                                                   ' ヘッダーの高さ
			.RowTemplate.Height = 38                                                    ' テンプレートの行高さを設定

		End With

		' 列タイトル
		dgvColSetText(dgv, "", 190, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
		dgv.Columns(EM_COMMON_GRID_COL_ID.emTxt_Name).DefaultCellStyle.WrapMode = DataGridViewTriState.True

		For ii As Integer = 1 To MX_FILTER_COLMAX - 1
			' グレード設定
			dgvColSetText(dgv, CStr(ii), 100, False, False,
						  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

		Next ii
	End Sub

	Private Overloads Sub dgvInitialize_Output(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal bDisptype As Boolean)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)                         ' 選択色無し
		Call dgv.DataGridViewDataExec(dgv)

		With dgv
			.ScrollBars = ScrollBars.Both                                 ' スクロールバーの表示

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
			.DefaultCellStyle.WrapMode = DataGridViewTriState.True
		End With

		'' 列定義

		'「上記以外」のグリッドの設定
		If bDisptype = False Then
			dgv.ColumnHeadersVisible = False        ' 列ヘッダー非表示
			dgv.ScrollBars = ScrollBars.None        ' スクロールバー非表示
		End If

		' NO
		dgvColSetText(dgv, "NO", 45, True, True,
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' 疵種
		If bDisptype = True Then

			' dgvEtc部品プロパティセット
			dgvColSetText(dgv, "疵種", 145, True, True,
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		Else
			' dgvEtc部品プロパティセット
			dgvColSetText(dgv, "", 145, True, True,
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		End If

		' 疵種EdasysID (非表示)
		dgvColSetText(dgv, "", 0, False, True,
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, ,, False)

		' グレード設定
		For ii As Integer = 1 To MAX_LONGPD_FILTER
			Using clm As New DataGridViewComboBoxColumn
				clm.HeaderText = CStr(ii)
				clm.Width = 100
				clm.SortMode = DataGridViewColumnSortMode.NotSortable
				clm.Frozen = False
				clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
				clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
				clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
				clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
				clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
				clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
				clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
				dgv.Columns.Add(clm)
			End Using
		Next ii

	End Sub


	''' <summary>
	''' タイトル列のセルが選択されたとき選択解除を行う 周期疵設定値
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvPeriodData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvFilter.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
		''タイトルが書かれているセルなら、選択解除する
		'If dgv.CurrentCell.ColumnIndex <= EM_PERIOD_GRID_COL_ID.emTxt_Title Then
		'    dgv.CurrentCell.Selected = False        ' 選択解除
		'End If
	End Sub

    ''' <summary>
    ''' セルの内容が変化した場合 周期疵設定値
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridViewのチェックボックス等押された場合 通常 セルを移動させないと確定にならない為の処置</remarks>
    Private Sub dgvPeriodData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvFilter.CurrentCellDirtyStateChanged
		' セルがコミットされていない変更を含む場合 コミットする
		' CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
	End Sub

	''' <summary>
	''' データグリッドビュー設定 全解除
	''' </summary>
	''' <param name="dgv">データグリッドビュー</param>
	''' <remarks></remarks>
	Private Overloads Sub dgvInitialize_SelectBtn(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)                            ' 選択色無し

		With dgv
			.RowCount = 0

			.ScrollBars = ScrollBars.None                                   ' スクロールバーの表示

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

		' データ部と列数をあわせるので、先頭にダミー列を追加する。
		' ダミー1
		dgvColSetText(dgv, "Dummy1", 40, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

		' ダミー2
		dgvColSetText(dgv, "Dummy2", 40, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

		' ダミー3
		dgvColSetText(dgv, "Dummy3", 40, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)


		'ここから本来のボタン設定
		For ii As Integer = EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start To MX_OUTPUT_COLMAX - 1
			' 全解除
			' 全解除ボタン
			dgvColSetButton(dgv, m_strBtnName(EM_SELECT_MODE.emCancel), 100, False, False,
								DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
								New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, m_strBtnName(EM_SELECT_MODE.emCancel))
			dgv.Columns(ii).DefaultCellStyle().BackColor = Color.AliceBlue              ' 背景
			dgv.Columns(ii).DefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 選択

		Next ii
	End Sub

	''' <summary>
	''' 更新ボタンの有効/無効
	''' </summary>
	''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
	''' <remarks></remarks>
	Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

		btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン
		pnlItem.Enabled = bPassResult                   ' コピー用リストの反映ボタン
	End Sub

	''' <summary>
	''' dgvTypeMscDataの横スクロールが変更されたときに その他dgvをスクロールさせる
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks>項目が増えたとき用</remarks>
	Private Sub dgvCommon_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvCommon.Scroll, dgvOutput.Scroll, dgvFilter.Scroll
		' スクロールイベントが水平スクロールなら
		If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
			' スクロール位置を合わせる
			Dim nScollingOffset As Integer = 0
			If sender Is dgvCommon Then
				nScollingOffset = dgvCommon.HorizontalScrollingOffset
				dgvOutput.HorizontalScrollingOffset = nScollingOffset
				dgvFilter.HorizontalScrollingOffset = nScollingOffset
			ElseIf sender Is dgvOutput Then
				nScollingOffset = dgvOutput.HorizontalScrollingOffset
				dgvCommon.HorizontalScrollingOffset = nScollingOffset
				dgvFilter.HorizontalScrollingOffset = nScollingOffset
			Else
				nScollingOffset = dgvFilter.HorizontalScrollingOffset
				dgvCommon.HorizontalScrollingOffset = nScollingOffset
				dgvOutput.HorizontalScrollingOffset = nScollingOffset
			End If
			dgvOutputEtc.HorizontalScrollingOffset = nScollingOffset
			dgvOutputBtnClear.HorizontalScrollingOffset = nScollingOffset
		End If
	End Sub


	''' <summary>
	''' 検査パターンコードの選択変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub cmbPCode_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbPCode.SelectedIndexChanged
		' 今の選択と主キーが同じ場合は無視
		If m_KeyCode = cmbPCode.Text Then Exit Sub

		'' 画面のデータを非接続型テーブルにセット
		' 初期画面表示時には、データ無しの為、画面のデータを取り込まないよ
		If m_KeyCode <> "" Then
			' 入力値チェック
			If Not Data_Check() Then
				' 入力異常時は、選択位置も戻す
				cmbPCode.Text = m_KeyCode
				Exit Sub
			End If

			' 変更前のキーに対し 画面データ取り込み
			Data_Get()
		End If

		' 選択変更に伴い ラベル値変更
		MyBase.SelectComboBoxItem(cmbPCode.SelectedIndex, cmbPCode, lblPCode)

		' 画面にパターン別の値を表示
		m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
		Data_Disp()
	End Sub

	''' <summary>
	''' 周期判定処理コンボの選択変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub cmbEnable_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbEnable.SelectedIndexChanged
		If cmbEnable.Items.Count = 0 Then
			Exit Sub
		End If

		If cmbEnable.Text = m_strCmbLongPcodeEnable(0) Then
			cmbEnable.BackColor = Color.LightGray
		Else
			cmbEnable.BackColor = Color.PeachPuff
		End If
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ アップコード 関連
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 反映ボタンクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click
		'''' 条件チェック
		'コピー元
		Dim strFromKey As String = ""                                    'コピー元キー 
		If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '元
		strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
		'コピー先
		Dim strToKey As String = MyBase.GetUpCodeItem_To(False)              'コピー先キー
		If strToKey.Length = 0 Then Exit Sub

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		frmMsg.SetSelectedFNo(0)

		Try

			' 登録しますか？
			frmMsg.Text = "条件確認"
			frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

			'''' アップデート 可変行のためUPDATE文は使えないので、デリートインサートしている
			If DB_UPDATE_UpCodeItem(strFromKey, strToKey) Then
				frmMsg.Text = "反映処理"
				frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
			Else
				frmMsg.Text = "反映処理"
				frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
			End If

			'''' データの再取得
			If Not DB_Select() Then
				' DB 失敗なので、画面強制終了
				frmMsg.Text = "異常"
				frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
				modMain.TaskEnd()                       '終了
			End If

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' 取得データ初期化
		Data_Init()

		'''' 画面再表示
		Data_Disp()
	End Sub

	''' <summary>
	''' コピー元のpcodeからコピー先のpcodeへデータをデリートインサートする
	''' </summary>
	''' <param name="strFromkey">コピー元キー</param>
	''' <param name="strToKey">コピー先のキーが入った文字列</param>
	''' <returns>正常/異常</returns>
	''' <remarks></remarks>
	Private Function DB_UPDATE_UpCodeItem(ByVal strFromkey As String, ByVal strToKey As String) As Boolean

		Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim strSQL As String = ""
		Dim strPcode() As String                                ' コピー先のpcode
		Dim strErrMsg As String = ""

		' 長周期欠陥検出_検査パターンコード別設定
		'''' SQL文の生成
		strSQL = "UPDATE " & DB_LONGPD_PCODE & " SET "
		' 可変列
		strSQL &= tClass_SQLServer.sqlColumnUpdate("Enable", "b")

		tClass_SQLServer.sqlColumnTrim(strSQL)      ' おまじない 必ず書いてね

		' From句
		strSQL &= "FROM " & DB_LONGPD_PCODE & " a "
		strSQL &= ", (SELECT * FROM " & DB_LONGPD_PCODE
		strSQL &= " WHERE pcode='" & strFromkey & "') b "

		' WHERE句
		strSQL &= "WHERE  a.pcode IN(" & strToKey & ")"

		' SQL文実行
		If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE & " 更新エラー [" & strErrMsg & "] " & Me.Text)
			Return False
		End If

		' 長周期欠陥検出_検査パターン×フィルター設定
		'''' SQL文の生成
		strSQL = "UPDATE " & DB_LONGPD_PCODE_FILTER & " SET "
		' 可変列
		strSQL &= tClass_SQLServer.sqlColumnUpdate("Name", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("Enable", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskFront", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskTail", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskLeft", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskRight", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DetectPdMin", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DetectPdMax", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DefectNumMin", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DefectNumMax", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("WidthDiv", "b")

		tClass_SQLServer.sqlColumnTrim(strSQL)      ' おまじない 必ず書いてね

		' From句
		strSQL &= "FROM " & DB_LONGPD_PCODE_FILTER & " a "
		strSQL &= ", (SELECT * FROM " & DB_LONGPD_PCODE_FILTER
		strSQL &= " WHERE pcode='" & strFromkey & "') b "

		' WHERE句
		strSQL &= "WHERE a.Filter = b.Filter AND a.pcode IN(" & strToKey & ")"

		' SQL文実行
		If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER & " 更新エラー [" & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'カンマ区切りのpcodeを配列に変換
		strPcode = Split(strToKey, ",")

		'コピー元のデータを取得
		strSQL = "SELECT pcode"
		strSQL &= ", Filter"
		strSQL &= ", TypeEdasysID"
		strSQL &= ", GrEdasysID"
		strSQL &= " FROM " & DB_LONGPD_PCODE_FILTER_OUTPUT
		strSQL &= " WHERE pcode='" & strFromkey & "'"

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCodeFilterOutput, m_Adapter_LongPd_PCodeFilterOutput, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		Try
			'''' デリートインサート
			If Not tcls_DB.Open() Then Return False

			'トランザクション開始
			tcls_DB.TransactStart()

			' レコードの削除
			Dim strSqlDel As String     ' レコード削除用sql
			strSqlDel = "DELETE FROM " & DB_LONGPD_PCODE_FILTER_OUTPUT & " WHERE pcode IN(" & strToKey & ")"

			If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False

			' レコードの挿入 コピー先pcodeの個数だけ繰り返し
			For ii As Integer = 0 To strPcode.Length - 1
				' コピー元pcodeのレコード数だけ繰り返し挿入
				For kk As Integer = 0 To m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count - 1
					With m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows(kk)

						'OUTPUT_PCODE登録SQL文作成
						strSQL = ""
						strSQL &= "INSERT INTO " & DB_LONGPD_PCODE_FILTER_OUTPUT & " VALUES ("
						strSQL &= strPcode(ii) & ","
						strSQL &= .Item("filter").ToString & ","
						strSQL &= .Item("TypeEdasysID").ToString & ","
						strSQL &= .Item("GrEdasysID").ToString & ")"

						'SQL実行
						If 0 > tcls_DB.DMLExecute(strSQL) Then
							' エラーならロールバックして終了
							tcls_DB.TransactRollback()
							Return False
						End If

					End With
				Next kk
			Next ii

			'コミット
			tcls_DB.TransactCommint()

			'統括機能へパラメータ変更通知
			modMain.Send_Main(CH_PARAM_LONGPD)
			modMain.HardCopy(Me)
			Return True
		Catch ex As Exception
			' エラーならロールバックして終了
			tcls_DB.TransactRollback()
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " データコピーエラー [" & ex.Message & "] " & Me.Text)
			Return False
		Finally
			tcls_DB.Close()
			tcls_DB.Dispose()
		End Try
	End Function



End Class
