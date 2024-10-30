'*********************************************************************************
'RS-232C通信クラス
'  受信は非同期対応
'  受信バッファは動的配列を使用の為、サイズ指定無し
'*********************************************************************************
Option Strict On                'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Runtime.InteropServices              'MarshalAsクラス等々
Imports System.Text                                 'StringBuilderクラス

#Const tDebug = False            'True時 デバックモード

Namespace tClass
    '    '********************************************************************
    '    ' API定数 定義
    '    '********************************************************************
    '    Friend Module tAPI_232C
    '        ''''''''' 構造体 & 定数
    '        ''' アクセス権限
    '        Public Const GENERIC_READ As Integer = &H80000000         ' オブジェクトへの読み取りアクセス
    '        Public Const GENERIC_WRITE As Integer = &H40000000        ' オブジェクトへの書き込みアクセス
    '        Public Const FILE_SHARE_READ As Integer = &H1
    '        Public Const FILE_SHARE_WRITE As Integer = &H2
    '        Public Const OPEN_EXISTING As Integer = 3
    '        Public Const FILE_ATTRIBUTE_NORMAL As Integer = &H80
    '        Public Const FILE_FLAG_OVERLAPPED As Integer = &H40000000

    '        ''' 非同期 ファイル読書構造体
    '        Public Structure OVERLAPPED
    '            Dim Internal As Integer
    '            Dim InternalHigh As Integer
    '            Dim Offset As Integer
    '            Dim OffsetHigh As Integer
    '            Dim hEvent As IntPtr
    '        End Structure

    '        ''' 通信設定 構造体
    '        Public Structure DCB
    '            Public DCBlength As Integer         '構造体のサイズ
    '            Public BaudRate As Integer          'ボーレイト（bps）の設定
    '            Public fBitFields As Integer        'ビット単位のフィールド定義
    '            Public wReserved As Short           '予約（0をセットする）
    '            Public XonLim As Short              '受信バッファ中のデータが何バイトになったらXon文字を送るかを指定
    '            Public XoffLim As Short             '受信バッファの空きが何バイトになったらXoff文字を送るかを指定
    '            Public ByteSize As Byte             '1データのビット数を指定
    '            Public Parity As Byte               'パリティの方式を指定
    '            Public StopBits As Byte             'ストップビット数を指定
    '            Public XonChar As Byte              'Xon文字を指定
    '            Public XoffChar As Byte             'Xoff文字を指定
    '            Public ErrorChar As Byte            'パリティエラーの場合に使う文字を指定
    '            Public EofChar As Byte              '非バイナリモードの場合のデータ終了文字の指定
    '            Public EvtChar As Byte              'イベントを生成する文字を指定
    '            Public wReserved1 As Short          '予約されています。使用しないでください。
    '        End Structure

    '        ''' タイムアウト 構造体
    '        Public Structure COMMTIMEOUTS
    '            Public ReadIntervalTimeout As Integer           '文字の読み込みの待ち時間
    '            Public ReadTotalTimeoutMultiplier As Integer    '読み込みの１文字あたりの時間
    '            Public ReadTotalTimeoutConstant As Integer      '読み込みの定数時間
    '            Public WriteTotalTimeoutMultiplier As Integer   '書き込みの１文字あたりの時間
    '            Public WriteTotalTimeoutConstant As Integer     '書き込みの定数時間
    '        End Structure

    '        '' 非同期で使おうと思ったけど やっぱりヤメ
    '        ''''' イベント生成
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
    '        ''' ファイル生成
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

    '        ''' ハンドル破棄
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

    '        ''' ファイル読込み
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

    '        ''' ファイル書込み
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

    '        ''' I/O操作の取り消しを行う (FILE_FLAG_OVERLAPPED 時のみ有効)
    '        <DllImport("KERNEL32.dll", EntryPoint:="CancelIo", SetLastError:=True)> _
    '        Public Function CancelIo( _
    '                ByVal hFile As IntPtr) _
    '            As Boolean
    '        End Function

    '        ''' シリアルポート設定
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

    '        ''' タイムアウト設定
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

    '        ''' バッファ初期化
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

    '        ''' バッファサイズ指定
    '        <DllImport("KERNEL32.dll", EntryPoint:="SetupComm", SetLastError:=True)> _
    '        Public Function SetupComm( _
    '                ByVal hFile As IntPtr, _
    '                ByVal dwInQueue As Integer, _
    '                ByVal dwOutQueue As Integer) _
    '            As Boolean
    '        End Function

    '        '''''' イベント生成
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
    '    '受け渡しクラス
    '    '********************************************************************
    '#Region "受け渡しクラス"
    '    ''' 受信 (タイムアウトも)
    '    Public Class tClass_232C_State_Recv
    '        Public buffer() As Byte                     '受信データ格納領域
    '    End Class
    '    ''' 送信 (タイムアウトも)
    '    Public Class tClass_232C_State_Send
    '        Public buffer() As Byte                     '送信データ格納領域
    '    End Class
    '    ''' 異常
    '    Public Class tClass_232C_State_Error
    '        Public cls232C As tClass_232C               '232Cクラス

    '        ''' コンストラクタ
    '        Public Sub New(ByVal cls As tClass_232C)
    '            ''' 初期化
    '            cls232C = cls
    '        End Sub
    '    End Class
    '#End Region


    '    '********************************************************************
    '    ' 232C 通信 
    '    '********************************************************************
    '    Public Class tClass_232C

    '        '********************************************************************
    '        '非同期で呼び出すメソッドと同じシグネチャのデリゲートを定義
    '        '********************************************************************
    '        Public Delegate Sub tDele_232C_Event(ByVal sender As Object, ByVal e As tEnum_232C_EventNO)             '非同期デリゲート
    '        ''' デリゲート イベント
    '        Public Enum tEnum_232C_EventNO
    '            '正常
    '            RECV = 1                '受信完了
    '            SEND = 2                '送信完了

    '            '異常
    '            RECV_ERROR = -1         '受信異常
    '            SEND_ERROR = -2         '送信異常
    '            RECV_TIMEOUT = -3       '受信タイムアウト
    '            SEND_TIMEOUT = -4       '送信タイムアウト
    '            RECV_STX_ERROR = -5     '受信データ一部破棄
    '            RECV_ETX_ERROR = -6     '受信データ一部破棄

    '            '外部からの起動用
    '            OTHER = 100            '外部からの起動用
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

    '        ''' 状態格納用構造体
    '        Public Structure tClass_232C_Status
    '            ''状態
    '            Dim RecvWait As Boolean     'True:受信中 Flase:何も無し
    '            Dim SendWait As Boolean     'True:送信中 Flase:何も無し
    '        End Structure

    '        '********************************************************************
    '        'メンバー変数定義
    '        '********************************************************************
    '        Private m_hCom As IntPtr                            'COMポート
    '        Private m_DCB As tAPI_232C.DCB                      'DCB
    '        Private m_Status As tClass_232C_Status              '232C通信状態

    '        Private m_WThreadRecv As tClass_WorkerThread        '受信スレッド
    '        Private m_WThreadSend As tClass_WorkerThread        '送信スレッド

    '        Private m_Dele232CEvent As tDele_232C_Event         'アンサーデリゲート

    '        Private m_Recv_STX() As Byte                        '受信伝文の開始判定用(可変長のみ)
    '        Private m_Recv_ETX() As Byte                        '受信伝文の終端判定用(可変長のみ)
    '        Private m_Send_STX() As Byte                        '送信付加用
    '        Private m_Send_ETX() As Byte                        '送信付加用
    '        Private m_TimeOutInterval As Integer = 1000         '送受信タイムアウト設定 [ms]

    '        ''' 受信用
    '        Private m_RecvMode As Boolean                       '受信モード False:可変長 True:固定長
    '        Private m_RecvNowBuf() As Byte                      '受信バッファ
    '        Private m_RecvBuf As IO.MemoryStream                '受信伝文バッファ(全部)
    '        ''' 送信用
    '        Private m_SendNowBuf() As Byte                      '送信バッファ


    '        '********************************************************************
    '        'プロパティ定義
    '        '********************************************************************
    '#Region "プロパティ定義"
    '        ''' DCBセット項目
    '        Public Enum ge_DCB_BaudRate             'ボーレート
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
    '        Public Enum ge_DCB_ByteSize             'ビット長
    '            i4 = 4
    '            i5 = 5
    '            i6 = 6
    '            i7 = 7
    '            i8 = 8
    '        End Enum
    '        Public Enum ge_DCB_StopBits             'ストップビット
    '            i1 = 0
    '            i1_5 = 1
    '            i2 = 2
    '        End Enum
    '        Public Enum ge_DCB_Parity               'パリティー
    '            no = 0
    '            odd = 1
    '            even = 2
    '            mark = 3
    '            space = 4
    '        End Enum

    '        '====================================================================
    '        'DCB 設定
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
    '        '受信伝文 開始コード(可変長のみ)
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
    '        '受信伝文 終了コード(可変長のみ)
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
    '        '送信伝文 開始コード
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
    '        '送信伝文 終了コード
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
    '        '送受信タイムアウト [ms]
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
    '        'メソッド定義
    '        '********************************************************************
    '        '----------------------------------------------------------------
    '        ' 概要：コンストラクタ
    '        ' 引数：(in :tDele_232C_Event) deleMethod = アンサー の メッソドアドレス
    '        '----------------------------------------------------------------
    '        Public Sub New(ByVal deleMethod As tDele_232C_Event)
    '            ''' 引数セット
    '            m_Dele232CEvent = deleMethod
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' 概要：オープン
    '        ' 引数：(in :String) strPort = オープンポート番号
    '        ' 引数：(in :Integer) iRecvMaxSize = 受信バッファ最大値
    '        ' 引数：(in :Integer) iSendMaxSize = 送信バッファ最大値
    '        ' 戻値：(Boolean) True:正常 False:異常
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Public Function Open(ByVal strPort As String, Optional ByVal iRecvMaxSize As Integer = 1024, Optional ByVal iSendMaxSize As Integer = 1024) As Boolean
    '            Dim blnRetc As Boolean
    '            Dim DCB As tAPI_232C.DCB

    '            ''' オープン
    '            m_hCom = tAPI_232C.CreateFile(strPort, tAPI_232C.GENERIC_READ Or tAPI_232C.GENERIC_WRITE, 0, Nothing, tAPI_232C.OPEN_EXISTING, tAPI_232C.FILE_ATTRIBUTE_NORMAL, Nothing)
    '            If m_hCom.ToInt32 <= 0 Then Return False

    '            ''' バッファサイズ指定
    '            blnRetc = tAPI_232C.SetupComm(m_hCom, iRecvMaxSize, iSendMaxSize)

    '            ''' ポートの設定
    '            blnRetc = tAPI_232C.GetCommState(m_hCom, DCB)
    '            DCB.BaudRate = m_DCB.BaudRate
    '            DCB.ByteSize = m_DCB.ByteSize
    '            DCB.Parity = m_DCB.Parity
    '            DCB.StopBits = m_DCB.StopBits
    '            blnRetc = tAPI_232C.SetCommState(m_hCom, DCB)

    '            ''' タイムアウト設定
    '            Dim ComTimeOut As tAPI_232C.COMMTIMEOUTS      'ComTimeOut
    '            'blnRetc = GetCommTimeouts(m_hCom, m_ComTimeOut)
    '            ComTimeOut.ReadIntervalTimeout = 100
    '            ComTimeOut.ReadTotalTimeoutConstant = 100
    '            ComTimeOut.ReadTotalTimeoutMultiplier = 0
    '            ComTimeOut.WriteTotalTimeoutConstant = 100
    '            ComTimeOut.WriteTotalTimeoutMultiplier = 0
    '            blnRetc = tAPI_232C.SetCommTimeouts(m_hCom, ComTimeOut)

    '            ''' バッファ初期化
    '            blnRetc = tAPI_232C.PurgeComm(m_hCom, PURGE_TXABORT Or PURGE_RXABORT Or PURGE_TXCLEAR Or PURGE_RXCLEAR)

    '            ''' 状態初期化
    '            With m_Status
    '                .RecvWait = False
    '                .SendWait = False
    '            End With

    '            '''' メインスレッド起動
    '            m_WThreadRecv = New tClass_WorkerThread
    '            m_WThreadSend = New tClass_WorkerThread
    '            Return True
    '        End Function

    '        '----------------------------------------------------------------
    '        ' 概要：クローズ
    '        ' 引数：(in :)  = 
    '        ' 戻値：() 
    '        ' 備考：
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
    '        ' 概要：書き込み
    '        ' 引数：(in :Byte) byteBuffer = 送信バイト配列
    '        ' 戻値：(Integer) 送信サイズ (エラー時0)
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Public Function Send(ByVal byteBuffer() As Byte) As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '通信ポートクローズ
    '            If m_Status.SendWait Then Return -2 '前回の送信中
    '            If byteBuffer.Length < 1 Then Return -3 '引数異常

    '            ''' 送信領域確保
    '            ReDim m_SendNowBuf(byteBuffer.Length - 1)
    '            Array.Copy(byteBuffer, m_SendNowBuf, byteBuffer.Length)

    '            ''' 受信スレッド開始
    '            m_WThreadSend.ThreadEnd(1)
    '            m_WThreadSend.TreadStart(AddressOf SendMainThread, True, Threading.ThreadPriority.Highest, "232C送信スレッド")
    '            Return 0
    '        End Function

    '        '----------------------------------------------------------------
    '        ' 概要：受信 終了
    '        ' 引数：(in :)  = 
    '        ' 戻値：() 
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Public Function Send_Stop() As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '通信ポートクローズ
    '            If Not m_Status.SendWait Then Return -2 '前回の受信中じゃ無い

    '            ''' 終了シグナル セット
    '            m_WThreadSend.g_hEnd.Set()
    '            m_WThreadSend.ThreadEnd(1000)
    '        End Function

    '        '----------------------------------------------------------------
    '        ' 概要：送信メインスレッド
    '        ' 引数：(in :)  = 
    '        ' 戻値：() 
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Private Sub SendMainThread()
    '            Dim blnRetc As Boolean
    '            Dim iSize As Integer            '送信サイズ
    '            Dim iti As Integer              '送信開始取り込み位置
    '            Dim iTotalSize As Integer = 0   '送信中サイズ
    '            Dim dTimeOut As Date            '送信タイムアウト時刻 何か受信でセット

    '            ''' 状態変更
    '            m_Status.SendWait = True
    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::SendMainThread] 送信開始 ", Now))
    '#End If

    '            Try
    '                ''' 送信
    '                While True
    '                    '' 送信
    '                    iti = m_SendNowBuf.Length - iTotalSize
    '                    Dim buf(iti - 1) As Byte '送信バッファ
    '                    Array.Copy(m_SendNowBuf, iTotalSize, buf, 0, iti)
    'SyncLock Me
    '                    blnRetc = tAPI_232C.WriteFile(m_hCom, buf, iti, iSize, Nothing)
    'End SyncLock
    '                    If Not blnRetc Then
    '                        '異常通知
    '                        Dim state As New tClass_232C_State_Error(Me)
    '                        m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND_ERROR, Nothing, Nothing)
    '                        Exit While
    '                    End If

    '                    ''' 送受信のタイムアウトチェック
    '                    If m_Status.SendWait And dTimeOut <> Date.MinValue Then
    '                        If dTimeOut < Now Then    '送信タイムアウト発生
    '#If tDebug Then
    '                            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_232C::SendMainThread] 送信タイムアウト [{1:yyyy/MM/dd hh:mm:ss.fff}]", Now, dTimeOut))
    '#End If
    '                            '' 通知
    '                            Dim state As New tClass_232C_State_Send
    '                            state.buffer = m_SendNowBuf
    '                            iTotalSize = 0
    '                            dTimeOut = Date.MinValue
    '                            m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND_TIMEOUT, Nothing, Nothing)
    '                            Exit While
    '                        End If
    '                    End If

    '                    '' 送信伝文処理
    '                    If iSize <> 0 Then
    '                        ''' サイズ加算
    '                        iTotalSize += iSize
    '                        dTimeOut = Now.AddMilliseconds(m_TimeOutInterval)
    '#If tDebug Then
    '                        Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::SendMainThread] 送信中…[{1}byte] {2}/{3} ", Now, iSize, iTotalSize, m_SendNowBuf.Length))
    '#End If
    '                        If iTotalSize = m_SendNowBuf.Length Then    '送信完了
    '                            '' 通知
    '                            Dim state As New tClass_232C_State_Send
    '                            state.buffer = m_SendNowBuf
    '                            iTotalSize = 0
    '                            dTimeOut = Date.MinValue
    '                            m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND, Nothing, Nothing)
    '                            Exit While
    '                        End If
    '                    Else
    '                        '異常通知 (多分クローズされている) 
    '                        Dim state As New tClass_232C_State_Error(Me)
    '                        m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.SEND_ERROR, Nothing, Nothing)
    '                        Exit While
    '                    End If
    '                End While
    '            Catch ex As Threading.ThreadAbortException
    '                'Abortをキャンセルする
    '                Threading.Thread.ResetAbort()
    '            Catch ex As Exception
    '            Finally

    '                m_Status.SendWait = False
    '#If tDebug Then
    '                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::SendMainThread] 送信完了 ", Now))
    '#End If
    '            End Try
    '        End Sub


    '        '----------------------------------------------------------------
    '        ' 概要：受信 開始
    '        ' 引数：(in :Integer) iSize = 受信サイズ (可変長時 1 を指定) (固定長時 1以外)
    '        ' 戻値：() 
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Public Function Recv(Optional ByVal iSize As Integer = 1) As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '通信ポートクローズ
    '            If m_Status.RecvWait Then Return -2 '前回の受信中
    '            If iSize < 1 Then Return -3 '引数異常

    '            ''' 受信領域確保
    '            ReDim m_RecvNowBuf(iSize - 1)
    '            m_RecvMode = CBool(IIf(iSize = 1, False, True))

    '            ''' 受信スレッド開始
    '            m_WThreadRecv.TreadStart(AddressOf RecvMainThread, True, Threading.ThreadPriority.AboveNormal, "232C受信スレッド")
    '            Return 0
    '        End Function

    '        '----------------------------------------------------------------
    '        ' 概要：受信 終了
    '        ' 引数：(in :)  = 
    '        ' 戻値：() 
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Public Function Recv_Stop() As Integer
    '            If m_hCom.ToInt32 <= 0 Then Return -1 '通信ポートクローズ
    '            If Not m_Status.RecvWait Then Return -2 '前回の受信中じゃ無い

    '            ''' 終了シグナル セット
    '            m_WThreadRecv.g_hEnd.Set()
    '            m_WThreadRecv.ThreadEnd(1000)
    '        End Function

    '        '----------------------------------------------------------------
    '        ' 概要：受信メインスレッド
    '        ' 引数：(in :)  = 
    '        ' 戻値：() 
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Private Sub RecvMainThread()
    '            Dim blnRetc As Boolean
    '            Dim iSize As Integer            '受信サイズ
    '            Dim iti As Integer              '受信開始取り込み位置
    '            Dim iTotalSize As Integer = 0   '受信中サイズ
    '            Dim dTimeOut As Date            '受信タイムアウト時刻 何か受信でセット

    '            ''' 状態変更
    '            m_Status.RecvWait = True
    '            m_RecvBuf = New IO.MemoryStream    '受信バッファ生成
    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::RecvMainThread] 受信開始 ", Now))
    '#End If

    '            ''' シグナル 生成
    '            Dim iRetc As Integer
    '            Dim EventAry(1) As Threading.WaitHandle             'シグナル待ち
    '            Dim sDummy As New Threading.ManualResetEvent(True)  'ダミーシグナル (常にシグナル状態)
    '            EventAry(0) = m_WThreadRecv.g_hEnd                  '終了シグナル
    '            EventAry(1) = sDummy                                '(配列の一番最後)

    '            Try
    '                ''' 受信
    '                While True
    '                    iRetc = Threading.WaitHandle.WaitAny(EventAry)
    '                    If iRetc = 0 Then Exit While '終了確認

    '                    '' 取り込み
    '                    iti = CInt(IIf(m_RecvMode, CInt(m_RecvNowBuf.Length - iTotalSize), 1))
    'SyncLock Me
    '                    blnRetc = tAPI_232C.ReadFile(m_hCom, m_RecvNowBuf, iti, iSize, Nothing)
    'End SyncLock
    '                    If Not blnRetc Then
    '                        '異常通知
    '                        Dim state As New tClass_232C_State_Error(Me)
    '                        m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV_ERROR, Nothing, Nothing)
    '                        Exit While
    '                    End If

    '                    ''' 送受信のタイムアウトチェック
    '                    If m_Status.RecvWait And dTimeOut <> Date.MinValue Then
    '                        If dTimeOut < Now Then    '受信タイムアウト発生
    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_232C::RecvMainThread] 受信タイムアウト [{1:yyyy/MM/dd hh:mm:ss.fff}]", Now, dTimeOut))
    '#End If
    '                            '' 通知
    '                            Dim state As New tClass_232C_State_Recv
    '                            state.buffer = m_RecvNowBuf
    '                            m_RecvBuf.Close()                   '受信バッファ初期化
    '                            m_RecvBuf = New IO.MemoryStream     '受信バッファ生成
    '                            iTotalSize = 0
    '                            dTimeOut = Date.MinValue
    '                            m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV_TIMEOUT, Nothing, Nothing)
    '                        End If
    '                    End If

    '                    '' 受信伝文処理
    '                    If iSize <> 0 Then
    '                        ''' サイズ加算
    '                        iTotalSize += iSize
    '                        dTimeOut = Now.AddMilliseconds(m_TimeOutInterval)

    '                        '''固定長か 可変長
    '                        If m_RecvMode Then  '固定長
    '                            ''' バッファ格納
    '                            m_RecvBuf.Write(m_RecvNowBuf, iTotalSize - iSize, iSize)

    '                            If iTotalSize = m_RecvNowBuf.Length Then    '受信完了
    '#If tDebug Then
    '    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::RecvMainThread] 固定長受信 サイズ={1}byte ", Now, m_RecvNowBuf.Length))
    '#End If
    '                                '' 通知
    '                                Dim state As New tClass_232C_State_Recv
    '                                state.buffer = m_RecvNowBuf
    '                                m_RecvBuf.Close()                   '受信バッファ初期化
    '                                m_RecvBuf = New IO.MemoryStream     '受信バッファ生成
    '                                iTotalSize = 0
    '                                dTimeOut = Date.MinValue
    '                                m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV, Nothing, Nothing)
    '                            End If
    '                        Else                '可変長
    '                            ''' バッファに格納
    '                            m_RecvBuf.WriteByte(m_RecvNowBuf(0))

    '                            ''STXチェック
    '                            subSTX_Check()
    '                            ''ETXチェック
    '                            If blnETX_Check() Then                  'ETX取得
    '                                '' 通知
    '                                Dim eNo As tEnum_232C_EventNO
    '                                Dim state As New tClass_232C_State_Recv
    '                                state.buffer = m_RecvBuf.ToArray
    '                                m_RecvBuf.Close()                   '受信バッファ初期化
    '                                m_RecvBuf = New IO.MemoryStream    '受信バッファ生成
    '                                iTotalSize = 0
    '                                dTimeOut = Date.MinValue
    '                                ''' 先頭にSTXがあるかチェック
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
    '                'Abortをキャンセルする
    '                Threading.Thread.ResetAbort()
    '            Catch ex As Exception
    '            Finally
    '                sDummy.Close()
    '                m_Status.RecvWait = False
    '                m_RecvBuf.Close()
    '                m_RecvBuf = Nothing
    '#If tDebug Then
    '                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::RecvMainThread] 受信完了 ", Now))
    '#End If
    '            End Try
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' 概要：受信伝文開始文字列チェック
    '        ' 引数：(in :)  =
    '        ' 戻値：() 
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Private Sub subSTX_Check()
    '            Dim cntSTX As Integer           'STX長    
    '            Dim index As Integer            'バッファ内の検索開始位置
    '            Dim blnRetc As Boolean

    '            ''' 条件チェック
    '            If m_Recv_STX Is Nothing Then Exit Sub
    '            cntSTX = m_Recv_STX.Length
    '            If cntSTX <= 0 Then Exit Sub
    '            index = CInt(m_RecvBuf.Length) - cntSTX
    '            If index < 0 Then Exit Sub

    '            ''' 開始文字列確認
    '            m_RecvBuf.Position = index
    '            For ii As Integer = 0 To cntSTX - 1
    '                If m_RecvBuf.ReadByte <> m_Recv_STX(ii) Then
    '                    m_RecvBuf.Position = m_RecvBuf.Length   'ポジションを最後尾に戻す
    '                    Exit Sub
    '                End If
    '            Next ii

    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::subSTX_Check] STX認知 ", Now))
    '#End If
    '            ''' バッファの途中にSTX？ (破棄データを異常として通知)
    '            If Not m_RecvBuf.Length = cntSTX Then
    '                Dim state As New tClass_232C_State_Recv
    '                ReDim state.buffer(index - 1)
    '                m_RecvBuf.Position = 0
    '                m_RecvBuf.Read(state.buffer, 0, index)
    '                m_Dele232CEvent.BeginInvoke(state, tEnum_232C_EventNO.RECV_ETX_ERROR, Nothing, Nothing)
    '            End If

    '            ''' OK確認
    '            'バッファ初期化後 再びSTXをセット
    '            m_RecvBuf.Close()               '受信バッファ初期化
    '            m_RecvBuf = New IO.MemoryStream
    '            For ii As Integer = 0 To cntSTX - 1
    '                m_RecvBuf.WriteByte(m_Recv_STX(ii))
    '            Next ii
    '        End Sub

    '        '----------------------------------------------------------------
    '        ' 概要：受信伝文終端文字列チェック
    '        ' 引数：(in :)  = 
    '        ' 戻値：(Boolean) True:終了 False:継続
    '        ' 備考：
    '        '----------------------------------------------------------------
    '        Private Function blnETX_Check() As Boolean
    '            Dim cntETX As Integer           'STX長    
    '            Dim index As Integer            'バッファ内の検索開始位置
    '            Dim blnRetc As Boolean

    '            ''' 条件チェック
    '            cntETX = m_Recv_ETX.Length
    '            If cntETX <= 0 Then Exit Function
    '            index = CInt(m_RecvBuf.Length) - cntETX
    '            If index < 0 Then Exit Function

    '            ''' 終端文字列確認
    '            m_RecvBuf.Position = index
    '            For ii As Integer = 0 To cntETX - 1
    '                If m_RecvBuf.ReadByte <> m_Recv_ETX(ii) Then
    '                    m_RecvBuf.Position = m_RecvBuf.Length   'ポジションを最後尾に戻す
    '                    Return False
    '                End If
    '            Next ii

    '#If tDebug Then
    '            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_232C::blnETX_Check] ETX認知 サイズ={1}byte", Now, m_RecvBuf.Length))
    '#End If
    '            Return True
    '        End Function
    '    End Class
End Namespace






