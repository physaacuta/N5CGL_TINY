'*********************************************************************************
' 警報出力設定画面クラス
'	[Ver]
'		Ver.01    2007/08/16  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmCommonAlarm
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル
    
    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmAlarm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "警報出力設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">選択を取りやめたい場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmAlarm_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
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
    ''' <remarks></remarks>
    Private Sub frmAlarm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmCommonAlarm           ' 警報出力設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' テキストボックスの設定
        txtInitialize()

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        ' 画面に値を表示        
        If Not Data_Disp() Then
            ' 画面表示失敗なので、強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DBにデータが無い為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
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
    Private Sub frmCommonAlarm_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub


    ''' <summary>
    ''' テキストボックスなどの初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub txtInitialize()
        ' テキストボックスの初期設定
        ' 溶接点不感帯先端
        txtYousetuSen.ImeMode = Windows.Forms.ImeMode.Disable   ' IMEモード使用不可
        txtYousetuSen.Text = ""                                 ' 初期化

        ' 溶接点不感帯尾端
        txtYousetuBi.ImeMode = Windows.Forms.ImeMode.Disable    ' IMEモード使用不可
        txtYousetuBi.Text = ""                                  ' 初期化

        ' 疵接近出力区間表前
        txtKizuTopMae.ImeMode = Windows.Forms.ImeMode.Disable   ' IMEモード使用不可
        txtKizuTopMae.Text = ""                                 ' 初期化

        ' 疵接近出力区間表後
        txtKizuTopGo.ImeMode = Windows.Forms.ImeMode.Disable    ' IMEモード使用不可
        txtKizuTopGo.Text = ""                                  ' 初期化

        ' ボイス出力位置表
        txtVoiceTop.ImeMode = Windows.Forms.ImeMode.Disable     ' IMEモード使用不可
        txtVoiceTop.Text = ""                                   ' 初期化
    End Sub

    ''' <summary>
    ''' このフォームで必要なデータを取得する
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT LINEID "
        strSQL &= ",疵接近ランプ_不感帯_先端"
        strSQL &= ",疵接近ランプ_不感帯_尾端"
        strSQL &= ",疵接近ランプ_出力区間_前"
        strSQL &= ",疵接近ランプ_出力区間_後"
        strSQL &= ",疵接近アラーム_不感帯_先端"
        strSQL &= ",疵接近アラーム_不感帯_尾端"
        strSQL &= ",疵接近アラーム_出力区間_前"
        strSQL &= ",疵接近アラーム_出力区間_後"
        strSQL &= ",ボイス_出力位置_前"

        strSQL &= " FROM " & DB_COMMON_MASTER

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

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

                'コマンドアダプタをセットして、更新
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            'コミット
            transact.Commit()

            '正常に登録された場合はメール送信
            modMain.Send_Main(CH_PARAM_SETTIG_ALARM)
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
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean
        Dim drow() As DataRow     ' データ行

        ' 主キーを指定して取り出してるので、1件しかないはず
        drow = m_DS.Tables(0).Select()

        ' 取得行が1行でないならエラー終了
        If drow.Length <> 1 Then Return False

        ' 画面に表示
        txtYousetuSen.Text = CStr(drow(0).Item("疵接近ランプ_不感帯_先端"))
        txtYousetuBi.Text = CStr(drow(0).Item("疵接近ランプ_不感帯_尾端"))
        txtKizuTopMae.Text = CStr(drow(0).Item("疵接近ランプ_出力区間_前"))
        txtKizuTopGo.Text = CStr(drow(0).Item("疵接近ランプ_出力区間_後"))
        txtAlarmSen.Text = CStr(drow(0).Item("疵接近アラーム_不感帯_先端"))
        txtAlarmBi.Text = CStr(drow(0).Item("疵接近アラーム_不感帯_尾端"))
        txtKizuBotMae.Text = CStr(drow(0).Item("疵接近アラーム_出力区間_前"))
        txtKizuBotGo.Text = CStr(drow(0).Item("疵接近アラーム_出力区間_後"))
        txtVoiceTop.Text = CStr(drow(0).Item("ボイス_出力位置_前"))

        Return True
    End Function

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     ' 作業用
        ' 疵接近ランプ 溶接点不感帯先端
        strWk = txtYousetuSen.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "疵接近ランプ 溶接点前後の不感帯 先端") <> 0 Then Return False
        txtYousetuSen.Text = strWk

        ' 疵接近ランプ 溶接点不感帯尾端
        strWk = txtYousetuBi.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "疵接近ランプ 溶接点前後の不感帯 尾端") <> 0 Then Return False
        txtYousetuBi.Text = strWk

        ' 疵接近アラーム 溶接点不感帯先端
        strWk = txtAlarmSen.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "疵接近アラーム 溶接点前後の不感帯 先端") <> 0 Then Return False
        txtAlarmSen.Text = strWk

        ' 疵接近アラーム 溶接点不感帯尾端
        strWk = txtAlarmBi.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "疵接近アラーム 溶接点前後の不感帯 尾端") <> 0 Then Return False
        txtAlarmBi.Text = strWk

        ' 疵接近出力区間表前
        strWk = txtKizuTopMae.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "疵接近出力区間 表検査台 手前") <> 0 Then Return False
        txtKizuTopMae.Text = strWk

        ' 疵接近出力区間表後
        strWk = txtKizuTopGo.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "疵接近出力区間 表検査台 通過後") <> 0 Then Return False
        txtKizuTopGo.Text = strWk

        ' 疵接近アラーム出力区間裏前
        strWk = txtKizuBotMae.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "疵接近出力区間 裏検査台 手前") <> 0 Then Return False
        txtKizuBotMae.Text = strWk

        ' 疵接近アラーム出力区間裏後
        strWk = txtKizuBotGo.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "疵接近出力区間 裏検査台 通過後") <> 0 Then Return False
        txtKizuBotGo.Text = strWk

        ' ボイス出力位置表
        strWk = txtVoiceTop.Text
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "ボイス出力位置 表検査台") <> 0 Then Return False
        txtVoiceTop.Text = strWk

        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim drow As DataRow     ' データ行

        ' 主キーを指定して取り出されてるので一件しかない
        drow = m_DS.Tables(0).Select()(0)

        ' セット
        drow.Item("疵接近ランプ_不感帯_先端") = CInt(txtYousetuSen.Text)
        drow.Item("疵接近ランプ_不感帯_尾端") = CInt(txtYousetuBi.Text)
        drow.Item("疵接近ランプ_出力区間_前") = CInt(txtKizuTopMae.Text)
        drow.Item("疵接近ランプ_出力区間_後") = CInt(txtKizuTopGo.Text)
        drow.Item("疵接近アラーム_不感帯_先端") = CInt(txtAlarmSen.Text)
        drow.Item("疵接近アラーム_不感帯_尾端") = CInt(txtAlarmBi.Text)
        drow.Item("疵接近アラーム_出力区間_前") = CInt(txtKizuBotMae.Text)
        drow.Item("疵接近アラーム_出力区間_後") = CInt(txtKizuBotGo.Text)
        drow.Item("ボイス_出力位置_前") = CInt(txtVoiceTop.Text)
    End Sub
    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()
        ' 入力値チェック
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '画面のデータを取得
            Data_Get()

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

            ' 現在の内容で再表示
            Data_Disp()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try
    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub
End Class
