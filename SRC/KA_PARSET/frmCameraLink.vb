'*********************************************************************************
' カメラリンク設定画面クラス
'	[Ver]
'		Ver.01    2019/04/10  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class frmCameraLink
    '/ ////////////////////////////////////////////////////////////////////////////////
    '  列挙体
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ロジックパラランPC
    ''' </summary>
    Private Enum EM_PARAPC
        PARA_PC1 = 0          ' ロジックパラランPC１
        PARA_PC2 = 1          ' ロジックパラランPC２
    End Enum

    '/ ////////////////////////////////////////////////////////////////////////////////
    '  構造体
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' M_PC_MASTERテーブルのPC情報を格納
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure PC_MASTER_VALUE
        Dim nPcId As Integer            ' PCID
        Dim nGroupNo As Integer         ' GroupNo
    End Structure

    ''' <summary>
    ''' パララン設定値
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure PARA_VALUE
        Dim nPcId As Integer            ' PCID
        Dim nLinkSw As Integer          ' リンクスイッチ種別
        Dim nFPGABoard As Integer       ' FPGAボードNo
        Dim nConnectCh As Integer       ' 接続CH（画面用　FPGA0,FPGA1の値が格納される）
        Dim nConnectChOutPut As Integer ' 接続CH（設定ファイル出力用 10～15の値が格納される）
        Dim bGetFlg As Boolean          ' 取得フラグ

        Public Sub Initialize()
            nPcId = 0
            nLinkSw = 0
            nFPGABoard = 0
            nConnectCh = 0
            nConnectChOutPut = 0
            bGetFlg = False
        End Sub
    End Structure

    ''' <summary> 統括iniファイル定義 </summary>
    Private Structure TOKATU_PC_INI
        Dim strIniPath_Set As String        ' 統括PC iniファイル 反映用
        Dim strIniPath_Exe As String        ' 統括PC iniファイル EXE下
        Dim strToIpAddr As String           ' 統括PC IPアドレス
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル
    Private m_lstPcMaster As List(Of PC_MASTER_VALUE) = New List(Of PC_MASTER_VALUE)        ' M_PC_MASTERテーブルのPC情報リスト
    Private m_nParaPcId(NUM_HANTEI_PARA - 1) As Integer                                     ' ロジックパラランPCのPCID格納先
    Private m_typNowPara(NUM_HANTEI_PARA - 1, NUM_CAMLINK_BOARD - 1) As PARA_VALUE          ' ロジックパラランPC接続設定（現在値）
    Private m_lstConnectCh(NUM_CAMLINKSW - 1) As List(Of Integer)                           ' 使用済接続CHリスト（リンクスイッチ毎）
    Private m_nLinkSwSelectIdx(NUM_HANTEI_PARA - 1) As Integer                              ' 現在選択中のリンクスイッチ（IDX）
    Private m_typTokatuIni As TOKATU_PC_INI                                                 ' 統括iniファイル定義
    Private m_bIniReadFlg As Boolean                                                        ' iniファイル読み込み正常／異常フラグ(異常時は参照のみ)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const PARA_PC_GROUP_NO As Integer = 5                   ' ロジックパラランPC　グループNo
    Private Const PARA_PC1_CAMERA_KIND As Integer = 2               ' ロジックパラランPC１　カメラ入力CH区分
    Private Const PARA_PC2_CAMERA_KIND As Integer = 3               ' ロジックパラランPC１　カメラ入力CH区分
    Private Const CAMERA_LINK_NAME As String = "LINKSW"             ' カメラリンクスイッチのコンボボックスで使用する名称
    Private Const FPGA0_CONNECTOR_MIN As Integer = 10               ' FPGA0での接続CH選択最小値
    Private Const FPGA0_CONNECTOR_MAX As Integer = 15               ' FPGA0での接続CH選択最大値
    Private Const FPGA1_CONNECTOR_MIN As Integer = 25               ' FPGA1での接続CH選択最小値
    Private Const FPGA1_CONNECTOR_MAX As Integer = 30               ' FPGA1での接続CH選択最大値


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "フォーム表示/終了イベント関連"
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmCameraLink            ' カメラリンク設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        '''' メンバ変数インスタンス化
        For ii As Integer = 0 To NUM_CAMLINKSW - 1
            m_lstConnectCh(ii) = New List(Of Integer)
        Next


        '''' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If


        '''' INIファイルから統括PC情報を取得
        m_bIniReadFlg = True
        If Not Read_Ini() Then
            gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.WAR, "iniファイル読込失敗のため、参照のみ")
            m_bIniReadFlg = False
        Else
            ' 設定対象PCか判定する。統括PCに接続可能かで判定。
            If Not CheckSettingPossiblePC() Then
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.WAR, "統括PCに接続不可のため、参照のみ")
                m_bIniReadFlg = False
            End If
        End If

        If m_bIniReadFlg = False Then
            ' INIファイル取得失敗時は更新ボタン無効
            UpdateEnabled(False)
        Else
            ' 更新ボタンの有効/無効
            Call UpdateEnabled(g_bPassFlg)
        End If


        '''' DBから取得した値をメンバ変数に設定
        If Not SetDBData() Then
            ' DB取得値異常なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB取得値が異常の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If


        '''' 画面に値を表示        
        If Not Data_Disp() Then
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("画面表示中に異常が発生した為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If


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
    Private Sub frmCameraLink_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    '''  iniファイル読み込み
    ''' </summary>
    ''' <returns>true:読み込み成功 false:読み込み失敗</returns>
    ''' <remarks>各前工程のiniファイル定義を読み込む</remarks>
    Private Function Read_Ini() As Boolean
        Dim bRet As Boolean = True
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        Dim key As String = ""

        Try

            '''' 統括PC IPアドレス取得
            key = "PC_IPADDR"
            m_typTokatuIni.strToIpAddr = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(KA_PARSET, key, TASKINI_NAME)
            If m_typTokatuIni.strToIpAddr = "" Then Throw New Exception(String.Format("iniファイル セクション[KA_PARSET] キー {1}が定義されていない為、" & vbCrLf & "「カメラリンク設定」" & vbCrLf & "は参照のみとなります。", key))

            '''' 統括PC EXE下iniファイルパス取得
            key = "INI_SAVE_PATH_EXE"
            m_typTokatuIni.strIniPath_Exe = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(KA_PARSET, key, TASKINI_NAME)
            If m_typTokatuIni.strIniPath_Exe = "" Then Throw New Exception(String.Format("iniファイル セクション[KA_PARSET] キー {1}が定義されていない為、" & vbCrLf & "「カメラリンク設定」" & vbCrLf & "は参照のみとなります。", key))

            '''' 統括PC 反映用iniファイルパス取得
            key = "INI_SAVE_PATH_INISET"
            m_typTokatuIni.strIniPath_Set = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(KA_PARSET, key, TASKINI_NAME)
            If m_typTokatuIni.strIniPath_Set = "" Then Throw New Exception(String.Format("iniファイル セクション[KA_PARSET] キー {1}が定義されていない為、" & vbCrLf & "「カメラリンク設定」" & vbCrLf & "は参照のみとなります。", key))

        Catch ex As Exception
            bRet = False
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' 設定可能PCか判定
    ''' </summary>
    ''' <returns>(True:設定可能 False:設定不可)</returns>
    ''' <remarks></remarks>
    Private Function CheckSettingPossiblePC() As Boolean

        '''' DBに登録された前工程コードが登録されているか
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "INIファイルの設定可否チェック")

        ' 統括PCにPingが通るか
        If Not PingCheck(m_typTokatuIni.strToIpAddr) Then Return False
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "統括PC Pingチェック完了 IP:" & m_typTokatuIni.strToIpAddr)
        ' 統括PC EXE下iniファイルが存在しているか
        If Not System.IO.File.Exists(m_typTokatuIni.strIniPath_Exe) Then Return False
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "統括PC EXE下iniファイルチェック完了 FILE:" & m_typTokatuIni.strIniPath_Exe)
        ' 統括PC 反映用iniファイルが存在しているか
        If Not System.IO.File.Exists(m_typTokatuIni.strIniPath_Set) Then Return False
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "統括PC 反映用iniファイルチェック完了 FILE:" & m_typTokatuIni.strIniPath_Set)

        Return True

    End Function

    ''' <summary>
    ''' ping確認
    ''' </summary>
    ''' <param name="IPaddress">ping確認のサーバーIPアドレス</param>
    ''' <returns>true:ping成功 false:ping失敗</returns>
    ''' <remarks></remarks>
    Private Function PingCheck(ByVal IPaddress As String) As Boolean
        Dim bRet As Boolean = True
        Try
            If Not My.Computer.Network.Ping(IPaddress) Then
                Throw New Exception("Ping 失敗")
            End If
        Catch ex As Exception
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' DBから取得した値をメンバ変数に設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Function SetDBData() As Boolean
        Dim nParaPc As Integer = 0
        Dim rows() As DataRow = m_DS.Tables(0).Select()

        ' DBから取得したPC情報分ループ
        For ii As Integer = 0 To rows.Length - 1
            Dim pcMasterVal As PC_MASTER_VALUE

            '''' PC情報リストに設定
            pcMasterVal.nPcId = CInt(rows(ii).Item("PCID"))         ' PCID
            pcMasterVal.nGroupNo = CInt(rows(ii).Item("GroupNo"))   ' GroupNo
            m_lstPcMaster.Add(pcMasterVal)

            '''' ロジックパラランPCIDリストに設定
            If PARA_PC_GROUP_NO = pcMasterVal.nGroupNo Then

                ' ロジックパラランPCが最大値以上設定されていた場合
                If NUM_HANTEI_PARA = nParaPc Then Return False

                ' グループNoが一致した場合ロジックパラランPCIDリストに格納
                ' PCIDが小さいPCからロジックパラランPC1,2,･･･とする
                m_nParaPcId(nParaPc) = pcMasterVal.nPcId
                nParaPc += 1
            End If
        Next

        ' ロジックパラランPCの数が最大値以外
        If NUM_HANTEI_PARA <> nParaPc Then Return False

        Return True
    End Function

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub

    ''' <summary>
    ''' 画面変更要求されたときの処理 (継承元クラスよりコールされる)
    ''' </summary>
    ''' <param name="frmID">選択画面ID</param>
    ''' <param name="cancel">選択を取りやめたい場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)
    End Sub
#End Region

#Region "DB関連処理"
    ''' <summary>
    ''' PCマスターからPC情報を取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT PCID "
        strSQL &= ",GroupNo "
        strSQL &= " FROM " & DB_PC_MASTER
        strSQL &= " ORDER BY PCID"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function
#End Region

#Region "画面表示関連"
    ''' <summary>
    ''' 設定ファイル情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean
        Dim bResult As Boolean = False

        ' LINKSWコンボボックスに値を設定する為イベントを解除
        RemoveHandler cmbLinkSw1.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged
        RemoveHandler cmbLinkSw2.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged

        Try
            '''' 画面項目の初期化
            ItemInitialize()

            '''' 接続CH設定
            SetConnector()

            '''' 画面リンクスイッチコンボボックスに値を設定
            SetLinkSwComboBox()

            '''' 画面接続CHコンボボックスに値を設定
            SetConnectComboBox(EM_PARAPC.PARA_PC1, cmbLinkSw1, cmbPara1ConnectCh1, cmbPara1ConnectCh2)
            SetConnectComboBox(EM_PARAPC.PARA_PC2, cmbLinkSw2, cmbPara2ConnectCh1, cmbPara2ConnectCh2)

            bResult = True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定ファイル情報表示エラー [" & ex.Message & "] " & Me.Text)
        Finally
            ' LINKSWコンボボックスに値を設定する為イベントを再設定
            AddHandler cmbLinkSw1.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged
            AddHandler cmbLinkSw2.SelectedIndexChanged, AddressOf cmbLinkSw_SelectedIndexChanged
        End Try

        Return bResult
    End Function

    ''' <summary>
    ''' 項目初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ItemInitialize()
        ' リンクスイッチコンボボックス初期化
        cmbLinkSw1.Items.Clear()
        cmbLinkSw2.Items.Clear()

        ' リンクスイッチコンボボックス項目値設定
        For ii As Integer = 1 To NUM_CAMLINKSW
            cmbLinkSw1.Items.Add("LINKSW" & ii)
            cmbLinkSw2.Items.Add("LINKSW" & ii)
        Next

        ' 接続CHコンボボックス 初期化
        cmbPara1ConnectCh1.Items.Clear()
        cmbPara1ConnectCh2.Items.Clear()
        cmbPara2ConnectCh1.Items.Clear()
        cmbPara2ConnectCh2.Items.Clear()

        ' 使用済接続CH 初期化
        For ii As Integer = 0 To NUM_CAMLINKSW - 1
            m_lstConnectCh(ii).Clear()
        Next

        ' ロジックパラランPC接続設定 初期化
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                m_typNowPara(ii, jj).Initialize()
            Next
        Next

    End Sub

    ''' <summary>
    ''' 接続CH設定
    ''' リンクスイッチ毎にロジックパラランPC以外で使用されている接続CHを取得
    ''' ロジックパラランPCの接続CH設定を取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetConnector()

        ' 統括PCにPingが通るか
        If Not PingCheck(m_typTokatuIni.strToIpAddr) Then Return

        ' DBから取得したPCID分ループ
        For Each pcMasterVal As PC_MASTER_VALUE In m_lstPcMaster
            For ii As Integer = 1 To NUM_CAMLINK_BOARD
                '''' INIファイルからカメラリンク設定を取得
                Dim strItemName As String = String.Format("CAMLINKSW_PCID{0}_{1}", pcMasterVal.nPcId, ii)
                Dim nCameraLink As Integer = tMod_IniFile.ReadIniInt(TO_DEFECT, strItemName, m_typTokatuIni.strIniPath_Exe, 0)

                ' カメラリンク未設定
                If 0 >= nCameraLink Then Continue For


                '''' カメラリンク設定からFPGAボードNo、接続コネクタ取得
                Dim nFPGABoard As Integer = nCameraLink \ 100                   ' FPGAボードNo
                Dim nConnectCh As Integer = nCameraLink - (nFPGABoard * 100)    ' 接続CH（画面用）
                Dim nConnectChOutPut As Integer = nConnectCh                    ' 接続CH（ファイル出力用）

                ' FPGA1だった場合、INIファイルの接続CHをFPGA1用の接続CHに変換
                If 0 <> nFPGABoard Mod NUM_CAMLINK_BOARD Then
                    nConnectCh = nConnectCh - FPGA0_CONNECTOR_MIN + FPGA1_CONNECTOR_MIN
                End If


                '''' FPGAボードNoからリンクスイッチを取得
                Dim nLinkSw As Integer = nFPGABoard \ NUM_CAMLINK_BOARD + 1

                ' LinkSwが対象外
                If nLinkSw > m_lstConnectCh.Length Then Continue For


                '''' ロジックパラランPCの接続設定を取得
                Dim bChAddFlg As Boolean = True      ' 使用済接続CH追加フラグ
                For jj As Integer = 0 To m_nParaPcId.Length - 1

                    ' ロジックパラランPCのPCIDと一致
                    If pcMasterVal.nPcId = m_nParaPcId(jj) Then
                        ' ロジックパラランPCの現在設定値を取得
                        m_typNowPara(jj, ii - 1).nPcId = pcMasterVal.nPcId
                        m_typNowPara(jj, ii - 1).nLinkSw = nLinkSw
                        m_typNowPara(jj, ii - 1).nFPGABoard = nFPGABoard
                        m_typNowPara(jj, ii - 1).nConnectCh = nConnectCh
                        m_typNowPara(jj, ii - 1).nConnectChOutPut = nConnectChOutPut
                        m_typNowPara(jj, ii - 1).bGetFlg = True

                        ' ロジックパラランPCに設定されている接続CHは使用済接続CHには追加しない
                        bChAddFlg = False
                        Exit For
                    End If
                Next


                '''' リンクスイッチ毎にロジックパラランPC以外で使用している接続CHを格納
                If bChAddFlg Then m_lstConnectCh(nLinkSw - 1).Add(nConnectCh)
            Next
        Next

    End Sub

    ''' <summary>
    ''' リンクスイッチコンボボックス設定
    ''' 画面のLinkSwコンボボックスに設定ファイルから取得したLinkSwの値を設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetLinkSwComboBox()
        Dim nSelectIdx As Integer

        '''' ロジックパラランPC1の設定
        ' デフォルト値は LINKSW1
        nSelectIdx = 0
        ' 接続先数分ループ
        For ii As Integer = 0 To NUM_CAMLINK_BOARD - 1

            ' ロジックパラランPC1のLinkSwを取得済
            If 0 < m_typNowPara(EM_PARAPC.PARA_PC1, ii).nLinkSw Then
                nSelectIdx = m_typNowPara(EM_PARAPC.PARA_PC1, ii).nLinkSw - 1
                Exit For
            End If

            ' LinkSwが取得できるまでループ
        Next
        cmbLinkSw1.SelectedIndex = nSelectIdx
        m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC1) = nSelectIdx

        '''' ロジックパラランPC2の設定
        ' デフォルト値は LINKSW1
        nSelectIdx = 0
        ' 接続先数分ループ
        For ii As Integer = 0 To NUM_CAMLINK_BOARD - 1

            ' ロジックパラランPC2のLinkSwを取得済
            If 0 < m_typNowPara(EM_PARAPC.PARA_PC2, ii).nLinkSw Then
                nSelectIdx = m_typNowPara(EM_PARAPC.PARA_PC2, ii).nLinkSw - 1
                Exit For
            End If

            ' LinkSwが取得できるまでループ
        Next
        cmbLinkSw2.SelectedIndex = nSelectIdx
        m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC2) = nSelectIdx

    End Sub

    ''' <summary>
    ''' 接続CHコンボボックス設定
    ''' 画面の接続CHコンボボックスに値を設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetConnectComboBox(ByVal nPC As EM_PARAPC, ByRef cmbLinkSw As ComboBox, ByRef cmbParaConCh1 As ComboBox, ByRef cmbParaConCh2 As ComboBox)
        Dim nLinkSwPara As Integer = 1  ' デフォルトはLINKSW1
        If 0 <= cmbLinkSw.SelectedIndex Then nLinkSwPara = cmbLinkSw.SelectedIndex + 1

        '''' 接続CHの追加
        ' FPGA0の接続CHを追加
        For ii As Integer = FPGA0_CONNECTOR_MIN To FPGA0_CONNECTOR_MAX

            '''' ロジックパラランPCの接続CH追加
            ' 既に使用済みの接続CHは選択不可
            If False = m_lstConnectCh(nLinkSwPara - 1).Contains(ii) Then
                cmbParaConCh1.Items.Add("CH" & ii)
                cmbParaConCh2.Items.Add("CH" & ii)
            End If
        Next

        ' FPGA1の接続CHを追加
        For ii As Integer = FPGA1_CONNECTOR_MIN To FPGA1_CONNECTOR_MAX

            '''' ロジックパラランPCの接続CH追加
            ' 既に使用済みの接続CHは選択不可
            If False = m_lstConnectCh(nLinkSwPara - 1).Contains(ii) Then
                cmbParaConCh1.Items.Add("CH" & ii)
                cmbParaConCh2.Items.Add("CH" & ii)
            End If
        Next


        '''' 初期選択値設定（存在しない接続CHを設定していた場合は未選択になる）
        cmbParaConCh1.SelectedIndex = cmbParaConCh1.FindString("CH" & m_typNowPara(nPC, 0).nConnectCh)
        cmbParaConCh2.SelectedIndex = cmbParaConCh2.FindString("CH" & m_typNowPara(nPC, 1).nConnectCh)
    End Sub

#End Region

#Region "INIファイル更新関連"
    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            '''' 画面設定値からロジックパラランPCの接続先を取得
            ' ロジックパラランPC１
            Dim typParaVal(NUM_HANTEI_PARA - 1, NUM_CAMLINK_BOARD - 1) As PARA_VALUE
            ' 接続先1の設定
            typParaVal(0, 0) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC1), cmbLinkSw1, cmbPara1ConnectCh1)
            ' 接続先2の設定
            typParaVal(0, 1) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC1), cmbLinkSw1, cmbPara1ConnectCh2)

            ' ロジックパラランPC２
            ' 接続先1の設定
            typParaVal(1, 0) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC2), cmbLinkSw2, cmbPara2ConnectCh1)
            ' 接続先2の設定
            typParaVal(1, 1) = GetParaSetting(m_nParaPcId(EM_PARAPC.PARA_PC2), cmbLinkSw2, cmbPara2ConnectCh2)


            '''' 入力値チェック
            If Not Data_Check(typParaVal) Then Exit Sub


            '''' メッセージ表示
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub


            '''' INIファイル更新
            If INI_UpDate(typParaVal) Then
                '''' ハードコピー保存
                modMain.HardCopy(Me)

                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If


            '''' 画面に値を表示 
            If Not Data_Disp() Then
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("画面表示中に異常が発生した為 画面を強制終了します", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '終了
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定ボタン押下時異常" & Me.Text)
        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' 画面項目からロジックパラランPC設定値を取得
    ''' </summary>
    ''' <param name="nPcId">ロジックパラランPCID</param>
    ''' <param name="cmbLinkSw">取得対象のカメラリンクスイッチコンボボックス</param>
    ''' <param name="cmbParaConnectCh">取得対象の接続CH</param>
    ''' <returns>>ロジックパラランPC設定値</returns>
    ''' <remarks></remarks>
    Private Function GetParaSetting(ByVal nPcId As Integer, ByVal cmbLinkSw As ComboBox, ByVal cmbParaConnectCh As ComboBox) As PARA_VALUE

        '''' ロジックパラランPC設定を取得
        Dim typParaVal As PARA_VALUE
        typParaVal.nPcId = nPcId
        typParaVal.nLinkSw = CInt(cmbLinkSw.Text.ToString.Replace(CAMERA_LINK_NAME, ""))
        typParaVal.nConnectCh = CInt(IIf("" = cmbParaConnectCh.Text.ToString(), -1, cmbParaConnectCh.Text.ToString.Replace("CH", "")))
        typParaVal.nConnectChOutPut = typParaVal.nConnectCh
        typParaVal.nFPGABoard = typParaVal.nLinkSw * NUM_CAMLINK_BOARD - NUM_CAMLINK_BOARD  ' FPGA0のFPGAボートNo

        ' 接続CHがFPGA1の場合
        If 0 <= typParaVal.nConnectChOutPut - FPGA1_CONNECTOR_MIN Then
            typParaVal.nConnectChOutPut = typParaVal.nConnectChOutPut - FPGA1_CONNECTOR_MIN + FPGA0_CONNECTOR_MIN
            typParaVal.nFPGABoard += NUM_CAMLINK_BOARD - 1
        End If

        Return typParaVal

    End Function

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal typParaVal(,) As PARA_VALUE) As Boolean
        Dim lstCheck As List(Of Integer) = New List(Of Integer)

        ' LINKSW内で重複した接続CHを設定していないか判定
        For ii As Integer = 0 To NUM_HANTEI_PARA - 1
            For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1

                ' 接続CH未選択
                If -1 = typParaVal(ii, jj).nConnectChOutPut Then
                    MsgBox(String.Format("ロジックパラランPC{0} 接続口{1}の接続CHを選択してください。", ii + 1, jj + 1), MsgBoxStyle.OkOnly, "入力チェック")
                    Return False
                End If

                ' KizuTask.ini CAMLINKSW_PCIDの出力値に変換して重複有無を判定
                Dim nLinkSwValue As Integer = CInt(String.Format("{0}{1}", typParaVal(ii, jj).nFPGABoard, typParaVal(ii, jj).nConnectChOutPut))

                ' 重複接続CH設定有
                If lstCheck.Contains(nLinkSwValue) Then
                    MsgBox(String.Format("LINKSW{0} 接続CHが重複しています。", typParaVal(ii, jj).nLinkSw), MsgBoxStyle.OkOnly, "入力チェック")
                    Return False
                End If

                lstCheck.Add(nLinkSwValue)
            Next
        Next

        Return True
    End Function

    ''' <summary>
    ''' INIファイルの更新
    ''' </summary>
    ''' <param name="typParaVal">画面設定値</param>
    ''' <remarks></remarks>
    Private Function INI_UpDate(ByVal typParaVal(,) As PARA_VALUE) As Boolean
        Dim bResult As Boolean = False
        Dim bIniExe As Boolean = False
        Dim bIniSet As Boolean = False

        Try
            '''' 統括PCに対してping確認
            If Not PingCheck(m_typTokatuIni.strToIpAddr) Then
                ' 統括PCにPingが通らない
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, "統括PCにPingが通りませんでした")
                Return False
            End If


            '''' 書き換えるiniファイルが存在しているか
            bIniExe = System.IO.File.Exists(m_typTokatuIni.strIniPath_Exe)
            bIniSet = System.IO.File.Exists(m_typTokatuIni.strIniPath_Set)
            If Not bIniExe OrElse Not bIniSet Then
                ' 指定されたiniファイルが存在しない
                Dim strMsg As String = ""
                strMsg = CStr(IIf(Not bIniSet, String.Format("[反映用：{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                strMsg &= CStr(IIf(Not bIniExe, String.Format("[EXE下：{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, "統括PCにiniファイルが存在しません " & strMsg)
                Return False
            End If


            '''' iniファイルのバックアップを作成
            bIniExe = CreateIniBackupFile(m_typTokatuIni.strIniPath_Exe)
            bIniSet = CreateIniBackupFile(m_typTokatuIni.strIniPath_Set)
            If Not bIniExe OrElse Not bIniSet Then
                ' 指定されたiniファイルのバックアップ作成に失敗
                Dim strMsg As String = ""
                strMsg = CStr(IIf(Not bIniSet, String.Format("[反映用：{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                strMsg &= CStr(IIf(Not bIniExe, String.Format("[EXE下：{0}]", m_typTokatuIni.strIniPath_Exe), ""))
                gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.ERR, "iniファイルのバックアップ作成に失敗しました " & strMsg)
                Return False
            End If


            '''' TO_DEFECT項目 画面設定値を反映
            For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                    ' EXE下のKizuTask.iniと配信用のKizuTask.iniを更新
                    WriteTO_DEFECT(m_typTokatuIni.strIniPath_Exe, typParaVal(ii, jj), jj + 1)
                    WriteTO_DEFECT(m_typTokatuIni.strIniPath_Set, typParaVal(ii, jj), jj + 1)
                Next
            Next


            '''' TO_LINKSW項目 現在設定値を初期値に設定
            For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                    ' 現在値未取得の場合、初期値設定処理は実施しない
                    If False = m_typNowPara(ii, jj).bGetFlg Then Continue For

                    ' EXE下のKizuTask.iniと配信用のKizuTask.iniを更新
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Exe, m_typNowPara(ii, jj), 0)
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Set, m_typNowPara(ii, jj), 0)
                Next
            Next


            '''' TO_LINKSW項目 画面設定値を反映
            Dim nKind As Integer = PARA_PC1_CAMERA_KIND
            For ii As Integer = 0 To NUM_HANTEI_PARA - 1
                ' パラランPC２を設定する場合
                If 1 = ii Then
                    nKind = PARA_PC2_CAMERA_KIND
                End If

                For jj As Integer = 0 To NUM_CAMLINK_BOARD - 1
                    ' EXE下のKizuTask.iniと配信用のKizuTask.iniを更新
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Exe, typParaVal(ii, jj), nKind)
                    WriteTO_LINKSW(m_typTokatuIni.strIniPath_Set, typParaVal(ii, jj), nKind)
                Next
            Next

            bResult = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "KizuTask.ini 更新エラー [" & ex.Message & "] " & Me.Text)
        End Try

        Return bResult
    End Function

    ''' <summary>
    ''' iniファイルのバックアップを作成する
    ''' </summary>
    ''' <param name="ini_filename">コピーするiniファイルのフルパス</param>
    ''' <returns>true:バックアップ成功 false;バックアップ失敗</returns>
    ''' <remarks>指定されたiniファイルと同一のフォルダに {元のファイル名}_bak.ini ファイルでコピーする</remarks>
    Private Function CreateIniBackupFile(ByVal ini_filename As String) As Boolean
        Dim bRet As Boolean = True

        Try
            Dim path As String = System.IO.Path.GetDirectoryName(ini_filename)
            Dim filename As String = System.IO.Path.GetFileNameWithoutExtension(ini_filename)
            Dim ext As String = System.IO.Path.GetExtension(ini_filename)
            Dim bak_filename As String = String.Format("{0}\{1}_bak{2}", path, filename, ext)

            System.IO.File.Copy(ini_filename, bak_filename, True)

        Catch ex As Exception
            bRet = False
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' INIファイル[TO_DEFECT]の設定を更新
    ''' </summary>
    ''' <param name="strWritePath">書込みファイルパス</param>
    ''' <param name="typParaVal">ロジックパラランPC設定値</param>
    ''' <param name="nBoard">接続先</param>
    ''' <remarks></remarks>
    Private Function WriteTO_DEFECT(ByVal strWritePath As String, ByVal typParaVal As PARA_VALUE, ByVal nBoard As Integer) As Boolean

        Dim bResult As Boolean = False
        Dim strItemName As String = String.Format("CAMLINKSW_PCID{0}_{1}", typParaVal.nPcId, nBoard)
        Dim nLinkSwValue As Integer = CInt(String.Format("{0}{1}", typParaVal.nFPGABoard, typParaVal.nConnectChOutPut))
        bResult = tClassLibrary.tMod.WriteIni(TO_DEFECT, strItemName, strWritePath, nLinkSwValue.ToString)

        Return bResult

    End Function

    ''' <summary>
    ''' INIファイル[TO_LINKSW]の設定を更新
    ''' </summary>
    ''' <param name="strWritePath">書込みファイルパス</param>
    ''' <param name="typParaVal">ロジックパラランPC設定値</param>
    ''' <param name="nKind">区分  初期化：0 / ロジックパラランPC1：2 / ロジックパラランPC2：3 </param>
    ''' <remarks></remarks>
    Private Function WriteTO_LINKSW(ByVal strWritePath As String, ByVal typParaVal As PARA_VALUE, ByVal nKind As Integer) As Boolean

        Dim bResult As Boolean = False
        Dim strItemName As String = String.Format("FPGA_{0}_BOARD_{1}", typParaVal.nFPGABoard, typParaVal.nConnectChOutPut)
        Dim strLinkSwValue As String = String.Format("0, {0}", nKind)
        bResult = tClassLibrary.tMod.WriteIni(TO_LINKSW, strItemName, strWritePath, strLinkSwValue)

        Return bResult

    End Function
#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '  イベントハンドラ関数
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "LINKSWコンボボックス関連イベント"
    ''' <summary>
    ''' LINKSW変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbLinkSw_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs)
        Dim cmbLinkSw As ComboBox = CType(sender, ComboBox)
        If cmbLinkSw1.Equals(cmbLinkSw) Then
            ' 同じLINKSWを再選択した場合は処理不要
            If m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC1) = cmbLinkSw1.SelectedIndex Then Exit Sub

            ' 接続CHをクリア
            cmbPara1ConnectCh1.Items.Clear()
            cmbPara1ConnectCh2.Items.Clear()

            ' 接続CHを再設定
            SetConnectComboBox(EM_PARAPC.PARA_PC1, cmbLinkSw1, cmbPara1ConnectCh1, cmbPara1ConnectCh2)

            ' 再設定時はコンボボックスには未選択を設定
            cmbPara1ConnectCh1.SelectedIndex = -1
            cmbPara1ConnectCh2.SelectedIndex = -1

            ' 選択LINKSWを取得
            m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC1) = cmbLinkSw1.SelectedIndex
        Else
            ' 同じLINKSWを再選択した場合は処理不要
            If m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC2) = cmbLinkSw2.SelectedIndex Then Exit Sub

            ' 接続CHをクリア
            cmbPara2ConnectCh1.Items.Clear()
            cmbPara2ConnectCh2.Items.Clear()

            ' 接続CHを再設定
            SetConnectComboBox(EM_PARAPC.PARA_PC2, cmbLinkSw2, cmbPara2ConnectCh1, cmbPara2ConnectCh2)

            ' 再設定時はコンボボックスには未選択を設定
            cmbPara2ConnectCh1.SelectedIndex = -1
            cmbPara2ConnectCh2.SelectedIndex = -1

            ' 選択LINKSWを取得
            m_nLinkSwSelectIdx(EM_PARAPC.PARA_PC2) = cmbLinkSw2.SelectedIndex
        End If

    End Sub
#End Region

#Region "キーイベント"
    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmCameraLink_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "カメラリンク設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLORへ切替処理
            Case 10
                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' 設定
            Case 11
                modMain.TaskEnd()               ' F12 終了
        End Select

    End Sub
#End Region

End Class
