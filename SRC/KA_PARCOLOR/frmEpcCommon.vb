'*******************************************************************************
' 固定パラメータ設定画面クラス
'   [Ver]
'       Ver.01    2013/07/08    初版
'
'	[メモ]
'		
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcCommon
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const GAMEN_ID_NAME As String = GAMEN_ID_EPC_COMMON
    Private Const MY_EM_FRM_ID As EM_FRM_ID = EM_FRM_ID.emfrmEpcCommon
    Private Const MY_CH_PARAM_ID As Integer = CH_PARAM_COMMON
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
    Private Sub frmFix_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        ' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        ' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "固定パラメータ設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmFix_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList

        ' いろいろ開放処理

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
    Private Sub frmEpcCommon_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        'タイトル設定
        Call SetTitleName(Me)

        'メニューリスト生成
        modMain.SetListMenu(Me)

        'システム名ラベル生成
        modMain.SetLabelSysName(Me)

        '自画面呼出
        MyBase.tProp_MyFrmID = MY_EM_FRM_ID
        'パラメータ変更区分セット
        MyBase.tProp_MyParamID = MY_CH_PARAM_ID

        ' カメラ名称セット
        Call modMain.SetCamAria(camName, 0)

        ' 更新ボタンの有効/無効
        MyBase.UpdateEnabled(g_bPassFlg)

        '=======================================================
        '対象データグリッドビュー設定
        Dim dgv() As DgvControlInfo
        ReDim dgv(2 - 1)
        dgv(0).dgv = dgvCommon
        dgv(0).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType
        dgv(0).wSize.nNo = 0
        dgv(0).wSize.nName = 284
        dgv(0).wSize.nValue = 150
        dgv(0).wSize.nRange = 150

        dgv(1).dgv = dgvCamSet
        dgv(1).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(1).wSize.initialize()

        '=======================================================
        '使用テーブル情報設定
        Dim wk As TABLE_INFO
        MyBase.Clear_BaseDBList()

        wk.sTable = DB_PARAM_COMMON
        wk.SelectSQLFormat = "SELECT [Key],{0} FROM {1} WHERE [Key]=" & PARAM_COMMON_KEY
        wk.UpdateSQLFormat = ""
        wk.bCopyDB = False
        MyBase.Add_BaseDBList(DB_PARAM_COMMON, wk)

        wk.sTable = DB_PARAM_CAMSET
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = ""
        wk.bCopyDB = False
        MyBase.Add_BaseDBList(DB_PARAM_CAMSET, wk)

        '=======================================================


        'データグリッド設定情報をDBから取得
        MyBase.InitGridSetting(GAMEN_ID_NAME, dgv)

        'ロード処理
        MyBase.InitLoading()


    End Sub

    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcCommon_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

End Class