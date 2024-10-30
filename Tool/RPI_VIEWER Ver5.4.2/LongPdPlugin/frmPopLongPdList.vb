Imports System.Windows.Forms
Imports System.Drawing
Imports System.Data.SqlClient
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class frmPopLongPdList
	Implements RpiPlugin.IPluginActivatable, RpiPlugin.IToolStrip, RpiPlugin.IDrawer, RpiPlugin.IRpiOpen

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' SQL���[�h
	''' </summary>
	Private Enum EM_SQLMODE As Integer
		NONE = -1               ' �ΏۊO
		KIZUKEN                 ' �r���Ǘ�No, DEFECT_INF
		KANRI                   ' �Ǘ�No, T_DEFECT_INF
		MANEGEMENT              ' ManagementNo, T_DEFECT_INF
	End Enum

	''' <summary>�v���O�C���p Ini�t�@�C��</summary>
	Private Const TASKINI_LONGPD As String = ".\LongPdPlugin.ini"

	''' <summary>���[�J���p Ini�L�[</summary>
	Private Const LOCALINI_SECTION As String = "LongPdList"

	Private Const DgvMaxCol_MRare As Integer = 6            '�f�[�^��MAX
	Private Const MAX_LONGPD_FILTER As Integer = 16

	' �O���b�h�@��ݒ�
	Private Enum EMLONGPD_COL_ID
		iTorB = 0               ' ��
		iWidDivPosNo            ' �Е����ʒu
		iWS                     ' WS�ʒu
		iDS                     ' DS�ʒu
		iCyclePitch             ' ��������s�b�`
		iGr                     ' Gr
	End Enum

	Private Structure TYP_GRADE_INF
		Dim nEdasysID As Integer
		Dim nPriority As Integer
		Dim strName As String
	End Structure

	Private Structure TYP_LONGPD_LIST
		Dim nTorB As Integer                        ' ��
		Dim nWidDivPosNo As Integer                 ' �Е����ʒu
		Dim dCyclePitch As Double                   ' ��������s�b�`	���������̓O���[�v���B

		Dim nDefNo As Integer                       ' �rNo				���ŏ��Ɍ�����������
		Dim fDs As Single                           ' DS�ʒu			���ŏ��Ɍ�����������
		Dim fWs As Single                           ' WS�ʒu			���ŏ��Ɍ�����������
		Dim lstGradeID As List(Of TYP_GRADE_INF)    ' �O���[�h���

		Public Sub Initialize()
			lstGradeID = New List(Of TYP_GRADE_INF)
		End Sub
	End Structure

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private mcls_RpiView As tClass_ViewRpiThread
	Private mcls_ReadRpi As tClass_ReadRPI

	Private m_strManagementNo As String             '�Ǘ�No

	Private m_dicDispData As Dictionary(Of String, List(Of TYP_LONGPD_LIST))
	Private m_bWsDs As Boolean                      ' True: ��WS �EDS, False:��DS �EWS
	Private m_nTimeOut As Integer = 30
	Private m_strCsvPath As String                          ' CSV�o�̓t�H���_
	Private m_strCsvFile As String                          ' CSV�o�̓t�@�C����


#Region "�C���^�[�t�F�[�X����"

	' RpiPlugin.IPluginActivatable		: RpiViewer�ɂ����New�����
	' RpiPlugin.IToolStrip				: �c�[���o�[�̃{�^���ǉ�
	' RpiPlugin.IDrawer					: �I�[�o�[���C�`��
	' RpiPlugin.IRpiOpen				: RPI�t�@�C����Open����/����O��Call�����

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
	''' Plugin������
	''' </summary>
	''' <param name="v"></param>
	''' <param name="mainfrm"></param>
	Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
		mcls_RpiView = v

		' �ݒ�̓Ǎ�
		m_nTimeOut = tMod.ReadIniInt(LOCALINI_SECTION, "TIME_OUT", TASKINI_LONGPD, 30)
		m_strCsvPath = tMod.ReadIniStr(LOCALINI_SECTION, "CSV_PATH", TASKINI_LONGPD, ".\\")
		m_strCsvFile = tMod.ReadIniStr(LOCALINI_SECTION, "CSV_FILE", TASKINI_LONGPD, "���������׈ꗗ.CSV")

		nudFrom.Value = tMod.ReadIniInt(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_FROM", RPI_VIEWER.modCommon.LOCALINI_NAME, 2470)
		nudTo.Value = tMod.ReadIniInt(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_TO", RPI_VIEWER.modCommon.LOCALINI_NAME, 2600)
		nudTolerance.Value = tMod.ReadIniInt(LOCALINI_SECTION, "TOLERANCE", RPI_VIEWER.modCommon.LOCALINI_NAME, 0)
	End Sub

#End Region

#Region "RpiPlugin.IToolStrip"

	Private m_tsbLongPdList As New System.Windows.Forms.ToolStripButton()

	''' <summary>
	''' �c�[���o�[������
	''' </summary>
	''' <param name="mainstrip"></param>
	Public Sub InitToolbar(mainstrip() As ToolStrip) Implements IToolStrip.InitToolbar

		' �M��DL�\���{�^���ǉ�
		With m_tsbLongPdList
			.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
			.Image = LabelIcon.Image
			.ImageTransparentColor = System.Drawing.Color.White
			.Name = "tsbLongPdList"
			.Size = New System.Drawing.Size(23, 23)
			.Text = "���������׈ꗗ"
			AddHandler .Click, AddressOf OnHandleDspClick
		End With

		Dim ipos As Integer = GetItemIndex(mainstrip(0), "tsbKizuList")
		mainstrip(0).Items.Insert(ipos + 1, m_tsbLongPdList)
	End Sub

#End Region

#Region "RpiPlugin.IRpiOpen"

	''' <summary>
	''' RPI�t�@�C��Open����
	''' </summary>
	''' <param name="rpi"></param>
	Public Sub OnRpiOpen(rpi As tClass_ReadRPI) Implements IRpiOpen.OnRpiOpen
		mcls_ReadRpi = rpi

		m_dicDispData = New Dictionary(Of String, List(Of TYP_LONGPD_LIST))

		PreDispProc(mcls_ReadRpi.GetRpiTHeader.cKizukenNo, mcls_RpiView.GetProp_DSWS)

	End Sub

	''' <summary>
	''' RPI�t�@�C��Close�O
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
	''' �S���摜�ɕ`��
	''' </summary>
	''' <param name="g"></param>
	Public Sub OnDraw(g As System.Drawing.Graphics) Implements IDrawer.OnDraw

	End Sub
#End Region

#End Region


	''' <summary>
	''' �O���b�h�̏�����
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
		Dim ii As Integer = 0
		With dgv
			.RowCount = 0

			'' �����`
			.AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
			.AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
			.AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
			.AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
			.AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
			.AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
			.ShowCellErrors = False
			.ShowCellToolTips = False
			.ShowEditingIcon = False
			.ShowRowErrors = False
			.ScrollBars = ScrollBars.Both


			.SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �Z���I��
			.MultiSelect = False                                            ' �����s�I��
			.ReadOnly = True                                                ' �ǂݎ���p
			.EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

			.ColumnHeadersVisible = True                                    ' ��w�b�_�\��
			.RowHeadersVisible = False                                      ' �s�w�b�_�\��
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

			'' �I���Z���F��`
			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' �I��
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			'' ���`
			.ColumnCount = DgvMaxCol_MRare           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.ColumnHeadersHeight = 40                                       ' ��w�b�_�T�C�Y

			'��P ���ڌ��o��
			.Columns(EMLONGPD_COL_ID.iTorB).HeaderText = "��"
			.Columns(EMLONGPD_COL_ID.iTorB).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(EMLONGPD_COL_ID.iTorB).Width = 40
			.Columns(EMLONGPD_COL_ID.iTorB).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iTorB).Frozen = True                                       ' �����ړ��֎~

			'��Q ���ڌ��o��
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).HeaderText = "�Е����ʒu" & vbCrLf & "(DS 0�`7 WS)"
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).Width = 120
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iWidDivPosNo).Frozen = True                                       ' �����ړ��֎~

			'��R ���ڌ��o��
			.Columns(EMLONGPD_COL_ID.iWS).HeaderText = IIf(m_bWsDs = True, "WS�`[mm]", "DS�`[mm]")
			.Columns(EMLONGPD_COL_ID.iWS).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(EMLONGPD_COL_ID.iWS).Width = 70
			.Columns(EMLONGPD_COL_ID.iWS).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iWS).Frozen = False

			'��S ���ڌ��o��
			.Columns(EMLONGPD_COL_ID.iDS).HeaderText = IIf(m_bWsDs = True, "DS�`[mm]", "WS�`[mm]")
			.Columns(EMLONGPD_COL_ID.iDS).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(EMLONGPD_COL_ID.iDS).Width = 70
			.Columns(EMLONGPD_COL_ID.iDS).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iDS).Frozen = False                                       ' �����ړ��֎~

			'��T ���ڌ��o��
			.Columns(EMLONGPD_COL_ID.iCyclePitch).HeaderText = "��������" & vbCrLf & "�߯�[mm]"
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

			'��	�U ���ڌ��o��
			.Columns(EMLONGPD_COL_ID.iGr).HeaderText = "Gr"
			.Columns(EMLONGPD_COL_ID.iGr).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(EMLONGPD_COL_ID.iGr).Width = nWidth
			.Columns(EMLONGPD_COL_ID.iGr).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(EMLONGPD_COL_ID.iGr).Frozen = False

		End With
	End Sub


	''' <summary>
	''' �\���O����
	''' </summary>
	Public Function PreDispProc(ByVal strKanriNo As String, ByVal bWsDs As Boolean) As Boolean
		m_strManagementNo = strKanriNo
		m_bWsDs = bWsDs

		'��ʏ�����
		Call InitDsp()

		'�f�[�^�擾
		If Not GetData() Then
			Return False
		End If

		'�f�[�^�\��
		Call DspData()

		Return True
	End Function


	''' <summary>
	''' �f�[�^�\��
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspData()
		' �f�[�^��\������B
		If "" = m_strManagementNo Then
			' �Y���f�[�^�Ȃ�
			Exit Sub
		End If

		Dim lstVal As List(Of TYP_LONGPD_LIST) = m_dicDispData(m_strManagementNo)

		Try
			With dgvLongPdList
				.SuspendLayout()
				.RowCount = 0
				.RowCount = lstVal.Count

				For ii As Integer = 0 To lstVal.Count - 1
					dgvLongPdList(EMLONGPD_COL_ID.iTorB, ii).Value = IIf(0 = lstVal(ii).nTorB, "�\", "��")
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
	''' ��ʏ�����
	''' </summary>
	''' <remarks></remarks>
	Private Sub InitDsp()
		'������
		dgvInitialize(dgvLongPdList)
	End Sub


	''' <summary>
	''' �f�[�^�擾
	''' </summary>
	''' <remarks></remarks>
	Private Function GetData() As Boolean

		Dim tcls_DB As tClass.tClass_SQLServer
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String    'SQL��
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

		' ������
		m_dicDispData.Clear()

		' ���[�h�擾
		Dim nMode As Integer = tMod.ReadIniInt(mcls_ReadRpi.GetRpiTHeader.cSysName, "SQL_MODE", TASKINI_LONGPD, EM_SQLMODE.NONE)
		ReDim strTableDefect(2 - 1)
		If EM_SQLMODE.KIZUKEN = nMode Then
			' �r���Ǘ�No
			strTableDefect(0) = "DEFECT_INF_T"
			strTableDefect(1) = "DEFECT_INF_B"
			strTableGrade = "GRADE_NAME"
			strKeyKanriNo = "�r���Ǘ�No"
			strKeyKizuNo = "�rNo"
			strDataDS = "DS�ʒu"
			strDataWS = "WS�ʒu"
			strDataDefectGrade = "����O���[�h"
			strDataGradeName = "Name"
			strDataGradePriority = "�D��x"

		ElseIf EM_SQLMODE.KANRI = nMode Then
			' �Ǘ�No
			strTableDefect(0) = "T_DEFECT_INF_T"
			strTableDefect(1) = "T_DEFECT_INF_B"
			strTableGrade = "GRADE_NAME"
			strKeyKanriNo = "�Ǘ�No"
			strKeyKizuNo = "�rNo"
			strDataDS = "DS�ʒu"
			strDataWS = "WS�ʒu"
			strDataDefectGrade = "����O���[�h"
			strDataGradeName = "Name"
			strDataGradePriority = "�D��x"

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
			' �ΏۊO
			Return False
		End If


		Dim liInfo As RPI_VIEWER.LineInfo = RPI_VIEWER.ParamLine.Find(mcls_ReadRpi.GetRpiTHeader.cSysName)
		tcls_DB = liInfo.CreateSqlConnection

		'DB�I�[�v��
		tcls_DB.SQLTimeOut = m_nTimeOut
		If Not tcls_DB.Open() Then
			Return False
		End If

		Try
			bRet = True
			Dim lstLongPd As New List(Of TYP_LONGPD_LIST)

			For nTorB As Integer = 0 To 2 - 1

				' SQL����
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

				' Select�����s
				sqlRead = tcls_DB.SelectExecute(strSQL)
				If sqlRead IsNot Nothing Then
					' �f�[�^�擾
					sqlRead.Read()
					While sqlRead.Read() = True

						' �f�[�^�擾
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
								' 0�ȏオ���o���ꂽ�l�B���̒l�͖����o�B
								' ���F�`�J�݂̂��Ώ�
								lstCycle.Add(dTemp)
							End If
						Next

						' ����ʁE�������ʒu�Ŏ����s�b�`�Ɍ��o���ꂽ�����̒l���L��ΏW�񂷂�B
						For ii As Integer = 0 To lstCycle.Count - 1
							Dim bExist As Boolean = False

							For jj As Integer = 0 To lstLongPd.Count - 1
								If lstLongPd(jj).nTorB = nTorB AndAlso
									   lstLongPd(jj).nWidDivPosNo = nWidDivPosNo AndAlso
									   lstLongPd(jj).dCyclePitch - nTolerance <= lstCycle(ii) AndAlso
									   lstLongPd(jj).dCyclePitch + nTolerance >= lstCycle(ii) Then

									' �������͏W��
									If 0 > lstLongPd(jj).lstGradeID.IndexOf(typGradeInf) Then
										lstLongPd(jj).lstGradeID.Add(typGradeInf)
										lstLongPd(jj).lstGradeID.Sort(AddressOf CompareGrade)
									End If

									bExist = True
									Exit For
								End If
							Next

							If False = bExist Then
								' ���߂Ă̎����͒ǉ�
								Dim typLongPd As New TYP_LONGPD_LIST
								typLongPd.Initialize()

								With typLongPd
									.nTorB = nTorB                                  ' ��
									.nWidDivPosNo = nWidDivPosNo                    ' �Е����ʒu
									.dCyclePitch = lstCycle(ii)                     ' ��������s�b�`	���������̓O���[�v���B
									.nDefNo = nDefectNo                             ' �rNo				���ŏ��Ɍ�����������
									.fDs = fXFromDSEdge                             ' DS�ʒu			���ŏ��Ɍ�����������
									.fWs = fXFromWSEdge                             ' WS�ʒu			���ŏ��Ɍ�����������
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


			' �擾�f�[�^����ёւ��Ēǉ�
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
	''' ���X�g�̔�r�p
	''' </summary>
	''' <param name="x"></param>
	''' <param name="y"></param>
	''' <returns>
	''' x �� y �� -1
	''' x �� y �� 0
	''' x �� y �� 1
	''' </returns>
	Private Function CompareValue(x As TYP_LONGPD_LIST, y As TYP_LONGPD_LIST) As Integer
		' ��
		If x.nTorB < y.nTorB Then
			Return -1
		ElseIf x.nTorB > y.nTorB Then
			Return 1
		Else
			' �Е����ʒu
			If x.nWidDivPosNo < y.nWidDivPosNo Then
				Return -1
			ElseIf x.nWidDivPosNo > y.nWidDivPosNo Then
				Return 1
			Else
				' ��������s�b�`
				If x.dCyclePitch < y.dCyclePitch Then
					Return -1
				ElseIf x.dCyclePitch > y.dCyclePitch Then
					Return 1
				Else
					If True = m_bWsDs Then
						' WS �`
						If x.fWs < y.fWs Then
							Return -1
						ElseIf x.fWs > y.fWs Then
							Return 1
						Else
							' DS �`
							If x.fDs < y.fDs Then
								Return -1
							ElseIf x.fDs > y.fDs Then
								Return 1
							Else
								' ��v�Ƃ���
								Return 0
							End If
						End If
					Else
						' DS �`
						If x.fDs < y.fDs Then
							Return -1
						ElseIf x.fDs > y.fDs Then
							Return 1
						Else
							' WS �`
							If x.fWs < y.fWs Then
								Return -1
							ElseIf x.fWs > y.fWs Then
								Return 1
							Else
								' ��v�Ƃ���
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
	''' �O���[�h�̔�r�p
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
	''' ��ʏI��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click

		'��ʏI��
		Me.Hide()

	End Sub

	''' <summary>
	''' CSV�o��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnCSV_Click(sender As Object, e As EventArgs) Handles btnCSV.Click
		Try
			CsvOut(CurDir, String.Format("�������ꗗ_{0}.CSV", Now.ToString("yyyyMMddhhmmss")))
		Catch ex As Exception
		End Try
	End Sub

	''' <summary>
	''' CSV�o��
	''' </summary>
	''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
	''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
	''' <param name="bDelComma">�J���}������</param>
	''' <remarks></remarks>
	Private Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""                      '1�s�̃f�[�^
		Dim strWk() As String
		Dim strWkData As String

		'' �t���p�X
		strFPath = IO.Path.Combine(strFolder, strName)

		'' �t�H���_�̗L���`�F�b�N(�����������낤)
		tMod.CheckFolder(strFolder)

		If dgvLongPdList.ColumnCount <= 0 Then

			Exit Sub
		End If

		'' CSV�o��
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' �I�[�v��
				cls_Stram.Open()

				'�w�b�_���o��
				strData = ""
				For ii As Integer = 0 To dgvLongPdList.ColumnCount - 1

					'���s�R�[�h�����ׂĎ�菜��
					strWk = dgvLongPdList.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
					strWkData = ""
					For jj As Integer = 0 To strWk.Length - 1
						strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
					Next jj

					strData &= Trim(strWkData) & ","
				Next ii

				'�Ō�̃^�u��r��
				strData = strData.Remove(strData.Length - 1, 1)

				cls_Stram.WriteLine(strData)

				'�f�[�^�o��
				For ii As Integer = 0 To dgvLongPdList.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvLongPdList.ColumnCount - 1
						'���ڂ��J���}�ŘA������
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

					'�Ō�̃^�u��r��
					strData = strData.Remove(strData.Length - 1, 1)

					cls_Stram.WriteLine(strData)
				Next ii

				' �N���[�Y
				cls_Stram.Close()
				strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

			Catch ex As Exception
				strMsg = "CSV�o�͂Ɏ��s���܂����B"
			End Try
		End Using

		'''' �K�C�_���X�\��
		Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
			frmMsg.ShowDialog()
		End Using
	End Sub


	''' <summary>
	''' �Z���I���Ǘ�No
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLongPdList_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvLongPdList.SelectionChanged

		'�I��������
		dgvLongPdList.CurrentCell.Selected = False

	End Sub

	''' <summary>
	''' ��ʃT�C�Y�ύX�C�x���g
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
	''' �c�[���{�^�� �\��Click����
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub OnHandleDspClick(sender As Object, e As EventArgs)
		' �\�����Ȃ����
		If Me.Visible Then
			Me.Visible = False
			Return
		End If

		' �Ăяo�����̏�ɕ\��
		Dim f As Form = Form.ActiveForm

		Me.Show(f)
	End Sub

	''' <summary>
	''' �t�H�[���I��
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
	''' �w��{�^���̃c�[���o�[��̈ʒu���擾
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
	''' ���f
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnSet_Click(sender As Object, e As EventArgs) Handles btnSet.Click

		tMod.WriteIni(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_FROM", RPI_VIEWER.modCommon.LOCALINI_NAME, nudFrom.Value.ToString)
		tMod.WriteIni(LOCALINI_SECTION, "NEAR_FURNACE_RANGE_TO", RPI_VIEWER.modCommon.LOCALINI_NAME, nudTo.Value.ToString)
		tMod.WriteIni(LOCALINI_SECTION, "TOLERANCE", RPI_VIEWER.modCommon.LOCALINI_NAME, nudTolerance.Value.ToString)

		'�f�[�^�擾
		If Not GetData() Then
			Exit Sub
		End If

		'�f�[�^�\��
		Call DspData()

	End Sub
End Class
