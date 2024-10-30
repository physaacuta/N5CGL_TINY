'*********************************************************************************
' ＥＰＣエッジ検出設定画面クラス
'	[Ver]
'		Ver.01    2013/07/21  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcEdge
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const GAMEN_ID_NAME As String = GAMEN_ID_EPC_EDGE
    Private Const MY_EM_FRM_ID As EM_FRM_ID = EM_FRM_ID.emfrmEpcEdge
    Private Const MY_CH_PARAM_ID As Integer = CH_PARAM_EDGE
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
    Private Sub frmEpcEdge_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 0
                btnF01.Select()                 ' F01コンボボックス変更中にファンクションが押された時の対策
                ChangeCode(False)               ' 前項
            Case 1
                btnF02.Select()                 ' F02コンボボックス変更中にファンクションが押された時の対策
                ChangeCode(True)                ' 次項
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "エッジ検出設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmEpcEdge_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
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
    Private Sub frmEpcEdge_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

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

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' 更新ボタンの有効/無効
        MyBase.UpdateEnabled(g_bPassFlg)

        '=======================================================
        '対象データグリッドビュー設定
        Dim dgv() As DgvControlInfo
		'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
		'ReDim dgv(4 - 1)
		ReDim dgv(5 - 1)
		'// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------		
        dgv(0).dgv = dgvCamset
        dgv(0).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(0).wSize.initialize()

        dgv(1).dgv = dgvCamSetScode
        dgv(1).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(1).wSize.initialize()

        dgv(2).dgv = dgvCamsetAngleScode
        dgv(2).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(2).wSize.initialize()

		'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
		'dgv(3).dgv = dgvPScode
		'dgv(3).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
		'dgv(3).wSize.nNo = 0
		'dgv(3).wSize.nName = 230
		'dgv(3).wSize.nValue = 300
		'dgv(3).wSize.nRange = 200

        dgv(3).dgv = dgvPScode
		dgv(3).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emRowType
        dgv(3).wSize.nNo = 0
        dgv(3).wSize.nName = 230
		dgv(3).wSize.nValue = 150
		dgv(3).wSize.nRange = 150

		dgv(4).dgv = dgvCamAngleScode
		dgv(4).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
		dgv(4).wSize.initialize()
		'// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------

        '=======================================================
        '使用テーブル情報設定
        Dim wk As TABLE_INFO
        MyBase.Clear_BaseDBList()

        wk.sTable = DB_PARAM_CAMSET
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = ""
        wk.bCopyDB = False
        MyBase.Add_BaseDBList(DB_PARAM_CAMSET, wk)

        wk.sTable = DB_PARAM_CAMSET_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camset = b.camset AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_CAMSET_SCODE, wk)

        wk.sTable = DB_PARAM_CAMSET_ANGLE_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle And a.camset = b.camset AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_CAMSET_ANGLE_SCODE, wk)

        wk.sTable = DB_PARAM_SCODE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_SCODE, wk)

		'// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
		wk.sTable = DB_PARAM_CAMANGLE_SCODE
		wk.SelectSQLFormat = SELECT_KEY_ORDER
		wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle AND a.scode IN({5})"
		wk.bCopyDB = True
		MyBase.Add_BaseDBList(DB_PARAM_CAMANGLE_SCODE, wk)
		'// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------

        '=======================================================

        'データグリッド設定情報をDBから取得
        MyBase.InitGridSetting(GAMEN_ID_NAME, dgv)

        '検査表面状態Noコンボボックス情報初期セット
        MyBase.Set_cmbSCode_Item(cmbSCode, lblSCode)

        'ロード処理
        MyBase.InitLoading()
    End Sub
    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcEdge_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

 End Class
