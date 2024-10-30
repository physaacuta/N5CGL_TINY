
'*********************************************************************************
' 検査実績検索画面の親クラス
'	[Ver]
'		Ver.01    2007/08/29  vs2005 対応
'
'	[メモ]
'		戸谷氏 製作版。    frmPop_Search2 の方を使用するようにしてください。
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Public Class frmPop_Search

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    '/ /////////////////////////////////////////////////////////////////////////////////

    '疵種・グレードフィルター列名
    Public Enum EM_FILTER_COL                ' 疵種
        nCheck = 0                           ' チェックボックス
        nName                                ' 名称
        nEdasysID                            ' EdasysID
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_strSection As String          '疵種リストのチェック情報のセクション名
    Private m_strTypeKey As String          '疵種リストのチェック情報を保存するのキー名
    Private m_strGradeKey As String         'グレードリストのチェック情報を保存するキー名
	Private m_sDbSession As String = INI_DB									' DBセッション
    Private m_strIniName As String = LOCALINI_NAME  'チェック情報を保存するINIファイル名

    ''' <summary>
    ''' 疵種・グレードリストのチェック情報を保存するセクション名をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
     , System.ComponentModel.Description("疵種・グレードリストのチェック情報を保存するセクション名")> _
    Public Property tProp_IniSection() As String
        Get
            Return m_strSection
        End Get
        Set(ByVal Value As String)
            m_strSection = Value
        End Set
    End Property

    ''' <summary>
    ''' 疵種チェック情報を保存するキー名をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
     , System.ComponentModel.Description("疵種チェック情報を保存するキー名")> _
    Public Property tProp_TypeKey() As String
        Get
            Return m_strTypeKey
        End Get
        Set(ByVal Value As String)
            m_strTypeKey = Value
        End Set
    End Property

    ''' <summary>
    ''' 疵種チェック情報を保存するキー名をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
     , System.ComponentModel.Description("疵種チェック情報を保存するキー名")> _
    Public Property tProp_GradeKey() As String
        Get
            Return m_strGradeKey
        End Get
        Set(ByVal Value As String)
            m_strGradeKey = Value
        End Set
    End Property


	''' <summary>
	''' DBセクション
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(INI_DB) _
     , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
     , System.ComponentModel.Description("DBセッション")> _
    Public Property tProp_DbSession() As String
        Get
            Return m_sDbSession
        End Get
        Set(ByVal Value As String)
            m_sDbSession = Value
        End Set
    End Property

    ''' <summary>
    ''' チェック情報を保存するINIファイル名
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(INI_DB) _
     , System.ComponentModel.Category("☆検査実績検索画面専用プロパティ") _
     , System.ComponentModel.Description("チェック情報を保存するINIファイル名")> _
    Public Property tProp_IniName() As String
        Get
            Return m_strIniName
        End Get
        Set(ByVal Value As String)
            m_strIniName = Value
        End Set
    End Property


    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        '疵種・グレードフィルター初期化
        Call dgvInitializeFilterGrid(dgvDispType, "疵種名称")
        Call dgvInitializeFilterGrid(dgvDispGrade, "グレード名称")

        '疵種全選択、全解除に該当グリッド情報をセット
        btnTypeAllSelect.Tag = dgvDispType
        btnTypeAllCancel.Tag = dgvDispType

        'グレード全選択、全解除に該当グリッド情報をセット
        btnGradeAllSelect.Tag = dgvDispGrade
        btnGradeAllCancel.Tag = dgvDispGrade

	End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 疵種フィルター グリッド初期化
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵種フィルター グリッド初期化
    ''' </summary>
    ''' <param name="dgv">フィルター用のデータグリッド</param>
    ''' <param name="strName">2列目の項目名</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitializeFilterGrid(ByVal dgv As DataGridView, ByVal strName As String)

        With dgv
            .RowCount = 0
            .ColumnCount = 0

            '' 動作定義
            .AutoGenerateColumns = False            ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False             ' ユーザー 行追加
            .AllowUserToDeleteRows = False          ' ユーザー 行削除
            .AllowUserToOrderColumns = False        ' ユーザー 列移動
            .AllowUserToResizeColumns = False        ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False          ' ユーザー 行サイズ変更
            .ShowCellErrors = False                 ' セルエラーを表示しない
            .ShowCellToolTips = False               ' ツールヒントを表示しない
            .ShowEditingIcon = False                ' 
            .ShowRowErrors = False                  '

            .SelectionMode = DataGridViewSelectionMode.CellSelect       ' セル選択
            .MultiSelect = False                                        ' 複数行選択
            .ReadOnly = False                                           ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                ' セル書き換え

            .ScrollBars = ScrollBars.Vertical                                                       'スクロールバー 縦のみ有効
            .ColumnHeadersVisible = True                                                           ' 列ヘッダ表示
            .RowHeadersVisible = False                                                              ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None                             ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None                                   ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' 選択
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            ''''' 列追加
            .Columns.Add(New DataGridViewCheckBoxColumn)  ' チェックボックス
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' EdasysID

            '''' 列定義
            '1列目:チェックボックス
            .Columns(EM_FILTER_COL.nCheck).HeaderText = ""
            .Columns(EM_FILTER_COL.nCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.nCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.nCheck).Width = 20
            .Columns(EM_FILTER_COL.nCheck).SortMode = DataGridViewColumnSortMode.NotSortable

            '2列目:疵種名称
            .Columns(EM_FILTER_COL.nName).HeaderText = strName & vbCrLf
            .Columns(EM_FILTER_COL.nName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.nName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.nName).Width = 260
            .Columns(EM_FILTER_COL.nName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:EdasysID
            .Columns(EM_FILTER_COL.nEdasysID).Visible = False   '非表示

        End With
    End Sub

    ''' <summary>
    ''' 疵種・グレードフィルタセット
    ''' </summary>
    ''' <param name="dgv">データグリッド</param>
    ''' <param name="strTable">テーブル名</param>
    ''' <param name="strSection">セクション名</param>
    ''' <param name="strKey">キー</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String, _
                                ByVal strSection As String, ByVal strKey As String)

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim iCnt As Integer = 0
        Dim strSetString As String = ""

        'INIファイルの設定を取得
        strSetString = tClassLibrary.tMod.ReadIniStr(strSection, strKey, m_strIniName)

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
                dgv(EM_FILTER_COL.nName, iCnt).ReadOnly = True

                'データセット
                dgv(EM_FILTER_COL.nName, iCnt).Value = sqlRead.GetString(0)
                dgv(EM_FILTER_COL.nName, iCnt).Style.WrapMode = DataGridViewTriState.False
                dgv(EM_FILTER_COL.nEdasysID, iCnt).Value = sqlRead.GetInt32(1)

                '未登録または、1が設定されている場合は、チェックを入れる
                If "" = strSetString OrElse "1" = Mid(strSetString, iCnt + 1, 1) Then
                    dgv(EM_FILTER_COL.nCheck, iCnt).Value = True
                Else
                    dgv(EM_FILTER_COL.nCheck, iCnt).Value = False
                End If

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
    ''' セルチェンジ処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvDispType.CellValueChanged, dgvDispGrade.CellValueChanged

        Dim dgv As DataGridView = CType(sender, DataGridView)

        If e.ColumnIndex = EM_FILTER_COL.nCheck Then

            'ヘッダは処理しない
            If e.RowIndex = -1 Then Exit Sub

            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                'チェックを付けた時
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
            Else
                'チェックを外した時
                If e.RowIndex Mod 2 = 0 Then
                    dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.White
                Else
                    dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.AliceBlue
                End If
            End If
        End If

    End Sub

    ''' <summary>
    ''' セルの状態が変化した時の処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvDispType.CurrentCellDirtyStateChanged, dgvDispGrade.CurrentCellDirtyStateChanged

        ' セルがコミットされていない変更を含む場合
        If dgvDispType.IsCurrentCellDirty Then
            'コミットする
            dgvDispType.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End If

        ' セルがコミットされていない変更を含む場合
        If dgvDispGrade.IsCurrentCellDirty Then
            'コミットする
            dgvDispGrade.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End If

    End Sub


    ''' <summary>
    ''' 画面終了
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click

        Me.Hide()

    End Sub
    ''' <summary>
    ''' 全選択ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllSelect.Click, btnGradeAllSelect.Click

        'チェックボックスを全てON
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), True)

    End Sub

    ''' <summary>
    ''' グリッドのチェックボックスセット
    ''' </summary>
    ''' <param name="dgvData">データグリッド</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Private Sub CheckGrid(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' グリッドの全てのチェックボックスを設定
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_FILTER_COL.nCheck, ii).Value = bValue
        Next ii

    End Sub
    ''' <summary>
    ''' 全解除ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

        'チェックボックスを全てOFF
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), False)

    End Sub

    ''' <summary>
    ''' 時刻フィルター 有効／無効設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkTimeSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkTimeSet.CheckedChanged

        ' コントロールの有効判定
        If chkTimeSet.Checked = True Then
            dtpTimeStart.Enabled = True
            dtpTimeEnd.Enabled = True
        Else
            dtpTimeStart.Enabled = False
            dtpTimeEnd.Enabled = False
        End If

        '日付フィルターが有効なら、日付フィルターも有効にする
        If chkTimeSet.Checked Then If chkDateSet.Checked = False Then chkDateSet.Checked = True

    End Sub
    ''' <summary>
    ''' 日付フィルター 有効／無効設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkDateSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkDateSet.CheckedChanged

        ' コントロールの有効判定
        If chkDateSet.Checked = True Then
            dtpDateStart.Enabled = True
            dtpDateEnd.Enabled = True
        Else
            dtpDateStart.Enabled = False
            dtpDateEnd.Enabled = False
        End If

        '日付フィルターが無効なら、時刻フィルターも無効にする
        If Not chkDateSet.Checked Then If chkTimeSet.Checked = True Then chkTimeSet.Checked = False

    End Sub


    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub


	''' <summary>
	''' 疵種、グレードフィルターのチェック保存
	''' </summary>
	''' <remarks></remarks>
	Public Sub SaveCheck()

		Dim strSetString As String
		Dim ii As Integer

		'疵種,グレードフィルターの条件をINIファイルへ登録
		With dgvDispType
			strSetString = ""
			For ii = 0 To dgvDispType.RowCount - 1
				strSetString = CStr(IIf(CType(dgvDispType(EM_FILTER_COL.nCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
			Next ii
            tClassLibrary.tMod.WriteIni(m_strSection, m_strTypeKey, m_strIniName, strSetString)
		End With

		With dgvDispGrade
			strSetString = ""
			For ii = 0 To dgvDispGrade.RowCount - 1
				strSetString = CStr(IIf(CType(dgvDispGrade(EM_FILTER_COL.nCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
			Next ii
            tClassLibrary.tMod.WriteIni(m_strSection, m_strGradeKey, m_strIniName, strSetString)
		End With

	End Sub

	'' 20130522 カンマを除去する為に引数追加
	''' <summary>
	''' CSV出力
	''' </summary>
	''' <param name="strFolder">保存ファイルのフォルダパス</param>
	''' <param name="strName">保存ファイルのファイル名</param>
	''' <param name="bDelComma">カンマを除去</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""						'1行のデータ
		Dim strWk() As String
		Dim strWkData As String

		'' フルパス
		strFPath = IO.Path.Combine(strFolder, strName)

		'' フォルダの有無チェック(無かったら作ろう)
		tMod.CheckFolder(strFolder)

		'' CSV出力
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' オープン
				cls_Stram.Open()



				'ヘッダを出力
				strData = ""
				For ii As Integer = 0 To dgvResult.ColumnCount - 1

					'改行コードをすべて取り除く
					strWk = dgvResult.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
					strWkData = ""
					For jj As Integer = 0 To strWk.Length - 1
						strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
					Next jj

					strData &= Trim(strWkData) & ","
				Next ii

				'最後のタブを排除
				strData = strData.Remove(strData.Length - 1, 1)

				cls_Stram.WriteLine(strData)

				'データ出力
				For ii As Integer = 0 To dgvResult.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvResult.ColumnCount - 1
						'項目をTabで連結する
						'' 20130522変更 カンマを除去------------------------>>>
						If True = bDelComma Then
							strData &= dgvResult(jj, ii).Value.ToString.Replace(",", "") & ","
						Else
							strData &= dgvResult(jj, ii).Value.ToString & ","
						End If
						'' <<<-------------------------------------------------
					Next jj

					'最後のタブを排除
					strData = strData.Remove(strData.Length - 1, 1)

					cls_Stram.WriteLine(strData)
				Next ii

				' クローズ
				cls_Stram.Close()
				strMsg = System.IO.Path.GetFullPath(strFPath) & "にCSVファイルを保存しました。"

			Catch ex As Exception
				strMsg = "CSV出力に失敗しました。"
			End Try
		End Using

		'''' ガイダンス表示
		Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
			frmMsg.ShowDialog()
		End Using
	End Sub


End Class
