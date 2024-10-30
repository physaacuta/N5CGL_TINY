'*********************************************************************************
'同期を行った配列要素の基底クラス
' Ver.01    2005/12/23  vs2005 対応
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Net
Imports System.Net.Sockets
'Imports tClassLibrary.tIface
'Imports tClassLibrary.tClass


#Const tDebug = False            'True時 デバックモード

Namespace tClass
        Public Class tClass_Socket

            '********************************************************************
            '受け渡しクラス
            '********************************************************************
#Region "受け渡しクラス"
            ''' <summary>
            ''' 受け渡し用の最上位クラス
            ''' </summary>
            Public Class tState
                ''' <summary>ソケットクラス</summary>
                Public clsSock As tClass_Socket

                Public Sub New(ByVal cls As tClass_Socket)
                    clsSock = cls				' 初期化
                End Sub
            End Class

            ''' <summary>コネクト時の受け渡し (tClass_Socket_Stateの継承)</summary> 
            Public Class tClass_Socket_State_Connect
                Inherits tState

                ''' コンストラクタ
                Public Sub New(ByVal cls As tClass_Socket)
                    MyBase.New(cls)

                End Sub
            End Class
            ''' <summary>アクセプト時の受け渡し (tClass_Socket_Stateの継承)</summary> 
            Public Class tClass_Socket_State_Accept
                Public clsSock As tClass_Socket					'ソケットクラス

                ''' コンストラクタ
                Public Sub New(ByVal cls As tClass_Socket)
                    '' 初期化
                    clsSock = cls
                End Sub
            End Class
            ''' 受信 (タイムアウトも)
            Public Class tClass_Socket_State_Recv
                Public clsSock As tClass_Socket					'ソケットクラス
                Public buffer() As Byte						'受信データ格納領域
                Public nowRecvSize As Integer				'現状の受信サイズ

                ''' コンストラクタ
                Public Sub New(ByVal cls As tClass_Socket, ByVal iSize As Integer)
                    '' 初期化
                    clsSock = cls
                    nowRecvSize = 0

                    '' バッファ領域確保
                    If iSize < 1 Then
                        ReDim buffer(0)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0}] ★ [tClass_Socket_Recv_State::New] バッファサイズ異常 [" & iSize & "]", Environment.TickCount))
#End If
                    Else
                        ReDim buffer(iSize - 1)
                    End If
                End Sub
            End Class
            ''' 送信 (タイムアウトも)
            Public Class tClass_Socket_State_Send
                Public clsSock As tClass_Socket					'ソケットクラス
                Public buffer() As Byte						'送信データ格納領域
                Public nowSendSize As Integer				'現状の送信サイズ

                ''' コンストラクタ
                Public Sub New(ByVal cls As tClass_Socket)
                    '' 初期化
                    clsSock = cls
                    nowSendSize = 0
                End Sub
            End Class
            ''' クローズ
            Public Class tClass_Socket_State_Close
                Public clsSock As tClass_Socket					'ソケットクラス
                Public IP As String						'IP
                Public Port As Integer					'ポート

                ''' コンストラクタ
                Public Sub New(ByVal cls As tClass_Socket, ByVal wkIP As String, ByVal wkPort As Integer)
                    '' 初期化
                    clsSock = cls
                    IP = wkIP
                    Port = wkPort
                End Sub
            End Class
            ''' 異常
            Public Class tClass_Socket_State_Error
                Public clsSock As tClass_Socket					'ソケットクラス
                Public ex As Exception							'異常状態
                ''' コンストラクタ
                Public Sub New(ByVal cls As tClass_Socket, ByVal exWk As Exception)
                    '' 初期化
                    clsSock = cls
                    ex = exWk
                End Sub
            End Class
#End Region


            '********************************************************************
            'ソケット 基底クラス
            '********************************************************************
            Public Class tClass_Socket_Base

                '********************************************************************
                '非同期で呼び出すメソッドと同じシグネチャのデリゲートを定義
                '********************************************************************
                Public Delegate Sub tDele_Sock_Event(ByVal sender As Object, ByVal e As tEnum_Sock_EventNO)				'非同期デリゲート
                ''' デリゲート イベント
                Public Enum tEnum_Sock_EventNO
                    '正常
                    CONNECT = 1				'コネクト完了
                    ACCEPT = 2				'アクセプト有り
                    RECV = 3				'受信完了
                    SEND = 4				'送信完了
                    CLOSE_SOCK = 5			'相手先からクローズされた
                    CLOSE_OTHER = 6			'強制的にクローズ

                    '異常
                    RECV_ERROR = -1			'受信異常
                    SEND_ERROR = -2			'送信異常
                    'RECV_TIMEOUT = -3       '受信タイムアウト
                    'SEND_TIMEOUT = -4       '送信タイムアウト

                    '外部からの起動用
                    OTHER = 100			   '外部からの起動用
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
                'メソッド定義
                '********************************************************************
                '----------------------------------------------------------------
                ' 概要：ソケット生成
                ' 引数：(in :SocketType) sType = スレッドで実行されるメソッド
                ' 引数：(in :ProtocolType) pType = ソケットタイプ
                ' 引数：(in :ThreadPriority) priority = プロトコルタイプ
                ' 戻値：(Socket) 生成ソケット
                ' 備考：
                'TCP
                'Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
                'UDP
                'Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp)
                '----------------------------------------------------------------
                Public Shared Function SockOpen(ByVal sType As SocketType, ByVal pType As ProtocolType) As Socket
                    '' ソケットの生成
                    Try
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket_Base::SockOpen] ソケット 生成", Now))
#End If
                        Return New Socket(AddressFamily.InterNetwork, sType, pType)
                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket_Base::SockOpen] ソケット 生成異常", Now))
#End If
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：ソケット終了
                ' 引数：(in :Socket) sock = ソケット
                ' 戻値：(Boolean) 復帰情報
                ' 備考：
                '----------------------------------------------------------------
                Public Shared Function SockClose(ByVal sock As Socket) As Boolean
                    If sock Is Nothing Then Return False
                    If sock.Handle.ToInt32 <= 0 Then Return True

                    '' ソケット終了
                    sock.Close()

                    ''正常にクローズされているかチェック
                    If Not sock.Connected Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket_Base::SockClose] ソケット 終了", Now))
#End If
                        Return True
                    Else
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket_Base::SockClose] ソケット終了 失敗", Now))
#End If
                        Return False
                    End If
                End Function

                '----------------------------------------------------------------
                ' 概要：ソケット 送受信停止
                ' 引数：(in :Socket) sock = ソケット
                ' 戻値：(Boolean) 復帰情報
                ' 備考：
                '----------------------------------------------------------------
                Public Shared Function SockShutdown(ByVal sock As Socket) As Boolean
                    If sock Is Nothing Then Return False
                    If sock.Handle.ToInt32 <= 0 Then Return True

                    '' ソケット終了
                    If sock.Connected Then					'接続中かチェック
                        sock.Shutdown(SocketShutdown.Both)	'バッファ内 送受信データ 無効化
                    End If
                    Return True
                End Function

                '----------------------------------------------------------------
                ' 概要：コネクト
                ' 引数：(in :Socket) sock = ソケット
                ' 引数：(in :IPEndPoint) localEP = エンドポイント
                ' 戻値：(Boolean) 復帰情報
                ' 備考：
                '----------------------------------------------------------------
                Public Shared Function Connect(ByVal sock As Socket, ByVal localEP As EndPoint) As Boolean
                    Try
                        '' コネクト待ち (ブロッキング)
                        sock.Connect(localEP)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket_Base::Connect] コネクト 完了", Now))
#End If
                        Return True
                    Catch ex As SocketException
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket_Base::Connect] コネクト 失敗 {1}", Now, ex.ErrorCode))
#End If
                        Return False
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：アクセプト
                ' 引数：(in :Socket) sock = ソケット
                ' 引数：(in :AsyncCallback) method = コールバック関数のアドレス
                ' 戻値：(IAsyncResult) 非同期操作オブジェクト
                ' 備考：
                '----------------------------------------------------------------
                Public Shared Function Listen(ByVal sock As Socket, ByVal method As AsyncCallback) As IAsyncResult
                    Dim ar As IAsyncResult
                    Try
                        '' アクセプト待ち
                        ar = sock.BeginAccept(method, sock)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket_Base::Listen] アクセプト待ち非同期要求", Now))
#End If
                        Return ar
                    Catch ex As SocketException
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket_Base::Listen] アクセプト待ち 失敗 {1]", Now, ex.ErrorCode))
#End If
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：非同期受信
                ' 引数：(in :Socket) sock = ソケット
                ' 引数：(in :tClass_Socket) clsSock = ソケットクラス
                ' 引数：(in :Integer) iSize = 受信サイズ
                ' 引数：(in :AsyncCallback) method = コールバック関数のアドレス
                ' 戻値：(IAsyncResult) 非同期操作のステータス 
                ' 備考：最初の場合
                '----------------------------------------------------------------
                Public Overloads Shared Function Recv(ByVal sock As Socket, ByVal clsSock As tClass_Socket, ByVal iSize As Integer, ByVal method As AsyncCallback) As IAsyncResult
                    Dim state As New tClass_Socket_State_Recv(clsSock, iSize)

                    Try
                        '' 非同期受信開始
                        'sock.BeginReceive(state.buffer, 0, iSize, 0, New AsyncCallback(AddressOf ReadCallback), state)
                        Return sock.BeginReceive(state.buffer, 0, iSize, 0, method, state)
                    Catch ex As Exception
                        Return Nothing
                    End Try
                End Function
                '----------------------------------------------------------------
                ' 概要：非同期受信
                ' 引数：(in :Socket) sock = ソケット
                ' 引数：(in :tClass_Socket_State_Recv) state = 受信受け渡しクラス
                ' 引数：(in :AsyncCallback) method = コールバック関数のアドレス
                ' 戻値：(IAsyncResult) 非同期操作のステータス 
                ' 備考：伝文が分割された場合等
                '----------------------------------------------------------------
                Public Overloads Shared Function Recv(ByVal sock As Socket, ByVal state As tClass_Socket_State_Recv, ByVal method As AsyncCallback) As IAsyncResult
                    Dim ar As IAsyncResult = Nothing
                    Dim iSize As Integer
                    Try
                        '' 途中の受信を再度読み込み
                        iSize = state.buffer.Length - state.nowRecvSize
                        Return sock.BeginReceive(state.buffer, state.nowRecvSize, iSize, 0, method, state)
                    Catch ex As Exception
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：非同期送信
                ' 引数：(in :Socket) sock = ソケット
                ' 引数：(in :tClass_Socket_State_Recv) state = 受信受け渡しクラス
                ' 引数：(in :AsyncCallback) method = コールバック関数のアドレス
                ' 戻値：(IAsyncResult) 非同期操作のステータス 
                ' 備考：伝文が分割された場合等
                '----------------------------------------------------------------
                Public Overloads Shared Function Send(ByVal sock As Socket, ByVal state As tClass_Socket_State_Send, ByVal method As AsyncCallback) As IAsyncResult
                    Dim ar As IAsyncResult = Nothing
                    Dim iSize As Integer
                    Try
                        '' 送信サイズ指定
                        iSize = state.buffer.Length - state.nowSendSize
                        '' 送信
                        Return sock.BeginSend(state.buffer, state.nowSendSize, iSize, 0, method, state)
                    Catch ex As Exception
                        Return Nothing
                    End Try
                End Function

                ''----------------------------------------------------------------
                '' 概要：同期送信
                '' 引数：(in :Socket) sock = ソケット
                '' 引数：(in :tClass_Socket_State_Recv) state = 受信受け渡しクラス
                '' 戻値：(Integer) 
                '' 備考：
                ''----------------------------------------------------------------
                'Public Overloads Shared Function SendLock(ByVal sock As Socket, ByVal state As tClass_Socket_State_Send) As Integer
                '	Dim iSize As Integer
                '	Try
                '		'' 送信サイズ指定
                '		iSize = state.buffer.Length - state.nowSendSize
                '		'' 送信
                '		Return sock.Send(state.buffer, state.nowSendSize, SocketFlags.None)
                '	Catch ex As Exception
                '		Return Nothing
                '	End Try
                'End Function

            End Class



            '********************************************************************
            'ソケット マネージャー 
            '********************************************************************
            Public Class tClass_SocketMgr
            Implements tIface.tIface_Find				'tClass_Arrayの検索用インターフェース実装

                '********************************************************************
                ' 定数定義
                '********************************************************************
                Private Const mc_ListenMaxCount As Integer = 32		'リスナー最大数 

                '********************************************************************
                'メンバー変数定義
                '********************************************************************
                Private m_SockBase As Socket				'最初に作ったソケット
                Private m_SockAry As tClass_Array			'ソケットクラスの全クラス
                Private m_EP As EndPoint					'エンドポイント

                Private ms_Listen As Threading.ManualResetEvent					'接続確立シグナル
                Private m_WThreadLs As tClass_WorkThread						'リスナー生成待ちスレッド

                Private m_DeleSockEvent As tClass_Socket_Base.tDele_Sock_Event	'アンサーデリゲート

                '********************************************************************
                'プロパティ定義
                '********************************************************************
#Region "プロパティ定義"
                '====================================================================
                'ソケット取得
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
                '基本メソッド定義
                '********************************************************************
                '----------------------------------------------------------------
                ' 概要：ソケットクラス 取得
                '----------------------------------------------------------------
                Public Overloads Function find(ByVal Index As Integer) As tClass_Socket
                    If m_SockAry.Count = 0 Or Index < 0 Or m_SockAry.Count < Index Then Return Nothing '上下限チェック

                    '' クラス取得
                    Return CType(m_SockAry.Item(Index), tClass_Socket)
                End Function
                Public Overloads Function find(ByVal IP As String, ByVal Port As Integer) As tClass_Socket
                    Try
                        Return CType(m_SockAry.Find(New Object() {IP, Port}, Me), tClass_Socket)
                    Catch ex As Exception
                        '' 該当無し
                        Return Nothing
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：検索用 インターフェース 実装
                '----------------------------------------------------------------
            Private Overloads Function Find(ByVal key As Object, ByVal aryList As System.Collections.ArrayList) As Integer Implements tIface.tIface_Find.Find
                Dim cls_Sock As tClass_Socket
                Dim ii As Integer
                Dim ary As Array = CType(key, Array)

                '' 検索
                For ii = 0 To aryList.Count - 1
                    cls_Sock = CType(aryList(ii), tClass_Socket)
                    If CStr(ary.GetValue(0)) = cls_Sock.tProp_ToIP And CInt(ary.GetValue(1)) = cls_Sock.tProp_ToPort Then
                        Return ii
                    End If
                Next ii
                Return -1
            End Function

                '----------------------------------------------------------------
                ' 概要：ソケットクラス リストから除外
                '----------------------------------------------------------------
                Public Overloads Sub delete(ByVal clsSock As tClass_Socket)
                    If clsSock Is Nothing Then Exit Sub

                    '' クラスが開放されているかチェック
                    clsSock.Dispose()

                    '' リストから除外
                    m_SockAry.Remove(clsSock)
                End Sub
                Public Overloads Sub delete(ByVal Index As Integer)
                    Me.delete(Me.find(Index))
                End Sub
                Public Overloads Sub delete(ByVal IP As String, ByVal Port As Integer)
                    Me.delete(Me.find(IP, Port))
                End Sub

                '********************************************************************
                'メソッド定義
                '********************************************************************
                '----------------------------------------------------------------
                ' 概要：コンストラクタ
                ' 引数：(in :tDele_Sock_Event) deleMethod = アンサー の メッソドアドレス
                '----------------------------------------------------------------
                Public Sub New(ByVal deleMethod As tClass_Socket_Base.tDele_Sock_Event)
                    '' 引数セット
                    m_DeleSockEvent = deleMethod

                    '' 変数初期化
                    m_SockAry = New tClass_Array
                End Sub

                '----------------------------------------------------------------
                ' 概要：ソケット生成
                ' 引数：(in :SocketType) sType = スレッドで実行されるメソッド
                ' 引数：(in :ProtocolType) pType = ソケットタイプ
                ' 引数：(in :ThreadPriority) priority = プロトコルタイプ
                ' 戻値：(Boolean) 復帰情報
                ' 備考：
                'TCP
                'Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
                'UDP
                'Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp)
                '----------------------------------------------------------------
                Public Function SockOpen(ByVal sType As SocketType, ByVal pType As ProtocolType) As Boolean

                    If Not m_SockBase Is Nothing Then Return False

                    '' ソケットの生成
                    Try
                        m_SockBase = tClass_Socket_Base.SockOpen(sType, pType)
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_SocketMgr::SockOpen] ソケット 生成", Now))
#End If
                        Return True
                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_SocketMgr::SockOpen] ソケット 生成異常", Now))
#End If
                        Return False
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：ソケット終了
                ' 引数：(in :Socket) sock = ソケット
                ' 戻値：(Boolean) 復帰情報
                ' 備考：
                '----------------------------------------------------------------
                Public Function SockClose() As Boolean
                    If m_SockBase Is Nothing Then Return False

                    '' アクセプト完了済みのソケットをクローズ
                    Listen_Stop()

                    '' ソケットを全部閉じる
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

                    '' ベースソケット終了
                    If tClass_Socket_Base.SockClose(m_SockBase) Then
                        m_SockBase = Nothing
                        Return True
                    Else
                        Return False
                    End If
                End Function

                '----------------------------------------------------------------
                ' 概要：コネクト
                ' 引数：(in :String) ip = ipアドレス (192.168.1.1)
                ' 引数：(in :Integer) port = ポート
                ' 戻値：(Boolean) 復帰情報
                ' 備考：コネクト完了までブロックされます
                '----------------------------------------------------------------
                Public Function Connect(ByVal ip As String, ByVal port As Integer) As Boolean
                    If m_SockBase Is Nothing Then Return False

                    Try
                        '' ネットワーク エンドポイント
                        m_EP = New IPEndPoint(IPAddress.Parse(ip), port)

                        '' コネクト
                        If Not tClass_Socket_Base.Connect(m_SockBase, m_EP) Then
                            Return False
                        End If
                        '' ソケットクラス生成
                        Dim Index As Integer
                        Index = m_SockAry.Add(New tClass_Socket(m_SockBase, m_DeleSockEvent))

                        '' 通知
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Connect(Me.find(Index)), tClass_Socket_Base.tEnum_Sock_EventNO.CONNECT, Nothing, Nothing)
                        Return True
                    Catch ex As Exception
                        Return False
                    End Try
                End Function

                '----------------------------------------------------------------
                ' 概要：アクセプト待ち開始
                ' 引数：(in :Integer) port = ポート
                ' 引数：(in :String) ip = ipアドレス
                ' 戻値：なし
                ' 備考：非同期です(ノンブロッキング)
                '----------------------------------------------------------------
                Public Sub Listen(ByVal port As Integer, Optional ByVal ip As String = "0.0.0.0")
                    If m_SockBase Is Nothing Then Return
                    If Not m_WThreadLs Is Nothing Then Return

                    '' 自分のアドレス取得
                    ''''====================================================
                    ' ケーブルを抜いて アクセプト待ちにした場合、127.0.0.1:port で待ちになる為、ケーブルがつながってコネクトして来ても
                    ' 接続されないと言うバグが発生  その対策
                    'Dim ipHostInfo As IPHostEntry = Dns.GetHostEntry(Dns.GetHostName())		'自分のネットワーク設定を取得
                    'If "127.0.0.1" = ipHostInfo.AddressList(0).ToString Then
                    '	m_EP = New IPEndPoint(IPAddress.Parse("0.0.0.0"), port)
                    'Else
                    '	m_EP = New IPEndPoint(ipHostInfo.AddressList(0), port)	  'ネットワーク エンドポイント
                    'End If
                    m_EP = New IPEndPoint(IPAddress.Parse(ip), port)
                    ''''====================================================

                    '' リスナースレッド開始
                    ms_Listen = New Threading.ManualResetEvent(False)
                    m_WThreadLs = New tClass_WorkThread
                    m_WThreadLs.ThreadStart(AddressOf Listen_Thread, Threading.ThreadPriority.Normal, "アクセプト待ちスレッド", False)
                End Sub

                '----------------------------------------------------------------
                ' 概要：アクセプト待ちスレッド停止
                ' 引数：(in :Integer) port = ポート
                ' 戻値：なし
                ' 備考：
                '----------------------------------------------------------------
                Private Sub Listen_Stop()
                    If m_SockBase Is Nothing Then Return
                    If m_WThreadLs Is Nothing Then Return

                    '' アクセプト待ちスレッド停止
                    'm_WThreadLs.g_evStop.Set()
                    m_WThreadLs.ThreadStop(1000)

                    '' アクセプト終了処理
                    m_WThreadLs = Nothing
                    ms_Listen.Close()
                    ms_Listen = Nothing
                End Sub

                '----------------------------------------------------------------
                ' 概要：アクセプト待ち ワーカースレッド
                ' 引数：(in :Integer) port = ポート
                ' 戻値：なし
                ' 備考：
                '----------------------------------------------------------------
                Private Sub Listen_Thread()
                    Dim ar As IAsyncResult			'非同期操作オブジェクト (現状は特に使用していない)

                    '' シグナル 生成
                    Dim iRetc As Integer
                    Dim EventAry(1) As Threading.WaitHandle				'シグナル待ち
                    EventAry(0) = m_WThreadLs.g_evStop					'終了シグナル
                    EventAry(1) = ms_Listen								'受信シグナル

                    Try
                        '' バインド
                        m_SockBase.Bind(m_EP)
                        m_SockBase.Listen(mc_ListenMaxCount)

                        '' アクセプト要求ループ
                        While True
                            ms_Listen.Reset()

                            '' アクセプト
                            ar = tClass_Socket_Base.Listen(m_SockBase, New AsyncCallback(AddressOf AcceptCallback))

                            '' シグナル待ち
                            iRetc = Threading.WaitHandle.WaitAny(EventAry)
                            If iRetc = 0 Then Exit While '終了確認
                        End While
                    Catch ex As Threading.ThreadAbortException
                        '' スレッド強制停止
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_SocketMgr::Listen_Thread] アクセプトスレッド強制停止", Now))
#End If
                    Catch ex As Exception
                        '' バインドエラー
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_SocketMgr::Listen_Thread] アクセプトスレッド異常", Now))
#End If
                        Exit Sub
                    End Try

                    '' アクセプト要求終了
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_SocketMgr::Listen_Thread] アクセプト待ち終了", Now))
#End If
                End Sub

                '----------------------------------------------------------------
                ' 概要：アクセプト有り
                ' 引数：(in :IAsyncResult) ar = 非同期操作のステータス
                ' 戻値：なし
                ' 備考：
                '----------------------------------------------------------------
                Private Sub AcceptCallback(ByVal ar As IAsyncResult)
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_SocketMgr::AcceptCallback] アクセプト コールバック", Now))
#End If
                    ''シグナル
                    If ms_Listen Is Nothing Then Exit Sub
                    ms_Listen.Set()

                    '' 接続元ソケット情報取得
                    Dim baseSock As Socket = CType(ar.AsyncState, Socket)		'm_Sockの事
                    If baseSock.Handle.ToInt32 < 0 Then Exit Sub '切断時 アクセプト中だった場合 コールバックされる為

                    '' 接続先ソケット情報取得
                    Dim newSock As Socket = baseSock.EndAccept(ar)				'接続してきたSocket

                    '' ソケットクラス生成
                    Dim Index As Integer
                    Index = m_SockAry.Add(New tClass_Socket(newSock, m_DeleSockEvent))

                    '' 通知
                    m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Accept(Me.find(Index)), tClass_Socket_Base.tEnum_Sock_EventNO.ACCEPT, Nothing, Nothing)
                End Sub

                '----------------------------------------------------------------
                ' 概要：全ソケットに送信
                ' 引数：(in :Byte) buffer = 送信データ
                ' 戻値：なし
                ' 備考：
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
                '' 概要：全ソケットに同期送信
                '' 引数：(in :Byte) buffer = 送信データ
                '' 戻値：なし
                '' 備考：
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
            'ソケット クラス (実際の送受信)
            '********************************************************************
            Public Class tClass_Socket
                Implements IDisposable							'デストラクタ定義

                '********************************************************************
                'メンバー変数定義
                '********************************************************************
                Private m_Sock As Socket				'ソケット

                Private m_DeleSockEvent As tClass_Socket_Base.tDele_Sock_Event	'アンサーデリゲート

                Private ms_StopOK As New Threading.ManualResetEvent(True)		'停止時非シグナル
                Private ms_RecvOK As New Threading.ManualResetEvent(True)		'受信中は非シグナル
                Private ms_SendOK As New Threading.ManualResetEvent(True)		'送信中は非シグナル

                Private m_arRecv As System.IAsyncResult				'受信中 非同期操作クラス
                Private m_arSend As System.IAsyncResult				'送信中 非同期操作クラス

                '********************************************************************
                'プロパティ定義
                '********************************************************************
#Region "プロパティ定義"
                '====================================================================
                '相手のIP取得
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
                '相手のPort取得
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
                'ソケット取得
                '====================================================================
                Public ReadOnly Property tProp_Sock() As Socket
                    Get
                        Return Me.m_Sock
                    End Get
                End Property

                '====================================================================
                '受信タイムアウト設定
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
                '送信タイムアウト設定
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
                'メソッド定義
                '********************************************************************
                '----------------------------------------------------------------
                ' 概要：コンストラクタ
                ' 引数：(in :Socket) sock = ソケット
                ' 引数：(in :tDele_Sock_Event) deleMethod = アンサー の メッソドアドレス
                '----------------------------------------------------------------
                Public Sub New(ByVal sock As Socket, ByVal deleMethod As tClass_Socket_Base.tDele_Sock_Event)
                    '' 引数セット
                    m_Sock = sock
                    m_DeleSockEvent = deleMethod
                    m_arRecv = Nothing
                    m_arSend = Nothing

                    '' デフォルトタイムアウト設定
                    Me.tProp_RecvTimeOut = 1000
                    Me.tProp_SendTimeOut = 1000
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::New] クラス生成 接続元 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                End Sub

                '----------------------------------------------------------------
                ' 概要：デストラクタ
                ' 引数：(in :)  = 
                '----------------------------------------------------------------
                Public Sub Dispose() Implements System.IDisposable.Dispose
                    If m_Sock Is Nothing Then Return

#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::Dispose] クラス開放 接続元 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If

                    '' 通知データ生成
                    Dim state As New tClass_Socket_State_Close(Me, Me.tProp_ToIP, Me.tProp_ToPort)

                    '' 停止シグナル
                    ms_StopOK.Reset()

                    '' 非同期の送受信を終了
                    tClass_Socket_Base.SockShutdown(m_Sock)
                    '' 送受信終了待ち
                    If Not ms_RecvOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Dispose] クラス開放 非同期受信 終了失敗 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If
                    If Not ms_SendOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Dispose] クラス開放 非同期受信 終了失敗 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If

                    '' ソケット終了
                    If tClass_Socket_Base.SockClose(m_Sock) Then
                        m_Sock = Nothing

                        '' 通知
                        m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.CLOSE_OTHER, Nothing, Nothing)
                    End If
                End Sub

                '----------------------------------------------------------------
                ' 概要：相手先からクローズ通知
                ' 引数：(in :)  = 
                '----------------------------------------------------------------
                Private Sub Close()
                    If m_Sock Is Nothing Then Return

#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::Close] クラス開放 接続元 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If

                    '' 通知データ生成
                    Dim state As New tClass_Socket_State_Close(Me, Me.tProp_ToIP, Me.tProp_ToPort)

                    '' 停止シグナル
                    ms_StopOK.Reset()

                    '' 非同期の送受信を終了
                    tClass_Socket_Base.SockShutdown(m_Sock)
                    '' 送受信終了待ち
                    If Not ms_RecvOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Close] クラス開放 非同期受信 終了失敗 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If
                    If Not ms_SendOK.WaitOne(5000, True) Then
#If tDebug Then
            Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Close] クラス開放 非同期受信 終了失敗 IP[{1}] ポート[{2}]", Now, Me.tProp_ToIP, Me.tProp_ToPort))
#End If
                    End If

                    '' ソケット終了
                    If tClass_Socket_Base.SockClose(m_Sock) Then
                        m_Sock = Nothing

                        '' 通知
                        m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.CLOSE_SOCK, Nothing, Nothing)
                    End If
                End Sub

                '----------------------------------------------------------------
                ' 概要：受信開始
                ' 引数：(in :) iSize = 受信バッファサイズ
                ' 戻値：なし
                ' 備考：可変バイト用
                '----------------------------------------------------------------
                Public Function Recv(ByVal iSize As Integer) As Integer
                    If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '切断時 送信中だった場合 コールバックされる為

                    '''''' 受信中は、再度受信しない
                    If Not m_arRecv Is Nothing AndAlso Not m_arRecv.IsCompleted Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Recv] 受信開始異常 送信先 IP[{1}] ポート[{2}] 受信バッファ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If
                        Return -3	'前回受信中
                    End If

#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::Recv] 受信開始 送信先 IP[{1}] ポート[{2}] 受信バッファ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If

                    '' 受信開始
                    ms_RecvOK.Reset()
                    m_arRecv = tClass_Socket_Base.Recv(m_Sock, Me, iSize, New AsyncCallback(AddressOf ReadCallback))
                    Return 0
                End Function
                '----------------------------------------------------------------
                ' 概要：受信完了
                ' 引数：(in :IAsyncResult) ar = 非同期操作のステータス
                '----------------------------------------------------------------
                Private Sub ReadCallback(ByVal ar As IAsyncResult)
                    Dim state As tClass_Socket_State_Recv		'非同期オブジェクト
                    Dim bytesRead As Integer					'受信バイト数

                    '' コールバック値取得
                    Try
                        state = CType(ar.AsyncState, tClass_Socket_State_Recv)
                        Dim sock As Socket = state.clsSock.tProp_Sock
                        If Not sock.Connected OrElse sock.Handle.ToInt32 < 0 Then Exit Sub '切断時 受信中だった場合 コールバックされる為
                        bytesRead = sock.EndReceive(ar)

                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::ReadCallback] 受信完了異常 {1}", Now, ex.Message))
#End If
                        '' 通知
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Error(Me, ex), tClass_Socket_Base.tEnum_Sock_EventNO.RECV_ERROR, Nothing, Nothing)
                        Exit Sub
                    Finally
                        '受信完了
                        ms_RecvOK.Set()
                    End Try

                    '' 停止？
                    If Not ms_StopOK.WaitOne(1, True) Then Exit Sub

                    '' 受信バイトサイズより処理分け
                    If bytesRead = 0 Then				'繋がっているソケットが切断
                        Me.Close()						  '終了

                    ElseIf bytesRead > 0 Then			'伝文受信
                        state.nowRecvSize = bytesRead
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::ReadCallback] 受信完了 送信先 IP[{1}] ポート[{2}] 受信サイズ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, state.nowRecvSize))
#End If
                        '' 通知
                        m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.RECV, Nothing, Nothing)
                    End If
                End Sub

                '----------------------------------------------------------------
                ' 概要：受信開始
                ' 引数：(in :) iSize = 受信サイズ
                ' 戻値：なし
                ' 備考：固定バイト用
                '----------------------------------------------------------------
                Public Function Recv_Len(ByVal iSize As Integer) As Integer
                    If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '切断時 送信中だった場合 コールバックされる為

                    '' 受信中は、再度受信しない
                    If Not m_arRecv Is Nothing AndAlso Not m_arRecv.IsCompleted Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Recv_Len] 受信開始異常 送信先 IP[{1}] ポート[{2}] 受信バッファ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If
                        Return -3	'前回受信中
                    End If

#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::Recv_Len] 受信開始 送信先 IP[{1}] ポート[{2}] 受信サイズ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, iSize))
#End If

                    '' 受信開始
                    ms_RecvOK.Reset()
                    m_arRecv = tClass_Socket_Base.Recv(m_Sock, Me, iSize, New AsyncCallback(AddressOf ReadCallback_Len))
                    Return 0
                End Function
                '----------------------------------------------------------------
                ' 概要：受信完了
                ' 引数：(in :IAsyncResult) ar = 非同期操作のステータス
                '----------------------------------------------------------------
                Private Sub ReadCallback_Len(ByVal ar As IAsyncResult)
                    Dim state As tClass_Socket_State_Recv		'非同期オブジェクト
                    Dim bytesRead As Integer					'受信バイト数

                    '' コールバック値取得
                    Try
                        state = CType(ar.AsyncState, tClass_Socket_State_Recv)
                        Dim sock As Socket = state.clsSock.tProp_Sock
                        If Not sock.Connected OrElse sock.Handle.ToInt32 < 0 Then Exit Sub '切断時 受信中だった場合 コールバックされる為
                        bytesRead = sock.EndReceive(ar)

                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::ReadCallback_Len] 受信完了異常 {1}", Now, ex.Message))
#End If
                        '' 通知
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Error(Me, ex), tClass_Socket_Base.tEnum_Sock_EventNO.RECV_ERROR, Nothing, Nothing)
                        Exit Sub
                    Finally
                        '受信完了
                        ms_RecvOK.Set()
                    End Try

                    '' 停止？
                    If Not ms_StopOK.WaitOne(1, True) Then Exit Sub

                    '' 受信バイトサイズより処理分け
                    If bytesRead = 0 Then				'繋がっているソケットが切断
                        Me.Close()						  '終了

                    ElseIf bytesRead > 0 Then			'伝文受信
                        state.nowRecvSize += bytesRead
                        If state.nowRecvSize = state.buffer.Length Then		'指定バイト到達
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::ReadCallback_Len] 受信完了 送信先 IP[{1}] ポート[{2}] 受信サイズ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, state.nowRecvSize))
#End If
                            '' 通知
                            m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.RECV, Nothing, Nothing)
                        Else												'再度読み込み
                            tClass_Socket_Base.Recv(state.clsSock.m_Sock, state, New AsyncCallback(AddressOf ReadCallback_Len))
                        End If
                    End If
                End Sub

'				'----------------------------------------------------------------
'				' 概要：同期送信
'				' 引数：(in :Byte) buffer = 送信データ
'				' 戻値：なし
'				' 備考：固定バイト用
'				'----------------------------------------------------------------
'				Public Function SendLock(ByVal buffer() As Byte) As Integer
'					If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '切断時 送信中だった場合 コールバックされる為

'					'' 送信中は、再度送信しない
'					If Not m_arSend Is Nothing AndAlso Not m_arSend.IsCompleted Then
'#If tDebug Then
'                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Send] 送信開始異常 送信先 IP[{1}] ポート[{2}] 送信バッファ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
'#End If
'						Return -3	'前回送信中
'					End If

'					Dim state As New tClass_Socket_State_Send(Me)
'					state.buffer = buffer
'#If tDebug Then
'                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::Send] 送信開始 送信先 IP[{1}] ポート[{2}] 送信サイズ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
'#End If
'					'' 送信開始
'					If 0 <> tClass_Socket_Base.SendLock(m_Sock, state) Then
'						Return 0
'					Else
'						Return -1
'					End If
'					'tClass_Socket_Base.Send(m_Sock, state, New AsyncCallback(AddressOf SendCallback))
'					Return 0
'				End Function

                '----------------------------------------------------------------
                ' 概要：送信開始
                ' 引数：(in :Byte) buffer = 送信データ
                ' 戻値：なし
                ' 備考：固定バイト用
                '----------------------------------------------------------------
                Public Function Send(ByVal buffer() As Byte) As Integer
                    If m_Sock Is Nothing OrElse Not m_Sock.Connected OrElse m_Sock.Handle.ToInt32 < 0 Then Return -1 '切断時 送信中だった場合 コールバックされる為

                    '' 送信中は、再度送信しない
                    If Not m_arSend Is Nothing AndAlso Not m_arSend.IsCompleted Then
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::Send] 送信開始異常 送信先 IP[{1}] ポート[{2}] 送信バッファ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
#End If
                        Return -3	'前回送信中
                    End If

                    Dim state As New tClass_Socket_State_Send(Me)
                    state.buffer = buffer
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::Send] 送信開始 送信先 IP[{1}] ポート[{2}] 送信サイズ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, buffer.Length))
#End If
                    '' 送信開始
                    ms_SendOK.Reset()
                    m_arSend = tClass_Socket_Base.Send(m_Sock, state, New AsyncCallback(AddressOf SendCallback))
                    'tClass_Socket_Base.Send(m_Sock, state, New AsyncCallback(AddressOf SendCallback))
                    Return 0
                End Function
                '----------------------------------------------------------------
                ' 概要：送信完了
                ' 引数：(in :IAsyncResult) ar = 非同期操作のステータス
                '----------------------------------------------------------------
                Private Sub SendCallback(ByVal ar As IAsyncResult)
                    Dim state As tClass_Socket_State_Send		'非同期オブジェクト
                    Dim bytesSend As Integer					'送信バイト数

                    '' コールバック値取得
                    Try
                        state = CType(ar.AsyncState, tClass_Socket_State_Send)
                        Dim sock As Socket = state.clsSock.tProp_Sock
                        If Not sock.Connected OrElse sock.Handle.ToInt32 < 0 Then Exit Sub '切断時 送信中だった場合 コールバックされる為
                        bytesSend = sock.EndSend(ar)

                    Catch ex As Exception
#If tDebug Then
                    Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ★ [tClass_Socket::SendCallback] 送信完了異常 {1}", Now, ex.Message))
#End If
                        '' 通知
                        m_DeleSockEvent.BeginInvoke(New tClass_Socket_State_Error(Me, ex), tClass_Socket_Base.tEnum_Sock_EventNO.SEND_ERROR, Nothing, Nothing)
                        Exit Sub
                    Finally
                        '受信完了
                        ms_SendOK.Set()

                    End Try

                    '' 停止？
                    If Not ms_StopOK.WaitOne(1, True) Then Exit Sub

                    '' 受信バイトサイズより処理分け
                    If bytesSend = 0 Then				'繋がっているソケットが切断
                        Me.Close()						  '終了

                    ElseIf bytesSend > 0 Then			'伝文受信
                        state.nowSendSize += bytesSend
                        If state.nowSendSize = state.buffer.Length Then		'指定バイト到達
#If tDebug Then
                Debug.WriteLine(String.Format("[{0:yyyy/MM/dd hh:mm:ss.fff}] ☆ [tClass_Socket::SendCallback] 送信完了 送信先 IP[{1}] ポート[{2}] 送信サイズ[{3}]", Now, Me.tProp_ToIP, Me.tProp_ToPort, state.nowSendSize))
#End If

                            '' 通知
                            m_DeleSockEvent.BeginInvoke(state, tClass_Socket_Base.tEnum_Sock_EventNO.SEND, Nothing, Nothing)
                        Else												'再度読み込み
                            tClass_Socket_Base.Send(state.clsSock.m_Sock, state, New AsyncCallback(AddressOf SendCallback))
                        End If
                    End If
                End Sub

            End Class
        End Class
End Namespace