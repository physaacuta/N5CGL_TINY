Imports tClassLibrary


Public Class frmMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\����
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���t�v���b�g�f�[�^
    ''' </summary>
    Private Structure TYP_PROTDATA
        Dim sCoilNo As String                       ' �R�C��No
        Dim nCoilLen As Integer                     ' ���������� [m] (\ 1000) ����

        Dim nCntO() As Integer                      ' �\�Ώ��r����
        Dim nCntU() As Integer                      ' ���Ώ��r����

        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim nCntO(GC_MAX_SELECT_TYPE - 1)
            ReDim nCntU(GC_MAX_SELECT_TYPE - 1)
        End Sub
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mfrm_Setting As New frmDspSetting                ' �\�������ݒ���
    Private WithEvents mcls_Map_O As New tClass.tClass_MapBase
    Private WithEvents mcls_Map_U As New tClass.tClass_MapBase
    Private mcls_pMap(1) As tClass.tClass_MapBase                       ' �}�b�v�`����N���X (�g���₷���悤�ɔz��)


    '' �ێ����[�N�f�[�^
    Private m_nProtData As Integer                                      ' �L���v���b�g�f�[�^����
    Private m_typProtData(GC_MAX_PROTDATA - 1) As TYP_PROTDATA          ' �O���t�v���b�g�f�[�^ (�ێ����[�N�̈�)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{���\�b�h
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '''' �^�C�g���ݒ�
        Call SetTitleName(Me)

        '''' �ݒ��ʂ����t�H�[���̎q���ɒǉ�
        Me.AddOwnedForm(mfrm_Setting)

        '''' �����o�[�ϐ�����
        ' �g���₷���悤�ɔz��
        mcls_pMap(0) = mcls_Map_O
        mcls_pMap(1) = mcls_Map_U


        '''' ���̑� ��������
        Call dgvInitialize(dgvCount)

        '''' �}�b�v������`
        Call MapInitialize()

        '''' ��ʏ�����
        Call DspInit()

        ' �r�_�}�b�v�̕\���{�^���̃T�C�Y�𒲐�
        btnDspDefMap.Visible = True

        With lblF1
            .Location = New Point(1157, 9)
            .Size = New Size(40, 20)
        End With

        With lblF5
            .Location = New Point(1157, 39)
            .Size = New Size(40, 20)
            .Visible = True
        End With

        With btnSetting
            .Location = New Point(1180, 4)
            .Size = New Size(78, 30)
            .Font = New Font("�l�r ����", 9.75, FontStyle.Bold)
            .Text = "�\���ݒ�"
        End With

        With btnDspDefMap
            .Location = New Point(1180, 34)
            .Size = New Size(78, 30)
            .Visible = True
        End With

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h���� (frmBase��Protected Overrides Sub LoadEnding()����̃I�[�o�[���C�h�̓���)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmMenu_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        '''' ��ʕ\���v��
        Call gfrm_Setting_EvDsp()
    End Sub

    ''' <summary>
    ''' ��� ���悤�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' ���C���X���b�h�I���ʒm
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' �t�H�[���L�[�_�E��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTansho_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.F1 Then                ' �\�������ݒ���
            Call btnSetting_Click(Nothing, Nothing)

        ElseIf e.KeyCode = Keys.F5 Then            ' �r�_�}�b�v���
            Call btnDspDefMap_Click(Nothing, Nothing)

        ElseIf True = e.Shift And e.KeyCode = Keys.F12 Then         ' �I��

            Call modMain.SendMailDefMap(FACT_KS_END)

            '�^�X�N�I��
            Me.Close()
        End If
    End Sub

    ''' <summary>
    ''' �\�������ݒ��ʃ{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSetting_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSetting.Click
        ' �\�������ݒ���
        Call mfrm_Setting.Disp()
        Call mfrm_Setting.Show()
    End Sub

    ''' <summary>
    ''' �r�_�}�b�v�{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnDspDefMap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDspDefMap.Click
        Dim strExecPath As String = String.Format(".\\{0}.exe", SO_DEFMAP)

        Try
            Call Diagnostics.Process.Start(strExecPath)
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�_�}�b�v�N���G���[")
        End Try
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' ��ʏ����\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting

        '''' �w�b�_�[��
        lblOrderNo.Text = ""
        lblCoilNo.Text = ""
        lblLength.Text = ""
        lblSizeZai.Text = ""
        lblSizeSei.Text = ""

        '''' �O���b�h����
        ' 1�s��
        For ii As Integer = 0 To dgvCount.Columns.Count - 1
            dgvCount(ii, 0).Value = typ.sDspType(ii)
            dgvCount(ii, 0).Style.ForeColor = typ.colorLine(ii)
        Next ii
        ' 2�s��
        For ii As Integer = 0 To dgvCount.Columns.Count - 1
            dgvCount(ii, 1).Value = "0/0"
        Next ii
    End Sub



    ''' <summary>
    ''' ��ʃf�[�^�`�� (�ݒ�ύX���ɃC�x���g���s) (��ʕ\�����ɂ�)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub gfrm_Setting_EvDsp() Handles mfrm_Setting.EvDsp
        '''' ��ʏ�����
        Call DspInit()

        '''' �}�b�v�̈�Ē�`
        Call MapReSetring()

        '''' �v���b�g�f�[�^���Z�b�g
        Call GetProtData()

        '''' �r����񗓕\���̈�Đݒ�
        Call dgvWidthResize(dgvCount)

        '''' �}�b�v�Đ����˗�
        For ii As Integer = 0 To mcls_pMap.Length - 1
            Call mcls_pMap(ii).DrawPaint()
        Next

        '''' �}�b�v�ĕ`��
        Call picMap1.Refresh()
        Call picMap2.Refresh()
    End Sub

    ''' <summary>
    ''' �\���̃v���b�g�f�[�^���擾����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetProtData()
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting
        Dim ii As Integer
        Dim strSQL As String

        '''' �O���t�f�[�^������
        m_nProtData = 0
        For ii = 0 To m_typProtData.Length - 1
            m_typProtData(ii) = New TYP_PROTDATA
            m_typProtData(ii).initialize()
        Next ii


        '''' SQL�ڑ�����
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_RESULT & "]")
            Exit Sub
        End If


        ''''' ���Ԏw��̏ꍇ�A�\�������𒲐�
        'If 1 = typ.nSelectMode Then
        '	' ���ԓ������擾
        '	strSQL = "SELECT COUNT(*) "
        '	strSQL &= " FROM " & DB_COIL_RESULT & " r "
        '	strSQL &= " WHERE r.�o�^���t >= '" & Format(typ.daySelectS, "yyyy/MM/dd ") & "00:00:00'"
        '	strSQL &= " AND r.�o�^���t <= '" & Format(typ.daySelectE, "yyyy/MM/dd ") & "23:59:59'"
        '	' ���ԓ������擾
        '	Dim nMaxCnt As Integer
        '	If Not tcls_DB.GetSelectKey(strSQL, nMaxCnt) Then
        '		Exit Sub
        '	End If
        '	' ��������
        '	If typ.nXmax > nMaxCnt Then
        '		typ.nXmax = nMaxCnt
        '	End If
        'End If

        '''' ������Ԃ�\��
        If 1 = typ.nSelectMode Then     ' ���Ԏw��̏ꍇ
            lblSearch.Text = "���Ԏw��"
            lblSearch.BackColor = Color.Yellow
            lblSearch.Visible = True
        Else                            ' ���߃R�C���w��̏ꍇ
            lblSearch.Visible = False
        End If

        ''''' �r��ʑΉ�
        '''' �r������t�B���^��
        Dim strSqlFilter As String
        strSqlFilter = "�d������ <> 1  AND ����O���[�h IN("
        For ii = 0 To typ.nDspGrade.Length - 1
            If typ.nDspGrade.Length = 0 Then Continue For

            strSqlFilter &= typ.nDspGrade(ii)
            strSqlFilter &= ","
        Next
        strSqlFilter = strSqlFilter.Substring(0, strSqlFilter.Length - 1)   ' �Ō��,���폜
        strSqlFilter &= ")"

        '''' SQL������
        strSQL = "SELECT TOP " & typ.nXmax
        strSQL &= " r.�Ǘ�No, ISNULL(c.����No,'') as ����No, ISNULL(c.���YNo,'') as ���YNo, r.����������, "
        strSQL &= "ISNULL(c.�ޗ���,0) as �ޗ���, ISNULL(c.�ޗ���,0) as �ޗ���, ISNULL(c.������,0) as ������, ISNULL(c.������,0) as ������, "

        strSQL &= "ISNULL(o1.��, 0) as �\�r��1, ISNULL(o2.��, 0) as �\�r��2, ISNULL(o3.��, 0) as �\�r��3, ISNULL(o4.��, 0) as �\�r��4, ISNULL(o5.��, 0) as �\�r��5, "
        strSQL &= "ISNULL(u1.��, 0) as ���r��1, ISNULL(u2.��, 0) as ���r��2, ISNULL(u3.��, 0) as ���r��3, ISNULL(u4.��, 0) as ���r��4, ISNULL(u5.��, 0) as ���r��5 "
        strSQL &= " FROM " & DB_COIL_RESULT & " r "
        strSQL &= " LEFT OUTER JOIN " & DB_COIL_INF & " c "
        strSQL &= " ON r.�Ǘ�No = c.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(0)
        strSQL &= " GROUP BY �Ǘ�No ) o1 "
        strSQL &= " ON r.�Ǘ�No = o1.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(1)
        strSQL &= " GROUP BY �Ǘ�No ) o2 "
        strSQL &= " ON r.�Ǘ�No = o2.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(2)
        strSQL &= " GROUP BY �Ǘ�No ) o3 "
        strSQL &= " ON r.�Ǘ�No = o3.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(3)
        strSQL &= " GROUP BY �Ǘ�No ) o4 "
        strSQL &= " ON r.�Ǘ�No = o4.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_T & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(4)
        strSQL &= " GROUP BY �Ǘ�No ) o5 "
        strSQL &= " ON r.�Ǘ�No = o5.�Ǘ�No "


        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(0)
        strSQL &= " GROUP BY �Ǘ�No ) u1 "
        strSQL &= " ON r.�Ǘ�No = u1.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(1)
        strSQL &= " GROUP BY �Ǘ�No ) u2 "
        strSQL &= " ON r.�Ǘ�No = u2.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(2)
        strSQL &= " GROUP BY �Ǘ�No ) u3 "
        strSQL &= " ON r.�Ǘ�No = u3.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(3)
        strSQL &= " GROUP BY �Ǘ�No ) u4 "
        strSQL &= " ON r.�Ǘ�No = u4.�Ǘ�No "

        strSQL &= " LEFT OUTER JOIN (SELECT COUNT(*) as ��, �Ǘ�No from "
        strSQL &= DB_DEFECT_INF_B & "  "
        strSQL &= " WHERE " & strSqlFilter & " AND �����r��=" & typ.nDspType(4)
        strSQL &= " GROUP BY �Ǘ�No ) u5 "
        strSQL &= " ON r.�Ǘ�No = u5.�Ǘ�No "


        ' ���Ԏw��̏ꍇ�̂�
        If 1 = typ.nSelectMode Then
            strSQL &= " WHERE r.�o�^���t >= '" & Format(typ.daySelectS, "yyyy/MM/dd ") & "00:00:00'"
            strSQL &= " AND r.�o�^���t <= '" & Format(typ.daySelectE, "yyyy/MM/dd ") & "23:59:59'"
        End If

        strSQL &= " ORDER BY r.�o�^���t desc "


        '''' �f�[�^�擾
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_RESULT & "]")
            Exit Sub
        End If

        ' ���o��
        Try
            While sqlRead.Read()
                '' �ʏ�̃f�[�^�擾
                m_typProtData(m_nProtData).sCoilNo = sqlRead.GetString(sqlRead.GetOrdinal("���YNo"))
                m_typProtData(m_nProtData).nCoilLen = sqlRead.GetInt32(sqlRead.GetOrdinal("����������")) \ 1000
                m_typProtData(m_nProtData).nCntO(0) = sqlRead.GetInt32(sqlRead.GetOrdinal("�\�r��1"))
                m_typProtData(m_nProtData).nCntO(1) = sqlRead.GetInt32(sqlRead.GetOrdinal("�\�r��2"))
                m_typProtData(m_nProtData).nCntO(2) = sqlRead.GetInt32(sqlRead.GetOrdinal("�\�r��3"))
                m_typProtData(m_nProtData).nCntO(3) = sqlRead.GetInt32(sqlRead.GetOrdinal("�\�r��4"))
                m_typProtData(m_nProtData).nCntO(4) = sqlRead.GetInt32(sqlRead.GetOrdinal("�\�r��5"))
                m_typProtData(m_nProtData).nCntU(0) = sqlRead.GetInt32(sqlRead.GetOrdinal("���r��1"))
                m_typProtData(m_nProtData).nCntU(1) = sqlRead.GetInt32(sqlRead.GetOrdinal("���r��2"))
                m_typProtData(m_nProtData).nCntU(2) = sqlRead.GetInt32(sqlRead.GetOrdinal("���r��3"))
                m_typProtData(m_nProtData).nCntU(3) = sqlRead.GetInt32(sqlRead.GetOrdinal("���r��4"))
                m_typProtData(m_nProtData).nCntU(4) = sqlRead.GetInt32(sqlRead.GetOrdinal("���r��5"))


                '' �ŏ��̂P���ڂ݂̂̓��ꏈ��
                If m_nProtData = 0 Then
                    lblOrderNo.Text = sqlRead.GetString(sqlRead.GetOrdinal("����No"))
                    lblCoilNo.Text = m_typProtData(m_nProtData).sCoilNo
                    lblLength.Text = m_typProtData(m_nProtData).nCoilLen.ToString
                    lblSizeZai.Text = String.Format("{0:#0.000}�~{1}", sqlRead.GetInt32(sqlRead.GetOrdinal("�ޗ���")) / 1000, sqlRead.GetInt32(sqlRead.GetOrdinal("�ޗ���")))
                    lblSizeSei.Text = String.Format("{0:#0.000}�~{1}", sqlRead.GetInt32(sqlRead.GetOrdinal("������")) / 1000, sqlRead.GetInt32(sqlRead.GetOrdinal("������")))


                    ' �����Z�b�g
                    For ii = 0 To dgvCount.Columns.Count - 1
                        dgvCount(ii, 1).Value = String.Format("{0}/{1}", m_typProtData(m_nProtData).nCntO(ii), m_typProtData(m_nProtData).nCntU(ii))
                    Next ii
                End If

                m_nProtData += 1
            End While

        Catch ex As Exception
            m_nProtData = 0
        Finally
            tcls_DB.Cancel()
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


        '''' ���Ԏw��̏ꍇ�A�}�b�v�̈��Y������
        If 1 = typ.nSelectMode Then
            For ii = 0 To mcls_pMap.Length - 1
                With mcls_pMap(ii)
                    Dim nProntData As Integer = m_nProtData
                    If m_nProtData = .tProp_MapInf_LimitXmin Then
                        nProntData = 0
                    End If

                    '' �}�b�v�̈�
                    .tProp_MapInf_LimitXmax = nProntData
                End With
            Next ii
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �t�B���^�[�O���b�h �����`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r��t�B���^�[ �O���b�h������
    ''' </summary>
    ''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        '''' �Ǝ��̍��ڂ�ݒ�
        With dgv
            .RowCount = 2

            '' ��`
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �s�I��
            .ScrollBars = ScrollBars.Horizontal
            .ReadOnly = True
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX

            '' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (DataGridViewSelectionCancel���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 14.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .DefaultCellStyle.WrapMode = DataGridViewTriState.False


            '' ���`
            .ColumnCount = GC_MAX_SELECT_TYPE                           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersVisible = False
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 13.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' ��w�b�_�T�C�Y


            '�s��`
            For ii As Integer = 0 To .Rows.Count - 1
                .Rows(ii).Height = 28   '�s�̕����Z�b�g
            Next ii

            '' ���`
            For ii As Integer = 0 To .Columns.Count - 1
                .Columns(ii).Width = 96
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
            Next ii

            '' �񕝕ύX�C�x���g�ݒ�
            AddHandler .ColumnWidthChanged, AddressOf dgvColumnWidthChanged

        End With
    End Sub

    ''' <summary>
    ''' �񕝂̃T�C�Y�ύX�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvColumnWidthChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewColumnEventArgs)

        Dim dgv As tCnt.tCnt_DataGridView = CType(sender, tCnt.tCnt_DataGridView)

        ' �r����񗓕\���̈�Đݒ�
        dgvWidthResize(dgv)

    End Sub

    ''' <summary>
    ''' �r����񗓕\���̈�Đݒ�
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvWidthResize(ByVal dgv As tCnt.tCnt_DataGridView)
        Dim hsc As HScrollBar
        Dim nHeight As Integer

        ' �s�����擾
        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            ' �����X�N���[���o�[�\����
            nHeight = CInt((dgv.ClientSize.Height - hsc.Height) / dgv.Rows.Count)
        Else
            ' �����X�N���[���o�[���\��
            nHeight = CInt(dgv.ClientSize.Height / dgv.Rows.Count)
        End If

        ' �s�����̐ݒ�
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv.Rows(ii).Height = nHeight   ' �s����ݒ�

            ' �񐔕��̕����T�C�Y��ݒ�
            For jj As Integer = 0 To dgv.ColumnCount - 1
                If hsc.Visible Then
                    ' �����X�N���[���o�[�\����
                    dgv.Item(jj, ii).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Else
                    ' �����X�N���[���o�[���\��
                    Dim emSize As Single = 14.0
                    ' �\�Ɨ����r���l���Z���̈���ɑS�ĕ\���ł��Ȃ��ꍇ�i9�����ȏ�j
                    If ii = 1 AndAlso Not dgv(jj, ii).Value Is Nothing AndAlso dgv(jj, ii).Value.ToString.Count > 8 Then
                        ' �Z���̈���ɕ�����S�ĕ\���ł���悤�ɕ����T�C�Y��ύX
                        emSize = 10.0
                    End If

                    dgv(jj, ii).Style.Font = New System.Drawing.Font("�l�r ����", emSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End If
            Next
        Next

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �}�b�v�֌W
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �}�b�v��{����`
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInitialize()
        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        For ii As Integer = 0 To mcls_pMap.Length - 1
            With mcls_pMap(ii)
                '' �\���̈���
                .tProp_AriaInf_BackGroundLineColor = Pens.Black
                .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
                .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
                .tProp_AriaInf_AriaSize = picMap1.Size
                .tProp_AriaInf_Margin = New Padding(30, 10, 30, 40)

                '' �}�b�v�̈�
                .tProp_MapInf_IsCenterX = False
                .tProp_MapInf_IsCenterY = False
                .tProp_MapInf_IsBaseX_L = True
                .tProp_MapInf_IsBaseY_T = False
                .tProp_MapInf_IsStrechX = True
                .tProp_MapInf_IsStrechY = True
                .tProp_MapInf_LimitXmin = 1
                .tProp_MapInf_LimitXmax = 1000
                .tProp_MapInf_LimitYmin = 0
                .tProp_MapInf_LimitYmax = 100
                .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

                '' �}�b�v�ΏۊO�̈�ݒ�
                .tProp_LimitInf_LimitDraw = False
                '''' �R�C����ݒ肷��
                '.tProp_LimitInf_LimitXmin = .tProp_MapInf_LimitXmin
                '.tProp_LimitInf_LimitXmax = .tProp_MapInf_LimitXmax
                '.tProp_LimitInf_LimitYmin = .tProp_MapInf_LimitYmin
                '.tProp_LimitInf_LimitYmax = .tProp_MapInf_LimitYmax

                '' �������ݒ�
                Dim p As Pen = New Pen(Color.LightBlue, 1)
                p.DashStyle = Drawing2D.DashStyle.DashDot
                .tProp_MemoriInf_LinePen = p
                .tProp_MemoriInf_StringFont = New Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
                .tProp_MemoriInf_PosStringX = 10
                .tProp_MemoriInf_PosStringY = 1
                .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(100, 10)
                .tProp_MemoriInf_ScaleStringFormatX = "0�{�O"
                .tProp_MemoriInf_ScaleStringFormatY = "0'%'"
                .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1)
                .tProp_MemoriInf_StringDisp_T = False
                .tProp_MemoriInf_StringDisp_B = True
                .tProp_MemoriInf_StringDisp_L = True
                .tProp_MemoriInf_StringDisp_R = False
                .tProp_MemoriInf_LineDsp_X = True
                .tProp_MemoriInf_LineDsp_Y = True
            End With
        Next
    End Sub

    ''' <summary>
    ''' �}�b�v�Ē�`
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapReSetring()
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting

        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        For ii As Integer = 0 To mcls_pMap.Length - 1
            With mcls_pMap(ii)
                '' �}�b�v�̈�
                .tProp_MapInf_LimitXmax = typ.nXmax
                .tProp_MapInf_LimitYmax = typ.nYmax

                '' �������ݒ�
                .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(typ.nXinterval, typ.nYinterval)
            End With
        Next
    End Sub

    ''' <summary>
    ''' �`��
    ''' </summary>
    Private Sub PictureBox1_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles picMap1.Paint, picMap2.Paint
        Dim ou As Integer = CInt(IIf(sender Is picMap1, 0, 1))   ' �\��

        '''' �r�b�g�}�b�v�擾
        Dim bmp As Bitmap = mcls_pMap(ou).tProp_Bmp
        If bmp Is Nothing Then Return

        '''' �w�i���Z�b�g
        e.Graphics.DrawImage(bmp, 0, 0)
    End Sub




    ''' <summary>
    ''' ��ʃf�[�^�`��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics) Handles mcls_Map_O.tEv_DrawPaint_End, mcls_Map_U.tEv_DrawPaint_End
        If m_nProtData = 0 Then Return

        Dim ou As Integer = CInt(IIf(cls Is mcls_Map_O, 0, 1))   ' �\��
        Dim typ As frmDspSetting.TYP_SETTING = mfrm_Setting.tprop_TypSetting


        '''' �`��
        Dim dVal As Single
        Dim x1, y1, x2, y2 As Integer
        For ii As Integer = typ.nDspType.Length - 1 To 0 Step -1
            If typ.nDspType(ii) = 0 Then Continue For

            ' �n�_�Z�o (�擪�̍��W�Z�o)
            x1 = cls.GetVtoP_X(1)
            dVal = CSng(CInt(IIf(ou = 0, m_typProtData(0).nCntO(ii), m_typProtData(0).nCntU(ii))) / m_typProtData(0).nCoilLen) * 100
            If dVal > typ.nYmax Then dVal = typ.nYmax
            y1 = cls.GetVtoP_Y(dVal)

            Using p As New Pen(typ.colorLine(ii), 1)
                ' �n�_�̂݃v���b�g�̎w��
                Select Case ii
                    Case 0
                        g.FillEllipse(New SolidBrush(p.Color), x1 - 4, y1 - 4, 8, 8)       ' ��
                    Case 1
                        g.FillRectangle(New SolidBrush(p.Color), x1 - 3, y1 - 3, 6, 6)      ' ��
                    Case 2
                        g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x1, y1 + 4), New Point(x1 + 4, y1 - 4), New Point(x1 - 4, y1 - 4)})    ' ��
                    Case 3
                        g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x1, y1 - 4), New Point(x1 + 4, y1 + 4), New Point(x1 - 4, y1 + 4)})   ' ��
                    Case Else
                        g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x1, y1 - 4), New Point(x1 + 4, y1), New Point(x1, y1 + 4), New Point(x1 - 4, y1)}) ' ��
                End Select


                For jj As Integer = 1 To m_nProtData - 1
                    ' �I�_�Z�o
                    x2 = cls.GetVtoP_X(jj + 1)
                    dVal = CSng(CInt(IIf(ou = 0, m_typProtData(jj).nCntO(ii), m_typProtData(jj).nCntU(ii))) / m_typProtData(jj).nCoilLen) * 100
                    If dVal > typ.nYmax Then dVal = typ.nYmax
                    y2 = cls.GetVtoP_Y(dVal)

                    ' �`��
                    g.DrawLine(p, x1, y1, x2, y2)

                    ' �v���b�g�̎w��
                    Select Case ii
                        Case 0
                            g.FillEllipse(New SolidBrush(p.Color), x2 - 4, y2 - 4, 8, 8)       ' ��
                        Case 1
                            g.FillRectangle(New SolidBrush(p.Color), x2 - 3, y2 - 3, 6, 6)      ' ��
                        Case 2
                            g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x2, y2 + 4), New Point(x2 + 4, y2 - 4), New Point(x2 - 4, y2 - 4)})   ' ��
                        Case 3
                            g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x2, y2 - 4), New Point(x2 + 4, y2 + 4), New Point(x2 - 4, y2 + 4)})   ' ��
                        Case Else
                            g.FillPolygon(New SolidBrush(p.Color), New Point() {New Point(x2, y2 - 4), New Point(x2 + 4, y2), New Point(x2, y2 + 4), New Point(x2 - 4, y2)}) ' ��
                    End Select


                    ' ���̏���
                    x1 = x2
                    y1 = y2
                Next jj
            End Using

        Next ii


        '''' ���������̃R�C��No��\��
        Dim b As SolidBrush = New SolidBrush(Color.Black)
        Dim f As Font = New Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        ' ���_
        x1 = cls.GetVtoP_X(1)
        y1 = CInt(IIf(ou = 0, picMap1.Height, picMap2.Height))
        cls.DrawString(g, x1, y1, 0, -1, m_typProtData(0).sCoilNo, ContentAlignment.TopCenter, f, b)
        ' ��������
        For ii As Integer = typ.nXinterval To m_nProtData Step typ.nXinterval
            ' �\������f�[�^�́A��������100�̏ꍇ�́A�z���99�B�������P���z���0��\�����Ă����
            x1 = cls.GetVtoP_X(ii)
            cls.DrawString(g, x1, y1, 0, -1, m_typProtData(ii - 1).sCoilNo, ContentAlignment.TopCenter, f, b)
        Next ii
        b.Dispose()
        f.Dispose()
    End Sub


End Class
