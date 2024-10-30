'*********************************************************************************
' �p�����[�^�ݒ��ʂ̃��X�g�t���̌p���t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary.tClass

''' <summary>
''' �p�����[�^�ݒ��ʗp�̃��X�g�t���̌p���N���X
''' </summary>
''' <remarks></remarks>
Public Class frmMain_AutoParSet_UpList

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �A�b�v�R�[�h�̗�ʒu
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum em_UpCodeItem
        iCheck = 0                      ' �`�F�b�N�{�b�N�X
        iNo                             ' No
        iName                           ' ����
    End Enum

    Public m_KeyCode As String = ""                                    ' ��L�[
    Public m_cmbSCode As New ComboBox
    Public m_lblSCode As New Label
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_ParSet_UpList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        '''' �O���b�h�r���[��`
        dgvInitialize_UpCode(dgvUpCodeItem)
        '''' �O���b�h�r���[���`
        dgvColumnDef_UpCode(dgvUpCodeItem)
        MyBase.DataGridViewSelectionCancel(dgvUpCodeItem)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �A�b�v�R�[�h���� �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �A�b�v�R�[�h �R���{�{�b�N�X�ɃZ�b�g����Ă���A�C�e����S���Z�b�g
    ''' </summary>
    ''' <param name="cmb">�A�b�v�R�[�h�p�̃R���{�{�b�N�X</param>
    ''' <param name="dgv">�A�b�v�R�[�h�p�̃f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Public Sub SetCmbUpCodeItem(ByVal cmb As ComboBox, ByVal dgv As DataGridView)
        ' �A�b�v�R�[�h �R���{�{�b�N�X�ɃZ�b�g����Ă���A�C�e����S���Z�b�g
        dgv.RowCount = 0
        For Each item As cls_CmbItem In cmb.Items
            dgv.Rows.Add(False, item.id, item.name)
        Next item
    End Sub


    ''' <summary>
    ''' �A�b�v�R�[�h�����̑S�I���{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnUpCodeItem_All_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_All.Click
        ' �S�Ẵ`�F�b�N�{�b�N�XON
        For ii As Integer = 0 To dgvUpCodeItem.RowCount - 1
            dgvUpCodeItem(em_UpCodeItem.iCheck, ii).Value = True
        Next
    End Sub
    ''' <summary>
    ''' �A�b�v�R�[�h�����̑S�����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnUpCodeItem_Clrea_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_Clrea.Click
        ' �S�Ẵ`�F�b�N�{�b�N�XON
        For ii As Integer = 0 To dgvUpCodeItem.RowCount - 1
            dgvUpCodeItem(em_UpCodeItem.iCheck, ii).Value = False
        Next
    End Sub
    ''' <summary>
    ''' �A�b�v�R�[�h�̃`�F�b�N�{�b�N�X���ύX���ꂽ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvUpCodeItem_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvUpCodeItem.CellValueChanged
        ' �`�F�b�N�{�b�N�X�̃f�[�^���̂�
        If e.RowIndex < 0 OrElse e.ColumnIndex <> em_UpCodeItem.iCheck Then Exit Sub

        '1�s�S���F�ς�
        dgvUpCodeItem.Rows(e.RowIndex).DefaultCellStyle.BackColor = CType(IIf(CBool(dgvUpCodeItem(e.ColumnIndex, e.RowIndex).Value), g_ColorSetting, Color.Empty), Color)
    End Sub

    ''' <summary>
    ''' �Z���̓��e���ω������ꍇ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridView�̃`�F�b�N�{�b�N�X�������ꂽ�ꍇ �ʏ� �Z�����ړ������Ȃ��Ɗm��ɂȂ�Ȃ��ׂ̏��u</remarks>
    Private Sub dgvUpCodeItem_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvUpCodeItem.CurrentCellDirtyStateChanged
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ �R�~�b�g����
        If dgvUpCodeItem.IsCurrentCellDirty Then dgvUpCodeItem.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub


    ''' <summary>
    ''' �R���{�{�b�N�X �I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbUpCodeItem_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbUpCodeItem.SelectedIndexChanged
        ' �R���{�{�b�N�X�ύX
        MyBase.SelectComboBoxItem(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem, lblUpCodeItem)
    End Sub





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize_UpCode(ByVal dgv As DataGridView)
        With dgv
            .RowCount = 1
            .ColumnCount = 0

            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

        End With
    End Sub

    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvColumnDef_UpCode(ByVal dgv As DataGridView)
        With dgv
            .ColumnCount = 0

            ''=================================
            '' ���`

            ' �I��p�`�F�b�N�{�b�N�X
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = ""
                clm.Width = 20
                clm.Frozen = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter

                .Columns.Add(clm)
            End Using

            ' No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "No"
                clm.Width = 35
                clm.Frozen = False
                clm.Selected = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter

                .Columns.Add(clm)
            End Using

            ' ����
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "����"
                clm.Width = 110
                clm.Frozen = False
                clm.Selected = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter

                .Columns.Add(clm)
            End Using
        End With
    End Sub

    ''' <summary>
    ''' �A�b�v�R�[�h�̃R�s�[��A�C�e���擾
    ''' </summary>
    ''' <param name="isNum">��L�[�͐��l�p�̏ꍇTrue</param>
    ''' <returns>�A�C�e�� ("1,2,3")�܂���("'1','2','3'")</returns>
    ''' <remarks></remarks>
    Public Function GetUpCodeItem_To(ByVal isNum As Boolean) As String
        Dim strWk As String = ""
        '�`�F�b�N�A�C�e���擾
        For ii As Integer = 0 To dgvUpCodeItem.RowCount - 1
            If CBool(dgvUpCodeItem(0, ii).Value) Then
                If strWk.Length <> 0 Then
                    strWk &= ", "
                End If

                If isNum Then '���l
                    strWk &= SelectComboBoxKey(ii, cmbUpCodeItem)
                Else            ' ����
                    strWk &= "'" & SelectComboBoxKey(ii, cmbUpCodeItem) & "'"
                End If
            End If
        Next ii
        Return strWk
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �R�s�[������R�s�[��փf�[�^�X�V
    ''' </summary>
    ''' <param name="strSQL">���sSQL��</param>
    ''' <param name="strErrMsg">�G���[���b�Z�[�W</param>
    ''' <returns>�e�����󂯂��s����Ԃ��@-1:���s -9:DB�ڑ����s </returns>
    ''' <remarks></remarks>
    Public Function UpCode_UpDate(ByVal strSQL As String, ByRef strErrMsg As String) As Integer

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        '���b�Z�[�W������
        strErrMsg = ""

        Try

            ' SQL���s
            If Not tcls_DB.Open() Then
                strErrMsg = "DB�ڑ����s"
                Return -9
            End If

            ' �X�V
            Return tcls_DB.DMLExecute(strSQL)

        Catch ex As Exception
            strErrMsg = ex.Message
            Return -1
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

    End Function

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Public Overloads Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��
        pnlItem.Enabled = bPassResult                   ' �R�s�[�p���X�g�̔��f�{�^��

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �A�b�v�R�[�h �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���f�{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overridable Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click

        ' �R�s�[���̏����`�F�b�N
        '��
        Dim strFromKey As String = ""                                    '�R�s�[���L�[ 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '��
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        '��
        Dim strToKey As String = GetUpCodeItem_To(False)                '�R�s�[��L�[
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            'DB�R�s�[
            If CopyDB(strFromKey, strToKey) Then
                '����̎��͓����@�\�փp�����[�^�ύX�ʒm
                modMain.Send_Main(MyBase.tProp_MyParamID, CH_SCODE_ALL)
                modMain.HardCopy(Me)
                frmMsg.Text = "���f����"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "���f����"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' �f�[�^�̍Ď擾
            If Not MyBase.DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                frmMsg.Dispose()
                modMain.TaskEnd()                       '�I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' ��ʂɕ\��
        Data_Disp(m_KeyCode)

    End Sub
    ''' <summary>
    ''' �R�s�[����
    ''' </summary>
    ''' <param name="strFromKey">�R�s�[���L�[ </param>
    ''' <param name="strToKey">�R�s�[��L�[ </param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Public Overridable Function CopyDB(ByVal strFromKey As String, ByVal strToKey As String) As Boolean
        For Each Data As DataSetHolder In m_DataSets
            If Not CopyTargetTable(Data) Then Continue For
            '�\�ʏ�ԕ�(ParamScode)���X�V
            Dim strErrMsg As String = ""
            If 0 > UpCode_UpDate(MyBase.Create_Sql_Update(Data, strFromKey, strToKey), strErrMsg) Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, MyBase.GetItemDBName(Data) & " �X�V�G���[ [" & strErrMsg & "] " & Me.Text)
                Return False
            End If
        Next
        Return True
    End Function


    ''' <summary>
    ''' �����\�ʏ��No�R���{�{�b�N�X��񏉊��Z�b�g
    ''' </summary>
    ''' <param name="comb">�R���{�{�b�N�X�R���g���[��</param>
    ''' <param name="lblScode">���x��</param>
    ''' <remarks></remarks>
    Public Sub Set_cmbSCode_Item(ByVal comb As ComboBox, ByVal lblScode As Label)
        m_cmbSCode = comb
        m_lblSCode = lblScode
        ' ���̊֐��ɃC�x���g�n���h�����t
        AddHandler comb.SelectedIndexChanged, AddressOf cmbSCode_SelectedIndexChanged
    End Sub

    ''' <summary>
    ''' �����\�ʏ��No�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        ' ���̑I���Ǝ�L�[�������ꍇ�͖���
        If m_KeyCode = m_cmbSCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ�
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check() Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                m_cmbSCode.Text = m_KeyCode
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_Get()
        End If

        ' �I��ύX�ɔ��� ���x���l�ύX
        MyBase.SelectComboBoxItem(m_cmbSCode.SelectedIndex, m_cmbSCode, m_lblSCode)

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(m_cmbSCode.SelectedIndex, m_cmbSCode)
        Data_Disp(m_KeyCode)

        '���̑I����Ԃ�ۑ�
        g_nScodeIndex = m_cmbSCode.SelectedIndex

    End Sub

    ''' <summary>
    ''' �O�� ����
    ''' </summary>
    ''' <param name="bln">�O��:false ����:true</param>
    ''' <remarks></remarks>
    Public Sub ChangeCode(ByVal bln As Boolean)
        Dim rec As Integer = CInt(IIf(bln, m_cmbSCode.SelectedIndex + 1, m_cmbSCode.SelectedIndex - 1))
        ' �㉺���`�F�b�N
        If rec < 0 OrElse rec >= m_cmbSCode.Items.Count Then Exit Sub
        '�ؑ�
        m_cmbSCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' ���񃍁[�f�B���O����(�����\�ʏ��(scode)�L��)
    ''' </summary>
    ''' <remarks></remarks>
    Public Overloads Sub InitLoading()
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
            Exit Sub
        End If

        MyBase.dgvInitialize()

        ' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
        MyBase.SetComboBox_Scode(m_cmbSCode)

        If 0 < m_cmbSCode.Items.Count Then

            '�O�̉�ʂőI��������Ԃ��Z�b�g
            If m_cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '�Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
                m_cmbSCode.SelectedIndex = 0
            Else
                m_cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        For Each Data As DataSetHolder In m_DataSets
            Data.dgvCnt.dgv.DataGridViewSelectionRowColorChange(g_ColorSelection)
            MyBase.DataGridViewColumnWidthChanged(Data.dgvCnt.dgv)
        Next

        '=======================================================
        '�v�����^���Ȃ���΁A��ʈ������
        If Not tClass_HeadCopy.IsCheckPrinter Then
            btnF07.Enabled = False
        Else
            btnF07.Enabled = True
        End If

        frmMsg.Dispose()
        frmMsg = Nothing
    End Sub

    ''' <summary>
    ''' ���񃍁[�f�B���O����(�����\�ʏ��(scode)�L��)(�}�j���A���p)
    ''' </summary>
    ''' <remarks></remarks>
    Public Overloads Sub InitLoading2()
        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
            Exit Sub
        End If

        ' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
        MyBase.SetComboBox_Scode(m_cmbSCode)

        If 0 < m_cmbSCode.Items.Count Then

            '�O�̉�ʂőI��������Ԃ��Z�b�g
            If m_cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '�Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
                m_cmbSCode.SelectedIndex = 0
            Else
                m_cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '=======================================================

        frmMsg.Dispose()
        frmMsg = Nothing
    End Sub

End Class
