'*********************************************************************************
' フィルタ設定
'	[Ver]
'		Ver.01    2017/01/10  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt

Public Class frmEpcFilter

    Const SIGMA As Double = 0.01

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' アダプター
    Private m_DS As New DataSet                                     ' 非接続型テーブル（多重分解能）
    'Private m_DSCycle As New DataSet                                ' 非接続型テーブル（周期用分解能）
    Private m_AdapterFilter As New SqlClient.SqlDataAdapter         ' アダプター
    Private m_DsFilter As New DataSet                               ' 非接続型テーブルPARAM_FILTER

    Private m_nSelectIdx As Integer = -1                            ' 番号選択キー

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
    Private Sub frmEpcFilter_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "フィルタ設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSETへ切替処理
            Case 10
                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' F11 設定
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
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DS.Dispose()
        'm_DSCycle.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If
        m_DsFilter.Dispose()
        If Not m_AdapterFilter Is Nothing Then
            m_AdapterFilter.Dispose()
            m_AdapterFilter = Nothing
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
    Private Sub frmEpcFilter_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                 ' メニューリスト生成
        modMain.SetLabelSysName(Me)                             ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcFilter         ' 上位情報内容設定呼び出し

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        dgvInitializeCoef(dgvCoef)
        MyBase.DataGridViewSelectionCancel(dgvCoef)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)
            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        Else

            cmbId.DataSource = Nothing
            cmbSizeX.DataSource = Nothing

            ComboSet_Size()

            ComboSet_ID()

        Data_Disp()

            HandlerEnable(True)

            ' 更新ボタンの有効/無効
            Call UpdateEnabled(g_bPassFlg)

        End If

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        MyBase.DataGridViewSelectionRowColorChange(dgvCoef)

    End Sub
    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT * "
        strSQL &= " FROM " & DB_PARAM_FILTER
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsFilter, m_AdapterFilter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' <20221213> <溝口> <最終工程高機能化その1> --->>>
        '' SQL文生成
        'strSQL = "SELECT scode"
        'strSQL &= ",camangle"
        'strSQL &= ",id"
        'strSQL &= ",ReductionId"
        'strSQL &= ",FilterId"
        'strSQL &= ",name"

        'strSQL &= " FROM {0}"
        'strSQL &= " ORDER BY id ASC"

        '' SQL文実行（多重分解能設定）
        'If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_THRESHOLD), m_DS, m_Adapter, strErrMsg) Then
        '    gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
        '    Return False
        'End If

        '' SQL文生成
        strSQL = "SELECT scode"
        strSQL &= ",id_filterbin"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",name"

        strSQL &= " FROM {0}"
        strSQL &= " ORDER BY id_filterbin ASC"

        ' SQL文実行（多重分解能設定）
        If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_FILTERBIN), m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTERBIN & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If
        ' <20221213> <溝口> <最終工程高機能化その1> <<<---


        '' SQL文実行（周期用分解能設定）
        'If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_CYCLE_THRESHOLD), m_DSCycle, m_Adapter, strErrMsg) Then
        '    gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_CYCLE_THRESHOLD & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
        '    Return False
        'End If

        Return True

    End Function

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Try
            '値を登録
            If Not DB_DeleteUp() Then
                Return False
            Else
                modMain.Send_Main(CH_PARAM_FILTER)
            End If

            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)
            Return False
        End Try
    End Function

    ''' <summary>
    ''' 入力値チェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean

        Dim strWk As String = ""
        Dim strMsg As String = ""
        Dim dWk As Double

        If 0 > m_nSelectIdx Then
            Return False
        End If

        ' フィルターID
        Dim nId As Integer
        If Not GetCmbIdValue(m_nSelectIdx, nId) Then
            Return False
        End If
        If tClass_ImputCheck.CheckImputMsg_Int(nId.ToString, 1, 999999, True, "フィルタID") <> 0 Then Return False

        '名称
        strWk = txtName.Text.Trim
        If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 256, "名称") <> 0 Then Return False
        txtName.Text = strWk

        '名称の重複チェック
        Dim data() As DataRow = m_DsFilter.Tables(0).Select()
        For ii As Integer = 0 To data.Length - 1
            If CInt(data(ii).Item("id")) = nId Then
                Continue For
            End If
            '名称が同じならメッセージを出して終了
            If CStr(txtName.Text) = CStr(data(ii).Item("name")) Then
                strMsg = "【 フィルタ設定 】" & vbNewLine
                strMsg &= "同じ名称は登録できません。" & vbNewLine
                strMsg &= "- - - ID." & CStr(data(ii).Item("id")) & " と重複"
                MsgBox(strMsg)
                Return False
            End If
        Next

        'カーネルサイズ
        If cmbSizeX.Text <> lblSizeY.Text Then
            strMsg = "【 フィルタ設定 】" & vbNewLine
            strMsg &= "カーネルサイズがXとYで異なります。"
            MsgBox(strMsg)
            Return False
        End If

        '加重値
        For row As Integer = 0 To dgvCoef.RowCount - 1
            For col As Integer = 0 To dgvCoef.ColumnCount - 1
                If dgvCoef.Item(col, row).ReadOnly Then
                    Continue For
                End If
                strWk = CStr(dgvCoef.Item(col, row).Value)
                If Not Double.TryParse(strWk, dWk) Then
                    strMsg = "【 フィルタ設定 】" & vbNewLine
                    strMsg &= "係数には数値を入力してください。" & vbNewLine
                    strMsg &= "- - - " & String.Format("係数{0}{1}", row + 1, col + 1)
                    MsgBox(strMsg)
                    Return False
                End If
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, -255, 255, False, 16, String.Format("係数{0}{1}", row + 1, col + 1)) <> 0 Then Return False
            Next
        Next

        '正規係数
        strWk = CStr(txtNormalizeCoef.Text)
        If Not Double.TryParse(strWk, dWk) Then
            strMsg = "【 フィルタ設定 】" & vbNewLine
            strMsg &= "正規係数には数値を入力してください。"
            MsgBox(strMsg)
            Return False
        End If
        If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, -255, 255, False, 0, "正規化係数") <> 0 Then Return False
        If CDbl(txtNormalizeCoef.Text.Trim) < SIGMA And CDbl(txtNormalizeCoef.Text.Trim) > (SIGMA * -1) Then
            strMsg = "【 フィルタ設定 】" & vbNewLine
            strMsg &= "正規係数には数値を入力してください。(0以外)"
            MsgBox(strMsg)
            Return False
        End If

        ' 2019/10/11 正規化係数を手入力可能とする(輝度値が閾値0-255を超えるため)
        'txtNormalizeCoef.Text = strWk
        'If CDbl(txtNormalizeCoef.Text) <> CalcNomalizeCoef() Then
        '    strMsg = "【 フィルタ設定 】" & vbNewLine
        '    strMsg &= "正規係数が正しくありません。" & vbNewLine
        '    strMsg &= "- - - " & CStr(CalcNomalizeCoef())
        '    MsgBox(strMsg)
        '    Return False
        'End If

        'オフセット
        strWk = CStr(txtOffset.Text)
        If Not Double.TryParse(strWk, dWk) Then
            strMsg = "【 フィルタ設定 】" & vbNewLine
            strMsg &= "オフセットには数値を入力してください。"
            MsgBox(strMsg)
            Return False
        End If
        If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 32896, False, 0, "オフセット") <> 0 Then Return False

        Return True

    End Function

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Get() As Boolean

        Dim bRet As Boolean = False


        Dim nId As Integer
        If Not GetCmbIdValue(m_nSelectIdx, nId) Then
            Return bRet
        End If

        Dim strWhere As String = String.Format("id='{0}'", nId)                          ' WHERE句に相当の条件生成
        Dim data() As DataRow = m_DsFilter.Tables(0).Select(strWhere)
        Dim dataSet As DataRow = m_DsFilter.Tables(0).NewRow

        If 0 < data.Length Then
            dataSet = data(0)
                    End If

        Try
            ' フィルターID
            dataSet.Item("id") = nId
            ' 名称
            dataSet.Item("name") = txtName.Text.Trim
            ' カーネルサイズX
            dataSet.Item("SizeX") = CInt(cmbSizeX.SelectedValue)
            ' カーネルサイズY(カーネルサイズXと同じ)
            dataSet.Item("SizeY") = dataSet.Item("SizeX")
            ' 加重値
            For row As Integer = 0 To dgvCoef.RowCount - 1
                For col As Integer = 0 To dgvCoef.ColumnCount - 1
                    Dim strName As String = String.Format("Coef{0}{1}", row + 1, col + 1)
                    If dgvCoef.Item(col, row).ReadOnly Then
                        dataSet.Item(strName) = 0
                    Else
                        dataSet.Item(strName) = dgvCoef.Item(col, row).Value
                    End If
                Next
            Next
            ' 正規化係数
            dataSet.Item("NormalizeCoef") = CDbl(txtNormalizeCoef.Text.Trim)
            ' オフセット
            dataSet.Item("Offset") = CDbl(txtOffset.Text.Trim)

            bRet = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " データセットエラー [" & ex.Message & "] " & Me.Text)
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 非接続型テーブルに新規追加
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Add()

        Dim dataNew As DataRow

        Try

            Dim data() As DataRow = m_DsFilter.Tables(0).Select()

            'IDを取得
            Dim nNewId As Integer = 1   'ID初期値
            Dim nIns As Integer = 0
            'IDが歯抜けにならないように、全てのIDを検索する。
            For ii As Integer = 0 To data.Length - 1
                If nNewId = CInt(data(ii).Item("id")) Then
                    '同じIDが見つかったら、そのIDに1を加算したIDにする
                    nNewId = CInt(data(ii).Item("id")) + 1
                    nIns = ii + 1
                End If
            Next

            dataNew = m_DsFilter.Tables(0).NewRow
            dataNew.Item("id") = nNewId
            dataNew.Item("name") = String.Empty
            dataNew.Item("SizeX") = 0
            dataNew.Item("SizeY") = 0
            dataNew.Item("NormalizeCoef") = 0
            dataNew.Item("Offset") = 0
            For row As Integer = 0 To dgvCoef.RowCount - 1
                For col As Integer = 0 To dgvCoef.ColumnCount - 1
                    Dim strName As String = String.Format("Coef{0}{1}", row + 1, col + 1)
                    dataNew.Item(strName) = 0
                Next
            Next

            m_DsFilter.Tables(0).Rows.InsertAt(dataNew, nIns)
            m_nSelectIdx = nIns


        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " フィルター情報 非接続型テーブル データセットエラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' 非接続型テーブルからデータを削除
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Del(ByVal nId As Integer)

        Dim strWhere As String = String.Format("id='{0}'", nId)     ' WHERE句に相当の条件生成

        Try

            Dim data() As DataRow = m_DsFilter.Tables(0).Select(strWhere)                ' 選択中主キー値の行を取り出す

            If 1 > data.Length Then
                '削除するデータが見つからない
                Exit Try
            End If

            'データを削除
            m_DsFilter.Tables(0).Rows.Remove(data(0))

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " フィルター情報 非接続型テーブル データセットエラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' 削除対象IDが閾値上に登録されているかチェックを行う
    ''' </summary>
    ''' <param name="nId">フィルターID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckUseID(ByVal nId As Integer) As Boolean
        Dim strWhere As String
        Dim strMsg As String    'エラーメッセージ
        Dim data() As DataRow                                        ' データ行

        strWhere = String.Format("FilterId='{0}'", nId)             ' WHERE句に相当の条件生成
        data = m_DS.Tables(0).Select(strWhere)
        If 0 < data.Length Then
            '登録済みのため異常とする
            strMsg = "フィルタID= " & CStr(nId) & " は削除できません" & vbNewLine
            strMsg &= "多重分解能閾値設定にて使用しています。"
            MsgBox(strMsg)
            Return False
        End If
        'data = m_DSCycle.Tables(0).Select(strWhere)
        'If 0 < data.Length Then
        '    '登録済みのため異常とする
        '    strMsg = "フィルタID= " & CStr(nId) & " は削除できません" & vbNewLine
        '    strMsg &= "周期用分解能閾値設定にて使用しています。"
        '    MsgBox(strMsg)
        '    Return False
        'End If

        Return True
    End Function


    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        HandlerEnable(False)

        Try

            '入力値チェック
            If Not Data_Check() Then Exit Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '画面のデータを取得
            If Not Data_Get() Then
                Exit Try
            End If

            'ホントに登録
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            'このフォームで必要なデータを取得する
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       ' 終了
            End If

            'コンボボックス設定
            ComboSet_ID()

            'データ表示
            Data_Disp()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If

            HandlerEnable(True)
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

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_DeleteUp() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        Dim sqlReader As SqlClient.SqlDataReader = Nothing   'DataReaderオブジェクト
        Dim strSql As String    'SQL文

        Try
            'データベースオープン
            If Not tcls_DB.Open() Then Return False

            'トランザクション開始
            tcls_DB.TransactStart()

            strSql = "DELETE FROM " & DB_PARAM_FILTER
            'SQL文の発行
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            'データを登録 
            For ii As Integer = 0 To m_DsFilter.Tables(0).Rows.Count - 1
                With m_DsFilter.Tables(0).Rows(ii)

                    '登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_FILTER & " VALUES ("

                    strSql &= .Item("id").ToString & ","                        'フィルタID
                    strSql &= "'" & .Item("name").ToString & "',"               '名称
                    strSql &= .Item("SizeX").ToString & ","                     'Xサイズ
                    strSql &= .Item("SizeY").ToString & ","                     'Yサイズ
                    strSql &= .Item("Offset").ToString & ","                    'オフセット
                    strSql &= .Item("NormalizeCoef").ToString & ","             'フィルタ正規化係数

                    strSql &= .Item("Coef11").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef12").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef13").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef14").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef15").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef16").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef17").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef21").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef22").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef23").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef24").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef25").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef26").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef27").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef31").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef32").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef33").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef34").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef35").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef36").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef37").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef41").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef42").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef43").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef44").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef45").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef46").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef47").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef51").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef52").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef53").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef54").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef55").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef56").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef57").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef61").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef62").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef63").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef64").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef65").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef66").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef67").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef71").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef72").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef73").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef74").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef75").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef76").ToString & ","                    'フィルタ係数
                    strSql &= .Item("Coef77").ToString                          'フィルタ係数

                    strSql &= " )"

                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        'エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 登録エラー [" & ex.Message & "] " & Me.Text)
            'エラーならロールバックして終了
            tcls_DB.TransactRollback()
            Return False

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()

        End Try
    End Function


    ''' <summary>
    ''' コンボボックス設定(カーネルサイズX)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ComboSet_Size()

        Try

            cmbSizeX.Items.Clear()

            'データテーブルに表示名、設定値をセット
            Dim dt As New DataTable()
            Dim row As DataRow
            dt.Columns.Add("NAME", GetType(String))
            dt.Columns.Add("VALUE", GetType(Integer))
            row = dt.NewRow()
            row("NAME") = "フィルターなし"
            row("VALUE") = 0
            dt.Rows.Add(row)
            row = dt.NewRow()
            row("NAME") = "3"
            row("VALUE") = 3
            dt.Rows.Add(row)
            row = dt.NewRow()
            row("NAME") = "5"
            row("VALUE") = 5
            dt.Rows.Add(row)
            row = dt.NewRow()
            row("NAME") = "7"
            row("VALUE") = 7
            dt.Rows.Add(row)
            dt.AcceptChanges()
            'データソースにテーブルを登録
            cmbSizeX.DataSource = dt
            cmbSizeX.DisplayMember = "NAME"
            cmbSizeX.ValueMember = "VALUE"
            cmbSizeX.SelectedIndex = 0

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス設定エラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' コンボボックス設定(ID)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ComboSet_ID()

        Dim data() As DataRow = m_DsFilter.Tables(0).Select()

        Try

            Dim dt As DataTable

            If cmbId.DataSource Is Nothing Then
                ' データテーブルを新規に作成
                dt = New DataTable()
                dt.Columns.Add("NAME", GetType(String))
                dt.Columns.Add("VALUE", GetType(Integer))
            Else
                ' 既存のデータテーブルを再利用
                dt = CType(cmbId.DataSource, DataTable)
                dt.Clear()
                cmbId.DataSource = Nothing
            End If

            cmbId.Items.Clear()

            For ii As Integer = 0 To data.Length - 1
                Dim row As DataRow = dt.NewRow()
                row("NAME") = CStr(data(ii).Item("id"))
                row("VALUE") = CInt(data(ii).Item("id"))
                dt.Rows.Add(row)
            Next
            dt.AcceptChanges()
            cmbId.DataSource = dt
            cmbId.DisplayMember = "NAME"
            cmbId.ValueMember = "VALUE"

            If 0 < data.Length Then
                If 0 > m_nSelectIdx Then
                    m_nSelectIdx = 0
                End If
                If cmbId.Items.Count <= m_nSelectIdx Then
                    m_nSelectIdx = cmbId.Items.Count - 1
                End If
            Else
                m_nSelectIdx = -1
            End If

            cmbId.SelectedIndex = m_nSelectIdx

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス設定エラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean

        Dim bRet As Boolean = False

        Try

            '表示を初期化
            With dgvCoef
                For row As Integer = 0 To .RowCount - 1
                    For col As Integer = 0 To .ColumnCount - 1
                        .Item(col, row).Value = String.Empty
                        .Item(col, row).Style.BackColor = Color.White
                        .Item(col, row).ReadOnly = True
                    Next
                Next
            End With
            txtName.Text = String.Empty
            lblSizeY.Text = String.Empty
            lblSizeY.BackColor = Color.White
            txtNormalizeCoef.Text = String.Empty
            txtNormalizeCoef.BackColor = Color.White
            txtOffset.Text = String.Empty
            lblSizeX.Visible = False

            If 0 > cmbId.SelectedIndex Then
                ' フィルター情報が一件もない場合、No.及び、カーネルサイズの選択、削除ボタンを無効とする
                cmbId.Enabled = False
                cmbSizeX.Enabled = False
                cmbSizeX.SelectedIndex = -1
                btnDel.Enabled = False
                ' データの表示を行わない
                Exit Try
            End If

            ' フィルター情報が有る場合、No.及び、カーネルサイズの選択、削除ボタンを有効とする
            cmbId.Enabled = True
            cmbSizeX.Enabled = True
            If g_bPassFlg Then
                btnDel.Enabled = True
            End If

            Dim strNum As String = CStr(cmbId.SelectedValue)
            Dim strWhere As String = String.Format("id='{0}'", strNum)                  ' WHERE句に相当の条件生成
            Dim data() As DataRow = m_DsFilter.Tables(0).Select(strWhere)


            '取得データが無い場合
            If 1 > data.Length Then Exit Try

            ' 画面に表示
            ' 
            txtName.Text = CStr(data(0).Item("name"))

            Dim nX As Integer = CInt(data(0).Item("SizeX"))
            If CheckCmbSizeValue(nX) Then
                cmbSizeX.SelectedValue = nX
            Else
                ' カーネルサイズ(X)が不正の場合
                cmbSizeX.SelectedValue = 0      ' 加重値はフィルターなしとして設定しておく
                lblSizeX.Text = nX.ToString
                lblSizeX.BackColor = Color.Red
                lblSizeX.Visible = True
            End If
            Dim nY As Integer = CInt(data(0).Item("SizeY"))
            If CheckCmbSizeValue(nY) AndAlso nX = nY Then
                ' カーネルサイズが正しく、且つ、Xと同じ値の場合は、ComboBoxの表示Textと同じにする
                lblSizeY.BackColor = Color.White
                lblSizeY.Text = cmbSizeX.Text
            Else
                ' 上記以外は、異常表示
                lblSizeY.Text = nY.ToString
                lblSizeY.BackColor = Color.Red
            End If

            txtNormalizeCoef.Text = CStr(data(0).Item("NormalizeCoef"))
            txtOffset.Text = CStr(data(0).Item("Offset"))

            For row As Integer = 0 To dgvCoef.RowCount - 1
                For col As Integer = 0 To dgvCoef.ColumnCount - 1
                    Dim strItem As String = String.Format("Coef{0}{1}", row + 1, col + 1)
                    dgvCoef.Item(col, row).Value = data(0).Item(strItem)
                    dgvCoef.Item(col, row).ReadOnly = False
                    dgvCoef.Item(col, row).Style.BackColor = Color.White
                Next
            Next

            ChangeCoefRange()

            If Math.Abs(CalcNomalizeCoef() - CDbl(txtNormalizeCoef.Text.Trim)) > SIGMA Then
                ' 2019/10/11 正規化係数を手入力可能とする(輝度値が閾値0-255を超えるため) Red→LightPink
                '表示している正規化係数が計算値と異なる
                txtNormalizeCoef.BackColor = Color.LightPink
            Else

            End If

            bRet = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "表示エラー [" & ex.Message & "] " & Me.Text)

        Finally

        End Try

        Return bRet

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As tCnt_DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False                                         ' セルエラーの表示
            .ShowCellToolTips = False                                       ' ツールヒントの表示
            .ShowEditingIcon = False                                        ' 編集中のセルの明示
            .ShowRowErrors = False                                          ' エラー行の明示
            .ScrollBars = ScrollBars.None                                   ' スクロールバーの表示
            .ColumnHeadersHeight = 28                                       ' ヘッダーの高さ

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                               ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersVisible = True                                       ' 行ヘッダ表示
            .RowHeadersWidth = 70
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整なし
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整なし

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            ' 行ヘッダー 文字サイズ変更,文字色変更
            .RowHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowHeadersDefaultCellStyle.ForeColor = Color.Black
            .RowHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

        End With

    End Sub

    ''' <summary>
    '''加重値情報dgv初期化
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeCoef(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        ' 動作定義
        DgvInit(dgv)
        dgv.RowHeadersWidth = 40

        With dgv
            ' 列定義
            For col As Integer = 1 To 7
                Using clm As New DataGridViewTextBoxColumn
                    clm.HeaderText = col.ToString
                    clm.Width = 100
                    clm.Frozen = True
                    clm.ReadOnly = False
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                    .Columns.Add(clm)
                End Using
            Next

            '行定義
            .RowCount = 7
            For row As Integer = 1 To .RowCount
                .Rows(row - 1).HeaderCell.Value = row.ToString
                .Rows(row - 1).Height = 28
            Next

        End With

    End Sub

    ''' <summary>
    ''' カーネルサイズ選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSizeX_SelectedValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' カーネルサイズXのイベントではない
        If Not cmbSizeX.Equals(CType(sender, ComboBox)) Then Exit Sub

        'コンボボックスの設定により、異常ラベルは不可視にする
        lblSizeX.Visible = False

        ' カーネルサイズYの表示を変更
        lblSizeY.Text = cmbSizeX.Text
        lblSizeY.BackColor = Color.White

        HandlerEnable(False)

        ChangeCoefRange()

        HandlerEnable(True)

        ' 正規化係数を再計算した値に変更
        txtNormalizeCoef.Text = CalcNomalizeCoef.ToString
        txtNormalizeCoef.BackColor = Color.White

    End Sub

    ''' <summary>
    ''' 加重値領域変更
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeCoefRange()

        Dim nSize As Integer = CInt(cmbSizeX.SelectedValue)
        For row As Integer = 0 To dgvCoef.RowCount - 1
            For col As Integer = 0 To dgvCoef.ColumnCount - 1
                With dgvCoef.Item(col, row)
                    If nSize < row + 1 Or nSize < col + 1 Then
                        '範囲外の加重値は入力不可
                        .Value = 0
                        .ReadOnly = True
                        .Style.BackColor = Color.DarkGray
            Else
                        .ReadOnly = False
                        .Style.BackColor = Color.White
            End If
                End With
        Next
        Next
        dgvCoef.CurrentCell = Nothing

    End Sub

    ''' <summary>
    ''' 正規化係数を計算
    ''' </summary>
    ''' <returns>正規化係数</returns>
    ''' <remarks></remarks>
    Private Function CalcNomalizeCoef() As Double

        Dim dAns As Double = 0
        Dim bCoef As Boolean = False

        For row As Integer = 0 To dgvCoef.RowCount - 1
            For col As Integer = 0 To dgvCoef.ColumnCount - 1
                With dgvCoef.Item(col, row)
                    If .ReadOnly Then
                        Continue For
                    End If
                    Dim d As Double
                    If Not Double.TryParse(CStr(.Value), d) Then
                        Continue For
                    End If
                    dAns = dAns + d
                    bCoef = True
                End With
            Next
        Next

        If bCoef AndAlso Math.Abs(dAns) < SIGMA Then
            '加重値合計が0の場合は、正規化係数を1とする。
            Return 1
        End If

        Return dAns

    End Function

    ''' <summary>
    ''' 加重値変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvCoef_CellValueChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)

        If Not dgvCoef.Equals(CType(sender, tCnt_DataGridView)) Then Exit Sub

        Dim dCoef As Double = CalcNomalizeCoef()

        txtNormalizeCoef.Text = dCoef.ToString
        txtNormalizeCoef.BackColor = Color.White
        If Math.Abs(dCoef - 1) > SIGMA Then
            '正規化係数が1以外のときはオフセット"0"を自動表示
            txtOffset.Text = "0"
        End If
    End Sub


    ''' <summary>
    ''' イベントハンドラ設定
    ''' </summary>
    ''' <param name="bEnable"></param>
    ''' <remarks></remarks>
    Private Sub HandlerEnable(ByVal bEnable As Boolean)
        If bEnable Then
            AddHandler cmbSizeX.SelectedValueChanged, AddressOf cmbSizeX_SelectedValueChanged
            AddHandler cmbId.SelectedValueChanged, AddressOf cmbId_SelectedValueChanged
            AddHandler dgvCoef.CellValueChanged, AddressOf dgvCoef_CellValueChanged
        Else
            RemoveHandler cmbSizeX.SelectedValueChanged, AddressOf cmbSizeX_SelectedValueChanged
            RemoveHandler cmbId.SelectedValueChanged, AddressOf cmbId_SelectedValueChanged
            RemoveHandler dgvCoef.CellValueChanged, AddressOf dgvCoef_CellValueChanged
        End If
    End Sub

    ''' <summary>
    ''' コンボBOX(ID)変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbId_SelectedValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' 今の選択と主キーが同じ場合は無視 (選択して入力異常だったので 元に戻したよ の時の対策)
        If m_nSelectIdx = cmbId.SelectedIndex Then Exit Sub

        HandlerEnable(False)

        Try
            If 0 <= m_nSelectIdx Then
                ' 入力値チェック
                If Not Data_Check() Then
                    ' 入力異常時は、選択位置も戻す
                    cmbId.SelectedIndex = m_nSelectIdx
                    Exit Try
                End If
                ' 画面データ取り込み
                If Not Data_Get() Then
                    ' 取り込み異常時は、選択位置も戻す
                    cmbId.SelectedIndex = m_nSelectIdx
                    Exit Try
                End If
            End If

            '表示
            m_nSelectIdx = cmbId.SelectedIndex
            Data_Disp()

        Catch ex As Exception
        Finally
            HandlerEnable(True)
        End Try

    End Sub


    ''' <summary>
    ''' 新規追加
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRollAdd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAdd.Click


        HandlerEnable(False)

        Try
            If 0 <= m_nSelectIdx Then
                ' 新規追加する前に、現在表示しているデータを取り込む
                If Not Data_Check() Then
                    Exit Try
                End If
                If Not Data_Get() Then
                    Exit Try
                End If
            End If

            Data_Add()

            ComboSet_ID()

            Data_Disp()

        Catch ex As Exception
        Finally
            HandlerEnable(True)
        End Try

    End Sub

    ''' <summary>
    ''' 削除
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRolldel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDel.Click

        Dim nId As Integer = CInt(cmbId.SelectedValue)

        If Not CheckUseID(nId) Then
            'そもそも削除できない
            Exit Sub
                    End If

        ' 削除確認
        If MsgBox("表示中のフィルター情報を削除しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "削除確認") = MsgBoxResult.No Then Exit Sub

        HandlerEnable(False)

        Try
            ''フィルター情報の削除
            Data_Del(nId)

            'コンボボックス設定前に選択ロールを一つ前に切り替えておく
            m_nSelectIdx = m_nSelectIdx - 1

            'コンボボックス設定
            ComboSet_ID()

            'データ表示
            Call Data_Disp()

        Catch ex As Exception
        Finally
            HandlerEnable(True)
        End Try

    End Sub

    ''' <summary>
    ''' カーネルサイズの整合性確認
    ''' </summary>
    ''' <param name="nSize">カーネルサイズ</param>
    ''' <returns>TRUE:OK、FALSE:NG</returns>
    ''' <remarks></remarks>
    Private Function CheckCmbSizeValue(ByVal nSize As Integer) As Boolean


        ' コンボボックスに登録されているデータテーブルを確認する
        If cmbSizeX.DataSource Is Nothing Then
            ' データソースなし
            Return False
        End If

        Dim dt As DataTable = CType(cmbSizeX.DataSource, DataTable)
        For ii As Integer = 0 To dt.Rows.Count - 1
            If CInt(dt.Rows(ii).Item("VALUE")) = nSize Then
                'サイズ値が登録されている
                Return True
            End If
        Next

        ' サイズが見つからない
        Return False
    End Function

    ''' <summary>
    ''' フィルターID取得
    ''' </summary>
    ''' <param name="nIdx">選択インデックス</param>
    ''' <param name="nVal">フィルターID</param>
    ''' <returns>TRUE:OK、FALSE:NG</returns>
    ''' <remarks></remarks>
    Private Function GetCmbIdValue(ByVal nIdx As Integer, ByRef nVal As Integer) As Boolean

        ' コンボボックスに登録されているデータテーブルを確認する
        If cmbId.DataSource Is Nothing Then
            ' データソースなし
            Return False
        End If

        Dim dt As DataTable = CType(cmbId.DataSource, DataTable)
        If 0 > nIdx Or dt.Rows.Count - 1 < nIdx Then
            Return False
        End If

        nVal = CInt(dt.Rows(nIdx).Item("VALUE"))
        Return True

    End Function

End Class
