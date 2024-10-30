Imports System.Windows.Forms
Imports System.Drawing
Imports System.Data.SqlClient
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class frmPopLongPdList
	Implements RpiPlugin.IPluginActivatable, RpiPlugin.IToolStrip, RpiPlugin.IDrawer, RpiPlugin.IRpiOpen

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' SQLモード
	''' </summary>
	Private Enum EM_SQLMODE As Integer
		NONE = -1               ' 対象外
		KIZUKEN                 ' 疵検管理No, DEFECT_INF
		KANRI                   ' 管理No, T_DEFECT_INF
		MANEGEMENT              ' ManagementNo, T_DEFECT_INF
	End Enum

	''' <summary>プラグイン用 Iniファイル</summary>
	Private Const TASKINI_LONGPD As String = ".\LongPdPlugin.ini"

	''' <summary>ローカル用 Iniキー</summary>
	Private Const LOCALINI_SECTION As String = "LongPdList"

	Private Const DgvMaxCol_MRare As Integer = 6            'データ列MAX
	Private Const MAX_LONGPD_FILTER As Integer = 16

	' グリッド　列設定
	Private Enum EMLONGPD_COL_ID
		iTorB = 0               ' 面
		iWidDivPosNo            ' 巾分割位置
		iWS                     ' WS位置
		iDS                     ' DS位置
		iCyclePitch             ' 周期判定ピッチ
		iGr                     ' Gr
	End Enum

	Private Structure TYP_GRADE_INF
		Dim nEdasysID As Integer
		Dim nPriority As Integer
		Dim strName As String
	End Structure

	Private Structure TYP_LONGPD_LIST
		Dim nTorB As Integer                        ' 面
		Dim nWidDivPosNo As Integer                 ' 巾分割位置
		Dim dCyclePitch As Double                   ' 周期判定ピッチ	※公差内はグループ化。

		Dim nDefNo As Integer                       ' 疵No				※最初に見つかったもの
		Dim fDs As Single                           ' DS位置			※最初に見つかったもの
		Dim fWs As Single                           ' WS位置			※最初に見つかったもの
		Dim lstGradeID As List(Of TYP_GRADE_INF)    ' グレード情報

		Public Sub Initialize()
			lstGradeID = New List(Of TYP_GRADE_INF)
		End Sub
	End Structure

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private mcls_RpiView As tClass_ViewRpiThread
	Private mcls_ReadRpi As tClass_ReadRPI

	Private m_strManagementNo As String             '管理No

	Private m_dicDispData As Dictionary(Of String, List(Of TYP_LONGPD_LIST))
	Private m_bWsDs As Boolean                      ' True: 左WS 右DS, False:左DS 右WS
	Private m_nTimeOut As Integer = 30
	Private m_strCsvPath As String                          ' CSV出力フォルダ
	Private m_strCsvFile As String                          ' CSV出力ファイル名


#Region "インターフェース実装"

	' RpiPlugin.IPluginActivatable		: RpiViewerによってNewされる
	' RpiPlugin.IToolStrip				: ツールバーのボタン追加
	' RpiPlugin.IDrawer					: オーバーレイ描画
	' RpiPlugin.IRpiOpen				: RPIファイルのOpen直後/閉じる前にCallされる

#Region "RpiPlugin.IPluginActivatable"

	Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
		Get
			Return 350
		End Get
	End Property

	Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
		Get
			Return "frmLongPdList"
		End Get
	End Property

	''' <summary>
	''' Plugin初期化
	''' </summary>
	''' <param name="v"></param>
	''' <param name="mainfrm"></param>
	Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
		mcls_RpiView = v

		' 設定の読込
		m_nTimeOut = tMod.ReadIniInt(LOCALINI_SECTION, "TIME_OUT", TASKINI_LONGPD, 30)
		m_strCsvPath = tMod.ReadIniStr(LOCALINI_SECTION, "CSV_PATH", TASKINI_LONGPD, ".\\")
		m_strCsvFile = tMod.ReadIniStr(LOCALINI_SECTION, "CSV_FILE", TASKINI_LONGPD, "長周期欠陥一覧.CSV")

		nudFrom.Value = tMod.ReadIniInt(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_FROM", RPI_VIEWER.modCommon.LOCALINI_NAME, 2470)
		nudTo.Value = tMod.ReadIniInt(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_TO", RPI_VIEWER.modCommon.LOCALINI_NAME, 2600)
		nudTolerance.Value = tMod.ReadIniInt(LOCALINI_SECTION, "TOLERANCE", RPI_VIEWER.modCommon.LOCALINI_NAME, 0)
	End Sub

#End Region

#Region "RpiPlugin.IToolStrip"

	Private m_tsbLongPdList As New System.Windows.Forms.ToolStripButton()

	''' <summary>
	''' ツールバー初期化
	''' </summary>
	''' <param name="mainstrip"></param>
	Public Sub InitToolbar(mainstrip() As ToolStrip) Implements IToolStrip.InitToolbar

		' 熱延DL表示ボタン追加
		With m_tsbLongPdList
			.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
			.Image = LabelIcon.Image
			.ImageTransparentColor = System.Drawing.Color.White
			.Name = "tsbLongPdList"
			.Size = New System.Drawing.Size(23, 23)
			.Text = "長周期欠陥一覧"
			AddHandler .Click, AddressOf OnHandleDspClick
		End With

		Dim ipos As Integer = GetItemIndex(mainstrip(0), "tsbKizuList")
		mainstrip(0).Items.Insert(ipos + 1, m_tsbLongPdList)
	End Sub

#End Region

#Region "RpiPlugin.IRpiOpen"

	''' <summary>
	''' RPIファイルOpen直後
	''' </summary>
	''' <param name="rpi"></param>
	Public Sub OnRpiOpen(rpi As tClass_ReadRPI) Implements IRpiOpen.OnRpiOpen
		mcls_ReadRpi = rpi

		m_dicDispData = New Dictionary(Of String, List(Of TYP_LONGPD_LIST))

		PreDispProc(mcls_ReadRpi.GetRpiTHeader.cKizukenNo, mcls_RpiView.GetProp_DSWS)

	End Sub

	''' <summary>
	''' RPIファイルClose前
	''' </summary>
	Public Sub OnRpiClose() Implements IRpiOpen.OnRpiClose
		If mcls_RpiView.IsRpi = True Then

		End If
		m_strManagementNo = ""
	End Sub

#End Region

#Region "RpiPlugin.IDrawer"

	Public Sub OnChangeFrameNo(nf_start As Integer, nf_end As Integer) Implements IDrawer.OnChangeFrameNo

	End Sub

	''' <summary>
	''' 全長画像に描画
	''' </summary>
	''' <param name="g"></param>
	Public Sub OnDraw(g As System.Drawing.Graphics) Implements IDrawer.OnDraw

	End Sub
#End Region

#End Region


	''' <summary>
	''' グリッドの初期化
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
		Dim ii As Integer = 0
		With dgv
			.RowCount = 0

			'' 動作定義
			.AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
			.AllowUserToAddRows = False                                     ' ユーザー 行追加
			.AllowUserToDeleteRows = False                                  ' ユーザー 行削除
			.AllowUserToOrderColumns = False                                ' ユーザー 列移動
			.AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更
			.AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
			.ShowCellErrors = False
			.ShowCellToolTips = False
			.ShowEditingIcon = False
			.ShowRowErrors = False
			.ScrollBars = ScrollBars.Both


			.SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' セル選択
			.MultiSelect = False                                            ' 複数行選択
			.ReadOnly = True                                                ' 読み取り専用
			.EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

			.ColumnHeadersVisible = True                                    ' 列ヘッダ表示
			.RowHeadersVisible = False                                      ' 行ヘッダ表示
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

			'' 選択セル色定義
			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' 選択
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			'' 列定義
			.ColumnCount = DgvMaxCol_MRare           'textbox のみを追加する場合はこれの方が楽
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.ColumnHeadersHeight = 40                                       ' 列ヘッダサイズ

			'列１ 項目見出し
			.Columns(EMLONGPD_COL_ID.iTorB).HeaderText = "面"
			.Columns(EMLONGPD_COL_ID.iTorB).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(EMLONGPD_COL_ID.iTorB).Width = 40
			.Columns(EMLONGPD_COL_ID.iTorB).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iTorB).Frozen = True                                       ' 水平移動禁止

			'列２ 項目見出し
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).HeaderText = "巾分割位置" & vbCrLf & "(DS 0～7 WS)"
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).Width = 120
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).Frozen = True                                       ' 水平移動禁止

			'列３ 項目見出し
			.Columns(EMLONGPD_COL_ID.iWS).HeaderText = IIf(m_bWsDs = True, "WS～[mm]", "DS～[mm]")
			.Columns(EMLONGPD_COL_ID.iWS).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(EMLONGPD_COL_ID.iWS).Width = 70
			.Columns(EMLONGPD_COL_ID.iWS).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iWS).Frozen = False

			'列４ 項目見出し
			.Columns(EMLONGPD_COL_ID.iDS).HeaderText = IIf(m_bWsDs = True, "DS～[mm]", "WS～[mm]")
			.Columns(EMLONGPD_COL_ID.iDS).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(EMLONGPD_COL_ID.iDS).Width = 70
			.Columns(EMLONGPD_COL_ID.iDS).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iDS).Frozen = False                                       ' 水平移動禁止

			'列５ 項目見出し
			.Columns(EMLONGPD_COL_ID.iCyclePitch).HeaderText = "周期判定" & vbCrLf & "ﾋﾟｯﾁ[mm]"
			.Columns(EMLONGPD_COL_ID.iCyclePitch).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(EMLONGPD_COL_ID.iCyclePitch).Width = 90
			.Columns(EMLONGPD_COL_ID.iCyclePitch).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iCyclePitch).Frozen = False

			Dim nWidth As Integer = .Width
			For nColIndex As Integer = 0 To .ColumnCount - 1
				If nColIndex <> EMLONGPD_COL_ID.iGr Then
					nWidth = nWidth - .Columns(nColIndex).Width
				End If
			Next
			nWidth = nWidth - SystemInformation.VerticalScrollBarWidth - SystemInformation.Border3DSize.Width * 2
			If nWidth < 0 Then
				nWidth = 0
			End If

			'列	６ 項目見出し
			.Columns(EMLONGPD_COL_ID.iGr).HeaderText = "Gr"
			.Columns(EMLONGPD_COL_ID.iGr).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(EMLONGPD_COL_ID.iGr).Width = nWidth
			.Columns(EMLONGPD_COL_ID.iGr).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iGr).Frozen = False

		End With
	End Sub


	''' <summary>
	''' 表示前処理
	''' </summary>
	Public Function PreDispProc(ByVal strKanriNo As String, ByVal bWsDs As Boolean) As Boolean
		m_strManagementNo = strKanriNo
		m_bWsDs = bWsDs

		'画面初期化
		Call InitDsp()

		'データ取得
		If Not GetData() Then
			Return False
		End If

		'データ表示
		Call DspData()

		Return True
	End Function


	''' <summary>
	''' データ表示
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspData()
		' データを表示する。
		If "" = m_strManagementNo Then
			' 該当データなし
			Exit Sub
		End If

		Dim lstVal As List(Of TYP_LONGPD_LIST) = m_dicDispData(m_strManagementNo)

		Try
			With dgvLongPdList
				.SuspendLayout()
				.RowCount = 0
				.RowCount = lstVal.Count

				For ii As Integer = 0 To lstVal.Count - 1
					dgvLongPdList(EMLONGPD_COL_ID.iTorB, ii).Value = IIf(0 = lstVal(ii).nTorB, "表", "裏")
					dgvLongPdList(EMLONGPD_COL_ID.iWidDivPosNo, ii).Value = lstVal(ii).nWidDivPosNo.ToString
					If True = m_bWsDs Then
						dgvLongPdList(EMLONGPD_COL_ID.iWS, ii).Value = lstVal(ii).fWs.ToString("0.0")
						dgvLongPdList(EMLONGPD_COL_ID.iDS, ii).Value = lstVal(ii).fDs.ToString("0.0")
					Else
						dgvLongPdList(EMLONGPD_COL_ID.iDS, ii).Value = lstVal(ii).fWs.ToString("0.0")
						dgvLongPdList(EMLONGPD_COL_ID.iWS, ii).Value = lstVal(ii).fDs.ToString("0.0")
					End If

					dgvLongPdList(EMLONGPD_COL_ID.iCyclePitch, ii).Value = CInt(lstVal(ii).dCyclePitch)

					Dim strName As String = ""
					For jj As Integer = 0 To lstVal(ii).lstGradeID.Count - 1
						If 0 < jj Then
							strName = strName + " "
						End If
						strName = strName + lstVal(ii).lstGradeID(jj).strName
					Next
					dgvLongPdList(EMLONGPD_COL_ID.iGr, ii).Value = strName

				Next
			End With
		Catch ex As Exception
		Finally
			dgvLongPdList.ResumeLayout()
		End Try
	End Sub

	''' <summary>
	''' 画面初期化
	''' </summary>
	''' <remarks></remarks>
	Private Sub InitDsp()
		'初期化
		dgvInitialize(dgvLongPdList)
	End Sub


	''' <summary>
	''' データ取得
	''' </summary>
	''' <remarks></remarks>
	Private Function GetData() As Boolean

		Dim tcls_DB As tClass.tClass_SQLServer
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String    'SQL文
		Dim bRet As Boolean = False
		Dim strTableLongPd() As String = {"T_LONGPD_EXEC_RESULT_DEFECT_T", "T_LONGPD_EXEC_RESULT_DEFECT_B"}
		Dim strTableDefect() As String
		Dim strTableGrade As String = ""
		Dim strKeyKanriNo As String = ""
		Dim strKeyKizuNo As String = ""
		Dim strDataDS As String = ""
		Dim strDataWS As String = ""
		Dim strDataDefectGrade As String = ""
		Dim strDataGradeName As String = ""
		Dim strDataGradePriority As String = ""

		Dim nPitchFrom As Integer = CInt(nudFrom.Value)
		Dim nPitchTo As Integer = CInt(nudTo.Value)
		Dim nTolerance As Integer = CInt(nudTolerance.Value)

		If "" = m_strManagementNo Then
			Return False
		End If

		' 初期化
		m_dicDispData.Clear()

		' モード取得
		Dim nMode As Integer = tMod.ReadIniInt(mcls_ReadRpi.GetRpiTHeader.cSysName, "SQL_MODE", TASKINI_LONGPD, EM_SQLMODE.NONE)
		ReDim strTableDefect(2 - 1)
		If EM_SQLMODE.KIZUKEN = nMode Then
			' 疵検管理No
			strTableDefect(0) = "DEFECT_INF_T"
			strTableDefect(1) = "DEFECT_INF_B"
			strTableGrade = "GRADE_NAME"
			strKeyKanriNo = "疵検管理No"
			strKeyKizuNo = "疵No"
			strDataDS = "DS位置"
			strDataWS = "WS位置"
			strDataDefectGrade = "判定グレード"
			strDataGradeName = "Name"
			strDataGradePriority = "優先度"

		ElseIf EM_SQLMODE.KANRI = nMode Then
			' 管理No
			strTableDefect(0) = "T_DEFECT_INF_T"
			strTableDefect(1) = "T_DEFECT_INF_B"
			strTableGrade = "GRADE_NAME"
			strKeyKanriNo = "管理No"
			strKeyKizuNo = "疵No"
			strDataDS = "DS位置"
			strDataWS = "WS位置"
			strDataDefectGrade = "判定グレード"
			strDataGradeName = "Name"
			strDataGradePriority = "優先度"

		ElseIf EM_SQLMODE.MANEGEMENT = nMode Then
			' ManagementNo
			strTableDefect(0) = "T_DEFECT_INF_TOP"
			strTableDefect(1) = "T_DEFECT_INF_BOT"
			strTableGrade = "M_GRADE_NAME"
			strKeyKanriNo = "ManagementNo"
			strKeyKizuNo = "DefectNo"
			strDataDS = "XFromDSEdge"
			strDataWS = "XFromWSEdge"
			strDataDefectGrade = "GradeEdasysID"
			strDataGradeName = "Name"
			strDataGradePriority = "Priority"

		Else
			' 対象外
			Return False
		End If


		Dim liInfo As RPI_VIEWER.LineInfo = RPI_VIEWER.ParamLine.Find(mcls_ReadRpi.GetRpiTHeader.cSysName)
		tcls_DB = liInfo.CreateSqlConnection

		'DBオープン
		tcls_DB.SQLTimeOut = m_nTimeOut
		If Not tcls_DB.Open() Then
			Return False
		End If

		Try
			bRet = True
			Dim lstLongPd As New List(Of TYP_LONGPD_LIST)

			For nTorB As Integer = 0 To 2 - 1

				' SQL生成
				Dim sbSQL As New System.Text.StringBuilder

				sbSQL.AppendLine("")
				sbSQL.AppendLine("SELECT ")
				sbSQL.AppendLine("     L.WidDivPosNo")
				sbSQL.AppendLine("    ,L.DefectNo")
				sbSQL.AppendFormat("    ,I.{0} As XFromDSEdge", strDataDS)
				sbSQL.AppendLine("")
				sbSQL.AppendFormat("    ,I.{0} As XFromWSEdge", strDataWS)
				sbSQL.AppendLine("")
				sbSQL.AppendLine("    ,G.EdasysID As GradeEdasysID")
				sbSQL.AppendFormat("    ,G.{0} As GradeName", strDataGradeName)
				sbSQL.AppendLine("")
				sbSQL.AppendFormat("    ,G.{0} As GradePriority", strDataGradePriority)
				sbSQL.AppendLine("")
				For ii As Integer = 1 To MAX_LONGPD_FILTER
					sbSQL.AppendFormat("    ,FLOOR(L.Cycle_Filter{0}) As Cycle_Filter{0}", ii)
					sbSQL.AppendLine()
				Next
				sbSQL.AppendLine("FROM")
				sbSQL.AppendFormat("    {0} L", strTableLongPd(nTorB))
				sbSQL.AppendLine()
				sbSQL.AppendFormat("    INNER JOIN {0} I", strTableDefect(nTorB))
				sbSQL.AppendLine()
				sbSQL.AppendFormat("        ON  L.ManagementNo = I.{0}", strKeyKanriNo)
				sbSQL.AppendLine()
				sbSQL.AppendFormat("        AND L.DefectNo = I.{0}", strKeyKizuNo)
				sbSQL.AppendLine()
				sbSQL.AppendFormat("    INNER JOIN {0} G", strTableGrade)
				sbSQL.AppendLine()
				sbSQL.AppendFormat("        ON I.{0} = G.EdasysID", strDataDefectGrade)
				sbSQL.AppendLine()
				sbSQL.AppendLine("WHERE")
				sbSQL.AppendFormat("        L.ManagementNo = '{0}'", m_strManagementNo)
				sbSQL.AppendLine()
				sbSQL.AppendLine("    AND (")
				For ii As Integer = 1 To MAX_LONGPD_FILTER
					If ii > 1 Then
						sbSQL.Append(" OR ")
					End If
					sbSQL.AppendFormat(" Cycle_Filter{0} BETWEEN {1} AND {2}", ii, nPitchFrom, nPitchTo)
					sbSQL.AppendLine()
				Next
				sbSQL.AppendLine("    )")

				strSQL = sbSQL.ToString()

				' Select文実行
				sqlRead = tcls_DB.SelectExecute(strSQL)
				If sqlRead IsNot Nothing Then
					' データ取得
					sqlRead.Read()
					While sqlRead.Read() = True

						' データ取得
						Dim nWidDivPosNo As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("WidDivPosNo"))
						Dim nDefectNo As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("DefectNo"))
						Dim fXFromDSEdge As Single = sqlRead.GetFloat(sqlRead.GetOrdinal("XFromDSEdge"))
						Dim fXFromWSEdge As Single = sqlRead.GetFloat(sqlRead.GetOrdinal("XFromWSEdge"))

						Dim typGradeInf As New TYP_GRADE_INF
						typGradeInf.nEdasysID = sqlRead.GetInt32(sqlRead.GetOrdinal("GradeEdasysID"))
						typGradeInf.nPriority = sqlRead.GetInt32(sqlRead.GetOrdinal("GradePriority"))
						typGradeInf.strName = sqlRead.GetString(sqlRead.GetOrdinal("GradeName"))

						Dim lstCycle As New List(Of Double)
						For ii As Integer = 1 To MAX_LONGPD_FILTER
							Dim dTemp As Double = sqlRead.GetDouble(sqlRead.GetOrdinal(String.Format("Cycle_Filter{0}", ii)))
							If 0 <= dTemp AndAlso
								   nPitchFrom <= dTemp AndAlso
								   nPitchTo >= dTemp Then
								' 0以上が検出された値。負の値は未検出。
								' かつ炉チカのみが対象
								lstCycle.Add(dTemp)
							End If
						Next

						' 同一面・幅分割位置で周期ピッチに検出された周期の値が有れば集約する。
						For ii As Integer = 0 To lstCycle.Count - 1
							Dim bExist As Boolean = False

							For jj As Integer = 0 To lstLongPd.Count - 1
								If lstLongPd(jj).nTorB = nTorB AndAlso
									   lstLongPd(jj).nWidDivPosNo = nWidDivPosNo AndAlso
									   lstLongPd(jj).dCyclePitch - nTolerance <= lstCycle(ii) AndAlso
									   lstLongPd(jj).dCyclePitch + nTolerance >= lstCycle(ii) Then

									' 公差内は集約
									If 0 > lstLongPd(jj).lstGradeID.IndexOf(typGradeInf) Then
										lstLongPd(jj).lstGradeID.Add(typGradeInf)
										lstLongPd(jj).lstGradeID.Sort(AddressOf CompareGrade)
									End If

									bExist = True
									Exit For
								End If
							Next

							If False = bExist Then
								' 初めての周期は追加
								Dim typLongPd As New TYP_LONGPD_LIST
								typLongPd.Initialize()

								With typLongPd
									.nTorB = nTorB                                  ' 面
									.nWidDivPosNo = nWidDivPosNo                    ' 巾分割位置
									.dCyclePitch = lstCycle(ii)                     ' 周期判定ピッチ	※公差内はグループ化。
									.nDefNo = nDefectNo                             ' 疵No				※最初に見つかったもの
									.fDs = fXFromDSEdge                             ' DS位置			※最初に見つかったもの
									.fWs = fXFromWSEdge                             ' WS位置			※最初に見つかったもの
									.lstGradeID.Add(typGradeInf)
								End With

								lstLongPd.Add(typLongPd)
							End If
						Next
					End While
				End If

				If sqlRead IsNot Nothing Then
					sqlRead.Close()
				End If

				If bRet = False Then
					Exit Try
				End If
			Next


			' 取得データを並び替えて追加
			lstLongPd.Sort(AddressOf CompareValue)
			m_dicDispData.Add(m_strManagementNo, lstLongPd)

		Catch ex As Exception
			bRet = False
		Finally
			If sqlRead IsNot Nothing Then
				sqlRead.Close()
			End If
			sqlRead = Nothing
			tcls_DB.Close()
			tcls_DB.Dispose()
			tcls_DB = Nothing
		End Try

		Return bRet

	End Function

	''' <summary>
	''' リストの比較用
	''' </summary>
	''' <param name="x"></param>
	''' <param name="y"></param>
	''' <returns>
	''' x ＜ y ⇒ -1
	''' x ＝ y ⇒ 0
	''' x ＞ y ⇒ 1
	''' </returns>
	Private Function CompareValue(x As TYP_LONGPD_LIST, y As TYP_LONGPD_LIST) As Integer
		' 面
		If x.nTorB < y.nTorB Then
			Return -1
		ElseIf x.nTorB > y.nTorB Then
			Return 1
		Else
			' 巾分割位置
			If x.nWidDivPosNo < y.nWidDivPosNo Then
				Return -1
			ElseIf x.nWidDivPosNo > y.nWidDivPosNo Then
				Return 1
			Else
				' 周期判定ピッチ
				If x.dCyclePitch < y.dCyclePitch Then
					Return -1
				ElseIf x.dCyclePitch > y.dCyclePitch Then
					Return 1
				Else
					If True = m_bWsDs Then
						' WS ～
						If x.fWs < y.fWs Then
							Return -1
						ElseIf x.fWs > y.fWs Then
							Return 1
						Else
							' DS ～
							If x.fDs < y.fDs Then
								Return -1
							ElseIf x.fDs > y.fDs Then
								Return 1
							Else
								' 一致とする
								Return 0
							End If
						End If
					Else
						' DS ～
						If x.fDs < y.fDs Then
							Return -1
						ElseIf x.fDs > y.fDs Then
							Return 1
						Else
							' WS ～
							If x.fWs < y.fWs Then
								Return -1
							ElseIf x.fWs > y.fWs Then
								Return 1
							Else
								' 一致とする
								Return 0
							End If
						End If
					End If
				End If
			End If
		End If

		Return 0
	End Function

	''' <summary>
	''' グレードの比較用
	''' </summary>
	''' <param name="x"></param>
	''' <param name="y"></param>
	''' <returns></returns>
	Private Function CompareGrade(x As TYP_GRADE_INF, y As TYP_GRADE_INF) As Integer
		Dim nPriorityX As Integer = x.nPriority
		Dim nPriorityY As Integer = y.nPriority

		If nPriorityX < nPriorityY Then
			Return -1
		ElseIf nPriorityX > nPriorityY Then
			Return 1
		Else
			Return 0
		End If
	End Function


	''' <summary>
	''' 画面終了
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click

		'画面終了
		Me.Hide()

	End Sub

	''' <summary>
	''' CSV出力
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnCSV_Click(sender As Object, e As EventArgs) Handles btnCSV.Click
		Try
			CsvOut(CurDir, String.Format("長周期一覧_{0}.CSV", Now.ToString("yyyyMMddhhmmss")))
		Catch ex As Exception
		End Try
	End Sub

	''' <summary>
	''' CSV出力
	''' </summary>
	''' <param name="strFolder">保存ファイルのフォルダパス</param>
	''' <param name="strName">保存ファイルのファイル名</param>
	''' <param name="bDelComma">カンマを除去</param>
	''' <remarks></remarks>
	Private Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""                      '1行のデータ
		Dim strWk() As String
		Dim strWkData As String

		'' フルパス
		strFPath = IO.Path.Combine(strFolder, strName)

		'' フォルダの有無チェック(無かったら作ろう)
		tMod.CheckFolder(strFolder)

		If dgvLongPdList.ColumnCount <= 0 Then

			Exit Sub
		End If

		'' CSV出力
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' オープン
				cls_Stram.Open()

				'ヘッダを出力
				strData = ""
				For ii As Integer = 0 To dgvLongPdList.ColumnCount - 1

					'改行コードをすべて取り除く
					strWk = dgvLongPdList.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
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
				For ii As Integer = 0 To dgvLongPdList.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvLongPdList.ColumnCount - 1
						'項目をカンマで連結する
						If dgvLongPdList(jj, ii).Value IsNot Nothing Then
							If jj = EMLONGPD_COL_ID.iGr Then
								strData &= """"
							End If

							If True = bDelComma Then
								strData &= dgvLongPdList(jj, ii).Value.ToString.Replace(",", "")
							Else
								strData &= dgvLongPdList(jj, ii).Value.ToString
							End If
						End If

						If jj = EMLONGPD_COL_ID.iGr Then
							strData &= """"
						End If

						strData &= ","
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


	''' <summary>
	''' セル選択管理No
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLongPdList_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvLongPdList.SelectionChanged

		'選択を解除
		dgvLongPdList.CurrentCell.Selected = False

	End Sub

	''' <summary>
	''' 画面サイズ変更イベント
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub dgvLongPdList_SizeChanged(sender As Object, e As EventArgs) Handles dgvLongPdList.SizeChanged
		Try
			With dgvLongPdList
				Dim nWidth As Integer = .Width
				For nColIndex As Integer = 0 To .ColumnCount - 1
					If nColIndex <> EMLONGPD_COL_ID.iGr Then
						nWidth = nWidth - .Columns(nColIndex).Width
					End If
				Next
				nWidth = nWidth - SystemInformation.VerticalScrollBarWidth - SystemInformation.Border3DSize.Width * 2
				If nWidth < 60 Then
					nWidth = 60
				End If

				.Columns(EMLONGPD_COL_ID.iGr).Width = nWidth
			End With
		Catch ex As Exception

		End Try
	End Sub

	''' <summary>
	''' ツールボタン 表示Click処理
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub OnHandleDspClick(sender As Object, e As EventArgs)
		' 表示中なら消去
		If Me.Visible Then
			Me.Visible = False
			Return
		End If

		' 呼び出し元の上に表示
		Dim f As Form = Form.ActiveForm

		Me.Show(f)
	End Sub

	''' <summary>
	''' フォーム終了
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub frmPopN2TcmEdgeCrack_FormClosing(sender As Object, e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		If e.CloseReason = CloseReason.UserClosing Then
			e.Cancel = True
			Me.Visible = False
		End If
	End Sub

	''' <summary>
	''' 指定ボタンのツールバー上の位置を取得
	''' </summary>
	''' <param name="strip"></param>
	''' <param name="name"></param>
	''' <returns></returns>
	Function GetItemIndex(strip As ToolStrip, name As String) As Integer
		Dim items() As ToolStripItem = strip.Items.Find(name, False)
		If items.Length = 0 Then
			Return strip.Items.Count
		End If
		Return strip.Items.IndexOf(items(0))
	End Function

	''' <summary>
	''' 反映
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnSet_Click(sender As Object, e As EventArgs) Handles btnSet.Click

		tMod.WriteIni(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_FROM", RPI_VIEWER.modCommon.LOCALINI_NAME, nudFrom.Value.ToString)
		tMod.WriteIni(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_TO", RPI_VIEWER.modCommon.LOCALINI_NAME, nudTo.Value.ToString)
		tMod.WriteIni(LOCALINI_SECTION, "TOLERANCE", RPI_VIEWER.modCommon.LOCALINI_NAME, nudTolerance.Value.ToString)

		'データ取得
		If Not GetData() Then
			Exit Sub
		End If

		'データ表示
		Call DspData()

	End Sub
End Class
