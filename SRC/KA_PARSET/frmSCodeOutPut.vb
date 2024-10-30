'*********************************************************************************
' 検査表面状態別出力設定画面クラス
'	[Ver]
'		Ver.01    2007/08/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmSCodeOutPut
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' データグリッドビューの列
    Private Enum em_DataColumn
        iText_No = 0            ' テキストボックス  No
        iCombo_Name             ' コンボボックス  疵種
        iCombo_Dummy            ' コンボボックス  ダミー出力
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterSCode As SqlClient.SqlDataAdapter = Nothing        ' アダプター 検査表面状態別出力設定
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing         ' アダプター 疵種マスター
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing        ' アダプター グレードマスター

    Private m_DsSCode As New DataSet                                    ' 非接続型テーブル 検査表面状態別出力設定データ
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
    Private Sub frmSetSCode_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
                Call HardCopy(IMG_PATH, "検査表面状態別出力設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmSetSCode_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DsSCode.Dispose()
        m_DsType.Dispose()
        m_DsGrade.Dispose()

        If Not m_AdapterSCode Is Nothing Then
            m_AdapterSCode.Dispose()
            m_AdapterSCode = Nothing
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
    Private Sub frmSetSCode_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                 ' メニューリスト生成
        modMain.SetLabelSysName(Me)                             ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmSCodeOutput       ' 検査表面状態別出力設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' アップコードのコンボボックス
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
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
        Data_init()

        ' dgvの設定
        dgvInitialize(dgvData)
        dgvColunmDefData(dgvData, True)
        MyBase.DataGridViewSelectionCancel(dgvData)

        dgvInitialize(dgvEtc)
        dgvColunmDefData(dgvEtc, False)
        MyBase.DataGridViewSelectionCancel(dgvEtc)

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

        ' 画面にパターン別の値を表示
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        Data_Disp(m_KeyCode)

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        MyBase.DataGridViewSelectionRowColorChange(dgvData)

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
    Private Sub frmSetSCode_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

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

        ' SQL文生成 検査表面状態別出力は疵種のIDで昇順 複数テーブルを参照しているので、データアダプタでアップデートできない
        strSQL = "SELECT scode , "
        strSQL &= " 疵種EdasysID, "
        strSQL &= " ダミー出力 "
        strSQL &= " FROM " & DB_SCODE_OUTPUT
        strSQL &= " AS a LEFT OUTER JOIN " & DB_TYPE_NAME
        strSQL &= " AS b ON a.疵種EdasysID = b.EdasysID "
        strSQL &= " ORDER by b.ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsSCode, m_AdapterSCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_SCODE_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成 疵種はIDで昇順
        strSQL = "SELECT EdasysID, Name "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成 グレードは優先度で昇順
        strSQL = "SELECT EdasysID, Name ,マップ表示色 "
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
    ''' アップデート
    ''' </summary>
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

            ' デリートインサートするので検査表面状態出力設定テーブルの全項目を削除
            strSql = "DELETE FROM " & DB_SCODE_OUTPUT
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            '検査表面状態出力設定テーブルへデータを登録 
            'データテーブルは複数テーブルを参照している為、アップデートできないので、インサート文で
            For ii As Integer = 0 To m_DsSCode.Tables(0).Rows.Count - 1
                With m_DsSCode.Tables(0).Rows(ii)
                    If CInt(.Item("疵種EdasysID").ToString) <> 0 Then
                        'TYPEOUT登録SQL文作成
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_SCODE_OUTPUT & " VALUES ("
                        strSql &= .Item("scode").ToString & ","             'scode
                        strSql &= .Item("疵種EdasysID").ToString & ","      '疵種EdasysID
                        strSql &= .Item("ダミー出力").ToString & ")"        'ダミー出力

                        'SQL実行
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' エラーならロールバックして終了
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next

            'コミット
            tcls_DB.TransactCommint()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_SCODE, CH_SCODE_ALL)       ' 何を変更したかわからないので、全変更区分を送る
			modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' 後始末
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' 検査パターン変更時
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

        '今の選択状態を保存
        g_nScodeIndex = cmbSCode.SelectedIndex

    End Sub
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
            Next
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
    ''' <param name="key">主キーの値(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        ' 一括のテーブルなので、検査パターン毎の値のみ取り出し
        Dim strWhere As String = String.Format("scode='{0}' and 疵種EdasysID>=0", key)          ' WHERE句に相当の条件生成 0～999の疵種EdasysIDが検出されるはず
        Dim strWhereEtc As String = String.Format("scode='{0}' and 疵種EdasysID=-1", key)       ' WHERE句に相当の条件生成 -1の疵種EdasysIDが検出されるはず
        Dim drow() As DataRow = m_DsSCode.Tables(0).Select(strWhere)                            ' EdasysIDが1以上のデータを    0～64行
        Dim drowEtc() As DataRow = m_DsSCode.Tables(0).Select(strWhereEtc)                      ' EdasysIDが-1のその他データを取ってくる   0～1行
        Dim nRowCount As Integer = 0                                                            ' 行数カウント用

        Try
            ' 画面に表示
            dgvData.RowCount = MAX_TYPE                 ' 固定64行
            For ii As Integer = 0 To dgvData.RowCount - 1
                dgvData(em_DataColumn.iText_No, ii).Value = ii + 1                                  ' No
                If ii < drow.Length Then
                    ' 検査表面状態別出力設定テーブルをscodeで上から検索する (selectを使うとソートされるので)
                    For kk As Integer = nRowCount To m_DsSCode.Tables(0).Rows.Count - 1
                        ' scodeがkeyの値で、疵種EdasysIDが-1以外の行を取得し、行数カウントをインクリメント                    
                        If m_DsSCode.Tables(0).Rows(kk).Item("scode").ToString = key AndAlso
                        CInt(m_DsSCode.Tables(0).Rows(kk).Item("疵種EdasysID").ToString) >= 0 Then
                            dgvData(em_DataColumn.iCombo_Name, ii).Value = m_DsSCode.Tables(0).Rows(kk).Item("疵種EdasysID")    ' 疵種
                            dgvData(em_DataColumn.iCombo_Dummy, ii).Value = m_DsSCode.Tables(0).Rows(kk).Item("ダミー出力")     ' ダミー出力
                            nRowCount = kk + 1
                            Exit For
                        End If
                    Next kk
                Else
                    ' 表示データが無い行は0（無し）を入れる
                    dgvData(em_DataColumn.iCombo_Name, ii).Value = 0                                ' 疵種
                    dgvData(em_DataColumn.iCombo_Dummy, ii).Value = 0                               ' ダミー出力
                End If
            Next ii

            ' その他dgvの表示
            dgvEtc.RowCount = 1                                                             ' その他dgvは一行固定
            dgvEtc(em_DataColumn.iText_No, 0).Value = ""                                    ' No    空白
            dgvEtc(em_DataColumn.iCombo_Name, 0).Value = "上記以外"                         ' 疵種は上記以外で固定
            ' DBに表示データがあるか
            If drowEtc.Length > 0 Then
                dgvEtc(em_DataColumn.iCombo_Dummy, 0).Value = drowEtc(0).Item("ダミー出力") ' ダミー出力
            Else
                ' 表示データが無い行は0（無し）を入れる
                dgvEtc(em_DataColumn.iCombo_Dummy, 0).Value = 0 ' ダミー出力
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
    ''' 入力値のチェック
    ''' </summary>
    ''' <param name="key">主キーの値(scode) 今の所使わないかなー</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strMsg As String       ' エラーメッセージ用
        ' 行数分ループ コンボボックスしかないので、疵種EdasysIDの重複チェックのみ行う
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            ' 疵種が無しの行は無視
            If CInt(dgvData(em_DataColumn.iCombo_Name, ii).Value) = 0 Then Continue For

            ' 疵種の重複チェック
            For jj As Integer = ii + 1 To dgvData.Rows.Count - 1
                ' 比較先が無しの行は無視
                If CInt(dgvData(em_DataColumn.iCombo_Name, jj).Value) = 0 Then Continue For

                ' 疵種が同じならメッセージを出して終了
                If CInt(dgvData(em_DataColumn.iCombo_Name, ii).Value) = CInt(dgvData(em_DataColumn.iCombo_Name, jj).Value) Then
                    strMsg = "【 疵種 】" & vbNewLine
                    strMsg &= "同じ疵種は登録できません。" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                    MsgBox(strMsg)
                    Return False
                End If
            Next
        Next ii
        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <param name="key">主キーの値(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String = String.Format("scode='{0}'", key)  ' WHERE句に相当の条件生成
        Dim drow() As DataRow                                       ' データ行
        Dim nRowCount As Integer                                    ' 削除対象のデータ数
        drow = m_DsSCode.Tables(0).Select(strWhere)           ' scodeが主キーの値の行を取り出す

        '現在画面に表示しているscodeの非接続テーブルのデータを削除(後ろから削除)
        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DsSCode.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next

        ' 今表示しているデータを全て格納 
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            Dim drowNew As DataRow = m_DsSCode.Tables(0).NewRow()   ' 新規行
            ' データをセット
            drowNew.Item("scode") = CInt(key)
            drowNew.Item("疵種EdasysID") = dgvData(em_DataColumn.iCombo_Name, ii).Value
            drowNew.Item("ダミー出力") = dgvData(em_DataColumn.iCombo_Dummy, ii).Value

            ' テーブルに追加
            m_DsSCode.Tables(0).Rows.InsertAt(drowNew, m_DsSCode.Tables(0).Rows.Count)
        Next

        ' その他行も格納
        ' データをセット
        Dim drowNewEtc As DataRow = m_DsSCode.Tables(0).NewRow()  ' 新規行
        drowNewEtc.Item("scode") = CInt(key)
        drowNewEtc.Item("疵種EdasysID") = -1                            ' その他行のEdasysIDは-1で固定
        drowNewEtc.Item("ダミー出力") = dgvEtc(em_DataColumn.iCombo_Dummy, 0).Value
        ' テーブルに追加
        m_DsSCode.Tables(0).Rows.InsertAt(drowNewEtc, m_DsSCode.Tables(0).Rows.Count)
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

            '' 余計なEdasysIDの行をカット
            'Data_Cut()
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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv
            .ColumnCount = 0

            '' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                                ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                               ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '
            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With
    End Sub
    ''' <summary>
    ''' dgvの選択状態が変更されたら、セルの選択をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvData.SelectionChanged, dgvEtc.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        dgvData.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' データdgv列定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <param name="bDisptype">dgvのタイプ ture = dgvData, false = dgvEtc</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDefData(ByVal dgv As DataGridView, ByVal bDisptype As Boolean)

        With dgv
            ''=================================
            '' 列定義

            ' NO
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "NO"

                clm.Width = 60
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter

                .Columns.Add(clm)
            End Using

            ' 疵種
            If bDisptype = True Then
                ' dgvData部品プロパティセット
                Using clm As New DataGridViewComboBoxColumn
                    clm.HeaderText = "疵種"
                    clm.Width = 150
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = False
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DataSource = m_DsType.Tables(0)             ' コンボボックスで表示するデータソース（疵種テーブル）
                    clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
                    clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
                    clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
                    .Columns.Add(clm)
                End Using
            Else
                ' dgvEtc部品プロパティセット
                Using clm As New DataGridViewTextBoxColumn

                    clm.Width = 150
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = False
                    clm.ReadOnly = True
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            ' ダミー出力
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "ダミー出力"
                clm.Width = 150
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Frozen = False
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DataSource = m_DsGrade.Tables(0)            ' コンボボックスで表示するデータソース（グレード非接続テーブル）
                clm.ValueMember = "EdasysID"                    ' コンボボックスの実際の値（データソースの列名）
                clm.DisplayMember = "Name"                      ' コンボボックスで表示される値（データソースの列名）
                clm.MaxDropDownItems = 10                       ' ドロップダウンリストの表示数
                .Columns.Add(clm)
            End Using

            If bDisptype = False Then
                ' dgvEtcの独自設定
                .ColumnHeadersVisible = False                           ' ヘッダー非表示
            End If
        End With
    End Sub

    ''' <summary>
    ''' セルの状態が変化した時の処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvData.CurrentCellDirtyStateChanged, dgvEtc.CurrentCellDirtyStateChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        ' セルがコミットされていない変更を含む場合コミットする
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
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
        '切替
        cmbSCode.SelectedIndex = rec
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

        ' ダミー出力列なら色変更
        If e.ColumnIndex = em_DataColumn.iCombo_Dummy Then
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
        Next

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
        Dim strToKey As String = MyBase.GetUpCodeItem_To(True)              'コピー先キー
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '''' アップデート 可変行のためUPDATE文は使えないので、デリーとインサートしている
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

        '' カンマ区切りのscodeを配列に変換
        strScode = Split(strtokey, ",")

        'コピー元のデータを取得
        strSQL = "SELECT scode , "
        strSQL &= " 疵種EdasysID, "
        strSQL &= " ダミー出力 "
        strSQL &= " FROM " & DB_SCODE_OUTPUT
        strSQL &= " WHERE scode='" & strFromkey & "'"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsSCode, m_AdapterSCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_SCODE_OUTPUT & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If


        Try

            '''' デリートインサート
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            ' レコードの削除
            Dim strSqlDel As String     ' レコード削除用sql
            strSqlDel = "DELETE FROM " & DB_SCODE_OUTPUT & " WHERE scode IN(" & strToKey & ")"

            If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False


            ' レコードの挿入 コピー先scodeの個数だけ繰り返し
            For ii As Integer = 0 To strScode.Length - 1
                ' コピー元scodeのレコード数だけ繰り返し挿入
                For kk As Integer = 0 To m_DsSCode.Tables(0).Rows.Count - 1
                    With m_DsSCode.Tables(0).Rows(kk)
                        If CInt(.Item("疵種EdasysID").ToString) <> 0 Then
                            'OUTPUT_SCODE登録SQL文作成
                            strSQL = ""
                            strSQL &= "INSERT INTO " & DB_SCODE_OUTPUT & " VALUES ("
                            strSQL &= strScode(ii) & ","                        'scode
                            strSQL &= .Item("疵種EdasysID").ToString & ","      '疵種EdasysID
                            strSQL &= .Item("ダミー出力").ToString & ")"        'ダミー出力
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
            modMain.Send_Main(CH_PARAM_SETTIG_SCODE, CH_SCODE_ALL)
			modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コピーエラー [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
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
