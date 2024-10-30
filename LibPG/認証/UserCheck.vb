Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Option Explicit On
Imports System.Runtime.InteropServices          'MarshalAs�N���X���X
Imports tClassLibrary

Namespace tNinsho
    Public Module UserCheck
        'DLL��
        Const Dll32bit As String = "UserCheck32.dll"
        Const Dll64bit As String = "UserCheck64.dll"

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


        ''' <summary>
        ''' ���ʔF�؏���
        ''' </summary>
        ''' <param name="sTaskName">���^�X�N���i�F�ؗp�\�t�g���j</param>
        ''' <returns>true�F�F�ؐ���,false�F�F�؎��s</returns>
        ''' <remarks>�F�؎��s���A�G���[�|�b�v�A�b�v�\��</remarks>
        Public Function IsUserCheck(ByVal sTaskName As String) As Boolean
            Dim strDbIp As String = tMod.tMod_IniFile.ReadIniStr(INI_DB, "DBADDLESS", TASKINI_NAME)
            ' �F�،���
            Dim nRet As Integer = tNinsho.CheckAccess(strDbIp, SYSTEM_NAME, sTaskName)
            ' �F�؎��s�̏ꍇ
            If 0 > nRet Then
                ' �G���[���b�Z�[�W�擾
                Dim strErrMsg As String = tNinsho.GetErrorString + String.Format("(ErrMsg={0})", nRet)
                ' �|�b�v�A�b�v�\�����A���u�\�t�g���I������
                Dim frm_Msg As New tClassForm.frmPop_MsgBox(strErrMsg, "�F�؃G���[", Color.Red.ToArgb, , , Windows.Forms.DialogResult.OK)                       '���b�Z�[�W�{�b�N�X
                frm_Msg.ShowDialog()
                Return False
            End If
            Return True
        End Function


    End Module


    'Public Class tAuth



    '    Public Shared Sub Main()

    '        System.Console.WriteLine(AuthUser.GetIDKey())

    '        For Each s As String In AuthUser.GetPermittedLines(1)
    '            System.Console.WriteLine(s)
    '        Next

    '    End Sub

    'End Class

End Namespace

