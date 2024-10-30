'*********************************************************************************
'�@�O�H���p�����[�^���Ǘ��N���X
'	[Ver]
'		Ver.01    2012/09/01  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports System.Collections
Imports tClassLibrary
Imports tClassLibrary.tClass
Public Class clsPreParamManager

    Implements IDisposable                          ' �f�X�g���N�^��`
    '---------------------------------------------------------------------------
    ' �^��`
    '---------------------------------------------------------------------------
    Public Structure PRE_MASTER_DATA
        Dim nJudge As Integer                                               ' �L������
        Dim strPreName As String                                            ' ����
        Dim nPreDspMode As Integer                                          ' �O�H���\�����[�h
        Dim nImgTyp As Integer                                              ' �摜�`��(0�FJPEG    1�FBMP)
        Dim nOther As Integer                                               ' ������(0:����,1:����)
    End Structure

    ''' <summary>
    ''' �r��f�[�^
    ''' </summary>
    ''' <remarks>�O�H���r��}�X�^�[�e�[�u������K�v�ȃf�[�^�����ێ�</remarks>
    Private Structure PRE_TYPE_NAME_DATA
        Dim sFFname As String                                               ' FFName
        Dim nID As Integer                                                  ' ID
        Dim strName As String                                               ' Name
        Dim nPriority As Integer                                            ' �D��x
        Dim strMapName As String                                            ' �}�b�v�\������
        Dim nVisible As Integer                                             ' �\���L��
    End Structure

    ''' <summary>
    ''' �O���[�h�f�[�^
    ''' </summary>
    ''' <remarks>�O���[�h�}�X�^�[�e�[�u������K�v�ȃf�[�^�����ێ�</remarks>
    Private Structure PRE_GRADE_NAME_DATA
        Dim sFFname As String                                               ' FFName
        Dim nID As Integer                                                  ' �O���[�hID
        Dim strName As String                                               ' Name
        Dim nPriority As Integer                                            ' �D��x
        Dim nMapColor As Integer                                            ' �}�b�v�\���F
    End Structure



    '�O�H���r��L�[�\����
    Public Structure PreTypeNameKey
        Dim strProc As String           '�O�H���R�[�h
        Dim strFFTypeName As String     'FF����
    End Structure

    ''�O�H���r��L�[�\����
    'Public Structure PreTypeNameKey2
    '	Dim strProc As String			'�O�H���R�[�h
    '	Dim nTid As Integer				'�r��
    'End Structure

	'�O�H���O���[�h�L�[�\����
	Public Structure PreGradeNameKey
		Dim strProc As String			'�O�H���R�[�h
		Dim strFFGradeName As String	'FF����
	End Structure

    '�O�H���O���[�h�L�[�\����
    Public Structure PreGradeNameKey2
        Dim strProc As String           '�O�H���R�[�h
        Dim nGid As Integer             '�O���[�h
    End Structure

	'---------------------------------------------------------------------------
	' �����o�ϐ�
	'---------------------------------------------------------------------------
    Private m_bIsDesposed As Boolean = False                                    ' ���\�[�X�J���ς݃t���E�O True:�J���ς�
    Private mcls_Log As tClass_LogManager                                       ' ���O�Ǘ�

    Private m_dctPreMaster As Dictionary(Of String, PRE_MASTER_DATA)                    ' �O�H���}�X�^�[��� (�L�[�͑O�H��)
    Private m_dctPreType As Dictionary(Of PreTypeNameKey, PRE_TYPE_NAME_DATA)           ' �O�H���r��f�[�^�ꗗ (�L�[�͑O�H���r��L�[�\����)
    Private m_dctPreGrade As Dictionary(Of PreGradeNameKey, PRE_GRADE_NAME_DATA)        ' �O�H���O���[�h�f�[�^�ꗗ (�L�[�͑O�H���O���[�h�L�[�\����)

    'Private m_dctPreType2 As Dictionary(Of PreTypeNameKey2, PRE_TYPE_NAME_DATA)			' �O�H���r��f�[�^�ꗗ (�L�[�͑O�H���r��L�[�\����2)
    Private m_dctPreGrade2 As Dictionary(Of PreGradeNameKey2, PRE_GRADE_NAME_DATA)      ' �O�H���O���[�h�f�[�^�ꗗ (�L�[�͑O�H���O���[�h�L�[�\����2)

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="log">���O�N���X</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef log As tClass.tClass_LogManager)

        '���O�N���X
        mcls_Log = log                                                          ' ���O�}�l�[�W���ݒ�
        m_dctPreMaster = New Dictionary(Of String, PRE_MASTER_DATA)
		m_dctPreType = New Dictionary(Of PreTypeNameKey, PRE_TYPE_NAME_DATA)
		m_dctPreGrade = New Dictionary(Of PreGradeNameKey, PRE_GRADE_NAME_DATA)
        'm_dctPreType2 = New Dictionary(Of PreTypeNameKey2, PRE_TYPE_NAME_DATA)
        m_dctPreGrade2 = New Dictionary(Of PreGradeNameKey2, PRE_GRADE_NAME_DATA)

    End Sub


    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If m_bIsDesposed = False Then
            m_bIsDesposed = True                                                ' Dispose�ς݂Ƃ���

            m_dctPreMaster.Clear()
            m_dctPreMaster = Nothing
			m_dctPreType.Clear()
			m_dctPreType = Nothing
			m_dctPreGrade.Clear()
			m_dctPreGrade = Nothing
            'm_dctPreType2.Clear()
            'm_dctPreType2 = Nothing
            m_dctPreGrade2.Clear()
            m_dctPreGrade2 = Nothing
		End If
    End Sub

#Region " IDisposable Support "
    ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////


    ''' <summary>
    ''' �S�ēǂݍ���
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG</returns>
    ''' <remarks></remarks>
    Public Function ReadAll() As Boolean

        ' �r���萔�ǂݍ���
        If Not ReadPreMaster() Then
            Return False
        End If

        '�O�H���r��e�[�u���ǂݍ���
        If Not ReadPreTypeName() Then
            Return False
        End If

        '�O�H���O���[�h�e�[�u���ǂݍ���
        If Not ReadPreGradeName() Then
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �r���萔�擾
    ''' </summary>
    ''' <returns>���s���� True:���� False:���s</returns>
    ''' <remarks></remarks>
    Public Function ReadPreMaster() As Boolean
        Dim bRet As Boolean = True

        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            '-------------------------------------------------------------------
            ' SQL�쐬
            '-------------------------------------------------------------------
            strSql = ""
            strSql &= " SELECT �O�H���R�[�h"
            strSql &= "        ,�L������"
            strSql &= "        ,����"
            strSql &= "        ,�O�H���\�����[�h"
            strSql &= "        ,�摜�`��"
            strSql &= "        ,�����ޔ���"
            strSql &= " FROM " & DB_MAE_MASTER
            strSql &= " WHERE �L������ = 1"

            '-------------------------------------------------------------------
            ' SQL���s
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = tcls_DB.Open()                               ' DB�ڑ�
            If dbret = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "DB�ڑ��Ɏ��s")
                bRet = False
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSql)                             ' SQL���s
            If sqlRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "�O�H���}�X�^�[�擾SQL�ُ� sql=" & strSql)
                bRet = False
                Exit Try
            End If

            m_dctPreMaster.Clear()
            While sqlRead.Read() = True
                Dim master As New PRE_MASTER_DATA
                Dim strProc As String           '�O�H���R�[�h
                With master
                    strProc = CStr(sqlRead.GetValue(0))                     ' �O�H���R�[�h
                    .nJudge = CInt(sqlRead.GetValue(1))                     ' �L������
                    .strPreName = CStr(sqlRead.GetValue(2))                 ' �O�H���R�[�h
                    .nPreDspMode = CInt(sqlRead.GetValue(3))                ' �O�H���\�����[�h
                    .nImgTyp = CInt(sqlRead.GetValue(4))                    ' �摜�`��
                    .nOther = CInt(sqlRead.GetValue(5))                     ' �����ޔ���
                End With
                m_dctPreMaster.Add(strProc, master)
            End While

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("{0}�擾�Ɏ��s���܂����B[{1}]", DB_MAE_MASTER, ex.Message))
            bRet = False
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DB�ɐڑ�����������ꍇ
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �O�H���r��}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadPreTypeName() As Boolean
        Dim bRet As Boolean = True

        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
		Dim typPreTypeKey As PreTypeNameKey

        Try
            '-------------------------------------------------------------------
            ' SQL�쐬
            '-------------------------------------------------------------------
            strSql = ""
			strSql &= " SELECT "
			strSql &= " �O�H���R�[�h, "
			strSql &= " FFName, "
			strSql &= " ID, "
			strSql &= " Name, "
			strSql &= " �D��x, "
			strSql &= " �}�b�v�\������, "
			strSql &= " �\���L�� "
			strSql &= " FROM " & DB_MAE_TYPE_NAME
            strSql &= " ORDER BY �O�H���R�[�h ASC "

            '-------------------------------------------------------------------
            ' SQL���s
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = tcls_DB.Open()                               ' DB�ڑ�
            If dbret = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "DB�ڑ��Ɏ��s")
                bRet = False
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSql)                             ' SQL���s
            If sqlRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "�O�H���r��}�X�^�[�擾SQL�ُ� sql=" & strSql)
                bRet = False
                Exit Try
            End If

            m_dctPreType.Clear()
            While sqlRead.Read() = True

                '�L�[���Z�b�g
				typPreTypeKey.strProc = sqlRead.GetString(sqlRead.GetOrdinal("�O�H���R�[�h")).Trim
				typPreTypeKey.strFFTypeName = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim

				Dim type As New PRE_TYPE_NAME_DATA
                With type
                    .sFFname = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim
                    .nID = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("ID")))
                    .strName = sqlRead.GetString(sqlRead.GetOrdinal("Name")).Trim
                    .nPriority = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�D��x")))
                    .strMapName = sqlRead.GetString(sqlRead.GetOrdinal("�}�b�v�\������")).Trim
                    .nVisible = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�\���L��")))
                End With
                m_dctPreType.Add(typPreTypeKey, type)
            End While
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("{0}�擾�Ɏ��s���܂����B[{1}]", DB_MAE_TYPE_NAME, ex.Message))
            bRet = False
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DB�ɐڑ�����������ꍇ
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet
    End Function
    ''' <summary>
    ''' �O�H���O���[�h�}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadPreGradeName() As Boolean
        Dim bRet As Boolean = True

        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim typPreGradeKey As PreGradeNameKey
        Dim typPreGradeKey2 As PreGradeNameKey2

        Try
            '-------------------------------------------------------------------
            ' SQL�쐬
            '-------------------------------------------------------------------
            strSql = ""
			strSql &= " SELECT "
			strSql &= " �O�H���R�[�h, "
			strSql &= " FFName, "
			strSql &= " �O���[�hID, "
			strSql &= " Name, "
			strSql &= " �D��x, "
			strSql &= " �}�b�v�\���F "
            strSql &= " FROM " & DB_MAE_GRADE_NAME
            'strSql &= " ORDER BY �O�H���R�[�h ASC "
            strSql &= " ORDER BY �O�H���R�[�h ASC, �D��x ASC "

            '-------------------------------------------------------------------
            ' SQL���s
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = tcls_DB.Open()                               ' DB�ڑ�
            If dbret = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "DB�ڑ��Ɏ��s")
                bRet = False
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSql)                             ' SQL���s
            If sqlRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "�O�H���O���[�h�}�X�^�[�擾SQL�ُ� sql=" & strSql)
                bRet = False
                Exit Try
            End If

            m_dctPreGrade.Clear()
            m_dctPreGrade2.Clear()
            While sqlRead.Read() = True

                '�L�[���Z�b�g
                typPreGradeKey.strProc = sqlRead.GetString(sqlRead.GetOrdinal("�O�H���R�[�h")).Trim
                typPreGradeKey.strFFGradeName = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim

                typPreGradeKey2.strProc = sqlRead.GetString(sqlRead.GetOrdinal("�O�H���R�[�h")).Trim
                typPreGradeKey2.nGid = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�O���[�hID")))

                Dim gread As New PRE_GRADE_NAME_DATA
                With gread
                    .sFFname = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim
                    .nID = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�O���[�hID")))
                    .strName = sqlRead.GetString(sqlRead.GetOrdinal("Name")).Trim
                    .nPriority = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�D��x")))
                    .nMapColor = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�}�b�v�\���F")))
                End With
                m_dctPreGrade.Add(typPreGradeKey, gread)
                m_dctPreGrade2.Add(typPreGradeKey2, gread)
            End While
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("{0}�擾�Ɏ��s���܂����B[{1}]", DB_MAE_GRADE_NAME, ex.Message))
            bRet = False
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DB�ɐڑ�����������ꍇ
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �O�H���R�[�h�L������擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <returns>�Y�����ڂ̗L�������B  �i0�F����  1�F�L���j</returns>
    ''' <remarks></remarks>
    Public Function GetProcJudge(ByVal strProc As String) As Integer

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            Return 0
        End If

        Return m_dctPreMaster(strProc).nJudge
    End Function

    ''' <summary>
    ''' �O�H�����̎擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <returns>�O�H������</returns>
    ''' <remarks></remarks>
    Public Function GetProcName(ByVal strProc As String) As String

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            Return ""
        End If

        Return m_dctPreMaster(strProc).strPreName
    End Function

    ''' <summary>
    ''' �O�H���R�[�h�\�����[�h�擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <returns>0�F�{�V�X�e���Ŗ��Ǘ��A1�F�O�H���r�����擾 �� �r�摜�\���A2�FFF���̂݉�ʕ\��</returns>
    ''' <remarks></remarks>
    Public Function GetProcDspMode(ByVal strProc As String) As Integer

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            Return 0
        End If

        Return m_dctPreMaster(strProc).nPreDspMode
    End Function

    ''' <summary>
    ''' �摜�`���擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <returns>0�FJPEG    1�FBMP</returns>
    ''' <remarks></remarks>
    Public Function GetProcImgTyp(ByVal strProc As String) As Integer

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            Return 0
        End If

        Return m_dctPreMaster(strProc).nImgTyp
    End Function



    ''' <summary>
    ''' �r�햼���擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    '''<param name="strFFName">FFName</param>
    ''' <returns>�r�햼</returns>
    ''' <remarks></remarks>
    Public Function GetDspTypeName(ByVal strProc As String, ByVal strFFName As String) As String

        Dim typKey As PreTypeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFName.Trim

        If False = m_dctPreType.ContainsKey(typKey) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
            '�r�햼���o�^���͕s���Ƃ���
            'Return ""
            Return "�s��"
            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
        End If

        Return m_dctPreType(typKey).strName
    End Function

    ''' <summary>
    ''' �}�b�v�\���r�햼���擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>�}�b�v�\���r�햼</returns>
    ''' <remarks></remarks>
    Public Function GetDspMapName(ByVal strProc As String, ByVal strFFName As String) As String

        Dim typKey As PreTypeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFName.Trim

        If False = m_dctPreType.ContainsKey(typKey) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
            '�r�햼���o�^���͕s���Ƃ���
            'Return ""
            Return "�s��"
            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
        End If

        Return m_dctPreType(typKey).strMapName
    End Function
    ''' <summary>
    ''' �r��D��x�擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>�D��x</returns>
    ''' <remarks></remarks>
    Public Function GetTypePriority(ByVal strProc As String, ByVal strFFName As String) As Integer

        Dim typKey As PreTypeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFName.Trim

        If False = m_dctPreType.ContainsKey(typKey) Then                        ' �Ώ�KEY���Ȃ��ꍇ
            Return 999
        End If

        Return m_dctPreType(typKey).nPriority
    End Function

    '''' <summary>
    '''' �r��D��x�擾
    '''' </summary>
    ''''<param name="strProc">�O�H���R�[�h</param>
    '''' <param name="nTid">�r��</param>
    '''' <returns>�D��x</returns>
    '''' <remarks></remarks>
    'Public Function GetTypePriority2(ByVal strProc As String, ByVal nTid As Integer) As Integer

    '	Dim typKey As PreTypeNameKey2
    '	typKey.strProc = strProc.Trim
    '	typKey.nTid = nTid

    '	If False = m_dctPreType2.ContainsKey(typKey) Then						' �Ώ�KEY���Ȃ��ꍇ
    '		Return 999
    '	End If
    '	Return m_dctPreType2(typKey).nPriority

    'End Function

    ''' <summary>
    ''' �}�b�v�\���F���擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>�}�b�v�\���F</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeColor(ByVal strProc As String, ByVal strFFName As String) As Integer

        Dim typKey As PreGradeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFGradeName = strFFName.Trim

        If False = m_dctPreGrade.ContainsKey(typKey) Then
            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
            '�O���[�h�����o�^���͔w�i�F�𔒐F�Ƃ���
            'Return 0
            Return Color.White.ToArgb
            '2014.08.15 �쌴    ���ԘA�g    <<<<<<<<
        End If

        Return m_dctPreGrade(typKey).nMapColor
    End Function
    ''' <summary>
    ''' �O���[�h���̎擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>�O���[�h��</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeName(ByVal strProc As String, ByVal strFFName As String) As String

        Dim typKey As PreGradeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFGradeName = strFFName.Trim

        If False = m_dctPreGrade.ContainsKey(typKey) Then
            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
            '�O���[�h�����o�^���͕s���Ƃ���
            'Return ""
            Return "�s��"
            '2014.08.15 �쌴    ���ԘA�g    <<<<<<<<
        End If

        Return m_dctPreGrade(typKey).strName

    End Function
    ''' <summary>
    ''' �O���[�h�D��x�擾
    ''' </summary>
    '''<param name="strProc">�O�H���R�[�h</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetGradePriority(ByVal strProc As String, ByVal strFFName As String) As Integer

        Dim typKey As PreGradeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFGradeName = strFFName.Trim

        If False = m_dctPreGrade.ContainsKey(typKey) Then
            Return 999
        End If

        Return m_dctPreGrade(typKey).nPriority

    End Function
    '''' <summary>
    '''' �O���[�h�D��x�擾
    '''' </summary>
    ''''<param name="strProc">�O�H���R�[�h</param>
    '''' <param name="nGid">�O���[�h</param>
    '''' <returns></returns>
    '''' <remarks></remarks>
    'Public Function GetGradePriority2(ByVal strProc As String, ByVal nGid As Integer) As Integer

    '	Dim typKey As PreGradeNameKey2
    '	typKey.strProc = strProc.Trim
    '	typKey.nGid = nGid

    '	If False = m_dctPreGrade2.ContainsKey(typKey) Then
    '		Return 999
    '	End If

    '	Return m_dctPreGrade2(typKey).nPriority

    'End Function

    ''' <summary>
    ''' �D��x��r
    ''' </summary>
    ''' <param name="strPreProc">�O��O�H��</param>
    ''' <param name="strNowProc">����O�H��</param>
    ''' <param name="strPreFFTypeName">�O��FF�r��</param>
    ''' <param name="strNowFFTypeName">����FF�r��</param>
    ''' <param name="strPreFFGradeName">�O��FF�O���[�h</param>
    ''' <param name="strNowFFGradeName">����FF�O���[�h</param>
    ''' <returns>���A���(True:�D��x������,False:�D��x���Ⴂ)</returns>
    ''' <remarks></remarks>
    Public Function CheckPriority(ByVal strPreProc As String, ByVal strNowProc As String, ByVal strPreFFTypeName As String, ByVal strNowFFTypeName As String, _
                                    ByVal strPreFFGradeName As String, ByVal strNowFFGradeName As String) As Boolean



        '�O��̃O���[�h�����D��x����������΁A�D��x������
        If GetGradePriority(strPreProc, strPreFFGradeName) > GetGradePriority(strNowProc, strNowFFGradeName) Then

            Return True

        ElseIf GetGradePriority(strPreProc, strPreFFGradeName) = GetGradePriority(strNowProc, strNowFFGradeName) Then

            '�O���[�h�̗D��x�������Ȃ�A�r��̗D��x���r���A�D��x����������΁A�D��x������
            If GetTypePriority(strPreProc, strPreFFTypeName) > GetTypePriority(strNowProc, strNowFFTypeName) Then
                Return True
            End If

        End If

        Return False

    End Function

    '''' <summary>
    '''' �D��x��r
    '''' </summary>
    '''' <param name="strPreProc">�O��O�H��</param>
    '''' <param name="strNowProc">����O�H��</param>
    '''' <param name="nPreTid">�O��FF�r��</param>
    '''' <param name="nNowTid">����FF�r��</param>
    '''' <param name="strPreGid">�O��FF�O���[�h</param>
    '''' <param name="strNowGid">����FF�O���[�h</param>
    '''' <returns>���A���(True:�D��x������,False:�D��x���Ⴂ)</returns>
    '''' <remarks></remarks>
    'Public Function CheckPriority2(ByVal strPreProc As String, ByVal strNowProc As String, ByVal nPreTid As Integer, ByVal nNowTid As Integer, _
    '  ByVal strPreGid As Integer, ByVal strNowGid As Integer) As Boolean



    '	'�O��̃O���[�h�����D��x����������΁A�D��x������
    '	If GetGradePriority2(strPreProc, strPreGid) > GetGradePriority2(strNowProc, strNowGid) Then

    '		Return True

    '	ElseIf GetGradePriority2(strPreProc, strPreGid) = GetGradePriority2(strNowProc, strNowGid) Then

    '		'�O���[�h�̗D��x�������Ȃ�A�r��̗D��x���r���A�D��x����������΁A�D��x������
    '		If GetTypePriority2(strPreProc, nPreTid) > GetTypePriority2(strNowProc, nNowTid) Then
    '			Return True
    '		End If

    '	End If

    '	Return False

    'End Function
    ''' <summary>
    ''' �r�̗D���r���s���܂�
    ''' </summary>
    ''' <returns>-1:�r1���D��x�� 0:�����D��x 1:�r2���D��x��</returns>
    ''' <remarks></remarks>
    Public Function ComparePriority(ByVal strPreProc As String, ByVal strNowProc As String, ByVal strPreFFTypeName As String, ByVal strNowFFTypeName As String, _
                                    ByVal strPreFFGradeName As String, ByVal strNowFFGradeName As String) As Integer
        Dim nPri1 As Integer = GetGradePriority(strPreProc, strPreFFGradeName)
        Dim nPri2 As Integer = GetGradePriority(strNowProc, strNowFFGradeName)

        If nPri1 > nPri2 Then                                                   ' �r2�̃O���[�h�D��x������
            Return 1                                                            ' �r2���D��

        ElseIf nPri1 < nPri2 Then                                               ' �r1�̃O���[�h�D��x������
            Return -1                                                           ' �r1���D��

        Else                                                                    ' �O���[�h�D��x��������
            nPri1 = GetTypePriority(strPreProc, strPreFFTypeName)
            nPri2 = GetTypePriority(strNowProc, strNowFFTypeName)
            If nPri1 > nPri2 Then                                               ' �r2���r��D��x������
                Return 1                                                        ' �r2���D��

            ElseIf nPri1 < nPri2 Then                                           ' �r1���r��D��x������
                Return -1                                                       ' �r1���D��
            End If
        End If

        Return 0                                                                ' ����r�͓����D��x
    End Function

    ''' <summary>
    ''' �O�H���\���L���`�F�b�N
    ''' </summary>
    ''' <param name="strProc">�O�H��</param>
    ''' <param name="strFFType">FF�r��</param>
    ''' <param name="strFFGrade">FF�O���[�h</param>
    ''' <returns>True:�\������,False:�\�����Ȃ�</returns>
    ''' <remarks></remarks>
    Public Function CheckView(ByVal strProc As String, ByVal strFFType As String, ByVal strFFGrade As String) As Boolean
        Dim typKey As PreTypeNameKey
        Dim greKey As PreGradeNameKey
        Dim greKey2 As PreGradeNameKey2
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFType.Trim
        greKey.strProc = strProc.Trim
        greKey.strFFGradeName = strFFGrade.Trim


		'HACK 2015/04/07 ��{�I�ɂ͋��L�������ɑ��݂���f�[�^�͑S�ĕ\������̂ŁA�K��True�ŕԂ��悤�ɂ���
		Return True

        If Not m_dctPreType.ContainsKey(typKey) Then

            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
            '�r�햼�����o�^���ł��\���ΏۂƂ���
            Return True

            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
        End If

        If Not m_dctPreGrade.ContainsKey(greKey) Then

            '2014.08.15 �쌴    ���ԘA�g    >>>>>>>>
            '�O���[�h�������o�^���ł��\���ΏۂƂ���
            Return True
            '2014.08.15 �쌴    ���ԘA�g    <<<<<<<<
        End If

        greKey2.strProc = strProc
        greKey2.nGid = m_dctPreType(typKey).nVisible

        If Not m_dctPreGrade2.ContainsKey(greKey2) Then
            Return False
        End If

        If m_dctPreGrade2(greKey2).nPriority >= m_dctPreGrade(greKey).nPriority And _
        m_dctPreGrade(greKey).nPriority > 0 Then
            Return True
        End If

        Return False

    End Function

End Class
