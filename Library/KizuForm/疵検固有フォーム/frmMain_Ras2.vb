'*******************************************************************************
' RAS画面の親クラス
'	[Ver]
'		Ver.01    2010/01/05  新規作成
'
'	[メモ]
'		機器状態は登録されるコントロールのタグに設定される
'*******************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' RAS画面の親クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Ras2
	'/ /////////////////////////////////////////////////////////////////////////
	'/ 列挙体
	'/ /////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 機器区分
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_RAS_TYPE
		PC = 0																	' PCの状態を示す
		KIKI																	' 機器の状態を示す
	End Enum

	''' <summary>
	''' 機器状態の表示区分
	''' </summary>
	''' <remarks>RAS画面の各ラベルやボタンに表示する内容</remarks>
	Public Enum EM_RAS_DSPTYPE
		DSP_STRING = 0															' 文字列表示
		DSP_VALUE																' 値表示
		DSP_AUTO																' 自動判別
	End Enum

	''' <summary>
	''' 機器状態設定先
	''' </summary>
	''' <remarks></remarks>
	Public Enum EM_RAS_SETTYPE
		DSP_CONTROL = 0															' 登録されているコントロールに設定
		DSP_LINE																' (PC状態表示用) ラインに設定
	End Enum

	''' <summary>
	''' PC機能名表示区分
	''' </summary>
	''' <remarks>PCボタンに表示するPC機能名の表示区分</remarks>
	Public Enum EM_DSP_PCKIND_NAME
		PROGRAM = 0																' ソフト固定の名前を表示
		PCKIND																	' PC機能の名称を表示
	End Enum

	''' <summary>
	''' PC名表示区分
	''' </summary>
	''' <remarks>PCボタンに表示するPC名の表示区分</remarks>
	Public Enum EM_DSP_PC_NAME
		SYSTEM = 0																' PC名を表示
		USER																	' パソコン名を表示
	End Enum

	'/ /////////////////////////////////////////////////////////////////////////
	'/ 構造体
	'/ /////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 機器情報
	''' </summary>
	''' <remarks></remarks>
	Public Structure STATUS_INF
		Dim nKikiId As Integer													' 機器ID
		Dim nStatus As Integer													' 機器状態
		Dim nKikiErr As Integer													' 機器異常度
		Dim sDspData As String													' 画面表示データ
		Dim nPcKind As Integer													' PCKIND
		Dim nOffLine As Integer													' PC区分
		' 2014.05.21 竹山 VRCL用全長保存システム追加     >>>>>>>>
		Dim nKendErr As Integer													' 検査異常度
		' 2014.05.21 竹山 VRCL用全長保存システム追加     <<<<<<<<
	End Structure

	''' <summary>
	''' PC情報
	''' </summary>
	''' <remarks></remarks>
	Public Structure PC_INF
		Dim strNameSystem As String												' PC名称 (ネットワーク上のコンピュータ名)
		Dim strNameUser As String												' パソコン名称 (表示上のコンピュータ名)
	End Structure

	''' <summary>
	''' PC表示コントロール情報
	''' </summary>
	''' <remarks></remarks>
	Public Structure PC_CTRL
		Dim strName As String													' ボタン名
		Dim btnPcName As Button													' PCボタン
		Dim lblShadow As Label													' PCボタン影
		Dim lblLine As List(Of Label)											' 回線ラベル

		''' <summary>
		''' コントロール情報を初期化します
		''' </summary>
		''' <remarks></remarks>
		Public Sub initialize()
			lblLine = New List(Of Label)
		End Sub
	End Structure

	''' <summary>
	''' 機器状態表示コントロール情報
	''' </summary>
	''' <remarks></remarks>
	Private Structure KIKI_CTRL
		Dim nKikiType As EM_RAS_TYPE											' 機器区分
		Dim nSetType As EM_RAS_SETTYPE											' 状態設定先
		Dim nDspType As EM_RAS_DSPTYPE											' 機器表示区分
		Dim lstCtrl As List(Of Control)											' 1つの機器状態を表示するコントロール一覧

		''' <summary>
		''' コントロール情報を初期化します
		''' </summary>
		''' <remarks></remarks>
		Public Sub initialize()
			lstCtrl = New List(Of Control)
		End Sub
	End Structure

	'/ /////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////
	Private m_dctCtrlPC As New Dictionary(Of Integer, PC_CTRL)					' PC状態表示コントロール一覧 (キー:PCKIND)
	Private m_dctCtrlKiki As New Dictionary(Of Integer, KIKI_CTRL)				' 機器状態表示コントロール一覧 (キー:機器ID)
	Private m_dctPcInf As New Dictionary(Of Integer, PC_INF)					' PC情報一覧

	Private m_sDB_PC_INF As String = DB_PC_INF									' DBテーブル PC種別
	Private m_sDB_PC_M As String = DB_PC_MASTER									' DBテーブル PCマスター
	Private m_sDB_STATUS_I As String = DB_STATUS_INF							' DBテーブル 機器状態
	Private m_sDB_STATUS_M As String = DB_STATUS_MASTER							' DBテーブル 機器状態マスター
	Private m_sDB_COMMON As String = DB_COMMON_MASTER							' DBテーブル 疵検定数マスター

	Private m_nToukatu As Integer												' 統括PCのPCKIND
	Private m_nAllResetID As Integer											' 全PCのリセットするPCKINDをセット(蓄積,蓄積と統括が一緒の場合は統括)
	Private m_nLogDspCnt As Integer												' ログ表示数
	Private m_dgvLog As DataGridView											' ログ表示データグリッド
    Private m_strResetPass As String = ""                                       ' PCリセットパスワード
	Private m_nEventExit As Integer = FACT_KS_MASTER_06							' システム終了
	Private m_nEventReset As Integer = FACT_KS_MASTER_07						' PC再起動のイベント番号
	Private m_nEventLogOff As Integer = FACT_KS_MASTER_08						' PCログオフのイベント番号
	Private m_nLineID As Integer = LINE_ID										' ラインID
	Private m_nNoa As Integer													' NOA接続フラグ (0:オンライン 1:遠隔 9:デバッグ)
	Private m_sDbSession As String = INI_DB										' DBセクション名
	Private m_sMasterTask As String = KS_MASTER									' PCマスター管理プロセス名
	Private m_sMasterPC As String = MASTER_PC_NAME								' PCマスター管理が起動するPC名
	Private m_nDspPcKindName As EM_DSP_PCKIND_NAME = EM_DSP_PCKIND_NAME.PCKIND	' PCボタンに表示するPC機能名表示区分
	Private m_nDspPcName As EM_DSP_PC_NAME = EM_DSP_PC_NAME.USER				' PCボタンに表示PC名区分

	Protected Friend m_lstItem As New List(Of Control)							' 色表示するコントロールのリスト

	''' <summary>
	''' PC種別テーブル名の取得と設定
	''' </summary>
	''' <value>PC種別テーブル名</value>
	''' <returns>PC種別テーブル名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_PC_INF) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PC_INF のテーブル名称")> _
	Public Property tProp_TableName_PcInf() As String
		Get
			Return m_sDB_PC_INF
		End Get
		Set(ByVal Value As String)
			m_sDB_PC_INF = Value
		End Set
	End Property
	''' <summary>
	''' PCマスターテーブル名の取得と設定
	''' </summary>
	''' <value>PCマスターテーブル名</value>
	''' <returns>PCマスターテーブル名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_PC_MASTER) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PC_MASTER のテーブル名称")> _
	Public Property tProp_TableName_PcMaster() As String
		Get
			Return m_sDB_PC_M
		End Get
		Set(ByVal Value As String)
			m_sDB_PC_M = Value
		End Set
	End Property

	''' <summary>
	''' 機器状態テーブル名の取得と設定
	''' </summary>
	''' <value>機器状態テーブル名</value>
	''' <returns>機器状態テーブル名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_STATUS_INF) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("STATUS_INF のテーブル名称")> _
	Public Property tProp_TableName_StatusInf() As String
		Get
			Return m_sDB_STATUS_I
		End Get
		Set(ByVal Value As String)
			m_sDB_STATUS_I = Value
		End Set
	End Property

	''' <summary>
	''' 機器マスターテーブル名の取得と設定
	''' </summary>
	''' <value>機器マスターテーブル名</value>
	''' <returns>機器マスターテーブル名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_STATUS_MASTER) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("STATUS_MASTER のテーブル名称")> _
	Public Property tProp_TableName_StatusMaster() As String
		Get
			Return m_sDB_STATUS_M
		End Get
		Set(ByVal Value As String)
			m_sDB_STATUS_M = Value
		End Set
	End Property


	''' <summary>
	''' 疵検定数マスターテーブル名の取得と設定
	''' </summary>
	''' <value>疵検定数マスターテーブル名</value>
	''' <returns>疵検定数マスターテーブル名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(DB_COMMON_MASTER) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("COMMON_MASTER のテーブル名称")> _
	Public Property tProp_TableName_Com_Master() As String
		Get
			Return m_sDB_STATUS_M
		End Get
		Set(ByVal Value As String)
			m_sDB_STATUS_M = Value
		End Set
	End Property

	''' <summary>
	''' 統括PCのPCKIND値の取得と設定
	''' </summary>
	''' <value>統括PCのPCKIND値</value>
	''' <returns>統括PCのPCKIND値</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("統括PCのPCKIND値")> _
	Public Property tProp_TokatuID() As Integer
		Get
			Return m_nToukatu
		End Get
		Set(ByVal Value As Integer)
			m_nToukatu = Value
		End Set
	End Property

	''' <summary>
	''' 全PCリセット用PCKIND値の取得と設定
	''' </summary>
	''' <value>全PCリセット用PCKIND値</value>
	''' <returns>全PCリセット用PCKIND値</returns>
	''' <remarks>通常は、蓄積。蓄積がいない場合は統括</remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("全PCリセット用のPCKIND")> _
	Public Property tProp_AllPCRestID() As Integer
		Get
			Return m_nAllResetID
		End Get
		Set(ByVal Value As Integer)
			m_nAllResetID = Value
		End Set
	End Property

	''' <summary>
	''' ログ表示用データグリッドビューの取得と設定
	''' </summary>
	''' <value>ログ表示用データグリッドビュー</value>
	''' <returns>ログ表示用データグリッドビュー</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("ログ表示用データグリッドビュー")> _
	Public Property tProp_DspGrid() As DataGridView
		Get
			Return m_dgvLog
		End Get
		Set(ByVal Value As DataGridView)
			m_dgvLog = Value
		End Set
	End Property

	''' <summary>
	''' PCリセットパスワードの取得と設定
	''' </summary>
	''' <value>PCリセットパスワード</value>
	''' <returns>PCリセットパスワード</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PCリセットパスワード")> _
	Public Property tProp_Pass() As String
		Get
			Return m_strResetPass
		End Get
		Set(ByVal Value As String)
			m_strResetPass = Value
		End Set
	End Property

	''' <summary>
	''' PCリセットイベントNoの取得と設定
	''' </summary>
	''' <value>PCリセットイベントNo</value>
	''' <returns>PCリセットイベントNo</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PCリセットイベントNo")> _
	Public Property tProp_ResetEventNo() As Integer
		Get
			Return m_nEventReset
		End Get
		Set(ByVal Value As Integer)
			m_nEventReset = Value
		End Set
	End Property

	''' <summary>
	''' PCログオフイベントNoの取得と設定
	''' </summary>
	''' <value>PCログオフイベントNo</value>
	''' <returns>PCログオフイベントNo</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PCログオフイベントNo")> _
	Public Property tProp_LogOffEventNo() As Integer
		Get
			Return m_nEventLogOff
		End Get
		Set(ByVal Value As Integer)
			m_nEventLogOff = Value
		End Set
	End Property

	''' <summary>
	''' システム終了イベントNoの取得と設定
	''' </summary>
	''' <value>システム終了イベントNo</value>
	''' <returns>システム終了イベントNo</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("システム終了イベントNo")> _
	Public Property tProp_SystemExitEventNo() As Integer
		Get
			Return m_nEventExit
		End Get
		Set(ByVal Value As Integer)
			m_nEventExit = Value
		End Set
	End Property

	''' <summary>
	''' ラインIDの取得と設定
	''' </summary>
	''' <value>ラインID</value>
	''' <returns>ラインID</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("ラインID")> _
	Public Property tProp_LineID() As Integer
		Get
			Return m_nLineID
		End Get
		Set(ByVal Value As Integer)
			m_nLineID = Value
		End Set
	End Property

	''' <summary>
	''' NOA接続フラグの設定と取得
	''' </summary>
	''' <value>0:オンライン 1:遠隔 9:デバッグ</value>
	''' <returns>0:オンライン 1:遠隔 9:デバッグ</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("NOA接続フラグ 0:オンライン 1:遠隔 9:デバッグ")> _
	Public Property tProp_Noa() As Integer
		Get
			Return m_nNoa
		End Get
		Set(ByVal Value As Integer)
			m_nNoa = Value
		End Set
	End Property

	''' <summary>
	''' RAS画面表示ログ数の設定と取得
	''' </summary>
	''' <value>表示ログ数</value>
	''' <returns>表示ログ数</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("RAS画面表示ログ数")> _
	Public Property tProp_LogDspCnt() As Integer
		Get
			Return m_nLogDspCnt
		End Get
		Set(ByVal Value As Integer)
			m_nLogDspCnt = Value
		End Set
	End Property

	''' <summary>
	''' 疵検タスク設定ファイル内接続先DBアドレスが書かれたセクション名の取得と設定
	''' </summary>
	''' <value>DBセクション名</value>
	''' <returns>DBセクション名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(INI_DB) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("疵検タスク設定ファイル内接続先DBアドレスが書かれたセクション名")> _
	Public Property tProp_DbSession() As String
		Get
			Return m_sDbSession
		End Get
		Set(ByVal Value As String)
			m_sDbSession = Value
		End Set
	End Property

	''' <summary>
	''' PCマスター管理プロセス名の取得と設定
	''' </summary>
	''' <value>PCマスター管理プロセス名</value>
	''' <returns>PCマスター管理プロセス名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(KS_MASTER) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PCマスター管理プロセス名")> _
	Public Property tProp_MasterTask() As String
		Get
			Return m_sMasterTask
		End Get
		Set(ByVal Value As String)
			m_sMasterTask = Value
		End Set
	End Property

	''' <summary>
	''' PCマスター管理が起動するPC名の取得と設定
	''' </summary>
	''' <value>PCマスター管理が起動するPC名</value>
	''' <returns>PCマスター管理が起動するPC名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(MASTER_PC_NAME) _
	, System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	, System.ComponentModel.Description("PCマスター管理が起動するPC名")> _
	 Public Property tProp_MasterPC() As String
		Get
			Return m_sMasterPC
		End Get
		Set(ByVal Value As String)
			m_sMasterPC = Value
		End Set
	End Property

	''' <summary>
	'''  PCボタンに表示するPC機能の名称区分の取得と設定
	''' </summary>
	''' <value>0:プログラム固有 1:PC種別名</value>
	''' <returns>0:プログラム固有 1:PC種別名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(1) _
	, System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	, System.ComponentModel.Description("PCボタンに表示するPC機能の名称区分 0:プログラム固有 1:PC種別名")> _
	 Public Property tProp_PcKindName() As EM_DSP_PCKIND_NAME
		Get
			Return m_nDspPcKindName
		End Get
		Set(ByVal Value As EM_DSP_PCKIND_NAME)
			m_nDspPcKindName = Value
		End Set
	End Property

	''' <summary>
	'''  パソコン名称の使用設定(True:PC_INFのPC種別,False:プログラムの設定)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks>既設との互換のため残す</remarks>
	Public Property tProp_PcKindNameFlg() As Boolean
		Get
			If tProp_PcKindName = EM_DSP_PCKIND_NAME.PCKIND Then
				Return True
			Else
				Return False
			End If
		End Get
		Set(ByVal Value As Boolean)
			If Value = True Then
				tProp_PcKindName = EM_DSP_PCKIND_NAME.PCKIND
			Else
				tProp_PcKindName = EM_DSP_PCKIND_NAME.PROGRAM
			End If
		End Set
	End Property

	''' <summary>
	''' PCボタンに表示するPC名の表示区分の取得と設定
	''' </summary>
	''' <value>0:PC名 1:パソコン名</value>
	''' <returns>0:PC名 1:パソコン名</returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(1) _
	, System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	, System.ComponentModel.Description("PCボタンに表示するPC名の表示区分の取得と設定 0:PC名 1:パソコン名")> _
	 Public Property tProp_PcName() As EM_DSP_PC_NAME
		Get
			Return m_nDspPcName
		End Get
		Set(ByVal Value As EM_DSP_PC_NAME)
			m_nDspPcName = Value
		End Set
	End Property

	''' <summary>
	'''  パソコン名称の使用設定(True:PC_MASTERのパソコン名称,False:PC_MASTERのPC名)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property tProp_PcNameFlg() As Boolean
		Get
			If m_nDspPcName = EM_DSP_PC_NAME.USER Then
				Return True
			Else
				Return False
			End If
		End Get
		Set(ByVal Value As Boolean)
			If Value = True Then
				m_nDspPcName = EM_DSP_PC_NAME.USER
			Else
				m_nDspPcName = EM_DSP_PC_NAME.SYSTEM
			End If
		End Set
	End Property


	''' <summary>
	''' PC名ボタン、回線情報を登録
	''' </summary>
	''' <param name="nPcKind">PCKIND</param>
	''' <param name="btnPc">PCボタンコントロール</param>
	''' <param name="lblPcShadow">PCボタンの影ラベル</param>
	''' <param name="strName">ボタン名</param>
	''' <param name="lblLine1">回線ラベル</param>
	''' <param name="lblLine2">回線ラベル</param>
	''' <param name="lblLine3">回線ラベル</param>
	''' <param name="lblLine4">回線ラベル</param>
	''' <param name="lblLine5">回線ラベル</param>
	''' <param name="lblLine6">回線ラベル</param>
	''' <param name="lblLine7">回線ラベル</param>
	''' <param name="lblLine8">回線ラベル</param>
	''' <param name="lblLine9">回線ラベル</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddButtonPCName(ByVal nPcKind As Integer, ByVal btnPc As Button, _
	   ByVal lblPcShadow As Label, ByVal strName As String, _
	   Optional ByVal lblLine1 As Label = Nothing, _
	   Optional ByVal lblLine2 As Label = Nothing, Optional ByVal lblLine3 As Label = Nothing, _
	   Optional ByVal lblLine4 As Label = Nothing, Optional ByVal lblLine5 As Label = Nothing, _
	   Optional ByVal lblLine6 As Label = Nothing, Optional ByVal lblLine7 As Label = Nothing, _
	   Optional ByVal lblLine8 As Label = Nothing, Optional ByVal lblLine9 As Label = Nothing)

		Dim ctrl As New PC_CTRL
		With ctrl
			.initialize()														' PCコントロール情報初期化

			.strName = strName													' PC名称登録
			.btnPcName = btnPc													' PCボタン登録
			.lblShadow = lblPcShadow											' PCボタンの影登録

			If Not lblLine1 Is Nothing Then .lblLine.Add(lblLine1)
			If Not lblLine2 Is Nothing Then .lblLine.Add(lblLine2)
			If Not lblLine3 Is Nothing Then .lblLine.Add(lblLine3)
			If Not lblLine4 Is Nothing Then .lblLine.Add(lblLine4)
			If Not lblLine5 Is Nothing Then .lblLine.Add(lblLine5)
			If Not lblLine6 Is Nothing Then .lblLine.Add(lblLine6)
			If Not lblLine7 Is Nothing Then .lblLine.Add(lblLine7)
			If Not lblLine8 Is Nothing Then .lblLine.Add(lblLine8)
			If Not lblLine9 Is Nothing Then .lblLine.Add(lblLine9)
		End With

		m_dctCtrlPC.Add(nPcKind, ctrl)											' PCKINDをキーにしてボタンコントロールを登録
		AddHandler btnPc.Click, AddressOf btn_Click								' クリックイベントハンドラに追加
	End Sub

	''' <summary>
	''' 機器に対するコントロールの登録 (コントロールが複数の場合)
	''' </summary>
	''' <param name="nKikiID">機器ID</param>
	''' <param name="lstData">コントロール</param>
	''' <param name="nKikiType">機器区分</param>
	''' <param name="nSetType">状態設定区分</param>
	''' <param name="nDspType">表示区分</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddControlKiki( _
	 ByVal nKikiID As Integer, _
	 ByVal lstData As List(Of Control), _
	 Optional ByVal nKikiType As EM_RAS_TYPE = EM_RAS_TYPE.KIKI, _
	 Optional ByVal nSetType As EM_RAS_SETTYPE = EM_RAS_SETTYPE.DSP_CONTROL, _
	 Optional ByVal nDspType As EM_RAS_DSPTYPE = EM_RAS_DSPTYPE.DSP_STRING)

		Dim ctrl As New KIKI_CTRL												' 機器表示コントロール作成
		With ctrl
			.lstCtrl = lstData													' 機器表示コントロール一覧登録
			.nKikiType = nKikiType												' 機器区分
			.nSetType = nSetType												' 状態設定区分
			.nDspType = nDspType												' 表示区分
		End With

		m_dctCtrlKiki.Add(nKikiID, ctrl)										' 機器表示コントロールを登録
	End Sub

	''' <summary>
	''' 機器状態に紐づくコントロールを登録(コントロールが複数の場合)
	''' </summary>
	''' <param name="nKikiID">PCKIND</param>
	''' <param name="lstData">コントロール</param>
	''' <remarks>既設用のラッパ関数</remarks>
	Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal lstData As List(Of Control))
		AddControlKiki(nKikiID, lstData)
	End Sub

	''' <summary>
	''' 機器に対するコントロールの登録 (コントロールが１つの場合)
	''' </summary>
	''' <param name="nKikiID">機器ID</param>
	''' <param name="ctrl">コントロール</param>
	''' <param name="nKikiType">機器区分</param>
	''' <param name="nSetType">状態設定区分</param>
	''' <param name="nDspType">表示区分</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddControlKiki( _
	 ByVal nKikiID As Integer, _
	 ByVal ctrl As Control, _
	 Optional ByVal nKikiType As EM_RAS_TYPE = EM_RAS_TYPE.KIKI, _
	 Optional ByVal nSetType As EM_RAS_SETTYPE = EM_RAS_SETTYPE.DSP_CONTROL, _
	 Optional ByVal nDspType As EM_RAS_DSPTYPE = EM_RAS_DSPTYPE.DSP_STRING)

		Dim list As New List(Of Control)										' リスト管理なのでリストを作成する
		list.Add(ctrl)

		AddControlKiki(nKikiID, list, nKikiType, nSetType, nDspType)			' 機器登録
	End Sub

	''' <summary>
	''' 機器状態に紐づくコントロールを登録(コントロールが一つ場合)
	''' </summary>
	''' <param name="nKikiID">PCKIND</param>
	''' <param name="ctlData">コントロール</param>
	''' <remarks>既設の為のラッパ関数</remarks>
	Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal ctlData As Control)
		AddControlKiki(nKikiID, ctlData)
	End Sub

	''' <summary>
	''' 機器に対するコントロールの登録 (PC用)
	''' </summary>
	''' <param name="nKikiID">機器ID</param>
	''' <param name="bData">PCフラグ(True:PCボタンに異常フラグを立てる,False:回線ラベルに異常フラグを立てる</param>
	''' <remarks>既設の為に残す</remarks>
	Protected Friend Overridable Sub AddControlKiki(ByVal nKikiID As Integer, ByVal bData As Boolean)
		Dim nSetType As EM_RAS_SETTYPE = EM_RAS_SETTYPE.DSP_CONTROL
		If bData = False Then
			nSetType = EM_RAS_SETTYPE.DSP_LINE
		End If

		'機器登録
		AddControlKiki(nKikiID, CType(Nothing, List(Of Control)), EM_RAS_TYPE.PC, nSetType, EM_RAS_DSPTYPE.DSP_STRING)
	End Sub

	''' <summary>
	''' 機器状態に紐づくコントロールを登録
	''' </summary>
	''' <param name="nKikiID">PCKIND</param>
	''' <param name="bData">PCフラグ(True:PCボタンに異常フラグを立てる,False:回線ラベルに異常フラグを立てる</param>
	''' <remarks>既設の為のラッパ関数</remarks>
	Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal bData As Boolean)
		AddControlKiki(nKikiID, bData)
	End Sub

	''' <summary>
	''' PCボタンへPC名を登録
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetPCName()
		Dim tcls_DB As tClass.tClass_SQLServer = Nothing
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Try
			tcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
			If Not tcls_DB.Open() Then											' DBオープン。失敗したらあきらめる
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' PC情報取得SQL作成
			'-------------------------------------------------------------------
			Dim strSQL As String = ""
			strSQL = ""
			strSQL &= " SELECT A.PCKIND,"
			strSQL &= "        B.PC名,"
			strSQL &= "        A.PC種別,"
			strSQL &= "        B.パソコン名称"
			strSQL &= " FROM " & m_sDB_PC_INF & " A " & "INNER JOIN " & m_sDB_PC_M & " B "
			strSQL &= " ON A.PCID = B.PCID "

			sqlRead = tcls_DB.SelectExecute(strSQL)								' PC名をＤＢから取得する
			If sqlRead Is Nothing Then											' 取得失敗時は、あきらめる
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' PC情報設定とPCボタン名の設定
			'-------------------------------------------------------------------
			m_dctPcInf.Clear()													' PC情報初期化
			While sqlRead.Read()												' Trueの間はデータがあるs
				Dim nPcKind As Integer = sqlRead.GetInt32(0)					' PCKINDを取得

				If m_dctCtrlPC.ContainsKey(nPcKind) = True Then					' 登録されているPCなら設定

					Dim pc_inf As PC_INF = Nothing
					pc_inf.strNameSystem = sqlRead.GetString(1)
					pc_inf.strNameUser = sqlRead.GetString(3)

					Dim strBtnText As String = ""
					If m_nDspPcKindName = EM_DSP_PCKIND_NAME.PCKIND Then		' PC種別を表示するのであればPC種別を取得
						strBtnText = sqlRead.GetString(2)
					Else														' プログラム固有の機能名を表示
						strBtnText = m_dctCtrlPC(nPcKind).strName
					End If

					strBtnText &= vbCrLf										' 機能名とPC名で改行する

					If m_nDspPcName = EM_DSP_PC_NAME.USER Then					' パソコン名を表示する
						strBtnText &= pc_inf.strNameUser
					Else														' PC名を表示する
						strBtnText &= pc_inf.strNameSystem
					End If

					m_dctCtrlPC(nPcKind).btnPcName.Text = strBtnText			' ボタン名表示名を設定
					m_dctPcInf.Add(nPcKind, pc_inf)								' PC情報を保存
				End If
			End While

		Catch ex As Exception
		Finally
			If Not sqlRead Is Nothing Then sqlRead.Close()
			If Not tcls_DB Is Nothing Then tcls_DB.Dispose()
		End Try
	End Sub

	''' <summary>
	''' 機器情報の設定
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetErr()
		Dim tcls_DB As tClass.tClass_SQLServer = Nothing
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Try
			tcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
			If Not tcls_DB.Open() Then											' DBオープンに失敗したらあきらめる
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' 機器情報取得SQLの作成
			'-------------------------------------------------------------------
			Dim strSQL As String = ""
			strSQL &= " SELECT A.機器ID,"
			strSQL &= "        ISNULL(C.PCKIND,0),"
			strSQL &= "        B.機器状態,"
			strSQL &= "        A.機器異常度,"
			strSQL &= "        A.検査異常度,"
			strSQL &= "        ISNULL(C.PC区分,0)"
			strSQL &= " FROM " & m_sDB_STATUS_M & " A "
			strSQL &= "INNER JOIN " & m_sDB_STATUS_I & " B "
			strSQL &= " ON A.機器ID = B.機器ID "
			strSQL &= "LEFT OUTER JOIN " & m_sDB_PC_INF & " C "
			strSQL &= " ON A.PCID = C.PCID "

			sqlRead = tcls_DB.SelectExecute(strSQL)
			If sqlRead Is Nothing Then											' 機器情報を取得
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' 既に設定されている機器情報の初期化
			'-------------------------------------------------------------------
			Dim typErrInf As STATUS_INF = Nothing
			typErrInf.sDspData = ""

			For ii As Integer = 0 To m_lstItem.Count - 1
				m_lstItem(ii).Tag = typErrInf
			Next ii

			'-------------------------------------------------------------------
			' 機器情報設定
			'-------------------------------------------------------------------
			While sqlRead.Read()
				Dim nKikiID As Integer = CInt(sqlRead.GetValue(0))				' 機器ID
				Dim nKindID As Integer = CInt(sqlRead.GetValue(1))				' PCKIND
				Dim nStatus As Integer = CInt(sqlRead.GetValue(2))				' 機器状態
				Dim nKikiErr As Integer = CInt(sqlRead.GetValue(3))				' 機器異常度
				Dim nKensaErr As Integer = CInt(sqlRead.GetValue(4))			' 検査異常度
				Dim nPcDiv As Integer = CInt(sqlRead.GetValue(5))				' PC区分

				If m_dctCtrlKiki.ContainsKey(nKikiID) = False Then				' 未登録に機器IDがあれば読み飛ばす
					Continue While
				End If

				If 0 <> nKindID AndAlso m_dctCtrlPC.ContainsKey(nKindID) = False Then	' 未登録にPCIDがあれば読み飛ばす
					Continue While
				End If

				Dim kiki_ctrl As KIKI_CTRL = m_dctCtrlKiki(nKikiID)
				If kiki_ctrl.nKikiType = EM_RAS_TYPE.PC Then					' PCの場合
					If 0 <> nKindID Then										' PCKINDが有効の場合
						If kiki_ctrl.nSetType = EM_RAS_SETTYPE.DSP_CONTROL Then	' コントロールに直接設定
							SetStatusInf( _
							 m_dctCtrlPC(nKindID).btnPcName.Tag, _
							 nKikiID, _
							 nStatus, _
							 nKikiErr, _
							 nKensaErr, _
							 nPcDiv, _
							 nKindID)
						Else													' 線に設定
							For Each ctrl As Control In m_dctCtrlPC(nKindID).lblLine
								SetStatusInf( _
								 ctrl.Tag, _
								 nKikiID, _
								 nStatus, _
								 nKikiErr, _
								 nKensaErr, _
								 nPcDiv, _
								 nKindID)
							Next
						End If
					End If
				Else															' 機器の場合
					For Each ctrl As Control In m_dctCtrlKiki(nKikiID).lstCtrl
						SetStatusInf( _
						 ctrl.Tag, _
						 nKikiID, _
						 nStatus, _
						 nKikiErr, _
						 nKensaErr, _
						 nPcDiv)
					Next
				End If
			End While

		Catch ex As Exception
		Finally
			If Not sqlRead Is Nothing Then sqlRead.Close()
			If Not tcls_DB Is Nothing Then tcls_DB.Dispose()
		End Try
	End Sub
	''' <summary>
	''' ステータス情報をセット
	''' </summary>
	''' <param name="objTag">対象のコントロールのTag</param>
	''' <param name="nKikiId">機器ID</param>
	''' <param name="nStatus">機器状態</param>
	''' <param name="nKikiErr">機器異常度</param>
	''' <param name="nKendErr">検査異常度</param>
	''' <param name="nOffLine">PC区分(0:オンライン,1:オフライン)</param>
	''' <param name="nPcKind">PCKIND</param>
	''' <remarks></remarks>
	Private Sub SetStatusInf( _
	 ByRef objTag As Object, _
	 ByVal nKikiId As Integer, _
	 ByVal nStatus As Integer, _
	 ByVal nKikiErr As Integer, _
	 ByVal nKendErr As Integer, _
	 ByVal nOffLine As Integer, _
	 Optional ByVal nPcKind As Integer = 0)

		Dim inf As STATUS_INF = CType(objTag, STATUS_INF)

		Dim nDspType As EM_RAS_DSPTYPE = m_dctCtrlKiki(nKikiId).nDspType		' 表示区分の取得
		If nDspType = EM_RAS_DSPTYPE.DSP_VALUE _
		 OrElse (nDspType = EM_RAS_DSPTYPE.DSP_AUTO AndAlso nKikiErr = 0 AndAlso nKendErr = 0) Then	' 値を表示する場合
			inf.sDspData = CStr(nStatus)										' 表示値に値を設定する
			inf.nKikiId = nKikiId
		Else																	' 文字列を表示する場合
			If 0 <> nStatus And 0 = inf.nStatus Then							' 正常->異常へ変化したらステータスを設定
				inf.nStatus = nStatus
				inf.nKikiId = nKikiId
			End If

			If 0 <> nStatus And inf.nKikiErr < nKikiErr Then					' より深刻な機器異常になった場合に設定
				inf.nKikiErr = nKikiErr
				inf.nKikiId = nKikiId
			End If
		End If

		inf.nOffLine = nOffLine													' PC区分

		If 0 <> nPcKind Then													' PCKINDが有効であれば設定
			inf.nPcKind = nPcKind
		End If
		' 2014.05.21 竹山 VRCL用全長保存システム追加     >>>>>>>>
		' 機器状態が正常でなく、検査異常度が正常以外で登録されているなら、検査異常度を記憶する
		If 0 <> nStatus AndAlso 0 <> nKendErr AndAlso 0 = inf.nKendErr Then
			inf.nKendErr = nKendErr												' 検査異常度
		End If
		' 2014.05.21 竹山 VRCL用全長保存システム追加     <<<<<<<<
		objTag = inf															' 関連付けられたタグへ設定
	End Sub

	''' <summary>
	''' ステータス取得
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub timGetStatus_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timGetStatus.Tick
		Call SetPCName()														' PCボタンセット
		Call SetErr()															' ステータス状態取得
		Call DspLog()															' ログ表示
	End Sub

	''' <summary>
	''' ログ表示
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub DspLog()
		Dim tcls_DB As tClass.tClass_SQLServer = Nothing
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing

		Try
			tcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
			If Not tcls_DB.Open() Then											' DBオープに失敗したらあきらめる
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' ログ取得SQL作成
			'-------------------------------------------------------------------
			Dim strSQL As String = ""
			strSQL &= " SELECT A.異常MSG "
			strSQL &= " FROM " & m_sDB_STATUS_M & " A "
			strSQL &= " INNER JOIN " & m_sDB_STATUS_I & " B "
			strSQL &= " ON A.機器ID = B.機器ID "
			strSQL &= " LEFT OUTER JOIN " & m_sDB_PC_INF & " C "
			strSQL &= " ON A.PCID = C.PCID "
			strSQL &= " WHERE (A.機器異常度 <> 0 or A.検査異常度 <> 0)"
			strSQL &= "  AND ISNULL(C.PC区分,0) = 0 "
			strSQL &= "  AND B.機器状態 <> 0"
			strSQL &= " ORDER BY ISNULL(A.優先度, 1000) asc, A.機器異常度 desc "

			sqlRead = tcls_DB.SelectExecute(strSQL)
			If sqlRead Is Nothing Then											' SQL実行に失敗したらあきらめる
				Exit Sub
			End If

			'-------------------------------------------------------------------
			' ログ表示
			'-------------------------------------------------------------------
			For ii As Integer = 0 To m_nLogDspCnt - 1							' ログ表示初期化
				m_dgvLog(0, ii).Value = ""
			Next ii

			Dim nCnt As Integer = 0												' ログ表示件数
			While sqlRead.Read()
				If m_nLogDspCnt - 1 < nCnt Then									' 表示MAXを超えたら終了
					Exit While
				End If

				m_dgvLog(0, nCnt).Value = sqlRead.GetString(0)					' 異常MSGを表示
				nCnt += 1
			End While
		Catch ex As Exception
		Finally
			If Not sqlRead Is Nothing Then sqlRead.Close()
			If Not tcls_DB Is Nothing Then tcls_DB.Dispose()
		End Try
	End Sub

	''' <summary>
	''' PCボタンクリックイベント
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub btn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
		Dim strDspPcName As String	'メッセージに表示するPC名

		'2015.10.15 溝口 NOA_KAISEKI(-1) 追加
		'If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa) Then					' 遠隔時、デバッグ時は何もしない
		If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa Or -1 = m_nNoa) Then		' 遠隔時は何もしない
			Exit Sub
		End If

		'-----------------------------------------------------------------------
		' リセットPC名取得
		'-----------------------------------------------------------------------
		Dim nPcKind As Integer = -1
		For Each key As Integer In m_dctCtrlPC.Keys								' 押されたボタンを持つPCコントロールを検索
			If m_dctCtrlPC(key).btnPcName.Equals(sender) = True Then
				nPcKind = key
				Exit For
			End If
		Next

		Dim strPcName As String = m_dctPcInf(nPcKind).strNameSystem				' PC名取得
		If m_nDspPcName = EM_DSP_PC_NAME.USER Then								' 表示PC名称取得
			strDspPcName = m_dctPcInf(nPcKind).strNameUser
		Else
			strDspPcName = m_dctPcInf(nPcKind).strNameSystem
		End If

		'-----------------------------------------------------------------------
		' リセット確認
		'-----------------------------------------------------------------------
		Dim nResetMode As Windows.Forms.DialogResult							' リセットモード (Yes:ログオフ No:再起動 Cancel:中止)
		nResetMode = ConfRest(nPcKind, strDspPcName)							' リセット確認ダイアログ表示
		If nResetMode = Windows.Forms.DialogResult.Cancel Then
			Exit Sub
		End If

		If Windows.Forms.DialogResult.OK <> ConfPass() Then						' パスワード確認。違ってたらキャンセル
			Exit Sub
		End If

		'-----------------------------------------------------------------------
		' リセット要求送信
		'-----------------------------------------------------------------------
		If nPcKind = m_nAllResetID Then											' 蓄積の場合は、全PCのため"*"にする
			strPcName = "*"														' 
		ElseIf nPcKind = m_nToukatu Then										' 統括の場合は、マスターPC名
			strPcName = m_sMasterPC
		End If

		If nResetMode = Windows.Forms.DialogResult.Yes Then						' YESならログオフ
			Call SendReset(m_nEventLogOff, strPcName)
		Else																	' Noなら再起動
			Call SendReset(m_nEventReset, strPcName)
		End If
	End Sub

	''' <summary>
	''' リセット確認
	''' </summary>
	''' <param name="nPcKind">PCKIND</param>
	''' <param name="strPcName">PC名</param>
	''' <param name="bStopFlg">停止フラグ(True:停止,False:停止でない)</param>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
	Protected Friend Overridable Function ConfRest(Optional ByVal nPcKind As Integer = 0, Optional ByVal strPcName As String = "", Optional ByVal bStopFlg As Boolean = False) As DialogResult
		Dim strMsg As String													' 表示メッセージ 

		If nPcKind = m_nAllResetID Then											' 全PCリセットの場合
			strMsg = strPcName & "のリセットを行うと" & _
			   vbNewLine & "全てのPCがリセットされます。" & _
			   vbNewLine & "実行しますか？"

		ElseIf nPcKind = m_nToukatu Then										' 統括の場合
			strMsg = strPcName & "のリセットを行うと" & _
			   vbNewLine & "蓄積PCを除く全てのPCがリセットされます。" & _
			   vbNewLine & "実行しますか？"
		Else																	' 統括以外の場合
			strMsg = strPcName & "のリセットを実行しますか？"
		End If

		' 共通メッセージ
		strMsg &= vbCrLf & vbCrLf
		strMsg &= "はい   ： パソコンのログオフを実行" & vbCrLf
		strMsg &= "いいえ ： パソコンの再起動を実行" & vbCrLf
		strMsg &= "ｷｬﾝｾﾙ  ： 選択をキャンセル" & vbCrLf
		strMsg &= "   (通常、「はい。ログオフ」を選択)"

		Dim frmMsg As New tClassForm.frmPop_MsgBox3(strMsg, "確認", , True, True, True, DialogResult.Yes, DialogResult.No, DialogResult.Cancel)
		Dim bRet As DialogResult = frmMsg.ShowDialog()
		Return bRet
	End Function
	''' <summary>
	''' システム終了確認
	''' </summary>
	''' <param name="bStopFlg">停止フラグ(True:停止,False:停止でない)</param>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
	Protected Friend Overridable Function ConfShatdown(Optional ByVal bStopFlg As Boolean = False) As DialogResult
		Dim strMsg As String	'メッセージ 

		If bStopFlg Then														' PC名がない場合は、システム終了
			strMsg = "全PCのシャットダウンを実行しますか？"
		Else
			strMsg = "システムが稼動中です。" & _
			vbNewLine & "それでも、全PCのシャットダウンを実行しますか？"
		End If

		Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		Dim bRet As DialogResult = frmMsg.ShowDialog()
		Return bRet
	End Function

	''' <summary>
	''' パスワード確認
	''' </summary>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
    Protected Friend Overridable Function ConfPass() As DialogResult
        ' パスワード未設定の場合
        If "" = m_strResetPass Then
            Return DialogResult.OK
        End If

        Dim frmPass As New tClassForm.frmPop_Pass(m_strResetPass)
        Dim bRet As DialogResult = frmPass.ShowDialog()                         ' パスワード確認
        Return bRet
    End Function

	''' <summary>
	''' PCリセットorシステム終了メール送信
	''' </summary>
	''' <param name="nEventNo">イベントNo</param>
	''' <param name="strPcName">PC名</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SendReset(ByVal nEventNo As Integer, Optional ByVal strPcName As String = "")
		Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

		que.nEventNo = nEventNo													' PCリセット
		que.nLineNo = m_nLineID
		que.data = strPcName

		Call KizuLibCLI.KizuMod.Send_Mail(m_sMasterTask, m_sMasterPC, que)		' メール送信 
	End Sub

	''' <summary>
	''' システム終了処理
    ''' <param name="bStopFlg">停止中フラグ(True:停止中,False:それ以外)</param>
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExitSystem(ByVal bStopFlg As Boolean)
		If Windows.Forms.DialogResult.OK <> ConfShatdown(bStopFlg) Then			' リセット確認
			Exit Sub
		End If

		If Windows.Forms.DialogResult.OK <> ConfPass() Then						' パスワード確認
			Exit Sub
		End If
		Call SendReset(m_nEventExit)											' システム終了メール送信
	End Sub
End Class
