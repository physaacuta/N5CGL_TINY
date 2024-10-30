'*********************************************************************************
' ログ画面の親クラス
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' ログ画面の親クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Log
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ログ種別
	''' </summary>
	''' <remarks></remarks>
	Public Enum gem_LogMode
		''' <summary>全般</summary>
		iAns = 0
		''' <summary>ソフトウェア</summary>
		iSoft = 1
		''' <summary>ハードウェア</summary>
		iHard = 2
	End Enum

	''' <summary>
	''' ログ区分
	''' </summary>
	''' <remarks></remarks>
	Public Enum gem_LogKind
		''' <summary>イベント</summary>
		iEvt = 0
		''' <summary>重故障</summary>
		iJyu = 1
		''' <summary>軽故障</summary>
		iKei = 2
		''' <summary>コメント</summary>
		iCmt = 3
	End Enum

	Private Enum em_LogListColm
		idayAddTime = 0
		idayDspTime
		iKind
		iMode
		iTaskName
		iPcName
		iPcKindName
		iSysNo
		iMsg
		iSubMsg
		iPcKind																' コメント機能有効時のみデータセット
		iAutoNo																' コメント機能有効時のみデータセット


		iend
	End Enum

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_clLogKindColor(3) As Color									' ログ表示色 (0:イベント 1:重故障 2:軽故障 3:コメント)
	'Private m_nLogMaxRow As Integer											' ログ表示最大行数
	Private m_lstChkPCName As New List(Of CheckBox)							' ＰＣ名チェックボックスコントロール リスト
	Private m_lstChkPCKind As New List(Of CheckBox)							' PCKINDチェックボックスコントロール リスト
	Private m_lstChkKubun_A As New List(Of CheckBox)						' 全般区分チェックボックスコントロール リスト
	Private m_lstChkKubun_S As New List(Of CheckBox)						' ソフト区分チェックボックスコントロール リスト
	Private m_lstChkKubun_H As New List(Of CheckBox)						' ハード区分チェックボックスコントロール リスト

	' DB関連
	Private m_nLogModeSyubetu() As Integer = {1, 2, 3}						' ログ種別 (全般, ソフト, ハード)
	Private m_nLogKindKubun() As Integer = {1, 2, 3, 4}						' ログ区分 (イベント, 重故障, 軽故障, コメント)
	Private m_sDB_LOG_M As String = DB_LOG_MASTER							' DBテーブル ログマスタ
	Private m_sDB_LOG_I As String = DB_LOG_INF								' DBテーブル ログインフ
	Private m_sDB_PC_I As String = DB_PC_INF								' DBテーブル PC種別
	Private m_sDbSession As String = INI_DB									' DBセッション

	' ログ関連
	Private m_bLog_ExecCancel As Boolean = False							' キャンセル時 True

	' オプションモード
	Private m_bIsVisibleAddTime As Boolean = False							' 登録日付表示非表示機能
	Private m_bIsCommentOn As Boolean = False								' コメント機能ON/OFF (AutoNo列必須)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' LOG_MASTER の ログ区分 (イベント)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(1) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("LOG_MASTER の ログ区分 (イベント)")> _
	Public Property tProp_LogKind_Evt() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iEvt)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iEvt) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_MASTER の ログ区分 (重故障)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(2) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("LOG_MASTER の ログ区分 (重故障)")> _
	Public Property tProp_LogKind_Jyu() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iJyu)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iJyu) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_MASTER の ログ区分 (軽故障)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(3) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("LOG_MASTER の ログ区分 (軽故障)")> _
	Public Property tProp_LogKind_Kei() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iKei)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iKei) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_MASTER の ログ区分 (コメント)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(4) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("LOG_MASTER の ログ区分 (コメント)")> _
	Public Property tProp_LogKind_Cmt() As Integer
		Get
			Return m_nLogKindKubun(gem_LogKind.iCmt)
		End Get
		Set(ByVal Value As Integer)
			m_nLogKindKubun(gem_LogKind.iCmt) = Value
		End Set
	End Property
	''' <summary>
	''' LOG_Master のテーブル名称
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_LOG_MASTER) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("LOG_MASTER のテーブル名称")> _
	Public Property tProp_TableName_LogMaster() As String
		Get
			Return m_sDB_LOG_M
		End Get
		Set(ByVal Value As String)
			m_sDB_LOG_M = Value
		End Set
	End Property

	''' <summary>
	''' LOG_INF のテーブル名称
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_LOG_INF) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("LOG_INF のテーブル名称")> _
	Public Property tProp_TableName_LogInf() As String
		Get
			Return m_sDB_LOG_I
		End Get
		Set(ByVal Value As String)
			m_sDB_LOG_I = Value
		End Set
	End Property

	''' <summary>
	''' PC_INF のテーブル名称
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_PC_INF) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("PC_INF のテーブル名称")> _
	Public Property tProp_TableName_PcInf() As String
		Get
			Return m_sDB_PC_I
		End Get
		Set(ByVal Value As String)
			m_sDB_PC_I = Value
		End Set
	End Property

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
	''' 登録日付のON/OFF
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("登録日付 ON/OFF")> _
	Public Property tProp_IsVisibleAddTime() As Boolean
		Get
			Return m_bIsVisibleAddTime
		End Get
		Set(ByVal Value As Boolean)
			m_bIsVisibleAddTime = Value
		End Set
	End Property
	''' <summary>
	''' コメント機能のON/OFF
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("☆Log画面専用プロパティ") _
	 , System.ComponentModel.Description("コメント機能 ON/OFF (Load前に設定済みの事。実行中は、変更不可)")> _
	Public Property tProp_IsComment() As Boolean
		Get
			Return m_bIsCommentOn
		End Get
		Set(ByVal Value As Boolean)
			m_bIsCommentOn = Value
		End Set
	End Property

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部アクセス メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ログ表示色設定 (-1の時は、未指定とする）
	''' </summary>
	''' <param name="nEvt">イベント色</param>
	''' <param name="nJyu">重故障色</param>
	''' <param name="nKei">軽故障色</param>
	''' <param name="nCmt">コメント色</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetLogKindColor(Optional ByVal nEvt As Integer = -1, Optional ByVal nJyu As Integer = -1, Optional ByVal nKei As Integer = -1, Optional ByVal nCmt As Integer = -1)
		If -1 <> nEvt Then m_clLogKindColor(gem_LogKind.iEvt) = Color.FromArgb(nEvt) 'tMod.tMod_Common.GetItoColor(nEvt)
		If -1 <> nJyu Then m_clLogKindColor(gem_LogKind.iJyu) = Color.FromArgb(nJyu) 'tMod.tMod_Common.GetItoColor(nJyu)
		If -1 <> nKei Then m_clLogKindColor(gem_LogKind.iKei) = Color.FromArgb(nKei) 'tMod.tMod_Common.GetItoColor(nKei)
		If -1 <> nCmt Then m_clLogKindColor(gem_LogKind.iCmt) = Color.FromArgb(nCmt)
	End Sub

	''' <summary>
	''' ログ表示最大行数
	''' </summary>
	''' <param name="nMaxRow">ログ表示最大行数</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetLogMaxRow(ByVal nMaxRow As Integer)
		'm_nLogMaxRow = nMaxRow
		nudRowMaxCount.Value = nMaxRow
	End Sub

	''' <summary>
	''' ＰＣ名チェックボックスコントロール を登録
	''' </summary>
	''' <param name="chk">ＰＣ名チェックボックスコントロール</param>
	''' <param name="pcName">DB検索するPC名称</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddCheckBoxPCName(ByVal chk As CheckBox, ByVal pcName As String)
		chk.Tag = pcName		' Tagに文字列を追加
		m_lstChkPCName.Add(chk)		' リストに登録
		'' ついでにクリックイベントハンドラに追加
		AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
	End Sub

	''' <summary>
	''' PCKindチェックボックスコントロール を登録
	''' </summary>
	''' <param name="chk">PCKindチェックボックスコントロール</param>
	''' <param name="pcName">DB検索するPCKind</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddCheckBoxPCKind(ByVal chk As CheckBox, ByVal pcName As String)
		chk.Tag = pcName		' Tagに文字列を追加
		m_lstChkPCKind.Add(chk)		' リストに登録
		'' ついでにクリックイベントハンドラに追加
		AddHandler chk.CheckedChanged, AddressOf chk_CheckedChanged
	End Sub

	''' <summary>
	''' チェックボックス 背景色変更
	''' </summary>
	''' <param name="chk">チェックボックス</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox)
		If chk.Checked Then					' ON
			chk.BackColor = Color.Cyan
		Else								' OFF
			chk.BackColor = Drawing.SystemColors.Control
		End If
	End Sub
	''' <summary>
	''' チェックボックス 背景色変更
	''' </summary>
	''' <param name="chk">チェックボックス</param>
	''' <param name="checked"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetChkBackColor_Change(ByVal chk As CheckBox, ByVal checked As Boolean)
		If checked Then					' ON
			chk.Checked = checked
			chk.BackColor = Color.Cyan
		Else								' OFF
			chk.Checked = checked
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

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' 生成
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Log_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'' デフォルト色
		m_clLogKindColor(gem_LogKind.iEvt) = Color.Black
		m_clLogKindColor(gem_LogKind.iJyu) = Color.Red
		m_clLogKindColor(gem_LogKind.iKei) = Color.Blue
		m_clLogKindColor(gem_LogKind.iCmt) = Color.DarkGreen  'Color.FromArgb(0, 64, 0)

		'' グリッド定義
		dgvInitialize(dgvLog)
		'' 区分チェックボックスをリストに追加 (この順番 大事)
		m_lstChkKubun_A.Add(chkKubun_A_Evt)
		m_lstChkKubun_A.Add(chkKubun_A_Jyu)
		m_lstChkKubun_A.Add(chkKubun_A_Kei)
		m_lstChkKubun_A.Add(chkKubun_A_Cmt)
		m_lstChkKubun_S.Add(chkKubun_S_Evt)
		m_lstChkKubun_S.Add(chkKubun_S_Jyu)
		m_lstChkKubun_S.Add(chkKubun_S_Kei)
		m_lstChkKubun_S.Add(chkKubun_S_Cmt)
		m_lstChkKubun_H.Add(chkKubun_H_Evt)
		m_lstChkKubun_H.Add(chkKubun_H_Jyu)
		m_lstChkKubun_H.Add(chkKubun_H_Kei)
		m_lstChkKubun_H.Add(chkKubun_H_Cmt)

		'' 時刻セット
		dtpStartDay.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
		dtpStartTime.Value = CDate(Now.ToString("yyyy/MM/dd 00:00:00"))
		dtpEndDay.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))
		dtpEndTime.Value = CDate(Now.ToString("yyyy/MM/dd 23:59:59"))

		'' コメント機能
		chkKubun_A_Cmt.Enabled = m_bIsCommentOn
		chkKubun_S_Cmt.Enabled = m_bIsCommentOn
		chkKubun_H_Cmt.Enabled = m_bIsCommentOn
		my_btnFunction(8 - 1).Enabled = m_bIsCommentOn
		'If Not m_bIsCommentOn Then my_btnFunction(8 - 1).Text = ""
	End Sub
	''' <summary>
	''' 開放
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Log_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		m_lstChkPCName.Clear()
	End Sub
	''' <summary>
	''' チェックボックス チェック状態変更
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub chk_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkKubun_A_Jyu.CheckedChanged, chkKubun_A_Kei.CheckedChanged, chkKubun_A_Evt.CheckedChanged, chkKubun_H_Evt.CheckedChanged, chkKubun_H_Kei.CheckedChanged, chkKubun_H_Jyu.CheckedChanged, chkKubun_S_Evt.CheckedChanged, chkKubun_S_Kei.CheckedChanged, chkKubun_S_Jyu.CheckedChanged, chkKubun_A_Cmt.CheckedChanged, chkKubun_S_Cmt.CheckedChanged, chkKubun_H_Cmt.CheckedChanged
		' チェックボックス 背景変更
		SetChkBackColor_Change(CType(sender, CheckBox))
	End Sub
	''' <summary>
	''' 時刻表示
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub timClock_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timClock.Tick
		lblClock.Text = Now.ToString("yyyy/MM/dd HH:mm:ss")
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

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ DataGridView 関連
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 表示設定
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
		With dgv
			'' 動作定義
			.AutoGenerateColumns = False									' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
			.AllowUserToAddRows = False										' ユーザー 行追加
			.AllowUserToDeleteRows = False									' ユーザー 行削除
			.AllowUserToOrderColumns = False								' ユーザー 列移動
			.AllowUserToResizeColumns = True								' ユーザー 列サイズ変更
			.AllowUserToResizeRows = False									' ユーザー 行サイズ変更
			.ShowCellErrors = False
			.ShowCellToolTips = False
			.ShowEditingIcon = False
			.ShowRowErrors = False

			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' セル選択
			.MultiSelect = False											' 複数行選択
			.ReadOnly = True												' 読み取り専用
			.EditMode = DataGridViewEditMode.EditProgrammatically			' セル書き換え

			.ColumnHeadersVisible = True									' 列ヘッダ表示
			.RowHeadersVisible = False										' 行ヘッダ表示
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' 可視列サイズ自動調整
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' 可視行サイズ自動調整
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.DarkBlue			' 選択
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
			'.DefaultCellStyle().SelectionForeColor = Color.Transparent
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue 'Color.LightSteelBlue         ' 1行おきに色替え


			'' 列定義
			'.ColumnCount = 4			'textbox のみを追加する場合はこれの方が楽
			'.Columns.AddRange( _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn(), _
			'  New DataGridViewTextBoxColumn())

            .ColumnCount = CInt(IIf(m_bIsCommentOn, CType(em_LogListColm.iend, Integer), CType(em_LogListColm.iend - 2, Integer)))


			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.RowCount = 0
			.ColumnHeadersHeight = 25										' 列ヘッダサイズ

			'列9 (時刻)
			.Columns(em_LogListColm.idayAddTime).HeaderText = "ｿｰﾄ/検索用統一時刻"
			.Columns(em_LogListColm.idayAddTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.idayAddTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
			.Columns(em_LogListColm.idayAddTime).Width = 160
			.Columns(em_LogListColm.idayAddTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
			.Columns(em_LogListColm.idayAddTime).Visible = m_bIsVisibleAddTime
			'列0 (時刻)
			.Columns(em_LogListColm.idayDspTime).HeaderText = "各パソコン発生時刻"
			.Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
			.Columns(em_LogListColm.idayDspTime).Width = 160
			.Columns(em_LogListColm.idayDspTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
			'列1 (種別)
			.Columns(em_LogListColm.iKind).HeaderText = "種別"
			.Columns(em_LogListColm.iKind).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iKind).Width = 50
			.Columns(em_LogListColm.iKind).SortMode = DataGridViewColumnSortMode.NotSortable
			'列2 (区分)
			.Columns(em_LogListColm.iMode).HeaderText = "区分"
			.Columns(em_LogListColm.iMode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iMode).Width = 60
			.Columns(em_LogListColm.iMode).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(em_LogListColm.iMode).Frozen = True
			'列3 (発生元)
			.Columns(em_LogListColm.iTaskName).HeaderText = "発生元"
			.Columns(em_LogListColm.iTaskName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iTaskName).Width = 85
			.Columns(em_LogListColm.iTaskName).SortMode = DataGridViewColumnSortMode.NotSortable
			'列4 (ＰＣ名)
			.Columns(em_LogListColm.iPcName).HeaderText = "PC名"
			.Columns(em_LogListColm.iPcName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iPcName).Width = 80
			.Columns(em_LogListColm.iPcName).SortMode = DataGridViewColumnSortMode.NotSortable
			'列5 (ＰＣ種別)
			.Columns(em_LogListColm.iPcKindName).HeaderText = "PC機能"
			.Columns(em_LogListColm.iPcKindName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iPcKindName).Width = 110
			.Columns(em_LogListColm.iPcKindName).SortMode = DataGridViewColumnSortMode.NotSortable
			'列6 (コード)
			.Columns(em_LogListColm.iSysNo).HeaderText = "ｺｰﾄﾞ"
			.Columns(em_LogListColm.iSysNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(em_LogListColm.iSysNo).Width = 50
			.Columns(em_LogListColm.iSysNo).SortMode = DataGridViewColumnSortMode.NotSortable
			'列7 (内容)
			.Columns(em_LogListColm.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iMsg).HeaderText = "内容"
			.Columns(em_LogListColm.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iMsg).Width = 800
			.Columns(em_LogListColm.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable
			'列8 (詳細)
			.Columns(em_LogListColm.iSubMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iSubMsg).HeaderText = "詳細"
			.Columns(em_LogListColm.iSubMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(em_LogListColm.iSubMsg).Width = 500
			.Columns(em_LogListColm.iSubMsg).SortMode = DataGridViewColumnSortMode.NotSortable

			If m_bIsCommentOn Then
				'列10 (PCKIND) 非表示
				.Columns(em_LogListColm.iPcKind).HeaderText = "PCKIND"
				.Columns(em_LogListColm.iPcKind).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(em_LogListColm.iPcKind).Width = 80
				.Columns(em_LogListColm.iPcKind).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(em_LogListColm.iPcKind).Visible = False
				'列11 (AutoNo) 非表示
				.Columns(em_LogListColm.iAutoNo).HeaderText = "AutoNo"
				.Columns(em_LogListColm.iAutoNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(em_LogListColm.iAutoNo).Width = 80
				.Columns(em_LogListColm.iAutoNo).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(em_LogListColm.iAutoNo).Visible = False
			End If
		End With
	End Sub


	''' <summary>
	''' 選択行無効化対策 (選択行色替えを無効化する場合は、必須。無しにするとデータセット時の1行目が変な事になる)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLog_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvLog.SelectionChanged
		' 背景色が透明の場合のみキャンセル
		If dgvLog.DefaultCellStyle().SelectionBackColor = Color.Transparent Then CType(sender, DataGridView).ClearSelection()
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
			sql &= "ISNULL(I.発生日付時刻,''),"
			sql &= "ISNULL(M.種別,'0'),"
			sql &= "ISNULL(M.区分,'0'),"
			sql &= "ISNULL(I.発生元,''),"
			sql &= "ISNULL(I.PC名,''),"
			sql &= "ISNULL(P.PC種別,''),"
			sql &= "ISNULL(M.アラームNo,''),"
			sql &= "ISNULL(M.内容,''),"
			sql &= "ISNULL(I.内容,''),"
			sql &= "ISNULL(M.詳細,''), "
			sql &= "ISNULL(I.登録日付,'')"
			If m_bIsCommentOn Then
				sql &= ",ISNULL(P.PCKIND,'0'),"
				sql &= "ISNULL(I.AutoNO,'0')"
			End If
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
		sql &= "FROM " & m_sDB_LOG_I & " I "
		sql &= "LEFT OUTER JOIN " & m_sDB_LOG_M & " M ON I.アラームNo = M.アラームNo "
        sql &= "LEFT OUTER JOIN " & m_sDB_PC_I & " P ON I.PCKIND = P.PCKIND "

	End Sub
	''' <summary>
	''' SQL文生成 (WHERE句)
	''' </summary>
	''' <param name="sql">生成SQL文</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub subSQL_Create_Where(ByRef sql As String)
		Dim strWhere As String = ""				' Where句ワーク
		Dim ii As Integer
		Dim iWk As Integer
		Dim bWk As Boolean

		'''' 時刻フィルタ
		'' 開始
		If chkStartDay.Checked Then
            strWhere &= "AND I.発生日付時刻 >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
		End If
		'' 終了
		If chkEndDay.Checked Then
            strWhere &= "AND I.発生日付時刻 <= '" & dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss") & "' "
		End If


		'''' ＰＣ名フィルタ
		iWk = GetLstCheckCount(m_lstChkPCName)					' SQL簡略化対策
		If iWk = 0 OrElse iWk = m_lstChkPCName.Count Then		' 0件選択 又は 全件選択
			' 何も無し
		Else
			bWk = False											' PCフィルタが1件でも有り時 true
			For ii = 0 To m_lstChkPCName.Count - 1
				If m_lstChkPCName.Item(ii).Checked Then					' PCフィルタ チェック
					If Not bWk Then
						strWhere &= "AND ("
						bWk = True
					Else
						strWhere &= " OR "
					End If

					' PCセット
					strWhere &= " I.PC名 = '" & CStr(m_lstChkPCName.Item(ii).Tag) & "'"
				End If
			Next ii
			If bWk Then
				strWhere &= ") "
			End If
		End If

		'''' PCKindフィルタ
		iWk = GetLstCheckCount(m_lstChkPCKind)					' SQL簡略化対策
		If iWk = 0 OrElse iWk = m_lstChkPCKind.Count Then		' 0件選択 又は 全件選択
			' 何も無し
		Else
			bWk = False											' PCフィルタが1件でも有り時 true
			For ii = 0 To m_lstChkPCKind.Count - 1
				If m_lstChkPCKind.Item(ii).Checked Then					' PCフィルタ チェック
					If Not bWk Then
						strWhere &= "AND ("
						bWk = True
					Else
						strWhere &= " OR "
					End If

					' PCセット
					strWhere &= " I.PCKIND = " & CStr(m_lstChkPCKind.Item(ii).Tag)
				End If
			Next ii
			If bWk Then
				strWhere &= ") "
			End If
		End If

		'''' 区分フィルタ
		iWk = GetLstCheckCount(m_lstChkKubun_A)				' SQL簡略化対策
		iWk += GetLstCheckCount(m_lstChkKubun_S)
		iWk += GetLstCheckCount(m_lstChkKubun_H)
		If iWk = 0 Then										' 0件選択
			' 何も無し
		Else
			Dim bKubun As Boolean = False
			strWhere &= "AND ("
			'''''''''''''''''''''''
			bWk = False											' 区分フィルタが1件でも有り時 true
			For ii = 0 To m_lstChkKubun_A.Count - 1
				If m_lstChkKubun_A.Item(ii).Checked Then				' 区分フィルタ チェック
					If Not bWk Then
						If bKubun Then
							strWhere &= "OR "
						End If
						strWhere &= " (種別= " & CStr(m_nLogModeSyubetu(0)) & " AND ("
						bWk = True
						bKubun = True
					Else
						strWhere &= " OR "
					End If

					' PCセット
					strWhere &= " M.区分 = " & CStr(m_nLogKindKubun(ii))
				End If
			Next ii
			If bWk Then
				strWhere &= ")) "
			End If


			'''''''''''''''''''''''
			bWk = False											' 区分フィルタが1件でも有り時 true
			For ii = 0 To m_lstChkKubun_S.Count - 1
				If m_lstChkKubun_S.Item(ii).Checked Then				' 区分フィルタ チェック
					If Not bWk Then
						If bKubun Then
							strWhere &= "OR "
						End If
						strWhere &= " (種別= " & CStr(m_nLogModeSyubetu(1)) & " AND ("
						bWk = True
						bKubun = True
					Else
						strWhere &= " OR "
					End If

					' PCセット
					strWhere &= " M.区分 = " & CStr(m_nLogKindKubun(ii))
				End If
			Next ii
			If bWk Then
				strWhere &= ")) "
			End If

			'''''''''''''''''''''''
			bWk = False											' 区分フィルタが1件でも有り時 true
			For ii = 0 To m_lstChkKubun_H.Count - 1
				If m_lstChkKubun_H.Item(ii).Checked Then				' 区分フィルタ チェック
					If Not bWk Then
						If bKubun Then
							strWhere &= "OR "
						End If
						strWhere &= " (種別= " & CStr(m_nLogModeSyubetu(2)) & " AND ("
						bWk = True
						bKubun = True
					Else
						strWhere &= " OR "
					End If

					' PCセット
					strWhere &= " M.区分 = " & CStr(m_nLogKindKubun(ii))
				End If
			Next ii
			If bWk Then
				strWhere &= ")) "
			End If

			strWhere &= ")"
		End If


		'''' コードフィルタ
		'' 上下限範囲 & 特定コード抽出
		If IsNumeric(txtCode_From.Text) Or IsNumeric(txtCode_To.Text) Or 0 <> txtCode_Select.TextLength Then
			' 事前準備
			strWhere &= "AND ("
			bWk = False

			' 範囲指定
			If IsNumeric(txtCode_From.Text) And IsNumeric(txtCode_To.Text) Then
				strWhere &= "(M.アラームNo BETWEEN " & CStr(CInt(txtCode_From.Text)) & " AND " & CStr(CInt(txtCode_To.Text)) & ") "
				bWk = True
			ElseIf IsNumeric(txtCode_From.Text) Then
				strWhere &= "(M.アラームNo BETWEEN " & CStr(CInt(txtCode_From.Text)) & " AND 9999) "
				bWk = True
			ElseIf IsNumeric(txtCode_To.Text) Then
				strWhere &= "(M.アラームNo BETWEEN 0 AND " & CStr(CInt(txtCode_To.Text)) & ") "
				bWk = True
			End If

			' 特定コード抽出
			If (0 <> txtCode_Select.TextLength) Then
				Dim strAry() As String = txtCode_Select.Text.Split(Chr(Asc(",")))
				For ii = 0 To strAry.Length - 1
					If IsNumeric(strAry(ii)) Then					' 特定コード抽出有り
						If bWk Then
							strWhere &= " OR "
						End If

						strWhere &= "M.アラームNo=" & CStr(CInt(strAry(ii)))
						bWk = True
					End If
				Next ii
			End If

			' 後処理
			strWhere &= ") "
		End If
		'' 対象外コード指定
		bWk = False
		If (0 <> txtCode_Not.TextLength) Then
			Dim strAry() As String = txtCode_Not.Text.Split(Chr(Asc(",")))
			For ii = 0 To strAry.Length - 1
				If IsNumeric(strAry(ii)) Then					' 対象外コード指定有り
					If Not bWk Then
						strWhere &= "AND ( "
						bWk = True
					Else
						strWhere &= " AND "
					End If

					strWhere &= "M.アラームNo<>" & CStr(CInt(strAry(ii)))
				End If
			Next ii
			If bWk Then strWhere &= ") "
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
			sql &= "ORDER BY I.登録日付 DESC"
			If m_bIsCommentOn Then sql &= ", I.AutoNo DESC"
		End If
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 検索実行
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 検索実行中断
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecCancel_Log()
		m_bLog_ExecCancel = True
	End Sub
	''' <summary>
	''' ログ表示開始時
	''' </summary>
	''' <remarks></remarks>
    Protected Friend Overridable Sub ExecStart()
        '20180510 区分全選択、全解除追加 --->>>
        my_btnFunction(1).Enabled = False
        my_btnFunction(2).Enabled = False
        '20180510 区分全選択、全解除追加 <<<---
        my_btnFunction(3).Enabled = True
        my_btnFunction(4).Enabled = False
        my_btnFunction(11).Enabled = False
        m_bLog_ExecCancel = False
    End Sub
	''' <summary>
	''' ログ表示終了時
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecEnd()
        '20180510 区分全選択、全解除追加 --->>>
        my_btnFunction(1).Enabled = True
        my_btnFunction(2).Enabled = True
        '20180510 区分全選択、全解除追加 <<<---
        my_btnFunction(3).Enabled = False
		my_btnFunction(4).Enabled = True
		my_btnFunction(11).Enabled = True
		m_bLog_ExecCancel = False
	End Sub

	''' <summary>
	''' コメント編集
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecComment()
		If Not m_bIsCommentOn Then Return

		'''' 選択行確認
		Dim nIndex As Integer			' 選択行 (未選択時、-1) 
		If dgvLog.CurrentRow Is Nothing OrElse dgvLog.CurrentRow.Index = -1 Then nIndex = -1
		If -1 <> nIndex Then nIndex = dgvLog.CurrentRow.Index

		'''' 選択データ取得
		Dim typ As frmPop_LogComment.typSelectRowData = Nothing
		If -1 = nIndex Then				' 行未選択
			typ.nAutoNo = 0
			typ.nSysNo = 0
			typ.dayAddTime = Now
			typ.dayDspTime = Now
		Else							' 行選択
			typ.dayAddTime = CDate(dgvLog(em_LogListColm.idayAddTime, nIndex).Value)
			typ.dayDspTime = CDate(dgvLog(em_LogListColm.idayDspTime, nIndex).Value)

			If m_nLogKindKubun(gem_LogKind.iCmt) = CInt(dgvLog(em_LogListColm.iMode, nIndex).Value) Then		' コメントを選択
				typ.nAutoNo = CInt(dgvLog(em_LogListColm.iAutoNo, nIndex).Value)
				typ.nSysNo = CInt(dgvLog(em_LogListColm.iSysNo, nIndex).Value)
				typ.sTaskName = CStr(dgvLog(em_LogListColm.iTaskName, nIndex).Value)
				typ.sPcName = CStr(dgvLog(em_LogListColm.iPcName, nIndex).Value)
				typ.nPcKind = CInt(dgvLog(em_LogListColm.iPcKind, nIndex).Value)
				typ.sMsg = CStr(dgvLog(em_LogListColm.iMsg, nIndex).Value)
			Else																			' コメント以外
				typ.nAutoNo = 0
				typ.nSysNo = 0
				typ.sTaskName = ""
				typ.sPcName = ""
				typ.nPcKind = 0
				typ.sMsg = ""
			End If
		End If

		'''' コメント編集画面表示
		Using frm As New frmPop_LogComment
			' 必要な情報をセット
			frm.tProp_SelectRowData = typ		' 選択情報セット
			frm.tProp_DbSession = Me.m_sDbSession

			' 画面表示
			Call frm.ShowDialog()

			' ログ情報編集時 画面再描画
			Dim bAns As Boolean
			bAns = frm.Prop_IsDataUpDate()		' 何かしらの変更を行った場合 true
			If Not bAns Then Return
			MyBase.SetEvFunctionButton(5 - 1, True, Nothing)
		End Using
	End Sub

	''' <summary>
	''' 検索実行
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecSelect_Log()
		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String = ""
		Dim ii As Integer = 0
		Dim rowCnt As Integer = 0
		Dim rowMaxCnt As Integer = 0

		Try
			'''' 初期化
			Call ExecStart()
			dgvLog.RowCount = 0
			If Not tcls_DB.Open() Then Exit Sub
			Application.DoEvents()

			'''' リスト件数取得
            Call subSQL_Create(strSQL, False)
            If Not tcls_DB.GetSelectKey(strSQL, rowMaxCnt) Then Exit Sub
			' パフォーマンスの為に、データグリッドビューに行をセット
			If nudRowMaxCount.Value < rowMaxCnt Then		' 上限超え
				rowMaxCnt = CInt(nudRowMaxCount.Value)
				lblRowCount.BackColor = Color.Pink
			Else
				rowMaxCnt = rowMaxCnt
				lblRowCount.BackColor = SystemColors.Control
			End If
			dgvLog.RowCount = rowMaxCnt
			lblRowCount.Text = rowMaxCnt.ToString("#,##0")
			prbReadRow.Minimum = 0
			prbReadRow.Value = 0
			prbReadRow.Maximum = rowMaxCnt
			prbReadRow.Visible = True
            Application.DoEvents()

			'''' データを取得
			Call subSQL_Create(strSQL, True, rowMaxCnt)
			sqlRead = tcls_DB.SelectExecute(strSQL)	' Select文実行
			If sqlRead Is Nothing Then Exit Sub
			' データ取り出し
			While sqlRead.Read()

				' データ取り出し
				dgvLog(em_LogListColm.idayDspTime, rowCnt).Value = sqlRead.GetDateTime(0).ToString("yyyy/MM/dd HH:mm:ss")
				dgvLog(em_LogListColm.iKind, rowCnt).Value = sqlRead.GetInt32(1)
				dgvLog(em_LogListColm.iMode, rowCnt).Value = sqlRead.GetInt32(2)
				dgvLog(em_LogListColm.iTaskName, rowCnt).Value = sqlRead.GetString(3)
				dgvLog(em_LogListColm.iPcName, rowCnt).Value = sqlRead.GetString(4)
				dgvLog(em_LogListColm.iPcKindName, rowCnt).Value = sqlRead.GetString(5)
				dgvLog(em_LogListColm.iSysNo, rowCnt).Value = sqlRead.GetInt32(6)
				' 固定メッセージが無しの場合、内容の先頭にスペースが勝手についてしまうのをヤメ
				'dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(7) & " " & sqlRead.GetString(8)
				If "" <> sqlRead.GetString(7) Then
					dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(7) & " " & sqlRead.GetString(8)
				Else
					dgvLog(em_LogListColm.iMsg, rowCnt).Value = sqlRead.GetString(8)
				End If
				dgvLog(em_LogListColm.iSubMsg, rowCnt).Value = sqlRead.GetString(9)
				dgvLog(em_LogListColm.idayAddTime, rowCnt).Value = sqlRead.GetDateTime(10).ToString("yyyy/MM/dd HH:mm:ss")

				' コメント機能ON時のみ
				If m_bIsCommentOn Then
					dgvLog(em_LogListColm.iPcKind, rowCnt).Value = sqlRead.GetInt32(11)
					dgvLog(em_LogListColm.iAutoNo, rowCnt).Value = sqlRead.GetInt32(12)
				End If


                ' プログレスバー
                '20180510 プログレスバー処理変更 --->>>
                'prbReadRow.PerformStep()
                'If 0 = (rowCnt Mod 100) Then
                '    Application.DoEvents()
                'End If
                If 0 = (rowCnt Mod prbReadRow.Step) Then
                    prbReadRow.PerformStep()
                    Application.DoEvents()
                End If
                '20180510 プログレスバー処理変更 <<<---

				'''' 次行準備
				rowCnt += 1
				If rowCnt >= rowMaxCnt Then Exit Sub
				If m_bLog_ExecCancel Then					' キャンセル
					' キャンセルだったら 表示している行数だけにする
					dgvLog.RowCount = rowCnt
					lblRowCount.Text = rowCnt.ToString
					Exit Sub
				End If
			End While

			'''' DB切断
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
	''' 表示エリアのみの描画指定
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLog_CellFormatting(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs) Handles dgvLog.CellFormatting
		' 種別から文字に変更
		If e.ColumnIndex = em_LogListColm.iKind Then
			Dim strVal As String = ""
			Select Case CInt(e.Value)
				Case m_nLogModeSyubetu(gem_LogMode.iAns)
					strVal = "全般"
				Case m_nLogModeSyubetu(gem_LogMode.iSoft)
					strVal = "ｿﾌﾄ"
				Case m_nLogModeSyubetu(gem_LogMode.iHard)
					strVal = "ﾊｰﾄﾞ"
				Case Else
					strVal = ""
			End Select
			e.Value = strVal
		End If

		' 区分から文字に変更
		If e.ColumnIndex = em_LogListColm.iMode Then
			Dim strVal As String = ""
			Select Case CInt(e.Value)
				Case m_nLogKindKubun(gem_LogKind.iEvt)
					strVal = "ｲﾍﾞﾝﾄ"
				Case m_nLogKindKubun(gem_LogKind.iJyu)
					strVal = "重故障"
				Case m_nLogKindKubun(gem_LogKind.iKei)
					strVal = "軽故障"
				Case m_nLogKindKubun(gem_LogKind.iCmt)
					strVal = "ｺﾒﾝﾄ"
				Case Else
					strVal = ""
			End Select
			e.Value = strVal
		End If

		' セル文字色を指定 (いろいろやった結果これが一番早い)
		If e.CellStyle.ForeColor = Drawing.SystemColors.ControlText Then		'デフォルト色の場合のみ設定
			Dim color As Color = Drawing.Color.Black
			Select Case CInt(dgvLog(em_LogListColm.iMode, e.RowIndex).Value)
				Case m_nLogKindKubun(gem_LogKind.iEvt)
					color = m_clLogKindColor(gem_LogKind.iEvt)
				Case m_nLogKindKubun(gem_LogKind.iJyu)
					color = m_clLogKindColor(gem_LogKind.iJyu)
				Case m_nLogKindKubun(gem_LogKind.iKei)
					color = m_clLogKindColor(gem_LogKind.iKei)
				Case m_nLogKindKubun(gem_LogKind.iCmt)
					color = m_clLogKindColor(gem_LogKind.iCmt)
				Case Else
			End Select
			If e.CellStyle.ForeColor = color Then Exit Sub
			e.CellStyle.ForeColor = color

		End If
	End Sub


    ''' <summary>
    ''' CSV出力
    ''' </summary>
	''' <param name="strFolder">保存ファイルのフォルダパス</param>
	''' <param name="strName">保存ファイルのファイル名</param>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""						'1行のデータ

		'' フルパス
		strFPath = IO.Path.Combine(strFolder, strName)

		'' フォルダの有無チェック(無かったら作ろう)
		tMod.CheckFolder(strFolder)

		'' CSV出力
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' オープン
				cls_Stram.Open()

                strData = ""
                'ヘッダー出力
                For ii As Integer = 0 To dgvLog.Columns.Count - 1
                    strData &= dgvLog.Columns(ii).HeaderText & ","
                Next ii

                '最後の","を排除
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)


				'データ出力
				For ii As Integer = 0 To dgvLog.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvLog.ColumnCount - 1
						'区分の場合は、別処理
                        If jj = 2 Then
                            Select Case CInt(dgvLog(jj, ii).Value)
                                Case m_nLogModeSyubetu(gem_LogMode.iAns)
                                    strData &= "全般,"
                                Case m_nLogModeSyubetu(gem_LogMode.iSoft)
                                    strData &= "ｿﾌﾄ,"
                                Case m_nLogModeSyubetu(gem_LogMode.iHard)
                                    strData &= "ﾊｰﾄﾞ,"
                                Case Else
                                    strData &= ","
                            End Select

                        ElseIf jj = 3 Then
                            Select Case CInt(dgvLog(jj, ii).Value)
                                Case m_nLogKindKubun(gem_LogKind.iEvt)
                                    strData &= "ｲﾍﾞﾝﾄ,"
                                Case m_nLogKindKubun(gem_LogKind.iJyu)
                                    strData &= "重故障,"
                                Case m_nLogKindKubun(gem_LogKind.iKei)
                                    strData &= "軽故障,"
                                Case m_nLogKindKubun(gem_LogKind.iCmt)
                                    strData &= "ｺﾒﾝﾄ,"
                                Case Else
                                    strData &= ","
                            End Select
                        Else
                            strData &= dgvLog(jj, ii).Value.ToString & ","
                        End If
					Next jj

                    '最後の","を排除
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

    '20180510 区分全選択、全解除追加 --->>>
    ''' <summary>
    ''' 区分チェック全選択
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub btnAllOn_Click()

        For ii As Integer = 0 To m_lstChkKubun_A.Count - 1
            m_lstChkKubun_A(ii).CheckState = CheckState.Checked
        Next

        For ii As Integer = 0 To m_lstChkKubun_S.Count - 1
            m_lstChkKubun_S(ii).CheckState = CheckState.Checked
        Next

        For ii As Integer = 0 To m_lstChkKubun_H.Count - 1
            m_lstChkKubun_H(ii).CheckState = CheckState.Checked
        Next

    End Sub

    ''' <summary>
    ''' 区分チェック全解除
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub btnAllOff_Click()

        For ii As Integer = 0 To m_lstChkKubun_A.Count - 1
            m_lstChkKubun_A(ii).CheckState = CheckState.Unchecked
        Next

        For ii As Integer = 0 To m_lstChkKubun_S.Count - 1
            m_lstChkKubun_S(ii).CheckState = CheckState.Unchecked
        Next

        For ii As Integer = 0 To m_lstChkKubun_H.Count - 1
            m_lstChkKubun_H(ii).CheckState = CheckState.Unchecked
        Next

    End Sub
    '20180510 区分全選択、全解除追加 <<<---
End Class
