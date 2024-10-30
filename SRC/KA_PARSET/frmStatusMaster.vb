'*********************************************************************************
' 機器マスター設定画面
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

''' <summary>
''' 機器マスター設定画面
''' </summary>
''' <remarks></remarks>
Public Class frmStatusMaster
	'---------------------------------------------------------------------------
	' 列挙体定義
	'---------------------------------------------------------------------------
	''' <summary>
	''' 機器マスター設定列
	''' </summary>
	''' <remarks></remarks>
	Private Enum EM_COL_DGVMST
        emTxt_ID = 0                                  ' テキストボックス　機器ID
        emTxt_Name                                    ' テキストボックス　機器名
        emTxt_Inival                                  ' テキストボックス　初期値
        emCmb_Eerrscale                               ' コンボボックス　  異常範囲
        emCmb_Err_Kensa                               ' コンボボックス　  検査異常度
        emCmb_Err_Kiki                                ' コンボボックス　  機器異常度
        emTxt_Priority                                ' テキストボックス　優先度
		emTxt_Err_Msg								  ' テキストボックス　異常MSG
	End Enum

	'---------------------------------------------------------------------------
	' 構造体定義
	'---------------------------------------------------------------------------
	''' <summary>
	''' 機器マスター表示データ
	''' </summary>
	''' <remarks></remarks>
	Private Structure STATUS_DISP_VALUE
		Dim strName As String
		Dim oDBValue As Object
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
		Public Sub SetValue(ByVal name As String, ByVal value As Object, ByVal fore As Color, ByVal back As Color)
			strName = name
			oDBValue = value
			nForeClr = fore
			nBackClr = back
		End Sub
	End Structure

	'---------------------------------------------------------------------------
	' 定数定義
	'---------------------------------------------------------------------------
    Private m_stInitVal() As STATUS_DISP_VALUE                     ' 初期値
    Private m_stErrScale() As STATUS_DISP_VALUE                    ' 異常範囲
    Private m_stKensaErr() As STATUS_DISP_VALUE                    ' 検査異常度
    Private m_stKikiErr() As STATUS_DISP_VALUE                     ' 機器異常度

	'---------------------------------------------------------------------------
	' メンバ変数
    '---------------------------------------------------------------------------
    Private m_Adapter As New SqlClient.SqlDataAdapter              ' アダプターType
    Private m_DataSet As New DataSet                               ' 非接続型テーブルTYPE_NAME

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
	Private Sub frmStatusMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' ファンクションボタン有効項目のみ
		If Not enable Then Exit Sub

		'' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "機器マスター設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
	Private Sub frmStatusMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' いろいろ開放処理
		m_DataSet.Dispose()
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
	Private Sub frmStatusMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' メニューリスト生成
        modMain.SetLabelSysName(Me)                                 ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmStatusMaster          ' 機器マスター設定画面

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        Data_Init()                                                             ' 表示データ初期化
		dgvInitialize(dgvData)													' 疵種dgv初期化

        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       ' 終了
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
		' 初期値の表示情報作成
		'-----------------------------------------------------------------------
		ReDim m_stInitVal(3 - 1)
		m_stInitVal(0).SetValue("正常", 0, Color.Empty, Color.Empty)
		m_stInitVal(1).SetValue("異常", 1, Color.Empty, Color.Empty)
		m_stInitVal(2).SetValue("変更無し", DBNull.Value, Color.Empty, Color.Empty)

		'-----------------------------------------------------------------------
		' 異常範囲の表示情報作成
		'-----------------------------------------------------------------------
		ReDim m_stErrScale(4 - 1)
		m_stErrScale(0).SetValue("RAS画面のみ表示", -1, Color.Empty, Color.Empty)
		m_stErrScale(1).SetValue("全体", 0, Color.Empty, Color.Empty)
		m_stErrScale(2).SetValue("表面", 1, Color.Empty, Color.Empty)
		m_stErrScale(3).SetValue("裏面", 2, Color.Empty, Color.Empty)

		'-----------------------------------------------------------------------
		' 検査異常度の表示情報作成
		'-----------------------------------------------------------------------
		ReDim m_stKensaErr(6 - 1)
		m_stKensaErr(0).SetValue("正常", 0, Color.Yellow, g_ColorKensaOk)
		m_stKensaErr(1).SetValue("縮退", 1, Color.Yellow, g_ColorKensaNg)
		m_stKensaErr(2).SetValue("エッジ不正", 4, Color.Yellow, g_ColorKensaEdgeNg)
		m_stKensaErr(3).SetValue("未保存", 5, Color.Yellow, g_ColorKensaDbNon)
		m_stKensaErr(4).SetValue("未検査", 6, Color.Yellow, g_ColorKensaSkip)
        m_stKensaErr(5).SetValue("停止", 9, Color.White, g_ColorKensaStop)

        '-----------------------------------------------------------------------
        ' 機器異常度の表示情報作成
        '-----------------------------------------------------------------------
        ReDim m_stKikiErr(3 - 1)
        m_stKikiErr(0).SetValue("正常", 0, Color.Yellow, g_ColorKikiOk)
		m_stKikiErr(1).SetValue("軽故障", 1, Color.Black, g_ColorKikiKei)
		m_stKikiErr(2).SetValue("重故障", 3, Color.Black, g_ColorKikiJyu)

    End Sub

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
            ' 機器IDと機器名
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_ID, ii).Value = row.Item("機器ID")
            dgvData(EM_COL_DGVMST.emTxt_Name, ii).Value = row.Item("機器名")

            '-------------------------------------------------------------------
            ' 初期値
            '-------------------------------------------------------------------
            If row.Item("初期値") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value = row.Item("初期値")
            End If

            '-------------------------------------------------------------------
            ' 異常範囲
            '-------------------------------------------------------------------
            For Each dsp As STATUS_DISP_VALUE In m_stErrScale
                If dsp.oDBValue.Equals(row.Item("異常範囲")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Eerrscale, ii).Value = dsp.strName
                End If
            Next dsp

            '-------------------------------------------------------------------
            ' 検査異常度
            '-------------------------------------------------------------------
			For Each dsp As STATUS_DISP_VALUE In m_stKensaErr
				If dsp.oDBValue.Equals(row.Item("検査異常度")) = True Then
					dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, ii).Value = dsp.strName
				End If
			Next dsp


            '-------------------------------------------------------------------
            ' 機器異常度
            '-------------------------------------------------------------------
            For Each dsp As STATUS_DISP_VALUE In m_stKikiErr
                If dsp.oDBValue.Equals(row.Item("機器異常度")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, ii).Value = dsp.strName
                End If
            Next dsp

            '-------------------------------------------------------------------
            ' 優先度
            '-------------------------------------------------------------------
            If row.Item("優先度") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value = row.Item("優先度")
            End If

			'-------------------------------------------------------------------
            ' 異常MSG
            '-------------------------------------------------------------------
            If row.Item("異常MSG") Is DBNull.Value Then
                dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value = row.Item("異常MSG")
            End If

        Next ii
    End Sub

    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""

        '-----------------------------------------------------------------------
        ' 機器マスター取得文作成
        '-----------------------------------------------------------------------
        strSQL = "SELECT 機器ID"
        strSQL &= ", 機器名"
        strSQL &= ", 初期値"
        strSQL &= ", 異常範囲"
        strSQL &= ", 検査異常度"
        strSQL &= ", 機器異常度"
        strSQL &= ", 優先度"
		strSQL &= ", 異常MSG"
		strSQL &= " FROM " & DB_STATUS_MASTER
        strSQL &= " ORDER BY 機器ID ASC"

        '-----------------------------------------------------------------------
        ' 機器マスターテーブル取得
        '-----------------------------------------------------------------------
        Dim strErrMsg As String = ""
        If 0 > MyBase.Get_SelectData(strSQL, m_DataSet, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

	''' <summary>
	''' 画面資源の破棄イベント
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmStatusMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		If g_nDspfrmID = MyBase.tProp_MyFrmID Then
			modMain.TaskEnd()													'現在の表示中の画面が自分の画面ならアプリケーションを終了
		End If
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
                frmMsg.SetSelectedFNo(0)

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

        ' 画面に値を表示
        Data_Disp()

    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim nKikiId As Integer = 0

        ' ログ表示件数分ループ
        For ii As Integer = 0 To dgvData.RowCount - 1
            nKikiId = CInt(dgvData(EM_COL_DGVMST.emTxt_ID, ii).Value)

            Dim strValIni As String = CStr(dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strValIni, -999, 999, True, "機器ID=" & nKikiId & " 初期値") <> 0 Then
                Return False
            End If
            dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value = strValIni

            Dim strVal As String = CStr(dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strVal, 1, 999, True, "機器ID=" & nKikiId & " 優先度") <> 0 Then
                Return False
            End If
            dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value = strVal

            If Not Nothing Is dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value Then
                Dim strWk As String = CStr(dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "機器ID=" & nKikiId & " 異常MSG") <> 0 Then Return False
                dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value = strWk
            End If
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
            ' 初期値
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value)
            If strVal.Length > 0 Then
                row.Item("初期値") = CInt(strVal)
            Else
                row.Item("初期値") = DBNull.Value
            End If

            '-----------------------------------------------------------------------------
            ' 異常範囲
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Eerrscale, ii).Value)
            For Each dsp As STATUS_DISP_VALUE In m_stErrScale
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("異常範囲") = dsp.oDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' 検査異常度
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, ii).Value)
            For Each dsp As STATUS_DISP_VALUE In m_stKensaErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("検査異常度") = dsp.oDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' 機器異常度
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, ii).Value)
            For Each dsp As STATUS_DISP_VALUE In m_stKikiErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("機器異常度") = dsp.oDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' 優先度
            '-----------------------------------------------------------------------------
            If dgvData(EM_COL_DGVMST.emCmb_Eerrscale, ii).Value Is Nothing = True Then
                row.Item("優先度") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value)
                If strVal.Length > 0 Then
                    row.Item("優先度") = CInt(strVal)
                Else
                    row.Item("優先度") = DBNull.Value
                End If
            End If

			'-----------------------------------------------------------------------------
            ' 異常MSG
            '-----------------------------------------------------------------------------
            If dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value Is Nothing Then
                row.Item("異常MSG") = DBNull.Value
            Else
                If dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value.ToString = "" Then
                    row.Item("異常MSG") = DBNull.Value
                Else
                    row.Item("異常MSG") = dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value.ToString
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
            modMain.Send_Main(CH_PARAM_SETTIG_STATUS)                   ' 機器状態

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
        ' 機器ID
		dgvColSetText(dgv, "機器ID", 65, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, "")

        ' 機器名
		dgvColSetText(dgv, "機器名", 280, False, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' 初期値
        dgvColSetText(dgv, "初期値" & vbNewLine & "※1", 65, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' 異常範囲
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "異常範囲"
            clm.Width = 150
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As STATUS_DISP_VALUE In m_stErrScale
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 検査異常度
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "検査" & vbNewLine & "異常度"
			clm.Width = 120
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As STATUS_DISP_VALUE In m_stKensaErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 機器異常度
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "機器" & vbNewLine & "異常度"
            clm.Width = 86
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As STATUS_DISP_VALUE In m_stKikiErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 優先度
        dgvColSetText(dgv, "優先度" & vbNewLine & "※2", 65, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

		' 異常MSG
        Using clm As New DataGridViewTextBoxColumn
            clm.HeaderText = "異常MSG"
            clm.Width = 512
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.ReadOnly = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.MaxInputLength = 256
            dgv.Columns.Add(clm)
        End Using

    End Sub

	''' <summary>
	''' dgvのセルが選択されたときにコンボボックスならドロップダウンする
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks>%{DOWN}でのドロップダウン操作が、開発PCだと出来ない、原因不明</remarks>
	Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)                      ' 対象のdgv
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then    ' 列のタイプがコンボボックスか？
            SendKeys.Send("%{DOWN}")                                               ' ドロップダウン表示
        End If
    End Sub

    ''' <summary>
    ''' セルの値が変更されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged
        Dim nRow As Integer = e.RowIndex
        Dim nCol As Integer = e.ColumnIndex
        Dim strVal As String = CStr(dgvData(nCol, nRow).Value)

        If nCol = EM_COL_DGVMST.emCmb_Err_Kensa Then                               ' 検査異常度列の場合
            For ii As Integer = 0 To m_stKensaErr.Length - 1
                Dim dsp As STATUS_DISP_VALUE = m_stKensaErr(ii)
                If dsp.strName.CompareTo(strVal) = 0 Then
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, nRow).Style.ForeColor = m_stKensaErr(ii).nForeClr
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, nRow).Style.BackColor = m_stKensaErr(ii).nBackClr
                End If
            Next ii
        End If

        If nCol = EM_COL_DGVMST.emCmb_Err_Kiki Then                                ' 機器異常度列の場合
            For ii As Integer = 0 To m_stKikiErr.Length - 1
                Dim dsp As STATUS_DISP_VALUE = m_stKikiErr(ii)
                If dsp.strName.CompareTo(strVal) = 0 Then
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, nRow).Style.ForeColor = m_stKikiErr(ii).nForeClr
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, nRow).Style.BackColor = m_stKikiErr(ii).nBackClr
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
