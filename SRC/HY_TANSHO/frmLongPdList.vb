'*********************************************************************************
'　疵混入率画面クラス
'	[Ver]
'		Ver.01    2007/09/22  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Imports System.Text
Public Class frmLongPdList

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Const DgvMaxCol_MRare As Integer = 6            'データ列MAX

	' グリッド　列設定
	Private Enum EMLONGPD_COL_ID
		iTorB = 0               ' 面
		iWidDivPosNo            ' 巾分割位置
		iWS                     ' WS位置
		iDS                     ' DS位置
		iCyclePitch             ' 周期判定ピッチ
		iGr                     ' Gr
	End Enum

	Private Structure TYP_LONGPD_LIST
		Dim nTorB As Integer                        ' 面
		Dim nWidDivPosNo As Integer                 ' 巾分割位置
		Dim dCyclePitch As Double                   ' 周期判定ピッチ	※公差内はグループ化。

		Dim nDefNo As Integer                       ' 疵No				※最初に見つかったもの
		Dim fDs As Single                           ' DS位置			※最初に見つかったもの
		Dim fWs As Single                           ' WS位置			※最初に見つかったもの
		Dim lstGradeID As List(Of Integer)          ' グレードEdasysID

		Public Sub Initialize()
			lstGradeID = New List(Of Integer)
		End Sub
	End Structure

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private mcls_Log As tClass.tClass_LogManager    'ログ管理クラス
	Private mcls_Param As clsParamManager           'パラメータ管理クラス
	Private m_lstManagementNo As List(Of String)    '管理Noリスト
	Private m_InitFormSize As Size                  '画面初期化サイズ

	Private m_dicDispData As Dictionary(Of String, List(Of TYP_LONGPD_LIST))
	Private m_bTanshoMode As Boolean                ' 画面表示モード (True:探傷画面 False:実績画面)
	Private m_bWsDs As Boolean                      ' True: 左WS 右DS, False:左DS 右WS

	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <param name="cls_Log"></param>
	''' <remarks></remarks>
	Public Sub New(ByRef cls_Log As tClass.tClass_LogManager, ByRef cls_Param As clsParamManager, Optional ByVal bMode As Boolean = True)

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。

		Me.Text = String.Format("[{0}]", SYSTEM_TITLE) + Me.Text
		m_InitFormSize = Me.Size

		mcls_Log = cls_Log
		mcls_Param = cls_Param
		m_dicDispData = New Dictionary(Of String, List(Of TYP_LONGPD_LIST))
		m_bTanshoMode = bMode
		m_bWsDs = True

		'-------------------------------------------------------------------
		' ファンクションキー設定
		'-------------------------------------------------------------------
		AddFunctionButton(btnCSV, 9 - 1)
		AddFunctionButton(btnEnd, 12 - 1)

	End Sub

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
	Public Function PreDispProc(ByVal lstKanriNo As List(Of String), ByVal lstCoilNo As List(Of String), ByVal bWsDs As Boolean) As Boolean
		m_lstManagementNo = lstKanriNo
		m_bWsDs = bWsDs

		'画面初期化
		Call InitDsp(lstCoilNo)

		'データ取得
		If Not GetData() Then
			Return False
		End If

		'データ表示
		If cmbID.Text <> "" Then
			Call DspData(cmbID.SelectedIndex)
		End If

		Return True
	End Function


	''' <summary>
	''' データ表示
	''' </summary>
	''' <param name="nIndex">コンボのインデックス</param>
	''' <remarks></remarks>
	Private Sub DspData(ByVal nIndex As Integer)

		If 0 > nIndex Or nIndex > m_lstManagementNo.Count - 1 Then
			' 未選択は何もしない
			Exit Sub
		End If

		' データを表示する。
		If False = m_dicDispData.ContainsKey(m_lstManagementNo(nIndex)) Then
			' 該当データなし
			Exit Sub
		End If

		Dim lstVal As List(Of TYP_LONGPD_LIST) = m_dicDispData(m_lstManagementNo(nIndex))

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
						strName = strName + mcls_Param.GetGradeName(lstVal(ii).lstGradeID(jj))
					Next
					dgvLongPdList(EMLONGPD_COL_ID.iGr, ii).Value = strName

				Next
			End With
		Catch ex As Exception
			mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "長周期欠陥一覧表示エラー [" & ex.Message & "]")
		Finally
			dgvLongPdList.ResumeLayout()
		End Try
	End Sub

	''' <summary>
	''' 画面初期化
	''' </summary>
	''' <remarks></remarks>
	Private Sub InitDsp(ByVal lstCoil As List(Of String))

		If True = m_bTanshoMode Then
			' 探傷画面
			lblCoilNo.Visible = True
			cmbID.Visible = True
			btnCSV.Visible = False
			btnCSV.Enabled = False
		Else
			' 実績画面
			lblCoilNo.Visible = False
			cmbID.Visible = False
			btnCSV.Visible = True
			btnCSV.Enabled = True
		End If

		' 画面サイズ・表示位置
		Me.Size = m_InitFormSize
		Me.Left = (Owner.Width - Me.Width) / 2
		Me.Top = (Owner.Height - Me.Height) / 2

		'疵混入率表 初期化
		dgvInitialize(dgvLongPdList)

		'コイルNo
		cmbID.Items.Clear()
		If lstCoil.Count > 0 Then
			For ii As Integer = 0 To lstCoil.Count - 1
				cmbID.Items.Add(lstCoil(ii))
			Next

			cmbID.SelectedIndex = 0
		End If
	End Sub


	''' <summary>
	''' データ取得
	''' </summary>
	''' <remarks></remarks>
	Private Function GetData() As Boolean

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String    'SQL文
		Dim bRet As Boolean = False
		Dim strTableLongPd() As String = {DB_LONGPD_EXEC_RESULT_DEFECT_T, DB_LONGPD_EXEC_RESULT_DEFECT_B}
		Dim strTableDefect() As String = {DB_DEFECT_INF_T, DB_DEFECT_INF_B}
		Dim nPitchFrom As Integer = mcls_Param.Pro_LongPd_NearFurnaceRangeFrom
		Dim nPitchTo As Integer = mcls_Param.Pro_LongPd_NearFurnaceRangeTo
		Dim nTolerance As Integer = mcls_Param.Pro_LongPd_PeriodicPitchTolerance

		' 初期化
		m_dicDispData.Clear()

		'DBオープン
		If Not tcls_DB.Open() Then
			mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]")
			Return False
		End If

		Try
			bRet = True
			For nListIndex As Integer = 0 To m_lstManagementNo.Count - 1
				Dim lstLongPd As New List(Of TYP_LONGPD_LIST)

				For nTorB As Integer = 0 To NUM_MEN - 1

					' SQL生成
					Dim sbSQL As New StringBuilder

					sbSQL.AppendLine("")
					sbSQL.AppendLine("SELECT ")
					sbSQL.AppendLine("     L.WidDivPosNo")
					sbSQL.AppendLine("    ,L.DefectNo")
					sbSQL.AppendLine("    ,I.DS位置 As XFromDSEdge")
					sbSQL.AppendLine("    ,I.WS位置 As XFromWSEdge")
					sbSQL.AppendLine("    ,G.EdasysID As GradeEdasysID")
					For ii As Integer = 1 To MAX_LONGPD_FILTER
						sbSQL.AppendFormat("    ,FLOOR(L.Cycle_Filter{0}) As Cycle_Filter{0}", ii)
						sbSQL.AppendLine()
					Next
					sbSQL.AppendLine("FROM")
					sbSQL.AppendFormat("    {0} L", strTableLongPd(nTorB))
					sbSQL.AppendLine()
					sbSQL.AppendFormat("    INNER JOIN {0} I", strTableDefect(nTorB))
					sbSQL.AppendLine()
					sbSQL.AppendLine("        ON  L.ManagementNo = I.管理No")
					sbSQL.AppendLine("        AND L.DefectNo = I.疵No")
					sbSQL.AppendFormat("    INNER JOIN {0} G", DB_GRADE_NAME)
					sbSQL.AppendLine()
					sbSQL.AppendLine("        ON I.判定グレード = G.EdasysID")
					sbSQL.AppendLine("WHERE")
					sbSQL.AppendFormat("        L.ManagementNo = '{0}'", m_lstManagementNo(nListIndex))
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
							Dim nGradeEdasysID As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("GradeEdasysID"))
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
										If 0 > lstLongPd(jj).lstGradeID.IndexOf(nGradeEdasysID) Then
											lstLongPd(jj).lstGradeID.Add(nGradeEdasysID)
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
										.lstGradeID.Add(nGradeEdasysID)
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
						Exit For
					End If
				Next

				If bRet = False Then
					Exit For
				End If

				' 取得データを並び替えて追加
				lstLongPd.Sort(AddressOf CompareValue)
				m_dicDispData.Add(m_lstManagementNo(nListIndex), lstLongPd)
			Next

		Catch ex As Exception
			mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "長周期欠陥一覧取得エラー [" & ex.Message & "]")
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
	Private Function CompareGrade(x As Integer, y As Integer) As Integer
		Dim nPriorityX As Integer = mcls_Param.GetGradePriority(x)
		Dim nPriorityY As Integer = mcls_Param.GetGradePriority(y)

		If nPriorityX < nPriorityY Then
			Return -1
		ElseIf nPriorityX > nPriorityY Then
			Return 1
		Else
			Return 0
		End If
	End Function


	'***************************************************************************
	' キーイベント
	'***************************************************************************
	''' <summary>
	''' ファンクションボタン
	''' </summary>
	''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
	''' <param name="onKey"></param>
	Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		Try
			Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
				'---------------------------------------------------------------
				Case 9 - 1                                                     ' CSV出力
					If btnCSV.Visible = True Then
						btnCSV_Click(Nothing, Nothing)
					End If
				'---------------------------------------------------------------
				Case 12 - 1                                                     ' 終了
					btnEnd_Click(Nothing, Nothing)
			End Select
		Catch ex As Exception
		End Try
	End Sub

	''' <summary>
	''' 画面終了
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

		'画面終了
		Me.Hide()

	End Sub

	''' <summary>
	''' CSV出力
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnCSV_Click(sender As Object, e As EventArgs)
		Try
			CsvOut(CSV_PATH, String.Format("長周期一覧_{0}.CSV", cmbID.Text.TrimEnd))
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
	''' コンボ選択
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub cmbID_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbID.SelectedIndexChanged
		DspData(cmbID.SelectedIndex)
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


End Class
