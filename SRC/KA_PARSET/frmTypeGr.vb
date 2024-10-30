'*********************************************************************************
' �r��E�O���[�h�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmTypeGr
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �r��f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMNTYPE
        emNo = 0                            ' �e�L�X�g�{�b�N�X No
        emTxt_EdasysID                      ' �e�L�X�g�{�b�N�X EdasysID
        emTxt_Name                          ' �e�L�X�g�{�b�N�X �r�햼��
        emTxt_Score                         ' �e�L�X�g�{�b�N�X Score
        emTxt_Map                           ' �e�L�X�g�{�b�N�X �}�b�v�\������
        emTxt_FFSendName                    ' �e�L�X�g�{�b�N�X FF�`������
        emCmb_ProGroup                      ' �R���{�{�b�N�X   �v���R���O���[�v
        emChk_PeriodProcMask                ' �`�F�b�N�{�b�N�X PeriodProcMask
        emBtn_MapSymbolColor                ' �r�}�b�v�V���{���\���F
        emCmb_MapSynbol                     ' �R���{�{�b�N�X   �r�}�b�v�V���{��
        emCmb_Pictype1                      ' �R���{�{�b�N�X   �؏o�摜���1
        emCmb_Pictype2                      ' �R���{�{�b�N�X   �؏o�摜���2
        emInsButton                         ' �}���{�^��
        emDelButton                         ' �폜�{�^��
    End Enum

    ' �O���[�h�f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMNGRADE
        emNo = 0                            ' �e�L�X�g�{�b�N�X No
        emTxt_EdasysID                      ' �e�L�X�g�{�b�N�X EdasysID
        emTxt_Name                          ' �e�L�X�g�{�b�N�X �O���[�h����
        emTxt_Score                         ' �e�L�X�g�{�b�N�X Score
        emBtn_MapButton                     ' �{�^�� �T����ʕ\���F
        emCmb_BadGrade                      ' �R���{�{�b�N�X   ���׃O���[�h
        emCmb_FFSendName                    ' FF�`������
        emCmb_ProGroup                      ' �R���{�{�b�N�X   �v���R���O���[�v
        emBtn_MapSynbolColor                ' �{�^�� �r�}�b�v�\���F
        emTxt_NumLimit                      ' �e�L�X�g�{�b�N�X NumLimit
        emInsButton                         ' �}���{�^��
        emDelButton                         ' �폜�{�^��
    End Enum

    '�폜�Ώۂ̃O���[�hEdasysID�Ǝ��ɗD��x�̍����O���[�hEdasysID���i�[
    Private Structure GradeStruct
        Dim nEdasysID As Integer        ' �폜�Ώۂ̃O���[�hEdasysID
        Dim nNextEdasysID As Integer    ' ���ɗD��x�̍����O���[�hEdasysID    
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterType As New SqlClient.SqlDataAdapter       ' �A�_�v�^�[Type
    Private m_AdapterGrade As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[Grade
    Private m_AdapterGradeWk As New SqlClient.SqlDataAdapter    ' �A�_�v�^�[Grade �R���{�{�b�N�X�ݒ�p
    Private m_DsType As New DataSet                             ' ��ڑ��^�e�[�u��TYPE_NAME
    Private m_DsGrade As New DataSet                            ' ��ڑ��^�e�[�u��GRADE_NAME
    Private m_DsGradeWk As New DataSet                          ' ��ڑ��^�e�[�u��GRADE �R���{�{�b�N�X�ݒ�p

    Private m_nSendScode As Integer                             ' ���[���ő��M����Scode
    Private m_nDelTypeEdasysID(MAX_TYPE - 1) As Integer         ' �폜�Ώۂ��r��EdasysID
    Private mtyp_Grade(MAX_GRADE - 1) As GradeStruct            ' �폜�Ώۂ̃O���[�hEdasysID�Ǝ���Score�̍���EdasysID�i�[�p

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
    Private Sub frmTypeGr_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�r��E�O���[�h�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">�I��������߂�ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DsType.Dispose()
        If Not m_AdapterType Is Nothing Then
            m_AdapterType.Dispose()
            m_AdapterType = Nothing
        End If

        m_DsGrade.Dispose()
        If Not m_AdapterGrade Is Nothing Then
            m_AdapterGrade.Dispose()
            m_AdapterGrade = Nothing
        End If

        m_DsGradeWk.Dispose()
        If Not m_AdapterGradeWk Is Nothing Then
            m_AdapterGradeWk.Dispose()
            m_AdapterGradeWk = Nothing
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
    Private Sub frmTypeGr_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                 ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                             ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmTypeGr            ' �r��E�O���[�h�ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        '�O���[�hdgv������
        dgvInitializeDefGrade(dgvGrade)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '�R���{�{�b�N�X�pGRADE_NAME��ҏW
        GradeData_Init()
        '�r��dgv������ GRADE_NAME���g�p����ׁA�f�[�^�擾��Ɏ��{
        dgvInitializeDefType(dgvType)

        ' �f�[�^�̃��R�[�h����������
        Data_Init()

        ' ��ʂɒl��\��
        Data_Disp()

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvType.DataGridViewSelectionRowColorChange(g_ColorSelection)     ' �r��dgv
        dgvGrade.DataGridViewSelectionRowColorChange(g_ColorSelection)    ' �O���[�hdgv

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
        strSQL = "SELECT EdasysID"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", �D��x"
        strSQL &= ", �}�b�v�\������"
        strSQL &= ", FF�`������"
        strSQL &= ", �v���R���O���[�v"
        strSQL &= ", Score"
        strSQL &= ", PeriodProcMask"
        strSQL &= ", �r�}�b�v�V���{���\���F"
        strSQL &= ", �r�}�b�v�V���{��"
        strSQL &= ", �؏o�摜���1"
        strSQL &= ", �؏o�摜���2"

        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������
        strSQL = "SELECT EdasysID"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", �D��x"
        strSQL &= ", �}�b�v�\���F"
        strSQL &= ", ���׃O���[�h"
        strSQL &= ", FF�`������"
        strSQL &= ", �v���R���O���[�v"
        strSQL &= ", �r�}�b�v�V���{���\���F"
        strSQL &= ", Score"
        strSQL &= ", NumLimit"

        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'SQL�����s(�\���p)
        If 0 > MyBase.Get_SelectData(strSQL, m_DsGradeWk, m_AdapterGradeWk, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �r��}�X�^�[�A�O���[�h�}�X�^�[�AOUTPUT_PCODE�AOUTPUT_SCODE�e�[�u����ҏW����
    ''' </summary>
    ''' <returns>����:�ُ� �iTRUE:FALSE�j</returns>
    ''' <remarks>�r��}�X�^�[�A�O���[�h�}�X�^�[�e�[�u���̓f���[�g�C���T�[�g�̂��ߑS�폜�A
    '''          OUTPUT_PCODE�AOUTPUT_SCODE�͊֘A���R�[�h��ҏW</remarks>
    Private Function DB_Delete() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        Dim sqlReader As SqlClient.SqlDataReader = Nothing   'DataReader�I�u�W�F�N�g
        Dim strSql As String                                 'Sql��

        Dim strPcodeCol() As String
        Dim strScodeCol() As String
        Dim strPallOutCol() As String
        Dim strCommMasterCol() As String

        Try
            '''' �f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            'PCODE_OUTPUT�̗���擾
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_PCODE_OUTPUT

            '���R�[�h�Z�b�g
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strPcodeCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strPcodeCol.Length - 1
                strPcodeCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'SCODE_OUTPUT�̗���擾
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_SCODE_OUTPUT

            '���R�[�h�Z�b�g
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strScodeCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strScodeCol.Length - 1
                strScodeCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'PALL_OUTPUT�̗񖼂��擾
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_PALL_OUTPUT

            '���R�[�h�Z�b�g
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strPallOutCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strPallOutCol.Length - 1
                strPallOutCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'COMMON_MASTER�̗񖼂��擾
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_COMMON_MASTER

            '���R�[�h�Z�b�g
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strCommMasterCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strCommMasterCol.Length - 1
                strCommMasterCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' �폜�Ώۂ��r��EdasysID������Ƃ��ɁA�֘A����DB��ύX
            For ii As Integer = 0 To m_nDelTypeEdasysID.Length - 1
                ' EdasysID��1��菬������Ύ�
                If m_nDelTypeEdasysID(ii) < 1 Then Continue For

                ' �r��EdasysID�Ɋ֘A����DB��ύX
                ' PCODE_OUTPUT�̑Ώۂ��r��EdasysID������s���폜����
                'sql�쐬
                strSql = "DELETE FROM " & DB_PCODE_OUTPUT & " WHERE "
                strSql &= "�r��EdasysID = " & m_nDelTypeEdasysID(ii)
                'SQL���̔��s
                If 0 > tcls_DB.DMLExecute(strSql) Then
                    ' �G���[�Ȃ烍�[���o�b�N���ďI��
                    tcls_DB.TransactRollback()
                    Return False
                End If

                ' SCODE_OUTPUT���������ύX�Ώۂ�scode���擾����
                ' ���MScode�����łɑS���Ȃ猟�����Ȃ�
                If m_nSendScode <> CH_SCODE_ALL Then
                    'sql����
                    strSql = "SELECT scode FROM " & DB_SCODE_OUTPUT & " WHERE "
                    strSql &= "�r��EdasysID = " & m_nDelTypeEdasysID(ii)

                    '���R�[�h�Z�b�g
                    sqlReader = tcls_DB.SelectExecute(strSql)
                    'scode���擾()
                    While sqlReader.Read()
                        ' ���MScode�ƁA�ύX�Ώۂ�Scode���قȂ邩�H
                        If m_nSendScode <> CInt(sqlReader.GetValue(0)) Then
                            ' ���MScode��0���H
                            If m_nSendScode = 0 Then
                                ' 0�Ȃ�ύX�Ώۂ�Scode������
                                m_nSendScode = CInt(sqlReader.GetValue(0))
                            Else
                                ' �ύX�Ώۂ�Scode����������Ȃ� ���MScode��S����
                                m_nSendScode = CH_SCODE_ALL
                                Exit While
                            End If
                        End If
                    End While
                    sqlReader.Close()
                End If

                ' SCODE_OUTPUT�̑Ώۂ��r��EdasysID������s���폜����
                ' sql�쐬
                strSql = "DELETE FROM " & DB_SCODE_OUTPUT & " WHERE "
                strSql &= "�r��EdasysID = " & m_nDelTypeEdasysID(ii)
                'SQL���̔��s
                If 0 > tcls_DB.DMLExecute(strSql) Then
                    ' �G���[�Ȃ烍�[���o�b�N���ďI��
                    tcls_DB.TransactRollback()
                    Return False
                End If

                ' �r��EdasysID�Ɋ֘A����DB��ύX
                ' PALL_OUTPUT�̑Ώۂ��r��EdasysID������s���폜����
                'sql�쐬
                strSql = "DELETE FROM " & DB_PALL_OUTPUT & " WHERE "
                strSql &= "�r��EdasysID = " & m_nDelTypeEdasysID(ii)
                'SQL���̔��s
                If 0 > tcls_DB.DMLExecute(strSql) Then
                    ' �G���[�Ȃ烍�[���o�b�N���ďI��
                    tcls_DB.TransactRollback()
                    Return False
                End If
            Next ii

            'Score���傫����(�����ꍇ��ID����������)�ɗD��x��1�`�ݒ肷�邽�߂ɁAScore,ID�ŕ��ёւ���
            Dim drowType() As DataRow = m_DsType.Tables(0).Select("", "Score desc, ID")

            ' �폜�Ώۂ̃O���[�hEdasysID������Ƃ��ɁA�֘A����DB��ύX
            For ii As Integer = 0 To mtyp_Grade.Length - 1
                ' EdasysID��1��菬������Ύ�
                If mtyp_Grade(ii).nEdasysID < 1 Then Continue For
                ' PCODE_OUTPUT��GrEdasysID��ύX
                ' GrEdasysID���g�p���Ă��鍀�ڐ��J��Ԃ�
                For kk As Integer = 0 To strPcodeCol.Length - 1

                    '�񖼂�pcode�܂��́A�r��EdasysID�̏ꍇ�͍X�V���Ȃ��̂Ŏ���
                    If strPcodeCol(kk) = "pcode" Or strPcodeCol(kk) = "�r��EdasysID" Then Continue For

                    '''' SQL���̐���  �폜�ΏۂƓ���GrEdasysID�̍��ڂ����ɋ���GrEdasysID�ɂ���
                    strSql = "UPDATE " & DB_PCODE_OUTPUT & " SET "
                    strSql &= strPcodeCol(kk) & " = " & mtyp_Grade(ii).nNextEdasysID
                    strSql &= " WHERE " & strPcodeCol(kk) & " = " & mtyp_Grade(ii).nEdasysID
                    'SQL���̔��s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                Next kk

                ' PCODE_OUTPUT���������ύX�Ώۂ�scode���擾����
                ' ���Mscode�����łɑS���Ȃ猟�����Ȃ�
                If m_nSendScode <> CH_SCODE_ALL Then
                    'sql����
                    strSql = "SELECT scode FROM " & DB_SCODE_OUTPUT
                    strSql &= " WHERE " & strScodeCol(0) & " = " & mtyp_Grade(ii).nEdasysID

                    '���R�[�h�Z�b�g
                    sqlReader = tcls_DB.SelectExecute(strSql)

                    'scode���擾()
                    While sqlReader.Read()
                        ' ���MScode�ƁA�ύX�Ώۂ�Scode���قȂ邩�H
                        If m_nSendScode <> CInt(sqlReader.GetValue(0)) Then
                            ' ���MScode��0���H
                            If m_nSendScode = 0 Then
                                ' 0�Ȃ�ύX�Ώۂ�Scode������
                                m_nSendScode = CInt(sqlReader.GetValue(0))
                            Else
                                ' �ύX�Ώۂ�Scode����������Ȃ� ���MScode��S����
                                m_nSendScode = CH_SCODE_ALL
                                Exit While
                            End If
                        End If
                    End While
                    sqlReader.Close()
                End If

                ' SCODE_OUTPUT��GrEdasysID��ύX
                ' GrEdasysID���g�p���Ă��鍀�ڐ��J��Ԃ�
                For kk As Integer = 0 To strScodeCol.Length - 1

                    '�񖼂�scode�܂��́A�r��EdasysID�̏ꍇ�͍X�V���Ȃ��̂ŁA����
                    If strScodeCol(kk) = "scode" Or strScodeCol(kk) = "�r��EdasysID" Then Continue For

                    '''' SQL���̐���  �폜�ΏۂƓ���GrEdasysID�̍��ڂ����ɋ���GrEdasysID�ɂ���
                    strSql = "UPDATE " & DB_SCODE_OUTPUT & " SET "
                    strSql &= strScodeCol(kk) & " = " & mtyp_Grade(ii).nNextEdasysID
                    strSql &= " WHERE " & strScodeCol(kk) & " = " & mtyp_Grade(ii).nEdasysID
                    'SQL���̔��s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                Next kk

                ' PALL_OUTPUT��GrEdasysID��ύX
                ' GrEdasysID���g�p���Ă��鍀�ڐ��J��Ԃ�
                For kk As Integer = 0 To strPallOutCol.Length - 1

                    '�񖼂�pcode�܂��́A�r��EdasysID�̏ꍇ�͍X�V���Ȃ��̂Ŏ���
                    If strPallOutCol(kk) = "pcode" Or strPallOutCol(kk) = "�r��EdasysID" Then Continue For

                    '''' SQL���̐���  �폜�ΏۂƓ���GrEdasysID�̍��ڂ����ɋ���GrEdasysID�ɂ���
                    strSql = "UPDATE " & DB_PALL_OUTPUT & " SET "
                    strSql &= strPallOutCol(kk) & " = " & mtyp_Grade(ii).nNextEdasysID
                    strSql &= " WHERE " & strPallOutCol(kk) & " = " & mtyp_Grade(ii).nEdasysID
                    'SQL���̔��s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                Next kk
            Next ii

            ' �r��}�X�^�[�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_TYPE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To drowType.Length - 1
                With drowType(ii)

                    'TYPE_NAME�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_TYPE_NAME & " VALUES ("
                    strSql &= .Item("EdasysID").ToString & ","                  ' EdasysID
                    strSql &= .Item("ID").ToString & ","                        ' ID
                    strSql &= "'" & .Item("Name").ToString & "',"               ' Name
                    strSql &= (ii + 1) & ","                                    ' �D��x
                    strSql &= "'" & .Item("�}�b�v�\������").ToString & "',"     ' �}�b�v�\������
                    strSql &= "'" & .Item("FF�`������").ToString & "',"         ' FF�`������
                    strSql &= .Item("�v���R���O���[�v").ToString & ","          ' �v���R���O���[�v
                    strSql &= .Item("Score").ToString & ","                     ' Score
                    strSql &= .Item("PeriodProcMask").ToString & ","            ' PeriodProcMask
                    strSql &= .Item("�r�}�b�v�V���{���\���F").ToString & ","    ' �r�}�b�v�V���{���\���F
                    strSql &= .Item("�r�}�b�v�V���{��").ToString & ","          ' �r�}�b�v�V���{��
                    strSql &= .Item("�؏o�摜���1").ToString & ","             ' �؏o�摜���1
                    strSql &= .Item("�؏o�摜���2").ToString & ")"             ' �؏o�摜���2

                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If

                End With
            Next ii

            ' �O���[�h�}�X�^�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_GRADE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            'Score���傫����(�����ꍇ��ID����������)�ɗD��x��1�`�ݒ肷�邽�߂ɁAScore,ID�ŕ��ёւ���
            Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select("", "Score desc, ID")
            For ii As Integer = 0 To drowGrade.Length - 1
                With drowGrade(ii)

                    'GRADE_NAME�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_GRADE_NAME & " VALUES ("
                    strSql &= .Item("EdasysID").ToString & ","                      ' EdasysID
                    strSql &= .Item("ID").ToString & ","                            ' ID
                    strSql &= "'" & .Item("Name").ToString & "',"                   ' Name
                    strSql &= (ii + 1) & ","                                        ' �D��x
                    strSql &= .Item("�}�b�v�\���F").ToString & ","                  ' �}�b�v�\���F
                    strSql &= .Item("���׃O���[�h").ToString & ","                  ' ���׃O���[�h
                    strSql &= .Item("FF�`������").ToString & ","                    ' FF�`������
                    strSql &= .Item("�v���R���O���[�v").ToString & ","              ' FF�`������
                    strSql &= .Item("�r�}�b�v�V���{���\���F").ToString() & ","      ' �r�}�b�v�V���{���\���F
                    strSql &= .Item("Score").ToString & ","                         ' Score
                    strSql &= .Item("NumLimit").ToString & ")"                      ' NumLimit

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

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �r��E�O���[�h�o�^�G���[ [" & ex.Message & "] " & Me.Text)
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            Return False
        Finally
            '''' ��n��
            If Not sqlReader Is Nothing Then
                sqlReader.Close()
            End If
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function


    ''' <summary>
    ''' �폜�Ώۂ�DB��EdasysID��Score����A����Score�̍���EdasysID��dgvGrade���猟������
    ''' </summary>
    ''' <param name="nEdasysID">�폜�Ώۂ�EdasysID</param>
    ''' <param name="nScore">Score</param>
    ''' <returns>����Score�̍���EdasysID�i������΂O�j</returns>
    ''' <remarks></remarks>
    Private Function Data_SearchNextGr(ByVal nEdasysID As Integer, ByVal nScore As Integer) As Integer
        Dim nNextScore As Integer              ' ���ɋ���Score
        Dim nNextGrEdasysID As Integer         ' ���ɋ���EdasyssID

        ' �����p�̏�����
        nNextScore = -1
        nNextGrEdasysID = 0               ' ���ɋ���EdasysID���Ȃ���΂��̂܂܂��̒l�ƂȂ�

        '���ɋ���Score��GrEdasysID���擾�A������΂O�ɂȂ�
        For ii As Integer = 0 To dgvGrade.Rows.Count - 1
            ' EdasysID���󔒂łȂ��s���Ώ�
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value) = "" Then Continue For

            ' �폜�Ώۍs��Score���キ�āi�����l�ł��j�A���̒��ň�ԋ����s������
            If nScore >= CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value) And _
             CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value) > nNextScore Then
                ' EdasysID��Score���擾
                nNextGrEdasysID = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value)
                nNextScore = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value)
            End If
        Next ii
        Return nNextGrEdasysID
    End Function


    ''' <summary>
    ''' �����o�ϐ��̏����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()
        ' ���M����scode��������
        m_nSendScode = 0

        ' �폜�Ώۂ��r��EdasysID�z��̏�����
        For ii As Integer = 0 To m_nDelTypeEdasysID.Length - 1
            m_nDelTypeEdasysID(ii) = 0
        Next ii

        ' �폜�Ώۂ̃O���[�hEdasysID�z��̏�����
        For ii As Integer = 0 To mtyp_Grade.Length - 1
            mtyp_Grade(ii).nEdasysID = 0
            mtyp_Grade(ii).nNextEdasysID = 0
        Next ii
    End Sub

    ''' <summary>
    ''' dgvType�Ƀf�[�^�s��}��
    ''' </summary>
    ''' <param name="nRowIndex">�s�̑}���ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvType�Ƀ��R�[�h��}��
        dgvType.Rows.Insert(nRowIndex)

        ' �f�[�^������
        dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, nRowIndex).Value = ""                             ' �r��EdasysId
        dgvType(EM_DATACOLUMNTYPE.emTxt_Name, nRowIndex).Value = ""                                 ' �r�햼��
        dgvType(EM_DATACOLUMNTYPE.emTxt_Score, nRowIndex).Value = ""                                ' Score
        dgvType(EM_DATACOLUMNTYPE.emTxt_Map, nRowIndex).Value = ""                                  ' �}�b�v�\������
        dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, nRowIndex).Value = ""                           ' FF�`������
        dgvType(EM_DATACOLUMNTYPE.emCmb_ProGroup, nRowIndex).Value = "����"                         ' �v���R���O���[�v
        dgvType(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, nRowIndex).Value = 0                        ' PeriodProcMask
        dgvType(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, nRowIndex).Style.BackColor = Color.White    ' �r�}�b�v�V���{���\���F
        dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, nRowIndex).Value = "�E"                          ' �r�}�b�v�V���{��
        dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, nRowIndex).Value = "�����萔"                     ' �؏o�摜���1
        dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, nRowIndex).Value = "�����萔"                     ' �؏o�摜���2
    End Sub

    ''' <summary>
    ''' dgcGrade�Ƀf�[�^�s��}��
    ''' </summary>
    ''' <param name="nRowIndex">�s�̑}���ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_GradeData(ByVal nRowIndex As Integer)
        ' dgv���R�[�h��}��
        dgvGrade.Rows.Insert(nRowIndex)

        ' �f�[�^������
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, nRowIndex).Value = ""                           ' GrEdasysID
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, nRowIndex).Value = ""                               ' �O���[�h����
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, nRowIndex).Value = ""                              ' Score
        dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapButton, nRowIndex).Style.BackColor = Color.White       ' �}�b�v�\���F�ɔ��F��
        dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, nRowIndex).Value = "���Q"                       ' ���׃O���[�h�\��
        dgvGrade(EM_DATACOLUMNGRADE.emCmb_FFSendName, nRowIndex).Value = "����"                     ' FF�`������
        dgvGrade(EM_DATACOLUMNGRADE.emCmb_ProGroup, nRowIndex).Value = "����"                       ' �v���R���O���[�v
        dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, nRowIndex).Style.BackColor = Color.White  ' �r�}�b�v�V���{���\���F�𔒐F
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, nRowIndex).Value = ""                           ' NumLimit
    End Sub

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        '�r��̃f�[�^��\��
        Data_DispDgvType(dgvType)
        '�r��̃C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexDgvType(dgvType)

        '�O���[�h�̃f�[�^��\��
        Data_DispDgvGrade(dgvGrade)
        '�O���[�h�̃C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexDgvGrade(dgvGrade)

        ' �s�̐F��ς��� �i�����\���ŐF���ς��΍�j
        dgvType.CurrentRow.DefaultCellStyle.BackColor = Color.Empty
        dgvGrade.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' �r��dgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvType(ByVal dgv As DataGridView)
        Dim drowType() As DataRow = m_DsType.Tables(0).Select()           ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        '�s��������
        dgv.Rows.Clear()

        ' ��ʂɕ\��
        dgv.RowCount = MAX_TYPE                   ' �r��dgv�̃��R�[�h���͌Œ�
        For ii As Integer = 0 To dgv.RowCount - 1
            ' �r���ڑ��e�[�u���̒l������Ԃ͂��������B������Ώ����l
            If ii < drowType.Length Then
                ' EdasysID��NULL�̎��͋�
                If drowType(ii).Item("EdasysID") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = drowType(ii).Item("EdasysID")
                End If

                ' �r�햼�̂�NULL�̎��͋�
                If drowType(ii).Item("Name") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = drowType(ii).Item("Name")
                End If

                ' �}�b�v�\�����̂�NULL�̂Ƃ��͋�
                If drowType(ii).Item("�}�b�v�\������") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = drowType(ii).Item("�}�b�v�\������")
                End If

                'FF�`������ ��NULL�̂Ƃ��͋�
                If drowType(ii).Item("FF�`������") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = drowType(ii).Item("FF�`������")
                End If

                ' �v���R���O���[�v�̒l��NULL��0
                If (drowType(ii).Item("�v���R���O���[�v") Is System.DBNull.Value) OrElse
                    0 = CInt(drowType(ii).Item("�v���R���O���[�v")) Then
                    ' �v���R���O���[�v�̒l��NULL��0�Ȃ烊�X�g�{�b�N�X��"����"��\������
                    dgv(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value = "����"
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value = CStr(drowType(ii).Item("�v���R���O���[�v"))
                End If

                'Score ��NULL�̂Ƃ��͋�
                If drowType(ii).Item("Score") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = drowType(ii).Item("Score")
                End If

                'PeriodProcMask�̓o�^
                dgv(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value = drowType(ii).Item("PeriodProcMask")

                ' �r�}�b�v�V���{���\���F�̒l��NULL�̎��͔��F
                If drowType(ii).Item("�r�}�b�v�V���{���\���F") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor = Color.White            ' ���F
                Else
                    '�ݒ肳��Ă���F��\��
                    dgv(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor =
                         Color.FromArgb(CInt(drowType(ii).Item("�r�}�b�v�V���{���\���F")))          ' �r�}�b�v�V���{���\���F
                End If

                ' �r�}�b�v�V���{��
                If drowType(ii).Item("�r�}�b�v�V���{��").ToString.Trim = "0" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "�E"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "1" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "2" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "3" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "4" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "5" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "6" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "7" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "��"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "8" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "�~"
                ElseIf drowType(ii).Item("�r�}�b�v�V���{��").ToString = "9" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "�{"
                Else
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "�E"
                End If

                ' �؏o�摜���1
                If drowType(ii).Item("�؏o�摜���1").ToString.Trim = "0" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "�����萔"
                ElseIf drowType(ii).Item("�؏o�摜���1").ToString = "1" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "���摜"
                ElseIf drowType(ii).Item("�؏o�摜���1").ToString = "2" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "���摜"
                ElseIf drowType(ii).Item("�؏o�摜���1").ToString = "3" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "���摜2"
                ElseIf drowType(ii).Item("�؏o�摜���1").ToString = "4" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "���摜3"
                Else
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "�����萔"
                End If

                ' �؏o�摜���2
                If drowType(ii).Item("�؏o�摜���2").ToString.Trim = "0" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "�����萔"
                ElseIf drowType(ii).Item("�؏o�摜���2").ToString = "1" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "���摜"
                ElseIf drowType(ii).Item("�؏o�摜���2").ToString = "2" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "���摜"
                ElseIf drowType(ii).Item("�؏o�摜���2").ToString = "3" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "���摜2"
                ElseIf drowType(ii).Item("�؏o�摜���2").ToString = "4" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "���摜3"
                Else
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "�����萔"
                End If

            Else
                ' �f�[�^�������̂ŏ�����
                dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = ""                            ' �r��EdasysId
                dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""                                ' �r�햼��
                dgv(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = ""                               ' Score
                dgv(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = ""                                 ' �}�b�v�\������
                dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = ""                          ' FF�`������
                dgv(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value = "����"                        ' �v���R���r�O���[�v��"����"��\������
                dgv(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value = 0                       ' PeriodProcMask
                dgv(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor = Color.White   ' �r�}�b�v�V���{���\���F�ɔ��F��
                dgv(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "�E"                         ' �r�}�b�v�V���{��
                dgv(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "�����萔"                    ' �؏o�摜���1
                dgv(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "�����萔"                    ' �؏o�摜���2
            End If
        Next ii

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(1, 0).Selected = True
    End Sub

    ''' <summary>
    ''' �O���[�hdgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvGrade(ByVal dgv As DataGridView)
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select()          ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        '�s��������
        dgv.Rows.Clear()

        ' ��ʂɕ\��
        dgv.RowCount = MAX_GRADE                  ' �O���[�hdgv�̃��R�[�h���͌Œ�
        For ii As Integer = 0 To dgv.RowCount - 1
            If ii < drowGrade.Length Then
                ' GrEdasysID��NULL�̎��͋�
                If drowGrade(ii).Item("EdasysID") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = drowGrade(ii).Item("EdasysID")
                End If

                '�O���[�h���̂�NULL�̎��͋�
                If drowGrade(ii).Item("Name") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = drowGrade(ii).Item("Name")
                End If

                ' �}�b�v�\���F�̒l��NULL�̎��͔��F
                If drowGrade(ii).Item("�}�b�v�\���F") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor = Color.White    ' ���F
                Else
                    '�ݒ肳��Ă���F��\��
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor =
                      Color.FromArgb(CInt(drowGrade(ii).Item("�}�b�v�\���F")))                   ' �}�b�v�\���F
                End If

                ' ���׃O���[�h
                If drowGrade(ii).Item("���׃O���[�h").ToString.Trim = "0" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "���Q"
                ElseIf drowGrade(ii).Item("���׃O���[�h").ToString = "1" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "�y����"
                ElseIf drowGrade(ii).Item("���׃O���[�h").ToString = "2" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "������"
                ElseIf drowGrade(ii).Item("���׃O���[�h").ToString = "3" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "�d����"
                Else
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "���Q"
                End If

                ' FF�`�����̂̒l��NULL��0
                If (drowGrade(ii).Item("FF�`������") Is System.DBNull.Value) OrElse
                    0 = CInt(drowGrade(ii).Item("FF�`������")) Then
                    ' �l��NULL��0�Ȃ烊�X�g�{�b�N�X��"����"��\������
                    dgv(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value = "����"
                Else
                    ' �l��0�ȊO�Ȃ炻�̂܂�
                    dgv(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value = CStr(drowGrade(ii).Item("FF�`������"))  ' FF�`������
                End If

                ' �v���R���O���[�v�̒l��NULL��0
                If (drowGrade(ii).Item("�v���R���O���[�v") Is System.DBNull.Value) OrElse
                    0 = CInt(drowGrade(ii).Item("�v���R���O���[�v")) Then
                    ' �l��NULL��0�Ȃ烊�X�g�{�b�N�X��"����"��\������
                    dgv(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value = "����"
                Else
                    ' �l��0�ȊO�Ȃ炻�̂܂�
                    dgv(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value = CStr(drowGrade(ii).Item("�v���R���O���[�v"))
                End If

                ' �r�}�b�v�V���{���\���F�̒l��NULL�̎��͔��F
                If drowGrade(ii).Item("�r�}�b�v�V���{���\���F") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor = Color.White      ' ���F
                Else
                    '�ݒ肳��Ă���F��\��
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor =
                      Color.FromArgb(CInt(drowGrade(ii).Item("�r�}�b�v�V���{���\���F")))                ' �r�}�b�v�V���{���\���F
                End If

                'Score��NULL�̎��͋�
                If drowGrade(ii).Item("Score") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = drowGrade(ii).Item("Score")
                End If

                ' NumLimit
                dgv(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value = drowGrade(ii).Item("NumLimit")
            Else
                dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = ""                           ' GrEdasysID
                dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""                               ' �O���[�h����
                dgv(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = ""                              ' Score
                dgv(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor = Color.White       ' �}�b�v�\���F�ɔ��F��
                dgv(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "���Q"                       ' ���׃O���[�h��"���Q"��\������
                dgv(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value = "����"                     ' FF�`������
                dgv(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value = "����"                       ' �v���R���r�O���[�v��"����"��\������
                dgv(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor = Color.White  ' �r�}�b�v�V���{���\���F�ɔ��F��  
                dgv(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value = ""                           ' NumLimit
            End If
        Next ii

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(1, 0).Selected = True
    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '��ʂ���폜���ꂽEdasysID���擾
            Data_CheckDelete()

            ' ��ʂ̃f�[�^���擾
            Data_Get()

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

            ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If
            '�f�[�^���Ď擾�����ׁA�R���{�{�b�N�X�pGRADE_NAME��ҏW
            GradeData_Init()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �ݒ�̏�����
        Data_Init()

        ' ��ʂɒl��\��
        Data_Disp()
    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     ' dgv�̃f�[�^�i�[�p
        Dim strMsg As String    ' �d���p�̃G���[���b�Z�[�W
        ' �r��dgv�A1024�s���[�v
        For ii As Integer = 0 To dgvType.Rows.Count - 1
            ' EdasysID
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "�r��ݒ� No=" & ii + 1 & " EdasysID") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = strWk

            ' Edasysid���󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            ' EdasysID�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvType.Rows.Count - 1
                ' ��r���EdasysID�����l�Ȃ��r
                If IsNumeric(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, jj).Value) Then
                    ' EdasysID�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value) = CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, jj).Value) Then
                        strMsg = "�y �r��ݒ� �z" & vbNewLine
                        strMsg &= "����EdasysID�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �r�햼��
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "�r��ݒ� No=" & ii + 1 & " �r�햼��") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = strWk

            ' �r���̂̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvType.Rows.Count - 1
                ' ��r���EdasysID�����l�Ȃ�O���[�h���̂��r
                If IsNumeric(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, jj).Value) Then
                    ' �O���[�h���̂������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value) = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, jj).Value) Then
                        strMsg = "�y �r��ݒ� �z" & vbNewLine
                        strMsg &= "�����r�햼�͓̂o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            'Score
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "�r��ݒ� No=" & ii + 1 & " Score") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = CInt(strWk)

            ' �}�b�v�\������
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 6, "�r��ݒ� No=" & ii + 1 & " �}�b�v�\������") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = strWk

			'FF�`������
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "�r��ݒ� No=" & ii + 1 & " FF�`������") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = strWk

        Next ii

        ' �O���[�hdgv8�s���[�v
        For ii As Integer = 0 To dgvGrade.Rows.Count - 1
            ' EdasysID
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "�O���[�h�ݒ� No=" & ii + 1 & " EdasysID") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = strWk

            ' Edasysid���󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            ' EdasysID�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvGrade.Rows.Count - 1
                ' ��r���EdasysID�����l�Ȃ��r
                If IsNumeric(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, jj).Value) Then
                    ' EdasysID�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value) = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, jj).Value) Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "����EdasysID�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �O���[�h����
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "�O���[�h�ݒ� No=" & ii + 1 & " �O���[�h����") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = strWk

            ' �O���[�h���̂̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvGrade.Rows.Count - 1
                ' ��r���EdasysID�����l�Ȃ�O���[�h���̂��r
                If IsNumeric(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, jj).Value) Then
                    ' �O���[�h���̂������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value) = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, jj).Value) Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "�����O���[�h���͓̂o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' Score
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "�O���[�h�ݒ� No=" & ii + 1 & " Score") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = strWk

            ' ���׃O���[�h          �R���{�{�b�N�X�Ȃ̂Ń`�F�b�N���Ȃ�

            ' �O���[�h�ʑ��M�����
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "�O���[�h�ݒ� No=" & ii + 1 & " �O���[�h�ʑ��M�����") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value = strWk

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\������EdasysID��DB��EdasysID���ׂāA�폜���ꂽEdasysID�����邩�`�F�b�N����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_CheckDelete()
        Dim bCheck As Boolean   '�`�F�b�N�t���O True�Ȃ�EdasysID������

        ' �r���ڑ��e�[�u���őS�s���[�v
        For ii As Integer = 0 To m_DsType.Tables(0).Rows.Count - 1
            ' �`�F�b�N�t���O�̏�����
            bCheck = False
            ' �r��dgv�őS�s���[�v
            For kk As Integer = 0 To dgvType.Rows.Count - 1
                ' EdasysID���󔒍s�ȊO�ŁA��r
                If CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, kk).Value) <> "" AndAlso _
                 CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, kk).Value) = CInt(m_DsType.Tables(0).Rows(ii).Item("EdasysID")) Then
                    ' EdasysID����v�����Ƃ��A�`�F�b�NOK
                    bCheck = True
                    Exit For
                End If
            Next kk
            ' �`�F�b�N�t���O��False�̂Ƃ������ꂽID�Ȃ̂ŁA�o�^����
            If bCheck = False Then
                m_nDelTypeEdasysID(ii) = CInt(m_DsType.Tables(0).Rows(ii).Item("EdasysID"))
            End If
        Next ii

        ' �O���[�h��ڑ��e�[�u���őS�s���[�v
        For ii As Integer = 0 To m_DsGrade.Tables(0).Rows.Count - 1
            ' �`�F�b�N�t���O�̏�����
            bCheck = False
            ' �O���[�hdgv�őS�s���[�v
            For kk As Integer = 0 To dgvGrade.Rows.Count - 1
                ' EdasysID���󔒍s�ȊO�Ŕ�r
                If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, kk).Value) <> "" AndAlso _
                  CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, kk).Value) = CInt(m_DsGrade.Tables(0).Rows(ii).Item("EdasysID")) Then
                    ' EdasysID����v�����Ƃ��A�`�F�b�NOK
                    bCheck = True
                    Exit For
                End If
            Next kk
            ' �`�F�b�N�t���O��False�̂Ƃ������ꂽEdasysID�Ȃ̂ŁA����Score�̋���EdasysID����������
            If bCheck = False Then
                ' �폜�Ώۂ�EdasysID���i�[
                mtyp_Grade(ii).nEdasysID = CInt(m_DsGrade.Tables(0).Rows(ii).Item("EdasysID"))
                ' ���ɋ���Score��GrEdasysID������
                mtyp_Grade(ii).nNextEdasysID = Data_SearchNextGr(mtyp_Grade(ii).nEdasysID, CInt(m_DsGrade.Tables(0).Rows(ii).Item("Score")))
            End If
            '// <<<--- 2013.03.27 �D��x�����ݒ�
        Next ii
    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim nCountNo As Integer   'No�J�E���g�p

        ' �f���[�g�C���T�[�g����ׁA�������Ă���s���N���A����
        m_DsType.Tables(0).Rows.Clear()

        'No������
        nCountNo = 0

        ' �r��dgv�̒l���r���ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvType.RowCount - 1
            'EdasysID ���󔒈ȊO�̍s�����擾
            If CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value) = "" Then Continue For
            Dim drowType As DataRow = m_DsType.Tables(0).NewRow()                                                ' �V�K�s

            drowType.Item("EdasysID") = CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value)                ' EdasysID
            drowType.Item("ID") = nCountNo                                                                       ' ID�͕\������
            drowType.Item("Name") = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)                        ' Name
            drowType.Item("Score") = CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value)                      ' Score
            drowType.Item("�}�b�v�\������") = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value)               ' �}�b�v�\������
            drowType.Item("FF�`������") = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value)            ' FF�`������

            '�v���R���O���[�v�A�R���{�{�b�N�X�Ȃ̂ŁA������I������Ă�����0��
            If CStr(dgvType(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value) = "����" Then
                drowType.Item("�v���R���O���[�v") = 0
            Else
                drowType.Item("�v���R���O���[�v") = CInt(dgvType(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value)
            End If

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			' drowType.Item("PeriodProcMask") = CByte(dgvType(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value)   ' PeriodProcMask
			drowType.Item("PeriodProcMask") = CInt(dgvType(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value)   ' PeriodProcMask
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------


            '�r�}�b�v�V���{���\���F �{�^���̔w�i�F��ARGB�`���ɂ��Ċi�[
            drowType.Item("�r�}�b�v�V���{���\���F") =
                dgvType(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor.ToArgb()

            ' �r�}�b�v�V���{���́A�R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "�E" Then
                drowType.Item("�r�}�b�v�V���{��") = 0
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 1
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 2
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 3
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 4
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 5
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 6
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "��" Then
                drowType.Item("�r�}�b�v�V���{��") = 7
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "�~" Then
                drowType.Item("�r�}�b�v�V���{��") = 8
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "�{" Then
                drowType.Item("�r�}�b�v�V���{��") = 9
            Else
                drowType.Item("�r�}�b�v�V���{��") = 0
            End If

            ' �؏o�摜���1�́A�R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "�����萔" Then
                drowType.Item("�؏o�摜���1") = 0
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "���摜" Then
                drowType.Item("�؏o�摜���1") = 1
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "���摜" Then
                drowType.Item("�؏o�摜���1") = 2
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "���摜2" Then
                drowType.Item("�؏o�摜���1") = 3
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "���摜3" Then
                drowType.Item("�؏o�摜���1") = 4
            Else
                drowType.Item("�؏o�摜���1") = 0
            End If

            ' �؏o�摜���2�́A�R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "�����萔" Then
                drowType.Item("�؏o�摜���2") = 0
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "���摜" Then
                drowType.Item("�؏o�摜���2") = 1
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "���摜" Then
                drowType.Item("�؏o�摜���2") = 2
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "���摜2" Then
                drowType.Item("�؏o�摜���2") = 3
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "���摜3" Then
                drowType.Item("�؏o�摜���2") = 4
            Else
                drowType.Item("�؏o�摜���2") = 0
            End If

            m_DsType.Tables(0).Rows.Add(drowType)     '�s��ǉ�
            nCountNo += 1   'No=No+1
        Next ii

        ' �f���[�g�C���T�[�g����ׁA�������Ă���s���N���A����
        m_DsGrade.Tables(0).Rows.Clear()

        'No������
        nCountNo = 0

        ' �O���[�hdgv�̒l���O���[�h��ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvGrade.RowCount - 1
            'EdasysID ���󔒈ȊO�̍s�����擾
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value) = "" Then Continue For
            Dim drowGrade As DataRow = m_DsGrade.Tables(0).NewRow()                                 ' �V�K�s

            drowGrade.Item("EdasysID") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value)    ' EdasysID
            drowGrade.Item("ID") = nCountNo                                                             ' ID�͐U��Ȃ���
            drowGrade.Item("Name") = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)            ' Name
            drowGrade.Item("Score") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value)          ' Score
            '�}�b�v�\���F �{�^���̔w�i�F��ARGB�`���ɂ��Ċi�[
            drowGrade.Item("�}�b�v�\���F") =
             dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor.ToArgb()

            '���׃O���[�h �R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "���Q" Then
                drowGrade.Item("���׃O���[�h") = 0
            ElseIf dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "�y����" Then
                drowGrade.Item("���׃O���[�h") = 1
            ElseIf dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "������" Then
                drowGrade.Item("���׃O���[�h") = 2
            ElseIf dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "�d����" Then
                drowGrade.Item("���׃O���[�h") = 3
            Else
                drowGrade.Item("���׃O���[�h") = "���Q"
            End If

            'FF�`�����́A�R���{�{�b�N�X�Ȃ̂ŁA������I������Ă�����0��
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value) = "����" Then
                drowGrade.Item("FF�`������") = 0
            Else
                drowGrade.Item("FF�`������") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value)
            End If

            '�v���R���O���[�v�A�R���{�{�b�N�X�Ȃ̂ŁA������I������Ă�����0��
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value) = "����" Then
                drowGrade.Item("�v���R���O���[�v") = 0
            Else
                drowGrade.Item("�v���R���O���[�v") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value)
            End If

            '�r�}�b�v�V���{���\���F �{�^���̔w�i�F��ARGB�`���ɂ��Ċi�[
            drowGrade.Item("�r�}�b�v�V���{���\���F") =
             dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor.ToArgb()

            drowGrade.Item("NumLimit") = CByte(dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value)   ' NumLimit

            m_DsGrade.Tables(0).Rows.Add(drowGrade)       '�s��ǉ�
            nCountNo += 1                                 'No = No + 1
        Next ii
    End Sub

	''' <summary>
	''' �A�b�v�f�[�g
	''' </summary>
	''' <returns>���A���(True:����,False:�ُ�)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Try
			' �r��}�X�^�[�e�[�u���ƃO���[�h�}�X�^�[�e�[�u���̒l��o�^
			If Not DB_Delete() Then
				Return False
			End If

			'�����@�\�փp�����[�^�ύX�ʒm
			modMain.Send_Main(CH_PARAM_TYPE_GRADE, m_nSendScode)		' �r��EGr
			modMain.HardCopy(Me)
			Return True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
			Return False
		End Try
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ DataGridView �֘A
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �r��dgv�̗��`
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Sub dgvInitializeDefType(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)						' �I��F����

		With dgv
			.ColumnHeadersHeight = 42									' �w�b�_�[�̍���
			.AllowUserToResizeColumns = True							' ���[�U�[ ��T�C�Y�ύX
			.ScrollBars = ScrollBars.Both								' �X�N���[���o�[�̕\��

			' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		' �I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		' ��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
		End With

		'' ���`
		' No
		dgvColSetText(dgv, "No", 42, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' EdasysID
		dgvColSetText(dgv, "EdasysID" & vbNewLine & "(1�ȏ�)", 85, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �r�햼��
        dgvColSetText(dgv, "�r�햼��" & vbNewLine & "(���p�U�S����)", 148, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' Score
        dgvColSetText(dgv, "Score" & vbNewLine & "���P", 67, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.TopCenter, DataGridViewContentAlignment.TopCenter, "")

		' �}�b�v�\������
		dgvColSetText(dgv, "�}�b�v" & vbNewLine & "�\������", 86, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' FF�`������
        dgvColSetText(dgv, "FF" & vbNewLine & "�`������", 80, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        '  �v���R���r�O���[�v
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�v���R��" & vbNewLine & "�r�O���[�v"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            lstItem.Items.AddRange("����", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10")
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' PeriodProcMask
        dgvColSetCheck(dgv, "PeriodProcMask", 70, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

        ' �r�}�b�v�V���{���\���F
        Using clm As New DataGridViewButtonColumn
            clm.HeaderText = "�r�}�b�v" & vbNewLine & "�\���F"
            clm.Width = 86
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            Dim mapBtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
            mapBtnText.UseColumnTextForButtonValue = True
            mapBtnText.Text = "�ύX"
            dgv.Columns.Add(clm)
        End Using

        ' �r�}�b�v�V���{��
        Using clm As New DataGridViewComboBoxColumn
			clm.HeaderText = "�r�}�b�v" & vbNewLine & "�V���{��"
			clm.Width = 80
			clm.SortMode = DataGridViewColumnSortMode.NotSortable
			clm.Frozen = False
			clm.ReadOnly = False
			clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
			Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("�E", "��", "��", "��", "��", "��", "��", "��", "�~", "�{")
			listItemFF.MaxDropDownItems = listItemFF.Items.Count
			dgv.Columns.Add(clm)
        End Using

        ' �؏o�摜���1
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�摜�\��1"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("�����萔", "���摜", "���摜", "���摜2", "���摜3")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            clm.Visible = True
            dgv.Columns.Add(clm)
        End Using

        ' �؏o�摜���2
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�摜�\��2"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("�����萔", "���摜", "���摜", "���摜2", "���摜3")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            clm.Visible = True
            dgv.Columns.Add(clm)
        End Using

        ' ��֑}��
        dgvColSetButton(dgv, "���" & vbNewLine & "�}��", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�}��")

		' �폜
		dgvColSetButton(dgv, "�폜", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�폜")

	End Sub

	''' <summary>
	''' �O���[�hdgv�̗��`
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Sub dgvInitializeDefGrade(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)						' �I��F����

		With dgv
			.ColumnHeadersHeight = 42									' �w�b�_�[�̍���
			.AllowUserToResizeColumns = True							' ���[�U�[ ��T�C�Y�ύX
			.ScrollBars = ScrollBars.Both								' �X�N���[���o�[�̕\��

			' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		' �I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		' ��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
		End With

		'' ���`
		' No
		dgvColSetText(dgv, "No", 42, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' EdasysID
		dgvColSetText(dgv, "EdasysID" & vbNewLine & "(1�ȏ�)", 80, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �O���[�h����
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		'dgvColSetText(dgv, "�O���[�h����" & vbNewLine & "(���p64����)", 198, False, False,
		'DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

		dgvColSetText(dgv, "�O���[�h����" & vbNewLine & "(���p64����)", 181, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        ' Score
        dgvColSetText(dgv, "Score" & vbNewLine & "���P", 57, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.TopCenter, DataGridViewContentAlignment.TopCenter, "")

		' �}�b�v�\���F
		dgvColSetButton(dgv, "�T�����" & vbNewLine & "�\���F", 80, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
		New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�ύX")

        '  ���׃O���[�h
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "����" & vbNewLine & "�O���[�h"
            clm.Width = 83
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("���Q", "�y����", "������", "�d����")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        '  FF�`������
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "FF" & vbNewLine & "�`������"
            clm.Width = 83
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("����", "1", "2", "3", "4", "5", "6", "7", "8")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        '  �v���R���r�O���[�v
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�v���R��" & vbNewLine & "Gr�O���[�v"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItemPro As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            lstItemPro.Items.AddRange("����", "1", "2", "3", "4", "5", "6")
            lstItemPro.MaxDropDownItems = lstItemPro.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' �r�}�b�v�V���{���\���F
        dgvColSetButton(dgv, "�r�}�b�v" & vbNewLine & "�\���F", 80, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
        New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�ύX")

        ' NumLimit
        dgvColSetText(dgv, "�O���[�h��" & vbNewLine & "���M�����", 94, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' ��֑}��
        dgvColSetButton(dgv, "���" & vbNewLine & "�}��", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�}��")

		' �폜
		dgvColSetButton(dgv, "�폜", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�폜")

	End Sub

    ''' <summary>
    ''' �r��dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvType_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvType.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNTYPE.emBtn_MapSymbolColor     '�r�}�b�v�V���{���\���F
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
            Case EM_DATACOLUMNTYPE.emInsButton     '��֑}���{�^��
                InsBtnClick_Type(dgv, e.RowIndex)
            Case EM_DATACOLUMNTYPE.emDelButton     '�폜�{�^��
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' �O���[�hdgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGrade_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGrade.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNGRADE.emBtn_MapButton, EM_DATACOLUMNGRADE.emBtn_MapSynbolColor  '�}�b�v�\���F,�r�}�b�v�V���{���\���F
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
            Case EM_DATACOLUMNGRADE.emInsButton    '��֑}���{�^��
                InsBtnClick_Grade(dgv, e.RowIndex)
            Case EM_DATACOLUMNGRADE.emDelButton    '�폜�{�^��
                DelBtnClick_Grade(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' �}�b�v�\���F�{�^���������ꂽ��A�F�I���_�C�A���O��\�����A�{�^���̐F��ύX
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nColIndex">�{�^���̗�C���f�b�N�X</param>
    ''' <param name="nRowIndex">�{�^���̍s�C���f�b�N�X</param>
    ''' <remarks></remarks>
    Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
        '�F�I���_�C�A���O�̐ݒ�
        '�F�̍쐬���g�p���Ȃ�
        dlgColor.AllowFullOpen = False

        '�F�I���_�C�A���O�̑I��F���A�{�^���̔w�i�F�ɂ���
        dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

        ' �F�I���_�C�A���O��ok�{�^���������ꂽ��A�{�^���̐F��ύX���� 
        If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
            '�I��F���{�^���̔w�i�F��
            dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
        End If

        dlgColor.Dispose()
    End Sub

    ''' <summary>
    ''' �r��dgv�̑}���{�^�������ꂽ�Ƃ��̏���
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

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            '�ŏI�s�̃e�L�X�g�{�b�N�X�ɉ�������̃f�[�^�������Ă���ꍇ�͑}���s��
            If Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Score, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Map, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, dgv.Rows.Count - 1).Value)) <> "" Then

                frmMsg.Text = "�����m�F"
                frmMsg.ChangeMsg("�ŏI�s�Ƀf�[�^�����݂��܂��B", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Exit Sub
            End If

            '�}�����邩���b�Z�[�W���o��
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�}�����܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �r��dgv�Ƀ��R�[�h��}��
        InsertAt_TypeData(nRowIndex)

        ' �r��dgv�̍ŏI�s���폜
        dgv.Rows.RemoveAt(dgvType.Rows.Count - 1)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexDgvType(dgv)

    End Sub

    ''' <summary>
    ''' �r��dgv�̍폜�{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' �폜���邩���b�Z�[�W���o��
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�폜���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �r��dgv�̍ŏI�s�Ƀ��R�[�h��}��
        InsertAt_TypeData(dgv.Rows.Count)

        ' �r��dgv�̃��R�[�h���폜
        dgv.Rows.RemoveAt(nRowIndex)

        ' No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexDgvType(dgv)

    End Sub

    ''' <summary>
    ''' �O���[�hdgv�̑}���{�^�������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nRowIndex">���R�[�h��}������ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Grade(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        ' �}���{�^�����g�p�\���`�F�b�N
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' �ŏI�s�͎g�p�s�Ȃ̂Ŕ�����
            Exit Sub
        End If

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' �ŏI�s�̃e�L�X�g�{�b�N�X�ɉ�������̃f�[�^�������Ă���ꍇ�͑}���s��
            If Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, dgv.Rows.Count - 1).Value)) <> "" Or
               Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or
               Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_Score, dgv.Rows.Count - 1).Value)) <> "" Or
               Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_NumLimit, dgv.Rows.Count - 1).Value)) <> "" Then
                frmMsg.Text = "�����m�F"
                frmMsg.ChangeMsg("�ŏI�s�Ƀf�[�^�����݂��܂��B", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Exit Sub
            End If


            ' �}�����邩���b�Z�[�W���o��
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�}�����܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' �O���[�hdgv�Ƀ��R�[�h��}�� (�\���ʒu������Ȃ��悤�ɑ}�����폜�̏��ōs��)
        InsertAt_GradeData(nRowIndex)

        ' �O���[�hdgv�̍ŏI�s���폜
        dgv.Rows.RemoveAt(dgv.Rows.Count - 1)

        ' No��U��Ȃ����A�{�^����\�����Ȃ���
        InitIndexDgvGrade(dgv)

    End Sub

    ''' <summary>
    ''' �O���[�hdgv�̍폜�{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Grade(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' �폜���邩���b�Z�[�W���o��
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�폜���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try
        ' �O���[�hdgv�̍ŏI�s�Ƀ��R�[�h��}��  (�\���ʒu������Ȃ��悤�ɑ}�����폜�̏��ōs��)
        InsertAt_GradeData(dgv.Rows.Count)

        ' �O���[�hdgv�̍s���폜
        dgv.Rows.RemoveAt(nRowIndex)

        ' No��U��Ȃ����A�{�^����\�����Ȃ���
        InitIndexDgvGrade(dgv)

    End Sub

    ''' <summary>
    ''' dgvType�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">�r��dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvType(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNTYPE.emNo, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�

            ' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
            If ii = dgv.RowCount - 1 Then
                ' �ŏI�s�͑}���{�^�����g�p�s��
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Gray
            Else
                ' ����ȊO�͎g�p��
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' dgvGrade�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">�O���[�hdgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvGrade(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNGRADE.emNo, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�

            ' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
            If ii = dgv.RowCount - 1 Then
                ' �ŏI�s�͑}���{�^�����g�p�s��
                dgv(EM_DATACOLUMNGRADE.emInsButton, ii).Style.ForeColor = Color.Gray
            Else
                ' ����ȊO�͎g�p��
                dgv(EM_DATACOLUMNGRADE.emInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvGrade.SelectionChanged, dgvType.SelectionChanged
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
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGrade.CellEnter, dgvType.CellEnter
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

    Private Sub dgvGrade_DataError(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewDataErrorEventArgs) Handles dgvGrade.DataError
        If Not (e.Exception Is Nothing) Then
            MessageBox.Show(Me, String.Format("({0},{1})�̃Z���ŃG���[���������܂����B " + vbCrLf + vbCrLf + "�����F {2}", _
            e.ColumnIndex, e.RowIndex, e.Exception.Message), "�G���[���������܂���", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End If
    End Sub

    ''' <summary>
    ''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
    ''' </summary>
    ''' <remarks>�O���[�h���̂Ɉȏ������A�����̍s��ǉ��A</remarks>
    Private Sub GradeData_Init()
        Dim drowNewGrade As DataRow     ' �r��O���[�h�̐V�K�s

        Try
            '�O���[�h��ڑ��e�[�u���̐ݒ�
            ' �S�s���[�v �O���[�h�̖��̂Ɉȏ������
            For ii As Integer = 0 To m_DsGradeWk.Tables(0).Rows.Count - 1
                m_DsGradeWk.Tables(0).Rows(ii).Item("Name") = m_DsGradeWk.Tables(0).Rows(ii).Item("Name").ToString & "�ȏ�"
            Next ii
            ' �O���[�h�̐V�K�s��ݒ� EdasysID�A�O���[�h���́A�}�b�v�\���F
            drowNewGrade = m_DsGradeWk.Tables(0).NewRow()
            drowNewGrade.Item("EdasysID") = 0
            drowNewGrade.Item("Name") = "-"
            drowNewGrade.Item("�}�b�v�\���F") = -1      ' �}�b�v�\���F�A�g��Ȃ����ǈꉞ�ݒ�
            ' �e�[�u���̈�ԏ�ɑ}��
            m_DsGradeWk.Tables(0).Rows.InsertAt(drowNewGrade, 0)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�p�f�[�^ �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

End Class
