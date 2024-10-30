'*********************************************************************************
' �G�b�W���o�������
'	[Ver]
'		Ver.01    2011/06/01  �V�K�쐬
'
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports System.IO

Public Class frmEdgeList
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////

    '�G�b�W���o���э\���� 
    Private Structure EDGE_RESULT_INF
        Dim strKizuKenNo As String          '�Ǘ�No
        Dim strCoilNo As String             '�R�C���ԍ�
        Dim strInDate As String             '�o�^���t

        Dim strMinItaHaba_O As String       '���ŏ�(�\)
        Dim strMaxItaHaba_O As String       '���ő�(�\)
        Dim strOutSideN_O As String         '�����e�͈͊O��(�\)
        Dim strNgN_Left_O As String         '�G�b�W���o���s��_��(�\)
        Dim strNgN_Right_O As String        '�G�b�W���o���s��_�E(�\)
        Dim strMinItaHaba_U As String       '���ŏ�(��)
        Dim strMaxItaHaba_U As String       '���ő�(��)
        Dim strOutSideN_U As String         '�����e�͈͊O��(��)
        Dim strNgN_Left_U As String         '�G�b�W���o���s��_��(��)
        Dim strNgN_Right_U As String        '�G�b�W���o���s��_�E(��)

        '�C�j�V�����C�Y
        Public Sub initialize()
            strKizuKenNo = ""               '�Ǘ�No
            strCoilNO = ""                  '�R�C���ԍ�
            strInDate = ""                  '�o�^���t

            strMinItaHaba_O = ""            '���ŏ�(�\)
            strMaxItaHaba_O = ""            '���ő�(�\)
            strOutSideN_O = ""              '�����e�͈͊O��(�\)
            strNgN_Left_O = ""              '�G�b�W���o���s��_��(�\)
            strNgN_Right_O = ""             '�G�b�W���o���s��_�E(�\)

            strMinItaHaba_U = ""            '���ŏ�(��)
            strMaxItaHaba_U = ""            '���ő�(��)
            strOutSideN_U = ""              '�����e�͈͊O��(��)
            strNgN_Left_U = ""              '�G�b�W���o���s��_��(��)
            strNgN_Right_U = ""             '�G�b�W���o���s��_�E(��)
        End Sub

    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_proID As System.Diagnostics.Process = Nothing                 'Excel�̃v���Z�XID

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        MyBase.SetButtonEnable(0, True)
        MyBase.SetButtonEnable(1, True)
        MyBase.SetButtonEnable(2, True)
        Me.TopMost = False

    End Sub
    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeList_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

    End Sub
    ''' <summary>
    ''' �t�@���N�V�����{�^����������
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeList_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        Dim bRet As Boolean

        If 0 = FncNo Or 4 = FncNo Then

            'CSV�쐬
            Select Case FncNo
                Case 0  '�\��
                    bRet = MakeCSV(0)
                Case 4  '�ۑ�
                    bRet = MakeCSV(1)
            End Select

            If Not bRet Then

                Dim strMsg As String

                strMsg = "CSV�̍쐬�Ɏ��s���܂����B"
                '''' �K�C�_���X�\��
                Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
                    frmMsg.ShowDialog()
                End Using
            End If
            Exit Sub
        End If

        Me.Close()

        '�Ăь��֒ʒm
        'RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub

    ''' <summary>
    ''' ���t,���ԃR���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub


    ''' <summary>
    ''' CSV�쐬
    ''' </summary>
    ''' <param name="nMode">0:�\��,1:���s</param>
    ''' <returns>���A���(Ture:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function MakeCSV(ByVal nMode As Integer) As Boolean

        Dim strPath As String = ""                      '�p�X
        Dim lstData As New List(Of EDGE_RESULT_INF)     '�G�b�W���o���уf�[�^���X�g
        Dim strDateStart As String                      '�J�n���t
        Dim strDateEnd As String                        '�I�����t

        '���X�g������
        lstData.Clear()
        strDateStart = Format(dtpDateStart.Value, "yyyy/MM/dd ") & Format(dtpTimeStart.Value, "HH:mm:ss")
        strDateEnd = Format(dtpDateEnd.Value, "yyyy/MM/dd ") & Format(dtpTimeEnd.Value, "HH:mm:ss")

        '�f�[�^�擾
        If Not GetData(strDateStart, strDateEnd, lstData) Then Return False

        '''' �t�H���_�̗L���`�F�b�N
        tMod.CheckFolder(CSV_PATH)

        Select Case nMode

            Case 0  '�\��
                '��Ɨ̈�֕ۑ�
                strPath = CSV_PATH & "\�G�b�W���o�������.csv"
                strPath = IO.Path.GetFullPath(strPath)
            Case 1  '�ۑ�
                'TEMP�t�H���_�֕ۑ�
                strPath = CSV_PATH & "\�G�b�W���o�������_" & Format(Now, "yyyyMMdd_HHmmss") & ".csv"
                strPath = IO.Path.GetFullPath(strPath)

        End Select

        '����t�@�C���ǂݎ���p����
        Dim fi As FileInfo = New FileInfo(strPath)
        If IO.File.Exists(strPath) Then
            '�ǂݎ���p����
            fi.IsReadOnly = False
        End If

        ''CSV�f�[�^�Z�b�g
        If Not SetData(strPath, lstData) Then Return False

        Select Case nMode

            Case 0  '�\��
                '�ǂݎ���p�ŕ\��
                fi.IsReadOnly = True

                If tClassLibrary.tClass.tClass_Excel_Base.Check_Install_Excel Then
                    m_proID = tClassLibrary.tClass.tClass_Excel_Base.Start_ExcelFile(strPath)
                Else
                    ' Execl���C���X�g�[������Ă��Ȃ��ꍇ�́A�������ŊJ���B
                    System.Diagnostics.Process.Start("notepad.exe", strPath)
                End If
                'm_proID = tClassLibrary.tClass.tClass_Excel_Base.Start_ExcelFile(strPath)

            Case 1  '�ۑ�

                Dim strMsg As String

                strMsg = strPath & "�ɕۑ����܂����B"
                '''' �K�C�_���X�\��
                Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
                    frmMsg.ShowDialog()
                End Using
        End Select

        lstData.Clear()
        lstData = Nothing

        Return True

    End Function

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="strStartDate">�J�n���t</param>
    ''' <param name="strEndDate">�I�����t</param>
    ''' <param name="lstData">�G�b�W���o���у��X�g</param>
    ''' <returns>���A���(Ture:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strStartDate As String, ByVal strEndDate As String, _
                             ByRef lstData As List(Of EDGE_RESULT_INF)) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""
        Dim typData As EDGE_RESULT_INF = Nothing            '�G�b�W���o����

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_EDGE_COIL_INF & "]")
            Return False
        End If

        'SQL���쐬
        strSql = ""

        strSql &= " SELECT "
        strSql &= " R.�Ǘ�No as �Ǘ�No, "
        strSql &= " ISNULL(C.���YNo, '') as �R�C���ԍ�, "
        strSql &= " ISNULL(R.�o�^���t, '') as �o�^���t, "

        strSql &= " EO.���ŏ� as �\_���ŏ�, "
        strSql &= " EO.���ő� as �\_���ő�, "
        strSql &= " EO.�����e�͈͊O�� as �\_�����e�͈͊O��, "
        strSql &= " EO.�G�b�W���o���s��_�� as �\_�G�b�W���o���s��_��, "
        strSql &= " EO.�G�b�W���o���s��_�E as �\_�G�b�W���o���s��_�E, "

        strSql &= " EU.���ŏ� as ��_���ŏ�, "
        strSql &= " EU.���ő� as ��_���ő�, "
        strSql &= " EU.�����e�͈͊O�� as ��_�����e�͈͊O��, "
        strSql &= " EU.�G�b�W���o���s��_�� as ��_�G�b�W���o���s��_��, "
        strSql &= " EU.�G�b�W���o���s��_�E as ��_�G�b�W���o���s��_�E "

        strSql &= " FROM  " & DB_COIL_RESULT & " R "

        strSql &= " LEFT OUTER JOIN " & DB_COIL_INF & " C "
        strSql &= " ON C.�Ǘ�No = R.�Ǘ�No "

        strSql &= " LEFT OUTER JOIN "
        strSql &= " (SELECT * FROM " & DB_EDGE_COIL_INF & " WHERE �\���敪=0 ) EO"
        strSql &= " ON EO.�Ǘ�No = R.�Ǘ�No "

        strSql &= " LEFT OUTER JOIN "
        strSql &= " (SELECT * FROM " & DB_EDGE_COIL_INF & " WHERE �\���敪=1 ) EU"
        strSql &= " ON EU.�Ǘ�No = R.�Ǘ�No "

        strSql &= " WHERE 1 = 1"

        If Not chkALL.Checked Then
            strSql &= " AND NOT ( EO.�Ǘ�No IS  NULL AND EU.�Ǘ�No IS NULL ) "
        End If

        strSql &= " AND R.�o�^���t >= '" & strStartDate & "'"

        strSql &= " AND R.�o�^���t <= '" & strEndDate & "'"

        strSql &= " Order By R.�o�^���t DESC "

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSql)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_EDGE_COIL_INF & "]")
            Return False
        End If

        Try

            While sqlRead.Read()

                typData.initialize()
                typData.strKizuKenNo = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�Ǘ�No")))
                typData.strCoilNo = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�R�C���ԍ�")))
                typData.strInDate = sqlRead.GetDateTime(sqlRead.GetOrdinal("�o�^���t")).ToString("yyyy/MM/dd HH:mm:ss")

                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("�\_���ŏ�")) Then
                    typData.strMinItaHaba_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�\_���ŏ�")))
                End If

                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("�\_���ő�")) Then
                    typData.strMaxItaHaba_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�\_���ő�")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("�\_�����e�͈͊O��")) Then
                    typData.strOutSideN_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�\_�����e�͈͊O��")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("�\_�G�b�W���o���s��_��")) Then
                    typData.strNgN_Left_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�\_�G�b�W���o���s��_��")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("�\_�G�b�W���o���s��_�E")) Then
                    typData.strNgN_Right_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("�\_�G�b�W���o���s��_�E")))
                End If

                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("��_���ŏ�")) Then
                    typData.strMinItaHaba_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("��_���ŏ�")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("��_���ő�")) Then
                    typData.strMaxItaHaba_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("��_���ő�")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("��_�����e�͈͊O��")) Then
                    typData.strOutSideN_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("��_�����e�͈͊O��")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("��_�G�b�W���o���s��_��")) Then
                    typData.strNgN_Left_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("��_�G�b�W���o���s��_��")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("��_�G�b�W���o���s��_�E")) Then
                    typData.strNgN_Right_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("��_�G�b�W���o���s��_�E")))
                End If

                '���X�g�֒ǉ�
                lstData.Add(typData)

            End While

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�G�b�W���o���ю擾�G���[ [" & ex.Message & "]")
            Return False
        Finally

            tcls_DB.Cancel()
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing

        End Try

    End Function

    ''' <summary>
    ''' CSV�փf�[�^�Z�b�g
    ''' </summary>
    ''' <param name="strPath">�p�X</param>
    ''' <param name="lstData">���X�g</param>
    ''' <returns>���A���(Ture:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function SetData(ByVal strPath As String, ByVal lstData As List(Of EDGE_RESULT_INF)) As Boolean
        Dim strData As String = ""                      '1�s�̃f�[�^

        '' CSV�o��
        Using cls_Stram As New tClass.tClass_FileStream(strPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' �I�[�v��
                cls_Stram.Open()

                strData = ""
                '�w�b�_�[�o��
                strData &= "�Ǘ�No,"
                strData &= "�R�C���ԍ�,"
                strData &= "�o�^���t,"

                strData &= "�\_���ŏ�,"
                strData &= "�\_���ő�,"
                strData &= "�\_�����e�͈͊O��,"
                strData &= "�\_�G�b�W���o���s��_��,"
                strData &= "�\_�G�b�W���o���s��_�E,"

                strData &= "��_���ŏ�,"
                strData &= "��_���ő�,"
                strData &= "��_�����e�͈͊O��,"
                strData &= "��_�G�b�W���o���s��_��,"
                strData &= "��_�G�b�W���o���s��_�E,"

                '�Ō��","��r��
                'strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)


                '�f�[�^�o��
                For ii As Integer = 0 To lstData.Count - 1
                    strData = ""
                    strData &= CStr(lstData(ii).strKizuKenNO) & ","
                    strData &= CStr(lstData(ii).strCoilNo) & ","
                    strData &= CStr(lstData(ii).strInDate) & ","

                    strData &= CStr(lstData(ii).strMinItaHaba_O) & ","
                    strData &= CStr(lstData(ii).strMaxItaHaba_O) & ","
                    strData &= CStr(lstData(ii).strOutSideN_O) & ","
                    strData &= CStr(lstData(ii).strNgN_Left_O) & ","
                    strData &= CStr(lstData(ii).strNgN_Right_O) & ","

                    strData &= CStr(lstData(ii).strMinItaHaba_U) & ","
                    strData &= CStr(lstData(ii).strMaxItaHaba_U) & ","
                    strData &= CStr(lstData(ii).strOutSideN_U) & ","
                    strData &= CStr(lstData(ii).strNgN_Left_U) & ","
                    strData &= CStr(lstData(ii).strNgN_Right_U) & ","

                    cls_Stram.WriteLine(strData)
                Next ii

                ' �N���[�Y
                cls_Stram.Close()

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "CSV�f�[�^�Z�b�g�G���[ [" & ex.Message & "]")
                Return False
            End Try
        End Using

        Return True

    End Function

End Class
