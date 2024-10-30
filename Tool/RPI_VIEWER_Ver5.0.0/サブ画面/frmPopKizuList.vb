Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopKizuList

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    Public Event tEv_KizuClick(ByVal pos As Point)						' �N���b�N (mm���W�n) 

    Public Event tEv_ReSetKizuDsp()										' �r�\���ăZ�b�g



    Private Enum EM_DEF
        em_No = 0		' �rNo
        em_T			' �r��
        em_G			' ��ڰ��
        em_Pos			' T�`
        em_C			' C�` (��-)
        em_L			' ���`
        em_R			' �E�`
        em_End
    End Enum

    Private Enum EM_ATTR
        em_Angle = 0	' �p�x
        em_Enum			' Enum
        em_ID			' EdasysID
        em_Name			' ����
        em_Val			' ������
        em_End
    End Enum



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�

    ' �N���X
    Private mcls_pParam As clsParamManager = Nothing					' �p�����[�^�Ǘ��N���X (�|�C���^)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPI�t�@�C���\���N���X (�|�C���^)

    ' �f�[�^
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' �r��񃊃X�g
    ' �������
    Private m_bDspOk As Boolean = False									' �U�z�}�ɂ����ƃf�[�^���\�����Ă���Ƃ�

    Private m_Session As String											' ini�t�@�C���Z�b�V����
    Private m_nAttrNum As Integer										' �����ʃe�[�u���� (1�p�x�Ȃ�A��{+�p�x1= 2�ƂȂ�)
    Private m_nAttrCnt(MC_MAX_IMG + 1) As Integer						' �e�����ʐ�
    Private m_nActiveNo As Integer										' �\�������rNo



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B
    ''' <summary>
    ''' �p�����[�^�N���X�Z�b�g
    ''' </summary>
    ''' <param name="cls">�v�Z�N���X</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    ''' RPI�N���X
    ''' </summary>
    ''' <param name="cls">RPI�N���X</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub

    ''' <summary>
    ''' �r��񃊃X�g
    ''' </summary>
    ''' <param name="lst">�r��񃊃X�g</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{
    ''' <summary>
    ''' ���[�h
    ''' </summary>
    Private Sub frmPopKizuList_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Call DgvInitDefect(dgvDefect)
        Call DgvInitAttr(dgvAttr)
    End Sub



    ''' <summary>
    ''' �}�b�v�`��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MapDisp()

        If Not mcls_pRpiView.IsRpi Then
            m_bDspOk = False

            dgvDefect.RowCount = 1		' 0�̓_��
            dgvAttr.RowCount = 1
            m_nActiveNo = 0
            Return
        End If

        '''' ��������́ARPI�t�@�C�����J���Ă���Ƃ��̂�
        If Me.Visible Then
            Call DgvAttr_RowSet(dgvAttr)
            Call SetDefectInf()
        End If

        m_bDspOk = True
    End Sub

    ''' <summary>
    ''' �V�����R�C���\��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub NewCoil()
        Call SetListType()
        Call SetListGr()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�U�C��
#Region "�f�U�C��"


    ''' <summary>
    ''' �����`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub DgvInitDefect(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv

            .ScrollBars = ScrollBars.Both								   ' �X�N���[���o�[�̕\��
            .AllowUserToResizeColumns = True								' ���[�U�[ ��T�C�Y�ύX


            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().WrapMode = DataGridViewTriState.True


            '' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "�rNo"
                clm.Width = 48
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "�r��"
                clm.Width = 80
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "Gr"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "T�`"
                clm.Width = 60
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "C�`"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "���`"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "�E�`"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
        End With
    End Sub
    Private Sub DgvInitAttr(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv
            .ScrollBars = ScrollBars.Both								   ' �X�N���[���o�[�̕\��
            .AllowUserToResizeColumns = True								' ���[�U�[ ��T�C�Y�ύX


            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().WrapMode = DataGridViewTriState.False


            '' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "�p�x"
                clm.Width = 36
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "Enum"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Visible = False
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "EdaID"
                clm.Width = 40
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "����"
                clm.Width = 150
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "������"
                clm.Width = 150
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
        End With
    End Sub


    ''' <summary>
    ''' �����ʖ��̃Z�b�g
    ''' </summary>
    Private Sub DgvAttr_RowSet(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        m_Session = "DB_" & mcls_pRpiView.GetRpi_THeader.cSysName
        m_nAttrNum = tMod.tMod_IniFile.ReadIniInt(m_Session, "ATTR_NUM", TASKINI_NAME)
        m_nActiveNo = 0
        For ii As Integer = 0 To m_nAttrCnt.Length - 1
            m_nAttrCnt(ii) = 0
        Next

        If 0 = m_nAttrNum Then Return
        m_nAttrNum = m_nAttrNum + 1	' ��{�����ʕ��͕K������

        '==============================
        ' �����ʖ��̃Z�b�g
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        'SQL���쐬
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_ATTR_NAME", TASKINI_NAME)

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Return

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return


            ' �f�[�^�擾
            dgv.Enabled = False
            dgv.RowCount = 0
            Dim nCnt As Integer = 0
            While sqlRead.Read()
                If m_nAttrNum - 1 < sqlRead.GetInt32(0) Then Continue While
                m_nAttrCnt(sqlRead.GetInt32(0)) += 1	' �s�����Z

                dgv.RowCount = nCnt + 1

                dgv(EM_ATTR.em_Angle, nCnt).Value = sqlRead.GetInt32(0)
                dgv(EM_ATTR.em_Enum, nCnt).Value = sqlRead.GetInt32(1)
                dgv(EM_ATTR.em_ID, nCnt).Value = sqlRead.GetInt32(2)
                dgv(EM_ATTR.em_Name, nCnt).Value = sqlRead.GetString(3)
                dgv(EM_ATTR.em_Val, nCnt).Value = ""


                nCnt += 1
            End While
        Catch ex As Exception
            Return
        Finally
            sqlRead.Close()
            tcls_DB.Close()
            tcls_DB.Dispose()

            dgv.Enabled = True
            dgv.Refresh()
        End Try
    End Sub

#End Region

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�[�^�Z�b�g
    ''' <summary>
    ''' �r�����Z�b�g
    ''' </summary>
    Private Sub SetDefectInf()
        dgvDefect.Enabled = False
        dgvDefect.RowCount = m_lstDetect.Count
        For ii As Integer = 0 To m_lstDetect.Count - 1
            dgvDefect.Rows(ii).Visible = m_lstDetect(ii).bDsp
            dgvDefect(EM_DEF.em_No, ii).Value = m_lstDetect(ii).nNo
            dgvDefect(EM_DEF.em_T, ii).Value = mcls_pParam.GetParamType(m_lstDetect(ii).tId).sSName
            dgvDefect(EM_DEF.em_G, ii).Value = mcls_pParam.GetParamGrade(m_lstDetect(ii).gId).sName
            dgvDefect(EM_DEF.em_Pos, ii).Value = String.Format("{0:0.0}", m_lstDetect(ii).nY / 1000)
            dgvDefect(EM_DEF.em_C, ii).Value = m_lstDetect(ii).nXc
            dgvDefect(EM_DEF.em_L, ii).Value = m_lstDetect(ii).nX
            dgvDefect(EM_DEF.em_R, ii).Value = m_lstDetect(ii).nXws
        Next
        dgvDefect.Enabled = True
        dgvDefect.Refresh()
    End Sub



    ''' <summary>
    ''' �����ʕ\��
    ''' </summary>
    ''' <param name="val">�I���r���</param>
    Private Sub SetAttrData(ByVal val As DATA_DETECT_INF)
        Dim nMen As Integer = mcls_pRpiView.GetRpi_THeader.nMen
        If 0 = m_nAttrNum Then Return

        '==============================
        ' �����ʖ��̃Z�b�g
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sSQL As String

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Return

            Dim nRow As Integer = 0						' �f�[�^�O���b�h�r���[�̍s
            For ii As Integer = 0 To m_nAttrNum - 1

                'SQL���쐬
                Dim sKey As String = String.Format("ATTR_INF_{0}_{1}", nMen, ii)
                sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, sKey, TASKINI_NAME)
                sSQL = String.Format(sSQL, mcls_pRpiView.GetRpi_THeader.cKizukenNo, Val.nNo)

                'SQL���s
                Using sqlRead As SqlClient.SqlDataReader = tcls_DB.SelectExecute(sSQL)
                    If sqlRead Is Nothing Then Return

                    While sqlRead.Read()
                        For jj As Integer = 0 To m_nAttrCnt(ii) - 1
                            dgvAttr(EM_ATTR.em_Val, nRow).Value = sqlRead.GetDouble(2 + jj)   ' 2�͎�L�[����
                            nRow += 1
                        Next
                    End While
                End Using
            Next

        Catch ex As Exception
            Return
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Sub


    ''' <summary>
    ''' �r��� �f�[�^�O���b�h�N���b�N
    ''' </summary>
    Private Sub dgvDefect_CellMouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles dgvDefect.CellMouseDown
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim row As Integer = e.RowIndex

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case EM_DEF.em_No		' �rNo
                RaiseEvent tEv_kizuClick(New Point(m_lstDetect(row).nFrameXc, m_lstDetect(row).nY))
                If m_nActiveNo <> m_lstDetect(row).nNo Then Call SetAttrData(m_lstDetect(row))

            Case Else
                If m_nActiveNo <> m_lstDetect(row).nNo Then Call SetAttrData(m_lstDetect(row))
        End Select
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �r��A�O���[�h
    ''' <summary>
    ''' �r�탊�X�g�ꗗ�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetListType()
        clbType.Items.Clear()

        For Each wk As clsParamManager.PARAM_TYPE In mcls_pParam.GetParamType()
            Dim item As TYP_ITEM
            item.sString = wk.sName
            item.nKey = wk.nEdasysID
            clbType.Items.Add(item, True)
        Next
    End Sub
    Private Sub SetListGr()
        clbGr.Items.Clear()

        For Each wk As clsParamManager.PARAM_GRADE In mcls_pParam.GetParamGrade()
            Dim item As TYP_ITEM
            item.sString = wk.sName
            item.nKey = wk.nEdasysID
            clbGr.Items.Add(item, True)
        Next
    End Sub

    ''' <summary>
    ''' �S�I���A�S����
    ''' </summary>
    Private Sub AllSetList(ByVal clb As CheckedListBox, ByVal bMode As Boolean)
        For ii As Integer = 0 To clb.Items.Count - 1
            clb.SetItemChecked(ii, bMode)
        Next
    End Sub
    Private Sub btnTypeAllOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllOk.Click
        AllSetList(clbType, True)
    End Sub
    Private Sub btnTypeAllNg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllNg.Click
        AllSetList(clbType, False)
    End Sub
    Private Sub btnGrAllOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGrAllOk.Click
        AllSetList(clbGr, True)
    End Sub
    Private Sub btnGrAllNg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGrAllNg.Click
        AllSetList(clbGr, False)
    End Sub


    ''' <summary>
    ''' �r�\���t�B���^ ���s
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub btnExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExec.Click
        Call FilterList()

        ' �e�픽�f
        '	Call SetDefectInf()
        RaiseEvent tEv_ReSetKizuDsp()
    End Sub


    ''' <summary>
    ''' �r�\���t�B���^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub FilterList()
        Dim nCnt As Integer

        '''' �\���Ώۂ�EdasysID�ꗗ
        Dim tId(clbType.CheckedIndices.Count - 1) As Integer
        Dim gId(clbGr.CheckedIndices.Count - 1) As Integer
        ' �r��
        nCnt = 0
        For Each item As TYP_ITEM In clbType.CheckedItems
            tId(nCnt) = item.nKey
            nCnt += 1
        Next

        ' Gr
        nCnt = 0
        For Each item As TYP_ITEM In clbGr.CheckedItems
            gId(nCnt) = item.nKey
            nCnt += 1
        Next



        '''' �\���Ώۂ�I��
        '		nCnt = 0
        For ii As Integer = 0 To m_lstDetect.Count - 1		' �l�����������邽�߁A�R���N�V�����ł�NG
            Dim data As DATA_DETECT_INF = m_lstDetect(ii)

            data.bDsp = False
            If -1 = Array.IndexOf(gId, data.gId) Then GoTo Ending
            If -1 = Array.IndexOf(tId, data.tId) Then GoTo Ending

            ' �����܂ŗ�����\���Ώ�
            data.bDsp = True

Ending:
            ' �X�V
            '			m_lstDetect(nCnt) = data
            '			nCnt += 1
            m_lstDetect(ii) = data
        Next
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����ʏo��
    Private Sub btnCsvKizu_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsvKizu.Click
        Call CsvOut(dgvDefect, CurDir, "�r.csv")
    End Sub

    Private Sub btnCsvAttr_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsvAttr.Click
        Call CsvOut(dgvAttr, CurDir, "������.csv")
    End Sub



    ''' <summary>
    ''' CSV�o��
    ''' </summary>
    ''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
    ''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CsvOut(ByVal dgv As DataGridView, ByVal strFolder As String, ByVal strName As String)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""						'1�s�̃f�[�^
        Dim strWk() As String
        Dim strWkData As String

        '' �t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        '' �t�H���_�̗L���`�F�b�N(�����������낤)
        tMod.CheckFolder(strFolder)

        '' CSV�o��
        Try
            Using cls_Stram As New System.IO.StreamWriter(strFPath, False, System.Text.Encoding.Default)
                '�w�b�_���o��
                strData = ""
                For ii As Integer = 0 To dgv.ColumnCount - 1

                    '���s�R�[�h�����ׂĎ�菜��
                    strWk = dgv.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
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
                For ii As Integer = 0 To dgv.Rows.Count - 1
                    If Not dgv.Rows(ii).Visible Then Continue For

                    strData = ""
                    For jj As Integer = 0 To dgv.ColumnCount - 1
                        '���ڂ�Tab�ŘA������
                        If dgv(jj, ii).Value Is Nothing Then
                            strData &= ","
                        Else
                            strData &= dgv(jj, ii).Value.ToString & ","
                        End If


                    Next jj

                    '�Ō�̃^�u��r��
                    strData = strData.Remove(strData.Length - 1, 1)

                    cls_Stram.WriteLine(strData)
                Next ii

            End Using
            strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"
        Catch ex As Exception
            strMsg = "CSV�o�͂Ɏ��s���܂����B"
        End Try

        '''' �K�C�_���X�\��
        MsgBox(strMsg)
    End Sub

End Class