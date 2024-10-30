'*********************************************************************************
' RAS画面の親クラス
'	[Ver]
'		Ver.01    2007/08/23  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' RAS画面の親クラス
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Ras

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    '/ /////////////////////////////////////////////////////////////////////////////////

    'PCKINDに紐づく情報
    Public Structure PC_CTRL
        Dim strName As String           'ボタン名
        Dim btnPcName As Button         'PCボタン
        Dim lblShadow As Label          'PCボタン影
        Dim lblLine As List(Of Label)   '回線ラベル

        'イニシャライズ
        Public Sub initialize()
            lblLine = New List(Of Label)
        End Sub
    End Structure

    '機器IDに紐づく情報
	Public Structure STATUS_INF
		Dim nKikiId As Integer		'機器ID
		Dim nStatus As Integer		'機器状態
		Dim nKikiErr As Integer		'機器異常度
		Dim sDspData As String		'画面表示データ
		Dim nPcKind As Integer		'PCKIND
		Dim nOffLine As Integer		'PC区分
		' 2014.05.21 竹山 VRCL用全長保存システム追加     >>>>>>>>
		Dim nKendErr As Integer		'検査異常度
		' 2014.05.21 竹山 VRCL用全長保存システム追加     <<<<<<<<
	End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_hasBtnPcName As New Hashtable         'PCボタン紐付け用
    Private m_hasKikiName As New Hashtable          '機器状態紐付け用
    Private m_dctPcName As New Dictionary(Of Integer, String)   'PCKINDに紐づくPC名を登録
    Private m_dctPcNameJis As New Dictionary(Of Integer, String) 'PCKINDに紐づくPC名称を登録

    Private m_sDB_PC_I As String = DB_PC_INF                    ' DBテーブル PC種別
    Private m_sDB_PC_M As String = DB_PC_MASTER                 ' DBテーブル PCマスター
    Private m_sDB_STATUS_I As String = DB_STATUS_INF            ' DBテーブル 機器状態
    Private m_sDB_STATUS_M As String = DB_STATUS_MASTER         ' DBテーブル 機器状態マスター
    Private m_sDB_COMMON As String = DB_COMMON_MASTER           ' DBテーブル 疵検定数マスター
    Private m_nToukatu As Integer                               ' 統括のPCKINDIDをセット
    Private m_nAllResetID As Integer                            ' 全PCのリセットするPCKINDをセット(蓄積,蓄積と統括が一緒の場合は統括)
    Private m_nLogDspCnt As Integer                             ' ログ表示数
    Private m_dgvData As DataGridView                           ' ログ表示グリッド
    Private m_sPass As String = ""                              ' パスワード
    Private m_nSYSTEM_EXIT As Integer = FACT_KS_MASTER_06       ' システム終了
	Private m_nRESET_EVENT As Integer = FACT_KS_MASTER_07		' PC再起動のイベント番号
	Private m_nLOGOFF_EVENT As Integer = FACT_KS_MASTER_08		' PCログオフのイベント番号
    Private m_nLineID As Integer = LINE_ID                      ' ラインID
	Private m_nNoa As Integer									' NOA接続フラグ (0:オンライン  1:遠隔)
	Private m_sDbSession As String = INI_DB						' DBセッション
	Private m_sMasterTask As String = KS_MASTER					' マスタータスク
    Private m_sMasterPC As String = MASTER_PC_NAME              ' マスターPC(KS_MASTERが起動するPC)
    Private m_bPcKindNameFlg As Boolean = True                  ' PC_MASTERのPC種別を使うか、プログラムで表示させるかのフラグ(True:PC種別,False:プログラム)
    Private m_bPcNameFlg As Boolean = True                      ' パソコン名称をPC_MASTERのPC名を使うか、パソコン名称を使うかのフラグ(True:パソコン名称,False:PC名)
    Protected Friend m_lstItem As New List(Of Object)           ' 色表示するコントロールのリスト

    ''' <summary>
    ''' PC_INF のテーブル名称
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
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
    ''' PC_MASTER のテーブル名称
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' STATUS_INF のテーブル名称
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' STATUS_MASTER のテーブル名称
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' COMMON_MASTER のテーブル名称
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' 全PCリセット用のPCKINDをセット(通常は、蓄積。蓄積がいない場合は統括)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("全PCリセット用のPCKIND")> _
    Public Property tProp_TokatuID() As Integer
        Get
            Return m_nToukatu
        End Get
        Set(ByVal Value As Integer)
            m_nToukatu = Value
        End Set
    End Property

    ''' <summary>
    ''' 全PCリセット用のPCKINDをセット(通常は、蓄積。蓄積がいない場合は統括)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
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
    ''' ログのデータグリッドをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("データグリッド")> _
    Public Property tProp_DspGrid() As DataGridView
        Get
            Return m_dgvData
        End Get
        Set(ByVal Value As DataGridView)
            m_dgvData = Value
        End Set
    End Property

    ''' <summary>
    ''' PCリセットに使用するパスワードをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("パスワード")> _
    Public Property tProp_Pass() As String
        Get
            Return m_sPass
        End Get
        Set(ByVal Value As String)
            m_sPass = Value
        End Set
    End Property


    ''' <summary>
    ''' PCリセットのイベントNoをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("PCリセットのイベントNo")> _
    Public Property tProp_ResetEventNo() As Integer
        Get
            Return m_nRESET_EVENT
        End Get
        Set(ByVal Value As Integer)
            m_nRESET_EVENT = Value
        End Set
    End Property

	''' <summary>
	''' PCログオフのイベントNoをセット
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("PCログオフのイベントNo")> _
	Public Property tProp_LogOffEventNo() As Integer
		Get
			Return m_nLOGOFF_EVENT
		End Get
		Set(ByVal Value As Integer)
			m_nLOGOFF_EVENT = Value
		End Set
	End Property

    ''' <summary>
    ''' システム終了のイベントNoをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("システム終了のイベントNo")> _
    Public Property tProp_SystemExitEventNo() As Integer
        Get
            Return m_nSYSTEM_EXIT
        End Get
        Set(ByVal Value As Integer)
            m_nSYSTEM_EXIT = Value
        End Set
    End Property

    ''' <summary>
    ''' ラインIDをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
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
    ''' NOA接続フラグをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("NOA接続フラグ")> _
    Public Property tProp_Noa() As Integer
        Get
            Return m_nNoa
        End Get
        Set(ByVal Value As Integer)
            m_nNoa = Value
        End Set
    End Property

    ''' <summary>
    ''' RAS画面表示ログ数をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
     , System.ComponentModel.Description("RAS画面表示ログ数をセット")> _
    Public Property tProp_LogDspCnt() As Integer
        Get
            Return m_nLogDspCnt
        End Get
        Set(ByVal Value As Integer)
            m_nLogDspCnt = Value
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
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
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
	''' マスタータスク
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(KS_MASTER) _
	 , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	 , System.ComponentModel.Description("マスタータスク")> _
	Public Property tProp_MasterTask() As String
		Get
			Return m_sMasterTask
		End Get
		Set(ByVal Value As String)
			m_sMasterTask = Value
		End Set
	End Property

	''' <summary>
	''' マスターPC(KS_MASTER)が起動するPC
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	, System.ComponentModel.DefaultValue(MASTER_PC_NAME) _
	, System.ComponentModel.Category("☆RAS画面専用プロパティ") _
	, System.ComponentModel.Description("マスターPC")> _
	 Public Property tProp_MasterPC() As String
		Get
			Return m_sMasterPC
		End Get
		Set(ByVal Value As String)
			m_sMasterPC = Value
		End Set
	End Property

    ''' <summary>
    '''  パソコン名称の使用設定(True:PC_INFのPC種別,False:プログラムの設定)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
    , System.ComponentModel.DefaultValue(True) _
    , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
    , System.ComponentModel.Description("マスターPC")> _
     Public Property tProp_PcKindNameFlg() As Boolean
        Get
            Return m_bPcKindNameFlg
        End Get
        Set(ByVal Value As Boolean)
            m_bPcKindNameFlg = Value
        End Set
    End Property

    ''' <summary>
    '''  パソコン名称の使用設定(True:PC_MASTERのパソコン名称,False:PC_MASTERのPC名)
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
    , System.ComponentModel.DefaultValue(True) _
    , System.ComponentModel.Category("☆RAS画面専用プロパティ") _
    , System.ComponentModel.Description("マスターPC")> _
     Public Property tProp_PcNameFlg() As Boolean
        Get
            Return m_bPcNameFlg
        End Get
        Set(ByVal Value As Boolean)
            m_bPcNameFlg = Value
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

        Dim typData As New PC_CTRL

        'イニシャライズ
        typData.initialize()

        'PCボタンとPCの影ラベルを登録
        typData.strName = strName
        typData.btnPcName = btnPc
        typData.lblShadow = lblPcShadow

        '回線ラベルを登録
        If Not lblLine1 Is Nothing Then typData.lblLine.Add(lblLine1)
        If Not lblLine2 Is Nothing Then typData.lblLine.Add(lblLine2)
        If Not lblLine3 Is Nothing Then typData.lblLine.Add(lblLine3)
        If Not lblLine4 Is Nothing Then typData.lblLine.Add(lblLine4)
        If Not lblLine5 Is Nothing Then typData.lblLine.Add(lblLine5)
        If Not lblLine6 Is Nothing Then typData.lblLine.Add(lblLine6)
        If Not lblLine7 Is Nothing Then typData.lblLine.Add(lblLine7)
        If Not lblLine8 Is Nothing Then typData.lblLine.Add(lblLine8)
        If Not lblLine9 Is Nothing Then typData.lblLine.Add(lblLine9)

        'PCKINDをキーにしてボタンコントロールを登録
        m_hasBtnPcName.Add(nPcKind, typData)

        '' ついでにクリックイベントハンドラに追加
        AddHandler btnPc.Click, AddressOf btn_Click


    End Sub
    ''' <summary>
    ''' 機器状態に紐づくコントロールを登録(コントロールが複数の場合)
    ''' </summary>
    ''' <param name="nKikiID">PCKIND</param>
    ''' <param name="lstData">コントロール</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal lstData As List(Of Control))

        'PCKINDをキーにしてボタンコントロールを登録
        m_hasKikiName.Add(nKikiID, lstData)

    End Sub

    ''' <summary>
    ''' 機器状態に紐づくコントロールを登録(コントロールが一つ場合)
    ''' </summary>
    ''' <param name="nKikiID">PCKIND</param>
    ''' <param name="ctlData">コントロール</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal ctlData As Control)

        Dim lstData As New List(Of Control)

        lstData.Add(ctlData)

        'PCKINDをキーにしてボタンコントロールを登録
        m_hasKikiName.Add(nKikiID, lstData)

    End Sub

    ''' <summary>
    ''' 機器状態に紐づくコントロールを登録
    ''' </summary>
    ''' <param name="nKikiID">PCKIND</param>
    ''' <param name="bData">PCフラグ(True:PCボタンに異常フラグを立てる,False:回線ラベルに異常フラグを立てる</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub AddButtonKikiName(ByVal nKikiID As Integer, ByVal bData As Boolean)


        'PCKINDをキーにしてボタンコントロールを登録
        m_hasKikiName.Add(nKikiID, bData)

    End Sub

    ''' <summary>
    ''' PCボタンへPC名を登録
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetPCName()

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim nPcKind As Integer
        Dim strName As String


        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT A.PCKIND,"
        strSQL &= "        B.PC名,"
        strSQL &= "        A.PC種別,"
        strSQL &= "        B.パソコン名称"
        strSQL &= " FROM " & m_sDB_PC_I & " A " & "INNER JOIN " & m_sDB_PC_M & " B "
        strSQL &= " ON A.PCID = B.PCID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            m_dctPcName.Clear()
            m_dctPcNameJis.Clear()

            ' データ表示
            While sqlRead.Read()

                nPcKind = sqlRead.GetInt32(0)   'PCKIND

                'PCKINDをキーにPC名を設定
                If m_hasBtnPcName.Contains(nPcKind) Then

                    If m_bPcKindNameFlg Then
                        'PC種別を表示
                        strName = sqlRead.GetString(2)
                    Else
                        'プログラムの設定を表示
                        strName = CType(m_hasBtnPcName(nPcKind), PC_CTRL).strName
                    End If

                    strName &= vbCrLf

                    If m_bPcNameFlg Then

                        'パソコン名称を表示
                        strName &= sqlRead.GetString(3)
                    Else

                        'PC名を表示
                        strName &= sqlRead.GetString(1)
                    End If


                    CType(m_hasBtnPcName(nPcKind), PC_CTRL).btnPcName.Text = strName

                    'PCKINDに紐づくPC名を登録
                    m_dctPcName.Add(nPcKind, sqlRead.GetString(1))

                    'PCKINDに紐づくPC名称を登録
                    m_dctPcNameJis.Add(nPcKind, sqlRead.GetString(3))
                End If

            End While

        Catch ex As Exception

        Finally

            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' エラー情報セット
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetErr()

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim typErrInf As STATUS_INF = Nothing

        Dim objData As Control      'ワークコントロール
        Dim nKikiID As Integer      '機器ID
        Dim nKindID As Integer      'PCKIND
        Dim nStatus As Integer      '機器状態
        Dim nKikiErr As Integer     '機器異常度
        Dim nKensaErr As Integer    '検査異常度
        Dim nPcDiv As Integer       'PC区分
        Dim ii As Integer           'カウンタ

        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT A.機器ID,"
        strSQL &= "        ISNULL(C.PCKIND,0),"
        strSQL &= "        B.機器状態,"
        strSQL &= "        A.機器異常度,"
        strSQL &= "        A.検査異常度,"
        strSQL &= "        ISNULL(C.PC区分,0)"
        strSQL &= " FROM " & m_sDB_STATUS_M & " A "
        strSQL &= "INNER JOIN " & m_sDB_STATUS_I & " B "
        strSQL &= " ON A.機器ID = B.機器ID "
        strSQL &= "LEFT OUTER JOIN " & m_sDB_PC_I & " C "
        strSQL &= " ON A.PCID = C.PCID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

			'コントロールのTag情報を初期化
			typErrInf.nKikiId = 0
            typErrInf.nStatus = 0
            typErrInf.nOffLine = 0
            typErrInf.sDspData = ""
            typErrInf.nKikiErr = 0

            For ii = 0 To m_lstItem.Count - 1
                CType(m_lstItem(ii), Control).Tag = typErrInf
            Next ii

            ' データ取得表示
            While sqlRead.Read()

                nKikiID = CInt(sqlRead.GetValue(0)) '機器ID
                nKindID = CInt(sqlRead.GetValue(1)) 'PCKIND
                nStatus = CInt(sqlRead.GetValue(2))  '機器状態
                nKikiErr = CInt(sqlRead.GetValue(3)) '機器異常度
                nKensaErr = CInt(sqlRead.GetValue(4))   '検査異常度
                nPcDiv = CInt(sqlRead.GetValue(5))  'PC区分

				'キーが存在しない場合は、飛ばす
                If Not m_hasKikiName.ContainsKey(nKikiID) Then Continue While

                'キーに紐づくデータが数字がチェック
                If IsNumeric(m_hasKikiName(nKikiID)) Then

                    If 0 <> nKindID Then

                        If CType(m_hasKikiName(nKikiID), Boolean) Then
                            '数字の場合は、PCKINDから紐づくボタンを設定

                            '異常情報セット
							Call SetStatusInf(CType(m_hasBtnPcName(nKindID), PC_CTRL).btnPcName.Tag, nKikiID, nStatus, _
								 nKikiErr, nKensaErr, nPcDiv, nKindID)


                        Else

                            '数字の場合は、PCKINDから紐づくラベルを設定
                            '回線関係のラベルを表示する
                            For ii = 0 To CType(m_hasBtnPcName(nKindID), PC_CTRL).lblLine.Count - 1

                                '異常情報セット
								Call SetStatusInf(CType(m_hasBtnPcName(nKindID), PC_CTRL).lblLine(ii).Tag, nKikiID, nStatus, _
									nKikiErr, nKensaErr, nPcDiv, nKindID)


                            Next ii
                        End If
                    End If

                Else

                    '該当するコントロール分セット
                    For Each objData In CType(m_hasKikiName(nKikiID), List(Of Control))

                        '異常情報セット
						Call SetStatusInf(CType(objData, Control).Tag, nKikiID, nStatus, _
											nKikiErr, nKensaErr, nPcDiv)
                    Next

                End If

            End While

        Catch ex As Exception

        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()


        End Try


    End Sub
    ''' <summary>
    ''' ステータス情報をセット
    ''' </summary>
	''' <param name="objData">対象のコントロールのTag</param>
	''' <param name="nKikiId">機器ID</param>
    ''' <param name="nStatus">機器状態</param>
    ''' <param name="nKikiErr">機器異常度</param>
    ''' <param name="nKendErr">検査異常度</param>
    ''' <param name="nOffLine">PC区分(0:オンライン,1:オフライン)</param>
    ''' <param name="nPcKind">PCKIND</param>
    ''' <remarks></remarks>
	Private Sub SetStatusInf(ByRef objData As Object, ByVal nKikiId As Integer, ByVal nStatus As Integer, _
		   ByVal nKikiErr As Integer, ByVal nKendErr As Integer, _
		   ByVal nOffLine As Integer, Optional ByVal nPcKind As Integer = 0)

		Dim typWkInf As STATUS_INF = Nothing

		typWkInf = CType(objData, STATUS_INF)

		If 0 = nKikiErr And 0 = nKendErr Then
			'機器異常度が0の場合は、機器状態を表示値にセット
			typWkInf.sDspData = CStr(nStatus)
			typWkInf.nKikiId = nKikiId
		Else

			'現在の状態が異常で異常のデータがセットされていない場合のみセット
			If 0 <> nStatus And 0 = typWkInf.nStatus Then
				typWkInf.nStatus = nStatus
				typWkInf.nKikiId = nKikiId
			End If

			'現在の機器異常度よりも高い異常度ならセット
			If 0 <> nStatus And typWkInf.nKikiErr < nKikiErr Then
				typWkInf.nKikiErr = nKikiErr
				typWkInf.nKikiId = nKikiId
			End If

		End If

		typWkInf.nOffLine = nOffLine	'PC区分

		If 0 <> nPcKind Then
			typWkInf.nPcKind = nPcKind		'PCKIND
		End If
		' 2014.05.21 竹山 VRCL用全長保存システム追加     >>>>>>>>
		' 機器状態が正常でなく、検査異常度が正常以外で登録されているなら、検査異常度を記憶する
		If 0 <> nStatus AndAlso 0 <> nKendErr AndAlso 0 = typWkInf.nKendErr Then
			typWkInf.nKendErr = nKendErr										' 検査異常度
		End If
		' 2014.05.21 竹山 VRCL用全長保存システム追加     <<<<<<<<

		'データをセット
		objData = typWkInf

	End Sub


    ''' <summary>
    ''' ステータス取得
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub timGetStatus_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timGetStatus.Tick

		'PCボタンセット
		Call SetPCName()

		'ステータス状態取得
		Call SetErr()

		'ログ表示
		Call DspLog()

	End Sub

    ''' <summary>
    ''' ログ表示
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub DspLog()


		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim nCnt As Integer  'ログ表示数
        Dim ii As Integer

        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT A.異常MSG "
        strSQL &= " FROM " & m_sDB_STATUS_M & " A "
        strSQL &= " INNER JOIN " & m_sDB_STATUS_I & " B "
        strSQL &= " ON A.機器ID = B.機器ID "
        strSQL &= " LEFT OUTER JOIN " & m_sDB_PC_I & " C "
        strSQL &= " ON A.PCID = C.PCID "
        strSQL &= " WHERE (A.機器異常度 <> 0 or A.検査異常度 <> 0)"
        strSQL &= "  AND ISNULL(C.PC区分,0) = 0 "
        strSQL &= "  AND B.機器状態 <> 0"
        strSQL &= " ORDER BY ISNULL(A.優先度, 1000) asc, A.機器異常度 desc "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            'グリッド初期化
            For ii = 0 To m_nLogDspCnt - 1
                m_dgvData(0, ii).Value = ""
            Next ii
            nCnt = 0
            ' データ取得表示
            While sqlRead.Read()

                '表示MAXを超えたら終了
                If m_nLogDspCnt - 1 < nCnt Then Exit While

                '異常MSGを表示
                m_dgvData(0, nCnt).Value = sqlRead.GetString(0)

                nCnt += 1

            End While


        Catch ex As Exception

        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' PCボタンクリックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub btn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

		Dim typStatus As STATUS_INF
		Dim nPcKind As Integer	'PCKIND
		Dim strPcName As String	  'PC名
		Dim strDspPcName As String	'メッセージに表示するPC名

		'遠隔時は、処理なし
		'2015.10.15 溝口 NOA_KAISEKI(-1) 追加
		'If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa) Then Exit Sub
		If Not (NOA_ONLINE = m_nNoa Or NOA_DEBUG = m_nNoa Or -1 = m_nNoa) Then Exit Sub ' 遠隔時、デバッグ時は何もしない

		'PC情報を取得
		typStatus = CType(CType(sender, Button).Tag, STATUS_INF)

		'PCKIND取得
		nPcKind = typStatus.nPcKind

		'該当PC名がなければ、終了
		If Not m_dctPcName.ContainsKey(nPcKind) Then
			Exit Sub
		End If

		'PC名取得
		strPcName = m_dctPcName(nPcKind)

		'表示PC名称取得
		If m_bPcNameFlg Then
			strDspPcName = m_dctPcNameJis(nPcKind)
		Else
			strDspPcName = m_dctPcName(nPcKind)
		End If

		'リセット確認
		Dim nResetMode As Windows.Forms.DialogResult		' リセットモード (Yes:ログオフ No:再起動 Cancel:中止)
		nResetMode = ConfRest(nPcKind, strDspPcName)
		If nResetMode = Windows.Forms.DialogResult.Cancel Then
			Exit Sub
		End If

		'パスワード確認
		If Windows.Forms.DialogResult.OK <> ConfPass() Then
			Exit Sub
		End If

		'リセット要求送信
		If nPcKind = m_nAllResetID Then

			'蓄積の場合は、全PCのため"*"にする
			strPcName = "*"

		ElseIf nPcKind = m_nToukatu Then

			'統括の場合は、マスターPC名
			strPcName = m_sMasterPC

		End If

		If nResetMode = Windows.Forms.DialogResult.Yes Then
			Call SendReset(m_nLOGOFF_EVENT, strPcName)
		Else
			Call SendReset(m_nRESET_EVENT, strPcName)
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


		Dim strMsg As String	'メッセージ 
		Dim bRet As DialogResult


		If nPcKind = m_nAllResetID Then

			'全PCリセットの場合
			strMsg = strPcName & "のリセットを行うと" & _
			   vbNewLine & "全てのPCがリセットされます。" & _
			   vbNewLine & "実行しますか？"

		ElseIf nPcKind = m_nToukatu Then

			strMsg = strPcName & "のリセットを行うと" & _
			   vbNewLine & "蓄積PCを除く全てのPCがリセットされます。" & _
			   vbNewLine & "実行しますか？"
		Else

			'統括でない
			strMsg = strPcName & "のリセットを実行しますか？"

		End If

		' 共通メッセージ
		strMsg &= vbCrLf & vbCrLf
		strMsg &= "はい   ： パソコンのログオフを実行" & vbCrLf
		strMsg &= "いいえ ： パソコンの再起動を実行" & vbCrLf
		strMsg &= "ｷｬﾝｾﾙ  ： 選択をキャンセル" & vbCrLf
		strMsg &= "   (通常、「はい。ログオフ」を選択)"

		Dim frmMsg As New tClassForm.frmPop_MsgBox3(strMsg, "確認", , True, True, True, Windows.Forms.DialogResult.Yes, Windows.Forms.DialogResult.No, Windows.Forms.DialogResult.Cancel)

		bRet = frmMsg.ShowDialog()

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
		Dim bRet As DialogResult


		'PC名がない場合は、システム終了
		If bStopFlg Then
			strMsg = "全PCのシャットダウンを実行しますか？"
		Else
			strMsg = "システムが稼動中です。" & _
			vbNewLine & "それでも、全PCのシャットダウンを実行しますか？"

		End If

		Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		bRet = frmMsg.ShowDialog()

		Return bRet

	End Function

    ''' <summary>
    ''' パスワード確認
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Protected Friend Overridable Function ConfPass() As DialogResult

        'パスワード未設定の場合
        If "" = m_sPass Then
            Return DialogResult.OK
        End If

        'パスワード確認
        Dim frmPass As New tClassForm.frmPop_Pass(m_sPass)
        Dim bRet As DialogResult

        bRet = frmPass.ShowDialog

        Return bRet

    End Function
    ''' <summary>
    ''' PCリセットorシステム終了メール送信
    ''' </summary>
    ''' <remarks></remarks>
	Protected Friend Overridable Sub SendReset(ByVal nEventNo As Integer, Optional ByVal strPcName As String = "")

		Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

		'PCリセット
		que.nEventNo = nEventNo
		que.nLineNo = m_nLineID
		que.data = strPcName

		'メール送信 
		Call KizuLibCLI.KizuMod.Send_Mail(m_sMasterTask, m_sMasterPC, que)

	End Sub

    ''' <summary>
    ''' システム終了処理
    ''' <param name="bStopFlg">停止中フラグ(True:停止中,False:それ以外)</param>
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExitSystem(ByVal bStopFlg As Boolean)

        'リセット確認
		If Windows.Forms.DialogResult.OK <> ConfShatdown(bStopFlg) Then
			Exit Sub
		End If

        'パスワード確認
		If Windows.Forms.DialogResult.OK <> ConfPass() Then
			Exit Sub
		End If

        'システム終了メール送信
        Call SendReset(m_nSYSTEM_EXIT)

    End Sub


End Class
