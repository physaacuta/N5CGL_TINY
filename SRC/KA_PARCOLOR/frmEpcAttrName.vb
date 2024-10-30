'*********************************************************************************
' �����ʖ��̃}�X�^�[�ݒ�
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcAttrName

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////

    ' �����ʖ��̃}�X�^�[�ݒ�
    Private Enum EM_ATTRNAME_GRID_ID
        emTxt_Enum = 0                  ' �e�L�X�g�{�b�N�X  �����ʕ��шʒu
        emTxt_EdasysID                  ' �e�L�X�g�{�b�N�X  EdasysID
        emTxt_Name                      ' �e�L�X�g�{�b�N�X  �����ʖ���
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterAttrName As SqlClient.SqlDataAdapter = Nothing     ' �A�_�v�^�[ (AttrName_Desk)
    Private m_DsAttrName As New DataSet                                 ' ��ڑ��^�e�[�u�� (AttrName_Desk)
    Private m_KeyCode As String = ""                                    ' ��L�[

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����{�^������������
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmEpcAttrName_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�����ʖ��̃}�X�^�[�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARSET�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' F11 �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select
    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂�ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmEpcAttrName_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        '' ���낢��J������
        'AttrName�֌W
        m_DsAttrName.Dispose()
        If Not m_AdapterAttrName Is Nothing Then
            m_AdapterAttrName.Dispose()
            m_AdapterAttrName = Nothing
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
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcAttrName_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcAttrName           ' �����ʖ��̃}�X�^�[�ݒ�Ăяo��

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �����ʖ��̃}�X�^�[�ݒ�
        dgvInitializeAttrName(dgvAttrName)

        ' �R���{�{�b�N�X�ɓ����ʃe�[�u�����Z�b�g
        SetComboBox_Attr()


        '' �f�[�^�̎擾
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '' ��ʂɒl��\��
        If Not Data_Disp(m_KeyCode) Then
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If


        ''�I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvAttrName.DataGridViewSelectionRowColorChange(g_ColorSelection)

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
    Private Sub frmGapSetting_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetComboBox_Attr()

        ' �����ʃe�[�u��
        cmbAttrName.Items.Add("��{������")
        cmbAttrName.Items.Add("�p�x�ʓ�����1")
        cmbAttrName.Items.Add("�p�x�ʓ�����2")
        cmbAttrName.Items.Add("�p�x�ʓ�����3")
        cmbAttrName.Items.Add("�p�x�ʓ�����4")
        m_KeyCode = "0"
        cmbAttrName.SelectedIndex = CInt(m_KeyCode)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ''AttrName�f�[�^�擾
        strSQL = "SELECT Angle"
        strSQL &= ", Enum, EdasysID, Name"
        strSQL &= " FROM " & DB_ATTR_NAME
        strSQL &= " ORDER BY Angle, Enum ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsAttrName, m_AdapterAttrName, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_ATTR_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp(ByVal key As String) As Boolean

        'AttrName�f�[�^�̕\��
        If Not Data_DispAttrName(key) Then Return False

        Return True
    End Function

    ''' <summary>
    ''' �����ʖ��̃}�X�^�[(AttrName)�f�[�^�̕\��
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_DispAttrName(ByVal key As String) As Boolean
        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�e�[�u�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("Angle='{0}'", key)          ' WHERE��ɑ����̏�������
        Dim drowAttrName() As DataRow = m_DsAttrName.Tables(0).Select(strWhere)         ' �S���R�[�h��\�����邽��WHERE��Ȃ�


        ' ��ʂɕ\��
        Try
            dgvAttrName.RowCount = drowAttrName.Length + 1                  ' ��{�I�Ƀ}�X�^�[�ɂ͐���ȃ��R�[�h��o�^�ς݂̂͂������A�ėp�I�ɂ����

            For ii As Integer = 0 To drowAttrName.Length - 1
                dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value = drowAttrName(ii).Item("Enum")                       ' �����ʕ��шʒu
                dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value = drowAttrName(ii).Item("EdasysID")               ' ������EdasysID
                dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value = drowAttrName(ii).Item("Name")                       ' �����ʖ���

            Next ii

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_ATTR_NAME & " �\���G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function

    ''' <summary>
    ''' �����ʃe�[�u���ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbAttrAngle_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbAttrName.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖���
        If m_KeyCode = CStr(cmbAttrName.SelectedIndex) Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ�
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                cmbAttrName.SelectedIndex = CInt(m_KeyCode)
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_Get(m_KeyCode)
        End If

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = CStr(cmbAttrName.SelectedIndex)
        Data_Disp(m_KeyCode)

    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���͒l�`�F�b�N
        If Not Data_Check(m_KeyCode) Then Exit Sub

        Data_Get(m_KeyCode)

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

                Data_Disp(m_KeyCode)
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If

        End Try

    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean

        'AttrName�f�[�^���̓`�F�b�N
        If Not Data_CheckAttrName(key) Then Return False

        Return True
    End Function

    ''' <summary>
    ''' �����ʖ��̃}�X�^�[(AttrName)�f�[�^���̓`�F�b�N
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_CheckAttrName(ByVal key As String) As Boolean

        Dim strWk As String = ""
        Dim strMsg As String    ' �d���p�̃G���[���b�Z�[�W
        Dim drowAttrName() As DataRow = m_DsAttrName.Tables(0).Select()         ' �S���R�[�h��\�����邽��WHERE��Ȃ�
        Dim nAngle As Integer
        Dim nEnum As Integer
        Dim nEdasysID As Integer

        For ii As Integer = 0 To dgvAttrName.Rows.Count - 2

            ' �����ʕ��шʒu
            strWk = CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, False, "�����ʕ��шʒu", True) <> 0 Then Return False
            dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value = strWk

            ' EdasysID
            strWk = CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99999, False, "������EdasysID", True) <> 0 Then Return False
            dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value = strWk

            ' EdasysID��0�̏ꍇ�͏d���`�F�b�N���s��Ȃ�
            If 0 <> CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value) Then
                ' EdasysID�̏d���`�F�b�N(���e�[�u��)
                For jj As Integer = ii + 1 To dgvAttrName.Rows.Count - 1
                    ' ��r���EdasysID�����l�Ȃ��r
                    If IsNumeric(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, jj).Value) Then
                        ' EdasysID�������Ȃ烁�b�Z�[�W���o���ďI��
                        If CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value) = CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, jj).Value) Then
                            strMsg = "�y �����ʖ���Ͻ���ݒ� �z" & vbNewLine
                            strMsg &= "����EdasysID�͓o�^�ł��܂���B" & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(CInt(key)).ToString & " ���шʒu " & CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value) & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(CInt(key)).ToString & " ���шʒu " & CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, jj).Value)
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj

                ' EdasysID�̏d���`�F�b�N(�ʃe�[�u��)
                For jj As Integer = 0 To drowAttrName.Length - 1
                    nAngle = CInt(drowAttrName(jj).Item("Angle"))
                    nEnum = CInt(drowAttrName(jj).Item("Enum"))
                    nEdasysID = CInt(drowAttrName(jj).Item("EdasysID"))
                    If CInt(key) <> nAngle Then
                        If CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value) = nEdasysID Then
                            strMsg = "�y �����ʖ���Ͻ���ݒ� �z" & vbNewLine
                            strMsg &= "����EdasysID�͓o�^�ł��܂���B" & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(CInt(key)).ToString & " ���шʒu " & CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value) & vbNewLine
                            strMsg &= "- - - " & cmbAttrName.Items.Item(nAngle).ToString & " ���шʒu " & CStr(nEnum)
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj
            End If

			' ����
			If System.DBNull.Value Is dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value Then
			Else
				strWk = CStr(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value)
				If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 64, "�����ʖ���", True) <> 0 Then Return False
				dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value = strWk
			End If
		Next ii

        Return True

    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)

        'AttrName�Z�b�g
        Call Data_GetAttrName(key)

    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^(AttrName)���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_GetAttrName(ByVal key As String)

        '' �ꊇ�̃e�[�u���Ȃ̂ŁA�e�[�u�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("Angle='{0}'", key)                  ' WHERE��ɑ����̏�������
        Dim drowAttrName() As DataRow = m_DsAttrName.Tables(0).Select(strWhere)     ' �S���R�[�h��\�����邽��WHERE��Ȃ�
        Dim nEnum As Integer = 0

        Try
            ' ����No�����[�v
            For ii As Integer = 0 To drowAttrName.Length - 1

                ' �Z�b�g
                drowAttrName(ii).Item("Angle") = key                                                            ' �����ʃe�[�u��
                drowAttrName(ii).Item("Enum") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value           ' �����ʕ��шʒu
                drowAttrName(ii).Item("EdasysID") = CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value)   ' ������EdasysID
                drowAttrName(ii).Item("Name") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value          ' �����ʖ���
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_ATTR_NAME & " �\���G���[ [" & ex.Message & "] " & Me.Text)
        End Try

        ' �V�K�s�̒ǉ�
        If drowAttrName.Length < (dgvAttrName.RowCount - 1) Then
            For ii As Integer = drowAttrName.Length To dgvAttrName.RowCount - 2
                Dim drowNew As DataRow = m_DsAttrName.Tables(0).NewRow                                          ' �V�K�s
                drowNew.Item("Angle") = key                                                                     ' �����ʃe�[�u��
                drowNew.Item("Enum") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Enum, ii).Value                    ' �����ʕ��шʒu
                drowNew.Item("EdasysID") = CInt(dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_EdasysID, ii).Value)      ' ������EdasysID
                drowNew.Item("Name") = dgvAttrName(EM_ATTRNAME_GRID_ID.emTxt_Name, ii).Value                    ' �����ʖ���
                ' �e�[�u���ɒǉ�
                m_DsAttrName.Tables(0).Rows.InsertAt(drowNew, m_DsAttrName.Tables(0).Rows.Count)

            Next ii
		End If

    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
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

            ' �f���[�g�C���T�[�g����̂Ńe�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_ATTR_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To m_DsAttrName.Tables(0).Rows.Count - 1
                With m_DsAttrName.Tables(0).Rows(ii)
                    '�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_ATTR_NAME & " VALUES ("
                    strSql &= .Item("Angle").ToString & ","                 ' �����ʃe�[�u��
                    strSql &= .Item("Enum").ToString & ","                  ' �����ʕ��шʒu
                    strSql &= .Item("EdasysID").ToString & ",'"             ' ������EdasysID
                    strSql &= .Item("Name").ToString & "')"                 ' �����ʖ���

                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_ATTR_NAME)     ' �����ʖ���
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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����ʖ��̃}�X�^�[�ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeAttrName(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        '''' �ʐݒ�
        With dgv
            .RowCount = 0

            .AllowUserToAddRows = True                                      ' ���[�U�[ �s�ǉ�

            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

        End With

        '' ���`
        ' �����ʕ��шʒu
        dgvColSetText(dgv, "�����ʕ��шʒu" & vbNewLine & "(0�`255)", 120, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ������EdasysID
        dgvColSetText(dgv, "������EdasysID" & vbNewLine & "(0�ȏ�)", 120, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' �����ʖ���
        dgvColSetText(dgv, "�����ʖ���", 520, False, False, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�̍ŏI�s�ǉ�������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvAttrName_UserAddedRow(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewRowEventArgs) Handles dgvAttrName.UserAddedRow
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �����ʕ��шʒu�A�ԕ\��
        If dgv.RowCount <= 2 Then           ' �󔒂����̂Ƃ���(�ŏ���)1�s�ǉ����̏��� 
            ' �P�s�ڒǉ����̏���
            dgv(EM_ATTRNAME_GRID_ID.emTxt_Enum, dgv.RowCount - 2).Value = "1"
        Else
            ' �Q�s�ڈȍ~�ǉ����̏���
            dgv(EM_ATTRNAME_GRID_ID.emTxt_Enum, dgv.RowCount - 2).Value = CInt(dgv(EM_ATTRNAME_GRID_ID.emTxt_Enum, dgv.RowCount - 3).Value) + 1
        End If

    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

End Class