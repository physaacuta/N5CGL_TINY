'*********************************************************************************
' 疵検定数設定画面クラス
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmCommonMaster

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
	Private m_AdapterGrade As New SqlClient.SqlDataAdapter          ' アダプター グレードマスター

	Private m_DS As New DataSet                                     ' 非接続型テーブル
	Private m_DsGrade As New DataSet                                ' 非接続型テーブル グレードマスター コンボボックス用

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Const m_PtnNsu As Integer = MAX_PCODE

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メニューボタン
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ファンクション切替
	''' </summary>
	''' <param name="FncNo">ファンクションNo (0オリジン)</param>
	''' <param name="enable">ファンクションボタンの有効無効状態</param>
	''' <param name="onKey">同時に押されているキー</param>
	''' <remarks></remarks>
	Private Sub frmCommonMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' ファンクションボタン有効項目のみ
		If Not enable Then Exit Sub

		'' ファンクション切替
		Select Case FncNo                       ' ※0オリジン
			Case 6                              ' F07 画面印刷
				Call PrintOut()
			Case 7                              ' F08 画面保存
				Call HardCopy(IMG_PATH, "定数設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
			Case 8
				modMain.ChgProject()            ' F09 KA_PARCOLORへ切替処理
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
	Private Sub frmCommonMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' いろいろ開放処理
		m_DS.Dispose()
		If Not m_Adapter Is Nothing Then
			m_Adapter.Dispose()
			m_Adapter = Nothing
		End If

		m_DsGrade.Dispose()
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
	''' <remarks></remarks>
	Private Sub frmCommonMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		modMain.SetListMenu(Me)                                     ' メニューリスト生成
		modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
		MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmCommonMaster          ' 疵検定数設定画面呼出

		'タイトル設定
		Call SetTitleName(Me)

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		' このフォームで必要なデータを取得する
		If Not DB_Select() Then
			' DB 失敗なので、画面強制終了
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "異常"
			frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '終了
		End If

		' ここの設定
		nudInitialize()

		' 画面に値を表示        
		If Not Data_Disp() Then
			' 画面表示失敗なので、強制終了
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "異常"
			frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '終了
		End If

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

	End Sub

	''' <summary>
	''' ディスポーズ
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmCommonMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

		'現在の表示中の画面が自分の画面ならアプリケーションを終了
		If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

	End Sub

	''' <summary>
	''' テキストボックスなどの初期化
	''' </summary>
	''' <remarks></remarks>
	Private Sub nudInitialize()
		' グループボックスの表示名称を変更
		gupDispCount.Text = "デフォルト表示件数"

		' ログ表示件数の初期設定
		nudDispCount.ImeMode = Windows.Forms.ImeMode.Disable        ' IMEモード使用不可
		nudDispCount.Increment = 1                                  ' 増減量
		nudDispCount.Maximum = 999999                               ' 最大値
		nudDispCount.Minimum = 1                                    ' 最小値

		' 探傷画面
		' 探傷画面表示区間小
		nudDispSmall.ImeMode = Windows.Forms.ImeMode.Disable        ' IMEモード使用不可
		nudDispSmall.Increment = MAP_REPRES_MIN                     ' 増減量
		nudDispSmall.Maximum = 1000                                 ' 最大値
		nudDispSmall.Minimum = MAP_REPRES_MIN                       ' 最小値

		' 探傷画面表示区間大
		nudDispBig.ImeMode = Windows.Forms.ImeMode.Disable          ' IMEモード使用不可
		nudDispBig.Increment = MAP_REPRES_MIN                       ' 増減量
		nudDispBig.Maximum = 1000                                   ' 最大値
		nudDispBig.Minimum = MAP_REPRES_MIN                         ' 最小値

		' ウォーマ材指定位置の初期設定
		For ii As Integer = 0 To m_PtnNsu - 1
			cmbWormPos.Items.Add(ii + 1)
		Next ii
		cmbWormPos.DropDownStyle = ComboBoxStyle.DropDownList

		' Jpeg切り出し画像品質の初期設定
		nudJpegGazo.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可
		nudJpegGazo.Increment = 1                                   ' 増減量
		nudJpegGazo.Maximum = 90                                    ' 最大値
		nudJpegGazo.Minimum = 1                                     ' 最小値

		' カメラ温度閾値の初期設定
		nudCamTemp.ImeMode = Windows.Forms.ImeMode.Disable          ' IMEモード使用不可
		nudCamTemp.Increment = 1                                    ' 増減量
		nudCamTemp.Maximum = 99                                     ' 最大値
		nudCamTemp.Minimum = 1                                      ' 最小値

		'エッジ整合性_実施種別
		cmbEdgType.Items.Clear()
		cmbEdgType.Items.Add("無し")
		cmbEdgType.Items.Add("整合性判定のみ")
		'cmbEdgType.Items.Add("整合性判定＋エッジ範囲指定")

		' 検出範囲
		txtEdgeKenHan.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可
		' リカバリー検出範囲
		txtEdgeReCov.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可
		' 板幅許容誤差
		txtEdgeHabaGoso.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可
		' 前フレームとの許容誤差
		txtEdgeFrmGoso.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可

		' 溶接後不感帯の初期設定
		nudEdgeFukantai.ImeMode = Windows.Forms.ImeMode.Disable     ' IMEモード使用不可
		nudEdgeFukantai.Increment = 1                               ' 増減量
		nudEdgeFukantai.Maximum = 1000                              ' 最大値
		nudEdgeFukantai.Minimum = 0                                 ' 最小値

		' エッジ検出異常FF送信の初期設定
		nudEdgeFF.ImeMode = Windows.Forms.ImeMode.Disable           ' IMEモード使用不可
		nudEdgeFF.Increment = 1                                     ' 増減量
		nudEdgeFF.Maximum = 9999                                    ' 最大値
		nudEdgeFF.Minimum = 0                                       ' 最小値

		'エッジ検出_実施種別
		cmbEdgOutType.Items.Clear()
		cmbEdgOutType.Items.Add("無し")
		cmbEdgOutType.Items.Add("統括エッジ範囲指定")

		'エッジ検出_検出範囲
		txtEdgeOutKenHan.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可

		' 疵マップ疵個数閾値の初期設定 
		' 軽
		nudKizumapTempKei.ImeMode = Windows.Forms.ImeMode.Disable        ' IMEモード使用不可
		nudKizumapTempKei.Increment = 1                                  ' 増減量
		nudKizumapTempKei.Maximum = 99                                   ' 最大値
		nudKizumapTempKei.Minimum = 1                                    ' 最小値
		' 中
		nudKizumapTempChu.ImeMode = Windows.Forms.ImeMode.Disable        ' IMEモード使用不可
		nudKizumapTempChu.Increment = 1                                  ' 増減量
		nudKizumapTempChu.Maximum = 99                                   ' 最大値
		nudKizumapTempChu.Minimum = 1                                    ' 最小値
		' 重
		nudKizumapTempZyu.ImeMode = Windows.Forms.ImeMode.Disable        ' IMEモード使用不可
		nudKizumapTempZyu.Increment = 1                                  ' 増減量
		nudKizumapTempZyu.Maximum = 99                                   ' 最大値
		nudKizumapTempZyu.Minimum = 1                                    ' 最小値

		' 検査基点_上流カメラの初期設定
		nudCamera.ImeMode = Windows.Forms.ImeMode.Disable                ' IMEモード使用不可
		nudCamera.Increment = 1                                          ' 増減量
		nudCamera.Maximum = 999999                                       ' 最大値
		nudCamera.Minimum = 1                                            ' 最小値

		' 検査基点_SPMの初期設定
		nudSpm.ImeMode = Windows.Forms.ImeMode.Disable                   ' IMEモード使用不可
		nudSpm.Increment = 1                                             ' 増減量
		nudSpm.Maximum = 999999                                          ' 最大値
		nudSpm.Minimum = 1                                               ' 最小値

		' 表示基点_裏検査台の初期設定
		nudExaminingTableB.ImeMode = Windows.Forms.ImeMode.Disable       ' IMEモード使用不可
		nudExaminingTableB.Increment = 1                                 ' 増減量
		nudExaminingTableB.Maximum = 999999                              ' 最大値
		nudExaminingTableB.Minimum = 1                                   ' 最小値

		' 表示基点_表検査台の初期設定
		nudExaminingTableT.ImeMode = Windows.Forms.ImeMode.Disable       ' IMEモード使用不可
		nudExaminingTableT.Increment = 1                                 ' 増減量
		nudExaminingTableT.Maximum = 999999                              ' 最大値
		nudExaminingTableT.Minimum = 1                                   ' 最小値

		' 表示基点_シャーの初期設定
		nudCutPos.ImeMode = Windows.Forms.ImeMode.Disable                ' IMEモード使用不可
		nudCutPos.Increment = 1                                          ' 増減量
		nudCutPos.Maximum = 999999                                       ' 最大値
		nudCutPos.Minimum = 1                                            ' 最小値

		' 全長画像_保存_R
		cmbImageSaveR.Items.Add("無し")
		cmbImageSaveR.Items.Add("有り")
		cmbImageSaveR.DropDownStyle = ComboBoxStyle.DropDownList
		' 全長画像_保存_G
		cmbImageSaveG.Items.Add("無し")
		cmbImageSaveG.Items.Add("有り")
		cmbImageSaveG.DropDownStyle = ComboBoxStyle.DropDownList
		' 全長画像_保存_B
		cmbImageSaveB.Items.Add("無し")
		cmbImageSaveB.Items.Add("有り")
		cmbImageSaveB.DropDownStyle = ComboBoxStyle.DropDownList
		' 全長画像_保存_差
		cmbImageSaveRB.Items.Add("無し")
		cmbImageSaveRB.Items.Add("有り")
		cmbImageSaveRB.DropDownStyle = ComboBoxStyle.DropDownList

		' PLG距離比較異常上限値
		txtHikakuPlgHigh.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可
		' PLG距離表示異常上限値
		txtHyoziPlgHigh.ImeMode = Windows.Forms.ImeMode.Disable          ' IMEモード使用不可
		' プロコンコイル長比較異常上限値
		txtProcomHigh.ImeMode = Windows.Forms.ImeMode.Disable            ' IMEモード使用不可

		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		'-----------------------
		' 長周期欠陥ガイダンス
		'-----------------------

		' ガイダンス有無
		cmbLongPd_GuidanceAvailable.Items.Clear()
		cmbLongPd_GuidanceAvailable.Items.Add("無し")
		cmbLongPd_GuidanceAvailable.Items.Add("有り")

		' 確認実施間隔
		nudLongPd_GuidanceCheckInterval.ImeMode = Windows.Forms.ImeMode.Disable       ' IMEモード使用不可
		nudLongPd_GuidanceCheckInterval.Increment = 1                                 ' 増減量
		nudLongPd_GuidanceCheckInterval.Maximum = 999999                              ' 最大値
		nudLongPd_GuidanceCheckInterval.Minimum = 1                                   ' 最小値

		' 炉チカ判定範囲 From
		nudLongPd_NearFurnaceRangeFrom.ImeMode = Windows.Forms.ImeMode.Disable       ' IMEモード使用不可
		nudLongPd_NearFurnaceRangeFrom.Increment = 1                                 ' 増減量
		nudLongPd_NearFurnaceRangeFrom.Maximum = COIL_LEN_MAX * 1000                 ' 最大値
		nudLongPd_NearFurnaceRangeFrom.Minimum = 1                                   ' 最小値

		' 炉チカ判定範囲 To
		nudLongPd_NearFurnaceRangeTo.ImeMode = Windows.Forms.ImeMode.Disable         ' IMEモード使用不可
		nudLongPd_NearFurnaceRangeTo.Increment = 1                                   ' 増減量
		nudLongPd_NearFurnaceRangeTo.Maximum = COIL_LEN_MAX * 1000                   ' 最大値
		nudLongPd_NearFurnaceRangeTo.Minimum = 1                                     ' 最小値

		' 周期ピッチ画面表示用公差
		nudLongPd_PeriodicPitchTolerance.ImeMode = Windows.Forms.ImeMode.Disable     ' IMEモード使用不可
		nudLongPd_PeriodicPitchTolerance.Increment = 1                               ' 増減量
		nudLongPd_PeriodicPitchTolerance.Maximum = COIL_LEN_MAX * 1000               ' 最大値
		nudLongPd_PeriodicPitchTolerance.Minimum = 1                                 ' 最小値
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	End Sub

	''' <summary>
	''' このフォームで必要なデータを取得する
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function DB_Select() As Boolean
		Dim strSQL As String = ""
		Dim strErrMsg As String = ""

		' SQL文生成
		strSQL = "SELECT LINEID"
		strSQL &= ",RAS正常色"
		strSQL &= ",RAS異常色"
		strSQL &= ",RAS注意色"
		strSQL &= ",LOG表示件数"
		strSQL &= ",LOGイベント色"
		strSQL &= ",LOG重故障色"
		strSQL &= ",LOG軽故障色"
		strSQL &= ",探傷画面表示区間小"
		strSQL &= ",探傷画面表示区間大"
		strSQL &= ",ウォーマ材指定位置"
		strSQL &= ",Jpeg切り出し画像品質"
		strSQL &= ",カメラ温度閾値"
		strSQL &= ",エッジ整合性_実施種別"
		strSQL &= ",エッジ整合性_検出範囲"
		strSQL &= ",エッジ整合性_リカバリー検出範囲"
		strSQL &= ",エッジ整合性_板幅許容誤差"
		strSQL &= ",エッジ整合性_前フレームとの許容誤差"
		strSQL &= ",エッジ整合性_溶接後不感帯"
		strSQL &= ",エッジ整合性_エッジ検出異常FF送信"
		strSQL &= ",エッジ検出_実施種別"
		strSQL &= ",エッジ検出_検出範囲"
		strSQL &= ",重複疵マップ背景色_重"
		strSQL &= ",重複疵マップ背景色_中"
		strSQL &= ",重複疵マップ背景色_軽"
		strSQL &= ",重複疵マップ疵個数閾値_重"
		strSQL &= ",重複疵マップ疵個数閾値_中"
		strSQL &= ",重複疵マップ疵個数閾値_軽"
		strSQL &= ",設備長_検査基点_上流カメラ"
		strSQL &= ",設備長_検査基点_SPM"
		strSQL &= ",設備長_表示基点_裏検査台"
		strSQL &= ",設備長_表示基点_表検査台"
		strSQL &= ",設備長_表示基点_シャー"
		strSQL &= ",全長画像_保存_R"
		strSQL &= ",全長画像_保存_G"
		strSQL &= ",全長画像_保存_B"
		strSQL &= ",全長画像_保存_差"
		strSQL &= ",PLG距離比較異常上限値"
		strSQL &= ",PLG距離表示異常上限値"
		strSQL &= ",プロコンコイル長比較異常上限値"
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		strSQL &= ",LongPd_GuidanceAvailable"
		strSQL &= ",LongPd_GuidanceCheckInterval"
		strSQL &= ",LongPd_NearFurnaceRangeFrom"
		strSQL &= ",LongPd_NearFurnaceRangeTo"
		strSQL &= ",LongPd_PeriodicPitchTolerance"
		strSQL &= ",LongPd_GuidanceForeColor"
		strSQL &= ",LongPd_GuidanceBackColor"
		strSQL &= ",LongPd_GuidanceMessage"
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

		strSQL &= " FROM " & DB_COMMON_MASTER

		'SQL文実行
		If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		' SQL文生成 グレードは優先度で昇順
		strSQL = "SELECT EdasysID, Name ,マップ表示色 "
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
	''' DBの情報を画面に表示
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function Data_Disp() As Boolean
		Dim drow() As DataRow     ' データ行

		' 主キーを指定して取り出してるので、1件しかないはず
		drow = m_DS.Tables(0).Select()

		' 取得行が1行でないならエラー終了
		If drow.Length <> 1 Then Return False

		' 画面に表示
		lblNormalColor.BackColor = Color.FromArgb(CInt(drow(0).Item("RAS正常色")))              ' RAS正常色
		lblAbnormalColor.BackColor = Color.FromArgb(CInt(drow(0).Item("RAS異常色")))            ' RAS異常色
		lblCautionColor.BackColor = Color.FromArgb(CInt(drow(0).Item("RAS注意色")))             ' RAS注意色
		nudDispCount.Text = CStr(drow(0).Item("LOG表示件数"))                                   ' LOG表示件数
		lblEventColor.ForeColor = Color.FromArgb(CInt(drow(0).Item("LOGイベント色")))           ' LOGイベント色
		lblZyuColor.ForeColor = Color.FromArgb(CInt(drow(0).Item("LOG重故障色")))               ' LOG重故障色
		lblKeiColor.ForeColor = Color.FromArgb(CInt(drow(0).Item("LOG軽故障色")))               ' LOG軽故障色
		nudDispSmall.Text = CStr(drow(0).Item("探傷画面表示区間小"))                            ' 探傷画面表示区間小
		nudDispBig.Text = CStr(drow(0).Item("探傷画面表示区間大"))                              ' 探傷画面表示区間大

		If CInt(drow(0).Item("ウォーマ材指定位置")) >= 1 And
		   CInt(drow(0).Item("ウォーマ材指定位置")) <= m_PtnNsu Then
			cmbWormPos.Text = CStr(drow(0).Item("ウォーマ材指定位置"))                          ' ウォーマ材指定位置
		End If

		nudJpegGazo.Text = CStr(drow(0).Item("Jpeg切り出し画像品質"))                           ' Jpeg切り出し画像品質
		nudCamTemp.Text = CStr(drow(0).Item("カメラ温度閾値"))                                  ' カメラ温度閾値
		Dim nEdgType As Integer = CInt(drow(0).Item("エッジ整合性_実施種別"))                   ' エッジ整合性_実施種別
		Select Case nEdgType
			Case 0 To 1
				cmbEdgType.SelectedIndex = nEdgType
			Case Else
				cmbEdgType.SelectedIndex = 0
		End Select
		txtEdgeKenHan.Text = CStr(drow(0).Item("エッジ整合性_検出範囲"))                        ' エッジ整合性_検出範囲
		txtEdgeReCov.Text = CStr(drow(0).Item("エッジ整合性_リカバリー検出範囲"))               ' エッジ整合性_リカバリー検出範囲
		txtEdgeHabaGoso.Text = CStr(drow(0).Item("エッジ整合性_板幅許容誤差"))                  ' エッジ整合性_板幅許容誤差
		txtEdgeFrmGoso.Text = CStr(drow(0).Item("エッジ整合性_前フレームとの許容誤差"))         ' エッジ整合性_前フレームとの許容誤差
		nudEdgeFukantai.Text = CStr(drow(0).Item("エッジ整合性_溶接後不感帯"))                  ' エッジ整合性_溶接後不感帯
		nudEdgeFF.Text = CStr(drow(0).Item("エッジ整合性_エッジ検出異常FF送信"))                ' エッジ整合性_エッジ検出異常FF送信

		Dim nEdgOutType As Integer = CInt(drow(0).Item("エッジ検出_実施種別"))                   ' エッジ検出_実施種別
		Select Case nEdgOutType
			Case 0 To 1
				cmbEdgOutType.SelectedIndex = nEdgOutType
			Case Else
				cmbEdgOutType.SelectedIndex = 0
		End Select
		txtEdgeOutKenHan.Text = CStr(drow(0).Item("エッジ検出_検出範囲"))                        ' エッジ検出_検出範囲

		lblMapZyuColor.BackColor = Color.FromArgb(CInt(drow(0).Item("重複疵マップ背景色_重")))  ' 重複疵マップ背景色_重
		lblMapChuColor.BackColor = Color.FromArgb(CInt(drow(0).Item("重複疵マップ背景色_中")))  ' 重複疵マップ背景色_中
		lblMapKeiColor.BackColor = Color.FromArgb(CInt(drow(0).Item("重複疵マップ背景色_軽")))  ' 重複疵マップ背景色_軽

		nudKizumapTempZyu.Text = CStr(drow(0).Item("重複疵マップ疵個数閾値_重"))                ' 重複疵マップ疵個数閾値_重
		nudKizumapTempChu.Text = CStr(drow(0).Item("重複疵マップ疵個数閾値_中"))                ' 重複疵マップ疵個数閾値_中
		nudKizumapTempKei.Text = CStr(drow(0).Item("重複疵マップ疵個数閾値_軽"))                ' 重複疵マップ疵個数閾値_軽
		lblKzcntMiddleH.Text = nudKizumapTempZyu.Text                                           ' 重複疵マップ疵個数閾値_中 上限
		lblKzcntLightH.Text = nudKizumapTempChu.Text                                            ' 重複疵マップ疵個数閾値_軽 上限

		'基点距離設定
		nudCamera.Text = CStr(drow(0).Item("設備長_検査基点_上流カメラ"))                       ' 設備長_検査基点_上流カメラ
		nudSpm.Text = CStr(drow(0).Item("設備長_検査基点_SPM"))                                 ' 設備長_検査基点_SPM
		nudExaminingTableB.Text = CStr(drow(0).Item("設備長_表示基点_裏検査台"))                ' 設備長_表示基点_裏検査台
		nudExaminingTableT.Text = CStr(drow(0).Item("設備長_表示基点_表検査台"))                ' 設備長_表示基点_表検査台
		nudCutPos.Text = CStr(drow(0).Item("設備長_表示基点_シャー"))                           ' 設備長_表示基点_シャー

		' 全長画像
		If 1 = CInt(drow(0).Item("全長画像_保存_R")) Then
			cmbImageSaveR.Text = "有り"
		Else
			cmbImageSaveR.Text = "無し"
		End If
		If 1 = CInt(drow(0).Item("全長画像_保存_G")) Then
			cmbImageSaveG.Text = "有り"
		Else
			cmbImageSaveG.Text = "無し"
		End If
		If 1 = CInt(drow(0).Item("全長画像_保存_B")) Then
			cmbImageSaveB.Text = "有り"
		Else
			cmbImageSaveB.Text = "無し"
		End If
		If 1 = CInt(drow(0).Item("全長画像_保存_差")) Then
			cmbImageSaveRB.Text = "有り"
		Else
			cmbImageSaveRB.Text = "無し"
		End If

		txtHikakuPlgHigh.Text = CStr(drow(0).Item("PLG距離比較異常上限値"))                            ' PLG距離比較異常上限値
		txtHyoziPlgHigh.Text = CStr(drow(0).Item("PLG距離表示異常上限値"))                             ' PLG距離表示異常上限値
		txtProcomHigh.Text = CStr(drow(0).Item("プロコンコイル長比較異常上限値"))                      ' プロコンコイル長比較異常上限値

		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		' 長周期欠陥ガイダンス
		If 1 = CInt(drow(0).Item("LongPd_GuidanceAvailable")) Then
			cmbLongPd_GuidanceAvailable.Text = "有り"
		Else
			cmbLongPd_GuidanceAvailable.Text = "無し"
		End If

		nudLongPd_GuidanceCheckInterval.Text = CStr(drow(0).Item("LongPd_GuidanceCheckInterval"))
		nudLongPd_NearFurnaceRangeFrom.Text = CStr(drow(0).Item("LongPd_NearFurnaceRangeFrom"))
		nudLongPd_NearFurnaceRangeTo.Text = CStr(drow(0).Item("LongPd_NearFurnaceRangeTo"))
		nudLongPd_PeriodicPitchTolerance.Text = CStr(drow(0).Item("LongPd_PeriodicPitchTolerance"))


		txtLongPd_GuidanceMessage.Text = CStr(drow(0).Item("LongPd_GuidanceMessage"))
		lblLongPd_GuidanceMessagePreview.ForeColor = Color.FromArgb(CInt(drow(0).Item("LongPd_GuidanceForeColor")))
		lblLongPd_GuidanceMessagePreview.BackColor = Color.FromArgb(CInt(drow(0).Item("LongPd_GuidanceBackColor")))
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

		Return True
	End Function

	''' <summary>
	''' 確定処理
	''' </summary>
	''' <remarks></remarks>
	Private Sub subData_UpDate()

		' 入力値チェック
		If Not Data_Check() Then Exit Sub

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		frmMsg.SetSelectedFNo(0)

		Try

			' 登録しますか？
			frmMsg.Text = "条件確認"
			frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

			'画面のデータを取得
			Data_Get()

			' ホントに登録        
			If DB_UpDate() Then
				frmMsg.Text = "確定処理"
				frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
				If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
			Else
				frmMsg.Text = "確定処理"
				frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
			End If

			' 現在の内容で再表示
			Data_Disp()

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

	End Sub

	''' <summary>
	''' 入力値のチェック
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function Data_Check() As Boolean
		Dim strWk As String         ' 文字列ワーク
		Dim strMsg As String        ' メッセージワーク
		Dim nWk As Integer          ' int ワーク
		Dim nWkH As Integer         ' 疵個数閾値 重 ワーク
		Dim nWkM As Integer         ' 疵個数閾値 中 ワーク
		Dim nWkL As Integer         ' 疵個数閾値 軽 ワーク

		' ログ表示件数
		strWk = nudDispCount.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "デフォルト表示件数") <> 0 Then Return False
		nudDispCount.Text = strWk

		' 探傷画面表示区間小
		strWk = nudDispSmall.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 5, 1000, False, "探傷画面表示区間小") <> 0 Then Return False
		nudDispSmall.Text = strWk

		' 表示区間が最小代表疵間隔の倍数かチェックする。倍数でなければメッセージを出して終了
		If MAP_REPRES_MIN > 0 Then
			nWk = CInt(strWk) Mod MAP_REPRES_MIN
			If nWk > 0 Then
				strMsg = "【 探傷画面表示区間小 】" & vbCrLf
				strMsg &= "適切な値ではありません。" & vbCrLf
				strMsg &= "入力値を " & MAP_REPRES_MIN & " の倍数にして下さい。"
				MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
				Return False
			End If
		End If

		' 探傷画面表示区間大
		strWk = nudDispBig.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 5, 1000, False, "探傷画面表示区間大") <> 0 Then Return False
		nudDispBig.Text = strWk

		' 表示区間が最小代表疵間隔の倍数かチェックする。倍数でなければメッセージを出して終了
		If MAP_REPRES_MIN > 0 Then
			nWk = CInt(strWk) Mod MAP_REPRES_MIN
			If nWk > 0 Then
				strMsg = "【 探傷画面表示区間大 】" & vbCrLf
				strMsg &= "適切な値ではありません。" & vbCrLf
				strMsg &= "入力値を " & MAP_REPRES_MIN & " の倍数にして下さい。"
				MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
				Return False
			End If
		End If

		' ウォーマ材指定位置
		' コンボボックスなのでチェック無し

		' Jpeg切り出し画像品質
		strWk = nudJpegGazo.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 90, False, "Jpeg切り出し画像品質") <> 0 Then Return False
		nudJpegGazo.Text = strWk

		' カメラ温度閾値
		strWk = nudCamTemp.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "カメラ温度閾値") <> 0 Then Return False
		nudCamTemp.Text = strWk

		' エッジ整合性_検出範囲
		strWk = txtEdgeKenHan.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 1, 999.99, False, 2, "エッジ整合性_検出範囲") <> 0 Then Return False
		txtEdgeKenHan.Text = strWk

		' エッジ整合性_リカバリー検出範囲
		strWk = txtEdgeReCov.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 1, 999.99, False, 2, "エッジ整合性_リカバリー検出範囲") <> 0 Then Return False
		txtEdgeReCov.Text = strWk

		' エッジ整合性_板幅許容誤差
		strWk = txtEdgeHabaGoso.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 999.99, False, 2, "エッジ整合性_板幅許容誤差") <> 0 Then Return False
		txtEdgeHabaGoso.Text = strWk

		' エッジ整合性_前フレームとの許容誤差
		strWk = txtEdgeFrmGoso.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 1, 999.99, False, 2, "エッジ整合性_前フレームとの許容誤差") <> 0 Then Return False
		txtEdgeFrmGoso.Text = strWk

		' エッジ整合性_溶接後不感帯
		strWk = nudEdgeFukantai.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 1000, False, "エッジ整合性_溶接後不感帯") <> 0 Then Return False
		nudEdgeFukantai.Text = strWk

		' エッジ整合性_エッジ検出異常FF送信
		strWk = nudEdgeFF.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 9999, False, "エッジ整合性_エッジ検出異常FF送信") <> 0 Then Return False
		nudEdgeFF.Text = strWk

		' エッジ検出_検出範囲
		strWk = txtEdgeOutKenHan.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 1800.0, False, 6, "エッジ検出_検出範囲") <> 0 Then Return False
		txtEdgeOutKenHan.Text = strWk

		' 重複疵マップ疵個数閾値_重
		strWk = nudKizumapTempZyu.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "重複疵マップ疵個数閾値_重") <> 0 Then Return False
		nudKizumapTempZyu.Text = strWk

		' 重複疵マップ疵個数閾値_中
		strWk = nudKizumapTempChu.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "重複疵マップ疵個数閾値_中") <> 0 Then Return False
		nudKizumapTempChu.Text = strWk

		' 重複疵マップ疵個数閾値_軽
		strWk = nudKizumapTempKei.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "重複疵マップ疵個数閾値_軽") <> 0 Then Return False
		nudKizumapTempKei.Text = strWk

		' 判定閾値の「 重 > 中 > 軽 」論理チェック
		nWkH = CInt(nudKizumapTempZyu.Text)                 ' 疵個数閾値 重
		nWkM = CInt(nudKizumapTempChu.Text)                 ' 疵個数閾値 中
		nWkL = CInt(nudKizumapTempKei.Text)                 ' 疵個数閾値 軽
		If nWkH > nWkM AndAlso nWkM > nWkL Then
			' 判定閾値の「 重 > 中 > 軽 」論理チェック正常
		Else
			' 入力範囲異常
			strMsg = "【 重複疵マップ表示設定 疵個数閾値 】" & vbCrLf
			strMsg &= "適切な値ではありません。" & vbCrLf
			strMsg &= "疵個数の関係を「 重 ＞ 中 ＞ 軽 」にしてください。"
			MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
			Return False
		End If

		' 設備長_検査基点_上流カメラ
		strWk = nudCamera.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "設備長_検査基点_上流カメラ") <> 0 Then Return False
		nudCamera.Text = strWk

		' 設備長_検査基点_SPM
		strWk = nudSpm.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "設備長_検査基点_SPM") <> 0 Then Return False
		nudSpm.Text = strWk

		' 設備長_表示基点_裏検査台
		strWk = nudExaminingTableB.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "設備長_表示基点_裏検査台") <> 0 Then Return False
		nudExaminingTableB.Text = strWk

		' 設備長_表示基点_表検査台
		strWk = nudExaminingTableT.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "設備長_表示基点_表検査台") <> 0 Then Return False
		nudExaminingTableT.Text = strWk

		' 設備長_表示基点_シャー
		strWk = nudCutPos.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "設備長_表示基点_シャー") <> 0 Then Return False
		nudCutPos.Text = strWk

		' PLG距離比較異常上限値
		strWk = txtHikakuPlgHigh.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 100, False, 5, "コイル長比較異常上限値_比較用PLG") <> 0 Then Return False
		txtHikakuPlgHigh.Text = strWk

		' PLG距離表示異常上限値
		strWk = txtHyoziPlgHigh.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 100, False, 5, "コイル長比較異常上限値_表示用PLG") <> 0 Then Return False
		txtHyoziPlgHigh.Text = strWk

		' プロコンコイル長比較異常上限値
		strWk = txtProcomHigh.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 100, False, 5, "コイル長比較異常上限値_プロコン") <> 0 Then Return False
		txtProcomHigh.Text = strWk

		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>

		' 長周期欠陥ガイダンス 確認実施間隔
		strWk = nudLongPd_GuidanceCheckInterval.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_GuidanceCheckInterval.Minimum), CLng(nudLongPd_GuidanceCheckInterval.Maximum), False, "長周期欠陥ガイダンス_確認実施間隔") <> 0 Then Return False
		nudLongPd_GuidanceCheckInterval.Text = strWk

		' 長周期欠陥ガイダンス 炉チカ判定範囲 From
		strWk = nudLongPd_NearFurnaceRangeFrom.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_NearFurnaceRangeFrom.Minimum), CLng(nudLongPd_NearFurnaceRangeFrom.Maximum), False, "長周期欠陥ガイダンス_炉チカ判定範囲 From") <> 0 Then Return False
		nudLongPd_NearFurnaceRangeFrom.Text = strWk

		' 長周期欠陥ガイダンス 炉チカ判定範囲 To
		strWk = nudLongPd_NearFurnaceRangeTo.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_NearFurnaceRangeTo.Minimum), CLng(nudLongPd_NearFurnaceRangeTo.Maximum), False, "長周期欠陥ガイダンス_炉チカ判定範囲 To") <> 0 Then Return False
		nudLongPd_NearFurnaceRangeTo.Text = strWk

		' 長周期欠陥ガイダンス 周期ピッチ画面表示用公差
		strWk = nudLongPd_PeriodicPitchTolerance.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_PeriodicPitchTolerance.Minimum), CLng(nudLongPd_PeriodicPitchTolerance.Maximum), False, "長周期欠陥ガイダンス_周期ピッチ画面表示用公差") <> 0 Then Return False
		nudLongPd_PeriodicPitchTolerance.Text = strWk

		' 長周期欠陥ガイダンス ガイダンスメッセージ
		strWk = txtLongPd_GuidanceMessage.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "長周期欠陥ガイダンス_ガイダンスメッセージ") <> 0 Then Return False
		txtLongPd_GuidanceMessage.Text = strWk
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

		Return True
	End Function

	''' <summary>
	''' 画面に表示中のデータを非接続型テーブルにセット
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Get()
		Dim drow As DataRow     ' データ行

		' 主キーを指定して取り出されてるので一件しかない
		drow = m_DS.Tables(0).Select()(0)

		' セット
		drow.Item("RAS正常色") = lblNormalColor.BackColor.ToArgb                        ' RAS正常色
		drow.Item("RAS異常色") = lblAbnormalColor.BackColor.ToArgb                      ' RAS異常色
		drow.Item("RAS注意色") = lblCautionColor.BackColor.ToArgb                       ' RAS注意色
		drow.Item("LOG表示件数") = CInt(nudDispCount.Text)                              ' LOG表示件数
		drow.Item("LOGイベント色") = lblEventColor.ForeColor.ToArgb                     ' LOGイベント色
		drow.Item("LOG重故障色") = lblZyuColor.ForeColor.ToArgb                         ' LOG重故障色
		drow.Item("LOG軽故障色") = lblKeiColor.ForeColor.ToArgb                         ' LOG軽故障色
		drow.Item("探傷画面表示区間小") = CInt(nudDispSmall.Text)                       ' 探傷画面表示区間小
		drow.Item("探傷画面表示区間大") = CInt(nudDispBig.Text)                         ' 探傷画面表示区間大
		drow.Item("ウォーマ材指定位置") = CInt(cmbWormPos.Text)                         ' ウォーマ材指定位置
		drow.Item("Jpeg切り出し画像品質") = CInt(nudJpegGazo.Text)                      ' Jpeg切り出し画像品質
		drow.Item("カメラ温度閾値") = CInt(nudCamTemp.Text)                             ' カメラ温度閾値

		drow.Item("エッジ整合性_実施種別") = cmbEdgType.SelectedIndex                   ' エッジ整合性_実施種別
		drow.Item("エッジ整合性_検出範囲") = CDbl(txtEdgeKenHan.Text)                   ' エッジ整合性_検出範囲
		drow.Item("エッジ整合性_リカバリー検出範囲") = CDbl(txtEdgeReCov.Text)          ' エッジ整合性_リカバリー検出範囲
		drow.Item("エッジ整合性_板幅許容誤差") = CDbl(txtEdgeHabaGoso.Text)             ' エッジ整合性_板幅許容誤差
		drow.Item("エッジ整合性_前フレームとの許容誤差") = CDbl(txtEdgeFrmGoso.Text)    ' エッジ整合性_前フレームとの許容誤差
		drow.Item("エッジ整合性_溶接後不感帯") = CInt(nudEdgeFukantai.Text)             ' エッジ整合性_溶接後不感帯
		drow.Item("エッジ整合性_エッジ検出異常FF送信") = CInt(nudEdgeFF.Text)           ' エッジ整合性_エッジ検出異常FF送信

		drow.Item("エッジ検出_実施種別") = cmbEdgOutType.SelectedIndex                   ' エッジ検出_実施種別
		drow.Item("エッジ検出_検出範囲") = CDbl(txtEdgeOutKenHan.Text)                   ' エッジ検出_検出範囲

		drow.Item("重複疵マップ背景色_重") = lblMapZyuColor.BackColor.ToArgb            ' 重複疵マップ背景色_重
		drow.Item("重複疵マップ背景色_中") = lblMapChuColor.BackColor.ToArgb            ' 重複疵マップ背景色_中
		drow.Item("重複疵マップ背景色_軽") = lblMapKeiColor.BackColor.ToArgb            ' 重複疵マップ背景色_軽

		drow.Item("重複疵マップ疵個数閾値_重") = CInt(nudKizumapTempZyu.Text)           ' 重複疵マップ疵個数閾値_重
		drow.Item("重複疵マップ疵個数閾値_中") = CInt(nudKizumapTempChu.Text)           ' 重複疵マップ疵個数閾値_中
		drow.Item("重複疵マップ疵個数閾値_軽") = CInt(nudKizumapTempKei.Text)           ' 重複疵マップ疵個数閾値_軽

		drow.Item("設備長_検査基点_上流カメラ") = CInt(nudCamera.Text)                  ' 設備長_検査基点_上流カメラ
		drow.Item("設備長_検査基点_SPM") = CInt(nudSpm.Text)                            ' 設備長_検査基点_SPM
		drow.Item("設備長_表示基点_裏検査台") = CInt(nudExaminingTableB.Text)           ' 設備長_表示基点_裏検査台
		drow.Item("設備長_表示基点_表検査台") = CInt(nudExaminingTableT.Text)           ' 設備長_表示基点_表検査台
		drow.Item("設備長_表示基点_シャー") = CInt(nudCutPos.Text)                      ' 設備長_表示基点_シャー

		'全長画像
		If cmbImageSaveR.Text = "有り" Then
			drow.Item("全長画像_保存_R") = 1
		Else
			drow.Item("全長画像_保存_R") = 0
		End If
		If cmbImageSaveG.Text = "有り" Then
			drow.Item("全長画像_保存_G") = 1
		Else
			drow.Item("全長画像_保存_G") = 0
		End If
		If cmbImageSaveB.Text = "有り" Then
			drow.Item("全長画像_保存_B") = 1
		Else
			drow.Item("全長画像_保存_B") = 0
		End If
		If cmbImageSaveRB.Text = "有り" Then
			drow.Item("全長画像_保存_差") = 1
		Else
			drow.Item("全長画像_保存_差") = 0
		End If


		drow.Item("PLG距離比較異常上限値") = CDbl(txtHikakuPlgHigh.Text)                     ' PLG距離比較異常上限値
		drow.Item("PLG距離表示異常上限値") = CDbl(txtHyoziPlgHigh.Text)                      ' PLG距離表示異常上限値
		drow.Item("プロコンコイル長比較異常上限値") = CDbl(txtProcomHigh.Text)               ' プロコンコイル長比較異常上限値

		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		' 長周期欠陥ガイダンス
		If cmbLongPd_GuidanceAvailable.Text = "有り" Then
			drow.Item("LongPd_GuidanceAvailable") = 1
		Else
			drow.Item("LongPd_GuidanceAvailable") = 0
		End If

		drow.Item("LongPd_GuidanceCheckInterval") = CInt(nudLongPd_GuidanceCheckInterval.Text)
		drow.Item("LongPd_NearFurnaceRangeFrom") = CInt(nudLongPd_NearFurnaceRangeFrom.Text)
		drow.Item("LongPd_NearFurnaceRangeTo") = CInt(nudLongPd_NearFurnaceRangeTo.Text)
		drow.Item("LongPd_PeriodicPitchTolerance") = CInt(nudLongPd_PeriodicPitchTolerance.Text)

		drow.Item("LongPd_GuidanceForeColor") = CInt(lblLongPd_GuidanceMessagePreview.ForeColor.ToArgb)
		drow.Item("LongPd_GuidanceBackColor") = CInt(lblLongPd_GuidanceMessagePreview.BackColor.ToArgb)
		drow.Item("LongPd_GuidanceMessage") = CStr(txtLongPd_GuidanceMessage.Text)
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
	End Sub

	''' <summary>
	''' アップデート
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Dim transact As SqlClient.SqlTransaction = Nothing   'トランザクション処理

		Try
			' DB接続
			m_Adapter.SelectCommand.Connection.Open()

			'トランザクション開始
			transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
			m_Adapter.SelectCommand.Transaction = transact

			Using cbWk As New SqlClient.SqlCommandBuilder()

				'コマンドアダプタをセットして、更新
				cbWk.DataAdapter = m_Adapter
				m_Adapter.Update(m_DS)

			End Using

			'コミット
			transact.Commit()

			'正常に登録された場合はメール送信
			modMain.Send_Main(CH_PARAM_SETTIG_COMMON)
			modMain.HardCopy(Me)
			Return True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)

			' エラーならロールバックして終了
			If Not transact Is Nothing Then
				transact.Rollback()
			End If

			Return False
		Finally
			' 後始末
			If m_Adapter.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter.SelectCommand.Connection.Close()
			End If

			If Not transact Is Nothing Then
				transact.Dispose()
			End If
		End Try
	End Function


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
		Next

		Return retColor
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 色変更処理
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 色選択ダイアログを表示し、ボタンの色を変更
	''' </summary>
	''' <param name="objColor">色変更対象のラベルまたはボタン</param>
	''' <param name="bFont">色変更対象が文字か背景か True:文字 False:背景</param>
	''' <remarks></remarks>
	Private Sub subGetDlgColor(ByVal objColor As Control, ByVal bFont As Boolean)

		'色選択ダイアログの設定
		'色の作成を使用しない
		dlgColor.AllowFullOpen = False

		If bFont Then
			'色選択ダイアログの選択色を、ボタンの文字色（前景色）にする
			dlgColor.Color = objColor.ForeColor
		Else
			'色選択ダイアログの選択色を、ボタンの背景色にする
			dlgColor.Color = objColor.BackColor
		End If

		' 色選択ダイアログでokボタンが押されたら、ボタンの色を変更する 
		If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
			If bFont Then
				'選択色をボタンの文字色に
				objColor.ForeColor = dlgColor.Color
			Else
				'選択色をボタンの背景色に
				objColor.BackColor = dlgColor.Color
			End If
		End If

		dlgColor.Dispose()
	End Sub

	''' <summary>
	''' イベント文字色変更ボタン押下時にラベルの文字色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnEventColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEventColor.Click
		'色選択ダイアログを表示し、ラベルの文字色を変更
		subGetDlgColor(lblEventColor, True)
	End Sub

	''' <summary>
	''' 軽故障文字色変更ボタン押下時にラベルの文字色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnKeiColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKeiColor.Click
		'色選択ダイアログを表示し、ラベルの文字色を変更
		subGetDlgColor(lblKeiColor, True)
	End Sub

	''' <summary>
	''' 重故障文字色変更ボタン押下時にラベルの文字色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnZyuColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnZyuColor.Click
		'色選択ダイアログを表示し、ラベルの文字色を変更
		subGetDlgColor(lblZyuColor, True)
	End Sub

	''' <summary>
	''' 正常色変更ボタン押下時にラベルの背景色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnNormalColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnNormalColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblNormalColor, False)
	End Sub

	''' <summary>
	''' 注意色変更ボタン押下時にラベルの背景色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnCautionColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCautionColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblCautionColor, False)
	End Sub

	''' <summary>
	''' 異常色変更ボタン押下時にラベルの背景色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAbnormalColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAbnormalColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblAbnormalColor, False)
	End Sub

	''' <summary>
	''' 重複疵マップ背景色_軽 変更ボタン押下時にラベルの背景色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMapKeiColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMapKeiColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblMapKeiColor, False)
	End Sub

	''' <summary>
	''' 重複疵マップ背景色_中 変更ボタン押下時にラベルの背景色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMapChuColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMapChuColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblMapChuColor, False)
	End Sub

	''' <summary>
	''' 重複疵マップ背景色_重 変更ボタン押下時にラベルの背景色を変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMapZyuColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMapZyuColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblMapZyuColor, False)
	End Sub

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	''' 長周期欠陥ガイダンスメッセージ 前景色
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnLongPd_GuidanceFrontColor_Click(sender As Object, e As EventArgs) Handles btnLongPd_GuidanceForeColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblLongPd_GuidanceMessagePreview, True)
	End Sub

	''' <summary>
	''' 長周期欠陥ガイダンスメッセージ 背景色
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnLongPd_GuidanceBackColor_Click(sender As Object, e As EventArgs) Handles btnLongPd_GuidanceBackColor.Click
		'色選択ダイアログを表示し、ラベルの背景色を変更
		subGetDlgColor(lblLongPd_GuidanceMessagePreview, False)
	End Sub

	''' <summary>
	''' 長周期欠陥ガイダンスメッセージ 内容変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub txtLongPd_GuidanceMessage_TextChanged(sender As Object, e As EventArgs) Handles txtLongPd_GuidanceMessage.TextChanged
		lblLongPd_GuidanceMessagePreview.Text = txtLongPd_GuidanceMessage.Text
	End Sub
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	''' <summary>
	''' 更新ボタンの有効/無効
	''' </summary>
	''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
	''' <remarks></remarks>
	Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

		btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

	End Sub
End Class

