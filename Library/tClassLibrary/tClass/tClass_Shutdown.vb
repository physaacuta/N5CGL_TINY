'*********************************************************************************
'�p�\�R���̏I���E�ċN�����𐧌䂷��N���X
'	[Ver]
'		Ver.01    2005/12/30  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices          ' DllImport

Namespace tClass
    ''' <summary>
    ''' �p�\�R���̏I���E�ċN�����𐧌䂷��N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Shutdown
        '********************************************************************
        '���J�萔��`
        '********************************************************************
        ''' <summary>
        ''' �p�\�R���̓����`
        ''' </summary>
        ''' <remarks></remarks>
        <Flags()> _
        Public Enum ShutdownFlag As Integer
            ''' <summary>���O�I�t</summary> 
            LogOff = &H0
            ''' <summary>�V���b�g�_�E��</summary> 
            Shutdown = &H1
            ''' <summary>�ċN��</summary> 
            Reboot = &H2
            ''' <summary>�����I�ɏI��</summary> 
            Force = &H4
            ''' <summary>�d���I�t</summary> 
            PowerOff = &H8
            ''' <summary>���������̃A�v���������I�ɏI�� (OR�ő��t���O�Ɠ����w��\)(NT�n�ȊO�͖���)</summary> 
            ForceIfHung = &H10
        End Enum

        '********************************************************************
        'API��`
        '********************************************************************
        <StructLayout(LayoutKind.Sequential, Pack:=4)> _
        Private Structure LUID_AND_ATTRIBUTES
            Dim Luid As Long
            Dim Attributes As Integer
        End Structure

        <StructLayout(LayoutKind.Sequential, Pack:=4)> _
        Private Structure TOKEN_PRIVILEGES
            Dim PrivilegeCount As Integer
            Dim Privilege As LUID_AND_ATTRIBUTES
        End Structure

        <DllImport("user32.dll")> _
        Private Shared Function ExitWindowsEx _
            (ByVal flag As Integer, _
            ByVal reserved As Integer) _
            As Boolean
        End Function

        <DllImport("advapi32.dll")> _
        Private Shared Function OpenProcessToken _
            (ByVal ProcessHandle As IntPtr, _
            ByVal DesiredAccess As Integer, _
            ByRef TokenHandle As IntPtr) _
            As Boolean
        End Function

        <DllImport("advapi32.dll")> _
        Private Shared Function LookupPrivilegeValue _
            (ByVal lpSystemName As String, _
            ByVal lpName As String, _
            ByRef lpLuid As Long) _
            As Boolean
        End Function

        <DllImport("advapi32.dll")> _
        Private Shared Function AdjustTokenPrivileges _
            (ByVal TokenHandle As IntPtr, _
            ByVal DisableAllPrivileges As Boolean, _
            ByRef NewState As TOKEN_PRIVILEGES, _
            ByVal BufferLength As Integer, _
            ByVal PreviousState As IntPtr, _
            ByVal ReturnLength As IntPtr) _
            As Boolean
        End Function

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �w�肵�����[�h�Ńp�\�R���̑�����s��
        ''' </summary>
        ''' <param name="flags">�p�\�R���̓����`</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Shared Function ExitWindows(ByVal flags As ShutdownFlag) As Boolean
            Dim blnRetc As Boolean
            Dim setFlg As ShutdownFlag = flags  '���s�t���O

            Try
                '''' �V�X�e����NT�n���`�F�b�N
                If System.Environment.OSVersion.Platform = System.PlatformID.Win32NT Then
                    setFlg = setFlg And Not ShutdownFlag.Force
                    setFlg = setFlg And Not ShutdownFlag.ForceIfHung
                    '''' ���O�I�t�̂Ƃ��͓��������蓖�Ă�K�v�͂Ȃ�
                    If setFlg <> ShutdownFlag.LogOff Then
                        '' �V���b�g�_�E�����������蓖�Ă�
                        SetShutdownPrivilege()
                    End If
                Else
                    '' NT�n�ȊO�ł�ForceIfHung�͖���
                    setFlg = setFlg And Not ShutdownFlag.ForceIfHung
                End If
                '''' ExitWindowsEx�֐����Ăяo��
                blnRetc = ExitWindowsEx(CInt(setFlg), 0)
            Catch ex As Exception
                blnRetc = False
            End Try
            Return blnRetc
        End Function

        ''' <summary>
        ''' ���݂̃v���Z�X�ɃV���b�g�_�E�����������蓖�Ă�
        ''' </summary>
        ''' <remarks></remarks>
        Private Shared Sub SetShutdownPrivilege()
            Const TOKEN_QUERY As Integer = &H8
            Const TOKEN_ADJUST_PRIVILEGES As Integer = &H20
            Const SE_SHUTDOWN_NAME As String = "SeShutdownPrivilege"
            Const SE_PRIVILEGE_ENABLED As Integer = &H2

            '''' �v���Z�X�n���h�����擾����
            Dim hProc As IntPtr
            hProc = Process.GetCurrentProcess().Handle

            '''' �g�[�N�����擾����
            Dim hToken As IntPtr = IntPtr.Zero
            If Not OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES Or TOKEN_QUERY, hToken) Then
                Throw New Exception("�g�[�N�����擾�ł��܂���ł����B")
            End If

            '''' LUID���擾����
            Dim luid As Long = 0
            If Not LookupPrivilegeValue(Nothing, SE_SHUTDOWN_NAME, luid) Then
                Throw New Exception("LUID���擾�ł��܂���ł����B")
            End If

            '''' SE_SHUTDOWN_NAME������ݒ肷��
            Dim tp As New TOKEN_PRIVILEGES
            tp.PrivilegeCount = 1
            tp.Privilege = New LUID_AND_ATTRIBUTES
            tp.Privilege.Luid = luid
            tp.Privilege.Attributes = SE_PRIVILEGE_ENABLED
            If Not AdjustTokenPrivileges(hToken, False, tp, 0, IntPtr.Zero, IntPtr.Zero) Then
                Throw New Exception("SE_SHUTDOWN_NAME������ݒ�ł��܂���ł����B")
            End If
        End Sub
    End Class
End Namespace
