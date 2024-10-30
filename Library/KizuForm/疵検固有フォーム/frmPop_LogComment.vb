'*********************************************************************************
' 故障ログ コメント追加画面
'	[Ver]
'		Ver.01    2009/09/14  vs2005 対応
'
'	[メモ]
'		LOG_INF に主キー設定されているタイプ(AutoNoが追加)のみ使用可能
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' 故障ログ コメント追加画面
''' </summary>
''' <remarks></remarks>
Public Class frmPop_LogComment

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開
	'/ /////////////////////////////////////////////////////////////////////////////////
	' 削除、編集の時のキーとする
	Public Structure typSelectRowData
		Dim nAutoNo As Integer												' AutoNo
		Dim nSysNo As Integer												' アラームNo (新規時、0)
		Dim dayAddTime As Date												' 選択した登録日付
		Dim dayDspTime As Date												' 選択した表示時間

		' 新規時 以降空白でOK
		Dim sTaskName As String												' 発生元
		Dim sPcName As String												' PC名
		Dim nPcKind As Integer												' PCKIND
		Dim sMsg As String													' 表示メッセージ
	End Structure


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private mcls_DB As tClass.tClass_SQLServer								' DB
	Private m_sDbSession As String = INI_DB									' DBセッション

	Private m_typSelectRowData As typSelectRowData							' 選択行の情報
	'Private m_nSelectAutoNo As Integer										' 選択している情報のAutoNo(主キーの一部)

	Private m_nLogModeSyubetu() As Integer = {SYSNO_COMMENT_ALL, SYSNO_COMMENT_SOFT, SYSNO_COMMENT_HARD} ' ログ種別 (全般, ソフト, ハード)
	Private m_nLogKindKubun As Integer = 4									' ログ区分 (4:コメント)
	Private m_bDataUpDate As Boolean = False								' 何かしらの変更処理実施時 true


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' DBセクション
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(INI_DB) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
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
	''' 選択行状態
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property tProp_SelectRowData() As typSelectRowData
		Get
			Return m_typSelectRowData
		End Get
		Set(ByVal Value As typSelectRowData)
			m_typSelectRowData = Value
		End Set
	End Property

	''' <summary>
	''' 変更状態取得
	''' </summary>
	Public ReadOnly Property Prop_IsDataUpDate() As Boolean
		Get
			Return m_bDataUpDate
		End Get
	End Property

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' フォームロード
	''' </summary>
	Private Sub frmPop_LogComment_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'''' ファンクションボタン定義
		MyBase.AddFunctionButton(btnInsert, 0)
		MyBase.AddFunctionButton(btnUpdate, 1)
		MyBase.AddFunctionButton(btnDelete, 3)
		MyBase.AddFunctionButton(btnClose, 11)


		'''' データベース接続
		mcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
		If Not mcls_DB.Open() Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("データベース接続失敗。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return
		End If

		'''' 画面初期セット
		Call DspInit()

		'''' 画面初期表示
		Call DspSelect()

		''''' AutoNoを取得
		'' コメントの編集時のみ
		'If 0 <> m_typSelectRowData.nSysNo Then
		'	m_nSelectAutoNo = GetSelectAutoNo(m_typSelectRowData.nSysNo, m_typSelectRowData.dayDspTime, m_typSelectRowData.dayAddTime)
		'End If
	End Sub

	''' <summary>
	''' フォームクローズ
	''' </summary>
	Private Sub frmPop_LogComment_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		mcls_DB.Close()
		mcls_DB.Dispose()
		mcls_DB = Nothing
	End Sub

	''' <summary>
	''' ファンクションボタン
	''' </summary>
	''' <param name="FncNo"></param>
	''' <param name="onKey"></param>
	''' <remarks></remarks>
	Private Sub frmPop_DefectInput_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' ファンクション切替
		Select Case FncNo							' 0オリジン
			'//-----------------------------------------------------------
			Case 1 - 1								  ' 新規登録
				If Not Exec_Insert() Then Return
				Me.Close()

				'//-----------------------------------------------------------
			Case 2 - 1								  ' 編集
				If Not Exec_Update() Then Return
				Me.Close()
				'//-----------------------------------------------------------
			Case 4 - 1								  ' 削除
				If Not Exec_Delete() Then Return
				Me.Close()

				'//-----------------------------------------------------------
			Case 12 - 1								  ' 終了
				Me.Close()
		End Select
	End Sub

	''' <summary>
	''' 画面の初期セット
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspInit()
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSql As String = ""

		'''' 区分
		cmbKind.Items.Clear()
		cmbKind.Items.Add(New tClass_CmbItem(m_nLogModeSyubetu(0), "全般"))
		cmbKind.Items.Add(New tClass_CmbItem(m_nLogModeSyubetu(1), "ｿﾌﾄｳｪｱ"))
		cmbKind.Items.Add(New tClass_CmbItem(m_nLogModeSyubetu(2), "ﾊｰﾄﾞｳｪｱ"))
		cmbKind.SelectedIndex = 0

		'''' PC名
		cmbPcName.Items.Clear()
		cmbPcName.Items.Add(New tClass_CmbItem(0, ""))
		cmbPcName.SelectedIndex = 0

		' Select文実行
		strSql = "SELECT i.PCKIND, m.PC名 "
		strSql &= "FROM " & DB_PC_MASTER & " m "
		strSql &= "LEFT OUTER JOIN " & DB_PC_INF & " i ON m.PCID = i.PCID "
		strSql &= " ORDER BY m.PCID ASC"
		sqlRead = mcls_DB.SelectExecute(strSql)
		If sqlRead Is Nothing Then
			Return
		End If

		'''' 全件取得
		Dim nRowCnt As Integer = 0
		Try
			Dim strWk As String = ""
			While sqlRead.Read()
	
				cmbPcName.Items.Add(New tClass_CmbItem(sqlRead.GetInt32(0), sqlRead.GetValue(1).ToString))

			End While
		Catch ex As Exception
			Return
		Finally
			sqlRead.Close()
			sqlRead = Nothing
		End Try
	End Sub

	''' <summary>
	''' 画面の選択状態表示
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspSelect()
		Dim ii As Integer
		Dim bNew As Boolean	' 新規時 true

		'' 新規 or 編集
		If 0 <> m_typSelectRowData.nSysNo Then
			bNew = False
		Else
			bNew = True
		End If

		'' ボタン状態
		btnUpdate.Enabled = Not bNew
		btnDelete.Enabled = Not bNew

		'' 時間
		dtpStartDay.Value = m_typSelectRowData.dayAddTime	' 時間表示は、登録日付の時間を使用する
		dtpStartTime.Value = m_typSelectRowData.dayAddTime

		'''' これ以降は、編集時のみ
		If bNew Then Return

		'' 区分
		If SYSNO_COMMENT_ALL = m_typSelectRowData.nSysNo Then cmbKind.SelectedIndex = 0
		If SYSNO_COMMENT_SOFT = m_typSelectRowData.nSysNo Then cmbKind.SelectedIndex = 1
		If SYSNO_COMMENT_HARD = m_typSelectRowData.nSysNo Then cmbKind.SelectedIndex = 2

		'' PC名称
		For ii = 0 To cmbPcName.Items.Count - 1
			If CType(cmbPcName.Items(ii), tClass_CmbItem).g_sName = m_typSelectRowData.sPcName Then
				cmbPcName.SelectedIndex = ii
				Exit For
			End If
		Next ii

		'' その他
		txtTask.Text = m_typSelectRowData.sTaskName
		txtMsg.Text = m_typSelectRowData.sMsg

	End Sub

	''' <summary>
	''' 日付コントロールに対して ホイールで加算減算が可能とする
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel
		' 現在のdatetimepicker に↑↓キーを押された事とする
		SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メイン処理
	'/ /////////////////////////////////////////////////////////////////////////////////
	'''' <summary>
	'''' AutoNoを取得
	'''' </summary>
	'''' <param name="nSysNo">シスログNo</param>
	'''' <param name="datDspTime">表示時刻</param>
	'''' <param name="datAddTime">登録日付</param>
	'''' <returns>AutoNo</returns>
	'''' <remarks></remarks>
	'Private Function GetSelectAutoNo(ByVal nSysNo As Integer, ByVal datDspTime As Date, ByVal datAddTime As Date) As Integer
	'	Dim strSql As String
	'	strSql = "SELECT TOP 1 AutoNo FROM " & DB_LOG_INF
	'	strSql &= " WHERE 発生日付時刻='" & datDspTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
	'	strSql &= " AND 登録日付='" & datAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
	'	strSql &= " AND アラームNo=" & nSysNo

	'	Dim nAutoNo As Integer
	'	mcls_DB.GetSelectKey(strSql, nAutoNo)
	'	Return nAutoNo
	'End Function

	''' <summary>
	''' 次のAutoNoを算出する
	''' </summary>
	''' <param name="datAddTime">登録日付</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetNextAutoNo(ByVal datAddTime As Date) As Integer
		Dim strSql As String
		' その時間の一番大きな値を取得する
		strSql = "SELECT MAX(AutoNo) FROM " & DB_LOG_INF
		strSql &= " WHERE 登録日付='" & datAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
		'strSql &= " AND AutoNo%10=9"		

		' 次の値を算出
		Dim nAutoNo As Integer
		mcls_DB.GetSelectKey(strSql, nAutoNo)
		nAutoNo += 10	' 次の値
		' コメントの場合 AutoNoの下1桁を9固定にする
		If 9 <> (nAutoNo Mod 10) Then
			nAutoNo = CInt(Int(nAutoNo / 10) * 10 + 9)
		End If
		Return nAutoNo
	End Function

	''' <summary>
	''' 入力チェック
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function CheckInputData(ByRef newTyp As typSelectRowData) As Boolean
		'''' データチェック
		If 0 <> tClass.tClass_ImputCheck.CheckImputData_Str(txtMsg.Text, 0, 256) Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("メッセージ 文字数異常(256文字以下)。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
		If 0 <> tClass.tClass_ImputCheck.CheckImputData_Str(txtTask.Text, 0, 16) Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("発生元 文字数異常(16文字以下)。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If

		'''' データ取得
		' シスログ
		newTyp.nSysNo = CType(cmbKind.SelectedItem, tClass_CmbItem).g_nKey
		' 時刻
		newTyp.dayAddTime = CDate(dtpStartDay.Value.ToString("yyyy/MM/dd") & dtpStartTime.Value.ToString(" HH:mm:ss"))
		newTyp.dayDspTime = newTyp.dayAddTime
		' PC名/PCKIND
		newTyp.sPcName = CType(cmbPcName.SelectedItem, tClass_CmbItem).g_sName
		newTyp.nPcKind = CType(cmbPcName.SelectedItem, tClass_CmbItem).g_nKey
		' その他
		newTyp.sMsg = txtMsg.Text
		newTyp.sTaskName = txtTask.Text

		Return True
	End Function


	''' <summary>
	''' 削除処理
	''' </summary>
	''' <remarks></remarks>
	Private Function Exec_Delete() As Boolean
		'' DBからコメントの削除
		If Db_Delete(m_typSelectRowData.dayAddTime, m_typSelectRowData.nAutoNo) Then
			m_bDataUpDate = True
			Return True
		Else
			Using frmMsg As New tClassForm.frmPop_MsgBox("コメント削除失敗。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
	End Function

	''' <summary>
	''' コメント行削除
	''' </summary>
	''' <param name="datAddTime">登録日付</param>
	''' <param name="nAutoNo">AutoNo</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function Db_Delete(ByVal datAddTime As Date, ByVal nAutoNo As Integer) As Boolean
		Dim strSql As String
		strSql = "DELETE FROM " & DB_LOG_INF
		strSql &= " WHERE 登録日付='" & datAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
		strSql &= " AND AutoNo=" & nAutoNo

		'''' 登録
		If 0 <= mcls_DB.DMLExecute(strSql) Then
			Return True
		Else
			Return False
		End If
	End Function

	''' <summary>
	''' 新規追加
	''' </summary>
	''' <remarks></remarks>
	Private Function Exec_Insert() As Boolean
		Dim newTyp As typSelectRowData = Nothing

		'''' 入力チェック
		If Not CheckInputData(newTyp) Then Return False

		'''' 主キーの算出
		Dim nAutoNo As Integer = 0
		nAutoNo = GetNextAutoNo(newTyp.dayAddTime)

		'''' 登録
		If Db_Insert(newTyp, nAutoNo) Then
			m_bDataUpDate = True
			Return True
		Else
			Using frmMsg As New tClassForm.frmPop_MsgBox("コメント新規登録失敗。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
	End Function

	''' <summary>
	''' コメント 新規追加
	''' </summary>
	''' <param name="newTyp">新規データ</param>
	''' <param name="nAutoNo">新規主キー</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function Db_Insert(ByVal newTyp As typSelectRowData, ByVal nAutoNo As Integer) As Boolean
		Dim strSql As String

		strSql = "INSERT INTO " & DB_LOG_INF
		strSql &= " VALUES("
		strSql &= "'" & newTyp.dayDspTime.ToString("yyyy/MM/dd HH:mm:ss") & "',"
		strSql &= "'" & newTyp.sTaskName & "',"

		strSql &= "'" & newTyp.sPcName & "',"
		strSql &= newTyp.nPcKind & ","
		strSql &= newTyp.nSysNo & ","
		strSql &= "'" & newTyp.sMsg & "',"
		strSql &= "'" & newTyp.dayAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "',"
		strSql &= nAutoNo

		strSql &= ")"

		'''' 登録
		If 0 <= mcls_DB.DMLExecute(strSql) Then
			Return True
		Else
			Return False
		End If
	End Function

	''' <summary>
	''' 編集
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function Exec_Update() As Boolean
		Dim newTyp As typSelectRowData = Nothing

		'''' 入力チェック
		If Not CheckInputData(newTyp) Then Return False

		'''' 今あるデータを削除
		If Not Db_Delete(m_typSelectRowData.dayAddTime, m_typSelectRowData.nAutoNo) Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("コメント編集失敗。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If

		'''' 主キーの算出
		Dim nAutoNo As Integer = 0
		' 時間が変更された場合は、新規にAutoNoを算出
		' 同じ時間ならAutoNoは使いまわす (表示順番が変更される為の対策)
		If m_typSelectRowData.dayAddTime = newTyp.dayAddTime Then
			nAutoNo = m_typSelectRowData.nAutoNo
		Else
			nAutoNo = GetNextAutoNo(newTyp.dayAddTime)
		End If


		'''' 登録
		If Db_Insert(newTyp, nAutoNo) Then
			m_bDataUpDate = True
			Return True
		Else
			Using frmMsg As New tClassForm.frmPop_MsgBox("コメント編集失敗。", "異常", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
	End Function


End Class
