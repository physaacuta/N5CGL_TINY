
'*********************************************************************************
' 検査実績検索画面の親クラス
'	[Ver]
'		Ver.01    2008/11/05  vs2005 対応
'
'	[メモ]
'		frmPop_Search をより汎用化したバージョン。 極力こちらを使用してください。
'*********************************************************************************
Option Strict On
Imports tClassLibrary

Public Class frmPop_Search2

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 構造体
	'/ /////////////////////////////////////////////////////////////////////////////////

	'疵種・グレードフィルター列名
	Public Enum EM_FILTER_COL						' 疵種
		emCheck = 0									' チェックボックス
		emName										' 名称
		emEdasysID									' EdasysID (主キー)
	End Enum


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Protected my_bCancel As Boolean = False					' キャンセル時 true


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_strSettingIniName As String = LOCALINI_NAME	' 設定情報保存 INIファイル名
	Private m_strSettingSession As String					' 設定情報保存 セクション名

	Private m_sDbSession As String = INI_DB					' DBセッション

	Private m_nCoilRowNext As Integer = -1					' 次コイルの行位置 (下行)
	Private m_nCoilRowPre As Integer = -1					' 前コイルの行位置 (上行)



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 設定情報保存 INIファイル名
	''' </summary>
	''' <value></value>
	<System.ComponentModel.Browsable(True) _
	  , System.ComponentModel.DefaultValue(INI_DB) _
	  , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
	  , System.ComponentModel.Description("設定情報保存 INIファイル名")> _
	 Public Property tProp_SettingIniName() As String
		Get
			Return m_strSettingIniName
		End Get
		Set(ByVal Value As String)
			m_strSettingIniName = Value
		End Set
	End Property

	''' <summary>
	''' 設定情報保存 セクション名
	''' </summary>
	''' <value></value>
	<System.ComponentModel.Browsable(True) _
	  , System.ComponentModel.DefaultValue(DB_PC_INF) _
	  , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
	  , System.ComponentModel.Description("設定情報保存 セクション名")> _
	 Public Property tProp_SettingIniSession() As String
		Get
			Return m_strSettingSession
		End Get
		Set(ByVal Value As String)
			m_strSettingSession = Value
		End Set
	End Property


	''' <summary>
	''' DBセクション
	''' </summary>
	''' <value></value>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(INI_DB) _
	 , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
	 , System.ComponentModel.Description("DBセッション")> _
	Public Property tProp_DbSession() As String
		Get
			Return m_sDbSession
		End Get
		Set(ByVal Value As String)
			m_sDbSession = Value
		End Set
	End Property

	''' <summary>
	''' 次コイルの行位置 (下行) (無効時-1)
	''' </summary>
	''' <value></value>
	Public Property tProp_CoilRowNext() As Integer
		Get
			Return m_nCoilRowNext
		End Get
		Set(ByVal Value As Integer)
			m_nCoilRowNext = Value
		End Set
	End Property

	''' <summary>
	''' 前コイルの行位置 (上行) (無効時-1)
	''' </summary>
	''' <value></value>
	Public Property tProp_CoilRowPre() As Integer
		Get
			Return m_nCoilRowPre
		End Get
		Set(ByVal Value As Integer)
			m_nCoilRowPre = Value
		End Set
	End Property


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 処理
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <remarks></remarks>
	Public Sub New()

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。

		'''' 検索結果 グリッドビュー
		Call dgvInitialize_Result(dgvResult)

		'''' フィルター用 グリッドビュー
		Call dgvInitialize_FilterGrid(dgvDispType)
		Call dgvInitColumn_FilterGrid(dgvDispType, "疵種名称")
		Call dgvFilter_AddHandler(dgvDispType)

		Call dgvInitialize_FilterGrid(dgvDispGrade)
		Call dgvInitColumn_FilterGrid(dgvDispGrade, "グレード名称")
		Call dgvFilter_AddHandler(dgvDispGrade)

		'疵種全選択、全解除に該当グリッド情報をセット
		btnTypeAllSelect.Tag = dgvDispType
		btnTypeAllCancel.Tag = dgvDispType

		'グレード全選択、全解除に該当グリッド情報をセット
		btnGradeAllSelect.Tag = dgvDispGrade
		btnGradeAllCancel.Tag = dgvDispGrade
	End Sub

	''' <summary>
	''' 画面終了
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click

		Me.Hide()

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ リストサーチ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 現在のコイルリストの前後をチェック
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CheckCoilRowNextPre()
		'''' ここでは、リスト表示されているものが全て対象と判断している。
		' ダメなら勝手にシャドウなりオーバーライドなりしてください。

		Dim nRow As Integer		' 現在行
		Dim nWk As Integer
		Try
			'現在行を取得
			nRow = dgvResult.CurrentCell.RowIndex
			If -1 = nRow Then
				Me.tProp_CoilRowNext = -1
				Me.tProp_CoilRowPre = -1
				Return
			End If

			' 次のコイル検索
			nWk = nRow + 1
			If nWk > dgvResult.RowCount - 1 Then
				Me.tProp_CoilRowNext = -1
			Else
				Me.tProp_CoilRowNext = nWk
			End If

			' 前のコイル検索
			nWk = nRow - 1
			If nWk < 0 Then
				Me.tProp_CoilRowPre = -1
			Else
				Me.tProp_CoilRowPre = nWk
			End If

		Catch ex As Exception
		End Try
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ グリッド初期化
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 疵種フィルター グリッド初期化
	''' </summary>
	''' <param name="dgv">フィルター用のデータグリッド</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize_FilterGrid(ByVal dgv As tCnt.tCnt_DataGridView)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)

		'''' 独自の項目を設定
		With dgv
			' '' 定義
			'.ColumnHeadersHeight = 25										' ヘッダーの高さ
			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' 行選択
			.ScrollBars = ScrollBars.Vertical								' スクロールバーの表示

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' 選択
			.DefaultCellStyle().SelectionForeColor = Color.Black			'
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)	'
			.DefaultCellStyle().ForeColor = Color.Black

			.RowCount = 0
		End With
	End Sub

	''' <summary>
	''' 検索結果 グリッド初期化
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize_Result(ByVal dgv As tCnt.tCnt_DataGridView)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)

		'''' 独自の項目を設定

		With dgv
			'' 定義
			.ColumnHeadersHeight = 45										' ヘッダーの高さ
			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' 行選択
			.ScrollBars = ScrollBars.Both

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' 選択
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
			.DefaultCellStyle().SelectionForeColor = Color.Black			'
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)	'
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
	Protected Friend Overridable Sub dgvInitColumn_FilterGrid(ByVal dgv As DataGridView, ByVal strName As String)
		With dgv
			''''' 列追加
			.Columns.Add(New DataGridViewCheckBoxColumn)  ' チェックボックス
			.Columns.Add(New DataGridViewTextBoxColumn)	  ' 名称
			.Columns.Add(New DataGridViewTextBoxColumn)	  ' EdasysID

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
			.Columns(EM_FILTER_COL.emEdasysID).Visible = False	'非表示
		End With
	End Sub




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
	''' 全選択ボタンクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllSelect.Click, btnGradeAllSelect.Click

		'チェックボックスを全てON
		Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), True)

	End Sub
	''' <summary>
	''' 全解除ボタンクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

		'チェックボックスを全てOFF
		Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), False)

	End Sub

	''' <summary>
	''' グリッドのチェックボックスセット
	''' </summary>
	''' <param name="dgvData">データグリッド</param>
	''' <param name="bValue">True or False</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CheckGridAllChange(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

		Dim ii As Integer

		' グリッドの全てのチェックボックスを設定
		For ii = 0 To dgvData.RowCount - 1
			dgvData(EM_FILTER_COL.emCheck, ii).Value = bValue
		Next ii

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ フィルター 情報
	'/ /////////////////////////////////////////////////////////////////////////////////


	''' <summary>
	''' 疵種・グレードフィルタセット
	''' </summary>
	''' <param name="dgv">データグリッド</param>
	''' <param name="strTable">テーブル名</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String)
		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String = ""
		Dim iCnt As Integer = 0

		'DBオープン
		If Not tcls_DB.Open() Then Exit Sub

		'SQL文作成
		strSQL = ""
		strSQL &= " SELECT Name, "
		strSQL &= "        EdasysID "
		strSQL &= " FROM " & strTable
		strSQL &= " ORDER BY ID "

		sqlRead = tcls_DB.SelectExecute(strSQL)
		If sqlRead Is Nothing Then Exit Sub

		Try

			While sqlRead.Read()

				dgv.RowCount += 1	'表示行数を設定

				''設定不可 指定
				dgv(EM_FILTER_COL.emName, iCnt).ReadOnly = True

				'データセット
				dgv(EM_FILTER_COL.emName, iCnt).Value = sqlRead.GetString(0)
				dgv(EM_FILTER_COL.emName, iCnt).Style.WrapMode = DataGridViewTriState.False
				dgv(EM_FILTER_COL.emCheck, iCnt).Value = False
				dgv(EM_FILTER_COL.emEdasysID, iCnt).Value = sqlRead.GetInt32(1)

				iCnt += 1

			End While

		Catch ex As Exception

		Finally
			' DB開放
			If Not sqlRead Is Nothing Then sqlRead.Close()
			Call tcls_DB.Dispose()

		End Try
	End Sub




	''' <summary>
	''' フィルターのチェック保存
	''' </summary>
	''' <param name="dgv">データグリッド</param>
	''' <param name="strInikey">iniファイルの対象キー</param>
	''' <remarks></remarks>
	Protected Sub SaveFilterData(ByVal dgv As DataGridView, ByVal strInikey As String)
		Dim strVal As String = ""

		' 疵種,グレードフィルターの条件をINIファイルへ登録
		For ii As Integer = 0 To dgv.RowCount - 1
			strVal &= CStr(IIf(CType(dgv(EM_FILTER_COL.emCheck, ii).Value, Boolean), "1", "0"))
		Next ii

		tClassLibrary.tMod.WriteIni(m_strSettingSession, strInikey, m_strSettingIniName, strVal)
	End Sub

	''' <summary>
	''' フィルターのチェックデータ読込
	''' </summary>
	''' <param name="dgv"></param>
	''' <param name="strInikey"></param>
	''' <remarks></remarks>
	Protected Sub ReadFilterData(ByVal dgv As DataGridView, ByVal strInikey As String)
		' INIファイルの設定を取得
		Dim strVal As String = tClassLibrary.tMod.ReadIniStr(m_strSettingSession, strInikey, m_strSettingIniName)
		If "" = strVal Then Exit Sub

		' 設定値を反映
		For ii As Integer = 0 To dgv.RowCount - 1

			'未登録または、1が設定されている場合は、チェックを入れる
			If "1" = Mid(strVal, ii + 1, 1) Then
				dgv(EM_FILTER_COL.emCheck, ii).Value = True
			Else
				dgv(EM_FILTER_COL.emCheck, ii).Value = False
			End If
		Next ii
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 検索中 プログレスバー 関係
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 検索中 プログレスバー 表示状態変更
	''' </summary>
	''' <param name="blnExec"></param>
	''' <remarks></remarks>
	Public Sub WaitControl_EnableMode(ByVal blnExec As Boolean)

		' コントロールの状態変更
		pnlMsg.Visible = blnExec

		btnEnd.Enabled = Not blnExec
		btnSearch.Enabled = Not blnExec
		btnSelect.Enabled = Not blnExec
		dgvResult.Enabled = Not blnExec

		' キャンセルフラグセット
		my_bCancel = Not blnExec
	End Sub

	''' <summary>
	''' 検索中 プログレスバー 表示
	''' </summary>
	''' <param name="strVal">表示メッセージ</param>
	''' <param name="nMax">最大値</param>
	''' <remarks></remarks>
	Public Sub WaitControl_Start(ByVal strVal As String, ByVal nMax As Long)
		txtMsg.Text = strVal
		prbMsg.Minimum = 0
        prbMsg.Maximum = CInt(IIf(nMax > prbMsg.Minimum, CType(nMax, Integer), CType(prbMsg.Minimum + 1, Integer)))
		prbMsg.Value = 0

		' このときは、キャンセルボタン等を押せるようにしたい為、DoEventを使用する
		Application.DoEvents()
	End Sub

	''' <summary>
	''' 検索中 プログレスバー 更新
	''' </summary>
	''' <param name="nVal">進捗値</param>
	''' <remarks></remarks>
	Public Sub WaitControl_Running(ByVal nVal As Integer)

		'進捗値セット
		prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))
		'prbMsg.Refresh()
		' このときは、キャンセルボタン等を押せるようにしたい為、DoEventを使用する
		Application.DoEvents()
	End Sub
	Public Sub WaitControl_Running()
		Call WaitControl_Running(prbMsg.Value + 1)
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ その他もろもろ
	'/ /////////////////////////////////////////////////////////////////////////////////
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
	''' 日付コントロールに対して ホイールで加算減算が可能とする
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
		' 現在のdatetimepicker に↑↓キーを押された事とする
		SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
	End Sub



	'' 20130522 カンマを除去する為に引数追加
	''' <summary>
	''' CSV出力
	''' </summary>
	''' <param name="strFolder">保存ファイルのフォルダパス</param>
	''' <param name="strName">保存ファイルのファイル名</param>
	''' <param name="bDelComma">カンマを除去</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""						'1行のデータ
		Dim strWk() As String
		Dim strWkData As String

		'' フルパス
		strFPath = IO.Path.Combine(strFolder, strName)

		'' フォルダの有無チェック(無かったら作ろう)
		tMod.CheckFolder(strFolder)

		'' CSV出力
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' オープン
				cls_Stram.Open()



				'ヘッダを出力
				strData = ""
				For ii As Integer = 0 To dgvResult.ColumnCount - 1

					'改行コードをすべて取り除く
					strWk = dgvResult.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
					strWkData = ""
					For jj As Integer = 0 To strWk.Length - 1
						strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
					Next jj

					strData &= Trim(strWkData) & ","
				Next ii

				'最後のタブを排除
				strData = strData.Remove(strData.Length - 1, 1)

				cls_Stram.WriteLine(strData)

				'データ出力
				For ii As Integer = 0 To dgvResult.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvResult.ColumnCount - 1
						'項目をTabで連結する
						'' 20130522変更 カンマを除去------------------------>>>
						If True = bDelComma Then
							strData &= dgvResult(jj, ii).Value.ToString.Replace(",", "") & ","
						Else
							strData &= dgvResult(jj, ii).Value.ToString & ","
						End If
						'' <<<-------------------------------------------------
					Next jj

					'最後のタブを排除
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

End Class
