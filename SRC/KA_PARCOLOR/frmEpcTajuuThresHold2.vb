''*********************************************************************************
'' ＥＰＣ多重分解能閾値設定画面クラス
''	[Ver]
''		Ver.01    2022/11/17  初版
''
''	[メモ]
''		
''*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcTajuuThresHold2
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const GAMEN_ID_NAME As String = GAMEN_ID_EPC_TAJUUTHRESHOLD
    Private Const MY_EM_FRM_ID As EM_FRM_ID = EM_FRM_ID.emfrmEpcTajuuThresHold
    Private Const MY_CH_PARAM_ID As Integer = CH_PARAM_TAJUUTHRESHOLD

    Private m_Adapter_Reduction As SqlClient.SqlDataAdapter = Nothing           ' アダプター 縮小率設定
    Private m_DS_Reduction As New DataSet                                       ' 非接続型テーブル 縮小率設定
    Private m_Adapter_Filter As SqlClient.SqlDataAdapter = Nothing              ' アダプター フィルタ設定
    Private m_DS_Filter As New DataSet                                          ' 非接続型テーブル フィルタ設定
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
    Private Sub frmEpcTajuuThresHold2_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

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
                Call HardCopy(IMG_PATH, "多重分解能閾値設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmEpcTajuuThresHold2_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' いろいろ開放処理

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
    Private Sub frm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

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
        ReDim dgv(3 - 1)

        dgv(0).dgv = dgvLabelImage
        dgv(0).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(0).wSize.initialize()

        dgv(1).dgv = dgvSimpleBin
        dgv(1).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(1).wSize.initialize()

        dgv(2).dgv = dgvFilterBin
        dgv(2).dgvType = EM_DATAGRIDVIEW_DISPTYPE.emColType
        dgv(2).wSize.initialize()
        '=======================================================
        Dim wk As TABLE_INFO
        MyBase.Clear_BaseDBList()

        wk.sTable = DB_PARAM_LABELIMAGE
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.id = b.id AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_LABELIMAGE, wk)

        wk.sTable = DB_PARAM_SIMPLEBIN
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.id_simplebin = b.id_simplebin AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_SIMPLEBIN, wk)

        wk.sTable = DB_PARAM_FILTERBIN
        wk.SelectSQLFormat = SELECT_KEY_ORDER
        wk.UpdateSQLFormat = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.id_filterbin = b.id_filterbin AND a.scode IN({5})"
        wk.bCopyDB = True
        MyBase.Add_BaseDBList(DB_PARAM_FILTERBIN, wk)
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
    Private Sub frmEpcTajuuThresHold2_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    Public Overrides Function Data_Disp(Optional ByVal Key As String = "") As Boolean
        Dim bRet As Boolean = MyBase.Data_Disp(Key)

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT id"
        strSQL &= ", name"
        strSQL &= " FROM " & DB_PARAM_REDUCTION
        strSQL &= " ORDER BY id ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Reduction, m_Adapter_Reduction, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
        End If

        ' SQL文生成 
        strSQL = "SELECT id"
        strSQL &= ", name"
        strSQL &= " FROM " & DB_PARAM_FILTER
        strSQL &= " ORDER BY id ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Filter, m_Adapter_Filter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
        End If

        Dim drowNewReduction As DataRow      ' 縮小率設定の新規行
        Dim drowNewFilter As DataRow         ' フィルタ設定の新規行

        Try
            drowNewReduction = m_DS_Reduction.Tables(0).NewRow()
            drowNewReduction.Item("id") = 0
            drowNewReduction.Item("Name") = "-"
            ' テーブルの一番上に挿入
            m_DS_Reduction.Tables(0).Rows.InsertAt(drowNewReduction, 0)

            drowNewFilter = m_DS_Filter.Tables(0).NewRow()
            drowNewFilter.Item("id") = 0
            drowNewFilter.Item("Name") = "-"
            ' テーブルの一番上に挿入
            m_DS_Filter.Tables(0).Rows.InsertAt(drowNewFilter, 0)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス用データ 設定エラー [" & ex.Message & "] " & Me.Text)
        End Try


        For ii As Integer = 0 To dgvFilterBin.RowCount - 1
            Using clm As New DataGridViewComboBoxCell
                clm.DataSource = m_DS_Filter.Tables(0)          ' コンボボックスで表示するデータソース
                clm.ValueMember = "id"                          ' コンボボックスの実際の値（データソースの列名）
                clm.DisplayMember = "name"                      ' コンボボックスで表示される値（データソースの列名）
                clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
                clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定

                dgvFilterBin(4, ii) = clm
            End Using
        Next

        For ii As Integer = 0 To dgvFilterBin.RowCount - 1
            Using clm As New DataGridViewComboBoxCell
                clm.DataSource = m_DS_Reduction.Tables(0)       ' コンボボックスで表示するデータソース
                clm.ValueMember = "id"                          ' コンボボックスの実際の値（データソースの列名）
                clm.DisplayMember = "name"                      ' コンボボックスで表示される値（データソースの列名）
                clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
                clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
                dgvFilterBin(3, ii) = clm
            End Using
        Next
        Return bRet
    End Function


End Class
