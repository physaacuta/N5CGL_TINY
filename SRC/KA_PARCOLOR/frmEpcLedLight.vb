'*********************************************************************************
' 照明設定画面クラス
'	[Ver]
'		Ver.01    2012/10/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcLedLight

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定数
	'/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum EM_LEDLIGHT_GIRD_COL_ID
        emTxt_Title = 0         ' テキストボックス  タイトル
        emTxt_M_rsw             ' テキストボックス  全体調光
        emTxt_Eq_data           ' テキストボックス  配光調整
    End Enum

    ' データグリッドビューの列o
    Private Enum EM_LEDLIGHT_GIRD_ROW_ID
        emTxt_Top1_In = 0       ' テキストボックス  表面1入力
        emTxt_Top1_LC           ' テキストボックス  表面1配光制御データ
        emTxt_Top2_In           ' テキストボックス  表面2入力
        emTxt_Top2_LC           ' テキストボックス  表面2配光制御データ
        emTxt_Top3_In           ' テキストボックス  表面3入力
        emTxt_Top3_LC           ' テキストボックス  表面3配光制御データ
        emTxt_Top4_In           ' テキストボックス  表面4入力
        emTxt_Top4_LC           ' テキストボックス  表面4配光制御データ
        emTxt_Top5_In           ' テキストボックス  表面5入力
        emTxt_Top5_LC           ' テキストボックス  表面5配光制御データ
        emTxt_Bot1_In           ' テキストボックス  裏面1入力
        emTxt_Bot1_LC           ' テキストボックス  裏面1配光制御データ
        emTxt_Bot2_In           ' テキストボックス  裏面2入力
        emTxt_Bot2_LC           ' テキストボックス  裏面2配光制御データ
        emTxt_Bot3_In           ' テキストボックス  裏面3入力
        emTxt_Bot3_LC           ' テキストボックス  裏面3配光制御データ
        emTxt_Bot4_In           ' テキストボックス  裏面4入力
        emTxt_Bot4_LC           ' テキストボックス  裏面4配光制御データ
        emTxt_Bot5_In           ' テキストボックス  裏面5入力
        emTxt_Bot5_LC           ' テキストボックス  裏面5配光制御データ


        em_END
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル
    Private m_KeyCode As String = ""                                ' 主キー

    'Private Const LED_ADJUST_CNT As Integer = 1
    Private Const LED_ADJUST_CNT As Integer = NUM_LED_BLOCK
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
	Private Sub frmLight_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
		If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 0
                ChangeCode(False)               ' F1 前項
            Case 1
                ChangeCode(True)                ' F2 次項
			Case 6								' F07 画面印刷
				Call PrintOut()
			Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "LED照明設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
				modMain.ChgProject()            ' F09 KA_PARSETへ切替処理
            Case 10
                ' subData_UpDate内でセルに値を設定するが、その際セルの値が変更されてしまうと
                ' セル変更イベントが呼ばれてしまう為、セル変更イベントを一旦削除
                dgvLight.Enabled = False
                RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' F11 設定

                ' セルデータ変更イベントを再登録
                AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
                dgvLight.Enabled = True
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
    Private Sub frmLight_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList

        ' いろいろ開放処理
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

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
    Private Sub frmEpcLedLight_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'm_bCellCheck = True
        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcLedLight           ' 照明設定画面

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        If NOA_OFFLINE = g_NOA Or g_bPassFlg = False Then
            btnTopReset.Enabled = False
            btnBotReset.Enabled = False
        End If

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' ここの設定
        dgvInitialize(dgvLight)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        ' コンボボックスに検査パターンをセット
        MyBase.SetComboBox_Scode(cmbSCode)

        If 0 < cmbSCode.Items.Count Then

            '前の画面で選択した状態をセット
            If cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '該当項目がない場合は、先頭を表示
                cmbSCode.SelectedIndex = 0
            Else
                cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        cmbSCode.DropDownStyle = ComboBoxStyle.DropDownList

        ' セル選択イベントにて実行される以下関数を実施しないとセルにデータが設定されない
        ' 検査パターンを表示
        Call ShowSelectData()

        ' フォームロード時にセル変更イベントが発生しないようにする為、セルへのデータ設定が完了後にイベントを登録
        ' イベント登録
        RemoveHandler cmbSCode.SelectedIndexChanged, AddressOf cmbSCode_SelectedIndexChanged
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        AddHandler cmbSCode.SelectedIndexChanged, AddressOf cmbSCode_SelectedIndexChanged
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcLedLight_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' このフォームで必要なデータを取得する DB読込
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = " SELECT scode"
        strSQL &= ", 照明照度_表"
        strSQL &= ", 照明照度2_表"
        strSQL &= ", 照明照度3_表"
        strSQL &= ", 照明照度4_表"
        strSQL &= ", 照明照度5_表"
        strSQL &= ", 照明照度_裏"
        strSQL &= ", 照明照度2_裏"
        strSQL &= ", 照明照度3_裏"
        strSQL &= ", 照明照度4_裏"
        strSQL &= ", 照明照度5_裏"
        strSQL &= ", 照明照度補正値_表"
        strSQL &= ", 照明照度補正値2_表"
        strSQL &= ", 照明照度補正値3_表"
        strSQL &= ", 照明照度補正値4_表"
        strSQL &= ", 照明照度補正値5_表"
        strSQL &= ", 照明照度補正値_裏"
        strSQL &= ", 照明照度補正値2_裏"
        strSQL &= ", 照明照度補正値3_裏"
        strSQL &= ", 照明照度補正値4_裏"
        strSQL &= ", 照明照度補正値5_裏"
        strSQL &= " FROM " & DB_PARAM_SCODE
        strSQL &= " ORDER BY scode ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_SCODE & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function


    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <remarks></remarks>
    Private Function Data_Disp(ByVal key As String) As Boolean

        ' 一括のテーブルなので、検査パターン毎の値のみ取り出し
        Dim strWhere As String = String.Format("scode='{0}'", key)          ' WHERE句に相当の条件生成
        Dim drowScode() As DataRow = m_DS.Tables(0).Select(strWhere)
        Dim bytData_Top1() As Byte                                          ' 照明照度補正値 表面1
        Dim bytData_Top2() As Byte                                          ' 照明照度補正値 表面2
        Dim bytData_Top3() As Byte                                          ' 照明照度補正値 表面3
        Dim bytData_Top4() As Byte                                          ' 照明照度補正値 表面4
        Dim bytData_Top5() As Byte                                          ' 照明照度補正値 表面5
        Dim bytData_Bot1() As Byte                                          ' 照明照度補正値 裏面1
        Dim bytData_Bot2() As Byte                                          ' 照明照度補正値 裏面2
        Dim bytData_Bot3() As Byte                                          ' 照明照度補正値 裏面3
        Dim bytData_Bot4() As Byte                                          ' 照明照度補正値 裏面4
        Dim bytData_Bot5() As Byte                                          ' 照明照度補正値 裏面5
        Dim nLcData As Integer                                              ' 配光制御データ
        Dim dResult As Double

        'データが登録されていない場合は強制終了させる
        If MAX_SCODE < drowScode.Length Or 0 >= drowScode.Length Then
            Data_Disp = False
            Exit Function
        End If

        ' 各行の書式設定
        dgvLight.RowCount = EM_LEDLIGHT_GIRD_ROW_ID.em_END

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = "表面 赤(入力)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = "表面 緑(入力:ＤＳ～Ｃ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = "表面 緑(入力:Ｃ～ＷＳ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = "表面 青(入力:ＤＳ～Ｃ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = "表面 青(入力:Ｃ～ＷＳ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = "　　 (参照：配光制御データ)"

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = "裏面 赤(入力)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = "裏面 緑(入力:ＤＳ～Ｃ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = "裏面 緑(入力:Ｃ～ＷＳ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = "裏面 青(入力:ＷＳ～Ｃ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = "　　 (参照：配光制御データ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = "裏面 青(入力:Ｃ～ＤＳ)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = "　　 (参照：配光制御データ)"

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Style.BackColor = Color.LightGray

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Style.BackColor = Color.LightGray

        For ii As Integer = 0 To LED_ADJUST_CNT - 1
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Style.BackColor = Color.LightGray

            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Style.BackColor = Color.LightGray
        Next ii

        '' 画面に表示
        ' 照明照度表示
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = CInt(drowScode(0).Item("照明照度_表"))     ' 照明照度_表
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = CInt(drowScode(0).Item("照明照度2_表"))    ' 照明照度_表
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = CInt(drowScode(0).Item("照明照度3_表"))    ' 照明照度_表
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = CInt(drowScode(0).Item("照明照度4_表"))    ' 照明照度_表
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = CInt(drowScode(0).Item("照明照度5_表"))    ' 照明照度_表

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = CInt(drowScode(0).Item("照明照度_裏"))     ' 照明照度_裏
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = CInt(drowScode(0).Item("照明照度2_裏"))    ' 照明照度_裏
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = CInt(drowScode(0).Item("照明照度3_裏"))    ' 照明照度_裏
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = CInt(drowScode(0).Item("照明照度4_裏"))    ' 照明照度_裏
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = CInt(drowScode(0).Item("照明照度5_裏"))    ' 照明照度_裏

        ' 照明照度補正値 Byte変換
        '表
        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値_表") Then
            bytData_Top1 = Nothing
        Else
            bytData_Top1 = CType(drowScode(0).Item("照明照度補正値_表"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値2_表") Then
            bytData_Top2 = Nothing
        Else
            bytData_Top2 = CType(drowScode(0).Item("照明照度補正値2_表"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値3_表") Then
            bytData_Top3 = Nothing
        Else
            bytData_Top3 = CType(drowScode(0).Item("照明照度補正値3_表"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値4_表") Then
            bytData_Top4 = Nothing
        Else
            bytData_Top4 = CType(drowScode(0).Item("照明照度補正値4_表"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値5_表") Then
            bytData_Top5 = Nothing
        Else
            bytData_Top5 = CType(drowScode(0).Item("照明照度補正値5_表"), Byte())
        End If

        '裏
        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値_裏") Then
            bytData_Bot1 = Nothing
        Else
            bytData_Bot1 = CType(drowScode(0).Item("照明照度補正値_裏"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値2_裏") Then
            bytData_Bot2 = Nothing
        Else
            bytData_Bot2 = CType(drowScode(0).Item("照明照度補正値2_裏"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値3_裏") Then
            bytData_Bot3 = Nothing
        Else
            bytData_Bot3 = CType(drowScode(0).Item("照明照度補正値3_裏"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値4_裏") Then
            bytData_Bot4 = Nothing
        Else
            bytData_Bot4 = CType(drowScode(0).Item("照明照度補正値4_裏"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("照明照度補正値5_裏") Then
            bytData_Bot5 = Nothing
        Else
            bytData_Bot5 = CType(drowScode(0).Item("照明照度補正値5_裏"), Byte())
        End If

        ' 照明照度補正値 配列指定
        ReDim Preserve bytData_Top1(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top2(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top3(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top4(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top5(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot1(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot2(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot3(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot4(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot5(LED_ADJUST_CNT - 1)

        ' 照明照度補正値 画面表示
        For ii As Integer = 0 To LED_ADJUST_CNT - 1
            '' 照明照度表面1
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = bytData_Top1(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)

            '' 照明照度表面2
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = bytData_Top2(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)

            '' 照明照度表面3
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = bytData_Top3(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)

            '' 照明照度表面4
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = bytData_Top4(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)

            '' 照明照度表面5
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = bytData_Top5(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)

            '' 照明照度裏面1
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = bytData_Bot1(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)

            '' 照明照度裏面2
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = bytData_Bot2(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)

            '' 照明照度裏面3
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = bytData_Bot3(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)

            '' 照明照度裏面4
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = bytData_Bot4(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)

            '' 照明照度裏面5
            ' 配光調整データ
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = bytData_Bot5(ii)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value))
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)

        Next ii

        Data_Disp = True

    End Function

    ''' <summary>
    ''' 検査パターン変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' ShowSelectData内でセルに値を設定するが、その際セルの値が変更されてしまうと
        ' セル変更イベントが呼ばれてしまう為、セル変更イベントを一旦削除
        dgvLight.Enabled = False
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        ShowSelectData()

        ' セルデータ変更イベントを再登録
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        dgvLight.Enabled = True

    End Sub

    ''' <summary>
    ''' 選択した検査パターンのデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ShowSelectData()
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので、元に戻した場合の対策)
        If m_KeyCode = cmbSCode.Text Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まない
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then
                ' 入力異常時は、選択位置も戻す
                cmbSCode.Text = m_KeyCode
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_Get(m_KeyCode)
        End If


        ' 選択変更に伴い ラベル値変更
        MyBase.SelectComboBoxItem(cmbSCode.SelectedIndex, cmbSCode, lblSCode)

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        Data_Disp(m_KeyCode)

        '今の選択状態を保存
        g_nScodeIndex = cmbSCode.SelectedIndex
    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 今の画面表示中の値と取り込み
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then Exit Sub

            Data_Get(m_KeyCode)
        End If

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String
        Dim blnNull As Boolean = False

        ' 照明照度_表1
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 表面1") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = strWk

        ' 照明照度_表2
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 表面2") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = strWk

        ' 照明照度_表3
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 表面3") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = strWk

        ' 照明照度_表4
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 表面4") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = strWk

        ' 照明照度_表5
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 表面5") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = strWk

        ' 照明照度_裏1
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 裏面1") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = strWk

        ' 照明照度_裏2
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 裏面2") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = strWk

        ' 照明照度_裏3
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 裏面3") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = strWk

        ' 照明照度_裏4
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 裏面4") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = strWk

        ' 照明照度_裏5
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "全体調光 裏面5") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = strWk

        For ii As Integer = 0 To LED_ADJUST_CNT - 1
            ' 照明照度補正値_表1
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 表面1") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = strWk

            ' 照明照度補正値_表2
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 表面2") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = strWk

            ' 照明照度補正値_表3
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 表面3") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = strWk

            ' 照明照度補正値_表4
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 表面4") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = strWk

            ' 照明照度補正値_表5
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 表面5") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = strWk

            ' 照明照度補正値_裏1
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 裏面1") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = strWk

            ' 照明照度補正値_裏2
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 裏面2") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = strWk

            ' 照明照度補正値_裏3
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 裏面3") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = strWk

            ' 照明照度補正値_裏4
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 裏面4") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = strWk

            ' 照明照度補正値_裏5
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "配光調整" & CStr(ii + 1) & " 裏面5") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = strWk

        Next ii

        Return True

    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String                                  ' WHERE句に相当の条件生成
        Dim drowScode() As DataRow
        Dim bytData_Top1(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 表面1
        Dim bytData_Top2(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 表面2
        Dim bytData_Top3(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 表面3
        Dim bytData_Top4(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 表面4
        Dim bytData_Top5(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 表面5
        Dim bytData_Bot1(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 裏面1
        Dim bytData_Bot2(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 裏面2
        Dim bytData_Bot3(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 裏面3
        Dim bytData_Bot4(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 裏面4
        Dim bytData_Bot5(LED_ADJUST_CNT - 1) As Byte            ' 照明照度補正値 裏面5
        Dim ii As Integer

        strWhere = String.Format("scode='{0}'", key)            ' WHERE句に相当の条件生成
        drowScode = m_DS.Tables(0).Select(strWhere)             ' 主キーを指定しての取り出しなので、一件しかない

        '' 念の為に件数チェック
        If MAX_SCODE < drowScode.Length Or 0 >= drowScode.Length Then
            Exit Sub
        End If

        drowScode(0).Item("照明照度_表") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)       ' 照明照度_表
        drowScode(0).Item("照明照度2_表") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)      ' 照明照度_表
        drowScode(0).Item("照明照度3_表") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)      ' 照明照度_表
        drowScode(0).Item("照明照度4_表") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)      ' 照明照度_表
        drowScode(0).Item("照明照度5_表") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)      ' 照明照度_表
        drowScode(0).Item("照明照度_裏") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)       ' 照明照度_裏
        drowScode(0).Item("照明照度2_裏") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)      ' 照明照度_裏
        drowScode(0).Item("照明照度3_裏") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)      ' 照明照度_裏
        drowScode(0).Item("照明照度4_裏") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)      ' 照明照度_裏
        drowScode(0).Item("照明照度5_裏") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)      ' 照明照度_裏
        For ii = 0 To LED_ADJUST_CNT - 1
            bytData_Top1(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)
            bytData_Top2(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)
            bytData_Top3(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)
            bytData_Top4(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)
            bytData_Top5(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)
            bytData_Bot1(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)
            bytData_Bot2(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)
            bytData_Bot3(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)
            bytData_Bot4(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)
            bytData_Bot5(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)
        Next ii
        drowScode(0).Item("照明照度補正値_表") = bytData_Top1
        drowScode(0).Item("照明照度補正値2_表") = bytData_Top2
        drowScode(0).Item("照明照度補正値3_表") = bytData_Top3
        drowScode(0).Item("照明照度補正値4_表") = bytData_Top4
        drowScode(0).Item("照明照度補正値5_表") = bytData_Top5
        drowScode(0).Item("照明照度補正値_裏") = bytData_Bot1
        drowScode(0).Item("照明照度補正値2_裏") = bytData_Bot2
        drowScode(0).Item("照明照度補正値3_裏") = bytData_Bot3
        drowScode(0).Item("照明照度補正値4_裏") = bytData_Bot4
        drowScode(0).Item("照明照度補正値5_裏") = bytData_Bot5

    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   'トランザクション処理

        Try
            ' DB接続
            m_Adapter.SelectCommand.Connection.Open()

            'トランザクション開始
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                'アダプタをセットして更新
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            'コミット
            transact.Commit()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_LIGHT, CH_SCODE_ALL)        'どのテーブルが変更されたかわからないのでALLで送信
            modMain.HardCopy(Me)

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)

            ' エラーならロールバックして終了
            If Not transact Is Nothing Then
                transact.Rollback()
            End If

            Return False
        Finally
            ' 後始末
            If m_Adapter.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter.SelectCommand.Connection.Close()
            End If

            If Not transact Is Nothing Then
                transact.Dispose()
            End If
        End Try
    End Function

    ''' <summary>
    ''' LED照明累計時間_表リセットボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTopReset_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnTopReset.Click

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' リセットしますか？
            frmMsg.Text = "確認"
            frmMsg.ChangeMsg("LED照明累計時間_表面が初期化されます。リセットしますか？", Color.Red.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_KT_LEDMGR_05
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' データ部
        que.data(0) = 0                         ' 表裏区分 (表:0   裏:1)
        que.data(1) = 0                         ' 変更検査表面状態No (検査表面状態No:1～  全部:0)

        KizuLibCLI.KizuMod.Send_Mail(KT_LEDMGR, "*", que)

    End Sub

    ''' <summary>
    ''' LED照明累計時間_裏リセットボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnBotReset_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnBotReset.Click

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' リセットしますか？
            frmMsg.Text = "確認"
            frmMsg.ChangeMsg("LED照明累計時間_裏面が初期化されます。リセットしますか？", Color.Red.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_KT_LEDMGR_05
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' データ部
        que.data(0) = 1                         ' 表裏区分 (表:0   裏:1)
        que.data(1) = 0                         ' 変更検査表面状態No (検査表面状態No:1～  全部:0)

        KizuLibCLI.KizuMod.Send_Mail(KT_LEDMGR, "*", que)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' dgv初期化
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' ヘッダーの高さ

        End With

        ' タイトル
        dgvColSetText(dgv, "", 240, True, True,
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' 全体調光
        dgvColSetText(dgv, "全体調光", 100, True, False,
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 配光調整
        For ii As Integer = 1 To LED_ADJUST_CNT
            dgvColSetText(dgv, "配光調整" & CStr(ii), 100, False, False,
                              DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        Next ii

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他こまごました操作 関連
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 前項 次項
    ''' </summary>
    ''' <param name="bln">前項:false 次項:true</param>
    ''' <remarks></remarks>
    Private Sub ChangeCode(ByVal bln As Boolean)
        Dim rec As Integer = CInt(IIf(bln, cmbSCode.SelectedIndex + 1, cmbSCode.SelectedIndex - 1))
        ' 上下限チェック
        If rec < 0 OrElse rec >= cmbSCode.Items.Count Then Exit Sub
        ' 切替
        cmbSCode.SelectedIndex = rec
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ アップコード 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 反映ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click

        '''' 条件チェック
        '元
        Dim strFromKey As String = ""                                    'コピー元キー 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '元
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        '先
        Dim strToKey As String = MyBase.GetUpCodeItem_To(False)          'コピー先キー
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '''' DBコピー
            If CopyDB(strFromKey, strToKey) Then
                '統括機能へパラメータ変更通知
                modMain.Send_Main(CH_PARAM_LIGHT, CH_SCODE_ALL)
                modMain.HardCopy(Me)

                frmMsg.Text = "反映処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "反映処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            '''' データの再取得
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '終了
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try


        ' Data_Disp内でセルに値を設定するが、その際セルの値が変更されてしまうと
        ' セル変更イベントが呼ばれてしまう為、セル変更イベントを一旦削除
        dgvLight.Enabled = False
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        '''' 画面再表示
        Data_Disp(m_KeyCode)

        ' セルデータ変更イベントを再登録
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        dgvLight.Enabled = True
    End Sub

    ''' <summary>
    ''' コピー処理
    ''' </summary>
    ''' <param name="strFromKey">コピー元キー </param>
    ''' <param name="strToKey">コピー先キー </param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function CopyDB(ByVal strFromKey As String, ByVal strToKey As String) As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        '表面状態別(ParamScode)を更新
        ' SQL文生成
        '''' SQL文の生成
        strSQL = "UPDATE " & DB_PARAM_SCODE & " SET "
        '可変列
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度2_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度3_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度4_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度5_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度2_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度3_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度4_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度5_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値2_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値3_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値4_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値5_表", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値2_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値3_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値4_裏", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("照明照度補正値5_裏", "b")
        tClass_SQLServer.sqlColumnTrim(strSQL)                  ' おまじない、必ず記述

        'From句
        strSQL &= " FROM " & DB_PARAM_SCODE & " a "
        strSQL &= ", (SELECT * FROM " & DB_PARAM_SCODE & " WHERE scode='" & strFromKey & "') b "
        'WHERE句
        strSQL &= " WHERE a.scode IN(" & strToKey & ")"

        'SQL文実行
        If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_SCODE & " 更新エラー [" & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' 設定調光値表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Get_LC_DATA(ByRef m_rsw As Integer, ByRef eq_data As Integer) As Integer
        Dim ans As Integer

        ans = CInt(Int(m_rsw / 2) + Int(m_rsw * eq_data / 512) + 1)

        'Dim dWK As Double
        ''設定調光値取得
        'dWK = (m_rsw / 2) + (m_rsw * eq_data / 512)
        ''小数点切捨て
        'If dWK < 0.5 Then
        '    dWK = 0
        'Else
        '    dWK = dWK - 0.5
        'End If
        ''少数点切捨て後表示
        'ans = CInt(dWK)

        Return ans

    End Function

    ''' <summary>
    ''' 配光制御データ表示（セルデータ変更時用）
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DISP_LC_DATA()
        Dim ii As Integer = 0
        Dim nMRsw As Integer                                               ' 全体調光データ
        Dim nEqData As Integer                                              ' 配光調整データ
        Dim nLcData As Integer                                              ' 配光制御データ
        Dim dResult As Double

        For ii = 0 To LED_ADJUST_CNT - 1
            '' 照明照度表面1
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = String.Format("Hex:{0:X}", nMRsw)


            '' 照明照度表面2
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度表面3
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度表面4
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度表面5
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = String.Format("Hex:{0:X}", nMRsw)


            '' 照明照度裏面1
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度裏面2
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度裏面3
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度裏面4
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' 照明照度裏面5
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value), nEqData)
            ' 配光制御データ演算
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' 配光制御データ表示
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = String.Format("Hex:{0:X}", nMRsw)


        Next ii
    End Sub

    ''' <summary>
    ''' セルデータ変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvLight_CellValueChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)

        ' DISP_LC_DATA内でセルに値を設定するが、その際セルの値が変更されてしまうと
        ' セル変更イベントが呼ばれてしまう為、セル変更イベントを一旦削除
        dgvLight.Enabled = False
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        DISP_LC_DATA()

        ' セルデータ変更イベントを再登録
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        dgvLight.Enabled = True

    End Sub

    '''' <summary>
    '''' 更新ボタンの有効/無効
    '''' </summary>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン
        pnlItem.Enabled = bPassResult                   ' コピー用リストの反映ボタン

    End Sub
End Class
