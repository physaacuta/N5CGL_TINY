'*********************************************************************************
'�@�p�����[�^���Ǘ��N���X
'	[Ver]
'		Ver.01    2007/08/03  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports System.Collections
Imports tClassLibrary

Public Class clsParamManager

    Implements IDisposable                          ' �f�X�g���N�^��`

    Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�
    Private m_dctDspTypeName As New Dictionary(Of Integer, String)          '�r�햼
    Private m_dctDspMapName As New Dictionary(Of Integer, String)           '�}�b�v�\���r�햼
    Private m_dctTypePriority As New Dictionary(Of Integer, Integer)        '�r��D��x
    Private m_dctDspGradeColor As New Dictionary(Of Integer, Integer)       '�O���[�h�F
    Private m_dctDspGradeName As New Dictionary(Of Integer, String)         '�O���[�h��
    Private m_dctGradePriority As New Dictionary(Of Integer, Integer)       '�O���[�h�D��x
    Private m_dctDefectGrade As New Dictionary(Of Integer, Integer)         '���׃O���[�h
    Private m_dctMapSymbol As New Dictionary(Of Integer, Integer)           '�r�}�b�v�V���{��
    Private m_dctMapSymbolColor As New Dictionary(Of Integer, Integer)      '�r�}�b�v�V���{���\���F
    Private m_Pout As PARAM_OUTPUT    '�p�����[�^�ݒ��ʏ��ۑ��p
    Private mcls_Log As tClass.tClass_LogManager           ' ���O�Ǘ��N���X
    Private m_pLockObject As Object                 ' SyncLock�����I�u�W�F�N�g

    '//==========================================================
    '// �����p�^�[���ʃp�����[�^�ݒ�\���́i��ʕ\���Ώ۔���p�j
    '//==========================================================
    '//�r�했�̕\�����\����
    Private Structure PARAM_OUTPUT_TYPE
        Dim edasysID As Integer
        Dim disp As Integer
        Public Sub initialize()
            edasysID = 0
            disp = 0
        End Sub
    End Structure

    '//�p�^�[���R�[�h���̏��\����
    Private Structure PARAM_OUTPUT_PCODE
        Dim num As Integer
        Dim pcode As Integer
        Dim type() As PARAM_OUTPUT_TYPE
        Dim def As PARAM_OUTPUT_TYPE
        Public Sub initialize()
            ReDim type(MAX_TYPE - 1)
            num = 0
            pcode = 0
        End Sub
    End Structure

    '//�����p�^�[���ʃp�����[�^���\����
    Private Structure PARAM_OUTPUT
        Dim pcode() As PARAM_OUTPUT_PCODE
        Public Sub initialize()
            ReDim pcode(MAX_PCODE - 1)
        End Sub
    End Structure

    '********************************************************************
    '�萔��`
    '********************************************************************

    '====================================================================
    '	�r�_�}�b�v�\����ʁi�U�z�}�j�p���r�V���{����`
    ' 0�F�E  1:��  2:��  3:��  4:�� 5:��  6:��  7:��  8:�~  9:�{
    '====================================================================
    ''// �r�}�b�v�\����ʁi�U�z�}�j�p���r�V���{����`
    Private PLOT_SYMBOL_TYPE() As String = { _
     "�E", "��", "��", "��", "��", "��", "��", "��", "�~", "�{"}             ' �r�V���{����`


    '********************************************************************
    '�v���p�e�B�[��`
    '********************************************************************

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClass.tClass_LogManager)

        '���O�N���X
        mcls_Log = clsLog

        m_pLockObject = New Object

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
            End If

            ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
            m_dctDspTypeName = Nothing
            m_dctDspMapName = Nothing
            m_dctTypePriority = Nothing
            m_dctDspGradeColor = Nothing
            m_dctDspGradeName = Nothing
            m_dctGradePriority = Nothing
            m_dctDefectGrade = Nothing
            m_pLockObject = Nothing

            m_dctMapSymbol = Nothing
            m_dctMapSymbolColor = Nothing

        End If
        Me.disposedValue = True
    End Sub
#Region " IDisposable Support "
    ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' �S�ēǂݍ���
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG</returns>
    ''' <remarks></remarks>
    Public Function ReadAll() As Boolean

        '�r��e�[�u���ǂݍ���
        If Not ReadTypeName() Then Return False

        '�O���[�h�e�[�u���ǂݍ���
        If Not ReadGradeName() Then Return False

        '�����p�^�[���ʏo�͐ݒ�e�[�u��
        If Not ReadParam_OutPut() Then Return False

        Return True

    End Function

    ''' <summary>
    ''' �r��}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadTypeName() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim bRet As Boolean = False

        'SQL���쐬
        strSql = ""
        strSql &= " select EdasysID"
        strSql &= "        ,�}�b�v�\������"
        strSql &= "        ,Name"
        strSql &= "        ,�D��x"
        strSql &= "        ,�r�}�b�v�V���{��"
        strSql &= " from " & DB_TYPE_NAME
        strSql &= " order by ID ASC "

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Exit Function

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            SyncLock m_pLockObject

                m_dctDspTypeName.Clear()
                m_dctDspMapName.Clear()
                m_dctTypePriority.Clear()
                m_dctMapSymbol.Clear()

                While sqlRead.Read()

                    '�}�b�v�\�����̃Z�b�g
                    m_dctDspMapName.Add(CInt(sqlRead.GetValue(0)), sqlRead.GetString(1))

                    '�r�햼�Z�b�g
                    m_dctDspTypeName.Add(CInt(sqlRead.GetValue(0)), sqlRead.GetString(2))

                    '�D��x�Z�b�g
                    m_dctTypePriority.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(3)))

                    '�r�}�b�v�V���{���Z�b�g
                    m_dctMapSymbol.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(4)))

                End While

            End SyncLock

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "TYPE_NAME�擾���s")
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �O���[�h�}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadGradeName() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim bRet As Boolean = False

        'SQL���쐬
        strSql = ""
        strSql &= " select EdasysID"
        strSql &= "        ,�}�b�v�\���F"
        strSql &= "        ,Name"
        strSql &= "        ,�D��x"
        strSql &= "        ,���׃O���[�h"
        strSql &= "        ,�r�}�b�v�V���{���\���F"
        strSql &= " from " & DB_GRADE_NAME
        strSql &= " order by ID ASC "

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Exit Function

            ' SQL���s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            SyncLock m_pLockObject

                m_dctDspGradeColor.Clear()
                m_dctDspGradeName.Clear()
                m_dctGradePriority.Clear()
                m_dctDefectGrade.Clear()
                m_dctMapSymbolColor.Clear()

                While sqlRead.Read()

                    '�}�b�v�\���F�Z�b�g
                    m_dctDspGradeColor.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(1)))

                    '�O���[�h��
                    m_dctDspGradeName.Add(CInt(sqlRead.GetValue(0)), sqlRead.GetString(2))

                    '�O���[�h�r����
                    m_dctGradePriority.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(3)))

                    '���׃O���[�h
                    m_dctDefectGrade.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(4)))

                    '�r�}�b�v�V���{���\���F�Z�b�g
                    m_dctMapSymbolColor.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(5)))

                End While

            End SyncLock

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "GRADE_NAME�擾���s")
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

        Return bRet


    End Function

    ''' <summary>
    ''' �����p�^�[���ʏo�͐ݒ�p�����[�^�Ǎ��iHY_DEFMAP�p�j
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_OutPut() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim bRet As Boolean = False
        Dim pcode As String = ""             'pcode
        Dim o_pcode As String = ""           '������pcode
        Dim cd As Integer = 0                'pcode(�Y�����ϊ���)
        Dim cnt As Integer = 0               'pcode�����r��EdasysID���i�Y�����p�j

        Dim ii As Integer
        Dim jj As Integer
        Dim id As Integer = 0                '�r��EdasysID


        'SQL���쐬
        strSql = ""
        strSql &= " SELECT pcode, �r��EdasysID, �����P���r�_�}�b�v��ʕ\�� "
        strSql &= " from " & DB_PCODE_OUTPUT
        strSql &= " ORDER BY pcode, �r��EdasysID "

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Exit Function

            ' SQL���s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            SyncLock m_pLockObject

                '�ۑ��p�\���̏�����
                m_Pout.initialize()
                For ii = 0 To MAX_PCODE - 1
                    m_Pout.pcode(ii).initialize()
                    For jj = 0 To MAX_TYPE - 1
                        m_Pout.pcode(ii).type(jj).initialize()
                    Next jj
                    m_Pout.pcode(ii).def.initialize()
                Next ii

                While sqlRead.Read()

                    'PCODE�ǂݍ���
                    pcode = CStr(sqlRead.GetValue(0))
                    If pcode <> o_pcode Then    'PCODE���O��l�ƈقȂ�ꍇ
                        cd = ChangePcode(pcode) 'PCODE�̒l��Y�����ϊ�
                        If -1 = cd Then Continue While '-1�̏ꍇ�͎��̃��R�[�h��

                        'PCODE�i�Y�����j��ۑ�
                        m_Pout.pcode(cd).pcode = cd

                        'PCODE�l��ۑ�
                        o_pcode = pcode

                    End If

                    '�r��EdasysID�ǂݍ���
                    id = CInt(sqlRead.GetValue(1))
                    If id = -1 Then     '�r��EdasysID��-1�̏ꍇ�̓f�t�H���g�ɕۑ��iID���r�}�b�v�\���j
                        m_Pout.pcode(cd).def.edasysID = id
                        m_Pout.pcode(cd).def.disp = CInt(sqlRead.GetValue(2))
                    Else                '�ʏ�ۑ��iID���r�}�b�v�\���j
                        '�r�했�̌����擾
                        cnt = m_Pout.pcode(cd).num
                        m_Pout.pcode(cd).type(cnt).edasysID = id
                        m_Pout.pcode(cd).type(cnt).disp = CInt(sqlRead.GetValue(2))
                        m_Pout.pcode(cd).num = cnt + 1
                    End If

                End While

            End SyncLock

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "M_PCODE_OUTPUT�擾���s")
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

        Return bRet


    End Function

    ''' <summary>
    ''' �r�햼���擾
    ''' </summary>
    ''' <param name="pcode">�p�^�[���R�[�h</param>
    ''' <returns>�r�햼</returns>
    ''' <remarks></remarks>
    Private Function ChangePcode(ByVal pcode As String) As Integer

        '�p�����[�^�`�F�b�N
        If 1 > Asc(pcode) Or MAX_PCODE < Asc(pcode) Then
            Return -1
        End If

        Return Asc(pcode) - 1

    End Function

    ''' <summary>
    ''' ��ʕ\���Ώۂ��r�킩����
    ''' </summary>
    ''' <param name="pcode">�p�^�[���R�[�h</param>
    ''' <param name="tid">�r��EdasysID</param>
    ''' <param name="gid">�O���[�hEdasysID</param>
    ''' <returns>�r�햼</returns>
    ''' <remarks></remarks>
    Public Function Check_Output_DISP(ByVal pcode As String, ByVal tid As Integer, _
                                        ByVal gid As Integer) As Boolean

        Dim ii As Integer
        Dim pcd As Integer = 0      'PCODE�̓Y�����ϊ��p

        'PCODE�Y�����ϊ�
        pcd = ChangePcode(pcode) 'PCODE�̒l��Y�����ϊ�

        '�r���v���m�F
        For ii = 0 To m_Pout.pcode(pcd).num - 1
            If m_Pout.pcode(pcd).type(ii).edasysID = tid Then
                '�r���v�ŕ\���Ώ��r��EdasysID��0�̏ꍇ�͑ΏۊO
                If m_Pout.pcode(pcd).type(ii).disp = 0 Then Return False
                '�D��x��r
                If GetGradePriority(m_Pout.pcode(pcd).type(ii).disp) >= GetGradePriority(gid) Then
                    Return True
                Else
                    Return False
                End If
            End If
        Next ii

        '�r�킪��v���Ȃ��ꍇ
        '�f�t�H���g�\���̕\���Ώ��r��EdasysID��0�̏ꍇ�͑ΏۊO
        If m_Pout.pcode(pcd).def.disp = 0 Then Return False
        '�D��x��r
        If GetGradePriority(m_Pout.pcode(pcd).def.disp) >= GetGradePriority(gid) Then
            Return True
        Else
            Return False
        End If

    End Function


    ''' <summary>
    ''' �r�햼���擾
    ''' </summary>
    ''' <param name="nEdasysID">�r��EdasysID</param>
    ''' <returns>�r�햼</returns>
    ''' <remarks></remarks>
    Public Function GetDspTypeName(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""

        If m_dctDspTypeName.ContainsKey(nEdasysID) Then
            strRet = m_dctDspTypeName(nEdasysID)
        End If
        Return strRet

    End Function

    ''' <summary>
    ''' �}�b�v�\���r�햼���擾
    ''' </summary>
    ''' <param name="nEdasysID">�r��EdasysID</param>
    ''' <returns>�}�b�v�\���r�햼</returns>
    ''' <remarks></remarks>
    Public Function GetDspMapName(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""

        If m_dctDspMapName.ContainsKey(nEdasysID) Then
            strRet = m_dctDspMapName(nEdasysID)
        End If

        Return strRet

    End Function
    ''' <summary>
    ''' �r��D��x�擾
    ''' </summary>
    ''' <param name="nEdasysID"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function getTypePriority(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 999

        If m_dctTypePriority.ContainsKey(nEdasysID) Then
            nRet = m_dctTypePriority(nEdasysID)
        End If

        Return nRet

    End Function

    ''' <summary>
    ''' �}�b�v�\���F���擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>�}�b�v�\���F</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeColor(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 0

        If m_dctDspGradeColor.ContainsKey(nEdasysID) Then
            nRet = m_dctDspGradeColor(nEdasysID)
        End If

        Return nRet

    End Function
    ''' <summary>
    ''' �O���[�h���̎擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>�O���[�h��</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeName(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""

        If m_dctDspGradeName.ContainsKey(nEdasysID) Then
            strRet = m_dctDspGradeName(nEdasysID)
        End If

        Return strRet

    End Function
    ''' <summary>
    ''' �O���[�h�D��x�擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetGradePriority(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 999

        If m_dctGradePriority.ContainsKey(nEdasysID) Then
            nRet = m_dctGradePriority(nEdasysID)
        End If

        Return nRet

    End Function

    ''' <summary>
    ''' �r�}�b�v�V���{���擾
    ''' </summary>
    ''' <param name="nEdasysID">�r��EdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetMapSymbol(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""
        Dim nSymbolIdx As Integer = 0

        If m_dctMapSymbol.ContainsKey(nEdasysID) Then
            nSymbolIdx = m_dctMapSymbol(nEdasysID)
            strRet = PLOT_SYMBOL_TYPE(nSymbolIdx)
        End If

        Return strRet
    End Function

    ''' <summary>
    ''' �r�}�b�v�V���{���\���F�擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetMapSymbolColor(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 0

        If m_dctMapSymbolColor.ContainsKey(nEdasysID) Then
            nRet = m_dctMapSymbolColor(nEdasysID)
        End If

        Return nRet
    End Function

    ''' <summary>
    ''' �D��x��r
    ''' </summary>
    ''' <param name="nPreTypeEdasysID">�O���r��EdasysID</param>
    ''' <param name="nNowTypeEdasysID">�����r��EdasysID</param>
    ''' <param name="nPreGaradeEdasysID">�O��O���[�hEdasysID</param>
    ''' <param name="nNowGradeEdasysID">����O���[�hEdasysID</param>
    ''' <returns>���A���(True:�D��x������,False:�D��x���Ⴂ)</returns>
    ''' <remarks></remarks>
    Public Function CheckPriority(ByVal nPreTypeEdasysID As Integer, ByVal nNowTypeEdasysID As Integer, _
                                    ByVal nPreGaradeEdasysID As Integer, ByVal nNowGradeEdasysID As Integer) As Boolean



        '�O��̃O���[�h�����D��x����������΁A�D��x������
        If GetGradePriority(nPreGaradeEdasysID) > GetGradePriority(nNowGradeEdasysID) Then

            Return True

        ElseIf GetGradePriority(nPreGaradeEdasysID) = GetGradePriority(nNowGradeEdasysID) Then

            '�O���[�h�̗D��x�������Ȃ�A�r��̗D��x���r���A�D��x����������΁A�D��x������
            If getTypePriority(nPreTypeEdasysID) > getTypePriority(nNowTypeEdasysID) Then
                Return True
            End If

        End If

        Return False

    End Function


    ''' <summary>
    ''' ���׃O���[�h�擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>���׃O���[�h(0:���Q,1:�y����,2:������,3:�d����)</returns>
    ''' <remarks></remarks>
    Public Function GetDefectGrade(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 0

        If m_dctDefectGrade.ContainsKey(nEdasysID) Then
            nRet = m_dctDefectGrade(nEdasysID)
        End If

        Return nRet

    End Function

    ''' <summary>
    ''' �ł��D��x���Ⴂ�O���[�h���擾
    ''' </summary>
    ''' <returns>�ł��D��x���Ⴂ�O���[�h��EdasysID</returns>
    ''' <remarks></remarks>
    Public Function GetPriorityMostLowGrade() As Integer

        Dim nWkEdasysID As Integer = 0
        Dim nWkPriority As Integer = 0

        '�ł��D��x���Ⴂ�̂�����
        For Each nEdasyID As Integer In m_dctGradePriority.Keys

            If nWkPriority < m_dctGradePriority(nEdasyID) Then
                nWkEdasysID = nEdasyID
                nWkPriority = m_dctGradePriority(nEdasyID)
            End If

        Next

        Return nWkEdasysID

    End Function
End Class
