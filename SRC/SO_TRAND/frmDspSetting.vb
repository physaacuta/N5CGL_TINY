Imports tClassLibrary

Public Class frmDspSetting

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�\����
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_SETTING
        Dim nSelectMode As Integer          ' 0:���߃R�C�� 1:���Ԏw��

        Dim nDspType() As Integer           ' �I�������r��(EdasysID) GC_MAX_SELECT_TYPE
        Dim sDspType() As String            ' ���̂Ƃ��̖��O
        Dim nDspGrade() As Integer          ' �I�������O���[�h(EdasysID) 
        Dim sDspGrade() As String           ' ���̂Ƃ��̖��O


        Dim nXmax As Integer                ' �O���tX�� �ő�l
        Dim nXinterval As Integer           ' �O���tX�� �������Ԋu
        Dim nYmax As Integer                ' �O���tY�� �ő�l
        Dim nYinterval As Integer           ' �O���tY�� �������Ԋu

        Dim colorLine() As Color            ' ���̐F

        ' ������Ɠ���
        Dim daySelectS As Date              ' ����(�J�n)
        Dim daySelectE As Date              ' ����(�I��)


        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nDspType(GC_MAX_SELECT_TYPE - 1)
            ReDim sDspType(GC_MAX_SELECT_TYPE - 1)
            ReDim nDspGrade(MAX_GRADE - 1)
            ReDim sDspGrade(MAX_GRADE - 1)
            ReDim colorLine(GC_MAX_SELECT_TYPE - 1)
        End Sub
    End Structure

    '�r��E�O���[�h�t�B���^�[��
    Public Enum EM_FILTER_COL                       ' �r��
        emCheck = 0                                 ' �`�F�b�N�{�b�N�X
        emName                                      ' ����
        emEdasysID                                  ' EdasysID (��L�[)
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_typSetting As TYP_SETTING                     ' �ݒ���



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �ݒ蔽�f
    ''' </summary>
    ''' <remarks></remarks>
    Public Shadows Event EvDsp()


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���A�N�Z�X
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �ݒ�����擾
    ''' </summary>
    Public ReadOnly Property tprop_TypSetting() As TYP_SETTING
        Get
            Return m_typSetting
        End Get
    End Property

    ''' <summary>
    ''' ��ʕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Disp()
        '''' ������
        Call m_typSetting.initialize()

        '''' �����\��
        Call DspRefresh()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h
    '/ /////////////////////////////////////////////////////////////////////////////////


    ''' <summary>
    ''' �C�j�V�����C�Y
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        '''' �t�B���^�[�p �O���b�h�r���[
        Call dgvInitialize_FilterGrid(dgvDispType)
        Call dgvInitColumn_FilterGrid(dgvDispType, "�r�햼��")
        Call dgvFilter_AddHandler(dgvDispType)
        Call SetFilterData(dgvDispType, DB_TYPE_NAME)                               ' �f�[�^�\��
        btnTypeAllCancel.Tag = dgvDispType

        Call dgvInitialize_FilterGrid(dgvDispGrade)
        Call dgvInitColumn_FilterGrid(dgvDispGrade, "�O���[�h����")
        Call dgvFilter_AddHandler(dgvDispGrade)
        Call SetFilterData(dgvDispGrade, DB_GRADE_NAME)
        btnGradeAllSelect.Tag = dgvDispGrade
        btnGradeAllCancel.Tag = dgvDispGrade

        '''' ���t�f�[�^
        m_typSetting.daySelectS = CDate(Now.ToString("yyyy/MM/dd"))
        m_typSetting.daySelectE = CDate(Now.ToString("yyyy/MM/dd"))
        dtpDateStart.Value = m_typSetting.daySelectS
        dtpDateEnd.Value = m_typSetting.daySelectE

        '''' ���̑�
        Me.TopMost = True
        Me.nudXMax.Maximum = GC_MAX_PROTDATA

        '''' �f�[�^�Z�b�g
        Call Disp()
    End Sub


    ''' <summary>
    ''' �t�@���N�V�����{�^���N���b�N
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmDspSetting_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��
            Case 1 - 1                                  ' �o�^
                If Save() Then
                    ' �C�x���g�𔭐�������
                    RaiseEvent EvDsp()

                    Me.Hide()
                End If

            Case 12 - 1                                 ' �I��
                Me.Hide()
        End Select
    End Sub


    ''' <summary>
    ''' ��ʍĕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspRefresh()
        '' �ΏۃR�C���w��
        m_typSetting.nSelectMode = tMod.ReadIniInt(SO_TRAND, "SELECT_MODE", LOCALINI_NAME, 0)

        Call ChangeRadSelectMode(m_typSetting.nSelectMode)
        dtpDateStart.Value = m_typSetting.daySelectS
        dtpDateEnd.Value = m_typSetting.daySelectE

        '' �t�B���^�[�\��
        Call ReadFilterData(dgvDispType, DB_TYPE_NAME, m_typSetting.nDspType, m_typSetting.sDspType) ' �t�B���^�[�ۑ��l���f
        Call ReadFilterData(dgvDispGrade, DB_GRADE_NAME, m_typSetting.nDspGrade, m_typSetting.sDspGrade)


        '' �O���t
        m_typSetting.nXmax = tMod.ReadIniInt(SO_TRAND, "X_MAX", LOCALINI_NAME, 5)
        m_typSetting.nXinterval = tMod.ReadIniInt(SO_TRAND, "X_INTERVAL", LOCALINI_NAME, 1)
        m_typSetting.nYmax = tMod.ReadIniInt(SO_TRAND, "Y_MAX", LOCALINI_NAME, 5)
        m_typSetting.nYinterval = tMod.ReadIniInt(SO_TRAND, "Y_INTERVAL", LOCALINI_NAME, 1)

        nudXMax.Value = m_typSetting.nXmax
        nudXInterval.Value = m_typSetting.nXinterval
        nudYMax.Value = m_typSetting.nYmax
        nudYInterval.Value = m_typSetting.nYinterval

        '' �F
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
    ''' ini�t�@�C���ɕۑ����āA�����o�[�ɂ��o�^
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Save() As Boolean
        '''' �������`�F�b�N
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To dgvDispType.RowCount - 1
            If CBool(dgvDispType(EM_FILTER_COL.emCheck, ii).Value) Then
                nCnt += 1
            End If
        Next ii
        If nCnt > m_typSetting.nDspType.Length Then
            ' NG
            Using frmMsg As New tClassForm.frmPop_MsgBox("�r��̍ő�I�𐔂́A" & GC_MAX_SELECT_TYPE & "���ł��B", "�ُ�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End Using
        End If

        If nudYMax.Value < nudYInterval.Value Then
            Using frmMsg As New tClassForm.frmPop_MsgBox("�ő�l [��]�̓������Ԋu [��]���傫���l��ݒ肵�Ă��������B", "�ُ�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End Using
        End If

        If nudXMax.Value < nudXInterval.Value Then
            Using frmMsg As New tClassForm.frmPop_MsgBox("�ő�l [�{��]�̓������Ԋu [�{��]���傫���l��ݒ肵�Ă��������B", "�ُ�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Return False
            End Using
        End If

        '''' �o�^
        '' �ΏۃR�C���w��
        If rdoSelect1.Checked Then
            m_typSetting.nSelectMode = 0
        ElseIf rdoSelect2.Checked Then
            m_typSetting.nSelectMode = 1
        End If
        tClassLibrary.tMod.WriteIni(SO_TRAND, "SELECT_MODE", LOCALINI_NAME, CStr(m_typSetting.nSelectMode))

        m_typSetting.daySelectS = dtpDateStart.Value
        m_typSetting.daySelectE = dtpDateEnd.Value


        '' �t�B���^�[
        Call SaveFilterData(dgvDispType, DB_TYPE_NAME, m_typSetting.nDspType, m_typSetting.sDspType)
        Call SaveFilterData(dgvDispGrade, DB_GRADE_NAME, m_typSetting.nDspGrade, m_typSetting.sDspGrade)


        '' �O���t
        m_typSetting.nXmax = CInt(nudXMax.Value)
        m_typSetting.nXinterval = CInt(nudXInterval.Value)
        m_typSetting.nYmax = CInt(nudYMax.Value)
        m_typSetting.nYinterval = CInt(nudYInterval.Value)
        tClassLibrary.tMod.WriteIni(SO_TRAND, "X_MAX", LOCALINI_NAME, CStr(m_typSetting.nXmax))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "X_INTERVAL", LOCALINI_NAME, CStr(m_typSetting.nXinterval))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "Y_MAX", LOCALINI_NAME, CStr(m_typSetting.nYmax))
        tClassLibrary.tMod.WriteIni(SO_TRAND, "Y_INTERVAL", LOCALINI_NAME, CStr(m_typSetting.nYinterval))

        '' �F
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
    '/ �t�B���^�[�O���b�h �����`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r��t�B���^�[ �O���b�h������
    ''' </summary>
    ''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize_FilterGrid(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)

        '''' �Ǝ��̍��ڂ�ݒ�
        With dgv
            ' '' ��`
            '.ColumnHeadersHeight = 25										' �w�b�_�[�̍���
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �s�I��
            .ScrollBars = ScrollBars.Vertical

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 0
        End With
    End Sub
    ''' <summary>
    ''' �r��t�B���^�[ �O���b�h���`
    ''' </summary>
    ''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
    ''' <param name="strName">2��ڂ̍��ږ�</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitColumn_FilterGrid(ByVal dgv As DataGridView, ByVal strName As String)
        With dgv
            ''''' ��ǉ�
            .Columns.Add(New DataGridViewCheckBoxColumn)  ' �`�F�b�N�{�b�N�X
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' EdasysID

            '''' ���`
            '1���:�`�F�b�N�{�b�N�X
            .Columns(EM_FILTER_COL.emCheck).HeaderText = ""
            .Columns(EM_FILTER_COL.emCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).Width = 20
            .Columns(EM_FILTER_COL.emCheck).SortMode = DataGridViewColumnSortMode.NotSortable

            '2���:�r�햼��
            .Columns(EM_FILTER_COL.emName).HeaderText = strName & vbCrLf
            .Columns(EM_FILTER_COL.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).Width = .ClientSize.Width - 20 - 3
            .Columns(EM_FILTER_COL.emName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:EdasysID
            .Columns(EM_FILTER_COL.emEdasysID).Visible = False  '��\��
        End With
    End Sub

    ''' <summary>
    ''' �r��E�O���[�h�t�B���^�Z�b�g
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h</param>
    ''' <param name="strTable">�e�[�u����</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String)
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim iCnt As Integer = 0

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT Name, "
        strSQL &= "        EdasysID "
        strSQL &= " FROM " & strTable
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()

                dgv.RowCount += 1   '�\���s����ݒ�

                ''�ݒ�s�� �w��
                dgv(EM_FILTER_COL.emName, iCnt).ReadOnly = True

                '�f�[�^�Z�b�g
                dgv(EM_FILTER_COL.emName, iCnt).Value = sqlRead.GetString(0)
                dgv(EM_FILTER_COL.emName, iCnt).Style.WrapMode = DataGridViewTriState.False
                dgv(EM_FILTER_COL.emCheck, iCnt).Value = False
                dgv(EM_FILTER_COL.emEdasysID, iCnt).Value = sqlRead.GetInt32(1)

                iCnt += 1

            End While

        Catch ex As Exception

        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try
    End Sub
    ''' <summary>
    ''' �t�B���^�[�̃`�F�b�N�ۑ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h</param>
    ''' <param name="strInikey">ini�t�@�C���̑ΏۃL�[</param>
    ''' <remarks></remarks>
    Protected Sub SaveFilterData(ByVal dgv As DataGridView, ByVal strInikey As String, ByVal setVal() As Integer, ByVal name() As String)
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To setVal.Length - 1
            setVal(ii) = 0
            name(ii) = ""
        Next

        Dim strVal As String = ""

        ' �r��,�O���[�h�t�B���^�[�̏�����INI�t�@�C���֓o�^
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
    ''' �t�B���^�[�̃`�F�b�N�f�[�^�Ǎ�
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="strInikey"></param>
    ''' <param name="setVal">ON��Ԃ�EdasysID</param>
    ''' <remarks></remarks>
    Protected Sub ReadFilterData(ByVal dgv As DataGridView, ByVal strInikey As String, ByVal setVal() As Integer, ByVal name() As String)
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To setVal.Length - 1
            setVal(ii) = 0
            name(ii) = ""
        Next

        ' INI�t�@�C���̐ݒ���擾
        Dim strVal As String = tClassLibrary.tMod.ReadIniStr(SO_TRAND, strInikey, LOCALINI_NAME)
        If "" = strVal Then Exit Sub

        ' �ݒ�l�𔽉f
        For ii As Integer = 0 To dgv.RowCount - 1

            '���o�^�܂��́A1���ݒ肳��Ă���ꍇ�́A�`�F�b�N������
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
    ''' �O���b�h���� �n���h�����蓖��
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvFilter_AddHandler(ByVal dgv As tCnt.tCnt_DataGridView)
        AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
        AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv.dgvData_CurrentCellDirtyStateChanged
    End Sub

    ''' <summary>
    ''' �Z���`�F���W����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, tCnt.tCnt_DataGridView)
        '�w�b�_�͏������Ȃ�
        If e.RowIndex = -1 Then Exit Sub

        If e.ColumnIndex = EM_FILTER_COL.emCheck Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '' �`�F�b�N��t������
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
            Else
                '' �`�F�b�N���O������
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub
    ''' <summary>
    ''' �S�I���{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGradeAllSelect.Click

        '�`�F�b�N�{�b�N�X��S��ON
        Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), True)

    End Sub
    ''' <summary>
    ''' �S�����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

        '�`�F�b�N�{�b�N�X��S��OFF
        Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), False)

    End Sub

    ''' <summary>
    ''' �O���b�h�̃`�F�b�N�{�b�N�X�Z�b�g
    ''' </summary>
    ''' <param name="dgvData">�f�[�^�O���b�h</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CheckGridAllChange(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' �O���b�h�̑S�Ẵ`�F�b�N�{�b�N�X��ݒ�
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_FILTER_COL.emCheck, ii).Value = bValue
        Next ii

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑��ׂ�������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���W�I�{�^���؂�ւ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub rdoSelect_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles rdoSelect1.CheckedChanged, rdoSelect2.CheckedChanged
        Dim rdo As RadioButton = CType(sender, RadioButton)

        '' �`�F�b�N���O�ꂽ�揈���͉������Ȃ�
        If Not rdo.Checked Then
            Return
        End If

        '' �`�F�b�N��ԂɕύX�����ꍇ�̂�
        If rdo Is rdoSelect1 Then
            Call ChangeRadSelectMode(0)
        Else
            Call ChangeRadSelectMode(1)
        End If
    End Sub

    ''' <summary>
    ''' ���W�I�{�^���؂�ւ�����
    ''' </summary>
    ''' <param name="mode">0:���߃R�C�� 1:���Ԏw��</param>
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
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' ���F�w��{�^��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnLineColor1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLineColor1.Click, btnLineColor2.Click, btnLineColor3.Click, btnLineColor4.Click, btnLineColor5.Click
        Dim btn As Button = CType(sender, Button)
        Using dlgColor As New System.Windows.Forms.ColorDialog
            '�F�I���_�C�A���O�̐ݒ�
            '�F�̍쐬���g�p���Ȃ�
            dlgColor.AllowFullOpen = False

            '�F�I���_�C�A���O�̑I��F���A�{�^���̔w�i�F�ɂ���
            dlgColor.Color = btn.BackColor

            ' �F�I���_�C�A���O��ok�{�^���������ꂽ��A�{�^���̐F��ύX���� 
            If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
                '�I��F���{�^���̔w�i�F��
                btn.BackColor = dlgColor.Color
            End If
        End Using
    End Sub
End Class
