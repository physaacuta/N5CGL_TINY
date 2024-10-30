'*********************************************************************************
' 疵点マップ画面
'	[Ver]
'		Ver.01    2017/12/01  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports System.Runtime.InteropServices
Imports System.Text

Public Class frmMain
    Implements IDisposable                          ' デストラクタ定義

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクションボタン押下イベント (ベースの置き換え)
    ''' </summary>
    ''' <param name="FncNo">0オリジン</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デリゲート関数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Delegate Sub m_deleDspNew(ByVal bReadCoil As Boolean)    '別スレッドからのコントロールアクセス(最新読込み)
    Private Delegate Sub m_deleChangeParam(ByVal nMode As Integer)   '別スレッドからのコントロールアクセス(パラメータ再読み込み)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private DSP_DEFMAP_CNT As Integer = 3                               ' 表示疵マップ数
    Private DSP_DEFMAP_DATCNT As Integer = 10                           ' データ取得対象コイル数

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_ParamManager As clsParamManager                        ' パラメータ管理クラス
    Private mcls_Log As tClass.tClass_LogManager                        ' ログ管理クラス
    Private mcls_DefMap(DSP_DEFMAP_CNT - 1) As clsDefMap                ' 疵マップ表示情報クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_bLoad As Boolean = False                                  ' 画面ロード中フラグ
    Private m_bInit As Boolean = False                                  ' 画面初期化フラグ

    Private m_bUpTurn(NUM_MEN - 1) As Boolean                           ' 疵マップ 上下反転フラグ(Ture:反転,False:反転なし)
    Private m_bSideTurn(NUM_MEN - 1) As Boolean                         ' 疵マップ 左右反転フラグ(Ture:反転,False:反転なし)

    Private m_nDspStartIdx As Integer = 0                               ' 現在表示中の表示開始Index
    Private m_nDspMaxIdx As Integer = 0                                 ' 現在表示中の表示可能最大Index
    Private m_nDspMen As Integer = 0                                    ' 現在表示中の面(0:表 1:裏)
    Private m_nRefreshMode As EM_REFRESH_MODE = EM_REFRESH_MODE.AUTO    ' 更新モード

    Private m_ctlMap(DSP_DEFMAP_CNT - 1) As MAP_CTRL                    ' マップ用コントロール

    '表示用データ
    Private m_typDspData(DSP_DEFMAP_DATCNT - 1) As clsDefMap.DSP_DATA   ' 画面表示データ

    'マップコントロール
    Private Structure MAP_CTRL
        Dim picMap As PictureBox
        Dim lblName As Label
        Dim lblCoilNo As Label
    End Structure

    'ファンクションボタン定義
    Private Enum EM_BTN_MODE
        nPreCoil = 0        ' F1  前コイル
        nNextCoil           ' F2  次コイル
        nChangeOU           ' F3  裏面表示（表面表示）
        nYobi1              ' F4  
        nStop               ' F5  一時停止
        nYobi2              ' F6
        nYobi3              ' F7
        nYobi4              ' F8
        nYobi5              ' F9
        nPrint              ' F10 画面印刷
        nSave               ' F11 画面保存
        nClose              ' F12 閉じる
    End Enum

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    Public Sub New()
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '''' タイトル設定
        Call SetTitleName(Me)

        ' フォーム状態
        Me.tProp_EndButtonEnable = False

        m_bLoad = True
        For ii As Integer = 0 To DSP_DEFMAP_DATCNT - 1
            m_typDspData(ii).Initialize()
        Next

        '初期表示設定
        Call DspInitProcess()

        ' label初期設定
        m_ctlMap(0).lblCoilNo = lblCoilNo1
        m_ctlMap(1).lblCoilNo = lblCoilNo2
        m_ctlMap(2).lblCoilNo = lblCoilNo3
        m_ctlMap(0).lblName = lblDsp1
        m_ctlMap(1).lblName = lblDsp2
        m_ctlMap(2).lblName = lblDsp3

        ' PictureBox初期設定
        m_ctlMap(0).picMap = picMap1
        m_ctlMap(1).picMap = picMap2
        m_ctlMap(2).picMap = picMap3
        For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
            AddHandler m_ctlMap(ii).picMap.Paint, AddressOf picMap_Paint
        Next

        ' ボタン定義
        MyBase.AddFunctionButton(btnF01, EM_BTN_MODE.nPreCoil)      ' 前コイル
        MyBase.AddFunctionButton(btnF02, EM_BTN_MODE.nNextCoil)     ' 次コイル
        MyBase.AddFunctionButton(btnF03, EM_BTN_MODE.nChangeOU)     ' 裏面表示
        MyBase.AddFunctionButton(btnF05, EM_BTN_MODE.nStop)         ' 一次停止
        MyBase.AddFunctionButton(btnF10, EM_BTN_MODE.nPrint)        ' 画面印刷
        MyBase.AddFunctionButton(btnF11, EM_BTN_MODE.nSave)         ' 画面保存
        MyBase.AddFunctionButton(btnF12, EM_BTN_MODE.nClose)        ' 閉じる

        'クラス設定
        mcls_ParamManager = New clsParamManager(gcls_Log)       'パラメータ管理
        mcls_ParamManager.ReadAll()

        Call CreateDefMap()     '疵マップ表示情報管理

        '画面初期表示（空白）
        Call DspInit(True)

        Call ChangeRefreshMode(EM_REFRESH_MODE.AUTO)

        m_bLoad = False
        Application.DoEvents()

        '疵マップ実績画面表示（タイマーで起動）
        tmrDsp.Enabled = True

    End Sub

    ''' <summary>
    ''' 初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInitProcess()

        Dim blnTurn As Boolean

        '疵マップ上下反転（表）
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_UP_TOP", TASKINI_NAME), False, True))
        m_bUpTurn(0) = blnTurn

        '疵マップ上下反転（裏）
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_UP_BOT", TASKINI_NAME), False, True))
        m_bUpTurn(1) = blnTurn

        '疵マップ左右反転（表）
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_SIDE_TOP", TASKINI_NAME), False, True))
        m_bSideTurn(0) = blnTurn

        '疵マップ左右反転（裏）
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_SIDE_BOT", TASKINI_NAME), False, True))
        m_bSideTurn(1) = blnTurn

    End Sub


    ''' <summary>
    ''' 疵マップ情報管理クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateDefMap()

        Dim typDefMap As clsDefMap.DSP_CONTROL_DATA = Nothing

        '疵マップコントロール情報セット
        For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
            typDefMap.lblMsg = lblMsg         'ガイダンスメッセージ
            typDefMap.prbMsg = prbMsg         'ガイダンス進捗

            '疵マップラベル
            Select Case ii
                Case 0
                    typDefMap.lblWs = lblWs1
                    typDefMap.lblDs = lblDs1
                Case 1
                    typDefMap.lblWs = lblWs2
                    typDefMap.lblDs = lblDs2
                Case Else
                    typDefMap.lblWs = lblWs3
                    typDefMap.lblDs = lblDs3
            End Select

            typDefMap.lblNgMsg = lblNgMsg     '異常ガイダンス表示用

            '疵マップクラス
            mcls_DefMap(ii) = New clsDefMap(typDefMap, m_bUpTurn(m_nDspMen), m_bSideTurn(m_nDspMen), 0, mcls_Log, mcls_ParamManager)
        Next

    End Sub

    ''' <summary>
    ''' 初期化処理
    ''' </summary>
    ''' <param name="blnExec">ボタン無効判定(True:無効化、False:ボタン無効化無し)</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByVal blnExec As Boolean)

        Dim ii As Integer

        '画面基本情報初期表示
        Call DspBaseInit()

        '画面初期化フラグ設定
        m_bInit = True

        '疵マップ初期表示
        For ii = 0 To DSP_DEFMAP_CNT - 1
            mcls_DefMap(ii).DspInit()
            mcls_DefMap(ii).tProp_nCoilLen = 0          '初期表示のコイル長は0とする
            mcls_DefMap(ii).tProp_nCoilWid = 0          '初期表示のコイル幅は0とする。
        Next ii
        Call RefreshMap()                           ' マップ再描画

        m_nDspStartIdx = 0

    End Sub

    ''' <summary>
    ''' 初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspBaseInit()

        '空白表示
        For ii As Integer = 0 To m_ctlMap.Length - 1
            m_ctlMap(ii).lblCoilNo.Text = ""
            m_ctlMap(ii).lblName.Text = ""
        Next

        lblFace.Text = "表面"

        'リフレッシュイベントで表示
        Call DspBase()

    End Sub

    ''' <summary>
    ''' 表示処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspBase()
        For ii As Integer = 0 To m_ctlMap.Length - 1
            m_ctlMap(ii).lblCoilNo.Refresh()
            m_ctlMap(ii).lblName.Refresh()
        Next
    End Sub


    ''' <summary>
    ''' 疵マップ再描画
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub RefreshMap()
        For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
            m_ctlMap(ii).picMap.Refresh()
        Next
    End Sub


    ''' <summary>
    ''' 表示データ取得
    ''' </summary>
    ''' <param name="bGetCoilFlg">コイル情報取得フラグ(True:取得（規定値）</param>
    ''' <remarks></remarks>
    Private Sub GetDspData(Optional ByVal bGetCoilFlg As Boolean = True)
        Dim typDspDataBuf(DSP_DEFMAP_DATCNT - 1) As clsDefMap.DSP_DATA
        Dim strSql As String
        Dim nCnt As Integer = 0
        Dim nWk As Integer
        Dim strWk As String

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing


        Try
            Call DspGuidMsgLabel(True)

            'DBオープン
            If Not tcls_DB.Open() Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_INF & "]")
                Exit Sub
            End If

            If True = bGetCoilFlg Then

                Call DspWaitInf(String.Format("コイル情報取得中"), 1)

                ' 初期化
                For ii As Integer = 0 To typDspDataBuf.Length - 1
                    typDspDataBuf(ii).Initialize()
                Next

                m_nDspStartIdx = 0

                ' 表示対象のコイル10件を取得
                strSql = ""
                strSql &= "SELECT "
                strSql &= "    TOP " + DSP_DEFMAP_DATCNT.ToString + " "
                strSql &= "    * "
                strSql &= "FROM "
                strSql &= "    " + DB_COIL_INF + " I "
                strSql &= "    INNER JOIN " + DB_COIL_RESULT + " R "
                strSql &= "    ON I.管理No = R.管理No "
                strSql &= "ORDER BY "
                strSql &= "    I.登録日付 DESC "

                sqlRead = tcls_DB.SelectExecute(strSql)
                While sqlRead.Read()
                    With typDspDataBuf(nCnt)
                        .strKizukenNo = sqlRead.GetSqlValue(sqlRead.GetOrdinal("管理No")).ToString.Trim()

                        ' コイル情報(基本部)
                        strWk = ""
                        If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("生産No")) Then
                            strWk = sqlRead.GetString(sqlRead.GetOrdinal("生産No"))
                        End If
                        .typBaseData.strSeisanNo = strWk

                        nWk = 0
                        If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("材料板幅")) Then
                            nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("材料板幅")))
                        End If
                        .typBaseData.nWidZai = nWk

                        .typBaseData.nCoilLen = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("検査実測長")))

                        .typBaseData.nPCode(0) = sqlRead.GetInt32(sqlRead.GetOrdinal("検査パターンコード_表"))
                        .typBaseData.nPCode(1) = sqlRead.GetInt32(sqlRead.GetOrdinal("検査パターンコード_裏"))

                    End With

                    nCnt += 1
                End While

                sqlRead.Close()

                For nCut As Integer = 0 To typDspDataBuf.Count - 1
                    '--------------
                    'カット位置                    
                    'カット情報取得
                    GetCutInf(typDspDataBuf(nCut), tcls_DB)

                    '--------------
                    '採寸位置   
                    '採寸情報取得
                    '採寸情報の反映は実施しないので不要
                    'GetSaisunInf(typDspDataBuf(nCnt), tcls_DB)
                Next

                m_nDspMaxIdx = nCnt - 1
                m_typDspData = typDspDataBuf
            End If

            ' 表示対象の疵情報を取得
            Dim nSetIdx As Integer = m_nDspStartIdx
            For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
                If DSP_DEFMAP_DATCNT - 1 < nSetIdx Then
                    ' 取得上限を超過したら終了
                    Exit For
                End If

                If "" = m_typDspData(nSetIdx).strKizukenNo Then
                    ' コイルが無いなら終了
                    Exit For
                End If

                If True = m_typDspData(nSetIdx).bGetDefectFlg = True Then
                    ' 疵情報取得済ならスキップ
                    nSetIdx += 1
                    Continue For
                End If

                m_typDspData(nSetIdx).bGetDefectFlg = True

                ' 疵情報取得
                GetDefectInf(nSetIdx + 1, tcls_DB, m_typDspData(nSetIdx).strKizukenNo, m_typDspData(nSetIdx))

                nSetIdx += 1
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "表示データ取得失敗")

        Finally
            If sqlRead IsNot Nothing Then
                sqlRead.Close()
                sqlRead = Nothing
            End If

            If tcls_DB IsNot Nothing Then
                tcls_DB.Cancel()
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try
    End Sub

    ''' <summary>
    ''' カット情報取得
    ''' </summary>
    ''' <param name="typDspDataBuf">表示用データ</param>
    ''' <param name="clsDB">SQLサーバー接続クラス</param>
    ''' <remarks></remarks>
    Private Function GetCutInf(ByRef typDspDataBuf As clsDefMap.DSP_DATA, ByRef clsDB As tClass.tClass_SQLServer) As Boolean
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""       'SQL文
        Dim bRet As Boolean = True      '戻り値
        Dim nWk As Integer

        With typDspDataBuf
            strSql = ""
            strSql &= "SELECT"
            strSql &= " *"
            strSql &= " FROM"
            strSql &= " " & DB_COIL_CUT_POS
            strSql &= " WHERE"
            strSql &= " 管理No='" & .strKizukenNo & "'"
            strSql &= " AND 先頭位置 > 0"
            strSql &= " ORDER BY 入側子番"
            Try
                sqlRead = clsDB.SelectExecute(strSql)
                While sqlRead IsNot Nothing AndAlso sqlRead.Read()

                    Dim typCut As clsDefMap.CUT_DATA
                    typCut.initialize()

                    nWk = 0
                    If sqlRead.IsDBNull(sqlRead.GetOrdinal("先頭位置")) Then
                        nWk = 0
                    Else
                        nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("先頭位置")))
                    End If

                    'カット線有無を判定
                    If 0 >= nWk Then
                        typCut.cut_flg = 1      'カット線無し:コイル先端(長手位置0)
                    Else
                        typCut.cut_flg = 0      'カット線あり
                    End If

                    'カット位置
                    typCut.Cut_Point = nWk

                    'カット位置情報保存
                    .CutLineList.Add(typCut)
                End While

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_CUT_POS & "]")
            Finally
                If sqlRead IsNot Nothing Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If

            End Try
        End With

    End Function

    ''' <summary>
    ''' 採寸情報取得
    ''' </summary>
    ''' <param name="typDspDataBuf">表示用データ</param>
    ''' <param name="clsDB">SQLサーバー接続クラス</param>
    ''' <remarks></remarks>
    Private Function GetSaisunInf(ByRef typDspDataBuf As clsDefMap.DSP_DATA, ByRef clsDB As tClass.tClass_SQLServer) As Boolean
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""       'SQL文
        Dim bRet As Boolean = True      '戻り値
        Dim nWk As Integer

        With typDspDataBuf
            strSql = ""
            strSql &= "SELECT "
            strSql &= "    TOP " + MAX_SHACUT_COUNT.ToString + " "
            strSql &= "    * "
            strSql &= "FROM "
            strSql &= "    " + DB_COIL_SAISUN_RESULT + " "
            strSql &= "WHERE "
            strSql &= "    管理No = '" + .strKizukenNo + "' "
            strSql &= "ORDER BY "
            strSql &= "    ID DESC, 採寸No ASC "

            Try
                sqlRead = clsDB.SelectExecute(strSql)
                While sqlRead IsNot Nothing AndAlso sqlRead.Read()

                    Dim typSaisun As clsDefMap.SAISUN_DATA
                    typSaisun.initialize()

                    '採寸開始位置
                    nWk = 0
                    If sqlRead.IsDBNull(sqlRead.GetOrdinal("採寸開始位置")) Then
                        nWk = 0
                    Else
                        nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("採寸開始位置")))
                    End If
                    typSaisun.Start_Point = nWk

                    '採寸終了位置
                    nWk = 0
                    If sqlRead.IsDBNull(sqlRead.GetOrdinal("採寸終了位置")) Then
                        nWk = 0
                    Else
                        nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("採寸終了位置")))
                    End If
                    typSaisun.End_Point = nWk

                    '採寸情報保存
                    .SaisunList.Add(typSaisun)
                End While

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_SAISUN_RESULT & "]")
            Finally
                If sqlRead IsNot Nothing Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If

            End Try
        End With

    End Function

    ''' <summary>
    ''' 疵情報取得
    ''' </summary>
    ''' <param name="strKizuKen"></param>
    ''' <remarks></remarks>
    Private Function GetDefectInf(ByVal nCoilIdx As Integer, ByRef clsDB As tClass.tClass_SQLServer, ByVal strKizuKen As String, ByRef dspData As clsDefMap.DSP_DATA) As Boolean
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""       'SQL文
        Dim bRet As Boolean = True      '戻り値
        Dim ii As Integer               'ループカウンタ
        Dim nMaxCnt As Integer = 0      '合計疵個数
        Dim nCnt As Integer             '疵個数
        Dim strTableInf As String = ""  'テーブル
        Dim strTableAttr As String = "" 'テーブル

        For nMen As Integer = 0 To NUM_MEN - 1
            dspData.DefectList(nMen).Clear()
        Next

        '疵情報取得中
        Call DspWaitInf("疵情報取得中" + vbCrLf + String.Format("(コイル{0}本目)", nCoilIdx), 1)

        '疵個数取得
        For ii = 0 To 1
            'テーブル決定(0:表,1:裏)
            strTableInf = CStr(IIf(0 = ii, DB_DEFECT_INF_T, DB_DEFECT_INF_B))
            strTableAttr = CStr(IIf(0 = ii, DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR0_B))

            'SQL文取得
            CreateDefectSql(strSql, strTableInf, strTableAttr, strKizuKen, CStr(dspData.typBaseData.nPCode(ii)), False, True)

            'レコード読み込み
            nCnt = 0
            If Not clsDB.GetSelectKey(strSql, nCnt) Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵情報個数取得エラー [" & strTableInf & "]")
                Exit Function
            End If
            nMaxCnt += nCnt
        Next ii

        '疵情報取得中
        Call DspWaitInf("疵情報取得中" + vbCrLf + String.Format("(コイル{0}本目)", nCoilIdx), nMaxCnt)

        '疵情報取得

        Try
            nCnt = 1
            For ii = 0 To 1

                'テーブル決定(0:表,1:裏)
                strTableInf = CStr(IIf(0 = ii, DB_DEFECT_INF_T, DB_DEFECT_INF_B))
                strTableAttr = CStr(IIf(0 = ii, DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR0_B))

                'SQL文作成
                CreateDefectSql(strSql, strTableInf, strTableAttr, strKizuKen, CStr(dspData.typBaseData.nPCode(ii)), True, True)

                ' Select文実行
                sqlRead = clsDB.SelectExecute(strSql)
                If sqlRead Is Nothing Then
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & strTableInf & "]")
                    Return False
                End If

                While sqlRead.Read

                    '疵情報セット
                    Call SetDefectData(dspData, sqlRead, ii, False)

                    '状況表示
                    Call DspWaitProg(nCnt)
                    nCnt = nCnt + 1

                    Application.DoEvents()

                End While

                clsDB.Cancel()
                sqlRead.Close()
                sqlRead = Nothing
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵情報取得エラー [" & ex.Message & "]")
            bRet = False

        Finally

            clsDB.Cancel()
            If Not sqlRead Is Nothing Then
                sqlRead.Close()
            End If
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' 疵情報取得SQL作成
    ''' </summary>
    ''' <param name="strSql">SQL文</param>
    ''' <param name="strTableInf">テーブル名(疵情報)</param>
    ''' <param name="strTableAttr">テーブル名(特徴量)</param>
    ''' <param name="strKizuKen">管理No</param>
    ''' <param name="strPCode">PCode</param>
    ''' <param name="bMode">取得モード(True:通常,False:個数(COUNT(*)))</param>
    ''' <param name="bNagate">長手方向(True:F→T,False:T→F)</param>
    ''' <remarks></remarks>
    Private Sub CreateDefectSql(ByRef strSql As String, ByVal strTableInf As String, ByVal strTableAttr As String,
                                ByVal strKizuKen As String, ByVal strPCode As String, ByVal bMode As Boolean, ByVal bNagate As Boolean)

        Dim sbSql As New StringBuilder

        strSql = ""

        sbSql.AppendFormat("")

        If Not bMode Then
            sbSql.AppendFormat(" SELECT COUNT(*) ")
        Else
            sbSql.AppendFormat(" SELECT")
            sbSql.AppendFormat(" I.疵No,")
            sbSql.AppendFormat(" I.表裏区分,")
            sbSql.AppendFormat(" I.判定疵種,")
            sbSql.AppendFormat(" I.判定グレード,")
            sbSql.AppendFormat(" I.長手位置,")
            sbSql.AppendFormat(" ISNULL(I.幅方向絶対位置,0) As 幅方向絶対位置")
        End If

        sbSql.AppendFormat(" FROM ")
        sbSql.AppendFormat(" ( ")
        ' 疵情報(指定された疵種)
        sbSql.AppendFormat(" ( ")
        sbSql.AppendFormat(" SELECT ")
        sbSql.AppendFormat("     D.* ")
        sbSql.AppendFormat(" FROM ")
        sbSql.AppendFormat("     {0} D ", strTableInf)
        sbSql.AppendFormat("     INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("     ON D.判定グレード = G.EdasysId ")
        sbSql.AppendFormat("     INNER JOIN  ")
        sbSql.AppendFormat("     ( ")
        sbSql.AppendFormat("     SELECT ")
        sbSql.AppendFormat("         P.疵種EdasysId, ")
        sbSql.AppendFormat("         P.入側単位疵点マップ画面表示, ")
        sbSql.AppendFormat("         G.優先度 ")
        sbSql.AppendFormat("     FROM ")
        sbSql.AppendFormat("         M_PCODE_OUTPUT P ")
        sbSql.AppendFormat("         INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("         ON P.入側単位疵点マップ画面表示 = G.EdasysId ")
        sbSql.AppendFormat("     WHERE ")
        sbSql.AppendFormat("         P.PCODE = '{0}' AND P.疵種EdasysId > -1 AND P.入側単位疵点マップ画面表示 > 0 ", strPCode)
        sbSql.AppendFormat("     ) F1 ")
        sbSql.AppendFormat("     ON F1.疵種EdasysId = D.判定疵種 AND F1.優先度 >= G.優先度 ")
        sbSql.AppendFormat(" WHERE ")
        sbSql.AppendFormat("     D.管理No = '{0}' ", strKizuKen)
        sbSql.AppendFormat(" )  ")


        sbSql.AppendFormat(" UNION ")

        ' 疵情報(その他疵)
        sbSql.AppendFormat(" ( ")
        sbSql.AppendFormat(" SELECT ")
        sbSql.AppendFormat("     D.* ")
        sbSql.AppendFormat(" FROM ")
        sbSql.AppendFormat("     {0} D ", strTableInf)
        sbSql.AppendFormat("     INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("     ON D.判定グレード = G.EdasysId ")
        sbSql.AppendFormat("     INNER JOIN  ")
        sbSql.AppendFormat("     ( ")
        sbSql.AppendFormat("     SELECT ")
        sbSql.AppendFormat("         P.疵種EdasysId, ")
        sbSql.AppendFormat("         P.入側単位疵点マップ画面表示, ")
        sbSql.AppendFormat("         G.優先度 ")
        sbSql.AppendFormat("     FROM ")
        sbSql.AppendFormat("         M_PCODE_OUTPUT P ")
        sbSql.AppendFormat("         INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("         ON P.入側単位疵点マップ画面表示 = G.EdasysId ")
        sbSql.AppendFormat("     WHERE ")
        sbSql.AppendFormat("         P.PCODE = '{0}' AND P.疵種EdasysId = -1 AND P.入側単位疵点マップ画面表示 > 0 ", strPCode)
        sbSql.AppendFormat("     ) F1 ")
        sbSql.AppendFormat("     ON F1.優先度 >= G.優先度 ")
        sbSql.AppendFormat("     INNER JOIN (   ")
        sbSql.AppendFormat("         SELECT ")
        sbSql.AppendFormat("             T.EdasysId ")
        sbSql.AppendFormat("         FROM ")
        sbSql.AppendFormat("             TYPE_NAME T ")
        sbSql.AppendFormat("         WHERE ")
        sbSql.AppendFormat("             EdasysId NOT IN ( ")
        sbSql.AppendFormat("                 SELECT ")
        sbSql.AppendFormat("                     疵種EdasysID ")
        sbSql.AppendFormat("                 FROM ")
        sbSql.AppendFormat("                     M_PCODE_OUTPUT ")
        sbSql.AppendFormat("                 WHERE ")
        sbSql.AppendFormat("                     PCODE = '{0}'  ", strPCode)
        sbSql.AppendFormat("                 AND 疵種EdasysId > -1 ")
        sbSql.AppendFormat("             ) ")
        sbSql.AppendFormat("     ) F2 ")
        sbSql.AppendFormat("     ON D.判定疵種 = F2.EdasysId  ")
        sbSql.AppendFormat(" WHERE ")
        sbSql.AppendFormat("     D.管理No = '{0}' ", strKizuKen)
        sbSql.AppendFormat(" )  ")
        sbSql.AppendFormat(" )I ")

        ' WHERE
        sbSql.AppendFormat(" WHERE I.重複結果 <> 1 ")

        If bMode Then
            If bNagate Then
                sbSql.AppendFormat(" order by I.長手位置 ASC")
            Else
                sbSql.AppendFormat(" order by I.長手位置 DESC")
            End If
        End If

        strSql = sbSql.ToString()

    End Sub

    ''' <summary>
    ''' 疵情報データセット
    ''' </summary>
    ''' <param name="sqlRead">SQLデータ</param>
    ''' <param name="nOuDiv">表裏区分</param>
    ''' <param name="bUpTrun">長手方向フラグ(True:逆転,False:正転)</param>
    ''' <remarks></remarks>
    Private Sub SetDefectData(ByRef dspData As clsDefMap.DSP_DATA, ByRef sqlRead As Data.SqlClient.SqlDataReader, ByVal nOuDiv As Integer, ByVal bUpTrun As Boolean)

        Dim typDefectInf As DETECT_BASE_DATA = Nothing  '疵情報構造体
        Dim nKizuNo As Integer = 0     '疵No
        Dim typDefMap As clsDefMap.MAP_DATA = Nothing

        Try
            '疵No 
            nKizuNo = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("疵No")))

            '疵情報データ部をセット
            With typDefectInf
                .TorB = CUShort(sqlRead.GetValue(sqlRead.GetOrdinal("表裏区分")))
                .nTid = CUShort(sqlRead.GetValue(sqlRead.GetOrdinal("判定疵種")))
                .nGid = CUShort(sqlRead.GetValue(sqlRead.GetOrdinal("判定グレード")))
                .y = CSng(sqlRead.GetValue(sqlRead.GetOrdinal("長手位置")))
                .nKizuNo = nKizuNo

                '長手方向逆転の場合、コイル長から引く
                If bUpTrun Then .y = dspData.typBaseData.nCoilLen * 1000 - .y

            End With

            ''疵マップ画面表示用疵情報取得

            '構造体初期化
            typDefMap.initialize()

            With typDefMap
                .t_id = typDefectInf.nTid
                .g_id = typDefectInf.nGid
                '長手方向逆転は疵マップ画面表示時に考慮する為、反転前情報を取得
                .y = CSng(sqlRead.GetValue(sqlRead.GetOrdinal("長手位置")))
                .absX = CSng(sqlRead.GetValue(sqlRead.GetOrdinal("幅方向絶対位置")))
            End With

            '長手位置最大の疵位置を保存
            If dspData.nMaxDefPos < CInt(typDefMap.y) Then dspData.nMaxDefPos = CInt(typDefMap.y)

            '疵マップ表示情報保存
            dspData.DefectList(nOuDiv).Add(typDefMap)

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵情報作成エラー [" & ex.Message & "]")
            Exit Sub
        End Try
    End Sub


    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData(Optional ByVal nDspStartIdx As Integer = 0, Optional ByVal nDspMen As Integer = 0)

        Dim strKensaO As String = ""        ' 検査基準_表
        Dim strKensaU As String = ""        ' 検査基準_裏
        Dim nCoilWid As Integer
        Dim nIndex As Integer = 0

        'ボタン無効化
        Call EnableCoilChangeButton(False)

        '画面初期化フラグ解除
        m_bInit = False

        '異常ガイダンス表示初期化
        DspNgGuid(False)

        m_nDspStartIdx = nDspStartIdx
        m_nDspMen = nDspMen

        ' 表示面表示
        If 0 = nDspMen Then
            lblFace.Text = "表面"
            lblFace.ForeColor = Color.Yellow
            lblFace.BackColor = Color.Blue
        Else
            lblFace.Text = "裏面"
            lblFace.ForeColor = Color.Black
            lblFace.BackColor = Color.Orange
        End If

        '★基本情報表示
        nIndex = 0
        For ii As Integer = nDspStartIdx To m_typDspData.Length - 1
            With m_typDspData(ii)
                m_ctlMap(nIndex).lblCoilNo.Text = .typBaseData.strSeisanNo

                Dim dFontSize As Single = GetLabelFontScaleSize(m_ctlMap(nIndex).lblCoilNo, m_ctlMap(nIndex).lblCoilNo.Text, 16, m_ctlMap(nIndex).lblCoilNo.Width)
                m_ctlMap(nIndex).lblCoilNo.Font = New System.Drawing.Font("ＭＳ 明朝", dFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End With
            m_ctlMap(nIndex).lblName.Text = String.Format("{0}本前", ii + 1)

            nIndex += 1
            If nIndex >= DSP_DEFMAP_CNT Then
                Exit For
            End If
        Next

        'リフレッシュイベントで表示
        Call DspBase()

        nIndex = 0
        For ii As Integer = nDspStartIdx To m_typDspData.Length - 1
            With m_typDspData(ii)
                nCoilWid = .typBaseData.nWidZai                          'コイル幅取得

                If .nMaxDefPos < 0 Then
                    '最長疵位置がマイナス値となる場合は0をセット
                    .nMaxDefPos = 0
                End If

                '★疵マップ情報表示
                '疵マップ設定

                ' コイル長
                If .nMaxDefPos <= .typBaseData.nCoilLen Then
                    mcls_DefMap(nIndex).tProp_nCoilLen = .typBaseData.nCoilLen       '検査実測長をセット
                Else
                    mcls_DefMap(nIndex).tProp_nCoilLen = .nMaxDefPos                 '最長疵位置をセット
                End If
                mcls_DefMap(nIndex).tProp_nCoilWid = nCoilWid                        'コイル幅セット
                mcls_DefMap(nIndex).tProp_DefMapData = .DefectList(nDspMen)          '疵情報セット
                mcls_DefMap(nIndex).tProp_nMode = nDspMen                            '表裏区分セット
                mcls_DefMap(nIndex).tProp_bUpTurn = m_bUpTurn(nDspMen)               '上下反転セット
                mcls_DefMap(nIndex).tProp_bSideTurn = m_bSideTurn(nDspMen)           '左右反転セット
            End With

            m_ctlMap(nIndex).picMap.Tag = m_typDspData(ii)

            nIndex += 1
            If nIndex >= DSP_DEFMAP_CNT Then
                Exit For
            End If
        Next

        ' 残りはクリア
        For ii As Integer = nIndex To DSP_DEFMAP_CNT - 1
            mcls_DefMap(ii).DspInit()
            m_ctlMap(ii).picMap.Tag = Nothing
            m_ctlMap(ii).lblName.Text = ""
            m_ctlMap(ii).lblCoilNo.Text = ""
        Next

        'ガイダンス表示
        DspGuidMsgLabel(True)

        '疵マップ描画
        Call RefreshMap()

        'ガイダンス表示
        DspGuidMsgLabel(False)

        ' ボタン有効化
        Call EnableCoilChangeButton(True)

    End Sub

    ''' <summary>
    ''' フォームクローズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' メインスレッド終了通知
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' 閉じるボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As Object, ByVal e As System.EventArgs)

        Me.Close()

    End Sub

    ''' <summary>
    ''' 疵マップ ペイントイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picMap_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        '画面ロード中イベントは無視
        If m_bLoad Then Exit Sub

        ' 描画対象コントロールの識別
        Dim nIndex As Integer = CInt(CType(sender, PictureBox).Name.Replace("picMap", "")) - 1

        If CType(sender, PictureBox).Tag Is Nothing OrElse Not (TypeOf CType(sender, PictureBox).Tag Is clsDefMap.DSP_DATA) Then
            Exit Sub
        End If

        Dim dspData As clsDefMap.DSP_DATA = CType(CType(sender, PictureBox).Tag, clsDefMap.DSP_DATA)

        '対象外塗り潰し
        '採寸情報の反映は実施しない
        'mcls_DefMap(nIndex).pinMapCutDrw(sender, e, dspData.SaisunList)

        '疵マップベース表示
        mcls_DefMap(nIndex).picMapBase_Paint(sender, e)

        '疵マップカット位置表示
        If Not m_bInit Then
            mcls_DefMap(nIndex).picMapCut_Paint(sender, e, dspData.CutLineList)
        End If

        '疵マップデータ表示（マーカー表示）
        mcls_DefMap(nIndex).picMapMarker_Paint(sender, e)
    End Sub

    ''' <summary>
    ''' ガイダンス欄表示
    ''' </summary>
    ''' <param name="blnExec"></param>
    ''' <remarks></remarks>
    Private Sub DspGuidMsgLabel(ByVal blnExec As Boolean)

        'データ取得中。。。
        pnlMsg.Visible = blnExec
        grpGuid.Visible = blnExec

        'ガイダンス表示初期化
        If Not blnExec Then
            lblMsg.Text = ""
            prbMsg.Value = 0
        End If
        'リフレッシュイベントで表示
        pnlMsg.Refresh()
        grpGuid.Refresh()
        lblMsg.Refresh()
        prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' 異常結果ガイダンス表示
    ''' </summary>
    ''' <param name="blnExec"></param>
    ''' <remarks></remarks>
    Private Sub DspNgGuid(ByVal blnExec As Boolean)

        '異常結果ガイダンス
        grpNgGuid.Visible = blnExec
        If Not blnExec Then lblNgMsg.Text = ""

        'リフレッシュイベントで表示
        grpNgGuid.Refresh()
        lblNgMsg.Refresh()

    End Sub

    ''' <summary>
    ''' データ取得状況表示中
    ''' </summary>
    ''' <param name="strVal"></param>
    ''' <param name="iMax"></param>
    ''' <remarks></remarks>
    Private Sub DspWaitInf(ByVal strVal As String, ByVal iMax As Long)
        lblMsg.Text = strVal
        prbMsg.Minimum = 0
        prbMsg.Maximum = CInt(IIf(iMax > prbMsg.Minimum, iMax, prbMsg.Minimum + 1))
        prbMsg.Value = 0

        'リフレッシュイベントで表示
        pnlMsg.Refresh()
        grpGuid.Refresh()
        lblMsg.Refresh()
        prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' データ取得中ガイダンス 更新
    ''' </summary>
    ''' <param name="nVal">進捗値</param>
    ''' <remarks></remarks>
    Private Sub DspWaitProg(ByVal nVal As Integer)

        '進捗値セット
        prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))

    End Sub

    ''' <summary>
    ''' 疵マップ実績画面表示（画面起動時のダイヤログ表示対応）
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tmrDsp.Tick
        '初期表示終了までイベントを無視
        If Not m_bLoad = True Then
            'タイマー停止
            tmrDsp.Enabled = False
            Application.DoEvents()

            'データ取得                
            Call GetDspData()

            '疵マップ実績画面表示
            Call DspData()

            Me.BringToFront()
        End If
    End Sub

    ''' <summary>
    ''' 疵点マップ強制最新表示
    ''' </summary>
    ''' <param name="bReadCoil">コイル情報再読込フラグ</param>
    ''' <remarks></remarks>
    Private Sub ForceDspNewData(ByVal bReadCoil As Boolean)
        '自動モードに切替
        Call ChangeRefreshMode(EM_REFRESH_MODE.AUTO)

        'データ取得                
        Call GetDspData(bReadCoil)

        '疵マップ実績画面表示
        Call DspData(0, m_nDspMen)
    End Sub


    ''' <summary>
    ''' ファンクションボタン押下イベント
    ''' </summary>
    ''' <param name="FncNo">0オリジン</param>
    ''' <param name="onKey">ボタンクリック時も取れるよ</param>
    ''' <remarks></remarks>
    Private Sub frmMain_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン
            Case EM_BTN_MODE.nPreCoil
                ' F1:前コイル
                Call ChangeDspCoil(m_nDspStartIdx - 1)

            Case EM_BTN_MODE.nNextCoil
                ' F2:次コイル
                Call ChangeDspCoil(m_nDspStartIdx + 1)

            Case EM_BTN_MODE.nChangeOU
                ' F3:裏面表示（表面表示）
                Dim nDspMen As Integer = CInt(IIf(0 = m_nDspMen, 1, 0))
                Call ChangeDspMen(nDspMen)

            Case EM_BTN_MODE.nStop
                ' F5:一時停止
                Dim nRefreshMode As EM_REFRESH_MODE = CType(IIf(EM_REFRESH_MODE.AUTO = m_nRefreshMode, EM_REFRESH_MODE.PAUSE, EM_REFRESH_MODE.AUTO), EM_REFRESH_MODE)
                Call ChangeRefreshMode(nRefreshMode)

            Case EM_BTN_MODE.nPrint
                ' F10:画面印刷
                MyBase.PrintOut()

            Case EM_BTN_MODE.nSave
                ' F11:画面保存
                MyBase.HardCopy(IMG_PATH, "疵点マップ画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

            Case EM_BTN_MODE.nClose
                ' F12:閉じる
                Me.Close()
        End Select

        '呼び元へ通知
        RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub

    ''' <summary>
    ''' 表示コイル切替
    ''' </summary>
    ''' <param name="nStartIdx">表示開始Index</param>
    ''' <remarks></remarks>
    Private Sub ChangeDspCoil(ByVal nStartIdx As Integer)
        m_nDspStartIdx = nStartIdx

        ' データ取得(疵情報のみ)
        Call GetDspData(False)

        ' 表示
        Call DspData(nStartIdx, m_nDspMen)
    End Sub


    ''' <summary>
    ''' 表裏面切替
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeDspMen(ByVal nDspMen As Integer)
        If 0 = nDspMen Then
            btnF03.Text = "裏面表示"
        Else
            btnF03.Text = "表面表示"
        End If

        Call DspData(m_nDspStartIdx, nDspMen)
    End Sub

    ''' <summary>
    ''' 表示モード切替
    ''' </summary>
    ''' <param name="nRefreshMode"></param>
    ''' <remarks></remarks>
    Private Sub ChangeRefreshMode(ByVal nRefreshMode As EM_REFRESH_MODE)
        If EM_REFRESH_MODE.PAUSE = nRefreshMode Then
            ' 一時停止
            m_nRefreshMode = EM_REFRESH_MODE.PAUSE
            Call EnableCoilChangeButton(True)
            lblMode.Text = "一時停止中"
            lblMode.BackColor = Color.Red
            lblMode.ForeColor = Color.White
            btnF05.Text = "停止解除"
            btnF05.BackColor = Color.Red
            btnF05.ForeColor = Color.Yellow
        Else
            ' 自動更新
            m_nRefreshMode = EM_REFRESH_MODE.AUTO
            Call EnableCoilChangeButton(False)
            lblMode.Text = "自動更新中"
            lblMode.BackColor = Color.Blue
            lblMode.ForeColor = Color.Yellow
            btnF05.Text = "一時停止"
            btnF05.BackColor = Color.FromKnownColor(KnownColor.Control)
            btnF05.ForeColor = Color.Black

            ' 最新表示
            If m_nDspStartIdx > 0 Then
                Call DspData(0, m_nDspMen)
            End If
        End If
    End Sub

    ''' <summary>
    ''' 前コイル、次コイルボタン制御
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub EnableCoilChangeButton(ByVal bEnable As Boolean)
        If True = bEnable Then
            If EM_REFRESH_MODE.PAUSE = m_nRefreshMode Then
                If 0 >= m_nDspStartIdx Then
                    btnF01.Enabled = False
                Else
                    btnF01.Enabled = True
                End If

                If m_nDspMaxIdx - DSP_DEFMAP_CNT >= m_nDspStartIdx Then
                    btnF02.Enabled = True
                Else
                    btnF02.Enabled = False
                End If
            Else
                btnF01.Enabled = False
                btnF02.Enabled = False
            End If
        Else
            btnF01.Enabled = False
            btnF02.Enabled = False
        End If
    End Sub

    ''' <summary>
    ''' パラメータ変更処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TranParam(ByVal nMode As Integer)

        Dim dele As New m_deleChangeParam(AddressOf ChangeParam)

        ' 別スレッドからのアクセス可能とする
        If Me.InvokeRequired Then                   ' 別スレッドからのアクセス要求
            Me.Invoke(dele, nMode)
        Else                                        ' フォーム生成スレッドからのアクセス要求
            dele.DynamicInvoke()
        End If

    End Sub

    ''' <summary>
    ''' パラメータ変更
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeParam(ByVal nMode As Integer)

        ' パラメータ再読込
        mcls_ParamManager.ReadAll()

        ' 画面再描画
        Call ForceDspNewData(True)

    End Sub

    ''' <summary>
    ''' 画面更新通知
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TranDspNew()

        Dim dele As New m_deleDspNew(AddressOf ForceDspNewData)

        ' 別スレッドからのアクセス可能とする
        If Me.InvokeRequired Then                   ' 別スレッドからのアクセス要求
            Me.Invoke(dele, True)
        Else                                        ' フォーム生成スレッドからのアクセス要求
            dele.DynamicInvoke()
        End If

    End Sub

End Class
