'*********************************************************************************
'MailSlot�̑���M���s����{�N���X
'*********************************************************************************
Option Strict On                'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices
Imports Microsoft.Win32.SafeHandles
Imports System.IO

''''' �\���̂�CommonQueMix��CommonQueFL���g�����ƁB

Namespace tClass
    ''' <summary>
    ''' MailSlot�̑��M���s���N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class MailSlot

        ''' <summary>
        ''' Windows API��A����ȏ�͑��M�ł��Ȃ��A�Ƃ��������B
        ''' </summary>
        Public Const MAX_MAILSLOT_SIZE As Integer = 512
        Protected Const GENERIC_WRITE As Integer = &H40000000         ' �I�u�W�F�N�g�ւ̏������݃A�N�Z�X
        Protected Const FILE_SHARE_READ As Integer = &H1
        Protected Const FILE_SHARE_WRITE As Integer = &H2
        Protected Const OPEN_EXISTING As Integer = 3
        Friend Class SafeNativeMethods
            <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
            Friend Shared Function CreateFile( _
                <System.Runtime.InteropServices.In()> ByVal lpFileName As String, _
                ByVal dwDesiredAccess As Integer, _
                ByVal dwShareMode As Integer, _
                ByVal lpSecurityAttributes As IntPtr, _
                ByVal dwCreationDisposition As Integer, _
                ByVal dwFlagsAndAttributes As Integer, _
                ByVal hTemplateFile As IntPtr) _
              As SafeFileHandle
            End Function

            <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
            Friend Shared Function WriteFile( _
                ByVal hFile As SafeFileHandle, _
                <System.Runtime.InteropServices.In(), MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=2)> ByVal lpBuffer As Byte(), _
                ByVal nNumberOfBytesToWrite As Integer, _
                <Out()> ByRef lpNumberOfBytesWritten As Integer, _
                ByVal lpOverlapped As IntPtr) _
              As Boolean
            End Function


            '' ���[���X���b�g����
            <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
            Friend Shared Function CreateMailslot( _
                     ByVal lpName As String, _
                     ByVal nMaxMessageSize As Integer, _
                     ByVal lReadTimeout As Integer, _
                     ByVal lpSecurityAttributes As IntPtr) As SafeFileHandle
            End Function

            <DllImport("kernel32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
            Friend Shared Function CancelIo(ByVal hFile As SafeFileHandle) As Boolean
            End Function

            <DllImport("kernel32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
            Friend Shared Function CancelIoEx(ByVal hFile As SafeFileHandle, ByVal pOverlapped As IntPtr) As Boolean
            End Function
        End Class


        Protected Shared Function OpenMailslot(targetpc As String, task As String) As SafeFileHandle
            Dim mailslot_name As String = String.Format("\\{0}\MailSlot\{1}", targetpc, task)
            Return SafeNativeMethods.CreateFile(mailslot_name, GENERIC_WRITE, FILE_SHARE_READ + FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero)
        End Function

        Protected Shared Function doSend(targetpc As String, task As String, msg() As Byte) As Integer
            Using hFile As SafeFileHandle = OpenMailslot(targetpc, task)
                If Not hFile.IsInvalid Then
                    Dim nWritten As Integer = 0
                    If SafeNativeMethods.WriteFile(hFile, msg, msg.Length, nWritten, IntPtr.Zero) Then
                        Return nWritten
                    End If
                    Return -Marshal.GetLastWin32Error()
                End If
            End Using
            Return -Marshal.GetLastWin32Error()
        End Function

        ''' <summary>
        ''' ���[���X���b�g���M
        ''' </summary>
        ''' <param name="strSendPCName">���M��PC��</param>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="byteArray">���M�f�[�^�z��</param>
        ''' <returns>��:�֐��G���[ 1�`:���M�T�C�Y</returns>
        ''' <remarks>����t�@�C���̃I�[�v���E�N���[�Y���s��</remarks>
        Public Overloads Shared Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal byteArray() As Byte) As Integer
            Return doSend(strSendPCName, strSendTaskName, byteArray)
        End Function


        ''' <summary>
        ''' ���[���X���b�g���M
        ''' </summary>
        ''' <param name="strSendPCName">���M��PC��</param>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="que">���M�L���[</param>
        ''' <returns>��:�֐��G���[ 1�`:���M�T�C�Y</returns>
        Public Overloads Shared Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal que As tClass.CommonQueBase) As Integer
            Return doSend(strSendPCName, strSendTaskName, que.ToByteArray())
        End Function

        ''' <summary>
        ''' ���[���X���b�g���M�B�݊����̂��߂Ɏc���Ă���B
        ''' </summary>
        <Obsolete("�f�[�^���ɂ́Atclass.CommonQue�n�̃N���X���g�p���Ă�������")> _
        Public Overloads Shared Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal objVal As Object) As Integer
            Dim byArray(Marshal.SizeOf(objVal) - 1) As Byte
            Using pin_ptr As New PinnedPtr(byArray)
                Marshal.StructureToPtr(objVal, pin_ptr.Ptr, False)
            End Using
            Return doSend(strSendPCName, strSendTaskName, byArray)
        End Function

        ''' <summary>
        ''' �u���[�h�L���X�g�Ń��[���X���b�g���M
        ''' </summary>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="que">���M�L���[</param>
        ''' <returns>��:�֐��G���[ 1�`:���M�T�C�Y</returns>
        ''' <remarks>����t�@�C���̃I�[�v���E�N���[�Y���s��</remarks>
        Public Shared Function Mail_Send_All(ByVal strSendTaskName As String, ByVal que As tClass.CommonQueBase) As Integer
            Return doSend("*", strSendTaskName, que.ToByteArray())
        End Function

        ''' <summary>
        ''' �u���[�h�L���X�g�Ń��[���X���b�g���M
        ''' </summary>
        ''' <param name="strSendTaskName">���M�惁�[���X���b�g����</param>
        ''' <param name="que">���M�L���[</param>
        ''' <returns>��:�֐��G���[ 1�`:���M�T�C�Y</returns>
        ''' <remarks>����t�@�C���̃I�[�v���E�N���[�Y���s��</remarks>
        Public Shared Function Mail_Send_Mypc(ByVal strSendTaskName As String, ByVal que As tClass.CommonQueBase) As Integer
            Return doSend(".", strSendTaskName, que.ToByteArray())
        End Function
    End Class


    ''' <summary>
    ''' Mailslot�̎�M���s���N���X�B
    ''' </summary>
    ''' <remarks></remarks>
    Public Class MailSlotServer(Of T As {new,  CommonQueBase})
        Implements IDisposable                          '�f�X�g���N�^��`

        Protected m_mailslot_name As String


        Private Function CreateMailslot(task As String) As Boolean
            m_mailslot_name = String.Format("\\.\MailSlot\{0}", task)
            m_hMailslot = MailSlot.SafeNativeMethods.CreateMailslot(m_mailslot_name, MailSlot.MAX_MAILSLOT_SIZE, -1, IntPtr.Zero)
            If Not m_hMailslot.IsInvalid Then
                m_fs = New System.IO.FileStream(m_hMailslot, System.IO.FileAccess.Read, MailSlot.MAX_MAILSLOT_SIZE, True)
                Return True
            End If
            Return False
        End Function


        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Protected m_hMailslot As SafeFileHandle
        Protected m_fs As FileStream

        '��M��荞�݃C�x���g. BeginInvoke���s�������̂ŁA�J�X�^���C�x���g�`��
        Public Delegate Sub tDele_GetMail(ByVal que As T) '��M��荞�݃f���Q�[�g
        Protected m_Control As System.Windows.Forms.Control      '��M��荞�ݗp�f���Q�[�g�I�u�W�F�N�g(�R���g���[���A�N�Z�X��)
        Private EventHandlerList As New List(Of tDele_GetMail)
        Public Custom Event tEv_RecvMail As tDele_GetMail
            AddHandler(ByVal value As tDele_GetMail)
                EventHandlerList.Add(value)
            End AddHandler
            RemoveHandler(ByVal value As tDele_GetMail)
                EventHandlerList.Remove(value)
            End RemoveHandler
            RaiseEvent(que As T)
                For Each handler As tDele_GetMail In EventHandlerList
                    If handler IsNot Nothing Then
                        handler.Invoke(que)
                    End If
                Next
            End RaiseEvent
        End Event



        ''' <summary>
        ''' ���[���X���b�g�T�[�o���J��
        ''' </summary>
        ''' <param name="strMyTaskName">���[���X���b�g���́��������g�̃^�X�N����</param>
        ''' <param name="interval">���͂�_�~�[�p�����[�^</param>
        ''' <remarks>�J���Ȃ��Ɨ�O����т܂��B</remarks>
        Public Sub New(ByVal strMyTaskName As String, Optional ByVal interval As Integer = 300)
            '''' ���[���X���b�g����
            If Not CreateMailslot(strMyTaskName) Then
                'Debug.Assert(False, "���[���X���b�g�������s")
                Throw New System.ComponentModel.Win32Exception("���[���X���b�g�������s")
            End If
        End Sub

        Public Sub New()
        End Sub

        Public Function StartInit(ByVal strTaskForm As System.Windows.Forms.Form) As Boolean
            If Not CreateMailslot(strTaskForm.ToString) Then
                Return False
            End If
            ReadStart(strTaskForm)
            Return True
        End Function

        ''' <summary>
        ''' �ق��̃^�X�N�Ȃǂ��A���[���X���b�g���J���Ă��邩�ǂ����H
        ''' </summary>
        ''' <param name="strTask"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function IsOpened(strTask As String) As Boolean
            Dim mailslot_name As String = String.Format("\\.\MailSlot\{0}", strTask)
            Using h As SafeFileHandle = MailSlot.SafeNativeMethods.CreateMailslot(mailslot_name, MailSlot.MAX_MAILSLOT_SIZE, -1, IntPtr.Zero)
                Return False
            End Using
            Return True
        End Function

        Public Sub Dispose() Implements IDisposable.Dispose
            If m_fs IsNot Nothing Then
                ReadStop()
                Dim fs As IDisposable = m_fs
                System.Threading.Interlocked.Exchange(m_fs, Nothing)
                fs.Dispose()
            End If

            m_hMailslot.Close()
            GC.SuppressFinalize(Me)
        End Sub


        Private m_bReading As Boolean = False

        Sub doRecvStart()
            m_bReading = True
            Dim buf(MailSlot.MAX_MAILSLOT_SIZE - 1) As Byte
            m_fs.BeginRead(buf, 0, MailSlot.MAX_MAILSLOT_SIZE, AddressOf OnRecvMail, buf)
        End Sub

        ''' <summary>
        ''' ��M�J�n�B
        ''' </summary>
        ''' <remarks>�R���g���[���ɂ͐G��܂���B</remarks>
        Public Sub ReadStart()
            If Not m_bReading Then
                m_Control = Nothing
                doRecvStart()
            End If
        End Sub


        ''' <summary>
        ''' ��M�J�n
        ''' </summary>
        ''' <param name="meControl">�R���g���[���B����������Form��Me�Ƃ��B</param>
        ''' <remarks>���C���X���b�h�ɑ����Ă���΂ǂ�ł�����</remarks>
        Public Sub ReadStart(ByVal meControl As System.Windows.Forms.Control)
            If Not m_bReading Then
                m_Control = meControl
                doRecvStart()
            End If
        End Sub


        Private Shared m_CancelIOExSupported As Integer
        ''' <summary>
        ''' ���s���̔񓯊��ǂݎ����L�����Z������
        ''' </summary>
        ''' <param name="hFile">�t�@�C���n���h��</param>
        ''' <remarks></remarks>
        Sub CancelIo(ByVal hFile As SafeFileHandle)
            If m_CancelIOExSupported = 0 Then
                Dim kernel As New NativeDLL("kernel32")
                If kernel.GetAddress("CancelIOEx") <> IntPtr.Zero Then
                    m_CancelIOExSupported = 1
                Else
                    m_CancelIOExSupported = -1
                End If
            End If

            If m_CancelIOExSupported > 0 Then
                MailSlot.SafeNativeMethods.CancelIoEx(hFile, IntPtr.Zero)
            ElseIf m_CancelIOExSupported < 0 Then
                MailSlot.SafeNativeMethods.CancelIo(hFile)
            End If
        End Sub
        ''' <summary>
        ''' ��M��~
        ''' </summary>
        Public Sub ReadStop()

            CancelIo(m_hMailslot)
        End Sub

        ''' <summary>
        ''' �񓯊���M����
        ''' </summary>
        ''' <param name="ar"></param>
        ''' <remarks></remarks>
        Protected Overridable Sub OnRecvMail(ar As IAsyncResult)
            m_bReading = False
            'Dispose�ɔ����ACancel�C�x���g
            If m_fs Is Nothing Then Exit Sub

            '�ǂݎ�茋�ʂ��擾�ACommonQue�^�ɕϊ��E
            Try
                Dim read_buf() As Byte = DirectCast(ar.AsyncState, Byte())
                Dim read_size As Integer = m_fs.EndRead(ar)
                Dim que As New T
                que.Read(read_buf, read_size)

                '���̂�݂���
                m_fs.BeginRead(read_buf, 0, MailSlot.MAX_MAILSLOT_SIZE, AddressOf OnRecvMail, read_buf)
                m_bReading = True

                '��M�C�x���g��������
                If m_Control Is Nothing Then
                    RaiseEvent tEv_RecvMail(que)
                Else
                    For Each deleGetMail As tDele_GetMail In EventHandlerList
                        m_Control.BeginInvoke(deleGetMail, que)
                    Next
                End If
                Return
            Catch e As OperationCanceledException
                'CancelIO�����s���ꂽ�B�Ƃ肠�����A����BeginRead�͂�߂Ƃ��B
                Return
            End Try

            doRecvStart()
        End Sub
    End Class
End Namespace