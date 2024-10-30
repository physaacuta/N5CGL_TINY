'*********************************************************************************
'RS-232C�ʐM�N���X
'  ��M�͔񓯊��Ή�
'  ��M�o�b�t�@�͓��I�z����g�p�ׁ̈A�T�C�Y�w�薳��
'*********************************************************************************
Option Strict On                'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Runtime.InteropServices              'MarshalAs�N���X���X
Imports System.Text                                 'StringBuilder�N���X

#Const tDebug = False            'True�� �f�o�b�N���[�h

Namespace tClass
    '    '********************************************************************
    '    ' API�萔 ��`
    '    '********************************************************************
    '    Friend Module tAPI_232C
    '        ''''''''' �\���� & �萔
    '        ''' �A�N�Z�X����
    '        Public Const GENERIC_READ As Integer = &H80000000         ' �I�u�W�F�N�g�ւ̓ǂݎ��A�N�Z�X
    '        Public Const GENERIC_WRITE As Integer = &H40000000        ' �I�u�W�F�N�g�ւ̏������݃A�N�Z�X
    '        Public Const FILE_SHARE_READ As Integer = &H1
    '        Public Const FILE_SHARE_WRITE As Integer = &H2
    '        Public Const OPEN_EXISTING As Integer = 3
    '        Public Const FILE_ATTRIBUTE_NORMAL As Integer = &H80
    '        Public Const FILE_FLAG_OVERLAPPED As Integer = &H40000000

    '        ''' �񓯊� �t�@�C���Ǐ��\����
    '        Public Structure OVERLAPPED
    '            Dim Internal As Integer
    '            Dim InternalHigh As Integer
    '            Dim Offset As Integer
    '            Dim OffsetHigh As Integer
    '            Dim hEvent As IntPtr
    '        End Structure

    '        ''' �ʐM�ݒ� �\����
    '        Public Structure DCB
    '            Public DCBlength As Integer         '�\���̂̃T�C�Y
    '            Public BaudRate As Integer          '�{�[���C�g�ibps�j�̐ݒ�
    '            Public fBitFields As Integer        '�r�b�g�P�ʂ̃t�B�[���h��`
    '            Public wReserved As Short           '�\��i0���Z�b�g����j
    '            Public XonLim As Short              '��M�o�b�t�@���̃f�[�^�����o�C�g�ɂȂ�����Xon�����𑗂邩���w��
    '            Public XoffLim As Short             '��M�o�b�t�@�̋󂫂����o�C�g�ɂȂ�����Xoff�����𑗂邩���w��
    '            Public ByteSize As Byte             '1�f�[�^�̃r�b�g�����w��
    '            Public Parity As Byte               '�p���e�B�̕������w��
    '            Public StopBits As Byte             '�X�g�b�v�r�b�g�����w��
    '            Public XonChar As Byte              'Xon�������w��
    '            Public XoffChar As Byte             'Xoff�������w��
    '            Public ErrorChar As Byte            '�p���e�B�G���[�̏ꍇ�Ɏg���������w��
    '            Public EofChar As Byte              '��o�C�i�����[�h�̏ꍇ�̃f�[�^�I�������̎w��
    '            Public EvtChar As Byte              '�C�x���g�𐶐����镶�����w��
    '            Public wReserved1 As Short          '�\�񂳂�Ă��܂��B�g�p���Ȃ��ł��������B
    '        End Structure

    '        ''' �^�C���A�E�g �\����
    '        Public Structure COMMTIMEOUTS
    '            Public ReadIntervalTimeout As Integer           '�����̓ǂݍ��݂̑҂�����
    '            Public ReadTotalTimeoutMultiplier As Integer    '�ǂݍ��݂̂P����������̎���
    '            Public ReadTotalTimeoutConstant As Integer      '�ǂݍ��݂̒萔����
    '            Public WriteTotalTimeoutMultiplier As Integer   '�������݂̂P����������̎���
    '            Public WriteTotalTimeoutConstant As Integer     '�������݂̒萔����
    '        End Structure

    '        '' �񓯊��Ŏg�����Ǝv�������� ����ς胄��
    '        ''''' �C�x���g����
    '        ''<DllImport("KERNEL32.dll", EntryPoint:="CreateEvent", SetLastError:=True)> _
    '        ''Public Function CreateEvent( _
    '        ''        ByVal lpEventAttributes As IntPtr, _
    '        ''        ByVal bManualReset As Boolean, _
    '        ''        ByVal bInitialState As Boolean, _
    '        ''        <MarshalAs(UnmanagedType.LPStr)> ByVal lpName As String) _
    '        ''    As IntPtr
    '        ''End Function
    '        ''Public Declare Function GetOverlappedResult Lib "kernel32" Alias "GetOverlappedResult" (ByVal hFile As IntPtr, ByRef lpOverlapped As OVERLAPPED, ByRef lpNumberOfBytesTransferred As Integer, ByVal bWait As Integer) As Integer



    '        ''''''''' API
    '        ''' �t�@�C������
    '        <DllImport("KERNEL32.DLL", EntryPoint:="CreateFileA", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
    '        Public Function CreateFile( _
    '                <MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String, _
    '                ByVal dwDesiredAccess As Integer, _
    '                ByVal dwShareMode As Integer, _
    '                ByVal lpSecurityAttributes As Integer, _
    '                ByVal dwCreationDisposition As Integer, _
    '                ByVal dwFlagsAndAttributes As Integer, _
    '                ByVal hTemplateFile As Integer) _
    '            As IntPtr
    '            'As Integer
    '        End Function

    '        ''' �n���h���j��
    '        <DllImport("KERNEL32.dll", EntryPoint:="CloseHandle", SetLastError:=True)> _
    '        Public Function CloseHandle( _
    '                ByVal hObject As IntPtr) _
    '            As Boolean
    '        End Function
    '        <DllImport("KERNEL32.dll", EntryPoint:="CloseHandle", SetLastError:=True)> _
    '        Public Function CloseHandle( _
    '                ByVal hObject As Integer) _
    '            As Boolean
    '        End Function

    '        ''' �t�@�C���Ǎ���
    '        <DllImport("KERNEL32.dll", EntryPoint:="ReadFile", SetLastError:=True)> _
    '        Public Function ReadFile( _
    '                ByVal hFile As IntPtr, _
    '                ByVal lpBuffer As Byte(), _
    '                ByVal nNumberOfBytesToRead As Integer, _
    '                ByRef lpNumberOfBytesRead As Integer, _
    '                ByRef lpOverlapped As OVERLAPPED) _
    '            As Boolean
    '        End Function
    '        <DllImport("KERNEL32.dll", EntryPoint:="ReadFile", SetLastError:=True)> _
    '        Public Function ReadFile( _
    '                ByVal hFile As IntPtr, _
    '                ByVal lpBuffer As IntPtr, _
    '                ByVal nNumberOfBytesToRead As Integer, _
    '                ByRef lpNumberOfBytesRead As Integer, _
    '                ByRef lpOverlapped As OVERLAPPED) _
    '            As Boolean
    '        End Function

    '        ''' �t�@�C��������
    '        <DllImport("KERNEL32.dll", EntryPoint:="WriteFile", SetLastError:=True)> _
    '        Public Function WriteFile( _
    '                ByVal hFile As IntPtr, _
    '                ByVal lpBuffer As Byte(), _
    '                ByVal nNumberOfBytesToWrite As Integer, _
    '                ByRef lpNumberOfBytesWritten As Integer, _
    '                ByRef lpOverlapped As OVERLAPPED) _
    '            As Boolean
    '        End Function
    '        <DllImport("KERNEL32.dll", EntryPoint:="WriteFile", SetLastError:=True)> _
    '        Public Function WriteFile( _
    '                ByVal hFile As IntPtr, _
    '                ByVal lpBuffer As IntPtr, _
    '                ByVal nNumberOfBytesToWrite As Integer, _
    '                ByRef lpNumberOfBytesWritten As Integer, _
    '                ByRef lpOverlapped As OVERLAPPED) _
    '            As Boolean
    '        End Function

    '        ''' I/O����̎��������s�� (FILE_FLAG_OVERLAPPED ���̂ݗL��)
    '        <DllImport("KERNEL32.dll", EntryPoint:="CancelIo", SetLastError:=True)> _
    '        Public Function CancelIo( _
    '                ByVal hFile As IntPtr) _
    '            As Boolean
    '        End Function

    '        ''' �V���A���|�[�g�ݒ�
    '        <DllImport("KERNEL32.dll", EntryPoint:="GetCommState", SetLastError:=True)> _
    '        Public Function GetCommState( _
    '                ByVal nCid As IntPtr, _
    '                ByRef lpDCB As DCB) _
    '            As Boolean
    '        End Function
    '        <DllImport("KERNEL32.dll", EntryPoint:="SetCommState", SetLastError:=True)> _
    '        Public Function SetCommState( _
    '                ByVal nCid As IntPtr, _
    '                ByRef lpDCB As DCB) _
    '            As Boolean
    '        End Function

    '        ''' �^�C���A�E�g�ݒ�
    '        <DllImport("KERNEL32.dll", EntryPoint:="GetCommTimeouts", SetLastError:=True)> _
    '        Public Function GetCommTimeouts( _
    '                ByVal hFile As IntPtr, _
    '                ByRef lpCommTimeouts As COMMTIMEOUTS) _
    '            As Boolean
    '        End Function
    '        <DllImport("KERNEL32.dll", EntryPoint:="SetCommTimeouts", SetLastError:=True)> _
    '        Public Function SetCommTimeouts( _
    '                ByVal hFile As IntPtr, _
    '                ByRef lpCommTimeouts As COMMTIMEOUTS) _
    '            As Boolean
    '        End Function

    '        ''' �o�b�t�@������
    '        Public Const PURGE_TXABORT As Integer = &H1     '// Kill the pending/current writes to the comm port.
    '        Public Const PURGE_RXABORT As Integer = &H2     '// Kill the pending/current reads to the comm port.
    '        Public Const PURGE_TXCLEAR As Integer = &H4     '// Kill the transmit queue if there.
    '        Public Const PURGE_RXCLEAR As Integer = &H8     '// Kill the typeahead buffer if there.

    '        <DllImport("KERNEL32.dll", EntryPoint:="PurgeComm", SetLastError:=True)> _
    '        Public Function PurgeComm( _
    '                ByVal hFile As IntPtr, _
    '                ByVal dwFlags As Integer) _
    '            As Boolean
    '        End Function

    '        ''' �o�b�t�@�T�C�Y�w��
    '        <DllImport("KERNEL32.dll", EntryPoint:="SetupComm", SetLastError:=True)> _
    '        Public Function SetupComm( _
    '                ByVal hFile As IntPtr, _
    '                ByVal dwInQueue As Integer, _
    '                ByVal dwOutQueue As Integer) _
    '            As Boolean
    '        End Function

    '        '''''' �C�x���g����
    '        '''<DllImport("KERNEL32.dll", EntryPoint:="CreateEvent", SetLastError:=True)> _
    '        '''Public Function CreateEvent( _
    '        '''        ByVal lpEventAttributes As IntPtr, _
    '        '''        ByVal bManualReset As Boolean, _
    '        '''        ByVal bInitialState As Boolean, _
    '        '''        <MarshalAs(UnmanagedType.LPStr)> ByVal lpName As String) _
    '        '''    As IntPtr
    '        '''End Function
    '    End Module


    '    '********************************************************************
    '    '�󂯓n���N���X
    '    '********************************************************************
    '#Region "�󂯓n���N���X"
    '    ''' ��M (�^�C���A�E�g��)
    '    Public Class tClass_232C_State_Recv
    '        Public buffer() As Byte                     '��M�f�[�^�i�[�̈�
    '    End Class
    '    ''' ���M (�^�C���A�E�g��)
    '    Public Class tClass_232C_State_Send
    '        Public buffer() As Byte                     '���M�f�[�^�i�[�̈�
    '    End Class
    '    ''' �ُ�
    '    Public Class tClass_232C_State_Error
    '        Public cls232C As tClass_232C               '232C�N���X

    '        ''' �R���X�g���N�^
    '        Public Sub New(ByVal cls As tClass_232C)
    '            ''' ������
    '            cls232C = cls
    '        End Sub
    '    End Class
    '#End Region


    '    '********************************************************************
    '    ' 232C �ʐM 
    '    '********************************************************************
    '    Public Class tClass_232C

    '        '********************************************************************
    '        '�񓯊��ŌĂяo�����\�b�h�Ɠ����V�O�l�`���̃f���Q�[�g���`
    '        '********************************************************************
    '        Public Delegate Sub tDele_232C_Event(ByVal sender As Object, ByVal e As tEnum_232C_EventNO)             '�񓯊��f���Q�[�g
    '        ''' �f���Q�[�g �C�x���g
    '        Public Enum tEnum_232C_EventNO
    '            '����
    '            RECV = 1                '��M����
    '            SEND = 2                '���M����

    '            '�ُ�
    '            RECV_ERROR = -1         '��M�ُ�
    '            SEND_ERROR = -2         '���M�ُ�
    '            RECV_TIMEOUT = -3       '��M�^�C���A�E�g
    '            SEND_TIMEOUT = -4       '���M�^�C���A�E�g
    '            RECV_STX_ERROR = -5     '��M�f�[�^�ꕔ�j��
    '            RECV_ETX_ERROR = -6     '��M�f�[�^�ꕔ�j��

    '            '�O������̋N���p
    '            OTHER = 100            '�O������̋N���p
    '            OTHER1
    '            OTHER2
    '            OTHER3
    '            OTHER4
    '            OTHER5
    '            OTHER6
    '            OTHER7
    '            OTHER8
    '            OTHER9
    '        End Enum

    '        ''' ��Ԋi�[�p�\����
    '        Public Structure tClass_232C_Status
    '            ''���
    '            Dim RecvWait As Boolean     'True:��M�� Flase:��������
    '            Dim SendWait As Boolean     'True:���M�� Flase:��������
    '        End Structure

    '        '********************************************************************
    '        '�����o�[�ϐ���`
    '        '********************************************************************
    '        Private m_hCom As IntPtr                            'COM�|�[�g
    '        Private m_DCB As tAPI_232C.DCB                      'DCB
    '        Private m_Status As tClass_232C_Status              '232C�ʐM���

    '        Private m_WThreadRecv As tClass_WorkerThread        '��M�X���b�h
    '        Private m_WThreadSend As tClass_WorkerThread        '���M�X���b�h

    '        Private m_Dele232CEvent As tDele_232C_Event         '�A���T�[�f���Q�[�g

    '        Private m_Recv_STX() As Byte                        '��M�`���̊J�n����p(�ϒ��̂�)
    '        Private m_Recv_ETX() As Byte                        '��M�`���̏I�[����p(�ϒ��̂�)
    '        Private m_Send_STX() As Byte                        '���M�t���p
    '        Private m_Send_ETX() As Byte                        '���M�t���p
    '        Private m_TimeOutInterval As Integer = 1000         '����M�^�C���A�E�g�ݒ� [ms]

    '        ''' ��M�p
    '        Private m_RecvMode As Boolean                       '��M���[�h False:�ϒ� True:�Œ蒷
    '        Private m_RecvNowBuf() As Byte                      '��M�o�b�t�@
    '        Private m_RecvBuf As IO.MemoryStream                '��M�`���o�b�t�@(�S��)
    '        ''' ���M�p
    '        Private m_SendNowBuf() As Byte                      '���M�o�b�t�@


    '        '********************************************************************
    '        '�v���p�e�B��`
    '        '********************************************************************
    '#Region "�v���p�e�B��`"
    '        ''' DCB�Z�b�g����
    '        Public Enum ge_DCB_BaudRate             '�{�[���[�g
    '            i300 = 300
    '            i600 = 600
    '            i1200 = 1200
    '            i1800 = 1800
    '            i2400 = 2400
    '            i4800 = 4800
    '            i9600 = 9600
    '            i14400 = 14400
    '            i19200 = 19200
    '            i38400 = 38400
    '        End Enum
    '        Public Enum ge_DCB_ByteSize             '�r�b�g��
    '            i4 = 4
    '            i5 = 5
    '            i6 = 6
    '            i7 = 7
    '            i8 = 8
    '        End Enum
    '        Public Enum ge_DCB_StopBits             '�X�g�b�v�r�b�g
    '            i1 = 0
    '            i1_5 = 1
    '            i2 = 2
    '        End Enum
    '        Public Enum ge_DCB_Parity               '�p���e�B�[
    '            no = 0
    '            odd = 1
    '            even = 2
    '            mark = 3
    '            space = 4
    '        End Enum

    '        '====================================================================
    '        'DCB �ݒ�
    '        '====================================================================
    '        Public Property tProp_DCB_BaudRate() As ge_DCB_BaudRate
    '        Get
    '            Return CType(m_DCB.BaudRate, ge_DCB_BaudRate)
    '        End Get
    '        Set(ByVal Value As ge_DCB_BaudRate)
    '            m_DCB.BaudRate = Value
    '        End Set
    '        End Property
    '        Public Property tProp_DCB_ByteSize() As ge_DCB_ByteSize
    '        Get
    '            Return CType(m_DCB.ByteSize, ge_DCB_ByteSize)
    '        End Get
    '        Set(ByVal Value As ge_DCB_ByteSize)
    '            m_DCB.ByteSize = CByte(Value)
    '        End Set
    '        End Property
    '        Public Property tProp_DCB_StopBits() As ge_DCB_StopBits
    '        Get
    '            Return CType(m_DCB.StopBits, ge_DCB_StopBits)
    '        End Get
    '        Set(ByVal Value As ge_DCB_StopBits)
    '            m_DCB.StopBits = CByte(Value)
    '        End Set
    '        End Property
    '        Public Property tProp_DCB_Parity() As ge_DCB_Parity
    '        Get
    '            Return CType(m_DCB.Parity, ge_DCB_Parity)
    '        End Get
    '        Set(ByVal Value As ge_DCB_Parity)
    '            m_DCB.Parity = CByte(Value)
    '        End Set
    '        End Property

    '        '====================================================================
    '        '��M�`�� �J�n�R�[�h(�ϒ��̂�)
    '        '====================================================================
    '        Public Property tProp_Recv_STX() As Byte()
    '        Get
    '            Return m_Recv_STX
    '        End Get
    '        Set(ByVal Value As Byte())
    '            ReDim m_Recv_STX(Value.Length - 1)
    '            Value.CopyTo(m_Recv_STX, 0)
    '        End Set
    '        End Property
    '        '====================================================================
    '        '��M�`�� �I���R�[�h(�ϒ��̂�)
    '        '====================================================================
    '        Public Property tProp_Recv_ETX() As Byte()
    '        Get
    '            Return m_Recv_ETX
    '        End Get
    '        Set(ByVal Value As Byte())
    '            ReDim m_Recv_ETX(Value.Length - 1)
    '            Value.CopyTo(m_Recv_ETX, 0)
    '        End Set
    '        End Property

    '        '====================================================================
    '        '���M�`�� �J�n�R�[�h
    '        '====================================================================
    '        Public Property tProp_Send_STX() As Byte()
    '        Get
    '            Return m_Send_STX
    '        End Get
    '        Set(ByVal Value As Byte())
    '            ReDim m_Send_STX(Value.Length - 1)
    '            Value.CopyTo(m_Send_STX, 0)
    '        End Set
    '        End Property
    '        '====================================================================
    '        '���M�`�� �I���R�[�h
    '        '====================================================================
    '        Public Property tProp_Send_ETX() As Byte()
    '        Get
    '            Return m_Send_ETX
    '        End Get
    '        Set(ByVal Value As Byte())
    '            ReDim m_Send_ETX(Value.Length - 1)
    '            Value.CopyTo(m_Send_ETX, 0)
    '        End Set
    '        End Property

    '        '====================================================================
    '        '����M�^�C���A�E�g [ms]
    '        '====================================================================
    '        Public Property tProp_TimeOutInterval() As Integer
    '        Get
    '            Return m_TimeOutInterval
    '        End Get
    '        Set(ByVal Value As Integer)
    '            m_TimeOutInterval = Value
    '        End Set
    '        End Property
    '#End Region

    '        '********************************************************************
    '        '���\�b�h��`
    '        '********************************************************************
    '        '----------------------------------------------------------------
    '        ' �T�v�F�R���X�g���N�^
    '        ' �����F(in :tDele_232C_Event) deleMethod = �A���T�[ �� ���b�\�h�A�h���X
    '        '----------------------------------------------------------------
    '        Public Sub New(ByVal deleMethod As tDele_232C_Event)
    '            ''' �����Z�b�g
    '            m_Dele232CEvent = deleMethod
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' �T�v�F�I�[�v��
    '        ' �����F(in :String) strPort = �I�[�v���|�[�g�ԍ�
    '        ' �����F(in :Integer) iRecvMaxSize = ��M�o�b�t�@�ő�l
    '        ' �����F(in :Integer) iSendMaxSize = ���M�o�b�t�@�ő�l
    '        ' �ߒl�F(Boolean) True:���� False:�ُ�
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Public Function Open(ByVal strPort As String, Optional ByVal iRecvMaxSize As Integer = 1024, Optional ByVal iSendMaxSize As Integer = 1024) As Boolean
    '            Dim blnRetc As Boolean
    '            Dim DCB As tAPI_232C.DCB

    '            ''' �I�[�v��
    '            m_hCom = tAPI_232C.CreateFile(strPort, tAPI_232C.GENERIC_READ Or tAPI_232C.GENERIC_WRITE, 0, Nothing, tAPI_232C.OPEN_EXISTING, tAPI_232C.FILE_ATTRIBUTE_NORMAL, Nothing)
    '            If m_hCom.ToInt32 <= 0 Then Return False

    '            ''' �o�b�t�@�T�C�Y�w��
    '            blnRetc = tAPI_232C.SetupComm(m_hCom, iRecvMaxSize, iSendMaxSize)

    '            ''' �|�[�g�̐ݒ�
    '            blnRetc = tAPI_232C.GetCommState(m_hCom, DCB)
    '            DCB.BaudRate = m_DCB.BaudRate
    '            DCB.ByteSize = m_DCB.ByteSize
    '            DCB.Parity = m_DCB.Parity
    '            DCB.StopBits = m_DCB.StopBits
    '            blnRetc = tAPI_232C.SetCommState(m_hCom, DCB)

    '            ''' �^�C���A�E�g�ݒ�
    '            Dim ComTimeOut As tAPI_232C.COMMTIMEOUTS      'ComTimeOut
    '            'blnRetc = GetCommTimeouts(m_hCom, m_ComTimeOut)
    '            ComTimeOut.ReadIntervalTimeout = 100
    '            ComTimeOut.ReadTotalTimeoutConstant = 100
    '            ComTimeOut.ReadTotalTimeoutMultiplier = 0
    '            ComTimeOut.WriteTotalTimeoutConstant = 100
    '            ComTimeOut.WriteTotalTimeoutMultiplier = 0
    '            blnRetc = tAPI_232C.SetCommTimeouts(m_hCom, ComTimeOut)

    '            ''' �o�b�t�@������
    '            blnRetc = tAPI_232C.PurgeComm(m_hCom, PURGE_TXABORT Or PURGE_RXABORT Or PURGE_TXCLEAR Or PURGE_RXCLEAR)

    '            ''' ��ԏ�����
    '            With m_Status
    '                .RecvWait = False
    '                .SendWait = False
    '            End With

    '            '''' ���C���X���b�h�N��
    '            m_WThreadRecv = New tClass_WorkerThread
    '            m_WThreadSend = New tClass_WorkerThread
    '            Return True
    '        End Function

    '        '----------------------------------------------------------------
    '        ' �T�v�F�N���[�Y
    '        ' �����F(in :)  = 
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Public Sub Close()
    '            Dim blnRetc As Boolean
    '            If m_hCom.ToInt32 <= 0 Then Return
    '            Send_Stop()
    '            Recv_Stop()
    '            m_WThreadSend = Nothing
    '            m_WThreadRecv = Nothing
    '            blnRetc = CloseHandle(m_hCom)
    '            m_hCom = IntPtr.Zero
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' �T�v�F��������
    '        ' �����F(in :Byte) byteBuffer = ���M�o�C�g�z��
    '        ' �ߒl�F(Integer) ���M�T�C�Y (�G���[��0)
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Public Function Send(ByVal byteBuffer() As Byte) As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '�ʐM�|�[�g�N���[�Y
    '            If m_Status.SendWait Then Return -2 '�O��̑��M��
    '            If byteBuffer.Length < 1 Then Return -3 '�����ُ�

    '            ''' ���M�̈�m��
    '            ReDim m_SendNowBuf(byteBuffer.Length - 1)
    '            Array.Copy(byteBuffer, m_SendNowBuf, byteBuffer.Length)

    '            ''' ��M�X���b�h�J�n
    '            m_WThreadSend.ThreadEnd(1)
    '            m_WThreadSend.TreadStart(AddressOf SendMainThread, True, Threading.ThreadPriority.Highest, "232C���M�X���b�h")
    '            Return 0
    '        End Function

    '        '----------------------------------------------------------------
    '        ' �T�v�F��M �I��
    '        ' �����F(in :)  = 
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Public Function Send_Stop() As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '�ʐM�|�[�g�N���[�Y
    '            If Not m_Status.SendWait Then Return -2 '�O��̎�M�����ᖳ��

    '            ''' �I���V�O�i�� �Z�b�g
    '            m_WThreadSend.g_hEnd.Set()
    '            m_WThreadSend.ThreadEnd(1000)
    '        End Function

    '        '----------------------------------------------------------------
    '        ' �T�v�F���M���C���X���b�h
    '        ' �����F(in :)  = 
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Private Sub SendMainThread()
    '            Dim blnRetc As Boolean
    '            Dim iSize As Integer            '���M�T�C�Y
    '            Dim iti As Integer              '���M�J�n��荞�݈ʒu
    '            Dim iTotalSize As Integer = 0   '���M���T�C�Y
    '            Dim dTimeOut As Date            '���M�^�C���A�E�g���� ������M�ŃZ�b�g

    '            ''' ��ԕύX
    '            m_Status.SendWait = True
    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::SendMainThread] ���M�J�n ", Now))
    '#End If

    '            Try
    '                ''' ���M
    '                While True
    '                    '' ���M
    '                    iti = m_SendNowBuf.Length - iTotalSize
    '                    Dim buf(iti - 1) As Byte '���M�o�b�t�@
    '                    Array.Copy(m_SendNowBuf, iTotalSize, buf, 0, iti)
    'SyncLock Me
    '                    blnRetc = tAPI_232C.WriteFile(m_hCom, buf, iti, iSize, Nothing)
    'End SyncLock
    '                    If Not blnRetc Then
    '                        '�ُ�ʒm
    '                        Dim state As New tClass_232C_State_Error(Me)
    '                        m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND_ERROR, Nothing, Nothing)
    '                        Exit While
    '                    End If

    '                    ''' ����M�̃^�C���A�E�g�`�F�b�N
    '                    If m_Status.SendWait And dTimeOut <> Date.MinValue Then
    '                        If dTimeOut < Now Then    '���M�^�C���A�E�g����
    '#If tDebug Then
    '                            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::SendMainThread] ���M�^�C���A�E�g [{1:yyyy/MM/dd hh:mm:ss.fff}]", Now, dTimeOut))
    '#End If
    '                            '' �ʒm
    '                            Dim state As New tClass_232C_State_Send
    '                            state.buffer = m_SendNowBuf
    '                            iTotalSize = 0
    '                            dTimeOut = Date.MinValue
    '                            m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND_TIMEOUT, Nothing, Nothing)
    '                            Exit While
    '                        End If
    '                    End If

    '                    '' ���M�`������
    '                    If iSize <> 0 Then
    '                        ''' �T�C�Y���Z
    '                        iTotalSize += iSize
    '                        dTimeOut = Now.AddMilliseconds(m_TimeOutInterval)
    '#If tDebug Then
    '                        Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::SendMainThread] ���M���c[{1}byte] {2}/{3} ", Now, iSize, iTotalSize, m_SendNowBuf.Length))
    '#End If
    '                        If iTotalSize = m_SendNowBuf.Length Then    '���M����
    '                            '' �ʒm
    '                            Dim state As New tClass_232C_State_Send
    '                            state.buffer = m_SendNowBuf
    '                            iTotalSize = 0
    '                            dTimeOut = Date.MinValue
    '                            m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND, Nothing, Nothing)
    '                            Exit While
    '                        End If
    '                    Else
    '                        '�ُ�ʒm (�����N���[�Y����Ă���) 
    '                        Dim state As New tClass_232C_State_Error(Me)
    '                        m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND_ERROR, Nothing, Nothing)
    '                        Exit While
    '                    End If
    '                End While
    '            Catch ex As Threading.ThreadAbortException
    '                'Abort���L�����Z������
    '                Threading.Thread.ResetAbort()
    '            Catch ex As Exception
    '            Finally

    '                m_Status.SendWait = False
    '#If tDebug Then
    '                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::SendMainThread] ���M���� ", Now))
    '#End If
    '            End Try
    '        End Sub


    '        '----------------------------------------------------------------
    '        ' �T�v�F��M �J�n
    '        ' �����F(in :Integer) iSize = ��M�T�C�Y (�ϒ��� 1 ���w��) (�Œ蒷�� 1�ȊO)
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Public Function Recv(Optional ByVal iSize As Integer = 1) As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '�ʐM�|�[�g�N���[�Y
    '            If m_Status.RecvWait Then Return -2 '�O��̎�M��
    '            If iSize < 1 Then Return -3 '�����ُ�

    '            ''' ��M�̈�m��
    '            ReDim m_RecvNowBuf(iSize - 1)
    '            m_RecvMode = CBool(IIf(iSize = 1, False, True))

    '            ''' ��M�X���b�h�J�n
    '            m_WThreadRecv.TreadStart(AddressOf RecvMainThread, True, Threading.ThreadPriority.AboveNormal, "232C��M�X���b�h")
    '            Return 0
    '        End Function

    '        '----------------------------------------------------------------
    '        ' �T�v�F��M �I��
    '        ' �����F(in :)  = 
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Public Function Recv_Stop() As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '�ʐM�|�[�g�N���[�Y
    '            If Not m_Status.RecvWait Then Return -2 '�O��̎�M�����ᖳ��

    '            ''' �I���V�O�i�� �Z�b�g
    '            m_WThreadRecv.g_hEnd.Set()
    '            m_WThreadRecv.ThreadEnd(1000)
    '        End Function

    '        '----------------------------------------------------------------
    '        ' �T�v�F��M���C���X���b�h
    '        ' �����F(in :)  = 
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Private Sub RecvMainThread()
    '            Dim blnRetc As Boolean
    '            Dim iSize As Integer            '��M�T�C�Y
    '            Dim iti As Integer              '��M�J�n��荞�݈ʒu
    '            Dim iTotalSize As Integer = 0   '��M���T�C�Y
    '            Dim dTimeOut As Date            '��M�^�C���A�E�g���� ������M�ŃZ�b�g

    '            ''' ��ԕύX
    '            m_Status.RecvWait = True
    '            m_RecvBuf = New IO.MemoryStream    '��M�o�b�t�@����
    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::RecvMainThread] ��M�J�n ", Now))
    '#End If

    '            ''' �V�O�i�� ����
    '            Dim iRetc As Integer
    '            Dim EventAry(1) As Threading.WaitHandle             '�V�O�i���҂�
    '            Dim sDummy As New Threading.ManualResetEvent(True)  '�_�~�[�V�O�i�� (��ɃV�O�i�����)
    '            EventAry(0) = m_WThreadRecv.g_hEnd                  '�I���V�O�i��
    '            EventAry(1) = sDummy                                '(�z��̈�ԍŌ�)

    '            Try
    '                ''' ��M
    '                While True
    '                    iRetc = Threading.WaitHandle.WaitAny(EventAry)
    '                    If iRetc = 0 Then Exit While '�I���m�F

    '                    '' ��荞��
    '                    iti = CInt(IIf(m_RecvMode, CInt(m_RecvNowBuf.Length - iTotalSize), 1))
    'SyncLock Me
    '                    blnRetc = tAPI_232C.ReadFile(m_hCom, m_RecvNowBuf, iti, iSize, Nothing)
    'End SyncLock
    '                    If Not blnRetc Then
    '                        '�ُ�ʒm
    '                        Dim state As New tClass_232C_State_Error(Me)
    '                        m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV_ERROR, Nothing, Nothing)
    '                        Exit While
    '                    End If

    '                    ''' ����M�̃^�C���A�E�g�`�F�b�N
    '                    If m_Status.RecvWait And dTimeOut <> Date.MinValue Then
    '                        If dTimeOut < Now Then    '��M�^�C���A�E�g����
    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::RecvMainThread] ��M�^�C���A�E�g [{1:yyyy/MM/dd hh:mm:ss.fff}]", Now, dTimeOut))
    '#End If
    '                            '' �ʒm
    '                            Dim state As New tClass_232C_State_Recv
    '                            state.buffer = m_RecvNowBuf
    '                            m_RecvBuf.Close()                   '��M�o�b�t�@������
    '                            m_RecvBuf = New IO.MemoryStream     '��M�o�b�t�@����
    '                            iTotalSize = 0
    '                            dTimeOut = Date.MinValue
    '                            m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV_TIMEOUT, Nothing, Nothing)
    '                        End If
    '                    End If

    '                    '' ��M�`������
    '                    If iSize <> 0 Then
    '                        ''' �T�C�Y���Z
    '                        iTotalSize += iSize
    '                        dTimeOut = Now.AddMilliseconds(m_TimeOutInterval)

    '                        '''�Œ蒷�� �ϒ�
    '                        If m_RecvMode Then  '�Œ蒷
    '                            ''' �o�b�t�@�i�[
    '                            m_RecvBuf.Write(m_RecvNowBuf, iTotalSize - iSize, iSize)

    '                            If iTotalSize = m_RecvNowBuf.Length Then    '��M����
    '#If tDebug Then
    '    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::RecvMainThread] �Œ蒷��M �T�C�Y={1}byte ", Now, m_RecvNowBuf.Length))
    '#End If
    '                                '' �ʒm
    '                                Dim state As New tClass_232C_State_Recv
    '                                state.buffer = m_RecvNowBuf
    '                                m_RecvBuf.Close()                   '��M�o�b�t�@������
    '                                m_RecvBuf = New IO.MemoryStream     '��M�o�b�t�@����
    '                                iTotalSize = 0
    '                                dTimeOut = Date.MinValue
    '                                m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV, Nothing, Nothing)
    '                            End If
    '                        Else                '�ϒ�
    '                            ''' �o�b�t�@�Ɋi�[
    '                            m_RecvBuf.WriteByte(m_RecvNowBuf(0))

    '                            ''STX�`�F�b�N
    '                            subSTX_Check()
    '                            ''ETX�`�F�b�N
    '                            If blnETX_Check() Then                  'ETX�擾
    '                                '' �ʒm
    '                                Dim eNo As tEnum_232C_EventNO
    '                                Dim state As New tClass_232C_State_Recv
    '                                state.buffer = m_RecvBuf.ToArray
    '                                m_RecvBuf.Close()                   '��M�o�b�t�@������
    '                                m_RecvBuf = New IO.MemoryStream    '��M�o�b�t�@����
    '                                iTotalSize = 0
    '                                dTimeOut = Date.MinValue
    '                                ''' �擪��STX�����邩�`�F�b�N
    '                                eNo = tEnum_232C_EventNO.RECV
    '                                For ii As Integer = 0 To m_Recv_STX.Length - 1
    '                                    If state.buffer(ii) <> m_Recv_STX(ii) Then
    '                                        eNo = tEnum_232C_EventNO.RECV_STX_ERROR
    '                                    End If
    '                                Next
    '                                m_Dele232CEvent.BeginInvoke(state, eNo, Nothing, Nothing)
    '                            End If
    '                        End If
    '                    End If
    '                End While
    '            Catch ex As Threading.ThreadAbortException
    '                'Abort���L�����Z������
    '                Threading.Thread.ResetAbort()
    '            Catch ex As Exception
    '            Finally
    '                sDummy.Close()
    '                m_Status.RecvWait = False
    '                m_RecvBuf.Close()
    '                m_RecvBuf = Nothing
    '#If tDebug Then
    '                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::RecvMainThread] ��M���� ", Now))
    '#End If
    '            End Try
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' �T�v�F��M�`���J�n������`�F�b�N
    '        ' �����F(in :)  =
    '        ' �ߒl�F() 
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Private Sub subSTX_Check()
    '            Dim cntSTX As Integer           'STX��    
    '            Dim index As Integer            '�o�b�t�@���̌����J�n�ʒu
    '            Dim blnRetc As Boolean

    '            ''' �����`�F�b�N
    '            If m_Recv_STX Is Nothing Then Exit Sub
    '            cntSTX = m_Recv_STX.Length
    '            If cntSTX <= 0 Then Exit Sub
    '            index = CInt(m_RecvBuf.Length) - cntSTX
    '            If index < 0 Then Exit Sub

    '            ''' �J�n������m�F
    '            m_RecvBuf.Position = index
    '            For ii As Integer = 0 To cntSTX - 1
    '                If m_RecvBuf.ReadByte <> m_Recv_STX(ii) Then
    '                    m_RecvBuf.Position = m_RecvBuf.Length   '�|�W�V�������Ō���ɖ߂�
    '                    Exit Sub
    '                End If
    '            Next ii

    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::subSTX_Check] STX�F�m ", Now))
    '#End If
    '            ''' �o�b�t�@�̓r����STX�H (�j���f�[�^���ُ�Ƃ��Ēʒm)
    '            If Not m_RecvBuf.Length = cntSTX Then
    '                Dim state As New tClass_232C_State_Recv
    '                ReDim state.buffer(index - 1)
    '                m_RecvBuf.Position = 0
    '                m_RecvBuf.Read(state.buffer, 0, index)
    '                m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV_ETX_ERROR, Nothing, Nothing)
    '            End If

    '            ''' OK�m�F
    '            '�o�b�t�@�������� �Ă�STX���Z�b�g
    '            m_RecvBuf.Close()               '��M�o�b�t�@������
    '            m_RecvBuf = New IO.MemoryStream
    '            For ii As Integer = 0 To cntSTX - 1
    '                m_RecvBuf.WriteByte(m_Recv_STX(ii))
    '            Next ii
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' �T�v�F��M�`���I�[������`�F�b�N
    '        ' �����F(in :)  = 
    '        ' �ߒl�F(Boolean) True:�I�� False:�p��
    '        ' ���l�F
    '        '----------------------------------------------------------------
    '        Private Function blnETX_Check() As Boolean
    '            Dim cntETX As Integer           'STX��    
    '            Dim index As Integer            '�o�b�t�@���̌����J�n�ʒu
    '            Dim blnRetc As Boolean

    '            ''' �����`�F�b�N
    '            cntETX = m_Recv_ETX.Length
    '            If cntETX <= 0 Then Exit Function
    '            index = CInt(m_RecvBuf.Length) - cntETX
    '            If index < 0 Then Exit Function

    '            ''' �I�[������m�F
    '            m_RecvBuf.Position = index
    '            For ii As Integer = 0 To cntETX - 1
    '                If m_RecvBuf.ReadByte <> m_Recv_ETX(ii) Then
    '                    m_RecvBuf.Position = m_RecvBuf.Length   '�|�W�V�������Ō���ɖ߂�
    '                    Return False
    '                End If
    '            Next ii

    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_232C::blnETX_Check] ETX�F�m �T�C�Y={1}byte", Now, m_RecvBuf.Length))
    '#End If
    '            Return True
    '        End Function
    '    End Class
End Namespace






