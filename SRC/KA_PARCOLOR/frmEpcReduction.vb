'*********************************************************************************
' �k�����ݒ�
'	[Ver]
'		Ver.01    2017/01/10  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcReduction
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum em_DataColumn
        emTxt_Order = 0                 ' �e�L�X�g�{�b�N�X  No
        emTxt_ID                        ' �e�L�X�g�{�b�N�X  �k����ID
        emTxt_Name                      ' �e�L�X�g�{�b�N�X  ����
        emTxt_MeanX                     ' �e�L�X�g�{�b�N�X  ���σT�C�YX
        emTxt_MeanY                     ' �e�L�X�g�{�b�N�X  ���σT�C�YY
        emTxt_RatioX                    ' �e�L�X�g�{�b�N�X  �k����X
        emTxt_RatioY                    ' �e�L�X�g�{�b�N�X  �k����Y
        iInsButton                      ' �}���{�^��
        iDelButton                      ' �폜�{�^��
    End Enum

    '�폜�Ώۂ�ID
    Private Structure TypeStruct
        Dim nID As Integer        ' �폜�Ώۂ�ID

        Public Sub initialize()
            nID = 0
        End Sub

    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u���i���d����\�j
    Private m_AdapterReduction As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[
    Private m_DsReduction As New DataSet                            ' ��ڑ��^�e�[�u��PARAM_REDUCTION
    Private mlst_DelID As New List(Of TypeStruct)                   ' �폜�Ώۂ�ID

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
    Private Sub frmEpcReduction_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�k�����ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If
        m_DsReduction.Dispose()
        If Not m_AdapterReduction Is Nothing Then
            m_AdapterReduction.Dispose()
            m_AdapterReduction = Nothing
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
    Private Sub frmEpcReduction_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcReduction          ' ��ʏ����e�ݒ�Ăяo��

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()
        End If

        'dgv������
        dgvInitialize(dgvReduction)
        dgvColunmDef(dgvReduction)

        ' ��ʂɒl��\��
        Data_Disp()

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        MyBase.DataGridViewSelectionRowColorChange(dgvReduction)

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub
    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL������
        strSQL = "SELECT id, "
        strSQL &= "name, MeanX, MeanY, RatioX, RatioY "
        strSQL &= " FROM " & DB_PARAM_REDUCTION
        strSQL &= " ORDER BY ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsReduction, m_AdapterReduction, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' <20221213> <�a��> <�ŏI�H�����@�\������1> --->>>
        '' SQL������
        'strSQL = "SELECT scode"
        'strSQL &= ",camangle"
        'strSQL &= ",id"
        'strSQL &= ",ReductionId"
        'strSQL &= ",FilterId"
        'strSQL &= ",name"

        'strSQL &= " FROM {0}"
        'strSQL &= " ORDER BY id ASC"

        '' SQL�����s�i���d����\�ݒ�j
        'If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_THRESHOLD), m_DS, m_Adapter, strErrMsg) Then
        '    gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
        '    Return False
        'End If

        '' SQL������
        strSQL = "SELECT scode"
        strSQL &= ",id_filterbin"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",name"

        strSQL &= " FROM {0}"
        strSQL &= " ORDER BY id_filterbin ASC"

        ' SQL�����s�i���d����\�ݒ�j
        If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_FILTERBIN), m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTERBIN & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If
        ' <20221213> <�a��> <�ŏI�H�����@�\������1> <<<---

        Return True

    End Function

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Try
            '�l��o�^
            If Not DB_DeleteUp() Then
                Return False
            Else
                modMain.Send_Main(CH_PARAM_REDUCTION)
            End If

            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try
    End Function

    ''' <summary>
    ''' dgv�Ƀf�[�^�s��}��
    ''' </summary>
    ''' <param name="nRowIndex">�s�̑}���ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvReduction�Ƀ��R�[�h��}��
        dgvReduction.Rows.Insert(nRowIndex)

        ' �f�[�^������
        dgvReduction(em_DataColumn.emTxt_ID, nRowIndex).Value = ""         ' �k����ID
        dgvReduction(em_DataColumn.emTxt_Name, nRowIndex).Value = ""       ' ����
        dgvReduction(em_DataColumn.emTxt_MeanX, nRowIndex).Value = ""      ' ���σT�C�YX
        dgvReduction(em_DataColumn.emTxt_MeanY, nRowIndex).Value = ""      ' ���σT�C�YY
        dgvReduction(em_DataColumn.emTxt_RatioX, nRowIndex).Value = ""     ' �k����X
        dgvReduction(em_DataColumn.emTxt_RatioY, nRowIndex).Value = ""     ' �k����Y

    End Sub

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        '�f�[�^��\��
        Data_DispdgvReduction(dgvReduction)
        '�C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexdgvReduction(dgvReduction)

        ' �s�̐F��ς��� �i�����\���ŐF���ς��΍�j
        dgvReduction.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' dgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispdgvReduction(ByVal dgv As DataGridView)
        Dim drowReduction() As DataRow = m_DsReduction.Tables(0).Select()         ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        '�s��������
        dgv.Rows.Clear()
        ' ��ʂɕ\��
        dgv.RowCount = MAX_REDUCTION                                       ' dgv�̃��R�[�h���͌Œ�

        ' ��ʂɕ\��
        Try

            For ii As Integer = 0 To drowReduction.Length - 1
                If ii < dgv.RowCount Then
                    dgv(em_DataColumn.emTxt_ID, ii).Value = drowReduction(ii).Item("id")              ' �k����ID
                    dgv(em_DataColumn.emTxt_Name, ii).Value = drowReduction(ii).Item("name")          ' ����
                    dgv(em_DataColumn.emTxt_MeanX, ii).Value = drowReduction(ii).Item("MeanX")        ' ���σT�C�YX
                    dgv(em_DataColumn.emTxt_MeanY, ii).Value = drowReduction(ii).Item("MeanY")        ' ���σT�C�YY
                    dgv(em_DataColumn.emTxt_RatioX, ii).Value = drowReduction(ii).Item("RatioX")      ' �k����X
                    dgv(em_DataColumn.emTxt_RatioY, ii).Value = drowReduction(ii).Item("RatioY")      ' �k����Y
                End If
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_PCINFO_DESC & " �\���G���[ [" & ex.Message & "] " & Me.Text)
        End Try

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(1, 0).Selected = True

        Return
    End Sub

    ''' <summary>
    ''' ���͒l�`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     'dgv�̃f�[�^�i�[�p
        Dim strMsg As String    '�d���p�̃G���[���b�Z�[�W

        'dgv�s���[�v
        For ii As Integer = 0 To dgvReduction.Rows.Count - 1
            'ID�������͂ł���΃`�F�b�N�s�v
            If Trim(CStr(dgvReduction(em_DataColumn.emTxt_ID, ii).Value)) = "" Then
                Continue For
            End If
            ' �k����ID
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_ID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, True, "�k�����ݒ� No=" & ii + 1 & " �k����ID") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_ID, ii).Value = strWk

            ' �d���`�F�b�N
            For jj As Integer = ii + 1 To dgvReduction.Rows.Count - 1
                ' ��r���ID�����l�Ȃ��r
                If IsNumeric(dgvReduction(em_DataColumn.emTxt_ID, jj).Value) Then
                    ' ID�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvReduction(em_DataColumn.emTxt_ID, ii).Value) = CInt(dgvReduction(em_DataColumn.emTxt_ID, jj).Value) Then
                        strMsg = "�y �k�����ݒ� �z" & vbNewLine
                        strMsg &= "����ID�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next

            '����
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 256, "�k�����ݒ� No=" & ii + 1 & " ����") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_Name, ii).Value = strWk

            '���̂̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvReduction.Rows.Count - 1
                '���̂������Ȃ烁�b�Z�[�W���o���ďI��
                If CStr(dgvReduction(em_DataColumn.emTxt_Name, ii).Value) = CStr(dgvReduction(em_DataColumn.emTxt_Name, jj).Value) Then
                    strMsg = "�y �k�����ݒ� �z" & vbNewLine
                    strMsg &= "�������͓̂o�^�ł��܂���B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                    MsgBox(strMsg)
                    Return False
                End If
            Next

            '���σT�C�YX
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_MeanX, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 512, False, "�k�����ݒ� No=" & ii + 1 & " ���σT�C�YX") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_MeanX, ii).Value = strWk

            '���σT�C�YY
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_MeanY, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 512, False, "�k�����ݒ� No=" & ii + 1 & " ���σT�C�YY") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_MeanY, ii).Value = strWk

            '�k����X
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_RatioX, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 256, False, "�k�����ݒ� No=" & ii + 1 & " �k����X") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_RatioX, ii).Value = strWk

            '�k����Y
            strWk = CStr(dgvReduction(em_DataColumn.emTxt_RatioY, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 256, False, "�k�����ݒ� No=" & ii + 1 & " �k����Y") <> 0 Then Return False
            dgvReduction(em_DataColumn.emTxt_RatioY, ii).Value = strWk

        Next ii

        Return True

    End Function

    ''' <summary>
    ''' ��ʂɕ\������ID��DB��ID���ׂāA�폜���ꂽID�����邩�`�F�b�N����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_CheckDelete()
        Dim drowReduction() As DataRow = m_DsReduction.Tables(0).Select()
        Dim bCheck As Boolean   '�`�F�b�N�t���O True�Ȃ�ID������
        Dim typ_Type As TypeStruct = Nothing

        ' ��ڑ��e�[�u���őS�s���[�v
        For ii As Integer = 0 To drowReduction.Length - 1
            ' �`�F�b�N�t���O�̏�����
            bCheck = False
            ' dgv�őS�s���[�v
            For kk As Integer = 0 To dgvReduction.Rows.Count - 1

                typ_Type.nID = CInt(drowReduction(ii).Item("id"))      ' �k����ID

                ' ID���󔒍s�ȊO�ŁA��r
                If CStr(dgvReduction(em_DataColumn.emTxt_ID, kk).Value) <> "" AndAlso _
                    CInt(dgvReduction(em_DataColumn.emTxt_ID, kk).Value) = CInt(typ_Type.nID) Then
                    ' ID����v�����Ƃ��A�`�F�b�NOK
                    bCheck = True
                    ' �폜�����L�[���܂��������ꍇ
                    If mlst_DelID.Contains(typ_Type) Then
                        '�폜���X�g���폜
                        mlst_DelID.Remove(typ_Type)
                    End If
                    Exit For
                End If
            Next kk

            ' �`�F�b�N�t���O��False�̂Ƃ������ꂽID�Ȃ̂ŁA�o�^����
            If bCheck = False Then
                typ_Type.initialize()
                ' �폜�Ώۂ�ID���i�[
                typ_Type.nID = CInt(drowReduction(ii).Item("id"))

                ' ���ɃL�[���������ꍇ
                If mlst_DelID.Contains(typ_Type) Then
                    '�폜���X�g���폜
                    mlst_DelID.Remove(typ_Type)
                End If
                mlst_DelID.Add(typ_Type)
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' �폜�Ώ�ID��臒l��ɓo�^����Ă��邩�`�F�b�N���s��
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DelIDSetUseCheck() As Boolean
        Dim strWhere As String
        Dim typ_Type As TypeStruct = Nothing
        Dim strMsg As String    '�G���[���b�Z�[�W
        For ii As Integer = 0 To mlst_DelID.Count - 1
            Dim drow() As DataRow                                        ' �f�[�^�s
            typ_Type = mlst_DelID(ii)
            strWhere = String.Format("ReductionId='{0}'", typ_Type.nID)          ' WHERE��ɑ����̏�������
            drow = m_DS.Tables(0).Select(strWhere)
            If 1 <= drow.Length Then
                '�o�^�ς݂̂��߈ُ�Ƃ���
                strMsg = "�k����ID= " & CStr(typ_Type.nID) & " �͍폜�ł��܂���" & vbNewLine
                strMsg &= "���d����\臒l�ݒ�ɂĎg�p���Ă��܂��B"
                MsgBox(strMsg)
                Return False
            End If
        Next ii
        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim drowReduction() As DataRow = m_DsReduction.Tables(0).Select()
        Dim nRowCount As Integer    '�폜�Ώۂ̃f�[�^��

        '���݉�ʂɕ\�����Ă���O�H���R�[�h�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        nRowCount = drowReduction.Length
        For ii As Integer = 1 To nRowCount
            m_DsReduction.Tables(0).Rows.Remove(drowReduction(nRowCount - ii))
        Next

        ' dgv�̒l���ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvReduction.RowCount - 1
            'ID ���󔒈ȊO�̍s�����擾
            If CStr(dgvReduction(em_DataColumn.emTxt_ID, ii).Value) = "" Then Continue For
            Dim drowNew As DataRow = m_DsReduction.Tables(0).NewRow()    '�V�K�s

            '�f�[�^���Z�b�g
            drowNew.Item("id") = CInt(dgvReduction(em_DataColumn.emTxt_ID, ii).Value)               ' �k����ID
            drowNew.Item("name") = CStr(dgvReduction(em_DataColumn.emTxt_Name, ii).Value)           ' ����
            drowNew.Item("MeanX") = CInt(dgvReduction(em_DataColumn.emTxt_MeanX, ii).Value)         ' ���σT�C�YX
            drowNew.Item("MeanY") = CInt(dgvReduction(em_DataColumn.emTxt_MeanY, ii).Value)         ' ���σT�C�YY
            drowNew.Item("RatioX") = CInt(dgvReduction(em_DataColumn.emTxt_RatioX, ii).Value)       ' �k����X
            drowNew.Item("RatioY") = CInt(dgvReduction(em_DataColumn.emTxt_RatioY, ii).Value)       ' �k����Y
            '�e�[�u���ɒǉ�
            m_DsReduction.Tables(0).Rows.InsertAt(drowNew, m_DsReduction.Tables(0).Rows.Count)
        Next
    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        '���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '��ʂ���폜���ꂽID���擾
            Data_CheckDelete()

            ' �폜�Ώ�ID��臒l��ɓo�^����Ă��邩�`�F�b�N���s��
            If Not DelIDSetUseCheck() Then
                '��ʂɒl��\�����Ȃ���
                Data_Disp()
                Exit Sub
            End If

            '��ʂ̃f�[�^���擾
            Data_Get()

            '��ʂ���폜���ꂽID���ēx�o�^���ꂽ�����擾
            Data_CheckDelete()

            '�z���g�ɓo�^
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            '���̃t�H�[���ŕK�v�ȃf�[�^���擾����
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       ' �I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �ݒ�̏�����
        Data_Init()

        '��ʂɒl��\��
        Data_Disp()

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[������
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        ''''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���
            .AllowUserToResizeColumns = True                            ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With


    End Sub

    ''' <summary>
    ''' dgv�̗��`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDef(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        With dgv
            ''=================================
            '' ���`
            ' No
            dgvColSetText(dgv, "No", 50, True, True, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
            ' �k����ID
            dgvColSetText(dgv, "�k����ID", 100, True, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

            ' ����
            dgvColSetText(dgv, "����", 260, True, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

            ' ���σT�C�YX
            dgvColSetText(dgv, "���σT�C�YX" & vbNewLine & "( 1 - 512 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' ���σT�C�YY
            dgvColSetText(dgv, "���σT�C�YY" & vbNewLine & "( 1 - 512 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' �k����X
            dgvColSetText(dgv, "�k����X" & vbNewLine & "( 1 - 256 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' �k����Y
            dgvColSetText(dgv, "�k����Y" & vbNewLine & "( 1 - 256 )", 110, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

            ' ��֑}��
            dgvColSetButton(dgv, "���" & vbNewLine & "�}��", 50, False, False, _
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                            New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�}��")

            ' �폜
            dgvColSetButton(dgv, "�폜", 50, False, False, _
                            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                            New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�폜")

        End With
    End Sub


    ''' <summary>
    ''' dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvReduction_A_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvReduction.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case em_DataColumn.iInsButton   '��֑}���{�^��
                InsBtnClick_Type(dgv, e.RowIndex)
            Case em_DataColumn.iDelButton   '�폜�{�^��
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' dgv�̑}���{�^�������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nRowIndex">���R�[�h��}������ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        ' �}���{�^�����g�p�\���`�F�b�N
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' �ŏI�s�͎g�p�s�Ȃ̂Ŕ�����
            Exit Sub
        End If

        '�ŏI�s�̃e�L�X�g�{�b�N�X�ɉ�������̃f�[�^�������Ă���ꍇ�͑}���s��
        If Trim(CStr(dgv(em_DataColumn.emTxt_ID, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_MeanX, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_MeanY, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_RatioX, dgv.Rows.Count - 1).Value)) <> "" Or _
        Trim(CStr(dgv(em_DataColumn.emTxt_RatioY, dgv.Rows.Count - 1).Value)) <> "" Then
            ' <<----------------
            MsgBox("�ŏI�s�Ƀf�[�^�����݂��܂��B")
            Exit Sub
        End If

        '�}�����邩���b�Z�[�W���o��
        If MsgBox("�}�����܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�����m�F") = MsgBoxResult.No Then Exit Sub

        ' dgv�Ƀ��R�[�h��}��
        InsertAt_TypeData(nRowIndex)

        ' dgv�̍ŏI�s���폜
        dgv.Rows.RemoveAt(dgvReduction.Rows.Count - 1)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexdgvReduction(dgv)
    End Sub

    ''' <summary>
    ''' dgv�̍폜�{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        '�폜���邩���b�Z�[�W���o��
        If MsgBox("�폜���܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�����m�F") = MsgBoxResult.No Then Exit Sub

        ' dgv�̍ŏI�s�Ƀ��R�[�h��}��
        InsertAt_TypeData(dgv.Rows.Count)

        ' dgv�̃��R�[�h���폜
        dgv.Rows.RemoveAt(nRowIndex)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexdgvReduction(dgv)
    End Sub

    ''' <summary>
    ''' dgvReduction�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexdgvReduction(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(em_DataColumn.emTxt_Order, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�

            ' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
            If ii = dgv.RowCount - 1 Then
                '�ŏI�s�͑}���{�^�����g�p�s��
                dgv(em_DataColumn.iInsButton, ii).Style.ForeColor = Color.Gray
            Else
                '����ȊO�͎g�p��
                dgv(em_DataColumn.iInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next
    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv
        dgv.CurrentCell.Selected = False
    End Sub


    '�Z�����I�����ꂽ�Ƃ��̏���
    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
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


    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_DeleteUp() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        Dim sqlReader As SqlClient.SqlDataReader = Nothing   'DataReader�I�u�W�F�N�g
        Dim strSql As String    'SQL��

        Try
            '�f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            strSql = "DELETE FROM " & DB_PARAM_REDUCTION
            'SQL���̔��s
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�f�[�^��o�^ 
            For ii As Integer = 0 To m_DsReduction.Tables(0).Rows.Count - 1
                With m_DsReduction.Tables(0).Rows(ii)

                    '�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_REDUCTION & " VALUES ("

                    strSql &= .Item("id").ToString & ","                        '�k����ID
                    strSql &= "'" & .Item("name").ToString & "',"               '����
                    strSql &= .Item("MeanX").ToString & ","                     '���σT�C�YX
                    strSql &= .Item("MeanY").ToString & ","                     '���σT�C�YY
                    strSql &= .Item("RatioX").ToString & ","                    '�k����X
                    strSql &= .Item("RatioY").ToString                          '�k����Y
                    strSql &= " )"

                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        '�G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �o�^�G���[ [" & ex.Message & "] " & Me.Text)
            '�G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            Return False

        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()

        End Try
    End Function

    ''' <summary>
    ''' �����o�ϐ��̏����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()
        ' �폜�Ώۂ�ID���X�g�̏�����
        mlst_DelID.Clear()
    End Sub

End Class
