'*********************************************************************************
' �����\�ʏ�ԕʏo�͐ݒ��ʃN���X
'	[Ver]
'		Ver.01    2007/08/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmSCodeOutPut
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum em_DataColumn
        iText_No = 0            ' �e�L�X�g�{�b�N�X  No
        iCombo_Name             ' �R���{�{�b�N�X  �r��
        iCombo_Dummy            ' �R���{�{�b�N�X  �_�~�[�o��
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterSCode As SqlClient.SqlDataAdapter = Nothing        ' �A�_�v�^�[ �����\�ʏ�ԕʏo�͐ݒ�
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[ �r��}�X�^�[
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing        ' �A�_�v�^�[ �O���[�h�}�X�^�[

    Private m_DsSCode As New DataSet                                    ' ��ڑ��^�e�[�u�� �����\�ʏ�ԕʏo�͐ݒ�f�[�^
    Private m_DsType As New DataSet                                     ' ��ڑ��`�e�[�u�� �r��}�X�^�[ �R���{�{�b�N�X�p
    Private m_DsGrade As New DataSet                                    ' ��ڑ��^�e�[�u�� �O���[�h�}�X�^�[ �R���{�{�b�N�X�p
    Private m_KeyCode As String = ""                                    ' ��L�[

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���j���[�{�^��
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmSetSCode_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 0
                btnF01.Select()                 ' F01�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(False)               ' �O��
            Case 1
                btnF02.Select()                 ' F02�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(True)                ' ����
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�����\�ʏ�ԕʏo�͐ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLOR�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select

    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmSetSCode_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DsSCode.Dispose()
        m_DsType.Dispose()
        m_DsGrade.Dispose()

        If Not m_AdapterSCode Is Nothing Then
            m_AdapterSCode.Dispose()
            m_AdapterSCode = Nothing
        End If

        If Not m_AdapterType Is Nothing Then
            m_AdapterType.Dispose()
            m_AdapterType = Nothing
        End If

        If Not m_AdapterGrade Is Nothing Then
            m_AdapterGrade.Dispose()
            m_AdapterGrade = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmSetSCode_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                 ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                             ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmSCodeOutput       ' �����\�ʏ�ԕʏo�͐ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �A�b�v�R�[�h�̃R���{�{�b�N�X
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾���� �R���{�{�b�N�X�̒l���Ƃ��Ă���̂ŁAdgv�ݒ����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' �R���{�{�b�N�X�̕\���f�[�^��������
        Data_init()

        ' dgv�̐ݒ�
        dgvInitialize(dgvData)
        dgvColunmDefData(dgvData, True)
        MyBase.DataGridViewSelectionCancel(dgvData)

        dgvInitialize(dgvEtc)
        dgvColunmDefData(dgvEtc, False)
        MyBase.DataGridViewSelectionCancel(dgvEtc)

        ' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
        MyBase.SetComboBox_Scode(cmbSCode)

        If 0 < cmbSCode.Items.Count Then
            '�O�̉�ʂőI��������Ԃ��Z�b�g
            If cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '�Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
                cmbSCode.SelectedIndex = 0
            Else
                cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        Data_Disp(m_KeyCode)

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        MyBase.DataGridViewSelectionRowColorChange(dgvData)

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

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
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSetSCode_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL������ �����\�ʏ�ԕʏo�͂��r���ID�ŏ��� �����e�[�u�����Q�Ƃ��Ă���̂ŁA�f�[�^�A�_�v�^�ŃA�b�v�f�[�g�ł��Ȃ�
        strSQL = "SELECT scode , "
        strSQL &= " �r��EdasysID, "
        strSQL &= " �_�~�[�o�� "
        strSQL &= " FROM " & DB_SCODE_OUTPUT
        strSQL &= " AS a LEFT OUTER JOIN " & DB_TYPE_NAME
        strSQL &= " AS b ON a.�r��EdasysID = b.EdasysID "
        strSQL &= " ORDER by b.ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsSCode, m_AdapterSCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_SCODE_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������ �r���ID�ŏ���
        strSQL = "SELECT EdasysID, Name "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������ �O���[�h�͗D��x�ŏ���
        strSQL = "SELECT EdasysID, Name ,�}�b�v�\���F "
        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY �D��x ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql��

        Try
            '''' �f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' �f���[�g�C���T�[�g����̂Ō����\�ʏ�ԏo�͐ݒ�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_SCODE_OUTPUT
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�����\�ʏ�ԏo�͐ݒ�e�[�u���փf�[�^��o�^ 
            '�f�[�^�e�[�u���͕����e�[�u�����Q�Ƃ��Ă���ׁA�A�b�v�f�[�g�ł��Ȃ��̂ŁA�C���T�[�g����
            For ii As Integer = 0 To m_DsSCode.Tables(0).Rows.Count - 1
                With m_DsSCode.Tables(0).Rows(ii)
                    If CInt(.Item("�r��EdasysID").ToString) <> 0 Then
                        'TYPEOUT�o�^SQL���쐬
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_SCODE_OUTPUT & " VALUES ("
                        strSql &= .Item("scode").ToString & ","             'scode
                        strSql &= .Item("�r��EdasysID").ToString & ","      '�r��EdasysID
                        strSql &= .Item("�_�~�[�o��").ToString & ")"        '�_�~�[�o��

                        'SQL���s
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' �G���[�Ȃ烍�[���o�b�N���ďI��
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_SCODE, CH_SCODE_ALL)       ' ����ύX�������킩��Ȃ��̂ŁA�S�ύX�敪�𑗂�
			modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' ��n��
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' �����p�^�[���ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbSCode.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂� ���ɖ߂����� �̎��̑΍�)
        If m_KeyCode = cmbSCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ���
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                cmbSCode.Text = m_KeyCode
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_Get(m_KeyCode)
        End If

        ' �I��ύX�ɔ��� ���x���l�ύX
        MyBase.SelectComboBoxItem(cmbSCode.SelectedIndex, cmbSCode, lblSCode)

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        Data_Disp(m_KeyCode)

        '���̑I����Ԃ�ۑ�
        g_nScodeIndex = cmbSCode.SelectedIndex

    End Sub
    ''' <summary>
    ''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
    ''' </summary>
    ''' <remarks>�O���[�h���̂Ɉȏ������A�����̍s��ǉ��A</remarks>
    Private Sub Data_Init()
        Dim drowNewType As DataRow      ' �r��}�X�^�[�̐V�K�s
        Dim drowNewGrade As DataRow     ' �r��O���[�h�̐V�K�s

        Try
            '�r���ڑ��e�[�u���̐ݒ�
            ' �r��̐V�K�s��ݒ� EdasysID�A�r���� 
            drowNewType = m_DsType.Tables(0).NewRow()
            drowNewType.Item("EdasysID") = 0
            drowNewType.Item("Name") = "����"
            ' �e�[�u���̈�ԏ�ɑ}��
            m_DsType.Tables(0).Rows.InsertAt(drowNewType, 0)

            '�O���[�h��ڑ��e�[�u���̐ݒ�
            ' �S�s���[�v �O���[�h�̖��̂Ɉȏ������
            For ii As Integer = 0 To m_DsGrade.Tables(0).Rows.Count - 1
                m_DsGrade.Tables(0).Rows(ii).Item("Name") = m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString & "�ȏ�"
            Next
            ' �O���[�h�̐V�K�s��ݒ� EdasysID�A�O���[�h���́A�}�b�v�\���F
            drowNewGrade = m_DsGrade.Tables(0).NewRow()
            drowNewGrade.Item("EdasysID") = 0
            drowNewGrade.Item("Name") = "-"
            drowNewGrade.Item("�}�b�v�\���F") = -1      ' �}�b�v�\���F�A�g��Ȃ����ǈꉞ�ݒ�
            ' �e�[�u���̈�ԏ�ɑ}��
            m_DsGrade.Tables(0).Rows.InsertAt(drowNewGrade, 0)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�p�f�[�^ �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub
    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�����p�^�[�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("scode='{0}' and �r��EdasysID>=0", key)          ' WHERE��ɑ����̏������� 0�`999���r��EdasysID�����o�����͂�
        Dim strWhereEtc As String = String.Format("scode='{0}' and �r��EdasysID=-1", key)       ' WHERE��ɑ����̏������� -1���r��EdasysID�����o�����͂�
        Dim drow() As DataRow = m_DsSCode.Tables(0).Select(strWhere)                            ' EdasysID��1�ȏ�̃f�[�^��    0�`64�s
        Dim drowEtc() As DataRow = m_DsSCode.Tables(0).Select(strWhereEtc)                      ' EdasysID��-1�̂��̑��f�[�^������Ă���   0�`1�s
        Dim nRowCount As Integer = 0                                                            ' �s���J�E���g�p

        Try
            ' ��ʂɕ\��
            dgvData.RowCount = MAX_TYPE                 ' �Œ�64�s
            For ii As Integer = 0 To dgvData.RowCount - 1
                dgvData(em_DataColumn.iText_No, ii).Value = ii + 1                                  ' No
                If ii < drow.Length Then
                    ' �����\�ʏ�ԕʏo�͐ݒ�e�[�u����scode�ŏォ�猟������ (select���g���ƃ\�[�g�����̂�)
                    For kk As Integer = nRowCount To m_DsSCode.Tables(0).Rows.Count - 1
                        ' scode��key�̒l�ŁA�r��EdasysID��-1�ȊO�̍s���擾���A�s���J�E���g���C���N�������g                    
                        If m_DsSCode.Tables(0).Rows(kk).Item("scode").ToString = key AndAlso
                        CInt(m_DsSCode.Tables(0).Rows(kk).Item("�r��EdasysID").ToString) >= 0 Then
                            dgvData(em_DataColumn.iCombo_Name, ii).Value = m_DsSCode.Tables(0).Rows(kk).Item("�r��EdasysID")    ' �r��
                            dgvData(em_DataColumn.iCombo_Dummy, ii).Value = m_DsSCode.Tables(0).Rows(kk).Item("�_�~�[�o��")     ' �_�~�[�o��
                            nRowCount = kk + 1
                            Exit For
                        End If
                    Next kk
                Else
                    ' �\���f�[�^�������s��0�i�����j������
                    dgvData(em_DataColumn.iCombo_Name, ii).Value = 0                                ' �r��
                    dgvData(em_DataColumn.iCombo_Dummy, ii).Value = 0                               ' �_�~�[�o��
                End If
            Next ii

            ' ���̑�dgv�̕\��
            dgvEtc.RowCount = 1                                                             ' ���̑�dgv�͈�s�Œ�
            dgvEtc(em_DataColumn.iText_No, 0).Value = ""                                    ' No    ��
            dgvEtc(em_DataColumn.iCombo_Name, 0).Value = "��L�ȊO"                         ' �r��͏�L�ȊO�ŌŒ�
            ' DB�ɕ\���f�[�^�����邩
            If drowEtc.Length > 0 Then
                dgvEtc(em_DataColumn.iCombo_Dummy, 0).Value = drowEtc(0).Item("�_�~�[�o��") ' �_�~�[�o��
            Else
                ' �\���f�[�^�������s��0�i�����j������
                dgvEtc(em_DataColumn.iCombo_Dummy, 0).Value = 0 ' �_�~�[�o��
            End If
            ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
            dgvData(1, 0).Selected = True
            dgvEtc(1, 0).Selected = True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return
        End Try
    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode) ���̏��g��Ȃ����ȁ[</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strMsg As String       ' �G���[���b�Z�[�W�p
        ' �s�������[�v �R���{�{�b�N�X�����Ȃ��̂ŁA�r��EdasysID�̏d���`�F�b�N�̂ݍs��
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            ' �r�킪�����̍s�͖���
            If CInt(dgvData(em_DataColumn.iCombo_Name, ii).Value) = 0 Then Continue For

            ' �r��̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvData.Rows.Count - 1
                ' ��r�悪�����̍s�͖���
                If CInt(dgvData(em_DataColumn.iCombo_Name, jj).Value) = 0 Then Continue For

                ' �r�킪�����Ȃ烁�b�Z�[�W���o���ďI��
                If CInt(dgvData(em_DataColumn.iCombo_Name, ii).Value) = CInt(dgvData(em_DataColumn.iCombo_Name, jj).Value) Then
                    strMsg = "�y �r�� �z" & vbNewLine
                    strMsg &= "�����r��͓o�^�ł��܂���B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                    MsgBox(strMsg)
                    Return False
                End If
            Next
        Next ii
        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String = String.Format("scode='{0}'", key)  ' WHERE��ɑ����̏�������
        Dim drow() As DataRow                                       ' �f�[�^�s
        Dim nRowCount As Integer                                    ' �폜�Ώۂ̃f�[�^��
        drow = m_DsSCode.Tables(0).Select(strWhere)           ' scode����L�[�̒l�̍s�����o��

        '���݉�ʂɕ\�����Ă���scode�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DsSCode.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next

        ' ���\�����Ă���f�[�^��S�Ċi�[ 
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            Dim drowNew As DataRow = m_DsSCode.Tables(0).NewRow()   ' �V�K�s
            ' �f�[�^���Z�b�g
            drowNew.Item("scode") = CInt(key)
            drowNew.Item("�r��EdasysID") = dgvData(em_DataColumn.iCombo_Name, ii).Value
            drowNew.Item("�_�~�[�o��") = dgvData(em_DataColumn.iCombo_Dummy, ii).Value

            ' �e�[�u���ɒǉ�
            m_DsSCode.Tables(0).Rows.InsertAt(drowNew, m_DsSCode.Tables(0).Rows.Count)
        Next

        ' ���̑��s���i�[
        ' �f�[�^���Z�b�g
        Dim drowNewEtc As DataRow = m_DsSCode.Tables(0).NewRow()  ' �V�K�s
        drowNewEtc.Item("scode") = CInt(key)
        drowNewEtc.Item("�r��EdasysID") = -1                            ' ���̑��s��EdasysID��-1�ŌŒ�
        drowNewEtc.Item("�_�~�[�o��") = dgvEtc(em_DataColumn.iCombo_Dummy, 0).Value
        ' �e�[�u���ɒǉ�
        m_DsSCode.Tables(0).Rows.InsertAt(drowNewEtc, m_DsSCode.Tables(0).Rows.Count)
    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���̉�ʕ\�����̒l�Ǝ�荞��
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then Exit Sub

            Data_Get(m_KeyCode)
        End If

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '' �]�v��EdasysID�̍s���J�b�g
            'Data_Cut()
            ' �z���g�ɓo�^
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' �f�[�^�̍Ď擾
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �\���f�[�^���������A�����s��ǉ�������
        Data_Init()

        ' �ĕ\��
        Data_Disp(m_KeyCode)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As DataGridView)
        With dgv
            .ColumnCount = 0

            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '
            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent      ' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With
    End Sub
    ''' <summary>
    ''' dgv�̑I����Ԃ��ύX���ꂽ��A�Z���̑I�����L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvData.SelectionChanged, dgvEtc.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        dgvData.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' �f�[�^dgv���`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <param name="bDisptype">dgv�̃^�C�v ture = dgvData, false = dgvEtc</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDefData(ByVal dgv As DataGridView, ByVal bDisptype As Boolean)

        With dgv
            ''=================================
            '' ���`

            ' NO
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "NO"

                clm.Width = 60
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter

                .Columns.Add(clm)
            End Using

            ' �r��
            If bDisptype = True Then
                ' dgvData���i�v���p�e�B�Z�b�g
                Using clm As New DataGridViewComboBoxColumn
                    clm.HeaderText = "�r��"
                    clm.Width = 150
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = False
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DataSource = m_DsType.Tables(0)             ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�r��e�[�u���j
                    clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
                    clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
                    clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
                    .Columns.Add(clm)
                End Using
            Else
                ' dgvEtc���i�v���p�e�B�Z�b�g
                Using clm As New DataGridViewTextBoxColumn

                    clm.Width = 150
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.Frozen = False
                    clm.ReadOnly = True
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    .Columns.Add(clm)
                End Using
            End If

            ' �_�~�[�o��
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "�_�~�[�o��"
                clm.Width = 150
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Frozen = False
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DataSource = m_DsGrade.Tables(0)            ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
                clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
                clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
                clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
                .Columns.Add(clm)
            End Using

            If bDisptype = False Then
                ' dgvEtc�̓Ǝ��ݒ�
                .ColumnHeadersVisible = False                           ' �w�b�_�[��\��
            End If
        End With
    End Sub

    ''' <summary>
    ''' �Z���̏�Ԃ��ω��������̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvData.CurrentCellDirtyStateChanged, dgvEtc.CurrentCellDirtyStateChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ�R�~�b�g����
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub
    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter, dgvEtc.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑����܂��܂������� �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O�� ����
    ''' </summary>
    ''' <param name="bln">�O��:false ����:true</param>
    ''' <remarks></remarks>
    Private Sub ChangeCode(ByVal bln As Boolean)
        Dim rec As Integer = CInt(IIf(bln, cmbSCode.SelectedIndex + 1, cmbSCode.SelectedIndex - 1))
        ' �㉺���`�F�b�N
        If rec < 0 OrElse rec >= cmbSCode.Items.Count Then Exit Sub
        '�ؑ�
        cmbSCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' ����Z���̒l���ς������F��ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged, dgvEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s�ȊO�̓X���[
        If e.RowIndex < 0 Then Exit Sub

        ' �_�~�[�o�͗�Ȃ�F�ύX
        If e.ColumnIndex = em_DataColumn.iCombo_Dummy Then
            ' �Z���̒l���擾
            Dim strEdasysID As String = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

            ' �Z���̒l�iEdasysID�j�ŐF���擾���A�w�i�F��ύX���� 
            dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
        End If
    End Sub

    ''' <summary>
    ''' �F�擾
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbColor(ByVal key As String) As Color
        ' EdasysID�̒l�����o��
        Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE��ɑ����̏�������
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select(strWhere)       ' 1�s����͂�

        Dim retColor As Color = Color.Empty   ' �߂�l �����l��NULL�F

        ' Key��0�Ȃ�NULL�F��Ԃ�
        If 0 = CInt(key) Then Return retColor

        ' 1�s�����Ȃ��Ǝv������
        For ii As Integer = 0 To drowGrade.Length - 1
            ' �J���[���擾
            retColor = Color.FromArgb(CInt(drowGrade(0).Item("�}�b�v�\���F").ToString))
        Next

        Return retColor
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �A�b�v�R�[�h �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���f�{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click
        '''' �����`�F�b�N
        '�R�s�[��
        Dim strFromKey As String = ""                                    '�R�s�[���L�[ 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '��
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        '�R�s�[��
        Dim strToKey As String = MyBase.GetUpCodeItem_To(True)              '�R�s�[��L�[
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '''' �A�b�v�f�[�g �ύs�̂���UPDATE���͎g���Ȃ��̂ŁA�f���[�ƃC���T�[�g���Ă���
            If DB_UPDATE_UpCodeItem(strFromKey, strToKey) Then
                frmMsg.Text = "���f����"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "���f����"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            '''' �f�[�^�̍Ď擾
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �擾�f�[�^������
        Data_Init()

        '''' ��ʍĕ\��
        Data_Disp(m_KeyCode)
    End Sub
    ''' <summary>
    ''' �R�s�[����scode����R�s�[���scode�փf�[�^���f���[�g�C���T�[�g����
    ''' </summary>
    ''' <param name="strFromkey">�R�s�[���L�[</param>
    ''' <param name="strToKey">�R�s�[��̃L�[��������������</param>
    ''' <returns>����/�ُ�</returns>
    ''' <remarks></remarks>
    Private Function DB_UPDATE_UpCodeItem(ByVal strFromkey As String, ByVal strToKey As String) As Boolean
        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String = ""
        Dim strScode() As String                                ' �R�s�[���scode
        Dim strErrMsg As String = ""

        '' �J���}��؂��scode��z��ɕϊ�
        strScode = Split(strtokey, ",")

        '�R�s�[���̃f�[�^���擾
        strSQL = "SELECT scode , "
        strSQL &= " �r��EdasysID, "
        strSQL &= " �_�~�[�o�� "
        strSQL &= " FROM " & DB_SCODE_OUTPUT
        strSQL &= " WHERE scode='" & strFromkey & "'"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsSCode, m_AdapterSCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_SCODE_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If


        Try

            '''' �f���[�g�C���T�[�g
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' ���R�[�h�̍폜
            Dim strSqlDel As String     ' ���R�[�h�폜�psql
            strSqlDel = "DELETE FROM " & DB_SCODE_OUTPUT & " WHERE scode IN(" & strToKey & ")"

            If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False


            ' ���R�[�h�̑}�� �R�s�[��scode�̌������J��Ԃ�
            For ii As Integer = 0 To strScode.Length - 1
                ' �R�s�[��scode�̃��R�[�h�������J��Ԃ��}��
                For kk As Integer = 0 To m_DsSCode.Tables(0).Rows.Count - 1
                    With m_DsSCode.Tables(0).Rows(kk)
                        If CInt(.Item("�r��EdasysID").ToString) <> 0 Then
                            'OUTPUT_SCODE�o�^SQL���쐬
                            strSQL = ""
                            strSQL &= "INSERT INTO " & DB_SCODE_OUTPUT & " VALUES ("
                            strSQL &= strScode(ii) & ","                        'scode
                            strSQL &= .Item("�r��EdasysID").ToString & ","      '�r��EdasysID
                            strSQL &= .Item("�_�~�[�o��").ToString & ")"        '�_�~�[�o��
                            'SQL���s
                            If 0 > tcls_DB.DMLExecute(strSQL) Then
                                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                                tcls_DB.TransactRollback()
                                Return False
                            End If
                        End If
                    End With
                Next kk
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_SCODE, CH_SCODE_ALL)
			modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R�s�[�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
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
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��
        pnlItem.Enabled = bPassResult                   ' �R�s�[�p���X�g�̔��f�{�^��

    End Sub

End Class
