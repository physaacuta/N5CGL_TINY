Imports tClassLibrary

Public Class frmDspSetting

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー構造体
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 設定
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_SETTING
        Dim nSelectMode As Integer          ' 0:直近コイル 1:期間指定

        Dim nDspType() As Integer           ' 選択した疵種(EdasysID) GC_MAX_SELECT_TYPE
        Dim sDspType() As String            ' そのときの名前
        Dim nDspGrade() As Integer          ' 選択したグレード(EdasysID) 
        Dim sDspGrade() As String           ' そのときの名前


        Dim nXmax As Integer                ' グラフX軸 最大値
        Dim nXinterval As Integer           ' グラフX軸 メモリ間隔
        Dim nYmax As Integer                ' グラフY軸 最大値
        Dim nYinterval As Integer           ' グラフY軸 メモリ間隔

        Dim colorLine() As Color            ' 線の色

        ' ちょっと特殊
        Dim daySelectS As Date              ' 期間(開始)
        Dim daySelectE As Date              ' 期間(終了)


        'イニシャライズ
        Public Sub initialize()
            ReDim nDspType(GC_MAX_SELECT_TYPE - 1)
            ReDim sDspType(GC_MAX_SELECT_TYPE - 1)
            ReDim nDspGrade(MAX_GRADE - 1)
            ReDim sDspGrade(MAX_GRADE - 1)
            ReDim colorLine(GC_MAX_SELECT_TYPE - 1)
        End Sub
    End Structure

    '疵種・グレードフィルター列名
    Public Enum EM_FILTER_COL                       ' 疵種
        emCheck = 0                                 ' チェックボックス
        emName                                      ' 名称
        emEdasysID                                  ' EdasysID (主キー)
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_typSetting As TYP_SETTING                     ' 設定情報



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 設定反映
    ''' </summary>
    ''' <remarks></remarks>
    Public Shadows Event EvDsp()


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部アクセス
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 設定条件取得
    ''' </summary>
    Public ReadOnly Property tprop_TypSetting() As TYP_SETTING
        Get
            Return m_typSetting
        End Get
    End Property

    ''' <summary>
    ''' 画面表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Disp()
        '''' 初期化
        Call m_typSetting.initialize()

        '''' 初期表示
        Call DspRefresh()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////


    ''' <summary>
    ''' イニシャライズ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        '''' フィルター用 グリッドビュー
        Call dgvInitialize_FilterGrid(dgvDispType)
        Call dgvInitColumn_FilterGrid(dgvDispType, "疵種名称")
        Call dgvFilter_AddHandler(dgvDispType)
        Call SetFilterData(dgvDispType, DB_TYPE_NAME)                               ' データ表示
        btnTypeAllCancel.Tag = dgvDispType

        Call dgvInitialize_FilterGrid(dgvDispGrade)
        Call dgvInitColumn_FilterGrid(dgvDispGrade, "グレード名称")
        Call dgvFilter_AddHandler(dgvDispGrade)
        Call SetFilterData(dgvDispGrade, DB_GRADE_NAME)
        btnGradeAllSelect.Tag = dgvDispGrade
        btnGradeAllCancel.Tag = dgvDispGrade

        '''' 日付データ
        m_typSetting.daySelectS = CDate(Now.ToString("yyyy/MM/dd"))
        m_typSetting.daySelectE = CDate(Now.ToString("yyyy/MM/dd"))
        dtpDateStart.Value = m_typSetting.daySelectS
        dtpDateEnd.Value = m_typSetting.daySelectE

        '''' その他
        Me.TopMost = True
        Me.nudXMax.Maximum = GC_MAX_PROTDATA

        '''' データセット
        Call Disp()
    End Sub


    ''' <summary>
    ''' ファンクションボタンクリック
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmDspSetting_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン
            Case 1 - 1                                  ' 登録
                If Save() Then
                    ' イベントを発生させる
                    RaiseEvent EvDsp()

                    Me.Hide()
                End If

            Case 12 - 1                                 ' 終了
                Me.Hide()
        End Select
    End Sub


    ''' <summary>
    ''' 画面再表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspRefresh()
        '' 対象コイル指定
        m_typSetting.nSelectMode = tMod.ReadIniInt(SO_TRAND, "SELECT_MODE", LOCALINI_NAME, 0)

        Call ChangeRadSelectMode(m_typSetting.nSelectMode)
        dtpDateStart.Value = m_typSetting.daySelectS
        dtpDateEnd.Value = m_typSetting.daySelectE

        '' フィルター表示
        Call ReadFilterData(dgvDispType, DB_TYPE_NAME, m_typSetting.nDspType, m_typSetting.sDspType) ' フィルター保存値反映
        Call ReadFilterData(dgvDispGrade, DB_GRADE_NAME, m_typSetting.nDspGrade, m_typSetting.sDspGrade)


        '' グラフ
        m_typSetting.nXmax = tMod.ReadIniInt(SO_TRAND, "X_MAX", LOCALINI_NAME, 5)
        m_typSetting.nXinterval = tMod.ReadIniInt(SO_TRAND, "X_INTERVAL", LOCALINI_NAME, 1)
        m_typSetting.nYmax = tMod.ReadIniInt(SO_TRAND, "Y_MAX", LOCALINI_NAME, 5)
        m_typSetting.nYinterval = tMod.ReadIniInt(SO_TRAND, "Y_INTERVAL", LOCALINI_NAME, 1)

        nudXMax.Value = m_typSetting.nXmax
        nudXInterval.Value = m_typSetting.nXinterval
        nudYMax.Value = m_typSetting.nYmax
        nudYInterval.Value = m_typSetting.nYinterval

        '' 色
        m_typSetting.colorLine(0) = tMod.GetIntToColor(tMod.ReadIniInt(SO_TRAND, "LINE_COLOR_1", LOCALINI_NAME, &HFF0000))
        m_typSetting.colorLine(1) = tMod.GetIntToColor(tMod.ReadIniInt(SO_TRAND, "LINE_COLOR_2", LOCALINI_NAME, &HFF0000))
        m_typSetting.colorLine(2) = tMod.GetIntToColor(tMod.ReadIniInt(SO_TRAND, "LINE_COLOR_3", LOCALINI_NAME, &HFF0000))
        m_typSetting.colorLine(3) = tMod.GetIntToColor(tMod.ReadIniInt(SO_TRAND, "LINE_COLOR_4", LOCALINI_NAME, &HFF0000))
        m_typSetting.colorLine(4) = tMod.GetIntToColor(tMod.ReadIniInt(SO_TRAND, "LINE_COLOR_5", LOCALINI_NAME, &HFF0000))
        btnLineColor1.BackColor = m_typSetting.colorLine(0)
        btnLineColor2.BackColor = m_typSetting.colorLine(1)
        btnLineColor3.BackColor = m_typSetting.colorLine(2)
        btnLineColor4.BackColor = m_typSetting.colorLine(3)
        btnLineColor5.BackColor = m_typSetting.colorLine(4)
    End Sub


    ''' <summary>
    ''' iniファイルに保存して、メンバーにも登録
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Save() As Boolean
        '''' 整合性チェック
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To dgvDispType.RowCount - 1
            If CBool(dgvDispType(EM_FILTER_COL.emCheck, ii).Value) Then
                nCnt += 1
            End If
        Next ii
        If nCnt > m_typSetting.nDspType.Length Then
            ' NG
            Using frmMsg As New tClassForm.frmPop_MsgBox("疵種の最大選択数は、" & GC_MAX_SELECT_TYPE & "件です。", "異常", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End Using
        End If

        If nudYMax.Value < nudYInterval.Value Then
            Using frmMsg As New tClassForm.frmPop_MsgBox("最大値 [％]はメモリ間隔 [％]より大きい値を設定してください。", "異常", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End Using
        End If

        If nudXMax.Value < nudXInterval.Value Then
            Using frmMsg As New tClassForm.frmPop_MsgBox("最大値 [本数]はメモリ間隔 [本数]より大きい値を設定してください。", "異常", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End Using
        End If

        '''' 登録
        '' 対象コイル指定
        If rdoSelect1.Checked Then
            m_typSetting.nSelectMode = 0
        ElseIf rdoSelect2.Checked Then
            m_typSetting.nSelectMode = 1
        End If
        tClassLibrary.tMod.WriteIni(SO_TRAND, "SELECT_MODE", LOCALINI_NAME, CStr(m_typSetting.nSelectMode))

        m_typSetting.daySelectS = dtpDateStart.Value
        m_typSetting.daySelectE = dtpDateEnd.Value


        '' フィルター
        Call SaveFilterData(dgvDispType, DB_TYPE_NAME, m_typSetting.nDspType, m_typSetting.sDspType)
        Call SaveFilterData(dgvDispGrade, DB_GRADE_NAME, m_typSetting.nDspGrade, m_typSetting.sDspGrade)


        '' グラフ
        m_typSetting.nXmax = CInt(nudXMax.Value)
        m_typSetting.nXinterval = CInt(nudXInterval.Value)
        m_typSetting.nYmax = CInt(nudYMax.Value)
        m_typSetting.nYinterval = CInt(nudYInterval.Value)
        tClassLibrary.tMod.WriteIni(SO_TRAND, "X_MAX", LOCALINI_NAME, CStr(m_typSetting.nXmax))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "X_INTERVAL", LOCALINI_NAME, CStr(m_typSetting.nXinterval))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "Y_MAX", LOCALINI_NAME, CStr(m_typSetting.nYmax))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "Y_INTERVAL", LOCALINI_NAME, CStr(m_typSetting.nYinterval))

        '' 色
        m_typSetting.colorLine(0) = btnLineColor1.BackColor
        m_typSetting.colorLine(1) = btnLineColor2.BackColor
        m_typSetting.colorLine(2) = btnLineColor3.BackColor
        m_typSetting.colorLine(3) = btnLineColor4.BackColor
        m_typSetting.colorLine(4) = btnLineColor5.BackColor

        tClassLibrary.tMod.WriteIni(SO_TRAND, "LINE_COLOR_1", LOCALINI_NAME, CStr(tMod.GetColorToInt(m_typSetting.colorLine(0))))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "LINE_COLOR_2", LOCALINI_NAME, CStr(tMod.GetColorToInt(m_typSetting.colorLine(1))))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "LINE_COLOR_3", LOCALINI_NAME, CStr(tMod.GetColorToInt(m_typSetting.colorLine(2))))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "LINE_COLOR_4", LOCALINI_NAME, CStr(tMod.GetColorToInt(m_typSetting.colorLine(3))))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "LINE_COLOR_5", LOCALINI_NAME, CStr(tMod.GetColorToInt(m_typSetting.colorLine(4))))
        Return True
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ フィルターグリッド 動作定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵種フィルター グリッド初期化
    ''' </summary>
    ''' <param name="dgv">フィルター用のデータグリッド</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize_FilterGrid(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)

        '''' 独自の項目を設定
        With dgv
            ' '' 定義
            '.ColumnHeadersHeight = 25										' ヘッダーの高さ
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 行選択
            .ScrollBars = ScrollBars.Vertical

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 0
        End With
    End Sub
    ''' <summary>
    ''' 疵種フィルター グリッド列定義
    ''' </summary>
    ''' <param name="dgv">フィルター用のデータグリッド</param>
    ''' <param name="strName">2列目の項目名</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitColumn_FilterGrid(ByVal dgv As DataGridView, ByVal strName As String)
        With dgv
            ''''' 列追加
            .Columns.Add(New DataGridViewCheckBoxColumn)  ' チェックボックス
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' EdasysID

            '''' 列定義
            '1列目:チェックボックス
            .Columns(EM_FILTER_COL.emCheck).HeaderText = ""
            .Columns(EM_FILTER_COL.emCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).Width = 20
            .Columns(EM_FILTER_COL.emCheck).SortMode = DataGridViewColumnSortMode.NotSortable

            '2列目:疵種名称
            .Columns(EM_FILTER_COL.emName).HeaderText = strName & vbCrLf
            .Columns(EM_FILTER_COL.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).Width = .ClientSize.Width - 20 - 3
            .Columns(EM_FILTER_COL.emName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:EdasysID
            .Columns(EM_FILTER_COL.emEdasysID).Visible = False  '非表示
        End With
    End Sub

    ''' <summary>
    ''' 疵種・グレードフィルタセット
    ''' </summary>
    ''' <param name="dgv">データグリッド</param>
    ''' <param name="strTable">テーブル名</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String)
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim iCnt As Integer = 0

        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT Name, "
        strSQL &= "        EdasysID "
        strSQL &= " FROM " & strTable
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()

                dgv.RowCount += 1   '表示行数を設定

                ''設定不可 指定
                dgv(EM_FILTER_COL.emName, iCnt).ReadOnly = True

                'データセット
                dgv(EM_FILTER_COL.emName, iCnt).Value = sqlRead.GetString(0)
                dgv(EM_FILTER_COL.emName, iCnt).Style.WrapMode = DataGridViewTriState.False
                dgv(EM_FILTER_COL.emCheck, iCnt).Value = False
                dgv(EM_FILTER_COL.emEdasysID, iCnt).Value = sqlRead.GetInt32(1)

                iCnt += 1

            End While

        Catch ex As Exception

        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try
    End Sub
    ''' <summary>
    ''' フィルターのチェック保存
    ''' </summary>
    ''' <param name="dgv">データグリッド</param>
    ''' <param name="strInikey">iniファイルの対象キー</param>
    ''' <remarks></remarks>
    Protected Sub SaveFilterData(ByVal dgv As DataGridView, ByVal strInikey As String, ByVal setVal() As Integer, ByVal name() As String)
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To setVal.Length - 1
            setVal(ii) = 0
            name(ii) = ""
        Next

        Dim strVal As String = ""

        ' 疵種,グレードフィルターの条件をINIファイルへ登録
        For ii As Integer = 0 To dgv.RowCount - 1
            strVal &= CStr(IIf(CType(dgv(EM_FILTER_COL.emCheck, ii).Value, Boolean), "1", "0"))

            If CBool(dgv(EM_FILTER_COL.emCheck, ii).Value) Then
                setVal(nCnt) = CInt(dgv(EM_FILTER_COL.emEdasysID, ii).Value)
                name(nCnt) = CStr(dgv(EM_FILTER_COL.emName, ii).Value)
                nCnt += 1
            End If
        Next ii

        tClassLibrary.tMod.WriteIni(SO_TRAND, strInikey, LOCALINI_NAME, strVal)
    End Sub

    ''' <summary>
    ''' フィルターのチェックデータ読込
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="strInikey"></param>
    ''' <param name="setVal">ON状態のEdasysID</param>
    ''' <remarks></remarks>
    Protected Sub ReadFilterData(ByVal dgv As DataGridView, ByVal strInikey As String, ByVal setVal() As Integer, ByVal name() As String)
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To setVal.Length - 1
            setVal(ii) = 0
            name(ii) = ""
        Next

        ' INIファイルの設定を取得
        Dim strVal As String = tClassLibrary.tMod.ReadIniStr(SO_TRAND, strInikey, LOCALINI_NAME)
        If "" = strVal Then Exit Sub

        ' 設定値を反映
        For ii As Integer = 0 To dgv.RowCount - 1

            '未登録または、1が設定されている場合は、チェックを入れる
            If "1" = Mid(strVal, ii + 1, 1) Then
                If nCnt >= setVal.Length Then Continue For

                dgv(EM_FILTER_COL.emCheck, ii).Value = True
                setVal(nCnt) = CInt(dgv(EM_FILTER_COL.emEdasysID, ii).Value)
                name(nCnt) = CStr(dgv(EM_FILTER_COL.emName, ii).Value)
                nCnt += 1
            Else
                dgv(EM_FILTER_COL.emCheck, ii).Value = False
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' グリッド動作 ハンドル割り当て
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvFilter_AddHandler(ByVal dgv As tCnt.tCnt_DataGridView)
        AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
        AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv.dgvData_CurrentCellDirtyStateChanged
    End Sub

    ''' <summary>
    ''' セルチェンジ処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, tCnt.tCnt_DataGridView)
        'ヘッダは処理しない
        If e.RowIndex = -1 Then Exit Sub

        If e.ColumnIndex = EM_FILTER_COL.emCheck Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '' チェックを付けた時
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
            Else
                '' チェックを外した時
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub
    ''' <summary>
    ''' 全選択ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGradeAllSelect.Click

        'チェックボックスを全てON
        Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), True)

    End Sub
    ''' <summary>
    ''' 全解除ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

        'チェックボックスを全てOFF
        Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), False)

    End Sub

    ''' <summary>
    ''' グリッドのチェックボックスセット
    ''' </summary>
    ''' <param name="dgvData">データグリッド</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CheckGridAllChange(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' グリッドの全てのチェックボックスを設定
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_FILTER_COL.emCheck, ii).Value = bValue
        Next ii

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他細かい処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ラジオボタン切り替え
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub rdoSelect_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles rdoSelect1.CheckedChanged, rdoSelect2.CheckedChanged
        Dim rdo As RadioButton = CType(sender, RadioButton)

        '' チェックが外れたよ処理は何もしない
        If Not rdo.Checked Then
            Return
        End If

        '' チェック状態に変更した場合のみ
        If rdo Is rdoSelect1 Then
            Call ChangeRadSelectMode(0)
        Else
            Call ChangeRadSelectMode(1)
        End If
    End Sub

    ''' <summary>
    ''' ラジオボタン切り替え処理
    ''' </summary>
    ''' <param name="mode">0:直近コイル 1:期間指定</param>
    ''' <remarks></remarks>
    Private Sub ChangeRadSelectMode(ByVal mode As Integer)
        If 0 = mode Then
            rdoSelect1.Checked = True
            dtpDateStart.Enabled = False
            dtpDateEnd.Enabled = False
        Else
            rdoSelect2.Checked = True
            dtpDateStart.Enabled = True
            dtpDateEnd.Enabled = True
        End If
    End Sub


    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' 線色指定ボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLineColor1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLineColor1.Click, btnLineColor2.Click, btnLineColor3.Click, btnLineColor4.Click, btnLineColor5.Click
        Dim btn As Button = CType(sender, Button)
        Using dlgColor As New System.Windows.Forms.ColorDialog
            '色選択ダイアログの設定
            '色の作成を使用しない
            dlgColor.AllowFullOpen = False

            '色選択ダイアログの選択色を、ボタンの背景色にする
            dlgColor.Color = btn.BackColor

            ' 色選択ダイアログでokボタンが押されたら、ボタンの色を変更する 
            If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
                '選択色をボタンの背景色に
                btn.BackColor = dlgColor.Color
            End If
        End Using
    End Sub
End Class
