'*********************************************************************************
' �t�B���^�ݒ�
'	[Ver]
'		Ver.01    2017/01/10  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt

Public Class frmEpcFilter

    Const SIGMA As Double = 0.01

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u���i���d����\�j
    'Private m_DSCycle As New DataSet                                ' ��ڑ��^�e�[�u���i�����p����\�j
    Private m_AdapterFilter As New SqlClient.SqlDataAdapter         ' �A�_�v�^�[
    Private m_DsFilter As New DataSet                               ' ��ڑ��^�e�[�u��PARAM_FILTER

    Private m_nSelectIdx As Integer = -1                            ' �ԍ��I���L�[

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
    Private Sub frmEpcFilter_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�t�B���^�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
        'm_DSCycle.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If
        m_DsFilter.Dispose()
        If Not m_AdapterFilter Is Nothing Then
            m_AdapterFilter.Dispose()
            m_AdapterFilter = Nothing
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
    Private Sub frmEpcFilter_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                 ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                             ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcFilter         ' ��ʏ����e�ݒ�Ăяo��

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        dgvInitializeCoef(dgvCoef)
        MyBase.DataGridViewSelectionCancel(dgvCoef)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)
            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        Else

            cmbId.DataSource = Nothing
            cmbSizeX.DataSource = Nothing

            ComboSet_Size()

            ComboSet_ID()

        Data_Disp()

            HandlerEnable(True)

            ' �X�V�{�^���̗L��/����
            Call UpdateEnabled(g_bPassFlg)

        End If

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        MyBase.DataGridViewSelectionRowColorChange(dgvCoef)

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
        strSQL = "SELECT * "
        strSQL &= " FROM " & DB_PARAM_FILTER
        strSQL &= " ORDER BY ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsFilter, m_AdapterFilter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
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


        '' SQL�����s�i�����p����\�ݒ�j
        'If 0 > MyBase.Get_SelectData(String.Format(strSQL, DB_PARAM_CYCLE_THRESHOLD), m_DSCycle, m_Adapter, strErrMsg) Then
        '    gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_CYCLE_THRESHOLD & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
        '    Return False
        'End If

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
                modMain.Send_Main(CH_PARAM_FILTER)
            End If

            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try
    End Function

    ''' <summary>
    ''' ���͒l�`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean

        Dim strWk As String = ""
        Dim strMsg As String = ""
        Dim dWk As Double

        If 0 > m_nSelectIdx Then
            Return False
        End If

        ' �t�B���^�[ID
        Dim nId As Integer
        If Not GetCmbIdValue(m_nSelectIdx, nId) Then
            Return False
        End If
        If tClass_ImputCheck.CheckImputMsg_Int(nId.ToString, 1, 999999, True, "�t�B���^ID") <> 0 Then Return False

        '����
        strWk = txtName.Text.Trim
        If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 256, "����") <> 0 Then Return False
        txtName.Text = strWk

        '���̂̏d���`�F�b�N
        Dim data() As DataRow = m_DsFilter.Tables(0).Select()
        For ii As Integer = 0 To data.Length - 1
            If CInt(data(ii).Item("id")) = nId Then
                Continue For
            End If
            '���̂������Ȃ烁�b�Z�[�W���o���ďI��
            If CStr(txtName.Text) = CStr(data(ii).Item("name")) Then
                strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
                strMsg &= "�������͓̂o�^�ł��܂���B" & vbNewLine
                strMsg &= "- - - ID." & CStr(data(ii).Item("id")) & " �Əd��"
                MsgBox(strMsg)
                Return False
            End If
        Next

        '�J�[�l���T�C�Y
        If cmbSizeX.Text <> lblSizeY.Text Then
            strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
            strMsg &= "�J�[�l���T�C�Y��X��Y�ňقȂ�܂��B"
            MsgBox(strMsg)
            Return False
        End If

        '���d�l
        For row As Integer = 0 To dgvCoef.RowCount - 1
            For col As Integer = 0 To dgvCoef.ColumnCount - 1
                If dgvCoef.Item(col, row).ReadOnly Then
                    Continue For
                End If
                strWk = CStr(dgvCoef.Item(col, row).Value)
                If Not Double.TryParse(strWk, dWk) Then
                    strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
                    strMsg &= "�W���ɂ͐��l����͂��Ă��������B" & vbNewLine
                    strMsg &= "- - - " & String.Format("�W��{0}{1}", row + 1, col + 1)
                    MsgBox(strMsg)
                    Return False
                End If
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, -255, 255, False, 16, String.Format("�W��{0}{1}", row + 1, col + 1)) <> 0 Then Return False
            Next
        Next

        '���K�W��
        strWk = CStr(txtNormalizeCoef.Text)
        If Not Double.TryParse(strWk, dWk) Then
            strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
            strMsg &= "���K�W���ɂ͐��l����͂��Ă��������B"
            MsgBox(strMsg)
            Return False
        End If
        If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, -255, 255, False, 0, "���K���W��") <> 0 Then Return False
        If CDbl(txtNormalizeCoef.Text.Trim) < SIGMA And CDbl(txtNormalizeCoef.Text.Trim) > (SIGMA * -1) Then
            strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
            strMsg &= "���K�W���ɂ͐��l����͂��Ă��������B(0�ȊO)"
            MsgBox(strMsg)
            Return False
        End If

        ' 2019/10/11 ���K���W��������͉\�Ƃ���(�P�x�l��臒l0-255�𒴂��邽��)
        'txtNormalizeCoef.Text = strWk
        'If CDbl(txtNormalizeCoef.Text) <> CalcNomalizeCoef() Then
        '    strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
        '    strMsg &= "���K�W��������������܂���B" & vbNewLine
        '    strMsg &= "- - - " & CStr(CalcNomalizeCoef())
        '    MsgBox(strMsg)
        '    Return False
        'End If

        '�I�t�Z�b�g
        strWk = CStr(txtOffset.Text)
        If Not Double.TryParse(strWk, dWk) Then
            strMsg = "�y �t�B���^�ݒ� �z" & vbNewLine
            strMsg &= "�I�t�Z�b�g�ɂ͐��l����͂��Ă��������B"
            MsgBox(strMsg)
            Return False
        End If
        If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 32896, False, 0, "�I�t�Z�b�g") <> 0 Then Return False

        Return True

    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_Get() As Boolean

        Dim bRet As Boolean = False


        Dim nId As Integer
        If Not GetCmbIdValue(m_nSelectIdx, nId) Then
            Return bRet
        End If

        Dim strWhere As String = String.Format("id='{0}'", nId)                          ' WHERE��ɑ����̏�������
        Dim data() As DataRow = m_DsFilter.Tables(0).Select(strWhere)
        Dim dataSet As DataRow = m_DsFilter.Tables(0).NewRow

        If 0 < data.Length Then
            dataSet = data(0)
                    End If

        Try
            ' �t�B���^�[ID
            dataSet.Item("id") = nId
            ' ����
            dataSet.Item("name") = txtName.Text.Trim
            ' �J�[�l���T�C�YX
            dataSet.Item("SizeX") = CInt(cmbSizeX.SelectedValue)
            ' �J�[�l���T�C�YY(�J�[�l���T�C�YX�Ɠ���)
            dataSet.Item("SizeY") = dataSet.Item("SizeX")
            ' ���d�l
            For row As Integer = 0 To dgvCoef.RowCount - 1
                For col As Integer = 0 To dgvCoef.ColumnCount - 1
                    Dim strName As String = String.Format("Coef{0}{1}", row + 1, col + 1)
                    If dgvCoef.Item(col, row).ReadOnly Then
                        dataSet.Item(strName) = 0
                    Else
                        dataSet.Item(strName) = dgvCoef.Item(col, row).Value
                    End If
                Next
            Next
            ' ���K���W��
            dataSet.Item("NormalizeCoef") = CDbl(txtNormalizeCoef.Text.Trim)
            ' �I�t�Z�b�g
            dataSet.Item("Offset") = CDbl(txtOffset.Text.Trim)

            bRet = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " �f�[�^�Z�b�g�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' ��ڑ��^�e�[�u���ɐV�K�ǉ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Add()

        Dim dataNew As DataRow

        Try

            Dim data() As DataRow = m_DsFilter.Tables(0).Select()

            'ID���擾
            Dim nNewId As Integer = 1   'ID�����l
            Dim nIns As Integer = 0
            'ID���������ɂȂ�Ȃ��悤�ɁA�S�Ă�ID����������B
            For ii As Integer = 0 To data.Length - 1
                If nNewId = CInt(data(ii).Item("id")) Then
                    '����ID������������A����ID��1�����Z����ID�ɂ���
                    nNewId = CInt(data(ii).Item("id")) + 1
                    nIns = ii + 1
                End If
            Next

            dataNew = m_DsFilter.Tables(0).NewRow
            dataNew.Item("id") = nNewId
            dataNew.Item("name") = String.Empty
            dataNew.Item("SizeX") = 0
            dataNew.Item("SizeY") = 0
            dataNew.Item("NormalizeCoef") = 0
            dataNew.Item("Offset") = 0
            For row As Integer = 0 To dgvCoef.RowCount - 1
                For col As Integer = 0 To dgvCoef.ColumnCount - 1
                    Dim strName As String = String.Format("Coef{0}{1}", row + 1, col + 1)
                    dataNew.Item(strName) = 0
                Next
            Next

            m_DsFilter.Tables(0).Rows.InsertAt(dataNew, nIns)
            m_nSelectIdx = nIns


        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " �t�B���^�[��� ��ڑ��^�e�[�u�� �f�[�^�Z�b�g�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' ��ڑ��^�e�[�u������f�[�^���폜
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Del(ByVal nId As Integer)

        Dim strWhere As String = String.Format("id='{0}'", nId)     ' WHERE��ɑ����̏�������

        Try

            Dim data() As DataRow = m_DsFilter.Tables(0).Select(strWhere)                ' �I�𒆎�L�[�l�̍s�����o��

            If 1 > data.Length Then
                '�폜����f�[�^��������Ȃ�
                Exit Try
            End If

            '�f�[�^���폜
            m_DsFilter.Tables(0).Rows.Remove(data(0))

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " �t�B���^�[��� ��ڑ��^�e�[�u�� �f�[�^�Z�b�g�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' �폜�Ώ�ID��臒l��ɓo�^����Ă��邩�`�F�b�N���s��
    ''' </summary>
    ''' <param name="nId">�t�B���^�[ID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckUseID(ByVal nId As Integer) As Boolean
        Dim strWhere As String
        Dim strMsg As String    '�G���[���b�Z�[�W
        Dim data() As DataRow                                        ' �f�[�^�s

        strWhere = String.Format("FilterId='{0}'", nId)             ' WHERE��ɑ����̏�������
        data = m_DS.Tables(0).Select(strWhere)
        If 0 < data.Length Then
            '�o�^�ς݂̂��߈ُ�Ƃ���
            strMsg = "�t�B���^ID= " & CStr(nId) & " �͍폜�ł��܂���" & vbNewLine
            strMsg &= "���d����\臒l�ݒ�ɂĎg�p���Ă��܂��B"
            MsgBox(strMsg)
            Return False
        End If
        'data = m_DSCycle.Tables(0).Select(strWhere)
        'If 0 < data.Length Then
        '    '�o�^�ς݂̂��߈ُ�Ƃ���
        '    strMsg = "�t�B���^ID= " & CStr(nId) & " �͍폜�ł��܂���" & vbNewLine
        '    strMsg &= "�����p����\臒l�ݒ�ɂĎg�p���Ă��܂��B"
        '    MsgBox(strMsg)
        '    Return False
        'End If

        Return True
    End Function


    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        HandlerEnable(False)

        Try

            '���͒l�`�F�b�N
            If Not Data_Check() Then Exit Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '��ʂ̃f�[�^���擾
            If Not Data_Get() Then
                Exit Try
            End If

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

            '�R���{�{�b�N�X�ݒ�
            ComboSet_ID()

            '�f�[�^�\��
            Data_Disp()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If

            HandlerEnable(True)
        End Try

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

            strSql = "DELETE FROM " & DB_PARAM_FILTER
            'SQL���̔��s
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�f�[�^��o�^ 
            For ii As Integer = 0 To m_DsFilter.Tables(0).Rows.Count - 1
                With m_DsFilter.Tables(0).Rows(ii)

                    '�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_FILTER & " VALUES ("

                    strSql &= .Item("id").ToString & ","                        '�t�B���^ID
                    strSql &= "'" & .Item("name").ToString & "',"               '����
                    strSql &= .Item("SizeX").ToString & ","                     'X�T�C�Y
                    strSql &= .Item("SizeY").ToString & ","                     'Y�T�C�Y
                    strSql &= .Item("Offset").ToString & ","                    '�I�t�Z�b�g
                    strSql &= .Item("NormalizeCoef").ToString & ","             '�t�B���^���K���W��

                    strSql &= .Item("Coef11").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef12").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef13").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef14").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef15").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef16").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef17").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef21").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef22").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef23").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef24").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef25").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef26").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef27").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef31").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef32").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef33").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef34").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef35").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef36").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef37").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef41").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef42").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef43").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef44").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef45").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef46").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef47").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef51").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef52").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef53").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef54").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef55").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef56").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef57").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef61").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef62").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef63").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef64").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef65").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef66").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef67").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef71").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef72").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef73").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef74").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef75").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef76").ToString & ","                    '�t�B���^�W��
                    strSql &= .Item("Coef77").ToString                          '�t�B���^�W��

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
    ''' �R���{�{�b�N�X�ݒ�(�J�[�l���T�C�YX)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ComboSet_Size()

        Try

            cmbSizeX.Items.Clear()

            '�f�[�^�e�[�u���ɕ\�����A�ݒ�l���Z�b�g
            Dim dt As New DataTable()
            Dim row As DataRow
            dt.Columns.Add("NAME", GetType(String))
            dt.Columns.Add("VALUE", GetType(Integer))
            row = dt.NewRow()
            row("NAME") = "�t�B���^�[�Ȃ�"
            row("VALUE") = 0
            dt.Rows.Add(row)
            row = dt.NewRow()
            row("NAME") = "3"
            row("VALUE") = 3
            dt.Rows.Add(row)
            row = dt.NewRow()
            row("NAME") = "5"
            row("VALUE") = 5
            dt.Rows.Add(row)
            row = dt.NewRow()
            row("NAME") = "7"
            row("VALUE") = 7
            dt.Rows.Add(row)
            dt.AcceptChanges()
            '�f�[�^�\�[�X�Ƀe�[�u����o�^
            cmbSizeX.DataSource = dt
            cmbSizeX.DisplayMember = "NAME"
            cmbSizeX.ValueMember = "VALUE"
            cmbSizeX.SelectedIndex = 0

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�ݒ�(ID)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ComboSet_ID()

        Dim data() As DataRow = m_DsFilter.Tables(0).Select()

        Try

            Dim dt As DataTable

            If cmbId.DataSource Is Nothing Then
                ' �f�[�^�e�[�u����V�K�ɍ쐬
                dt = New DataTable()
                dt.Columns.Add("NAME", GetType(String))
                dt.Columns.Add("VALUE", GetType(Integer))
            Else
                ' �����̃f�[�^�e�[�u�����ė��p
                dt = CType(cmbId.DataSource, DataTable)
                dt.Clear()
                cmbId.DataSource = Nothing
            End If

            cmbId.Items.Clear()

            For ii As Integer = 0 To data.Length - 1
                Dim row As DataRow = dt.NewRow()
                row("NAME") = CStr(data(ii).Item("id"))
                row("VALUE") = CInt(data(ii).Item("id"))
                dt.Rows.Add(row)
            Next
            dt.AcceptChanges()
            cmbId.DataSource = dt
            cmbId.DisplayMember = "NAME"
            cmbId.ValueMember = "VALUE"

            If 0 < data.Length Then
                If 0 > m_nSelectIdx Then
                    m_nSelectIdx = 0
                End If
                If cmbId.Items.Count <= m_nSelectIdx Then
                    m_nSelectIdx = cmbId.Items.Count - 1
                End If
            Else
                m_nSelectIdx = -1
            End If

            cmbId.SelectedIndex = m_nSelectIdx

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean

        Dim bRet As Boolean = False

        Try

            '�\����������
            With dgvCoef
                For row As Integer = 0 To .RowCount - 1
                    For col As Integer = 0 To .ColumnCount - 1
                        .Item(col, row).Value = String.Empty
                        .Item(col, row).Style.BackColor = Color.White
                        .Item(col, row).ReadOnly = True
                    Next
                Next
            End With
            txtName.Text = String.Empty
            lblSizeY.Text = String.Empty
            lblSizeY.BackColor = Color.White
            txtNormalizeCoef.Text = String.Empty
            txtNormalizeCoef.BackColor = Color.White
            txtOffset.Text = String.Empty
            lblSizeX.Visible = False

            If 0 > cmbId.SelectedIndex Then
                ' �t�B���^�[��񂪈ꌏ���Ȃ��ꍇ�ANo.�y�сA�J�[�l���T�C�Y�̑I���A�폜�{�^���𖳌��Ƃ���
                cmbId.Enabled = False
                cmbSizeX.Enabled = False
                cmbSizeX.SelectedIndex = -1
                btnDel.Enabled = False
                ' �f�[�^�̕\�����s��Ȃ�
                Exit Try
            End If

            ' �t�B���^�[��񂪗L��ꍇ�ANo.�y�сA�J�[�l���T�C�Y�̑I���A�폜�{�^����L���Ƃ���
            cmbId.Enabled = True
            cmbSizeX.Enabled = True
            If g_bPassFlg Then
                btnDel.Enabled = True
            End If

            Dim strNum As String = CStr(cmbId.SelectedValue)
            Dim strWhere As String = String.Format("id='{0}'", strNum)                  ' WHERE��ɑ����̏�������
            Dim data() As DataRow = m_DsFilter.Tables(0).Select(strWhere)


            '�擾�f�[�^�������ꍇ
            If 1 > data.Length Then Exit Try

            ' ��ʂɕ\��
            ' 
            txtName.Text = CStr(data(0).Item("name"))

            Dim nX As Integer = CInt(data(0).Item("SizeX"))
            If CheckCmbSizeValue(nX) Then
                cmbSizeX.SelectedValue = nX
            Else
                ' �J�[�l���T�C�Y(X)���s���̏ꍇ
                cmbSizeX.SelectedValue = 0      ' ���d�l�̓t�B���^�[�Ȃ��Ƃ��Đݒ肵�Ă���
                lblSizeX.Text = nX.ToString
                lblSizeX.BackColor = Color.Red
                lblSizeX.Visible = True
            End If
            Dim nY As Integer = CInt(data(0).Item("SizeY"))
            If CheckCmbSizeValue(nY) AndAlso nX = nY Then
                ' �J�[�l���T�C�Y���������A���AX�Ɠ����l�̏ꍇ�́AComboBox�̕\��Text�Ɠ����ɂ���
                lblSizeY.BackColor = Color.White
                lblSizeY.Text = cmbSizeX.Text
            Else
                ' ��L�ȊO�́A�ُ�\��
                lblSizeY.Text = nY.ToString
                lblSizeY.BackColor = Color.Red
            End If

            txtNormalizeCoef.Text = CStr(data(0).Item("NormalizeCoef"))
            txtOffset.Text = CStr(data(0).Item("Offset"))

            For row As Integer = 0 To dgvCoef.RowCount - 1
                For col As Integer = 0 To dgvCoef.ColumnCount - 1
                    Dim strItem As String = String.Format("Coef{0}{1}", row + 1, col + 1)
                    dgvCoef.Item(col, row).Value = data(0).Item(strItem)
                    dgvCoef.Item(col, row).ReadOnly = False
                    dgvCoef.Item(col, row).Style.BackColor = Color.White
                Next
            Next

            ChangeCoefRange()

            If Math.Abs(CalcNomalizeCoef() - CDbl(txtNormalizeCoef.Text.Trim)) > SIGMA Then
                ' 2019/10/11 ���K���W��������͉\�Ƃ���(�P�x�l��臒l0-255�𒴂��邽��) Red��LightPink
                '�\�����Ă��鐳�K���W�����v�Z�l�ƈقȂ�
                txtNormalizeCoef.BackColor = Color.LightPink
            Else

            End If

            bRet = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�\���G���[ [" & ex.Message & "] " & Me.Text)

        Finally

        End Try

        Return bRet

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As tCnt_DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                                         ' �Z���G���[�̕\��
            .ShowCellToolTips = False                                       ' �c�[���q���g�̕\��
            .ShowEditingIcon = False                                        ' �ҏW���̃Z���̖���
            .ShowRowErrors = False                                          ' �G���[�s�̖���
            .ScrollBars = ScrollBars.None                                   ' �X�N���[���o�[�̕\��
            .ColumnHeadersHeight = 28                                       ' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersVisible = True                                       ' �s�w�b�_�\��
            .RowHeadersWidth = 70
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y���������Ȃ�
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y���������Ȃ�

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            ' �s�w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .RowHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowHeadersDefaultCellStyle.ForeColor = Color.Black
            .RowHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

        End With

    End Sub

    ''' <summary>
    '''���d�l���dgv������
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeCoef(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        ' �����`
        DgvInit(dgv)
        dgv.RowHeadersWidth = 40

        With dgv
            ' ���`
            For col As Integer = 1 To 7
                Using clm As New DataGridViewTextBoxColumn
                    clm.HeaderText = col.ToString
                    clm.Width = 100
                    clm.Frozen = True
                    clm.ReadOnly = False
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                    .Columns.Add(clm)
                End Using
            Next

            '�s��`
            .RowCount = 7
            For row As Integer = 1 To .RowCount
                .Rows(row - 1).HeaderCell.Value = row.ToString
                .Rows(row - 1).Height = 28
            Next

        End With

    End Sub

    ''' <summary>
    ''' �J�[�l���T�C�Y�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSizeX_SelectedValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' �J�[�l���T�C�YX�̃C�x���g�ł͂Ȃ�
        If Not cmbSizeX.Equals(CType(sender, ComboBox)) Then Exit Sub

        '�R���{�{�b�N�X�̐ݒ�ɂ��A�ُ탉�x���͕s���ɂ���
        lblSizeX.Visible = False

        ' �J�[�l���T�C�YY�̕\����ύX
        lblSizeY.Text = cmbSizeX.Text
        lblSizeY.BackColor = Color.White

        HandlerEnable(False)

        ChangeCoefRange()

        HandlerEnable(True)

        ' ���K���W�����Čv�Z�����l�ɕύX
        txtNormalizeCoef.Text = CalcNomalizeCoef.ToString
        txtNormalizeCoef.BackColor = Color.White

    End Sub

    ''' <summary>
    ''' ���d�l�̈�ύX
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeCoefRange()

        Dim nSize As Integer = CInt(cmbSizeX.SelectedValue)
        For row As Integer = 0 To dgvCoef.RowCount - 1
            For col As Integer = 0 To dgvCoef.ColumnCount - 1
                With dgvCoef.Item(col, row)
                    If nSize < row + 1 Or nSize < col + 1 Then
                        '�͈͊O�̉��d�l�͓��͕s��
                        .Value = 0
                        .ReadOnly = True
                        .Style.BackColor = Color.DarkGray
            Else
                        .ReadOnly = False
                        .Style.BackColor = Color.White
            End If
                End With
        Next
        Next
        dgvCoef.CurrentCell = Nothing

    End Sub

    ''' <summary>
    ''' ���K���W�����v�Z
    ''' </summary>
    ''' <returns>���K���W��</returns>
    ''' <remarks></remarks>
    Private Function CalcNomalizeCoef() As Double

        Dim dAns As Double = 0
        Dim bCoef As Boolean = False

        For row As Integer = 0 To dgvCoef.RowCount - 1
            For col As Integer = 0 To dgvCoef.ColumnCount - 1
                With dgvCoef.Item(col, row)
                    If .ReadOnly Then
                        Continue For
                    End If
                    Dim d As Double
                    If Not Double.TryParse(CStr(.Value), d) Then
                        Continue For
                    End If
                    dAns = dAns + d
                    bCoef = True
                End With
            Next
        Next

        If bCoef AndAlso Math.Abs(dAns) < SIGMA Then
            '���d�l���v��0�̏ꍇ�́A���K���W����1�Ƃ���B
            Return 1
        End If

        Return dAns

    End Function

    ''' <summary>
    ''' ���d�l�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvCoef_CellValueChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)

        If Not dgvCoef.Equals(CType(sender, tCnt_DataGridView)) Then Exit Sub

        Dim dCoef As Double = CalcNomalizeCoef()

        txtNormalizeCoef.Text = dCoef.ToString
        txtNormalizeCoef.BackColor = Color.White
        If Math.Abs(dCoef - 1) > SIGMA Then
            '���K���W����1�ȊO�̂Ƃ��̓I�t�Z�b�g"0"�������\��
            txtOffset.Text = "0"
        End If
    End Sub


    ''' <summary>
    ''' �C�x���g�n���h���ݒ�
    ''' </summary>
    ''' <param name="bEnable"></param>
    ''' <remarks></remarks>
    Private Sub HandlerEnable(ByVal bEnable As Boolean)
        If bEnable Then
            AddHandler cmbSizeX.SelectedValueChanged, AddressOf cmbSizeX_SelectedValueChanged
            AddHandler cmbId.SelectedValueChanged, AddressOf cmbId_SelectedValueChanged
            AddHandler dgvCoef.CellValueChanged, AddressOf dgvCoef_CellValueChanged
        Else
            RemoveHandler cmbSizeX.SelectedValueChanged, AddressOf cmbSizeX_SelectedValueChanged
            RemoveHandler cmbId.SelectedValueChanged, AddressOf cmbId_SelectedValueChanged
            RemoveHandler dgvCoef.CellValueChanged, AddressOf dgvCoef_CellValueChanged
        End If
    End Sub

    ''' <summary>
    ''' �R���{BOX(ID)�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbId_SelectedValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂� ���ɖ߂����� �̎��̑΍�)
        If m_nSelectIdx = cmbId.SelectedIndex Then Exit Sub

        HandlerEnable(False)

        Try
            If 0 <= m_nSelectIdx Then
                ' ���͒l�`�F�b�N
                If Not Data_Check() Then
                    ' ���ُ͈펞�́A�I���ʒu���߂�
                    cmbId.SelectedIndex = m_nSelectIdx
                    Exit Try
                End If
                ' ��ʃf�[�^��荞��
                If Not Data_Get() Then
                    ' ��荞�ُ݈펞�́A�I���ʒu���߂�
                    cmbId.SelectedIndex = m_nSelectIdx
                    Exit Try
                End If
            End If

            '�\��
            m_nSelectIdx = cmbId.SelectedIndex
            Data_Disp()

        Catch ex As Exception
        Finally
            HandlerEnable(True)
        End Try

    End Sub


    ''' <summary>
    ''' �V�K�ǉ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRollAdd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAdd.Click


        HandlerEnable(False)

        Try
            If 0 <= m_nSelectIdx Then
                ' �V�K�ǉ�����O�ɁA���ݕ\�����Ă���f�[�^����荞��
                If Not Data_Check() Then
                    Exit Try
                End If
                If Not Data_Get() Then
                    Exit Try
                End If
            End If

            Data_Add()

            ComboSet_ID()

            Data_Disp()

        Catch ex As Exception
        Finally
            HandlerEnable(True)
        End Try

    End Sub

    ''' <summary>
    ''' �폜
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRolldel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDel.Click

        Dim nId As Integer = CInt(cmbId.SelectedValue)

        If Not CheckUseID(nId) Then
            '���������폜�ł��Ȃ�
            Exit Sub
                    End If

        ' �폜�m�F
        If MsgBox("�\�����̃t�B���^�[�����폜���܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�폜�m�F") = MsgBoxResult.No Then Exit Sub

        HandlerEnable(False)

        Try
            ''�t�B���^�[���̍폜
            Data_Del(nId)

            '�R���{�{�b�N�X�ݒ�O�ɑI�����[������O�ɐ؂�ւ��Ă���
            m_nSelectIdx = m_nSelectIdx - 1

            '�R���{�{�b�N�X�ݒ�
            ComboSet_ID()

            '�f�[�^�\��
            Call Data_Disp()

        Catch ex As Exception
        Finally
            HandlerEnable(True)
        End Try

    End Sub

    ''' <summary>
    ''' �J�[�l���T�C�Y�̐������m�F
    ''' </summary>
    ''' <param name="nSize">�J�[�l���T�C�Y</param>
    ''' <returns>TRUE:OK�AFALSE:NG</returns>
    ''' <remarks></remarks>
    Private Function CheckCmbSizeValue(ByVal nSize As Integer) As Boolean


        ' �R���{�{�b�N�X�ɓo�^����Ă���f�[�^�e�[�u�����m�F����
        If cmbSizeX.DataSource Is Nothing Then
            ' �f�[�^�\�[�X�Ȃ�
            Return False
        End If

        Dim dt As DataTable = CType(cmbSizeX.DataSource, DataTable)
        For ii As Integer = 0 To dt.Rows.Count - 1
            If CInt(dt.Rows(ii).Item("VALUE")) = nSize Then
                '�T�C�Y�l���o�^����Ă���
                Return True
            End If
        Next

        ' �T�C�Y��������Ȃ�
        Return False
    End Function

    ''' <summary>
    ''' �t�B���^�[ID�擾
    ''' </summary>
    ''' <param name="nIdx">�I���C���f�b�N�X</param>
    ''' <param name="nVal">�t�B���^�[ID</param>
    ''' <returns>TRUE:OK�AFALSE:NG</returns>
    ''' <remarks></remarks>
    Private Function GetCmbIdValue(ByVal nIdx As Integer, ByRef nVal As Integer) As Boolean

        ' �R���{�{�b�N�X�ɓo�^����Ă���f�[�^�e�[�u�����m�F����
        If cmbId.DataSource Is Nothing Then
            ' �f�[�^�\�[�X�Ȃ�
            Return False
        End If

        Dim dt As DataTable = CType(cmbId.DataSource, DataTable)
        If 0 > nIdx Or dt.Rows.Count - 1 < nIdx Then
            Return False
        End If

        nVal = CInt(dt.Rows(nIdx).Item("VALUE"))
        Return True

    End Function

End Class
