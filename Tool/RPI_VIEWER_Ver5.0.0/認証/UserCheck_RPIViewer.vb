Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Option Explicit On
Imports System.Runtime.InteropServices          'MarshalAs�N���X���X
Imports tClassLibrary

Namespace tNinsho
    Public Module UserCheck
        ' ===========================================================
        ' �萔
        ' ===========================================================
        ' DLL��
        Const Dll32bit As String = "UserCheck32.dll"
        Const Dll64bit As String = "UserCheck64.dll"
        ' �V�X�e����
        Const SYSTEM_NAME As String = "ALL"
        ' DB�ڑ���敪
        Enum EM_CONNECT_DB_TYPE
            emLocal = 0             ' ���[�J��
            emShare                 ' ����
            emTikuseki              ' �~��

            emEnd
        End Enum

        ' ===========================================================
        ' DLL�A�N�Z�X�N���X
        ' ===========================================================
        ''' <summary>
        ''' 32BitDLL�p�A�N�Z�X�N���X
        ''' </summary>
        ''' <remarks></remarks>
        Private Class Auth32
            Const Dll As String = Dll32bit
            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccess(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccessODBC(<MarshalAs(UnmanagedType.LPStr)> ByVal DSN As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetRemoteDBType(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAccessibleKensaNames(<MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As <MarshalAs(UnmanagedType.SafeArray)> String()
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAuthID() As <MarshalAs(UnmanagedType.BStr)> String
            End Function


            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetErrorString() As <MarshalAs(UnmanagedType.BStr)> String
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Sub Debug_SetAuthID(<MarshalAs(UnmanagedType.LPStr)> ByVal AuthID As String)
            End Sub
        End Class

        ''' <summary>
        ''' 64BitDLL�p�A�N�Z�X�N���X
        ''' </summary>
        ''' <remarks></remarks>
        Private Class Auth64
            Const Dll As String = Dll64bit

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccess(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function CheckAccessODBC(<MarshalAs(UnmanagedType.LPStr)> ByVal DSN As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SysID As String, <MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetRemoteDBType(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String) As Integer
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAuthID() As <MarshalAs(UnmanagedType.BStr)> String
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetAccessibleKensaNames(<MarshalAs(UnmanagedType.LPStr)> ByVal SoftID As String) As <MarshalAs(UnmanagedType.SafeArray)> String()
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Function GetErrorString() As <MarshalAs(UnmanagedType.BStr)> String
            End Function

            <DllImport(Dll, ExactSpelling:=True)> _
            Public Shared Sub Debug_SetAuthID(<MarshalAs(UnmanagedType.LPStr)> ByVal AuthID As String)
            End Sub
        End Class

        ' ===========================================================
        ' x86,x64�������Ŕ��f
        ' ===========================================================
        ''' <summary>
        ''' �����Ŏw�肳�ꂽ�������u�E�\�t�g�ɂ��āA�F�؃`�F�b�N�����{����
        ''' </summary>
        ''' <param name="DBIP">�ڑ����悤�Ƃ���f�[�^�x�[�X��IP�A�h���X</param>
        ''' <param name="SysID">�������u���� (NHOT_MAKI�Ȃ�)</param>
        ''' <param name="SoftID">�\�t�g���� ( SO_GAMEN�Ȃ�)</param>
        ''' <returns>����=�A�N�Z�X��</returns>
        Public Function CheckAccess(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String, ByVal SysID As String, Optional ByVal SoftID As String = "") As Integer
            If IntPtr.Size = 4 Then
                Return Auth32.CheckAccess(DBIP, SysID, SoftID)
            Else
                Return Auth64.CheckAccess(DBIP, SysID, SoftID)
            End If
        End Function

        ''' <summary>
        ''' �����Ŏw�肳�ꂽ�������u�E�\�t�g�ɂ��āA�F�؃`�F�b�N�����{����
        ''' </summary>
        ''' <param name="DSN">�ڑ����悤�Ƃ���f�[�^�x�[�X��ODBC��</param>
        ''' <param name="SysID">�������u���� (NHOT_MAKI�Ȃ�)</param>
        ''' <param name="SoftID">�\�t�g���� ( SO_GAMEN�Ȃ�)</param>
        ''' <returns>����=�A�N�Z�X��</returns>
        Public Function CheckAccessODBC(<MarshalAs(UnmanagedType.LPStr)> ByVal DSN As String, ByVal SysID As String, Optional ByVal SoftID As String = "") As Integer
            If IntPtr.Size = 4 Then
                Return Auth32.CheckAccessODBC(DSN, SysID, SoftID)
            Else
                Return Auth64.CheckAccessODBC(DSN, SysID, SoftID)
            End If
        End Function


        ''' <summary>
        ''' �����Ŏw�肳�ꂽIP�A�h���X�̋敪�i���[�J��or����or���n�j��Ԃ�
        ''' </summary>
        ''' <param name="DBIP">IP�A�h���X</param>
        ''' <returns>0:���[�J���A1:�����A2:���n</returns>
        Public Function GetRemoteDBType(<MarshalAs(UnmanagedType.LPStr)> ByVal DBIP As String) As Integer
            If IntPtr.Size = 4 Then
                Return Auth32.GetRemoteDBType(DBIP)
            Else
                Return Auth64.GetRemoteDBType(DBIP)
            End If
        End Function


        ''' <summary>
        ''' �����Ŏw�肳�ꂽ�\�t�g�̃A�N�Z�X��������錟�����u�̈ꗗ��z��Ƃ��ĕԂ��B
        ''' </summary>
        ''' <param name="SoftID">�\�t�g���� (SO_GAMEN�Ȃ�)</param>
        ''' <returns>�������u�̈ꗗ</returns>
        Public Function GetAccessibleKensaNames(Optional ByVal SoftID As String = "") As String()
            If IntPtr.Size = 4 Then
                Return Auth32.GetAccessibleKensaNames(SoftID)
            Else
                Return Auth64.GetAccessibleKensaNames(SoftID)
            End If
        End Function

        ''' <summary>
        ''' �F�؎��ȂǃG���[��������������ŌĂԂƁA���̌����𕶎���ŕԂ��B
        ''' </summary>
        ''' <returns>�G���[������</returns>
        ''' <remarks>�Ăяo�������������ꍇ�͋󕶎����Ԃ�</remarks>
        Public Function GetErrorString() As String
            If IntPtr.Size = 4 Then
                Return Auth32.GetErrorString()
            Else
                Return Auth64.GetErrorString()
            End If
        End Function

        ''' <summary>
        ''' PC���̔F��ID(�F��ID�I���AAD���[�U���AIP�A�h���X)���擾����
        ''' </summary>
        ''' <returns>�g0, ad\User5678, 10.1.1.1�h</returns>
        ''' <remarks>CSV�`��</remarks>
        Public Function GetAuthID() As String
            If IntPtr.Size = 4 Then
                Return Auth32.GetAuthID()
            Else
                Return Auth64.GetAuthID()
            End If
        End Function

        ''' <summary>
        ''' �F��ID���A�w�肵�������̂��̂ŏ㏑������
        ''' </summary>
        ''' <param name="AuthID">�����I�ɐݒ肷��F��ID</param>
        ''' <remarks>�����[�X�ł�DLL�ł̓_�~�[�֐��ŌĂ�ł������Ȃ��B</remarks>
        Public Sub Debug_SetAuthID(ByVal AuthID As String)
            If IntPtr.Size = 4 Then
                Auth32.Debug_SetAuthID(AuthID)
            Else
                Auth64.Debug_SetAuthID(AuthID)
            End If
        End Sub


        ' ===========================================================
        ' ���C���F�؏���
        ' ===========================================================
        ''' <summary>
        ''' �F�؏���
        ''' </summary>
        ''' <param name="sTaskName">���^�X�N���i�F�ؗp�\�t�g���j</param>
        ''' <returns>true�F�F�ؐ���,false�F�F�؎��s</returns>
        ''' <remarks>�F�؎��s���A�G���[�|�b�v�A�b�v�\��</remarks>
        Public Function IsUserCheck(ByVal sTaskName As String) As Boolean
            Dim strDbIp As String = ""                                      ' �ڑ���DB��IP�A�h���X
            Dim bConnectedDbType(EM_CONNECT_DB_TYPE.emEnd - 1) As Boolean   ' �ڑ���敪(true:�F�؍ς݁Afalse:�F�ؖ��ρA(0)=����,(1)=�~��)
            Dim nRet As Integer                                             ' �F�،���
            ' ������
            Array.Clear(bConnectedDbType, 0, EM_CONNECT_DB_TYPE.emEnd)
            
            For ii As Integer = 1 To MC_LINE_CNT
                Dim strKey As String        ' �擾�L�[
                Dim strWk As String

                '''' �R�C�������p���C�����擾
                ' L1�`MC_LINE_CNT�̕�����i"�\���p���C����", "���C���Z�N�V������"�j�擾
                strKey = String.Format("L_{0}", ii.ToString)
                strWk = tMod.tMod_IniFile.ReadIniStr("LINE", strKey, TASKINI_NAME)
                ' �󔒂��g����
                strWk.Trim()
                ' �擾������̒�����0�Ȃ�Continue
                If 0 >= strWk.Length Then Continue For
                ' �J���}��؂�ɒl����
                Dim strAry() As String = strWk.Split(CChar(","))
                ' 2�̗v�f���擾�o�������`�F�b�N
                If 2 <> strAry.Length Then Continue For
                ' �e�v�f���g����
                strAry(0).Trim()
                strAry(1).Trim()
                ' �e�v�f�̕�����̒�����0�Ȃ�Continue
                If 0 >= strAry(0).Length Then Continue For
                If 0 >= strAry(1).Length Then Continue For

                '''' �ڑ���敪�`�F�b�N�i�S���C���̐ڑ��悪���[�J���ior����or���n�~�ρj�ɓ��ꂳ��Ă��邩�H�j
                ' DB�A�N�Z�X���L���i=1�j�ȊO�Ȃ�Continue
                If 1 <> tMod.tMod_IniFile.ReadIniInt(strAry(1), "ENABLE_DB", TASKINI_NAME) Then Continue For
                ' �ڑ���DB�̃A�h���X���擾
                strDbIp = tMod.tMod_IniFile.ReadIniStr(strAry(1), "DBADDLESS", TASKINI_NAME)
                ' �ڑ���DB�̃A�h���X�`�F�b�N
                If 0 >= strDbIp.Length Then Continue For

                Dim nNewDbType As Integer  ' �V�����ڑ���敪
                ' �ڑ���敪���擾�i���[�J��=0,����=1,���n�~��=2�j
                nNewDbType = tNinsho.GetRemoteDBType(strDbIp)

                ' �擾�����ڑ���敪�ɂ��Ă܂��F�؂��Ă��Ȃ��ꍇ�͔F�؏������s��
                If False = bConnectedDbType(nNewDbType) Then
                    ' �F�؊J�n
                    nRet = tNinsho.CheckAccess(strDbIp, SYSTEM_NAME, sTaskName)
                    bConnectedDbType(nNewDbType) = True
                End If

                ' �F�؎��s�Ȃ烋�[�v�𔲂���
                If 0 > nRet Then Exit For
                ' ���ɓ����ƒ~�ςŔF�؍ς݂Ȃ烋�[�v�𔲂���
                If bConnectedDbType(EM_CONNECT_DB_TYPE.emShare) And _
                bConnectedDbType(EM_CONNECT_DB_TYPE.emTikuseki) Then Exit For
            Next

            ''''' �F�؏���
            ' �F�؎��s�̏ꍇ
            If 0 > nRet Then
                ' �G���[���b�Z�[�W�擾
                Dim strErrMsg As String = tNinsho.GetErrorString + String.Format("(ErrMsg={0})", nRet)
                ' �|�b�v�A�b�v�\�����A���u�\�t�g���I������
                MsgBox(strErrMsg, MsgBoxStyle.OkOnly, "�F�؃G���[")                       '���b�Z�[�W�{�b�N�X

                Return False
            End If
            Return True
        End Function

        ''' <summary>
        ''' DB���擾
        ''' </summary>
        ''' <param name="nDbType">�ڑ���敪</param>
        ''' <returns>"���[�J��"or"����"or"���n�~��"�̕�����</returns>
        ''' <remarks></remarks>
        Public Function GetDbName(ByVal nDbType As Integer) As String
            If 0 = nDbType Then
                Return "���[�J��"
            ElseIf 1 = nDbType Then
                Return "�����T�[�o�["
            ElseIf 2 = nDbType Then
                Return "���n�~�σT�[�o�["
            Else
                Return ""
            End If
        End Function

    End Module

End Namespace

