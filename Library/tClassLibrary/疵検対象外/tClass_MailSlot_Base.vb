'*********************************************************************************
'MailSlot�̑���M���s�����N���X
'[Ver]
' Ver.01    2005/12/18  vs2005 �Ή�
'
'[����]
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Threading
Imports System.Runtime.InteropServices

Namespace tClass

    ''' <summary>
    ''' MailSlot�̑���M���s�����N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_MailSlot_Base
        Implements IDisposable                          ' �f�X�g���N�^��`

        '********************************************************************
        ' API�萔 ��`
        '********************************************************************
#Region "API�萔��`"
        '' �A�N�Z�X����
        Protected Const GENERIC_READ As Integer = &H80000000          ' �I�u�W�F�N�g�ւ̓ǂݎ��A�N�Z�X
        Protected Const GENERIC_WRITE As Integer = &H40000000         ' �I�u�W�F�N�g�ւ̏������݃A�N�Z�X
        Protected Const FILE_SHARE_READ As Integer = &H1
        Protected Const FILE_SHARE_WRITE As Integer = &H2
        Protected Const OPEN_EXISTING As Integer = 3
        Protected Const FILE_ATTRIBUTE_NORMAL As Integer = &H80
        Protected Const FILE_FLAG_OVERLAPPED As Integer = &H40000000

        ''' <summary>
        ''' OVERLAPPED
        ''' </summary>
        ''' <remarks></remarks>
        Protected Structure OVERLAPPED
            Dim Internal As Long
            Dim InternalHigh As Long
            Dim offset As Long
            Dim OffsetHigh As Long
            Dim hEvent As Long
        End Structure

        ''''''''' API
        '' �t�@�C������
        <DllImport("KERNEL32.DLL", EntryPoint:="CreateFileA", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
        Protected Shared Function CreateFile( _
                <MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String, _
                ByVal dwDesiredAccess As Integer, _
                ByVal dwShareMode As Integer, _
                ByVal lpSecurityAttributes As Integer, _
                ByVal dwCreationDisposition As Integer, _
                ByVal dwFlagsAndAttributes As Integer, _
                ByVal hTemplateFile As Integer) _
            As IntPtr
        End Function

        '' �n���h���j��
        <DllImport("KERNEL32.dll", EntryPoint:="CloseHandle", SetLastError:=True)> _
        Protected Shared Function CloseHandle( _
                ByVal hObject As IntPtr) _
            As Boolean
        End Function
        <DllImport("KERNEL32.dll", EntryPoint:="CloseHandle", SetLastError:=True)> _
        Protected Shared Function CloseHandle( _
                ByVal hObject As Integer) _
            As Boolean
        End Function

        '' �t�@�C���Ǎ���
        <DllImport("KERNEL32.dll", EntryPoint:="ReadFile", SetLastError:=True)> _
        Protected Shared Function ReadFile( _
                ByVal hFile As IntPtr, _
                ByVal lpBuffer As Byte(), _
                ByVal nNumberOfBytesToRead As Integer, _
                ByRef lpNumberOfBytesRead As Integer, _
                ByRef lpOverlapped As OVERLAPPED) _
            As Boolean
        End Function
        <DllImport("KERNEL32.dll", EntryPoint:="ReadFile", SetLastError:=True)> _
        Protected Shared Function ReadFile( _
                ByVal hFile As IntPtr, _
                ByVal lpBuffer As IntPtr, _
                ByVal nNumberOfBytesToRead As Integer, _
                ByRef lpNumberOfBytesRead As Integer, _
                ByRef lpOverlapped As OVERLAPPED) _
            As Boolean
        End Function

        '' �t�@�C��������
        <DllImport("KERNEL32.dll", EntryPoint:="WriteFile", SetLastError:=True)> _
        Protected Shared Function WriteFile( _
          ByVal hFile As IntPtr, _
          ByVal lpBuffer As Byte(), _
          ByVal nNumberOfBytesToWrite As Integer, _
          ByRef lpNumberOfBytesWritten As Integer, _
          ByRef lpOverlapped As OVERLAPPED) _
         As Boolean
        End Function
        <DllImport("KERNEL32.dll", EntryPoint:="WriteFile", SetLastError:=True)> _
        Protected Shared Function WriteFile( _
                ByVal hFile As IntPtr, _
                ByVal lpBuffer As IntPtr, _
                ByVal nNumberOfBytesToWrite As Integer, _
                ByRef lpNumberOfBytesWritten As Integer, _
                ByRef lpOverlapped As OVERLAPPED) _
            As Boolean
        End Function

        '' I/O����̎��������s�� (FILE_FLAG_OVERLAPPED ���̂ݗL��)
        <DllImport("KERNEL32.dll", EntryPoint:="CancelIo", SetLastError:=True)> _
        Protected Shared Function CancelIo( _
                ByVal hFile As IntPtr) _
            As Boolean
        End Function

        '' ���[���X���b�g����
        <DllImport("KERNEL32.DLL", EntryPoint:="CreateMailslotA", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
        Protected Shared Function CreateMailslot( _
          <MarshalAs(UnmanagedType.LPStr)> ByVal lpName As String, _
           ByVal nMaxMessageSize As Integer, _
           ByVal lReadTimeout As Integer, _
           ByRef lpSecurityAttributes As SECURITY_ATTRIBUTES) _
         As IntPtr
        End Function


        '' ���[���X���b�g���擾
        <DllImport("KERNEL32.dll", EntryPoint:="GetMailslotInfo", SetLastError:=True)> _
        Protected Shared Function GetMailslotInfo( _
          ByVal hMailslot As IntPtr, _
          ByRef lpMaxMessageSize As Integer, _
          ByRef lpNextSize As Integer, _
          ByRef lpMessageCount As Integer, _
          ByRef lpReadTimeout As Integer) _
         As Boolean
        End Function

        '' �p���I�v�V����
        <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
        Protected Structure SECURITY_ATTRIBUTES
            Dim nLength As Integer
            Dim lpSecurityDescriptor As IntPtr
            Dim bInheritHandle As Integer
        End Structure
#End Region

        '********************************************************************
        '�O���[�o���萔��`
        '********************************************************************
        Public Const gcMaxMailSize As Integer = 400		' ���[���X���b�g�ő�T�C�Y
        Public Const gcMyPC As Char = "."c				' ��PC
        Public Const gcUDP As Char = "*"c				' UDP���M

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False		' �d������Ăяo�������o����ɂ�
        Private m_pLockObject As Object					' SyncLock�����I�u�W�F�N�g (���M���̂ݎg�p)

        '�|�C���^
        Protected m_hRecvMailPtr As IntPtr				' ��M���[���X���b�g �n���h��
        ' ''Protected m_hSendMailPtr As IntPtr              ' ���M���[���X���b�g �n���h��

        Protected m_hRecvBufPtr As IntPtr				' ��M�������̈�A�h���X
        Protected m_hSendBufPtr As IntPtr				' ���M�������̈�A�h���X

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '' ����M�f�[�^�\���̂̃T�C�Y�� �������̈�m�� (���[���X���b�g�̍ő�l)
            m_hRecvBufPtr = Marshal.AllocHGlobal(gcMaxMailSize)
            m_hSendBufPtr = Marshal.AllocHGlobal(gcMaxMailSize)
            m_pLockObject = New Object
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                Me.Mail_Close()
                ' ''Me.Mail_SendClose()
                If m_hRecvBufPtr.Equals(IntPtr.Zero) Then
                    Marshal.FreeHGlobal(m_hRecvBufPtr)
                    m_hRecvBufPtr = IntPtr.Zero
                End If
                If m_hSendBufPtr.Equals(IntPtr.Zero) Then
                    Marshal.FreeHGlobal(m_hSendBufPtr)
                    m_hSendBufPtr = IntPtr.Zero
                End If
                If Not m_pLockObject Is Nothing Then
                    m_pLockObject = Nothing
                End If
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks>�g�p��A�K���R�[�����鎖</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' ��M���[���X���b�g����
        ''' </summary>
        ''' <param name="strMyTaskName">���[���X���b�g����</param>
        ''' <param name="recvBufLen">��M�o�b�t�@�T�C�Y  �������g��Ȃ�</param>    
        ''' <param name="sendBufLen">���M�o�b�t�@�T�C�Y  �������g��Ȃ�</param>            
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function Mail_Open(ByVal strMyTaskName As String, Optional ByVal recvBufLen As Integer = gcMaxMailSize, Optional ByVal sendBufLen As Integer = gcMaxMailSize) As Boolean
            Dim strSrotName As String				' ���[���X���b�g����
            Dim tSA As SECURITY_ATTRIBUTES

            '' �I�[�v���`�F�b�N
            If Not m_hRecvMailPtr.Equals(IntPtr.Zero) Then Return False

            '' ���[���X���b�g�p�f�[�^����
            tSA.nLength = System.Runtime.InteropServices.Marshal.SizeOf(tSA)
            tSA.lpSecurityDescriptor = IntPtr.Zero
            tSA.bInheritHandle = 0 'False
            strSrotName = "\\" & gcMyPC & "\MailSlot\" & strMyTaskName


            '' ���[���X���b�g�I�[�v�� 
            m_hRecvMailPtr = CreateMailslot(strSrotName, 0, 0, tSA)
            If m_hRecvMailPtr.ToInt32 <= 0 Then
                m_hRecvMailPtr = IntPtr.Zero
                System.Console.WriteLine(System.Runtime.InteropServices.Marshal.GetLastWin32Error())
                Return False
            End If

            '' ����M�f�[�^�\���̂̃T�C�Y�� �������̈�m�� (���[���X���b�g�̍ő�l)
            'm_hRecvBufPtr = Marshal.AllocHGlobal(recvBufLen)
            'm_hSendBufPtr = Marshal.AllocHGlobal(sendBufLen)
            'm_pLockObject = New Object
            Return True
        End Function

        ''' <summary>
        ''' ���[���X���b�g�I��
        ''' </summary>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function Mail_Close() As Boolean
            '' �I�[�v���`�F�b�N(��M���[���X���b�g�̃n���h���͕K���o���Ă���͂��Ȃ̂�)
            If m_hRecvMailPtr.Equals(IntPtr.Zero) Then Return False

            '' ��M�t�@�C���J��
            CancelIo(m_hRecvMailPtr)	   '�r��IO���������
            CloseHandle(m_hRecvMailPtr)
            m_hRecvMailPtr = IntPtr.Zero
            Return True

            ' '' '' ���M�t�@�C���J��
            ' ''If Not m_hSendMailPtr.Equals(IntPtr.Zero) Then
            ' ''    CancelIo(m_hSendMailPtr)       '�r��IO���������
            ' ''    CloseHandle(m_hSendMailPtr)
            ' ''    m_hSendMailPtr = IntPtr.Zero
            ' ''End If

            ' '' ����M�������̈� �J��
            'Marshal.FreeHGlobal(m_hRecvBufPtr)
            'Marshal.FreeHGlobal(m_hSendBufPtr)
            'm_hRecvBufPtr = IntPtr.Zero
            'm_hSendBufPtr = IntPtr.Zero
            'm_pLockObject = Nothing
        End Function

        ''' <summary>
        ''' ���M
        ''' </summary>
        ''' <param name="strSendPCName">���M��PC��</param>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="byteArray">���M�f�[�^�z��</param>
        ''' <returns>-1:�֐��G���[ 1�`:���M�T�C�Y</returns>
        ''' <remarks>����t�@�C���̃I�[�v���E�N���[�Y���s��</remarks>
        Public Overloads Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal byteArray() As Byte) As Integer
            Dim handle As IntPtr					' �����݃t�@�C���n���h��
            Dim strSrotName As String				' ���[���X���b�g����
            Dim blnRetc As Boolean					' ���A���
            Dim getMailSize As Integer				' ���ۂ̏����܂ꂽ�T�C�Y

            '' ���[���X���b�g���̐���
            strSrotName = "\\" & strSendPCName & "\MailSlot\" & strSendTaskName

            '' �t�@�C������
            handle = CreateFile(strSrotName, GENERIC_WRITE, FILE_SHARE_WRITE Or FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
            If handle.ToInt32 <= 0 Then Return -9

            Try
                '' �t�@�C��������
                blnRetc = WriteFile(handle, byteArray, byteArray.Length, getMailSize, Nothing)
                If Not blnRetc Then Return -1
                Return getMailSize
            Catch ex As Exception
                Return -7
            Finally
                '' �t�@�C���J��
                CloseHandle(handle)
            End Try
        End Function

        ''' <summary>
        ''' ���M
        ''' </summary>
        ''' <param name="strSendPCName">���M��PC��</param>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="objVal">���M�\���̃f�[�^</param>
        ''' <returns>-1:�֐��G���[ 1�`:���M�T�C�Y</returns>
        ''' <remarks>����t�@�C���̃I�[�v���E�N���[�Y���s��</remarks>
        Public Overloads Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal objVal As ValueType) As Integer
            Dim handle As IntPtr					' �����݃t�@�C���n���h��
            Dim strSrotName As String				' ���[���X���b�g����
            Dim blnRetc As Boolean					' ���A���
            Dim getMailSize As Integer				' ���ۂ̏����܂ꂽ�T�C�Y

            SyncLock m_pLockObject
                '' ���[���X���b�g���̐���
                strSrotName = "\\" & strSendPCName & "\MailSlot\" & strSendTaskName

                '' �t�@�C������
                handle = CreateFile(strSrotName, GENERIC_WRITE, FILE_SHARE_WRITE Or FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
                If handle.ToInt32 <= 0 Then Return -9

                Try
                    '' �������ɑ��M�f�[�^�Z�b�g (���ꂪ��������E�E�E)
                    Marshal.StructureToPtr(objVal, m_hSendBufPtr, True)

                    '' �t�@�C��������
                    blnRetc = WriteFile(handle, m_hSendBufPtr, Marshal.SizeOf(objVal), getMailSize, Nothing)
                    If Not blnRetc Then Return -1
                    Return getMailSize
                Catch ex As Exception
                    Return -7
                Finally
                    '' �t�@�C���J��
                    CloseHandle(handle)
                End Try
            End SyncLock
        End Function

        ''' <param name="strSendPCName">���M��PC��</param>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="objVal">���M�C���X�^���X�f�[�^</param>
        Public Overloads Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal objVal As Object) As Integer
            '' �������ɑ��M�f�[�^�Z�b�g (���ꂪ��������E�E�E)
            Dim memS As New IO.MemoryStream()
            '' �o�C�i���t�H�[�}�b�^ �쐬
            Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
            '' �V���A����
            formatter.Serialize(memS, objVal)
            memS.Position = 0

            '' ���M
            Return Me.Mail_Send(strSendPCName, strSendTaskName, memS.GetBuffer())
        End Function


        '' ''----------------------------------------------------------------
        '' '' �T�v�F���M (���M���肪�����ꍇ)
        '' '' �����F(in :ValueType) objVal = ���M�f�[�^
        '' '' �ߒl�F(Integer) -1:�֐��G���[ 1�`:���M�T�C�Y
        '' '' ���l�F�K�� blnMail_SendOpen �� �Ă�ł�����
        '' ''----------------------------------------------------------------
        ' ''Public Overloads Function Mail_Send(ByVal objVal As ValueType) As Integer
        ' ''    Dim blnRetc As Boolean                  '���A���
        ' ''    Dim getMailSize As Integer              '���ۂ̏����܂ꂽ�T�C�Y

        ' ''    '' �I�[�v���`�F�b�N
        ' ''    If m_hSendMailPtr.Equals(IntPtr.Zero) Then Return -9

        ' ''    '' �������ɑ��M�f�[�^�Z�b�g (���ꂪ��������E�E�E)
        ' ''    Marshal.StructureToPtr(objVal, m_hSendBufPtr, True)

        ' ''    '' �t�@�C��������
        ' ''    blnRetc = WriteFile(m_hSendMailPtr, m_hSendBufPtr, Marshal.SizeOf(objVal), getMailSize, Nothing)
        ' ''    If Not blnRetc Then Return -1

        ' ''    Return getMailSize
        ' ''End Function

        '' ''----------------------------------------------------------------
        '' '' �T�v�F���M ����Ɛڑ�
        '' '' �����F(in :String) strSendPCName = ���MPC��
        '' '' �����F(in :String) strTaskName = ���M�^�X�N����
        '' '' �ߒl�F(Boolean) True:���� False:�ُ�
        '' '' ���l�F
        '' ''----------------------------------------------------------------
        ' ''Public Function Mail_SendOpen(ByVal strSendPCName As String, ByVal strSendTaskName As String) As Boolean
        ' ''    Dim handle As IntPtr
        ' ''    Dim strSrotName As String               '���[���X���b�g����

        ' ''    ''' �I�[�v���`�F�b�N
        ' ''    If Not m_hSendMailPtr.Equals(IntPtr.Zero) Then Return False

        ' ''    ''' ���[���X���b�g���̐���
        ' ''    strSrotName = "\\" & strSendPCName & "\MailSlot\" & strSendTaskName

        ' ''    ''' �t�@�C������
        ' ''    handle = CreateFile(strSrotName, GENERIC_WRITE, FILE_SHARE_WRITE Or FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
        ' ''    If handle.ToInt32 <= 0 Then
        ' ''        m_hSendMailPtr = IntPtr.Zero
        ' ''        Return False
        ' ''    End If
        ' ''    m_hSendMailPtr = handle
        ' ''    Return True
        ' ''End Function

        '' ''----------------------------------------------------------------
        '' '' �T�v�F���M ����Ɛؒf
        '' '' �����F( :)  = 
        '' '' �ߒl�F(Boolean) True:���� False:�ُ�
        '' '' ���l�F
        '' ''----------------------------------------------------------------
        ' ''Public Function Mail_SendClose() As Boolean
        ' ''    ''' �I�[�v���`�F�b�N
        ' ''    If m_hSendMailPtr.Equals(IntPtr.Zero) Then Return False

        ' ''    ''' �t�@�C���J��
        ' ''    CloseHandle(m_hSendMailPtr)
        ' ''    m_hSendMailPtr = IntPtr.Zero
        ' ''    Return True
        ' ''End Function

        ''' <summary>
        ''' ��M
        ''' </summary>
        ''' <param name="byteArray">��M�f�[�^</param>
        ''' <param name="recvSize">��M�T�C�Y</param>
        ''' <returns>-1:�֐��G���[ 0:��M���� 1�`:��M�T�C�Y</returns>
        ''' <remarks></remarks>
        Public Function Mail_Recv(ByRef byteArray() As Byte, Optional ByVal recvSize As Integer = gcMaxMailSize) As Integer
            Dim getMailSize As Integer		   '��M�T�C�Y
            Dim blnRetc As Boolean
            Try
                '' �t�@�C���Ǎ���
                blnRetc = ReadFile(m_hRecvMailPtr, byteArray, recvSize, getMailSize, Nothing)
                If Not blnRetc Then Return -1
                If getMailSize = 0 Then Return 0 '��M����

                '' �߂�l�Z�b�g
                Return getMailSize
            Catch ex As Exception
                Return -9
            End Try
        End Function

        ''' <summary>
        ''' ��M
        ''' </summary>
        ''' <param name="recvPtr">��M�������̈�A�h���X</param>
        ''' <param name="recvSize">��M�T�C�Y</param>
        ''' <returns>-1:�֐��G���[ 0:��M���� 1�`:��M�T�C�Y</returns>
        ''' <remarks></remarks>
        Public Function Mail_Recv(ByRef recvPtr As IntPtr, Optional ByVal recvSize As Integer = gcMaxMailSize) As Integer
            Dim getMailSize As Integer		   '��M�T�C�Y
            Dim blnRetc As Boolean
            Try
                '' �t�@�C���Ǎ���
                blnRetc = ReadFile(m_hRecvMailPtr, m_hRecvBufPtr, recvSize, getMailSize, Nothing)
                If Not blnRetc Then Return -1
                If getMailSize = 0 Then Return 0 '��M����

                '' �߂�l�Z�b�g
                recvPtr = m_hRecvBufPtr
                Return getMailSize
            Catch ex As Exception
                Return -9
            End Try
        End Function

        ''' <summary>
        ''' ��M
        ''' </summary>
        ''' <param name="objVal">��M�\����</param>
        ''' <returns>-1:�֐��G���[ 0:��M���� 1�`:��M�T�C�Y</returns>
        ''' <remarks></remarks>
        Public Function Mail_Recv(ByRef objVal As ValueType) As Integer
            Dim recvPtr As IntPtr	'��M�������̈�A�h���X
            Dim iRetc As Integer

            '' ��M
            iRetc = Mail_Recv(recvPtr, Marshal.SizeOf(objVal))
            If iRetc > 0 Then		'��M�f�[�^�L��
                '' ��M�f�[�^���w��\���̂ɃZ�b�g
                objVal = DirectCast(Marshal.PtrToStructure(recvPtr, objVal.GetType), ValueType)
            End If

            Return iRetc
        End Function

        Public Function Mail_Recv(ByRef objVal As Object, Optional ByVal recvSize As Integer = gcMaxMailSize) As Integer
            Dim iRetc As Integer
            Dim byteArray() As Byte = Nothing
            '' ��M
            iRetc = Mail_Recv(byteArray, recvSize)
            If iRetc > 0 Then		'��M�f�[�^�L��
                '' �������ɑ��M�f�[�^�Z�b�g (���ꂪ��������E�E�E)
                Dim memS As New IO.MemoryStream(byteArray)
                '' �o�C�i���t�H�[�}�b�^ �쐬
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                '' �V���A����
                objVal = formatter.Deserialize(memS)
            End If

            Return iRetc
        End Function


    End Class
End Namespace