'*********************************************************************************
' エントリー
'	[Ver]
'		Ver.01    2012/05/08  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports System.Reflection

Public Module modMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum EM_FRM_ID
        emfrmEpcCommon = 0           '固定パラメータ設定
        emfrmEpcPara                 'ロジックパララン設定
        emfrmEpcCropShrink           '間引き設定
        emfrmEpcCamera               'カメラ設定
        emfrmEpcMixColor             '混色分離設定
        emfrmEpcEdge                 'エッジ検出設定
        emfrmEpcShading              'ｼｪｰﾃﾞｨﾝｸﾞ補正設定
        emfrmEpcThresHold            '閾値設定
        emfrmEpcDetect               '疵検出設定
        emfrmEpcCycle                '周期疵判定
        emfrmEpcMerge                '統合処理設定
        emfrmEpcLedLight             'LED照明設定
        emfrmEpcTajuuThresHold       '多重分解能閾値設定
        emfrmEpcReduction            '縮小率設定
        emfrmEpcFilter               'フィルタ設定
        emfrmEpcPcInfo               '上位情報内容設定
		emfrmEpcAttrName             '特徴量名称マスター設定
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		emfrmLongPd                  '長周期欠陥検出設定画面 
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
		emfrmGridSetting             'ﾃﾞｰﾀｸﾞﾘｯﾄﾞ表示設定
        emfrmLovItem                 '選択項目ｱｲﾃﾑﾏｽﾀｰ
    End Enum

    Public Enum EM_PASSWORD_CHECK
        emPASS_CHECK_NON_NG = -1     'パスワード確認なし、設定変更不可（デフォルト)
        emPASS_CHECK_NON_OK = 0      'パスワード確認なし、設定変更可能
        emPASS_CHECK_ARI = 1         'パスワード確認あり
    End Enum

    Public Enum EM_HARDCOPY_CHECK
        emHardCopy_Nasi = 0          'ハードコピーなし
        emHardCopy_Ari               'ハードコピーあり
    End Enum

    Public Enum EM_LOGICPARA_CHECK
        emLogicPara_Nasi = 0        'ロジックパララン設定なし
        emLogicPara_Ari             'ロジックパララン設定あり
    End Enum

    Public Enum EM_LABEL_KIND
        emONLINE = 0                'ラベル名：オンライン
        emPARA                      'ラベル名：ロジックパララン
    End Enum

    Public Const COL_WIDTH_CAMERA_ANGLE As Integer = 50          'カメラ角度のデータグリッド幅
    Public Const COL_WIDTH_CAMERA_SET As Integer = 50            'カメラセットのデータグリッド幅
    Public Const PARAM_COMMON_KEY As Integer = 0                 ' 共通パラメータテーブル KEY

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ グローバル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_nScodeIndex As Integer = 0         '表面状態のリストインデックスを保存
    Public g_nPaternIndex As Integer = 0        '検査パターンのリストインデックスを保存
    Public gcls_Log As tClassLibrary.tClass.tClass_LogManager           ' ログ管理クラス
    Public g_nDspfrmID As Integer = 0           '現在表示中の画面ID

    Public g_NOA As Integer                     'NOA接続フラグ (0:オンライン  1:遠隔)
    Public g_sPass As String = ""               'パスワード文字列 INIファイルから取得
    Public g_bPassFlg As Boolean                'パスワード入力結果 (True:OK False:NG)
    Public g_iPassUmu As Integer                'パスワード確認有無(0:無し(設定不可) 1:無し(設定可) 2:パスワード確認有)
    Public g_iHardCopyFlg As Integer            '設定時のハードコピー有無(0:無し 1:実施)
    Public g_iParaRun As Integer                '設定変更時メールスロット送信無し(0:送信有り 1:送信無し)
    Public g_nParaFlg As Integer                'ロジックパララン設定 (0:ロジックパララン設定不可 1:ロジックパララン設定可)

    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod               ' 総合クラス （別スレッドなので注意）

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_ap As ApplicationContext = Nothing                        ' コンテキスト

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' エントリーポイント
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ※KizuLibCLI 32bit / 64bit 切り替え用
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        ' 認証処理
        If Not tNinsho.IsUserCheck(KA_PARCOLOR) Then
            Exit Sub
        End If

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll を 確認して下さい。" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("タスク生成 失敗"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Sub
        End Try

        'パスワード確認
        If EM_PASSWORD_CHECK.emPASS_CHECK_ARI = g_iPassUmu Then
            'パスワード入力()
            If Windows.Forms.DialogResult.OK <> ConfPass() Then
                g_bPassFlg = False
            Else
                g_bPassFlg = True
            End If
        Else
            If EM_PASSWORD_CHECK.emPASS_CHECK_NON_OK = g_iPassUmu Then
                g_bPassFlg = True
            Else
                g_bPassFlg = False
            End If
        End If

        '固定パラメータ設定画面を表示
        Call ShowForm(EM_FRM_ID.emfrmEpcCommon)

        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThreadがコールされるまで ここでブロック

        Task_Last()

    End Sub

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()
        'ラインIDセット
        KizuLibCLI.KizuMod.SetLineID()

        mcls_Mod = New KizuLibCLI.KizuMod

        '' タスクイニシャル
        If 0 <> mcls_Mod.Task_Init(KA_PARCOLOR) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", KA_PARCOLOR))

            End
        End If

        'ログクラス生成
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KA_PARCOLOR, Nothing, Nothing)
        gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, "パラメータ画面起動")

        '設定ファイル読込
        Call GetIniFile()
    End Sub

    ''' <summary>
    ''' タスク終了処理時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_Last()
        gcls_Log.Dispose()
        gcls_Log = Nothing

        ' タスク終了
        mcls_Mod.Task_Exit()
    End Sub

    ''' <summary>
    ''' タスク終了処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()

        ' スレッドが生成されていなかったら終了
        If m_ap Is Nothing Then
            gcls_Log.Dispose()
            gcls_Log = Nothing
            End
        End If

        m_ap.ExitThread()                                       ' メインスレッド終了

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォームタイトル設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub

    ''' <summary>
    ''' メニューリスト追加
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal frm As frmMain_AutoParSet)

        frm.SetListMenu("固定パラメータ設定", EM_FRM_ID.emfrmEpcCommon)
        ' パララン用設定かつオンライン以外の場合、ロジックパララン設定可能
        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            frm.SetListMenu("ロジックパララン設定", EM_FRM_ID.emfrmEpcPara)
        End If
        frm.SetListMenu("間引き設定", EM_FRM_ID.emfrmEpcCropShrink)
        frm.SetListMenu("カメラ設定", EM_FRM_ID.emfrmEpcCamera)
        frm.SetListMenu("混色分離設定", EM_FRM_ID.emfrmEpcMixColor)
        frm.SetListMenu("エッジ検出設定", EM_FRM_ID.emfrmEpcEdge)
        frm.SetListMenu("ｼｪｰﾃﾞｨﾝｸﾞ補正設定", EM_FRM_ID.emfrmEpcShading)
        frm.SetListMenu("閾値設定", EM_FRM_ID.emfrmEpcThresHold)
        frm.SetListMenu("疵検出設定", EM_FRM_ID.emfrmEpcDetect)
        frm.SetListMenu("周期疵判定", EM_FRM_ID.emfrmEpcCycle)
        frm.SetListMenu("統合処理設定", EM_FRM_ID.emfrmEpcMerge)
        frm.SetListMenu("LED照明設定", EM_FRM_ID.emfrmEpcLedLight)
        frm.SetListMenu("多重分解能閾値設定", EM_FRM_ID.emfrmEpcTajuuThresHold)
        frm.SetListMenu("縮小率設定", EM_FRM_ID.emfrmEpcReduction)
        frm.SetListMenu("フィルタ設定", EM_FRM_ID.emfrmEpcFilter)
        frm.SetListMenu("上位情報内容設定", EM_FRM_ID.emfrmEpcPcInfo)
		frm.SetListMenu("特徴量名称ﾏｽﾀｰ設定", EM_FRM_ID.emfrmEpcAttrName)
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		frm.SetListMenu("長周期欠陥検出設定", EM_FRM_ID.emfrmLongPd)
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
		frm.SetListMenu("ﾃﾞｰﾀｸﾞﾘｯﾄﾞ表示設定", EM_FRM_ID.emfrmGridSetting)
        frm.SetListMenu("選択項目ｱｲﾃﾑﾏｽﾀｰ", EM_FRM_ID.emfrmLovItem)
    End Sub

    ''' <summary>
    ''' メニューリスト追加
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetListMenu(ByVal frm As KizuForm.frmMain_ParSet)

        frm.SetListMenu("固定パラメータ設定", EM_FRM_ID.emfrmEpcCommon)
        ' パララン用設定かつオンライン以外の場合、ロジックパララン設定可能
        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            frm.SetListMenu("ロジックパララン設定", EM_FRM_ID.emfrmEpcPara)
        End If
        frm.SetListMenu("間引き設定", EM_FRM_ID.emfrmEpcCropShrink)
        frm.SetListMenu("カメラ設定", EM_FRM_ID.emfrmEpcCamera)
        frm.SetListMenu("混色分離設定", EM_FRM_ID.emfrmEpcMixColor)
        frm.SetListMenu("エッジ検出設定", EM_FRM_ID.emfrmEpcEdge)
        frm.SetListMenu("ｼｪｰﾃﾞｨﾝｸﾞ補正設定", EM_FRM_ID.emfrmEpcShading)
        frm.SetListMenu("閾値設定", EM_FRM_ID.emfrmEpcThresHold)
        frm.SetListMenu("疵検出設定", EM_FRM_ID.emfrmEpcDetect)
        frm.SetListMenu("周期疵判定", EM_FRM_ID.emfrmEpcCycle)
        frm.SetListMenu("統合処理設定", EM_FRM_ID.emfrmEpcMerge)
        frm.SetListMenu("LED照明設定", EM_FRM_ID.emfrmEpcLedLight)
        frm.SetListMenu("多重分解能閾値設定", EM_FRM_ID.emfrmEpcTajuuThresHold)
        frm.SetListMenu("縮小率設定", EM_FRM_ID.emfrmEpcReduction)
        frm.SetListMenu("フィルタ設定", EM_FRM_ID.emfrmEpcFilter)
        frm.SetListMenu("上位情報内容設定", EM_FRM_ID.emfrmEpcPcInfo)
		frm.SetListMenu("特徴量名称ﾏｽﾀｰ設定", EM_FRM_ID.emfrmEpcAttrName)
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		frm.SetListMenu("長周期欠陥検出設定", EM_FRM_ID.emfrmLongPd)
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<---------------
		frm.SetListMenu("ﾃﾞｰﾀｸﾞﾘｯﾄﾞ表示設定", EM_FRM_ID.emfrmGridSetting)
        frm.SetListMenu("選択項目ｱｲﾃﾑﾏｽﾀｰ", EM_FRM_ID.emfrmLovItem)
    End Sub

    ''' <summary>
    ''' システム名ラベル設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_AutoParSet_SysName)

        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            ' パララン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emPARA)
        Else
            ' オンライン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emONLINE)
        End If
    End Sub

    ''' <summary>
    ''' システム名ラベル設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_AutoParSet_UpList_SysName)

        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            ' パララン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emPARA)
        Else
            ' オンライン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emONLINE)
        End If
    End Sub

    ''' <summary>
    ''' システム名ラベル設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_ParSet_SysName)

        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            ' パララン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emPARA)
        Else
            ' オンライン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emONLINE)
        End If
    End Sub

    ''' <summary>
    ''' システム名ラベル設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal frm As frmMain_ParSet_UplList_SysName)

        If EM_LOGICPARA_CHECK.emLogicPara_Ari = g_nParaFlg Then
            ' パララン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emPARA)
        Else
            ' オンライン表示
            frm.SetLabelSysName(EM_LABEL_KIND.emONLINE)
        End If
    End Sub

    ''' <summary>
    ''' 画面表示
    ''' </summary>
    ''' <param name="frmID"></param>
    ''' <remarks></remarks>
    Public Sub ShowForm(ByVal frmID As Integer)
        Select Case frmID
            Case EM_FRM_ID.emfrmEpcCommon         ' 固定パラメータ設定
                frmEpcCommon.Show()
            Case EM_FRM_ID.emfrmEpcPara           ' ロジックパララン設定
                frmEpcPara.Show()
            Case EM_FRM_ID.emfrmEpcCropShrink     ' 間引き設定
                frmEpcCropShrink.Show()
            Case EM_FRM_ID.emfrmEpcCamera         ' カメラ設定
                frmEpcCamera.Show()
            Case EM_FRM_ID.emfrmEpcMixColor       ' 混色分離設定
                frmEpcMixColor.Show()
            Case EM_FRM_ID.emfrmEpcEdge           ' エッジ検出設定
                frmEpcEdge.Show()
            Case EM_FRM_ID.emfrmEpcShading        ' ｼｪｰﾃﾞｨﾝｸﾞ補正設定
                frmEpcShading.Show()
            Case EM_FRM_ID.emfrmEpcThresHold      ' 閾値設定
                frmEpcThresHold.Show()
            Case EM_FRM_ID.emfrmEpcDetect         ' 疵検出設定
                frmEpcDetect.Show()
            Case EM_FRM_ID.emfrmEpcCycle          ' 周期疵判定
                frmEpcCycle.Show()
            Case EM_FRM_ID.emfrmEpcMerge          ' 統合処理設定
                frmEpcMerge.Show()
            Case EM_FRM_ID.emfrmEpcLedLight       ' LED照明設定
                frmEpcLedLight.Show()
            Case EM_FRM_ID.emfrmEpcTajuuThresHold ' 多重分解能閾値設定
                frmEpcTajuuThresHold2.Show()
            Case EM_FRM_ID.emfrmEpcReduction      ' 縮小率設定
                frmEpcReduction.Show()
            Case EM_FRM_ID.emfrmEpcFilter         ' フィルタ設定
                frmEpcFilter.Show()
            Case EM_FRM_ID.emfrmEpcPcInfo         ' 上位情報内容設定
                frmEpcPcInfo.Show()
            Case EM_FRM_ID.emfrmEpcAttrName       ' 特徴量名称マスター設定
				frmEpcAttrName.Show()

				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			Case EM_FRM_ID.emfrmLongPd
				frmLongPd.Show()                  ' 長周期欠陥検出設定
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
			Case EM_FRM_ID.emfrmGridSetting       ' 自動生成画面データグリッド表示設定
                frmGridSetting.Show()
            Case EM_FRM_ID.emfrmLovItem           ' 自動生成画面選択項目アイテムマスター
                frmLovItem.Show()
            Case Else
        End Select

        g_nDspfrmID = frmID
    End Sub

    ''' <summary>
    ''' パラメータ変更通知
    ''' </summary>
    ''' <param name="paramID">パラメータテーブルID</param>
    ''' <param name="scode">変更検査表面状態No (全部時=CH_SCODE_ALL)</param>
    ''' <remarks></remarks>
    Public Sub Send_Main(ByVal paramID As Integer, Optional ByVal scode As Integer = 0)

        ' 判定パララン機能時は、メールスロット送信無し
        If 1 = g_iParaRun Then Exit Sub

        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_01
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' データ部
        que.data(0) = 0                         ' 遅延実行キー  0固定
        que.data(1) = paramID                   ' パラメータテーブルID
        que.data(2) = scode                     ' 変更検査表面状態No (検査表面状態No:1～  全部:CH_SCODE_ALL  無し:0)

        If Not LOCAL Then
            '本番
            KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, KizuLibCLI.KizuMod.Tokatu(), que)
        Else
            'テスト
            KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, ".", que)
        End If

    End Sub

    ''' <summary>
    ''' パスワード確認
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function ConfPass() As DialogResult

        'パスワード確認
        Dim frmPass As New frmPassword(g_sPass, , , "パラメータ設定画面 パスワード入力")
        Dim bRet As DialogResult

        bRet = frmPass.ShowDialog

        Return bRet

    End Function

    ''' <summary>
    ''' INIファイル読込
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetIniFile()

        '' iniファイルより 定数的なグローバール変数読み込み
        g_NOA = tMod.tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)

		'パスワード確認実行有無取得
		g_iPassUmu = tMod.ReadIniInt(KA_PARSET, "PASSCHK_UMU", TASKINI_NAME, EM_PASSWORD_CHECK.emPASS_CHECK_NON_OK)

		'パスワード取得
		g_sPass = tMod.ReadIniStr(KA_PARSET, "PASSWORD", TASKINI_NAME, "", 256)

		'ハードコピー有無取得
        g_iHardCopyFlg = tMod.ReadIniInt(KA_PARSET, "HARDCOPY_UMU", TASKINI_NAME, EM_HARDCOPY_CHECK.emHardCopy_Nasi)

        'メールスロット送信有無取得
        g_iParaRun = tMod.ReadIniInt(KA_PARSET, "PARA_RUN", TASKINI_NAME, 0)

        'ロジックパララン設定有無取得
        g_nParaFlg = tMod.ReadIniInt(KA_PARSET, "LOGICPARA_UMU", TASKINI_NAME, EM_LOGICPARA_CHECK.emLogicPara_Nasi)

    End Sub


    ''' <summary>
    ''' DB保存時のハードコピーを残しておく
    ''' </summary>
    ''' <param name="myForm"></param>
    ''' <remarks></remarks>
    Public Sub HardCopy(ByVal myForm As Form)
        Dim strFPath As String
        Dim strFolder As String
        Dim strName As String

        'ハードコピーの有無チェック
        If EM_HARDCOPY_CHECK.emHardCopy_Ari = g_iHardCopyFlg Then
            strFolder = IMG_PATH
            strName = myForm.Text.Replace("[" & SYSTEM_TITLE & "]", "").Trim & "_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg"

            '''' フルパス
            strFPath = IO.Path.Combine(strFolder, strName)

            '''' フォルダの有無チェック(無かったら作ろう)
            tMod.CheckFolder(strFolder)

            '''' ハードコピー
            Dim cls_Hcopy As New tClass.tClass_HeadCopy
            Call cls_Hcopy.HCopy(myForm, strFPath, Drawing.Imaging.ImageFormat.Jpeg)

        End If
    End Sub

    ''' <summary>
    ''' データグリッドビューテキストボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                             ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                             ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                             Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' データグリッドビューチェックボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetCheck(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                              ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                              ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                              Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewCheckBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub
    ''' <summary>
    ''' データグリッドビューボタン列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="fontDefaultFont">列フォント</param>
    ''' <param name="bBtnTextValue">ボタン内文字表示・非表示</param>
    ''' <param name="sBtnTextName">ボタン内文字設定</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetButton(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                               ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                               ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                               ByVal fontDefaultFont As System.Drawing.Font, ByVal bBtnTextValue As Boolean, ByVal sBtnTextName As String, _
                               Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewButtonColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 列フォント
            clm.DefaultCellStyle.Font = fontDefaultFont
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' ボタン設定作成
            Dim BtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
            ' ボタン内文字表示・非表示
            BtnText.UseColumnTextForButtonValue = bBtnTextValue
            ' ボタン内文字設定
            BtnText.Text = sBtnTextName

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub


    ''' <summary>
    '''プロジェクト切替処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ChgProject()

        'Dim startInfo As New ProcessStartInfo("KA_PARSET.exe")
        'startInfo.WorkingDirectory = "../../../KA_PARSET/bin/Debug"
        'System.Diagnostics.Process.Start(startInfo)
		Try
			tClassLibrary.tClass.tClass_TaskCheck.StartExe(KA_PARSET)
			TaskEnd()
		Catch ex As Exception
		End Try
	End Sub

    ''' <summary>
    ''' カメラ名称セット
    ''' </summary>
    ''' <param name="cntCamName">コントロール</param>
    ''' <param name="nPtn">パターン</param>
    ''' <remarks></remarks>
    Public Sub SetCamAria(ByVal cntCamName As CamName, Optional ByVal nPtn As Integer = 0)
        Dim sCamAngle As New List(Of String)
        Dim sCamSet As New List(Of String)
        Dim sCamSetKo As New List(Of String)

        '''' 角度
        sCamAngle.Add("1:R画像")
        sCamAngle.Add("2:G画像")
        sCamAngle.Add("3:B画像")
        sCamAngle.Add("4:G-B画像")
        cntCamName.SetName(0, sCamAngle)

        '''' カメラセット
        sCamSet.Add(" 1:表DS  6:表W4")
        sCamSet.Add(" 2:表D1  7:表W3")
        sCamSet.Add(" 3:表D2  8:表W2")
        sCamSet.Add(" 4:表D3  9:表W1")
        sCamSet.Add(" 5:表D4 10:表WS")
        sCamSet.Add("")
        sCamSet.Add("11:裏DS 16:裏W4")
        sCamSet.Add("12:裏D1 17:裏W3")
        sCamSet.Add("13:裏D2 18:裏W2")
        sCamSet.Add("14:裏D3 19:裏W1")
        sCamSet.Add("15:裏D4 20:裏WS")
        cntCamName.SetName(1, sCamSet)

    End Sub

    ''' <summary>
    ''' KizuLibCLI.DLLの読込み
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit切り替え対応</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function
End Module
