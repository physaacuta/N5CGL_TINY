'*********************************************************************************
' 検査パターン別出力設定画面クラス
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Public Class frmPCodeOutput
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum EM_DATACOLUMN
        emTxt_No = 0               ' テキストボックス  No
        emCmb_Name                 ' コンボボックス    疵種
        emCmb_Tansyo_Top           ' コンボボックス    探傷画面表示_表
        emCmb_Tansyo_Bot           ' コンボボックス　  探傷画面表示_裏
        emCmb_DefMap               ' コンボボックス    疵点マップ画面表示
        emCmb_DB_Ipan_Top          ' コンボボックス    DB保存_一般_表
        emCmb_DB_Ipan_Bot          ' コンボボックス    DB保存_一般_裏
        emCmb_DB_Yusen_Top         ' コンボボックス    DB保存_優先_表
        emCmb_DB_Yusen_Bot         ' コンボボックス    DB保存_優先_裏
        emCmb_FF                   ' コンボボックス    FF送信有無
        emCmb_KizuKei              ' コンボボックス    疵接近軽
        emCmb_KizuZyu              ' コンボボックス    疵接近重
        emCmb_PatLight             ' コンボボックス    パトライト出力
        emCmb_VoiceRen             ' コンボボックス    ボイス連続疵
        emCmb_VoiceZyu             ' コンボボックス    ボイス重大疵
        emCmb_VoiceHege            ' コンボボックス    ボイスヘゲ
        emCmb_VoiceSuriba          ' コンボボックス    ボイススリバ
        emCmb_VoiceTopBot          ' コンボボックス    ボイス表裏同一疵
        emCmb_VoiceSyuki           ' コンボボックス    ボイス周期疵
        emCmb_VoiceEtc             ' コンボボックス    ボイスその他疵
    End Enum

    Private m_AdapterPCode As SqlClient.SqlDataAdapter = Nothing        ' アダプター 検査パターン別出力設定
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing         ' アダプター 疵種マスター
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing        ' アダプター グレードマスター

    Private m_DsPCode As New DataSet                                    ' 非接続型テーブル 検査パターン別出力設定データ
    Private m_DsType As New DataSet                                     ' 非接続形テーブル 疵種マスター コンボボックス用
    Private m_DsGrade As New DataSet                                    ' 非接続型テーブル グレードマスター コンボボックス用
    Private m_KeyCode As String = ""                                    ' 主キー

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
    Private Sub frmSetPat_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
                Call HardCopy(IMG_PATH, "検査パターン別出力設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLORへ切替処理
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
    Private Sub frmSetPat_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' 開放処理
        m_DsPCode.Dispose()
        m_DsType.Dispose()
        m_DsGrade.Dispose()

        If Not m_AdapterPCode Is Nothing Then
            m_AdapterPCode.Dispose()
            m_AdapterPCode = Nothing
        End If

        If Not m_AdapterType Is Nothing Then
            m_AdapterType.Dispose()
            m_AdapterType = Nothing
        End If

        If Not m_AdapterGrade Is Nothing Then
            m_AdapterGrade.Dispose()
            m_AdapterGrade = Nothing
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
    Private Sub frmPCodeOutput_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                 ' メニューリスト生成
        modMain.SetLabelSysName(Me)                             ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCodeOutput       ' 検査パターン別出力設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Pcode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' このフォームで必要なデータを取得する コンボボックスの値もとってくるので、dgv設定より先
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        ' コンボボックスの表示データを初期化
        Data_Init()

        ' dgvの設定
        dgvInitialize(dgvData, True)

        dgvInitialize(dgvEtc, False)

        ' コンボボックスに検査パターンをセット
        MyBase.SetComboBox_Pcode(cmbPCode)

        If 0 < cmbPCode.Items.Count Then
            '前の画面で選択した状態をセット
            If cmbPCode.Items.Count - 1 < g_nPaternIndex Then
                '該当項目がない場合は、先頭を表示
                cmbPCode.SelectedIndex = 0
            Else
                cmbPCode.SelectedIndex = g_nPaternIndex
            End If
        Else
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
        Data_Disp(m_KeyCode)

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)
        dgvEtc.DataGridViewSelectionRowColorChange(g_ColorSelection)

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
    Private Sub frmPCodeOutput_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' このフォームで必要なデータを取得する
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""


        strSQL = "SELECT pcode"
        strSQL &= ", 疵種EdasysID"
        strSQL &= ", 探傷画面表示_表"
        strSQL &= ", 探傷画面表示_裏"
        strSQL &= ", 入側単位疵点マップ画面表示"
        strSQL &= ", FF送信有無"
        strSQL &= ", 疵接近軽"
        strSQL &= ", 疵接近重"
        strSQL &= ", パトライト出力"
        strSQL &= ", ボイス連続疵"
        strSQL &= ", ボイス重大疵"
        strSQL &= ", ボイスヘゲ"
        strSQL &= ", ボイススリバ"
        strSQL &= ", ボイス表裏同一疵"
        strSQL &= ", ボイス周期疵"
        strSQL &= ", ボイスその他疵"
        strSQL &= ", DB保存_一般_表"
        strSQL &= ", DB保存_一般_裏"
        strSQL &= ", DB保存_優先_表"
        strSQL &= ", DB保存_優先_裏"

        strSQL &= " FROM " & DB_PCODE_OUTPUT
        strSQL &= " AS a LEFT OUTER JOIN " & DB_TYPE_NAME
        strSQL &= " AS b ON a.疵種EdasysID = b.EdasysID"
        strSQL &= " ORDER by b.ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPCode, m_AdapterPCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成 疵種はIDで昇順
        strSQL = "SELECT EdasysID"
        strSQL &= ", Name"
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成 グレードは優先度で昇順
        strSQL = "SELECT EdasysID"
        strSQL &= ", Name"
        strSQL &= ", マップ表示色"
        strSQL &= ", 優先度"
        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY 優先度 ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DBからとってきたコンボボックス用の値を表示用に加工
    ''' </summary>
    ''' <remarks>グレード名称に以上をつける、無しの行を追加、</remarks>
    Private Sub Data_Init()
        Dim drowNewType As DataRow      ' 疵種マスターの新規行
        Dim drowNewGrade As DataRow     ' 疵種グレードの新規行

        Try
            '疵種非接続テーブルの設定
            ' 疵種の新規行を設定 EdasysID、疵名称 
            drowNewType = m_DsType.Tables(0).NewRow()
            drowNewType.Item("EdasysID") = 0
            drowNewType.Item("Name") = "無し"
            ' テーブルの一番上に挿入
            m_DsType.Tables(0).Rows.InsertAt(drowNewType, 0)

            'グレード非接続テーブルの設定
            ' 全行ループ グレードの名称に以上をつける
            For ii As Integer = 0 To m_DsGrade.Tables(0).Rows.Count - 1
                m_DsGrade.Tables(0).Rows(ii).Item("Name") = m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString & "以上"
            Next ii
            ' グレードの新規行を設定 EdasysID、グレード名称、マップ表示色
            drowNewGrade = m_DsGrade.Tables(0).NewRow()
            drowNewGrade.Item("EdasysID") = 0
            drowNewGrade.Item("Name") = "-"
            drowNewGrade.Item("マップ表示色") = -1      ' マップ表示色、使わないけど一応設定
            ' テーブルの一番上に挿入
            m_DsGrade.Tables(0).Rows.InsertAt(drowNewGrade, 0)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス用データ 設定エラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub
    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <param name="key">主キーの値(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        ' 一括のテーブルなので、検査パターン毎の値のみ取り出し
        Dim strWhere As String = String.Format("pcode='{0}' and 疵種EdasysID>=0", key)          ' WHERE句に相当の条件生成 0～999の疵種EdasysIDが検出されるはず
        Dim strWhereEtc As String = String.Format("pcode='{0}' and 疵種EdasysID=-1", key)       ' WHERE句に相当の条件生成 -1の疵種EdasysIDが検出されるはず
        Dim drow() As DataRow = m_DsPCode.Tables(0).Select(strWhere)                            ' EdasysIDが1以上のデータを    0～64行
        Dim drowEtc() As DataRow = m_DsPCode.Tables(0).Select(strWhereEtc)                      ' EdasysIDが-1のその他データを取ってくる   0～1行
        Dim nRowCount As Integer = 0                                                            ' 行数カウント用

        Try
            ' 画面に表示
            dgvData.RowCount = MAX_TYPE             ' 固定64行

            ' 64行に データをセット
            For ii As Integer = 0 To dgvData.RowCount - 1
                dgvData(EM_DATACOLUMN.emTxt_No, ii).Value = ii + 1                              ' No
                If ii < drow.Length Then
                    ' 検査パターン別出力設定テーブルをpcodeで上から検索する (selectを使うとソートされるので)
                    For kk As Integer = nRowCount To m_DsPCode.Tables(0).Rows.Count - 1
                        ' pcodeがkeyの値で、疵種EdasysIDが-1以外の行を取得し、行数カウントをインクリメント                    
                        If m_DsPCode.Tables(0).Rows(kk).Item("pcode").ToString = key AndAlso _
                            CInt(m_DsPCode.Tables(0).Rows(kk).Item("疵種EdasysID").ToString) >= 0 Then
                            dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("疵種EdasysID")
                            dgvData(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("探傷画面表示_表")
                            dgvData(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("探傷画面表示_裏")
                            dgvData(EM_DATACOLUMN.emCmb_DefMap, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("入側単位疵点マップ画面表示")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB保存_一般_表")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB保存_一般_裏")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB保存_優先_表")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB保存_優先_裏")
                            dgvData(EM_DATACOLUMN.emCmb_FF, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("FF送信有無")
                            dgvData(EM_DATACOLUMN.emCmb_KizuKei, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("疵接近軽")
                            dgvData(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("疵接近重")
                            dgvData(EM_DATACOLUMN.emCmb_PatLight, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("パトライト出力")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイス連続疵")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイス重大疵")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイスヘゲ")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイススリバ")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイス表裏同一疵")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイス周期疵")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("ボイスその他疵")

                            nRowCount = kk + 1
                            Exit For
                        End If
                    Next kk
                Else
                    ' データ初期化
                    dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DefMap, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_FF, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_KizuKei, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_PatLight, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value = 0
                End If
            Next ii

            'その他dgvの設定
            dgvEtc.RowCount = 1                                                                             ' 1行固定
            dgvEtc(EM_DATACOLUMN.emTxt_No, 0).Value = ""                                                    ' No 空白
            dgvEtc(EM_DATACOLUMN.emCmb_Name, 0).Value = "上記以外"                                          ' 疵種

            ' その他行の表示データがあるか
            If drowEtc.Length > 0 Then
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Top, 0).Value = drowEtc(0).Item("探傷画面表示_表")
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Bot, 0).Value = drowEtc(0).Item("探傷画面表示_裏")
                dgvEtc(EM_DATACOLUMN.emCmb_DefMap, 0).Value = drowEtc(0).Item("入側単位疵点マップ画面表示")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, 0).Value = drowEtc(0).Item("DB保存_一般_表")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, 0).Value = drowEtc(0).Item("DB保存_一般_裏")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, 0).Value = drowEtc(0).Item("DB保存_優先_表")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, 0).Value = drowEtc(0).Item("DB保存_優先_裏")
                dgvEtc(EM_DATACOLUMN.emCmb_FF, 0).Value = drowEtc(0).Item("FF送信有無")
                dgvEtc(EM_DATACOLUMN.emCmb_KizuKei, 0).Value = drowEtc(0).Item("疵接近軽")
                dgvEtc(EM_DATACOLUMN.emCmb_KizuZyu, 0).Value = drowEtc(0).Item("疵接近重")
                dgvEtc(EM_DATACOLUMN.emCmb_PatLight, 0).Value = drowEtc(0).Item("パトライト出力")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceRen, 0).Value = drowEtc(0).Item("ボイス連続疵")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceZyu, 0).Value = drowEtc(0).Item("ボイス重大疵")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceHege, 0).Value = drowEtc(0).Item("ボイスヘゲ")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSuriba, 0).Value = drowEtc(0).Item("ボイススリバ")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceTopBot, 0).Value = drowEtc(0).Item("ボイス表裏同一疵")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSyuki, 0).Value = drowEtc(0).Item("ボイス周期疵")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceEtc, 0).Value = drowEtc(0).Item("ボイスその他疵")

            Else
                ' 表示データが無い行は初期化する
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Top, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Bot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DefMap, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_FF, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_KizuKei, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_KizuZyu, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_PatLight, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceRen, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceZyu, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceHege, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSuriba, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceTopBot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSyuki, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceEtc, 0).Value = 0
            End If

            ' カーソルスタート位置は入力エリアの先頭
            dgvData(1, 0).Selected = True
            dgvEtc(1, 0).Selected = True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 設定エラー [" & ex.Message & "] " & Me.Text)
            Return
        End Try
    End Sub

    ''' <summary>
    ''' 検査パターン変更時
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbPCode.SelectedIndexChanged
        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので 元に戻した時の対策)
        If m_KeyCode = cmbPCode.Text Then Exit Sub

        '' 画面のデータを非接続型テーブルにセット
        ' 初期画面表示時には、データ無しの為、画面のデータを取り込まない
        If m_KeyCode <> "" Then
            ' 入力値チェック
            If Not Data_Check(m_KeyCode) Then
                ' 入力異常時は、選択位置も戻す
                cmbPCode.Text = m_KeyCode
                Exit Sub
            End If

            ' 変更前のキーに対し 画面データ取り込み
            Data_Get(m_KeyCode)
        End If

        ' 選択変更に伴い ラベル値変更
        MyBase.SelectComboBoxItem(cmbPCode.SelectedIndex, cmbPCode, lblPCode)

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
        Data_Disp(m_KeyCode)

        '現在の状態を保存
        g_nPaternIndex = cmbPCode.SelectedIndex

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

            ' データの再取得
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Black.ToArgb, True, False)
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

        ' 表示データを初期化、無し行を追加したり
        Data_Init()

        ' 再表示
        Data_Disp(m_KeyCode)
    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <param name="key">主キーの値(pcode) 今の所使わないかなー</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strMsg As String                    ' エラーメッセージ用
        Dim nCheckGr(0 To 1) As Integer         ' Gr設定値チェック用

        ' 行数分ループ コンボボックスしかないので、疵種EdasysIDの重複チェックのみ行う
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            ' 疵種が無しの行は無視
            If CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) = 0 Then Continue For

            ' 疵種の重複チェック
            For jj As Integer = ii + 1 To dgvData.Rows.Count - 1
                ' 比較先が無しの行は無視
                If CInt(dgvData(EM_DATACOLUMN.emCmb_Name, jj).Value) = 0 Then Continue For

                ' 疵種が同じならメッセージを出して終了
                If CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) = CInt(dgvData(EM_DATACOLUMN.emCmb_Name, jj).Value) Then
                    strMsg = "【 疵種 】" & vbNewLine
                    strMsg &= "同じ疵種は登録できません。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                    MsgBox(strMsg)
                    Return False
                End If
            Next jj

        Next ii

        For ii As Integer = 0 To dgvData.Rows.Count - 1

            ' データをセット
            If 0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) Then               ' 疵種

                nCheckGr(0) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value)         ' DB保存_一般_表
                nCheckGr(1) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value)        ' DB保存_優先_表

                If False = Data_Range_Check(nCheckGr) Then
                    If nCheckGr(0) <= 0 Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " のグレード設定が正しくありません。" & vbNewLine
                        strMsg &= "[DB保存_優先_表]を設定するには、[DB保存_一般_表]の設定が必要です"
                        MsgBox(strMsg, , "入力チェック")
                        Return False
                    Else
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " のグレード設定が正しくありません。" & vbNewLine
                        strMsg &= "[DB保存_一般_表] <= [DB保存_優先_表] となるように設定して下さい。"
                        MsgBox(strMsg, , "入力チェック")
                        Return False
                    End If
                End If
            End If

            ' データをセット
            If 0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) Then               ' 疵種

                nCheckGr(0) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value)         ' DB保存_一般_裏
                nCheckGr(1) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value)        ' DB保存_優先_裏

                If False = Data_Range_Check(nCheckGr) Then
                    If nCheckGr(0) <= 0 Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " のグレード設定が正しくありません。" & vbNewLine
                        strMsg &= "[DB保存_優先_裏]を設定するには、[DB保存_一般_裏]の設定が必要です。"
                        MsgBox(strMsg, , "入力チェック")
                        Return False
                    Else
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " のグレード設定が正しくありません。" & vbNewLine
                        strMsg &= "[DB保存_一般_裏] <= [DB保存_優先_裏] となるように設定して下さい。"
                        MsgBox(strMsg, , "入力チェック")
                        Return False
                    End If
                End If
            End If

        Next ii

        '' 上記以外のチェック
        ' DB保存_一般_表 <= DB保存_優先_表のチェック
        nCheckGr(0) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, 0).Value)         ' DB保存_一般_表
        nCheckGr(1) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, 0).Value)        ' DB保存_優先_表

        If False = Data_Range_Check(nCheckGr) Then
            If nCheckGr(0) <= 0 Then
                strMsg = "【 グレード設定 】" & vbNewLine
                strMsg &= "上記以外 のグレード設定が正しくありません。" & vbNewLine
                strMsg &= "[DB保存_優先_表]を設定するには、[DB保存_一般_表]の設定が必要です"
                MsgBox(strMsg, , "入力チェック")
                Return False
            Else
                strMsg = "【 グレード設定 】" & vbNewLine
                strMsg &= "上記以外 のグレード設定が正しくありません。" & vbNewLine
                strMsg &= "[DB保存_一般_表] <= [DB保存_優先_表] となるように設定して下さい。"
                MsgBox(strMsg, , "入力チェック")
                Return False
            End If
        End If

        ' DB保存_一般_裏 <= DB保存_優先_裏のチェック
        nCheckGr(0) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, 0).Value)         ' DB保存_一般_裏
        nCheckGr(1) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, 0).Value)        ' DB保存_優先_裏

        If False = Data_Range_Check(nCheckGr) Then
            If False = Data_Range_Check(nCheckGr) Then
                If nCheckGr(0) <= 0 Then
                    strMsg = "【 グレード設定 】" & vbNewLine
                    strMsg &= "上記以外 のグレード設定が正しくありません。" & vbNewLine
                    strMsg &= "[DB保存_優先_裏]を設定するには、[DB保存_一般_裏]の設定が必要です。"
                    MsgBox(strMsg, , "入力チェック")
                    Return False
                Else
                    strMsg = "【 グレード設定 】" & vbNewLine
                    strMsg &= "上記以外 のグレード設定が正しくありません。" & vbNewLine
                    strMsg &= "[DB保存_一般_裏] <= [DB保存_優先_裏] となるように設定して下さい。"
                    MsgBox(strMsg, , "入力チェック")
                    Return False
                End If
            End If
        End If

        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String = String.Format("pcode='{0}'", key)   ' WHERE句に相当の条件生成
        Dim drow() As DataRow                                        ' データ行
        Dim nRowCount As Integer                                     ' 削除対象のデータ数
        drow = m_DsPCode.Tables(0).Select(strWhere)                  ' pcodeが主キーの値の行を取り出す

        '現在画面に表示しているpcodeの非接続テーブルのデータを削除(後ろから削除)
        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DsPCode.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii

        ' 今表示しているデータを全て格納
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            Dim drowNew As DataRow = m_DsPCode.Tables(0).NewRow()                                      ' 新規行
            ' データをセット   
            drowNew.Item("pcode") = key                                                                ' pcode
            drowNew.Item("疵種EdasysID") = dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value                 ' 疵種
            drowNew.Item("探傷画面表示_表") = dgvData(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value        ' 探傷画面表示_表
            drowNew.Item("探傷画面表示_裏") = dgvData(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value        ' 探傷画面表示_裏
            drowNew.Item("入側単位疵点マップ画面表示") = dgvData(EM_DATACOLUMN.emCmb_DefMap, ii).Value ' 疵点マップ画面表示 
            drowNew.Item("DB保存_一般_表") = dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value        ' DB保存_一般_表
            drowNew.Item("DB保存_一般_裏") = dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value        ' DB保存_一般_裏
            drowNew.Item("DB保存_優先_表") = dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value       ' DB保存_優先_表
            drowNew.Item("DB保存_優先_裏") = dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value       ' DB保存_優先_裏
            drowNew.Item("FF送信有無") = dgvData(EM_DATACOLUMN.emCmb_FF, ii).Value                     ' FF送信有無
            drowNew.Item("疵接近軽") = dgvData(EM_DATACOLUMN.emCmb_KizuKei, ii).Value                  ' 疵接近軽
            drowNew.Item("疵接近重") = dgvData(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value                  ' 疵接近重
            drowNew.Item("パトライト出力") = dgvData(EM_DATACOLUMN.emCmb_PatLight, ii).Value           ' パトライト出力
            drowNew.Item("ボイス連続疵") = dgvData(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value             ' ボイス連続疵
            drowNew.Item("ボイス重大疵") = dgvData(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value             ' ボイス重大疵
            drowNew.Item("ボイスヘゲ") = dgvData(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value              ' ボイスヘゲ
            drowNew.Item("ボイススリバ") = dgvData(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value          ' ボイススリバ
            drowNew.Item("ボイス表裏同一疵") = dgvData(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value      ' ボイス表裏同一疵
            drowNew.Item("ボイス周期疵") = dgvData(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value           ' ボイス周期疵
            drowNew.Item("ボイスその他疵") = dgvData(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value           ' ボイスその他疵

            ' テーブルに追加
            m_DsPCode.Tables(0).Rows.InsertAt(drowNew, m_DsPCode.Tables(0).Rows.Count)
        Next ii

        ' その他dgvのデータを全て格納 1行しかないはず
        For ii As Integer = 0 To dgvEtc.Rows.Count - 1
            Dim drowNew As DataRow = m_DsPCode.Tables(0).NewRow()                                      ' 新規行
            ' データをセット
            drowNew.Item("pcode") = key                                                                ' pcode
            drowNew.Item("疵種EdasysID") = -1                                                          ' 疵種
            drowNew.Item("探傷画面表示_表") = dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value         ' 探傷画面表示_表
            drowNew.Item("探傷画面表示_裏") = dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value         ' 探傷画面表示_裏
            drowNew.Item("入側単位疵点マップ画面表示") = dgvEtc(EM_DATACOLUMN.emCmb_DefMap, ii).Value  ' 疵点マップ画面表示 
            drowNew.Item("DB保存_一般_表") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value         ' DB保存_一般_表
            drowNew.Item("DB保存_一般_裏") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value         ' DB保存_一般_裏
            drowNew.Item("DB保存_優先_表") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value        ' DB保存_優先_表
            drowNew.Item("DB保存_優先_裏") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value        ' DB保存_優先_裏
            drowNew.Item("FF送信有無") = dgvEtc(EM_DATACOLUMN.emCmb_FF, ii).Value                      ' FF送信有無
            drowNew.Item("疵接近軽") = dgvEtc(EM_DATACOLUMN.emCmb_KizuKei, ii).Value                   ' 疵接近軽
            drowNew.Item("疵接近重") = dgvEtc(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value                   ' 疵接近重
            drowNew.Item("パトライト出力") = dgvEtc(EM_DATACOLUMN.emCmb_PatLight, ii).Value            ' パトライト出力
            drowNew.Item("ボイス連続疵") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value              ' ボイス連続疵
            drowNew.Item("ボイス重大疵") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value              ' ボイス重大疵
            drowNew.Item("ボイスヘゲ") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value               ' ボイスヘゲ
            drowNew.Item("ボイススリバ") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value           ' ボイススリバ
            drowNew.Item("ボイス表裏同一疵") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value       ' ボイス表裏同一疵
            drowNew.Item("ボイス周期疵") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value            ' ボイス周期疵
            drowNew.Item("ボイスその他疵") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value            ' ボイスその他疵

            ' テーブルに追加
            m_DsPCode.Tables(0).Rows.InsertAt(drowNew, m_DsPCode.Tables(0).Rows.Count)
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

            ' デリートインサートするので検査パターン出力設定テーブルの全項目を削除
            strSql = "DELETE FROM " & DB_PCODE_OUTPUT
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            '検査パターン出力設定テーブルへデータを登録 
            'データテーブルは複数テーブルを参照している為、アップデートできないので、インサート文で
            For ii As Integer = 0 To m_DsPCode.Tables(0).Rows.Count - 1
                With m_DsPCode.Tables(0).Rows(ii)
                    If CInt(.Item("疵種EdasysID").ToString) <> 0 Then
                        'TYPEOUT登録SQL文作成
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_PCODE_OUTPUT & " VALUES ('"
                        strSql &= .Item("pcode").ToString & "',"            ' pcode
                        strSql &= .Item("疵種EdasysID").ToString & ","      ' 疵種EdasysID
                        strSql &= .Item("探傷画面表示_表").ToString & ","   ' 探傷画面表示_表
                        strSql &= .Item("探傷画面表示_裏").ToString & ","   ' 探傷画面表示_裏
                        strSql &= .Item("入側単位疵点マップ画面表示").ToString & ","  ' 疵点マップ画面表示
                        strSql &= .Item("FF送信有無").ToString & ","        ' FF送信有無
                        strSql &= .Item("疵接近軽").ToString & ","          ' 疵接近軽
                        strSql &= .Item("疵接近重").ToString & ","          ' 疵接近重
                        strSql &= .Item("パトライト出力").ToString & ","    ' パトライト出力
                        strSql &= .Item("ボイス連続疵").ToString & ","      ' ボイス連続疵
                        strSql &= .Item("ボイス重大疵").ToString & ","      ' ボイス重大疵
                        strSql &= .Item("ボイスヘゲ").ToString & ","        ' ボイスヘゲ
                        strSql &= .Item("ボイススリバ").ToString & ","      ' ボイススリバ
                        strSql &= .Item("ボイス表裏同一疵").ToString & ","  ' ボイス表裏同一疵
                        strSql &= .Item("ボイス周期疵").ToString & ","      ' ボイス周期疵
                        strSql &= .Item("ボイスその他疵").ToString & ","    ' ボイスその他疵
                        strSql &= .Item("DB保存_一般_表").ToString & ","    ' DB保存_一般_表
                        strSql &= .Item("DB保存_一般_裏").ToString & ","    ' DB保存_一般_裏
                        strSql &= .Item("DB保存_優先_表").ToString & ","    ' DB保存_優先_表
                        strSql &= .Item("DB保存_優先_裏").ToString & ")"    ' DB保存_優先_裏

                        'SQL実行
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' エラーならロールバックして終了
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
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
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal bDisptype As Boolean)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                         ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .ColumnHeadersHeight = 38                                     ' ヘッダーの高さ
            .RowTemplate.Height = 19                                      ' 改行されてる文字が見えてしまうので、デフォルトの行高さを変更
            .ScrollBars = ScrollBars.Both                                 ' スクロールバーの表示

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True
        End With

        '' 列定義

        '「上記以外」のグリッドの設定
        If bDisptype = False Then
            dgv.ColumnHeadersVisible = False        ' 列ヘッダー非表示
            dgv.ScrollBars = ScrollBars.None        ' スクロールバー非表示
        End If

        ' NO
        dgvColSetText(dgv, "NO", 35, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 疵種
        If bDisptype = True Then
            ' dgvData部品プロパティセット
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "疵種"
                clm.Width = 120
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Frozen = True
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DataSource = m_DsType.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
                clm.ValueMember = "EdasysID"             ' コンボボックスの実際の値（データソースの列名）
                clm.DisplayMember = "Name"               ' コンボボックスで表示される値（データソースの列名）
                clm.MaxDropDownItems = 10                ' ドロップダウンリストの表示数
                dgv.Columns.Add(clm)
            End Using
        Else
            ' dgvEtc部品プロパティセット
            dgvColSetText(dgv, "", 120, True, True, _
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        End If

        ' 探傷画面表示_表
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "探傷画面" & vbNewLine & "表示(表)"
            clm.Width = 78
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using


        ' 探傷画面表示_裏
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "探傷画面" & vbNewLine & "表示(裏)"
            clm.Width = 78
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' 入側単位疵点マップ画面表示
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "疵点ﾏｯﾌﾟ" & vbNewLine & "画面表示"
            clm.Width = 70
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)            ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' DB保存_一般_表
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ＤＢ保存" & vbNewLine & "一般(表)"
            clm.Width = 78
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' DB保存_一般_裏
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ＤＢ保存" & vbNewLine & "一般(裏)"
            clm.Width = 78
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' DB保存_優先_表
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ＤＢ保存" & vbNewLine & "優先(表)"
            clm.Width = 78
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' DB保存_優先_裏
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ＤＢ保存" & vbNewLine & "優先(裏)"
            clm.Width = 78
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"              ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                 ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                   ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' FF送信有無
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "FF送信" & vbNewLine & "有無"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        '疵接近軽 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "疵接近" & vbNewLine & "軽"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        '疵接近重 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "疵接近" & vbNewLine & "重"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        'パトライト出力 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ﾊﾟﾄﾗｲﾄ" & vbNewLine & "出力"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        'ボイス連続疵 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "連続疵"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        'ボイス重大疵 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "重大疵"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' ボイスヘゲ
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "ヘゲ"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        'ボイススリバ 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "スリバ"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' ボイス表裏同一疵
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "表裏疵"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        'ボイス周期疵 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "周期疵"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

        ' ボイスその他
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "ボイス" & vbNewLine & "その他"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' コンボボックスで表示するデータソース（グレード非接続テーブル）
            clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
            clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
            clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
            clm.DropDownWidth = 150                         ' 列の幅が狭いので、ドロップダウンリストの幅を指定
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' dgvのセルが選択されたときにコンボボックスならドロップダウンする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}でのドロップダウン操作が、開発PCだと出来ない、原因不明</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter, dgvEtc.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        ' 列のタイプがコンボボックスか？
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' ドロップダウン表示
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    ''' <summary>
    ''' dgvDataの横スクロールが変更されたときに その他dgvをスクロールさせる
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>項目が増えたとき用</remarks>
    Private Sub dgvData_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvData.Scroll
        ' スクロールイベントが水平スクロールなら
        If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
            ' dgvEtcとdgvDataのスクロール位置を合わせる
            dgvEtc.HorizontalScrollingOffset = dgvData.HorizontalScrollingOffset
        End If
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
        Dim rec As Integer = CInt(IIf(bln, cmbPCode.SelectedIndex + 1, cmbPCode.SelectedIndex - 1))
        ' 上下限チェック
        If rec < 0 OrElse rec >= cmbPCode.Items.Count Then Exit Sub
        '切替
        cmbPCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' 特定セルの値が変わったら色を変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged, dgvEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行以外はスルー
        If e.RowIndex < 0 Then Exit Sub

        ' GrEdasysIDを入れている列なら色変更
        If e.ColumnIndex = EM_DATACOLUMN.emCmb_Tansyo_Top OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_Tansyo_Bot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DefMap OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Ipan_Top OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Ipan_Bot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Yusen_Top OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Yusen_Bot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_FF OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_KizuKei OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_KizuZyu OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_PatLight OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceRen OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceZyu OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceHege OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceSuriba OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceTopBot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceSyuki OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceEtc Then

            ' セルの値を取得
            Dim strEdasysID As String = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

            ' セルの値（EdasysID）で色を取得し、背景色を変更する 
            dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
        End If
    End Sub

    ''' <summary>
    ''' 色取得
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbColor(ByVal key As String) As Color
        ' EdasysIDの値を取り出し
        Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE句に相当の条件生成
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select(strWhere)       ' 1行あるはず

        Dim retColor As Color = Color.Empty   ' 戻り値 初期値はNULL色

        ' Keyが0ならNULL色を返す
        If 0 = CInt(key) Then Return retColor

        ' 1行しかないと思うけど
        For ii As Integer = 0 To drowGrade.Length - 1
            ' カラーを取得
            retColor = Color.FromArgb(CInt(drowGrade(0).Item("マップ表示色").ToString))
        Next ii

        Return retColor
    End Function

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
        'コピー元
        Dim strFromKey As String = ""                                    'コピー元キー 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '元
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        'コピー先
        Dim strToKey As String = MyBase.GetUpCodeItem_To(False)              'コピー先キー
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

        ' 取得データ初期化
        Data_Init()

        '''' 画面再表示
        Data_Disp(m_KeyCode)
    End Sub

    ''' <summary>
    ''' コピー元のpcodeからコピー先のpcodeへデータをデリートインサートする
    ''' </summary>
    ''' <param name="strFromkey">コピー元キー</param>
    ''' <param name="strToKey">コピー先のキーが入った文字列</param>
    ''' <returns>正常/異常</returns>
    ''' <remarks></remarks>
    Private Function DB_UPDATE_UpCodeItem(ByVal strFromkey As String, ByVal strToKey As String) As Boolean

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String = ""
        Dim strPcode() As String                                ' コピー先のpcode
        Dim strErrMsg As String = ""

        'カンマ区切りのpcodeを配列に変換
        strPcode = Split(strToKey, ",")

        'コピー元のデータを取得
        strSQL = "SELECT pcode"
        strSQL &= ", 疵種EdasysID"
        strSQL &= ", 探傷画面表示_表"
        strSQL &= ", 探傷画面表示_裏"
        strSQL &= ", 入側単位疵点マップ画面表示"
        strSQL &= ", FF送信有無"
        strSQL &= ", 疵接近軽"
        strSQL &= ", 疵接近重"
        strSQL &= ", パトライト出力"
        strSQL &= ", ボイス連続疵"
        strSQL &= ", ボイス重大疵"
        strSQL &= ", ボイスヘゲ"
        strSQL &= ", ボイススリバ"
        strSQL &= ", ボイス表裏同一疵"
        strSQL &= ", ボイス周期疵"
        strSQL &= ", ボイスその他疵"
        strSQL &= ", DB保存_一般_表"
        strSQL &= ", DB保存_一般_裏"
        strSQL &= ", DB保存_優先_表"
        strSQL &= ", DB保存_優先_裏"

        strSQL &= " FROM " & DB_PCODE_OUTPUT
        strSQL &= " WHERE pcode='" & strFromkey & "'"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPCode, m_AdapterPCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_CAMSET & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Try

            '''' デリートインサート
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            ' レコードの削除
            Dim strSqlDel As String     ' レコード削除用sql
            strSqlDel = "DELETE FROM " & DB_PCODE_OUTPUT & " WHERE pcode IN(" & strToKey & ")"

            If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False

            ' レコードの挿入 コピー先pcodeの個数だけ繰り返し
            For ii As Integer = 0 To strPcode.Length - 1
                ' コピー元pcodeのレコード数だけ繰り返し挿入
                For kk As Integer = 0 To m_DsPCode.Tables(0).Rows.Count - 1
                    With m_DsPCode.Tables(0).Rows(kk)
                        If CInt(.Item("疵種EdasysID").ToString) <> 0 Then
                            'OUTPUT_PCODE登録SQL文作成
                            strSQL = ""
                            strSQL &= "INSERT INTO " & DB_PCODE_OUTPUT & " VALUES ("
                            strSQL &= strPcode(ii) & ","                        ' pcode
                            strSQL &= .Item("疵種EdasysID").ToString & ","      ' 疵種EdasysID
                            strSQL &= .Item("探傷画面表示_表").ToString & ","   ' 探傷画面表示_表
                            strSQL &= .Item("探傷画面表示_裏").ToString & ","   ' 探傷画面表示_裏
                            strSQL &= .Item("入側単位疵点マップ画面表示").ToString & ","  ' 疵点マップ画面表示
                            strSQL &= .Item("FF送信有無").ToString & ","        ' FF送信有無
                            strSQL &= .Item("疵接近軽").ToString & ","          ' 疵接近軽
                            strSQL &= .Item("疵接近重").ToString & ","          ' 疵接近重
                            strSQL &= .Item("パトライト出力").ToString & ","    ' パトライト出力
                            strSQL &= .Item("ボイス連続疵").ToString & ","      ' ボイス連続疵
                            strSQL &= .Item("ボイス重大疵").ToString & ","      ' ボイス重大疵
                            strSQL &= .Item("ボイスヘゲ").ToString & ","        ' ボイスヘゲ
                            strSQL &= .Item("ボイススリバ").ToString & ","      ' ボイススリバ
                            strSQL &= .Item("ボイス表裏同一疵").ToString & ","  ' ボイス表裏同一疵
                            strSQL &= .Item("ボイス周期疵").ToString & ","      ' ボイス周期疵
                            strSQL &= .Item("ボイスその他疵").ToString & ","    ' ボイスその他疵
                            strSQL &= .Item("DB保存_一般_表").ToString & ","    ' DB保存_一般_表
                            strSQL &= .Item("DB保存_一般_裏").ToString & ","    ' DB保存_一般_裏
                            strSQL &= .Item("DB保存_優先_表").ToString & ","    ' DB保存_優先_表
                            strSQL &= .Item("DB保存_優先_裏").ToString & ")"    ' DB保存_優先_裏

                            'SQL実行
                            If 0 > tcls_DB.DMLExecute(strSQL) Then
                                ' エラーならロールバックして終了
                                tcls_DB.TransactRollback()
                                Return False
                            End If
                        End If
                    End With
                Next kk
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
            modMain.HardCopy(Me)
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

    ''' <summary>
    ''' 入力値の範囲チェック
    ''' </summary>
    ''' <param name="nCheckGr">nCheckGr(0):DB保存_一般_表,DB保存_一般_裏、nCheckGr(1):DB保存_優先_表,DB保存_優先_裏</param>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_Range_Check(ByRef nCheckGr() As Integer) As Boolean

        ' DB保存_一般,優先 共に未選択はOK
        If 0 >= nCheckGr(0) And 0 >= nCheckGr(1) Then
            Return True
        End If

        ' DB保存_優先のみ選択ありはNG
        If 0 >= nCheckGr(0) And 0 < nCheckGr(1) Then
            Return False
        End If

        ' DB保存_一般のみ選択ありはOK
        If 0 < nCheckGr(0) And 0 >= nCheckGr(1) Then
            Return True
        End If

        Dim nCheckPriority1 As Integer = 0
        Dim nCheckPriority2 As Integer = 0

        nCheckPriority1 = GetCmbPriority(nCheckGr(0))     ' DB保存_一般（優先度）
        nCheckPriority2 = GetCmbPriority(nCheckGr(1))     ' DB保存_優先（優先度）

        ' DB保存_一般＞DB保存_優先はNG （値の小さい方が強い(優先度が高い)）
        If nCheckPriority1 < nCheckPriority2 Then
            Return False
        End If

        ' DB保存_一般＜＝DB保存_優先 OK
        Return True

    End Function

    ''' <summary>
    ''' GrEdasysIDに対する優先度取得
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbPriority(ByVal key As Integer) As Integer
        ' EdasysIDの値を取り出し
        Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE句に相当の条件生成
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select(strWhere)       ' 1行あるはず

        Dim retPriority As Integer = 0   ' 戻り値 初期値は0

        ' Keyが0なら0を返す
        If 0 = key Then Return retPriority

        ' 1行しかないと思うけど
        For ii As Integer = 0 To drowGrade.Length - 1
            ' 1行しかないと思うけど
            retPriority = CInt(drowGrade(0).Item("優先度").ToString)
        Next ii

        Return retPriority
    End Function

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン
        pnlItem.Enabled = bPassResult                   ' コピー用リストの反映ボタン

    End Sub

End Class
