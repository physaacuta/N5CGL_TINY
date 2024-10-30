Option Strict On
Imports System.Collections
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class clsParamManager

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔 �\���̒�`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r�� �\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure PARAM_TYPE
        ''' <summary>EdasysID</summary>
        Dim nEdasysID As Integer
        ''' <summary>�D��x</summary>
        Dim nYuusen As Integer
        ''' <summary>��������</summary>
        Dim sName As String
        ''' <summary>����</summary>
        Dim sSName As String
    End Structure

    ''' <summary>
    ''' �O���[�h �\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure PARAM_GRADE
        ''' <summary>EdasysID</summary>
        Dim nEdasysID As Integer
        ''' <summary>�D��x</summary>
        Dim nYuusen As Integer
        ''' <summary>��������</summary>
        Dim sName As String
        ''' <summary>�O���[�h�F (=�I�[�o�[���C�F�����p)</summary>
        Dim clrDspColor As Color
    End Structure
    ''' <summary>
    ''' �����\�ʏ�� �\����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure PARAM_SCODE
        ''' <summary>ID</summary>
        Dim nID As Integer
        ''' <summary>��������</summary>
        Dim sName As String
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �A�N�Z�X�Z�b�V����
    Private m_Session As String													' ini�t�@�C���Z�b�V����

    ' DB���
    Private m_Param_Type As New Dictionary(Of Integer, PARAM_TYPE)				' �r��
    Private m_Param_Grade As New Dictionary(Of Integer, PARAM_GRADE)			' �O���[�h
    Private m_Param_Scode As New Dictionary(Of Integer, PARAM_SCODE)		   ' �����\�ʏ��



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�A�N�Z�X
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r�� �w��EdasysID �擾
    ''' </summary>
    ''' <param name="nEdasysID">EdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetParamType(ByVal nEdasysID As Integer) As PARAM_TYPE
        If m_Param_Type.ContainsKey(nEdasysID) Then
            Return m_Param_Type(nEdasysID)
        Else
            Return Nothing
        End If
    End Function
    Public Function GetParamType() As Dictionary(Of Integer, PARAM_TYPE).ValueCollection
        Return m_Param_Type.Values
    End Function


    ''' <summary>
    ''' �O���[�h �w��EdasysID �擾
    ''' </summary>
    ''' <param name="nEdasysID">EdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetParamGrade(ByVal nEdasysID As Integer) As PARAM_GRADE
        If m_Param_Grade.ContainsKey(nEdasysID) Then
            Return m_Param_Grade(nEdasysID)
        Else
            Return Nothing
        End If
    End Function
    Public Function GetParamGrade() As Dictionary(Of Integer, PARAM_GRADE).ValueCollection
        Return m_Param_Grade.Values
    End Function

    ''' <summary>
    ''' Scode �w��ID �擾
    ''' </summary>
    ''' <param name="nID">ID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetParamScode(ByVal nID As Integer) As PARAM_SCODE
        If m_Param_Scode.ContainsKey(nID) Then
            Return m_Param_Scode(nID)
        Else
            Return Nothing
        End If
    End Function
    Public Function GetParamScode() As Dictionary(Of Integer, PARAM_SCODE).ValueCollection
        Return m_Param_Scode.Values
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{����
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' INI�t�@�C���Z�N�V����
    ''' </summary>
    ''' <param name="sSession">ini�t�@�C���Z�b�V����</param>
    ''' <remarks></remarks>
    Public Sub Set_DbSession(ByVal sSession As String)
        m_Session = sSession
    End Sub

    ''' <summary>
    ''' �S�ēǂݍ���
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG</returns>
    ''' <remarks></remarks>
    Public Function ReadAll() As Boolean
        If Not ReadParam_Type() Then Return False
        If Not ReadParam_Grade() Then Return False
        If Not ReadParam_Scode() Then Return False
        Return True
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DB�A�N�Z�X (�}�X�^�[�֌W)
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r��}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_Type() As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        Dim wk As PARAM_TYPE

        'SQL���쐬
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_TYPE", TASKINI_NAME)

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ��ُ� (ReadParam_Type)"))
                End If
                Return False
            End If

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s�ُ� (ReadParam_Type)({0})", sSQL))
                End If
                Return False
            End If


            ' �����o�b�t�@������
            m_Param_Type.Clear()

            ' �f�[�^�擾
            While sqlRead.Read()

                wk.nEdasysID = sqlRead.GetInt32(0)
                wk.nYuusen = sqlRead.GetInt32(1)
                wk.sName = sqlRead.GetString(2)
                wk.sSName = sqlRead.GetString(3)

                ' �o�^
                m_Param_Type.Add(wk.nEdasysID, wk)
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function


    ''' <summary>
    ''' �O���[�h�}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_Grade() As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        Dim wk As PARAM_GRADE

        'SQL���쐬
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_GRADE", TASKINI_NAME)

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ��ُ� (ReadParam_Grade)"))
                End If
                Return False
            End If

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s�ُ� (ReadParam_Grade)({0})", sSQL))
                End If
                Return False
            End If

            ' �����o�b�t�@������
            m_Param_Grade.Clear()

            ' �f�[�^�擾
            While sqlRead.Read()

                wk.nEdasysID = sqlRead.GetInt32(0)
                wk.nYuusen = sqlRead.GetInt32(1)
                wk.sName = sqlRead.GetString(2)
                wk.clrDspColor = tMod.GetIntToColor(sqlRead.GetInt32(3))

                ' �o�^
                m_Param_Grade.Add(wk.nEdasysID, wk)
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    ''' <summary>
    ''' �����\�ʏ�ԃ}�X�^�e�[�u��
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_Scode() As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        Dim wk As PARAM_SCODE

        'SQL���쐬
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_SCODE", TASKINI_NAME)
        If sSQL = "" Then
            m_Param_Scode.Clear()
            Return True
        End If

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ��ُ� (ReadParam_Scode)"))
                End If
                Return False
            End If
            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s�ُ� (ReadParam_Scode)({0})", sSQL))
                End If
                Return False
            End If

            ' �����o�b�t�@������
            m_Param_Scode.Clear()

            ' �f�[�^�擾
            While sqlRead.Read()

                wk.nID = sqlRead.GetInt32(0)
                wk.sName = sqlRead.GetString(1)

                ' �o�^
                m_Param_Scode.Add(wk.nID, wk)
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DB�A�N�Z�X (���ъ֌W)
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R�C�����
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Friend Function ReadZi_CoilInf(ByVal clsRpi As tClass_ViewRpiThread, ByRef typ As DATA_COIL) As Boolean
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim wk As DATA_COIL
        wk.sKizukenNo = clsRpi.GetRpi_THeader.cKizukenNo
        '' SQL����
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, "SQL_COIL_DATA", TASKINI_NAME)
        sSQL = String.Format(sSQL, wk.sKizukenNo)

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ��ُ� (ReadZi_CoilInf)"))
                End If
                Return False
            End If

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s�ُ� (ReadZi_CoilInf)({0})", sSQL))
                End If
                Return False
            End If

            ' �f�[�^�擾
            While sqlRead.Read()
                wk.sCoilNo = sqlRead.GetString(0)
                wk.sCoilNo = Trim(wk.sCoilNo)
                wk.nLen = sqlRead.GetInt32(1)
                wk.nWid = sqlRead.GetInt32(2)

                ' �o�^
                typ = wk
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function


    ''' <summary>
    ''' �r��� (���r�����r���̍���������)
    ''' </summary>
    ''' <returns>���A���(True:�ǂݍ���OK,False:�ǂݍ���NG)</returns>
    ''' <remarks></remarks>
    Friend Function ReadZi_DefectInf(ByVal clsRpi As tClass_ViewRpiThread, ByRef lst As List(Of DATA_DETECT_INF)) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sWk As String
        Dim wk As DATA_DETECT_INF

        '' SQL����
        sWk = "SELECT_INF_" & clsRpi.GetRpi_THeader.nMen.ToString
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, sWk, TASKINI_NAME)
        sSQL = String.Format(sSQL, clsRpi.GetRpi_THeader.cKizukenNo)



        '======================================================
        ' �f�[�^�擾
        Try
            '' DB�ڑ�
            If Not tcls_DB.Open() Then
                MsgBox("DB�Ɛڑ����s�B�r�I�[�o�[���C�̕\���͂ł��܂���B")
                Return False
            End If

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s�ُ� (ReadZi_DefectInf)({0})", sSQL))
                End If
                Return False
            End If

            ' �f�[�^�擾
            While sqlRead.Read()
                wk.bDsp = True	' �ŏ��͑S�I��

                wk.nNo = sqlRead.GetInt32(0)
                wk.tId = sqlRead.GetInt32(1)
                wk.gId = sqlRead.GetInt32(2)
                wk.nCamPos = sqlRead.GetInt32(3) - clsRpi.GetRpi_THeader.nStartCamset

                If sqlRead.Item(4).GetType().Name = "Int32" Then
                    wk.nFrameNum = sqlRead.GetInt32(4)
                    wk.nFrameNo = wk.nFrameNum - clsRpi.GetRpi_THeader.nFrameNumInit + 1
                    wk.nFrameXmin = sqlRead.GetInt32(5)
                    wk.nFrameXmax = sqlRead.GetInt32(6)
                    wk.nFrameYmin = sqlRead.GetInt32(7)
                    wk.nFrameYmax = sqlRead.GetInt32(8)
                Else
                    wk.nFrameNum = CInt(sqlRead.GetDouble(4))	' �l�̌ܓ����Ȃ��ƃ_���B�}�l�[�W�����g�X�^�W�I�ł́A�����Ɍ����Ă��z���g��0.999998�Ƃ��ɂȂ��Ă���
                    wk.nFrameNo = wk.nFrameNum - clsRpi.GetRpi_THeader.nFrameNumInit + 1
                    wk.nFrameXmin = CInt(sqlRead.GetDouble(5))
                    wk.nFrameXmax = CInt(sqlRead.GetDouble(6))
                    wk.nFrameYmin = CInt(sqlRead.GetDouble(7))
                    wk.nFrameYmax = CInt(sqlRead.GetDouble(8))
                End If
                wk.nFrameNoBase = wk.nFrameNo

                wk.nKiriXmin = sqlRead.GetInt32(9)
                wk.nKiriXmax = sqlRead.GetInt32(10)
                wk.nKiriYmin = sqlRead.GetInt32(11)
                wk.nKiriYmax = sqlRead.GetInt32(12)

                wk.nY = sqlRead.GetInt32(13)
                wk.nX = sqlRead.GetInt32(14)
                wk.nXws = sqlRead.GetInt32(15)

                '' C��ɕϊ�
                Dim nCenter As Integer = CInt((wk.nX + wk.nXws) / 2)		' �Z���^�[����̋����ɕϊ�
                If wk.nX < wk.nXws Then		' DS (-)
                    wk.nXc = (wk.nX - nCenter)
                Else						' WS (+)
                    wk.nXc = (nCenter - wk.nXws)
                End If

                nCenter = CInt(clsRpi.GetRpi_Header().width / 2)					' �Z���^�[
                Dim nWk As Integer = CInt(clsRpi.GetRpi_THeader().caminfo(wk.nCamPos).nBaseLeft + (wk.nFrameXmin + wk.nFrameXmax) / 2) - nCenter   ' �Z���^�[����̋����ɕϊ� [pix]
                wk.nFrameXc = CInt(nWk * clsRpi.GetRpi_Header().res_x)
                wk.nFrameBaseXc = wk.nFrameXc

                ' ���]�O
                wk.nBaseY = wk.nY
                wk.nBaseX = wk.nX
                wk.nBaseXws = wk.nXws
                wk.nBaseXc = wk.nXc


                '' �D��x�擾
                Try
                    wk.tYu = GetParamType(wk.tId).nYuusen
                    wk.gYu = GetParamGrade(wk.gId).nYuusen
                Catch ex As Exception
                End Try

                lst.Add(wk)
            End While

        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    ''' <summary>
    ''' �J�b�g���
    ''' </summary>
    Friend Function ReadZi_CutInf(ByVal clsRpi As tClass_ViewRpiThread, ByRef lst As List(Of DATA_CUT_INF)) As Boolean
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sWk As String
        Dim wk As DATA_CUT_INF

        '' SQL����
        sWk = "SELECT_CUT"
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, sWk, TASKINI_NAME)
        If 0 = sSQL.Length Then Return True
        sSQL = String.Format(sSQL, clsRpi.GetRpi_THeader.cKizukenNo)


        '======================================================
        ' �f�[�^�擾
        Try
            '' DB�ڑ�
            If Not tcls_DB.Open() Then
                MsgBox("DB�Ɛڑ����s�B�r�I�[�o�[���C�̕\���͂ł��܂���B")
                Return False
            End If

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return False

            ' �f�[�^�擾
            While sqlRead.Read()
                wk.nY = sqlRead.GetInt32(0)
                ' ���]�O
                wk.nBaseY = wk.nY

                lst.Add(wk)
            End While

        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True

    End Function

    ''' <summary>
    ''' �W�����v�f�s�b�`�l�擾
    ''' </summary>
    ''' <returns>�f�t�H���g�s�b�`�l[mm]</returns>
    ''' <remarks></remarks>
    Friend Function ReadZi_DefPitch(ByVal clsRpi As tClass_ViewRpiThread) As Integer
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim nWk As Integer = 0

        Dim sKizukenNo As String = clsRpi.GetRpi_THeader.cKizukenNo
        '' SQL����
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_JUMP", TASKINI_NAME)
        If 0 = sSQL.Length Then Return 0
        sSQL = String.Format(sSQL, sKizukenNo)

        Try
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Return 0

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return 0

            ' �f�[�^�擾
            While sqlRead.Read()
                nWk = sqlRead.GetInt32(0)
                Exit While
            End While
        Catch ex As Exception
            Return 0
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return nWk
    End Function


    ''' <summary>
    ''' ���]
    ''' </summary>
    Friend Sub ReadZi_DefectInfReverce(ByVal clsRpi As tClass_ViewRpiThread, ByRef lst As List(Of DATA_DETECT_INF), ByRef lstCut As List(Of DATA_CUT_INF))

        Dim nCoilLen As Integer = CInt(Int((clsRpi.GetRpi_Header.nframe + 1) * clsRpi.GetRpi_Header.height * clsRpi.GetRpi_Header.res_y))
        Dim bTB As Boolean = clsRpi.GetCls_ImgDsp.tProp_ImgKind_IsTurnTB
        Dim bDsWs As Boolean = clsRpi.GetCls_ImgDsp.tProp_ImgKind_IsTurnLR


        For ii As Integer = 0 To lst.Count - 1
            Dim wk As DATA_DETECT_INF = lst(ii)

            ' ���]
            If bTB Then
                wk.nY = nCoilLen - wk.nBaseY
            Else
                wk.nY = wk.nBaseY
            End If

            ' ���]
            If bDsWs Then
                wk.nX = wk.nBaseXws
                wk.nXws = wk.nBaseX
                wk.nXc = wk.nBaseXc * -1
                wk.nFrameXc = wk.nFrameBaseXc * -1
            Else
                wk.nX = wk.nBaseX
                wk.nXws = wk.nBaseXws
                wk.nXc = wk.nBaseXc
                wk.nFrameXc = wk.nFrameBaseXc
            End If


            lst(ii) = wk
        Next


        For ii As Integer = 0 To lstCut.Count - 1
            Dim wk As DATA_CUT_INF = lstCut(ii)

            ' ���]
            If bTB Then
                wk.nY = nCoilLen - wk.nBaseY
            Else
                wk.nY = wk.nBaseY
            End If
            lstCut(ii) = wk
        Next

    End Sub

End Class
