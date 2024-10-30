'*********************************************************************************
' ログマスター設定画面
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

''' <summary>
''' ログマスター設定画面
''' </summary>
''' <remarks></remarks>
Public Class frmLogMaster
	'---------------------------------------------------------------------------
	' 列挙体定義
	'---------------------------------------------------------------------------
	''' <summary>
    ''' ログマスター設定列
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_DGVMST
        emTxt_Alarm_No = 0                      ' テキストボックス　 アラームNo
        emCmb_Syubetsu                          ' コンボボックス　　 種別
        emCmb_Kubun                             ' コンボボックス     区分
		emChk_GaidUmu							' チェックボックス   オペガイ有無
		emTxt_Gaidtime							' テキストボックス   オペガイ時間
		emTxt_Naiyou							' テキストボックス   内容
        emTxt_Syousai                           ' テキストボックス   詳細
    End Enum

    '---------------------------------------------------------------------------
    ' 構造体定義
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' ログマスター表示データ
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure LOG_DISP_VALUE
        Dim strName As String
        Dim nDBValue As Integer
        Dim nForeClr As Color
        Dim nBackClr As Color

        ''' <summary>
        ''' データセット
        ''' </summary>
        ''' <param name="name">表示データ名</param>
        ''' <param name="value">DB上の値</param>
        ''' <param name="fore">表示文字色</param>
        ''' <param name="back">表示背景色</param>
        ''' <remarks></remarks>
        Public Sub SetValue(ByVal name As String, ByVal value As Integer, ByVal fore As Color, ByVal back As Color)
            strName = name
            nDBValue = value
            nForeClr = fore
            nBackClr = back
        End Sub
    End Structure

    '---------------------------------------------------------------------------
    ' 定数定義
    '---------------------------------------------------------------------------
    Private m_stSyubetsuErr() As LOG_DISP_VALUE             ' 種別
    Private m_stKubunErr() As LOG_DISP_VALUE                ' 区分

    '---------------------------------------------------------------------------
    ' メンバ変数
    '---------------------------------------------------------------------------
    Private m_Adapter As New SqlClient.SqlDataAdapter       ' アダプターType
    Private m_DataSet As New DataSet                        ' 非接続型テーブルTYPE_NAME

    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------

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
    Private Sub frmLogMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "ログマスター設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmLogMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DataSet.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmLogMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmLogMaster             ' ログマスター設定画面

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        Data_Init()                                                             ' 表示データ初期化
        dgvInitialize(dgvData)                                                  ' 疵種dgv初期化

        If Not DB_Select() Then                                                 ' ＤＢの値取得失敗なら終了
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                    ' 終了
        End If

        Data_Disp()                                                             ' 画面に値を表示

        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)           ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い

        Call UpdateEnabled(g_bPassFlg)                                          ' 更新ボタンの有効/無効

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
    ''' 表示データの初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()

        '-----------------------------------------------------------------------
        ' 種別の表示情報作成
        '-----------------------------------------------------------------------
        ReDim m_stSyubetsuErr(3 - 1)
		m_stSyubetsuErr(0).SetValue("全般", 1, Color.Yellow, g_ColorKikiOk)
        m_stSyubetsuErr(1).SetValue("ソフトウェア", 2, Color.Black, g_ColorKikiKei)
        m_stSyubetsuErr(2).SetValue("ハードウェア", 3, Color.Black, g_ColorKikiJyu)

        '-----------------------------------------------------------------------
        ' 区分の表示情報作成
        '-----------------------------------------------------------------------
		ReDim m_stKubunErr(4 - 1)
		m_stKubunErr(0).SetValue("イベント", 1, Color.Yellow, g_ColorKikiOk)
		m_stKubunErr(1).SetValue("重故障", 2, Color.Black, g_ColorKikiJyu)
		m_stKubunErr(2).SetValue("軽故障", 3, Color.Black, g_ColorKikiKei)
		m_stKubunErr(3).SetValue("コメント", 4, Color.Black, Color.White)

    End Sub

    ''' <summary>
    ''' 画面資源の破棄イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmLogMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then
            modMain.TaskEnd()                         '現在の表示中の画面が自分の画面ならアプリケーションを終了
        End If
    End Sub

    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""

        '-----------------------------------------------------------------------
        ' ログマスター取得文作成
        '-----------------------------------------------------------------------
        strSQL = "SELECT アラームNo"
        strSQL &= ", 種別, 区分"
		strSQL &= ", オペガイ有無, オペガイ時間"
		strSQL &= ", 内容, 詳細"
		strSQL &= " FROM " & DB_LOG_MASTER
		strSQL &= " ORDER BY アラームNo ASC"

        '-----------------------------------------------------------------------
        ' ログマスターテーブル取得
        '-----------------------------------------------------------------------
        Dim strErrMsg As String = ""
        If 0 > MyBase.Get_SelectData(strSQL, m_DataSet, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LOG_MASTER & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim rows() As DataRow = m_DataSet.Tables(0).Select()
        dgvData.RowCount = rows.Length
        For ii As Integer = 0 To rows.Length - 1
            Dim row As DataRow = rows(ii)
            '-------------------------------------------------------------------
            ' アラームNo
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_Alarm_No, ii).Value = row.Item("アラームNo")

            '-------------------------------------------------------------------
            ' 種別
            '-------------------------------------------------------------------
            For Each dsp As LOG_DISP_VALUE In m_stSyubetsuErr
                If dsp.nDBValue.Equals(row.Item("種別")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Syubetsu, ii).Value = dsp.strName
                End If
            Next dsp

            '-------------------------------------------------------------------
            ' 区分
            '-------------------------------------------------------------------
            For Each dsp As LOG_DISP_VALUE In m_stKubunErr
                If dsp.nDBValue.Equals(row.Item("区分")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Kubun, ii).Value = dsp.strName
                End If
            Next dsp

			'-------------------------------------------------------------------
			' オペガイ有無
            '-------------------------------------------------------------------
            If row.Item("オペガイ有無") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = False
            Else
                Dim nWK As Integer = CInt(row.Item("オペガイ有無"))
                If 1 = nWK Then
                    dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = True
                Else
                    dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = False
                End If
                'dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value = row.Item("オペガイ有無")
            End If

            '-------------------------------------------------------------------
            ' オペガイ時間
            '-------------------------------------------------------------------
            If row.Item("オペガイ時間") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value = row.Item("オペガイ時間")
            End If
            '-------------------------------------------------------------------
            ' 内容
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value = row.Item("内容")

            '-------------------------------------------------------------------
            ' 詳細
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value = row.Item("詳細")

        Next ii
    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check() Then Exit Sub

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' 画面のデータを取得
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

            ' このフォームで必要なデータを取得する
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                frmMsg.Dispose()
                modMain.TaskEnd()                       '終了
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "確定処理エラー [" & ex.Message & "] " & Me.Text)
        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        '' 画面に値を表示
        Data_Disp()

    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String
        Dim bNullChk As Boolean
        Dim nAlarmNo As Integer = 0

        ' アラームNo分ループ
        For ii As Integer = 0 To dgvData.RowCount - 1
            nAlarmNo = CInt(dgvData(EM_COL_DGVMST.emTxt_Alarm_No, ii).Value)

            If Not Nothing Is dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value Then        ' 内容の文字数チェック
                strWk = dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value.ToString
                If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 128, "アラームNo=" & nAlarmNo & " 内容") <> 0 Then Return False
                dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value = strWk
            End If

            If Not Nothing Is dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value Then        ' 詳細の文字数チェック
                strWk = dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value.ToString
                If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "アラームNo=" & nAlarmNo & " 詳細") <> 0 Then Return False
                dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value = strWk
            End If

            ' オペガイ時間
            ' オペガイ有無により NULL許可　を判定する
            If CBool(dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value) = True Then
                'オペガイ有の場合、NULL許可しない
                bNullChk = False
            Else
                'オペガイ無しの場合、NULL許可
                bNullChk = True
            End If

            strWk = CStr(dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value)
            'HACK 2015/02/18　アラームNOを表示
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 10, 172800, bNullChk, "アラームNo=" & nAlarmNo & " オペガイ時間") <> 0 Then Return False

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' 入力情報をデータセットに設定する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim rows() As DataRow = m_DataSet.Tables(0).Select()
        For ii As Integer = 0 To rows.Length - 1
            Dim row As DataRow = rows(ii)
            Dim strVal As String = ""

            '-----------------------------------------------------------------------------
            ' 種別
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Syubetsu, ii).Value)
            For Each dsp As LOG_DISP_VALUE In m_stSyubetsuErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("種別") = dsp.nDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' 区分
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Kubun, ii).Value)
            For Each dsp As LOG_DISP_VALUE In m_stKubunErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("区分") = dsp.nDBValue
                    Exit For
                End If
            Next dsp

			'-------------------------------------------------------------------
			' オペガイ有無
            '-------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value Then
                row.Item("オペガイ有無") = DBNull.Value
            Else
                If CBool(dgvData(EM_COL_DGVMST.emChk_GaidUmu, ii).Value) Then
                    row.Item("オペガイ有無") = 1                                              ' 有効
                Else
                    row.Item("オペガイ有無") = 0                                              ' 無効
                End If
            End If
            '-------------------------------------------------------------------
            ' オペガイ時間
            '-------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value Then
                row.Item("オペガイ時間") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value)
                If "" = strVal Then
                    row.Item("オペガイ時間") = DBNull.Value
                Else
                    row.Item("オペガイ時間") = dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value
                End If
            End If
            'row.Item("オペガイ時間") = dgvData(EM_COL_DGVMST.emTxt_Gaidtime, ii).Value
            '-----------------------------------------------------------------------------
            ' 内容
            '-----------------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value Then
                row.Item("内容") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Naiyou, ii).Value)
                If strVal.Length > 0 Then
                    row.Item("内容") = strVal
                Else
                    row.Item("内容") = DBNull.Value
                End If
            End If

            '-----------------------------------------------------------------------------
            ' 詳細
            '-----------------------------------------------------------------------------
            If IsDBNull(dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value) Or _
                Nothing Is dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value Then
                row.Item("詳細") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Syousai, ii).Value)
                If strVal.Length > 0 Then
                    row.Item("詳細") = strVal
                Else
                    row.Item("詳細") = DBNull.Value
                End If
            End If

        Next ii
    End Sub

    ''' <summary>
    ''' アップデート
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   'トランザクション処理

        Try
            ' DB接続
            m_Adapter.SelectCommand.Connection.Open()

            'トランザクション開始
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using builder As New SqlClient.SqlCommandBuilder()
                builder.DataAdapter = m_Adapter
                m_Adapter.Update(m_DataSet)                             ' 変更分をアップデートする
            End Using

            'コミット
            transact.Commit()

            '統括機能へパラメータ変更通知
            modMain.Send_Main(CH_PARAM_SETTIG_LOG)                      ' シスログ
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
		Call dgv.DataGridViewSelectionCancel(dgv)							' 選択色無し

		With dgv
			'-------------------------------------------------------------------
			' 基本動作定義
			'-------------------------------------------------------------------
			.ColumnHeadersHeight = 42										' ヘッダーの高さ
			.ScrollBars = ScrollBars.Both									' スクロールバーの表示

			' 列ヘッダー 文字サイズ変更,文字色変更
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		'選択色
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1行おきに色替え
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		'一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

			.AllowUserToResizeColumns = True											' ユーザー 列サイズ変更 可能とする

        End With

        '-------------------------------------------------------------------
        ' 列定義
        '-------------------------------------------------------------------
        ' アラームNo
		dgvColSetText(dgv, "ｱﾗｰﾑ" & vbNewLine & "No", 60, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 種別
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "種別"
			clm.Width = 140
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As LOG_DISP_VALUE In m_stSyubetsuErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            dgv.Columns.Add(clm)
        End Using

        ' 区分
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "区分"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As LOG_DISP_VALUE In m_stKubunErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

		' オペガイ有無
        dgvColSetCheck(dgv, "ｵﾍﾟｶﾞｲ" & vbNewLine & "有無※1", 80, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)


        ' オペガイ時間
		dgvColSetText(dgv, "ｵﾍﾟｶﾞｲ" & vbNewLine & "時間[秒]", 80, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 内容
		dgvColSetText(dgv, "内容", 285, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' 詳細
		dgvColSetText(dgv, "詳細", 285, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

    End Sub

    ''' <summary>
    ''' dgvのセルが選択されたときにコンボボックスならドロップダウンする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}でのドロップダウン操作が、開発PCだと出来ない、原因不明</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)                     ' 対象のdgv
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then   ' 列のタイプがコンボボックスかどうか
            SendKeys.Send("%{DOWN}")                                              ' ドロップダウン表示
        End If
    End Sub

    ''' <summary>
    ''' セルの値が変更されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged

        ' 列ヘッダー、行ヘッダー変更時は処理無し
        If e.RowIndex < 0 Then Exit Sub
        If e.ColumnIndex < 0 Then Exit Sub

        Dim nRow As Integer = e.RowIndex
        Dim nCol As Integer = e.ColumnIndex

        ' NULL値は処理無し
        If IsDBNull(dgvData(nCol, nRow).Value) Then
            Exit Sub
        End If

        Dim strVal As String = CStr(dgvData(nCol, nRow).Value)

        If nCol = EM_COL_DGVMST.emCmb_Kubun Then                                ' 区分の場合
            For ii As Integer = 0 To m_stKubunErr.Length - 1
                Dim dsp As LOG_DISP_VALUE = m_stKubunErr(ii)
                ' 区分変更時の色替え処理
                If dsp.strName.CompareTo(strVal) = 0 Then
                    dgvData(EM_COL_DGVMST.emCmb_Kubun, nRow).Style.ForeColor = m_stKubunErr(ii).nForeClr
                    dgvData(EM_COL_DGVMST.emCmb_Kubun, nRow).Style.BackColor = m_stKubunErr(ii).nBackClr
                End If
            Next ii
        End If
    End Sub

    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                                            ' ファンクションキーの設定ボタン

    End Sub

End Class
