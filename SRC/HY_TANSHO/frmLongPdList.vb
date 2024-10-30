'*********************************************************************************
'�@�r��������ʃN���X
'	[Ver]
'		Ver.01    2007/09/22  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Imports System.Text
Public Class frmLongPdList

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Const DgvMaxCol_MRare As Integer = 6            '�f�[�^��MAX

	' �O���b�h�@��ݒ�
	Private Enum EMLONGPD_COL_ID
		iTorB = 0               ' ��
		iWidDivPosNo            ' �Е����ʒu
		iWS                     ' WS�ʒu
		iDS                     ' DS�ʒu
		iCyclePitch             ' ��������s�b�`
		iGr                     ' Gr
	End Enum

	Private Structure TYP_LONGPD_LIST
		Dim nTorB As Integer                        ' ��
		Dim nWidDivPosNo As Integer                 ' �Е����ʒu
		Dim dCyclePitch As Double                   ' ��������s�b�`	���������̓O���[�v���B

		Dim nDefNo As Integer                       ' �rNo				���ŏ��Ɍ�����������
		Dim fDs As Single                           ' DS�ʒu			���ŏ��Ɍ�����������
		Dim fWs As Single                           ' WS�ʒu			���ŏ��Ɍ�����������
		Dim lstGradeID As List(Of Integer)          ' �O���[�hEdasysID

		Public Sub Initialize()
			lstGradeID = New List(Of Integer)
		End Sub
	End Structure

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private mcls_Log As tClass.tClass_LogManager    '���O�Ǘ��N���X
	Private mcls_Param As clsParamManager           '�p�����[�^�Ǘ��N���X
	Private m_lstManagementNo As List(Of String)    '�Ǘ�No���X�g
	Private m_InitFormSize As Size                  '��ʏ������T�C�Y

	Private m_dicDispData As Dictionary(Of String, List(Of TYP_LONGPD_LIST))
	Private m_bTanshoMode As Boolean                ' ��ʕ\�����[�h (True:�T����� False:���щ��)
	Private m_bWsDs As Boolean                      ' True: ��WS �EDS, False:��DS �EWS

	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <param name="cls_Log"></param>
	''' <remarks></remarks>
	Public Sub New(ByRef cls_Log As tClass.tClass_LogManager, ByRef cls_Param As clsParamManager, Optional ByVal bMode As Boolean = True)

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

		Me.Text = String.Format("[{0}]", SYSTEM_TITLE) + Me.Text
		m_InitFormSize = Me.Size

		mcls_Log = cls_Log
		mcls_Param = cls_Param
		m_dicDispData = New Dictionary(Of String, List(Of TYP_LONGPD_LIST))
		m_bTanshoMode = bMode
		m_bWsDs = True

		'-------------------------------------------------------------------
		' �t�@���N�V�����L�[�ݒ�
		'-------------------------------------------------------------------
		AddFunctionButton(btnCSV, 9 - 1)
		AddFunctionButton(btnEnd, 12 - 1)

	End Sub

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
	Public Function PreDispProc(ByVal lstKanriNo As List(Of String), ByVal lstCoilNo As List(Of String), ByVal bWsDs As Boolean) As Boolean
		m_lstManagementNo = lstKanriNo
		m_bWsDs = bWsDs

		'��ʏ�����
		Call InitDsp(lstCoilNo)

		'�f�[�^�擾
		If Not GetData() Then
			Return False
		End If

		'�f�[�^�\��
		If cmbID.Text <> "" Then
			Call DspData(cmbID.SelectedIndex)
		End If

		Return True
	End Function


	''' <summary>
	''' �f�[�^�\��
	''' </summary>
	''' <param name="nIndex">�R���{�̃C���f�b�N�X</param>
	''' <remarks></remarks>
	Private Sub DspData(ByVal nIndex As Integer)

		If 0 > nIndex Or nIndex > m_lstManagementNo.Count - 1 Then
			' ���I���͉������Ȃ�
			Exit Sub
		End If

		' �f�[�^��\������B
		If False = m_dicDispData.ContainsKey(m_lstManagementNo(nIndex)) Then
			' �Y���f�[�^�Ȃ�
			Exit Sub
		End If

		Dim lstVal As List(Of TYP_LONGPD_LIST) = m_dicDispData(m_lstManagementNo(nIndex))

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
						strName = strName + mcls_Param.GetGradeName(lstVal(ii).lstGradeID(jj))
					Next
					dgvLongPdList(EMLONGPD_COL_ID.iGr, ii).Value = strName

				Next
			End With
		Catch ex As Exception
			mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���������׈ꗗ�\���G���[ [" & ex.Message & "]")
		Finally
			dgvLongPdList.ResumeLayout()
		End Try
	End Sub

	''' <summary>
	''' ��ʏ�����
	''' </summary>
	''' <remarks></remarks>
	Private Sub InitDsp(ByVal lstCoil As List(Of String))

		If True = m_bTanshoMode Then
			' �T�����
			lblCoilNo.Visible = True
			cmbID.Visible = True
			btnCSV.Visible = False
			btnCSV.Enabled = False
		Else
			' ���щ��
			lblCoilNo.Visible = False
			cmbID.Visible = False
			btnCSV.Visible = True
			btnCSV.Enabled = True
		End If

		' ��ʃT�C�Y�E�\���ʒu
		Me.Size = m_InitFormSize
		Me.Left = (Owner.Width - Me.Width) / 2
		Me.Top = (Owner.Height - Me.Height) / 2

		'�r�������\ ������
		dgvInitialize(dgvLongPdList)

		'�R�C��No
		cmbID.Items.Clear()
		If lstCoil.Count > 0 Then
			For ii As Integer = 0 To lstCoil.Count - 1
				cmbID.Items.Add(lstCoil(ii))
			Next

			cmbID.SelectedIndex = 0
		End If
	End Sub


	''' <summary>
	''' �f�[�^�擾
	''' </summary>
	''' <remarks></remarks>
	Private Function GetData() As Boolean

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String    'SQL��
		Dim bRet As Boolean = False
		Dim strTableLongPd() As String = {DB_LONGPD_EXEC_RESULT_DEFECT_T, DB_LONGPD_EXEC_RESULT_DEFECT_B}
		Dim strTableDefect() As String = {DB_DEFECT_INF_T, DB_DEFECT_INF_B}
		Dim nPitchFrom As Integer = mcls_Param.Pro_LongPd_NearFurnaceRangeFrom
		Dim nPitchTo As Integer = mcls_Param.Pro_LongPd_NearFurnaceRangeTo
		Dim nTolerance As Integer = mcls_Param.Pro_LongPd_PeriodicPitchTolerance

		' ������
		m_dicDispData.Clear()

		'DB�I�[�v��
		If Not tcls_DB.Open() Then
			mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
			Return False
		End If

		Try
			bRet = True
			For nListIndex As Integer = 0 To m_lstManagementNo.Count - 1
				Dim lstLongPd As New List(Of TYP_LONGPD_LIST)

				For nTorB As Integer = 0 To NUM_MEN - 1

					' SQL����
					Dim sbSQL As New StringBuilder

					sbSQL.AppendLine("")
					sbSQL.AppendLine("SELECT ")
					sbSQL.AppendLine("     L.WidDivPosNo")
					sbSQL.AppendLine("    ,L.DefectNo")
					sbSQL.AppendLine("    ,I.DS�ʒu As XFromDSEdge")
					sbSQL.AppendLine("    ,I.WS�ʒu As XFromWSEdge")
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
					sbSQL.AppendLine("        ON  L.ManagementNo = I.�Ǘ�No")
					sbSQL.AppendLine("        AND L.DefectNo = I.�rNo")
					sbSQL.AppendFormat("    INNER JOIN {0} G", DB_GRADE_NAME)
					sbSQL.AppendLine()
					sbSQL.AppendLine("        ON I.����O���[�h = G.EdasysID")
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
							Dim nGradeEdasysID As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("GradeEdasysID"))
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
										If 0 > lstLongPd(jj).lstGradeID.IndexOf(nGradeEdasysID) Then
											lstLongPd(jj).lstGradeID.Add(nGradeEdasysID)
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

				' �擾�f�[�^����ёւ��Ēǉ�
				lstLongPd.Sort(AddressOf CompareValue)
				m_dicDispData.Add(m_lstManagementNo(nListIndex), lstLongPd)
			Next

		Catch ex As Exception
			mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���������׈ꗗ�擾�G���[ [" & ex.Message & "]")
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
	' �L�[�C�x���g
	'***************************************************************************
	''' <summary>
	''' �t�@���N�V�����{�^��
	''' </summary>
	''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
	''' <param name="onKey"></param>
	Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		Try
			Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
				'---------------------------------------------------------------
				Case 9 - 1                                                     ' CSV�o��
					If btnCSV.Visible = True Then
						btnCSV_Click(Nothing, Nothing)
					End If
				'---------------------------------------------------------------
				Case 12 - 1                                                     ' �I��
					btnEnd_Click(Nothing, Nothing)
			End Select
		Catch ex As Exception
		End Try
	End Sub

	''' <summary>
	''' ��ʏI��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

		'��ʏI��
		Me.Hide()

	End Sub

	''' <summary>
	''' CSV�o��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnCSV_Click(sender As Object, e As EventArgs)
		Try
			CsvOut(CSV_PATH, String.Format("�������ꗗ_{0}.CSV", cmbID.Text.TrimEnd))
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
	''' �R���{�I��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub cmbID_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbID.SelectedIndexChanged
		DspData(cmbID.SelectedIndex)
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


End Class
