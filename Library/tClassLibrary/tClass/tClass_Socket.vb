'*********************************************************************************
'�������s�����z��v�f�̊��N���X
' Ver.01    2005/12/23  vs2005 �Ή�
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Net
Imports System.Net.Sockets
'Imports tClassLibrary.tIface
'Imports tClassLibrary.tClass


#Const tDebug = False            'True�� �f�o�b�N���[�h

Namespace tClass
        Public Class tClass_Socket

            '********************************************************************
            '�󂯓n���N���X
            '********************************************************************
#Region "�󂯓n���N���X"
            ''' <summary>
            ''' �󂯓n���p�̍ŏ�ʃN���X
            ''' </summary>
            Public Class tState
                ''' <summary>�\�P�b�g�N���X</summary>
                Public clsSock As tClass_Socket

                Public Sub New(ByVal cls As tClass_Socket)
                    clsSock = cls				' ������
                End Sub
            End Class

            ''' <summary>�R�l�N�g���̎󂯓n�� (tClass_Socket_State�̌p��)</summary> 
            Public Class tClass_Socket_State_Connect
                Inherits tState

                ''' �R���X�g���N�^
                Public Sub New(ByVal cls As tClass_Socket)
                    MyBase.New(cls)

                End Sub
            End Class
            ''' <summary>�A�N�Z�v�g���̎󂯓n�� (tClass_Socket_State�̌p��)</summary> 
            Public Class tClass_Socket_State_Accept
                Public clsSock As tClass_Socket					'�\�P�b�g�N���X

                ''' �R���X�g���N�^
                Public Sub New(ByVal cls As tClass_Socket)
                    '' ������
                    clsSock = cls
                End Sub
            End Class
            ''' ��M (�^�C���A�E�g��)
            Public Class tClass_Socket_State_Recv
                Public clsSock As tClass_Socket					'�\�P�b�g�N���X
                Public buffer() As Byte						'��M�f�[�^�i�[�̈�
                Public nowRecvSize As Integer				'����̎�M�T�C�Y

                ''' �R���X�g���N�^
                Public Sub New(ByVal cls As tClass_Socket, ByVal iSize As Integer)
                    '' ������
                    clsSock = cls
                    nowRecvSize = 0

                    '' �o�b�t�@�̈�m��
                    If iSize < 1 Then
                        ReDim buffer(0)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0}] �� [tClass_Socket_Recv_State::New] �o�b�t�@�T�C�Y�ُ� [" & iSize & "]", Environment.TickCount))
#End If
                    Else
                        ReDim buffer(iSize - 1)
                    End If
                End Sub
            End Class
            ''' ���M (�^�C���A�E�g��)
            Public Class tClass_Socket_State_Send
                Public clsSock As tClass_Socket					'�\�P�b�g�N���X
                Public buffer() As Byte						'���M�f�[�^�i�[�̈�
                Public nowSendSize As Integer				'����̑��M�T�C�Y

                ''' �R���X�g���N�^
                Public Sub New(ByVal cls As tClass_Socket)
                    '' ������
                    clsSock = cls
                    nowSendSize = 0
                End Sub
            End Class
            ''' �N���[�Y
            Public Class tClass_Socket_State_Close
                Public clsSock As tClass_Socket					'�\�P�b�g�N���X
                Public IP As String						'IP
                Public Port As Integer					'�|�[�g

                ''' �R���X�g���N�^
                Public Sub New(ByVal cls As tClass_Socket, ByVal wkIP As String, ByVal wkPort As Integer)
                    '' ������
                    clsSock = cls
                    IP = wkIP
                    Port = wkPort
                End Sub
            End Class
            ''' �ُ�
            Public Class tClass_Socket_State_Error
                Public clsSock As tClass_Socket					'�\�P�b�g�N���X
                Public ex As Exception							'�ُ���
                ''' �R���X�g���N�^
                Public Sub New(ByVal cls As tClass_Socket, ByVal exWk As Exception)
                    '' ������
                    clsSock = cls
                    ex = exWk
                End Sub
            End Class
#End Region


            '********************************************************************
            '�\�P�b�g ���N���X
            '********************************************************************
            Public Class tClass_Socket_Base

                '********************************************************************
                '�񓯊��ŌĂяo�����\�b�h�Ɠ����V�O�l�`���̃f���Q�[�g���`
                '********************************************************************
                Public Delegate Sub tDele_Sock_Event(ByVal sender As Object, ByVal e As tEnum_Sock_EventNO)				'�񓯊��f���Q�[�g
                ''' �f���Q�[�g �C�x���g
                Public Enum tEnum_Sock_EventNO
                    '����
                    CONNECT = 1				'�R�l�N�g����
                    ACCEPT = 2				'�A�N�Z�v�g�L��
                    RECV = 3				'��M����
                    SEND = 4				'���M����
                    CLOSE_SOCK = 5			'����悩��N���[�Y���ꂽ
                    CLOSE_OTHER = 6			'�����I�ɃN���[�Y

                    '�ُ�
                    RECV_ERROR = -1			'��M�ُ�
                    SEND_ERROR = -2			'���M�ُ�
                    'RECV_TIMEOUT = -3       '��M�^�C���A�E�g
                    'SEND_TIMEOUT = -4       '���M�^�C���A�E�g

                    '�O������̋N���p
                    OTHER = 100			   '�O������̋N���p
                    OTHER1
                    OTHER2
                    OTHER3
                    OTHER4
                    OTHER5
                    OTHER6
                    OTHER7
                    OTHER8
                    OTHER9
                End Enum


                '********************************************************************
                '���\�b�h��`
                '********************************************************************
                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g����
                ' �����F(in :SocketType) sType = �X���b�h�Ŏ��s����郁�\�b�h
                ' �����F(in :ProtocolType) pType = �\�P�b�g�^�C�v
                ' �����F(in :ThreadPriority) priority = �v���g�R���^�C�v
                ' �ߒl�F(Socket) �����\�P�b�g
                ' ���l�F
                'TCP
                'Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
                'UDP
                'Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp)
                '----------------------------------------------------------------
                Public Shared Function SockOpen(ByVal sType As SocketType, ByVal pType As ProtocolType) As Socket
                    '' �\�P�b�g�̐���
                    Try
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::SockOpen] �\�P�b�g ����", Now))
#End If
                        Return New Socket(AddressFamily.InterNetwork, sType, pType)
                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::SockOpen] �\�P�b�g �����ُ�", Now))
#End If
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g�I��
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �ߒl�F(Boolean) ���A���
                ' ���l�F
                '----------------------------------------------------------------
                Public Shared Function SockClose(ByVal sock As Socket) As Boolean
                    If sock Is Nothing Then Return False
                    If sock.Handle.ToInt32 <= 0 Then Return True

                    '' �\�P�b�g�I��
                    sock.Close()

                    ''����ɃN���[�Y����Ă��邩�`�F�b�N
                    If Not sock.Connected Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::SockClose] �\�P�b�g �I��", Now))
#End If
                        Return True
                    Else
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::SockClose] �\�P�b�g�I�� ���s", Now))
#End If
                        Return False
                    End If
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g ����M��~
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �ߒl�F(Boolean) ���A���
                ' ���l�F
                '----------------------------------------------------------------
                Public Shared Function SockShutdown(ByVal sock As Socket) As Boolean
                    If sock Is Nothing Then Return False
                    If sock.Handle.ToInt32 <= 0 Then Return True

                    '' �\�P�b�g�I��
                    If sock.Connected Then					'�ڑ������`�F�b�N
                        sock.Shutdown(SocketShutdown.Both)	'�o�b�t�@�� ����M�f�[�^ ������
                    End If
                    Return True
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�R�l�N�g
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �����F(in :IPEndPoint) localEP = �G���h�|�C���g
                ' �ߒl�F(Boolean) ���A���
                ' ���l�F
                '----------------------------------------------------------------
                Public Shared Function Connect(ByVal sock As Socket, ByVal localEP As EndPoint) As Boolean
                    Try
                        '' �R�l�N�g�҂� (�u���b�L���O)
                        sock.Connect(localEP)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::Connect] �R�l�N�g ����", Now))
#End If
                        Return True
                    Catch ex As SocketException
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::Connect] �R�l�N�g ���s {1}", Now, ex.ErrorCode))
#End If
                        Return False
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�A�N�Z�v�g
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �����F(in :AsyncCallback) method = �R�[���o�b�N�֐��̃A�h���X
                ' �ߒl�F(IAsyncResult) �񓯊�����I�u�W�F�N�g
                ' ���l�F
                '----------------------------------------------------------------
                Public Shared Function Listen(ByVal sock As Socket, ByVal method As AsyncCallback) As IAsyncResult
                    Dim ar As IAsyncResult
                    Try
                        '' �A�N�Z�v�g�҂�
                        ar = sock.BeginAccept(method, sock)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::Listen] �A�N�Z�v�g�҂��񓯊��v��", Now))
#End If
                        Return ar
                    Catch ex As SocketException
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket_Base::Listen] �A�N�Z�v�g�҂� ���s {1]", Now, ex.ErrorCode))
#End If
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�񓯊���M
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �����F(in :tClass_Socket) clsSock = �\�P�b�g�N���X
                ' �����F(in :Integer) iSize = ��M�T�C�Y
                ' �����F(in :AsyncCallback) method = �R�[���o�b�N�֐��̃A�h���X
                ' �ߒl�F(IAsyncResult) �񓯊�����̃X�e�[�^�X 
                ' ���l�F�ŏ��̏ꍇ
                '----------------------------------------------------------------
                Public Overloads Shared Function Recv(ByVal sock As Socket, ByVal clsSock As tClass_Socket, ByVal iSize As Integer, ByVal method As AsyncCallback) As IAsyncResult
                    Dim state As New tClass_Socket_State_Recv(clsSock, iSize)

                    Try
                        '' �񓯊���M�J�n
                        'sock.BeginReceive(state.buffer, 0, iSize, 0, New AsyncCallback(AddressOf ReadCallback), state)
                        Return sock.BeginReceive(state.buffer, 0, iSize, 0, method, state)
                    Catch ex As Exception
                        Return Nothing
                    End Try
                End Function
                '----------------------------------------------------------------
                ' �T�v�F�񓯊���M
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �����F(in :tClass_Socket_State_Recv) state = ��M�󂯓n���N���X
                ' �����F(in :AsyncCallback) method = �R�[���o�b�N�֐��̃A�h���X
                ' �ߒl�F(IAsyncResult) �񓯊�����̃X�e�[�^�X 
                ' ���l�F�`�����������ꂽ�ꍇ��
                '----------------------------------------------------------------
                Public Overloads Shared Function Recv(ByVal sock As Socket, ByVal state As tClass_Socket_State_Recv, ByVal method As AsyncCallback) As IAsyncResult
                    Dim ar As IAsyncResult = Nothing
                    Dim iSize As Integer
                    Try
                        '' �r���̎�M���ēx�ǂݍ���
                        iSize = state.buffer.Length - state.nowRecvSize
                        Return sock.BeginReceive(state.buffer, state.nowRecvSize, iSize, 0, method, state)
                    Catch ex As Exception
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�񓯊����M
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �����F(in :tClass_Socket_State_Recv) state = ��M�󂯓n���N���X
                ' �����F(in :AsyncCallback) method = �R�[���o�b�N�֐��̃A�h���X
                ' �ߒl�F(IAsyncResult) �񓯊�����̃X�e�[�^�X 
                ' ���l�F�`�����������ꂽ�ꍇ��
                '----------------------------------------------------------------
                Public Overloads Shared Function Send(ByVal sock As Socket, ByVal state As tClass_Socket_State_Send, ByVal method As AsyncCallback) As IAsyncResult
                    Dim ar As IAsyncResult = Nothing
                    Dim iSize As Integer
                    Try
                        '' ���M�T�C�Y�w��
                        iSize = state.buffer.Length - state.nowSendSize
                        '' ���M
                        Return sock.BeginSend(state.buffer, state.nowSendSize, iSize, 0, method, state)
                    Catch ex As Exception
                        Return Nothing
                    End Try
                End Function

                ''----------------------------------------------------------------
                '' �T�v�F�������M
                '' �����F(in :Socket) sock = �\�P�b�g
                '' �����F(in :tClass_Socket_State_Recv) state = ��M�󂯓n���N���X
                '' �ߒl�F(Integer) 
                '' ���l�F
                ''----------------------------------------------------------------
                'Public Overloads Shared Function SendLock(ByVal sock As Socket, ByVal state As tClass_Socket_State_Send) As Integer
                '	Dim iSize As Integer
                '	Try
                '		'' ���M�T�C�Y�w��
                '		iSize = state.buffer.Length - state.nowSendSize
                '		'' ���M
                '		Return sock.Send(state.buffer, state.nowSendSize, SocketFlags.None)
                '	Catch ex As Exception
                '		Return Nothing
                '	End Try
                'End Function

            End Class



            '********************************************************************
            '�\�P�b�g �}�l�[�W���[ 
            '********************************************************************
            Public Class tClass_SocketMgr
            Implements tIface.tIface_Find				'tClass_Array�̌����p�C���^�[�t�F�[�X����

                '********************************************************************
                ' �萔��`
                '********************************************************************
                Private Const mc_ListenMaxCount As Integer = 32		'���X�i�[�ő吔 

                '********************************************************************
                '�����o�[�ϐ���`
                '********************************************************************
                Private m_SockBase As Socket				'�ŏ��ɍ�����\�P�b�g
                Private m_SockAry As tClass_Array			'�\�P�b�g�N���X�̑S�N���X
                Private m_EP As EndPoint					'�G���h�|�C���g

                Private ms_Listen As Threading.ManualResetEvent					'�ڑ��m���V�O�i��
                Private m_WThreadLs As tClass_WorkThread						'���X�i�[�����҂��X���b�h

                Private m_DeleSockEvent As tClass_Socket_Base.tDele_Sock_Event	'�A���T�[�f���Q�[�g

                '********************************************************************
                '�v���p�e�B��`
                '********************************************************************
#Region "�v���p�e�B��`"
                '====================================================================
                '�\�P�b�g�擾
                '====================================================================
                'Public Property tProp_BaseSock() As ge_DCB_BaudRate
                'Get
                '    Return CType(m_DCB.BaudRate, ge_DCB_BaudRate)
                'End Get
                'Set(ByVal Value As ge_DCB_BaudRate)
                '    m_DCB.BaudRate = Value
                'End Set
                'End Property
#End Region

                '********************************************************************
                '��{���\�b�h��`
                '********************************************************************
                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g�N���X �擾
                '----------------------------------------------------------------
                Public Overloads Function find(ByVal Index As Integer) As tClass_Socket
                    If m_SockAry.Count = 0 Or Index < 0 Or m_SockAry.Count < Index Then Return Nothing '�㉺���`�F�b�N

                    '' �N���X�擾
                    Return CType(m_SockAry.Item(Index), tClass_Socket)
                End Function
                Public Overloads Function find(ByVal IP As String, ByVal Port As Integer) As tClass_Socket
                    Try
                        Return CType(m_SockAry.Find(New Object() {IP, Port}, Me), tClass_Socket)
                    Catch ex As Exception
                        '' �Y������
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�����p �C���^�[�t�F�[�X ����
                '----------------------------------------------------------------
            Private Overloads Function Find(ByVal key As Object, ByVal aryList As System.Collections.ArrayList) As Integer Implements tIface.tIface_Find.Find
                Dim cls_Sock As tClass_Socket
                Dim ii As Integer
                Dim ary As Array = CType(key, Array)

                '' ����
                For ii = 0 To aryList.Count - 1
                    cls_Sock = CType(aryList(ii), tClass_Socket)
                    If CStr(ary.GetValue(0)) = cls_Sock.tProp_ToIP And CInt(ary.GetValue(1)) = cls_Sock.tProp_ToPort Then
                        Return ii
                    End If
                Next ii
                Return -1
            End Function

                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g�N���X ���X�g���珜�O
                '----------------------------------------------------------------
                Public Overloads Sub delete(ByVal clsSock As tClass_Socket)
                    If clsSock Is Nothing Then Exit Sub

                    '' �N���X���J������Ă��邩�`�F�b�N
                    clsSock.Dispose()

                    '' ���X�g���珜�O
                    m_SockAry.Remove(clsSock)
                End Sub
                Public Overloads Sub delete(ByVal Index As Integer)
                    Me.delete(Me.find(Index))
                End Sub
                Public Overloads Sub delete(ByVal IP As String, ByVal Port As Integer)
                    Me.delete(Me.find(IP, Port))
                End Sub

                '********************************************************************
                '���\�b�h��`
                '********************************************************************
                '----------------------------------------------------------------
                ' �T�v�F�R���X�g���N�^
                ' �����F(in :tDele_Sock_Event) deleMethod = �A���T�[ �� ���b�\�h�A�h���X
                '----------------------------------------------------------------
                Public Sub New(ByVal deleMethod As tClass_Socket_Base.tDele_Sock_Event)
                    '' �����Z�b�g
                    m_DeleSockEvent = deleMethod

                    '' �ϐ�������
                    m_SockAry = New tClass_Array
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g����
                ' �����F(in :SocketType) sType = �X���b�h�Ŏ��s����郁�\�b�h
                ' �����F(in :ProtocolType) pType = �\�P�b�g�^�C�v
                ' �����F(in :ThreadPriority) priority = �v���g�R���^�C�v
                ' �ߒl�F(Boolean) ���A���
                ' ���l�F
                'TCP
                'Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
                'UDP
                'Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp)
                '----------------------------------------------------------------
                Public Function SockOpen(ByVal sType As SocketType, ByVal pType As ProtocolType) As Boolean

                    If Not m_SockBase Is Nothing Then Return False

                    '' �\�P�b�g�̐���
                    Try
                        m_SockBase = tClass_Socket_Base.SockOpen(sType, pType)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_SocketMgr::SockOpen] �\�P�b�g ����", Now))
#End If
                        Return True
                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_SocketMgr::SockOpen] �\�P�b�g �����ُ�", Now))
#End If
                        Return False
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�\�P�b�g�I��
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �ߒl�F(Boolean) ���A���
                ' ���l�F
                '----------------------------------------------------------------
                Public Function SockClose() As Boolean
                    If m_SockBase Is Nothing Then Return False

                    '' �A�N�Z�v�g�����ς݂̃\�P�b�g���N���[�Y
                    Listen_Stop()

                    '' �\�P�b�g��S������
                    If Not m_SockAry Is Nothing Then
                        Dim cls_Sock As tClass_Socket
                        Dim cnt As Integer
                        Dim ii As Integer
                        cnt = m_SockAry.Count

                        For ii = cnt - 1 To 0 Step -1
                            cls_Sock = CType(m_SockAry.Item(ii), tClass_Socket)
                            Me.delete(cls_Sock)
                        Next ii
                    End If

                    '' �x�[�X�\�P�b�g�I��
                    If tClass_Socket_Base.SockClose(m_SockBase) Then
                        m_SockBase = Nothing
                        Return True
                    Else
                        Return False
                    End If
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�R�l�N�g
                ' �����F(in :String) ip = ip�A�h���X (192.168.1.1)
                ' �����F(in :Integer) port = �|�[�g
                ' �ߒl�F(Boolean) ���A���
                ' ���l�F�R�l�N�g�����܂Ńu���b�N����܂�
                '----------------------------------------------------------------
                Public Function Connect(ByVal ip As String, ByVal port As Integer) As Boolean
                    If m_SockBase Is Nothing Then Return False

                    Try
                        '' �l�b�g���[�N �G���h�|�C���g
                        m_EP = New IPEndPoint(IPAddress.Parse(ip), port)

                        '' �R�l�N�g
                        If Not tClass_Socket_Base.Connect(m_SockBase, m_EP) Then
                            Return False
                        End If
                        '' �\�P�b�g�N���X����
                        Dim Index As Integer
                        Index = m_SockAry.Add(New tClass_Socket(m_SockBase, m_DeleSockEvent))

                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Connect(Me.find(Index)), tClass_Socket_Base.tEnum_Sock_EventNO.CONNECT, Nothing, Nothing)
                        Return True
                    Catch ex As Exception
                        Return False
                    End Try
                End Function

                '----------------------------------------------------------------
                ' �T�v�F�A�N�Z�v�g�҂��J�n
                ' �����F(in :Integer) port = �|�[�g
                ' �����F(in :String) ip = ip�A�h���X
                ' �ߒl�F�Ȃ�
                ' ���l�F�񓯊��ł�(�m���u���b�L���O)
                '----------------------------------------------------------------
                Public Sub Listen(ByVal port As Integer, Optional ByVal ip As String = "0.0.0.0")
                    If m_SockBase Is Nothing Then Return
                    If Not m_WThreadLs Is Nothing Then Return

                    '' �����̃A�h���X�擾
                    ''''====================================================
                    ' �P�[�u���𔲂��� �A�N�Z�v�g�҂��ɂ����ꍇ�A127.0.0.1:port �ő҂��ɂȂ�ׁA�P�[�u�����Ȃ����ăR�l�N�g���ė��Ă�
                    ' �ڑ�����Ȃ��ƌ����o�O������  ���̑΍�
                    'Dim ipHostInfo As IPHostEntry = Dns.GetHostEntry(Dns.GetHostName())		'�����̃l�b�g���[�N�ݒ���擾
                    'If "127.0.0.1" = ipHostInfo.AddressList(0).ToString Then
                    '	m_EP = New IPEndPoint(IPAddress.Parse("0.0.0.0"), port)
                    'Else
                    '	m_EP = New IPEndPoint(ipHostInfo.AddressList(0), port)	  '�l�b�g���[�N �G���h�|�C���g
                    'End If
                    m_EP = New IPEndPoint(IPAddress.Parse(ip), port)
                    ''''====================================================

                    '' ���X�i�[�X���b�h�J�n
                    ms_Listen = New Threading.ManualResetEvent(False)
                    m_WThreadLs = New tClass_WorkThread
                    m_WThreadLs.ThreadStart(AddressOf Listen_Thread, Threading.ThreadPriority.Normal, "�A�N�Z�v�g�҂��X���b�h", False)
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F�A�N�Z�v�g�҂��X���b�h��~
                ' �����F(in :Integer) port = �|�[�g
                ' �ߒl�F�Ȃ�
                ' ���l�F
                '----------------------------------------------------------------
                Private Sub Listen_Stop()
                    If m_SockBase Is Nothing Then Return
                    If m_WThreadLs Is Nothing Then Return

                    '' �A�N�Z�v�g�҂��X���b�h��~
                    'm_WThreadLs.g_evStop.Set()
                    m_WThreadLs.ThreadStop(1000)

                    '' �A�N�Z�v�g�I������
                    m_WThreadLs = Nothing
                    ms_Listen.Close()
                    ms_Listen = Nothing
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F�A�N�Z�v�g�҂� ���[�J�[�X���b�h
                ' �����F(in :Integer) port = �|�[�g
                ' �ߒl�F�Ȃ�
                ' ���l�F
                '----------------------------------------------------------------
                Private Sub Listen_Thread()
                    Dim ar As IAsyncResult			'�񓯊�����I�u�W�F�N�g (����͓��Ɏg�p���Ă��Ȃ�)

                    '' �V�O�i�� ����
                    Dim iRetc As Integer
                    Dim EventAry(1) As Threading.WaitHandle				'�V�O�i���҂�
                    EventAry(0) = m_WThreadLs.g_evStop					'�I���V�O�i��
                    EventAry(1) = ms_Listen								'��M�V�O�i��

                    Try
                        '' �o�C���h
                        m_SockBase.Bind(m_EP)
                        m_SockBase.Listen(mc_ListenMaxCount)

                        '' �A�N�Z�v�g�v�����[�v
                        While True
                            ms_Listen.Reset()

                            '' �A�N�Z�v�g
                            ar = tClass_Socket_Base.Listen(m_SockBase, New AsyncCallback(AddressOf AcceptCallback))

                            '' �V�O�i���҂�
                            iRetc = Threading.WaitHandle.WaitAny(EventAry)
                            If iRetc = 0 Then Exit While '�I���m�F
                        End While
                    Catch ex As Threading.ThreadAbortException
                        '' �X���b�h������~
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_SocketMgr::Listen_Thread] �A�N�Z�v�g�X���b�h������~", Now))
#End If
                    Catch ex As Exception
                        '' �o�C���h�G���[
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_SocketMgr::Listen_Thread] �A�N�Z�v�g�X���b�h�ُ�", Now))
#End If
                        Exit Sub
                    End Try

                    '' �A�N�Z�v�g�v���I��
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_SocketMgr::Listen_Thread] �A�N�Z�v�g�҂��I��", Now))
#End If
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F�A�N�Z�v�g�L��
                ' �����F(in :IAsyncResult) ar = �񓯊�����̃X�e�[�^�X
                ' �ߒl�F�Ȃ�
                ' ���l�F
                '----------------------------------------------------------------
                Private Sub AcceptCallback(ByVal ar As IAsyncResult)
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_SocketMgr::AcceptCallback] �A�N�Z�v�g �R�[���o�b�N", Now))
#End If
                    ''�V�O�i��
                    If ms_Listen Is Nothing Then Exit Sub
                    ms_Listen.Set()

                    '' �ڑ����\�P�b�g���擾
                    Dim baseSock As Socket = CType(ar.AsyncState, Socket)		'm_Sock�̎�
                    If baseSock.Handle.ToInt32 < 0 Then Exit Sub '�ؒf�� �A�N�Z�v�g���������ꍇ �R�[���o�b�N������

                    '' �ڑ���\�P�b�g���擾
                    Dim newSock As Socket = baseSock.EndAccept(ar)				'�ڑ����Ă���Socket

                    '' �\�P�b�g�N���X����
                    Dim Index As Integer
                    Index = m_SockAry.Add(New tClass_Socket(newSock, m_DeleSockEvent))

                    '' �ʒm
                    m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Accept(Me.find(Index)), tClass_Socket_Base.tEnum_Sock_EventNO.ACCEPT, Nothing, Nothing)
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F�S�\�P�b�g�ɑ��M
                ' �����F(in :Byte) buffer = ���M�f�[�^
                ' �ߒl�F�Ȃ�
                ' ���l�F
                '----------------------------------------------------------------
                Public Sub Send_All(ByVal buffer() As Byte)
                    Dim cls_Sock As tClass_Socket
                    Dim ii As Integer
                    For ii = 0 To m_SockAry.Count - 1
                        cls_Sock = CType(m_SockAry.Item(ii), tClass_Socket)
                        cls_Sock.Send(buffer)
                    Next ii
                End Sub

                ''----------------------------------------------------------------
                '' �T�v�F�S�\�P�b�g�ɓ������M
                '' �����F(in :Byte) buffer = ���M�f�[�^
                '' �ߒl�F�Ȃ�
                '' ���l�F
                ''----------------------------------------------------------------
                'Public Sub SendLock_All(ByVal buffer() As Byte)
                '	Dim cls_Sock As tClass_Socket
                '	Dim ii As Integer
                '	For ii = 0 To m_SockAry.Count - 1
                '		cls_Sock = CType(m_SockAry.Item(ii), tClass_Socket)
                '		cls_Sock.SendLock(buffer)
                '	Next ii
                'End Sub

            End Class



            '********************************************************************
            '�\�P�b�g �N���X (���ۂ̑���M)
            '********************************************************************
            Public Class tClass_Socket
                Implements IDisposable							'�f�X�g���N�^��`

                '********************************************************************
                '�����o�[�ϐ���`
                '********************************************************************
                Private m_Sock As Socket				'�\�P�b�g

                Private m_DeleSockEvent As tClass_Socket_Base.tDele_Sock_Event	'�A���T�[�f���Q�[�g

                Private ms_StopOK As New Threading.ManualResetEvent(True)		'��~����V�O�i��
                Private ms_RecvOK As New Threading.ManualResetEvent(True)		'��M���͔�V�O�i��
                Private ms_SendOK As New Threading.ManualResetEvent(True)		'���M���͔�V�O�i��

                Private m_arRecv As System.IAsyncResult				'��M�� �񓯊�����N���X
                Private m_arSend As System.IAsyncResult				'���M�� �񓯊�����N���X

                '********************************************************************
                '�v���p�e�B��`
                '********************************************************************
#Region "�v���p�e�B��`"
                '====================================================================
                '�����IP�擾
                '====================================================================
                Public ReadOnly Property tProp_ToIP() As String
                    Get
                        Try
                            Return CType(m_Sock.RemoteEndPoint, IPEndPoint).Address.ToString
                        Catch ex As Exception
                            Return ""
                        End Try
                    End Get
                End Property
                '====================================================================
                '�����Port�擾
                '====================================================================
                Public ReadOnly Property tProp_ToPort() As Integer
                    Get
                        Try
                            Return CType(m_Sock.RemoteEndPoint, IPEndPoint).Port
                        Catch ex As Exception
                            Return 0
                        End Try
                    End Get
                End Property

                '====================================================================
                '�\�P�b�g�擾
                '====================================================================
                Public ReadOnly Property tProp_Sock() As Socket
                    Get
                        Return Me.m_Sock
                    End Get
                End Property

                '====================================================================
                '��M�^�C���A�E�g�ݒ�
                '====================================================================
                Public Property tProp_RecvTimeOut() As Integer
                    Get
                        Return CInt(Me.m_Sock.GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout))
                    End Get
                    Set(ByVal Value As Integer)
                        Me.m_Sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, Value)
                    End Set
                End Property
                '====================================================================
                '���M�^�C���A�E�g�ݒ�
                '====================================================================
                Public Property tProp_SendTimeOut() As Integer
                    Get
                        Return CInt(Me.m_Sock.GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout))
                    End Get
                    Set(ByVal Value As Integer)
                        Me.m_Sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, Value)
                    End Set
                End Property
#End Region

                '********************************************************************
                '���\�b�h��`
                '********************************************************************
                '----------------------------------------------------------------
                ' �T�v�F�R���X�g���N�^
                ' �����F(in :Socket) sock = �\�P�b�g
                ' �����F(in :tDele_Sock_Event) deleMethod = �A���T�[ �� ���b�\�h�A�h���X
                '----------------------------------------------------------------
                Public Sub New(ByVal sock As Socket, ByVal deleMethod As tClass_Socket_Base.tDele_Sock_Event)
                    '' �����Z�b�g
                    m_Sock = sock
                    m_DeleSockEvent = deleMethod
                    m_arRecv = Nothing
                    m_arSend = Nothing

                    '' �f�t�H���g�^�C���A�E�g�ݒ�
                    Me.tProp_RecvTimeOut = 1000
                    Me.tProp_SendTimeOut = 1000
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::New] �N���X���� �ڑ��� IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F�f�X�g���N�^
                ' �����F(in :)  = 
                '----------------------------------------------------------------
                Public Sub Dispose() Implements System.IDisposable.Dispose
                    If m_Sock Is Nothing Then Return

#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Dispose] �N���X�J�� �ڑ��� IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If

                    '' �ʒm�f�[�^����
                    Dim state As New tClass_Socket_State_Close(Me, Me.tProp_ToIP, Me.tProp_ToPort)

                    '' ��~�V�O�i��
                    ms_StopOK.Reset()

                    '' �񓯊��̑���M���I��
                    tClass_Socket_Base.SockShutdown(m_Sock)
                    '' ����M�I���҂�
                    If Not ms_RecvOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Dispose] �N���X�J�� �񓯊���M �I�����s IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If
                    If Not ms_SendOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Dispose] �N���X�J�� �񓯊���M �I�����s IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If

                    '' �\�P�b�g�I��
                    If tClass_Socket_Base.SockClose(m_Sock) Then
                        m_Sock = Nothing

                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.CLOSE_OTHER, Nothing, Nothing)
                    End If
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F����悩��N���[�Y�ʒm
                ' �����F(in :)  = 
                '----------------------------------------------------------------
                Private Sub Close()
                    If m_Sock Is Nothing Then Return

#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Close] �N���X�J�� �ڑ��� IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If

                    '' �ʒm�f�[�^����
                    Dim state As New tClass_Socket_State_Close(Me, Me.tProp_ToIP, Me.tProp_ToPort)

                    '' ��~�V�O�i��
                    ms_StopOK.Reset()

                    '' �񓯊��̑���M���I��
                    tClass_Socket_Base.SockShutdown(m_Sock)
                    '' ����M�I���҂�
                    If Not ms_RecvOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Close] �N���X�J�� �񓯊���M �I�����s IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If
                    If Not ms_SendOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Close] �N���X�J�� �񓯊���M �I�����s IP[{1}] �|�[�g[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If

                    '' �\�P�b�g�I��
                    If tClass_Socket_Base.SockClose(m_Sock) Then
                        m_Sock = Nothing

                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.CLOSE_SOCK, Nothing, Nothing)
                    End If
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F��M�J�n
                ' �����F(in :) iSize = ��M�o�b�t�@�T�C�Y
                ' �ߒl�F�Ȃ�
                ' ���l�F�σo�C�g�p
                '----------------------------------------------------------------
                Public Function Recv(ByVal iSize As Integer) As Integer
                    If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '�ؒf�� ���M���������ꍇ �R�[���o�b�N������

                    '''''' ��M���́A�ēx��M���Ȃ�
                    If Not m_arRecv Is Nothing AndAlso Not m_arRecv.IsCompleted Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Recv] ��M�J�n�ُ� ���M�� IP[{1}] �|�[�g[{2}] ��M�o�b�t�@[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If
                        Return -3	'�O���M��
                    End If

#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Recv] ��M�J�n ���M�� IP[{1}] �|�[�g[{2}] ��M�o�b�t�@[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If

                    '' ��M�J�n
                    ms_RecvOK.Reset()
                    m_arRecv = tClass_Socket_Base.Recv(m_Sock, Me, iSize, New AsyncCallback(AddressOf ReadCallback))
                    Return 0
                End Function
                '----------------------------------------------------------------
                ' �T�v�F��M����
                ' �����F(in :IAsyncResult) ar = �񓯊�����̃X�e�[�^�X
                '----------------------------------------------------------------
                Private Sub ReadCallback(ByVal ar As IAsyncResult)
                    Dim state As tClass_Socket_State_Recv		'�񓯊��I�u�W�F�N�g
                    Dim bytesRead As Integer					'��M�o�C�g��

                    '' �R�[���o�b�N�l�擾
                    Try
                        state = CType(ar.AsyncState, tClass_Socket_State_Recv)
                        Dim sock As Socket = state.clsSock.tProp_Sock
                        If Not sock.Connected OrElse sock.Handle.ToInt32 < 0 Then Exit Sub '�ؒf�� ��M���������ꍇ �R�[���o�b�N������
                        bytesRead = sock.EndReceive(ar)

                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::ReadCallback] ��M�����ُ� {1}", Now, ex.Message))
#End If
                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Error(Me, ex), tClass_Socket_Base.tEnum_Sock_EventNO.RECV_ERROR, Nothing, Nothing)
                        Exit Sub
                    Finally
                        '��M����
                        ms_RecvOK.Set()
                    End Try

                    '' ��~�H
                    If Not ms_StopOK.WaitOne(1, True) Then Exit Sub

                    '' ��M�o�C�g�T�C�Y��菈������
                    If bytesRead = 0 Then				'�q�����Ă���\�P�b�g���ؒf
                        Me.Close()						  '�I��

                    ElseIf bytesRead > 0 Then			'�`����M
                        state.nowRecvSize = bytesRead
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::ReadCallback] ��M���� ���M�� IP[{1}] �|�[�g[{2}] ��M�T�C�Y[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, state.nowRecvSize))
#End If
                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.RECV, Nothing, Nothing)
                    End If
                End Sub

                '----------------------------------------------------------------
                ' �T�v�F��M�J�n
                ' �����F(in :) iSize = ��M�T�C�Y
                ' �ߒl�F�Ȃ�
                ' ���l�F�Œ�o�C�g�p
                '----------------------------------------------------------------
                Public Function Recv_Len(ByVal iSize As Integer) As Integer
                    If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '�ؒf�� ���M���������ꍇ �R�[���o�b�N������

                    '' ��M���́A�ēx��M���Ȃ�
                    If Not m_arRecv Is Nothing AndAlso Not m_arRecv.IsCompleted Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Recv_Len] ��M�J�n�ُ� ���M�� IP[{1}] �|�[�g[{2}] ��M�o�b�t�@[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If
                        Return -3	'�O���M��
                    End If

#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Recv_Len] ��M�J�n ���M�� IP[{1}] �|�[�g[{2}] ��M�T�C�Y[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If

                    '' ��M�J�n
                    ms_RecvOK.Reset()
                    m_arRecv = tClass_Socket_Base.Recv(m_Sock, Me, iSize, New AsyncCallback(AddressOf ReadCallback_Len))
                    Return 0
                End Function
                '----------------------------------------------------------------
                ' �T�v�F��M����
                ' �����F(in :IAsyncResult) ar = �񓯊�����̃X�e�[�^�X
                '----------------------------------------------------------------
                Private Sub ReadCallback_Len(ByVal ar As IAsyncResult)
                    Dim state As tClass_Socket_State_Recv		'�񓯊��I�u�W�F�N�g
                    Dim bytesRead As Integer					'��M�o�C�g��

                    '' �R�[���o�b�N�l�擾
                    Try
                        state = CType(ar.AsyncState, tClass_Socket_State_Recv)
                        Dim sock As Socket = state.clsSock.tProp_Sock
                        If Not sock.Connected OrElse sock.Handle.ToInt32 < 0 Then Exit Sub '�ؒf�� ��M���������ꍇ �R�[���o�b�N������
                        bytesRead = sock.EndReceive(ar)

                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::ReadCallback_Len] ��M�����ُ� {1}", Now, ex.Message))
#End If
                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Error(Me, ex), tClass_Socket_Base.tEnum_Sock_EventNO.RECV_ERROR, Nothing, Nothing)
                        Exit Sub
                    Finally
                        '��M����
                        ms_RecvOK.Set()
                    End Try

                    '' ��~�H
                    If Not ms_StopOK.WaitOne(1, True) Then Exit Sub

                    '' ��M�o�C�g�T�C�Y��菈������
                    If bytesRead = 0 Then				'�q�����Ă���\�P�b�g���ؒf
                        Me.Close()						  '�I��

                    ElseIf bytesRead > 0 Then			'�`����M
                        state.nowRecvSize += bytesRead
                        If state.nowRecvSize = state.buffer.Length Then		'�w��o�C�g���B
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::ReadCallback_Len] ��M���� ���M�� IP[{1}] �|�[�g[{2}] ��M�T�C�Y[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, state.nowRecvSize))
#End If
                            '' �ʒm
                            m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.RECV, Nothing, Nothing)
                        Else												'�ēx�ǂݍ���
                            tClass_Socket_Base.Recv(state.clsSock.m_Sock, state, New AsyncCallback(AddressOf ReadCallback_Len))
                        End If
                    End If
                End Sub

'				'----------------------------------------------------------------
'				' �T�v�F�������M
'				' �����F(in :Byte) buffer = ���M�f�[�^
'				' �ߒl�F�Ȃ�
'				' ���l�F�Œ�o�C�g�p
'				'----------------------------------------------------------------
'				Public Function SendLock(ByVal buffer() As Byte) As Integer
'					If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '�ؒf�� ���M���������ꍇ �R�[���o�b�N������

'					'' ���M���́A�ēx���M���Ȃ�
'					If Not m_arSend Is Nothing AndAlso Not m_arSend.IsCompleted Then
'#If tDebug Then
'                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Send] ���M�J�n�ُ� ���M�� IP[{1}] �|�[�g[{2}] ���M�o�b�t�@[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
'#End If
'						Return -3	'�O�񑗐M��
'					End If

'					Dim state As New tClass_Socket_State_Send(Me)
'					state.buffer = buffer
'#If tDebug Then
'                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Send] ���M�J�n ���M�� IP[{1}] �|�[�g[{2}] ���M�T�C�Y[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
'#End If
'					'' ���M�J�n
'					If 0 <> tClass_Socket_Base.SendLock(m_Sock, state) Then
'						Return 0
'					Else
'						Return -1
'					End If
'					'tClass_Socket_Base.Send(m_Sock, state, New AsyncCallback(AddressOf SendCallback))
'					Return 0
'				End Function

                '----------------------------------------------------------------
                ' �T�v�F���M�J�n
                ' �����F(in :Byte) buffer = ���M�f�[�^
                ' �ߒl�F�Ȃ�
                ' ���l�F�Œ�o�C�g�p
                '----------------------------------------------------------------
                Public Function Send(ByVal buffer() As Byte) As Integer
                    If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '�ؒf�� ���M���������ꍇ �R�[���o�b�N������

                    '' ���M���́A�ēx���M���Ȃ�
                    If Not m_arSend Is Nothing AndAlso Not m_arSend.IsCompleted Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Send] ���M�J�n�ُ� ���M�� IP[{1}] �|�[�g[{2}] ���M�o�b�t�@[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
#End If
                        Return -3	'�O�񑗐M��
                    End If

                    Dim state As New tClass_Socket_State_Send(Me)
                    state.buffer = buffer
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::Send] ���M�J�n ���M�� IP[{1}] �|�[�g[{2}] ���M�T�C�Y[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
#End If
                    '' ���M�J�n
                    ms_SendOK.Reset()
                    m_arSend = tClass_Socket_Base.Send(m_Sock, state, New AsyncCallback(AddressOf SendCallback))
                    'tClass_Socket_Base.Send(m_Sock, state, New AsyncCallback(AddressOf SendCallback))
                    Return 0
                End Function
                '----------------------------------------------------------------
                ' �T�v�F���M����
                ' �����F(in :IAsyncResult) ar = �񓯊�����̃X�e�[�^�X
                '----------------------------------------------------------------
                Private Sub SendCallback(ByVal ar As IAsyncResult)
                    Dim state As tClass_Socket_State_Send		'�񓯊��I�u�W�F�N�g
                    Dim bytesSend As Integer					'���M�o�C�g��

                    '' �R�[���o�b�N�l�擾
                    Try
                        state = CType(ar.AsyncState, tClass_Socket_State_Send)
                        Dim sock As Socket = state.clsSock.tProp_Sock
                        If Not sock.Connected OrElse sock.Handle.ToInt32 < 0 Then Exit Sub '�ؒf�� ���M���������ꍇ �R�[���o�b�N������
                        bytesSend = sock.EndSend(ar)

                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::SendCallback] ���M�����ُ� {1}", Now, ex.Message))
#End If
                        '' �ʒm
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Error(Me, ex), tClass_Socket_Base.tEnum_Sock_EventNO.SEND_ERROR, Nothing, Nothing)
                        Exit Sub
                    Finally
                        '��M����
                        ms_SendOK.Set()

                    End Try

                    '' ��~�H
                    If Not ms_StopOK.WaitOne(1, True) Then Exit Sub

                    '' ��M�o�C�g�T�C�Y��菈������
                    If bytesSend = 0 Then				'�q�����Ă���\�P�b�g���ؒf
                        Me.Close()						  '�I��

                    ElseIf bytesSend > 0 Then			'�`����M
                        state.nowSendSize += bytesSend
                        If state.nowSendSize = state.buffer.Length Then		'�w��o�C�g���B
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] �� [tClass_Socket::SendCallback] ���M���� ���M�� IP[{1}] �|�[�g[{2}] ���M�T�C�Y[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, state.nowSendSize))
#End If

                            '' �ʒm
                            m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.SEND, Nothing, Nothing)
                        Else												'�ēx�ǂݍ���
                            tClass_Socket_Base.Send(state.clsSock.m_Sock, state, New AsyncCallback(AddressOf SendCallback))
                        End If
                    End If
                End Sub

            End Class
        End Class
End Namespace