'*********************************************************************************
' 多重分解能閾値設定画面クラス
'	[Ver]
'		Ver.01    2017/01/10  初版
'
'	[メモ]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcTajuuThresHold

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum EM_DATACOLUMN
        emTxt_No = 0                                        ' テキストボックス　条件No
        emCmb_ReductionId                                   ' コンボボックス 縮小率ID
        emCmb_FilterId                                      ' コンボボックス フィルタID
        emTxt_UpperThreshold                                ' テキストボックス　上閾値
        emTxt_LowerThreshold                                ' テキストボックス　下閾値
        emTxt_UpperLimit                                    ' テキストボックス　上リミッタ
        emTxt_LowerLimit                                    ' テキストボックス　下リミッタ
        emTxt_EdgeDeadband                                  ' テキストボックス　不感帯幅
        emTxt_name                                          ' テキストボックス　名称
        iInsButton                      ' 挿入ボタン
        iDelButton                      ' 削除ボタン
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル
    Private m_Adapter_Reduction As SqlClient.SqlDataAdapter = Nothing           ' アダプター 縮小率設定
    Private m_DS_Reduction As New DataSet                                       ' 非接続型テーブル 縮小率設定
    Private m_Adapter_Filter As SqlClient.SqlDataAdapter = Nothing              ' アダプター フィルタ設定
    Private m_DS_Filter As New DataSet                                          ' 非接続型テーブル フィルタ設定
    Private m_KeyCode As String = ""                                ' 主キー
    Private m_KeyAngle As Integer = 1                               ' 角度

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
    Private Sub frmEpcTajuuThresHold_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
    Private Sub frmEpcTajuuThresHold_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If
        m_DS_Reduction.Dispose()
        If Not m_Adapter_Reduction Is Nothing Then
            m_Adapter_Reduction.Dispose()
            m_Adapter_Reduction = Nothing
        End If
        m_DS_Filter.Dispose()
        If Not m_Adapter_Filter Is Nothing Then
            m_Adapter_Filter.Dispose()
            m_Adapter_Filter = Nothing
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
    ''' <remarks></remarks>
    Private Sub frmEpcTajuuThresHold_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcTajuuThresHold     ' 多重分解能閾値設定 画面呼出

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Scode(cmbUpCodeItem)

        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' カメラ名称セット
        Call modMain.SetCamAria(camName, 0)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       ' 終了
        End If

        ' DGV初期化
        dgvInitialize(dgvData)

        ' コンボボックスにパターンをセット
        MyBase.SetComboBox_Scode(cmbSCode)

        ' コンボボックスにカメラ角度を設定
        SetComboBox_Angle(cmbAngle)

        If 0 < cmbSCode.Items.Count Then
            ' 前の画面で選択した状態をセット
            If cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                ' 該当項目がない場合は、先頭を表示
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

            modMain.TaskEnd()                       ' 終了
        End If

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        m_KeyAngle = CInt(cmbAngle.Text)
        Data_Disp(m_KeyCode)

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)

        If dgvData.RowCount > 0 Then
            ' カーソルスタート位置は入力エリアの先頭
            dgvData(1, 0).Selected = True
        End If

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

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
    Private Sub frmEpcTajuuThresHold_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        ' 現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' DBからとってきたコンボボックス用の値を表示用に加工
    ''' </summary>
    Private Sub Data_Init()
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

    End Sub

    ''' <summary>
    ''' このフォームで必要なデータを取得する
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT scode"
        strSQL &= ",camangle"
        strSQL &= ",id"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",UpperThreshold"
        strSQL &= ",LowerThreshold"
        strSQL &= ",UpperLimit"
        strSQL &= ",LowerLimit"
        strSQL &= ",name"
        strSQL &= ",EdgeDeadband"

        strSQL &= " FROM " & DB_PARAM_THRESHOLD
        strSQL &= " ORDER BY id ASC"

        ' SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成
        strSQL = "SELECT id"
        strSQL &= ", name"
        strSQL &= " FROM " & DB_PARAM_REDUCTION
        strSQL &= " ORDER BY id ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Reduction, m_Adapter_Reduction, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成 
        strSQL = "SELECT id"
        strSQL &= ", name"
        strSQL &= " FROM " & DB_PARAM_FILTER
        strSQL &= " ORDER BY id ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Filter, m_Adapter_Filter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' コンボボックスの表示データを初期化
        Data_Init()

        Return True

    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        'インデックスとボタンの表示設定
        InitIndexdgv(dgvData)
        ' パターン毎の値のみ取り出し
        Dim strWhere As String = String.Format("scode='{0}' and camangle='{1}'", key, CInt(cmbAngle.Text))          ' WHERE句に相当の条件生成
        Dim drow() As DataRow = m_DS.Tables(0).Select(strWhere)

        ' 画面に表示
        dgvData.RowCount = MAX_TAJUUTHRESHOLD
        For ii As Integer = 0 To dgvData.RowCount - 1
            dgvData(EM_DATACOLUMN.emTxt_No, ii).Value = ii + 1
            If ii < drow.Length Then
                dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value = drow(ii).Item("FilterId")
                dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value = drow(ii).Item("ReductionId")
                dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value = drow(ii).Item("UpperLimit")
                dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value = drow(ii).Item("LowerLimit")
                dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value = drow(ii).Item("EdgeDeadband")
                dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value = drow(ii).Item("UpperThreshold")
                dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value = drow(ii).Item("LowerThreshold")
                dgvData(EM_DATACOLUMN.emTxt_name, ii).Value = drow(ii).Item("name")
            Else
                dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value = 0
                dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value = 0
                dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_name, ii).Value = ""
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' 検査表面状態変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbSCode.SelectedIndexChanged
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので 元に戻したよ の時の対策)
        If m_KeyCode = cmbSCode.Text Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まないよ
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

        '現在の状態を保存
        g_nScodeIndex = cmbSCode.SelectedIndex

    End Sub

    ''' <summary>
    ''' 角度変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbAngle_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbAngle.SelectedIndexChanged
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので 元に戻したよ の時の対策)
        If m_KeyAngle = CInt(cmbAngle.Text) Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まないよ
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then
                ' 入力異常時は、選択位置も戻す
                cmbAngle.Text = CStr(m_KeyAngle)
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_Get(m_KeyCode)
        End If

        ' 画面にパターン別の値を表示
        m_KeyAngle = CInt(cmbAngle.Text)
        Data_Disp(m_KeyCode)

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
                '表示更新
                Data_Disp(m_KeyCode)
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
    ''' <param name="key">主キーの値(scode) 今の所使わないかなー</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String
        Dim strMsg As String    'エラーメッセージ
        ' 条件No分ループ
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value) And _
               0 = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_name, ii).Value) Then
                '登録なし
            Else
                '登録有り
                If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value) Then
                    '縮小率ID指定無し
                    strMsg = "【 多重分解能閾値設定 】" & vbNewLine
                    strMsg &= "縮小率IDを設定してください。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1)
                    MsgBox(strMsg)
                    Return False
                End If
                If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value) Then
                    'フィルタID指定無し
                    strMsg = "【 多重分解能閾値設定 】" & vbNewLine
                    strMsg &= "フィルタIDを設定してください。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1)
                    MsgBox(strMsg)
                    Return False
                End If
                ' 上閾値
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " 上閾値") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value = strWk
                ' 下閾値
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " 下閾値") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value = strWk
                ' 上リミッタ
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " 上リミッタ") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value = strWk
                ' 下リミッタ
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " 下リミッタ") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value = strWk
                ' 不感帯幅
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value)
                If 0 <> tClass_ImputCheck.CheckImputData_Int(strWk, 0, 512, False) Then
                    strMsg = "【 No=" & ii + 1 & " 不感帯幅 】" & vbNewLine
                    strMsg &= "適切な値ではありません。" & vbNewLine
                    strMsg &= "入力範囲：「0 ～ 512」"
                    MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
                    Return False
                End If
                dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value = strWk
                ' 名称
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_name, ii).Value)
                If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 256, "No=" & ii + 1 & " 名称") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_name, ii).Value = strWk
            End If
        Next ii
        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String = String.Format("scode='{0}' and camangle='{1}'", key, m_KeyAngle)          ' WHERE句に相当の条件生成
        Dim drow() As DataRow                                        ' データ行
        Dim nRowCount As Integer                                     ' 削除対象のデータ数
        Dim nId As Integer = 0
        drow = m_DS.Tables(0).Select(strWhere)

        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DS.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii
        ' 条件No分ループ
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            Dim drowNew As DataRow = m_DS.Tables(0).NewRow()                                      ' 新規行

            If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value) And _
               0 = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_name, ii).Value) Then
                '登録無し
            Else
                '登録有り
                'IDをインクリメント
                nId = nId + 1
                drowNew.Item("scode") = key
                drowNew.Item("camangle") = m_KeyAngle
                drowNew.Item("id") = nId
                drowNew.Item("FilterId") = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value)
                drowNew.Item("ReductionId") = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value)
                drowNew.Item("UpperLimit") = CDbl(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value)
                drowNew.Item("LowerLimit") = CDbl(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value)
                drowNew.Item("EdgeDeadband") = CInt(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value)
                drowNew.Item("UpperThreshold") = CDbl(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value)
                drowNew.Item("LowerThreshold") = CDbl(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value)
                drowNew.Item("name") = dgvData(EM_DATACOLUMN.emTxt_name, ii).Value
                ' テーブルに追加
                m_DS.Tables(0).Rows.InsertAt(drowNew, m_DS.Tables(0).Rows.Count)
            End If

        Next ii
    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql文

        Try
            '''' データベースオープン
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            ' デリートインサートするので全項目を削除
            strSql = "DELETE FROM " & DB_PARAM_THRESHOLD
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To m_DS.Tables(0).Rows.Count - 1
                With m_DS.Tables(0).Rows(ii)
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_THRESHOLD & " VALUES ('"
                    strSql &= .Item("scode").ToString & "',"            ' scode
                    strSql &= .Item("camangle").ToString & ","          ' 角度
                    strSql &= .Item("id").ToString & ","                ' ID
                    strSql &= .Item("ReductionId").ToString & ","       ' 縮小率ID
                    strSql &= .Item("FilterId").ToString & ","          ' フィルタID
                    strSql &= .Item("UpperThreshold").ToString & ","    ' 上閾値
                    strSql &= .Item("LowerThreshold").ToString & ","    ' 下閾値
                    strSql &= .Item("UpperLimit").ToString & ","        ' 上リミッタ
                    strSql &= .Item("LowerLimit").ToString & ","        ' 下リミッタ
                    strSql &= "'" & .Item("name").ToString & "',"       ' 名称
                    strSql &= .Item("EdgeDeadband").ToString & " )"     ' 不感帯幅

                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                    'End If
                End With
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_TAJUUTHRESHOLD)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' 後始末
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' 選択色をバックカラーに
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' 選択色も一行おきに色変え

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42             ' ヘッダーの高さ

        End With

        '' 列定義

        ' No
        dgvColSetText(dgv, "NO", 60, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 縮小率ID
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "縮小率ID"
            clm.Width = 120
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.ReadOnly = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DS_Reduction.Tables(0)            ' コンボボックスで表示するデータソース
            clm.ValueMember = "id"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' フィルタID
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "フィルタID"
            clm.Width = 120
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.ReadOnly = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DS_Filter.Tables(0)            ' コンボボックスで表示するデータソース
            clm.ValueMember = "id"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' 上閾値
        dgvColSetText(dgv, "上閾値" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 下閾値
        dgvColSetText(dgv, "下閾値" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 上リミッタ
        dgvColSetText(dgv, "上リミッタ" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 下リミッタ
        dgvColSetText(dgv, "下リミッタ" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 不感帯幅
        dgvColSetText(dgv, "不感帯幅" & vbNewLine & "( 0 - 512 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 名称
        dgvColSetText(dgv, "名称", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)


        ' 上へ挿入
        dgvColSetButton(dgv, "上へ" & vbNewLine & "挿入", 50, False, False, _
                        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                        New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "挿入")

        ' 削除
        dgvColSetButton(dgv, "削除", 50, False, False, _
                        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                        New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "削除")

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
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing
        Dim strSQL As String = ""

        '''' 条件チェック
        ' 元
        Dim strFromKey As String = ""                                    ' コピー元キー 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '元
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        ' 先
        Dim strToKey As String = MyBase.GetUpCodeItem_To(False)          ' コピー先キー
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '''' アップデート 可変行のためUPDATE文は使えないので、デリートインサートしている
            If DB_UPDATE_UpCodeItem(strFromKey, strToKey) Then

                ' 統括機能へパラメータ変更通知
                modMain.Send_Main(CH_PARAM_TAJUUTHRESHOLD)
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
                modMain.TaskEnd()                       ' 終了
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        '''' 画面再表示
        Data_Disp(m_KeyCode)

    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)
        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン
        pnlItem.Enabled = bPassResult                   ' コピー用リストの反映ボタン
    End Sub
    ''' <summary>
    ''' コンボボックスにカメラ角度を登録
    ''' </summary>
    ''' <param name="cmb"></param>
    ''' <remarks></remarks>
    Private Sub SetComboBox_Angle(ByVal cmb As ComboBox)
        cmb.Items.Add("1")
        cmb.Items.Add("2")
        cmb.Items.Add("3")
        cmb.Items.Add("4")
        '初期選択をR画像とする
        cmb.Text = "1"
    End Sub

    ''' <summary>
    ''' dgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_A_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_DATACOLUMN.iInsButton   '上へ挿入ボタン
                InsBtnClick_Type(dgv, e.RowIndex)
            Case EM_DATACOLUMN.iDelButton   '削除ボタン
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' dgvの挿入ボタン押されたときの処理
    ''' </summary>
    ''' <param name="dgv">対象のデータグリッドビュー</param>
    ''' <param name="nRowIndex">レコードを挿入する位置</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        ' 挿入ボタンが使用可能かチェック
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' 最終行は使用不可なので抜ける
            Exit Sub
        End If

        ' 最終行にデータが入っている場合は挿入を行わない
        If 0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, dgv.Rows.Count - 1).Value) Or _
           0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_name, dgv.Rows.Count - 1).Value) Then
            MsgBox("最終行にデータが存在します。")
            Exit Sub
        End If

        '挿入するかメッセージを出す
        If MsgBox("挿入しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "条件確認") = MsgBoxResult.No Then Exit Sub

        ' dgvにレコードを挿入
        InsertAt_TypeData(nRowIndex)

        ' dgvの最終行を削除
        dgv.Rows.RemoveAt(dgv.Rows.Count - 1)

        'Noを振りなおしボタンを表示しなおす
        InitIndexdgv(dgv)
    End Sub

    ''' <summary>
    ''' dgvの削除ボタンが押されたときの処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        '削除するかメッセージを出す
        If MsgBox("削除しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "条件確認") = MsgBoxResult.No Then Exit Sub

        ' dgvの最終行にレコードを挿入
        InsertAt_TypeData(dgv.Rows.Count)

        ' dgvのレコードを削除
        dgv.Rows.RemoveAt(nRowIndex)

        'Noを振りなおしボタンを表示しなおす
        InitIndexdgv(dgv)
    End Sub

    ''' <summary>
    ''' dgvにデータ行を挿入
    ''' </summary>
    ''' <param name="nRowIndex">行の挿入位置</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvにレコードを挿入
        dgvData.Rows.Insert(nRowIndex)

        ' データ初期化
        dgvData(EM_DATACOLUMN.emCmb_FilterId, nRowIndex).Value = 0
        dgvData(EM_DATACOLUMN.emCmb_ReductionId, nRowIndex).Value = 0
        dgvData(EM_DATACOLUMN.emTxt_UpperLimit, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_LowerLimit, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_name, nRowIndex).Value = ""

    End Sub

    ''' <summary>
    ''' dgvのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexdgv(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMN.emTxt_No, ii).Value = ii + 1   ' 列のインデックス番号

            ' 挿入ボタンの使用可能状態を設定    (ボタンを使用不可にはできないので、色を変更）
            If ii = dgv.RowCount - 1 Then
                '最終行は挿入ボタンが使用不可
                dgv(EM_DATACOLUMN.iInsButton, ii).Style.ForeColor = Color.Gray
            Else
                'それ以外は使用可
                dgv(EM_DATACOLUMN.iInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next
    End Sub

    ''' <summary>
    ''' コピー元のscodeからコピー先のscodeへデータをデリートインサートする
    ''' </summary>
    ''' <param name="strFromkey">コピー元キー</param>
    ''' <param name="strToKey">コピー先のキーが入った文字列</param>
    ''' <returns>正常/異常</returns>
    ''' <remarks></remarks>
    Private Function DB_UPDATE_UpCodeItem(ByVal strFromkey As String, ByVal strToKey As String) As Boolean

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String = ""
        Dim strScode() As String                                ' コピー先のscode
        Dim strErrMsg As String = ""

        'カンマ区切りのscodeを配列に変換
        strScode = Split(strToKey, ",")

        'コピー元のデータを取得
        strSQL = "SELECT scode"
        strSQL &= ",camangle"
        strSQL &= ",id"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",UpperThreshold"
        strSQL &= ",LowerThreshold"
        strSQL &= ",UpperLimit"
        strSQL &= ",LowerLimit"
        strSQL &= ",name"
        strSQL &= ",EdgeDeadband"
        strSQL &= " FROM " & DB_PARAM_THRESHOLD
        strSQL &= " WHERE scode='" & strFromkey & "'"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Try

            '''' デリートインサート
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            ' レコードの削除
            Dim strSqlDel As String     ' レコード削除用sql
            strSqlDel = "DELETE FROM " & DB_PARAM_THRESHOLD & " WHERE scode IN(" & strToKey & ")"

            If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False

            ' レコードの挿入 コピー先scodeの個数だけ繰り返し
            For ii As Integer = 0 To strScode.Length - 1
                ' コピー元scodeのレコード数だけ繰り返し挿入
                For kk As Integer = 0 To m_DS.Tables(0).Rows.Count - 1
                    With m_DS.Tables(0).Rows(kk)
                        'DB_PARAM_THRESHOLD登録SQL文作成
                        strSQL = ""
                        strSQL &= "INSERT INTO " & DB_PARAM_THRESHOLD & " VALUES ("
                        strSQL &= strScode(ii) & ","                        ' scode
                        strSQL &= .Item("camangle").ToString & ","          ' 角度
                        strSQL &= .Item("id").ToString & ","                ' ID
                        strSQL &= .Item("ReductionId").ToString & ","       ' 縮小率ID
                        strSQL &= .Item("FilterId").ToString & ","          ' フィルタID
                        strSQL &= .Item("UpperThreshold").ToString & ","    ' 上閾値
                        strSQL &= .Item("LowerThreshold").ToString & ","    ' 下閾値
                        strSQL &= .Item("UpperLimit").ToString & ","        ' 上リミッタ
                        strSQL &= .Item("LowerLimit").ToString & ","        ' 下リミッタ
                        strSQL &= "'" & .Item("name").ToString & "',"       ' 名称
                        strSQL &= .Item("EdgeDeadband").ToString & " )"     ' 不感帯幅
                        'SQL実行
                        If 0 > tcls_DB.DMLExecute(strSQL) Then
                            ' エラーならロールバックして終了
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End With
                Next kk
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            Return True
        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " データコピーエラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function

End Class
