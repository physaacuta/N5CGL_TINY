'*********************************************************************************
'MailSlotの送受信を行う基本クラス
'*********************************************************************************
Option Strict On                'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Threading

''''' 構造体はこんな感じに定義する事
''<System.Runtime.InteropServices.StructLayout(Runtime.InteropServices.LayoutKind.Sequential)> _
''Public Structure laytest
''    Dim events As Integer
''    <System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst:=124)> Dim val As String
''    <System.Runtime.InteropServices.MarshalAs(Runtime.InteropServices.UnmanagedType.ByValArray, SizeConst:=32)> Dim yobi() As Integer

''    Public Sub init()
''        ReDim yobi(32)
''    End Sub
''End Structure

Namespace tClass
    Public Class tClass_MailSlot
        Inherits tClass_MailSlot_Base                   '親クラス
        Implements IDisposable                          'デストラクタ定義

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        'イベント
        Private m_hEvent_End As AutoResetEvent          '終了イベント

        '受信用
        Protected m_RecvThread As System.Threading.Thread       '受信スレッド
        Private m_RecvValue As System.ValueType                 '受信データ
        Private m_ReadInterval As Integer                       '読込み周期

        '受信取り込みイベント
        'Public Event getMailRecv(ByVal recvVal As ValueType)    '受信取り込みイベント
        Public Delegate Sub tDele_GetMail(ByVal recvVal As System.ValueType) '受信取り込みデリゲート

        Protected Friend m_Control As System.Windows.Forms.Control      '受信取り込み用デリゲートオブジェクト(コントロールアクセス時)
        Protected Friend m_DeleGetMail As tDele_GetMail                 '受信取り込み用デリゲートオブジェクト(コントロールアクセス無し時)

        '********************************************************************
        'メソッド定義
        '********************************************************************
        '----------------------------------------------------------------
        ' 概要：コンストラクタ
        ' 引数：(in :String) strMyTaskName = 自分自身のタスク名称
        ' 引数：(in :Integer) interval = 受信周期(ms)
        ' 戻値：なし
        ' 備考：
        '----------------------------------------------------------------
        Public Sub New(ByVal strMyTaskName As String, ByVal interval As Integer)
            MyBase.new()        '親クラスインスタンス生成

            '''' メールスロット生成
            If Not MyBase.Mail_Open(strMyTaskName) Then
                'Debug.Assert(False, "メールスロット生成失敗")
                Throw New System.Exception("メールスロット生成失敗")
            End If

            '''' private変数にセット
            m_hEvent_End = New AutoResetEvent(False)
            m_ReadInterval = interval
        End Sub

        '----------------------------------------------------------------
        ' 概要：デストラクタ
        ' 引数：( :)  = 
        ' 戻値：なし
        ' 備考：
        '----------------------------------------------------------------
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            '''' 受信スレッド停止
            Call Mail_ReadStop()

            '''' イベント後始末
            m_hEvent_End.Close()

            '''' メールスロット終了
            MyBase.Dispose(disposing)
        End Sub

        '----------------------------------------------------------------
        ' 概要：受信開始
        ' 引数：(in :ValueType) objVal = 受信構造体
        ' 引数：(in :tDele_GetMail) deleMethod = 受信メソッド
        ' 戻値：なし
        ' 備考：処理メソッド用(コントロールにアクセス厳禁)
        '----------------------------------------------------------------
        Public Sub Mail_ReadStart(ByVal objVal As ValueType, ByVal deleMethod As tDele_GetMail)
            If Not m_RecvThread Is Nothing Then Exit Sub

            '''' 受信用領域セット
            m_RecvValue = objVal
            m_DeleGetMail = deleMethod
            m_Control = Nothing

            '''' 受信スレッド生成
            m_RecvThread = New Thread(AddressOf MailThread)
            m_RecvThread.IsBackground = True   'バックグラウンドで生成
            m_RecvThread.Priority = Threading.ThreadPriority.AboveNormal '通常よりちょっと高めの優先度
            m_RecvThread.Start()
        End Sub

        '----------------------------------------------------------------
        ' 概要：受信開始
        ' 引数：(in :ValueType) objVal = 受信構造体
        ' 引数：(in :tDele_GetMail) deleMethod = 受信メソッド
        ' 引数：(in :Control) meCont = 操作コントロール(通常Meで良い？でもパフォーマンス的には操作コントロール指定の方がいい？？)
        ' 戻値：なし
        ' 備考：
        '----------------------------------------------------------------
        Public Sub Mail_ReadStart(ByVal objVal As ValueType, ByVal deleMethod As tDele_GetMail, ByVal meCont As System.Windows.Forms.Control)
            If Not m_RecvThread Is Nothing Then Exit Sub

            '''' 受信用領域セット
            m_RecvValue = objVal
            m_DeleGetMail = deleMethod
            m_Control = meCont

            '''' 受信スレッド生成
            m_RecvThread = New Thread(AddressOf MailThread)
            m_RecvThread.IsBackground = True   'バックグラウンドで生成
            m_RecvThread.Priority = Threading.ThreadPriority.AboveNormal '通常よりちょっと高めの優先度
            m_RecvThread.Start()
        End Sub

        '----------------------------------------------------------------
        ' 概要：受信停止
        ' 引数：( :)  = 
        ' 戻値：なし
        ' 備考：
        '----------------------------------------------------------------
        Public Sub Mail_ReadStop()
            '''' メインスレッド終了確認
            If Not m_RecvThread Is Nothing Then
                'メインスレッドの終了
                m_hEvent_End.Set()
                'スレッドが停止状態になるまでブロック
                If Not m_RecvThread.Join(1000) Then
                    '多分スレッドが WaitSleepJoin 状態になっている?(ありえない)
                    m_RecvThread.Interrupt()
                End If
                'スレッド終了
                m_RecvThread.Abort()
                m_RecvThread = Nothing
                'm_DeleGetMail.RemoveAll(m_DeleGetMail, m_DeleGetMail)
            End If
        End Sub

        '----------------------------------------------------------------
        ' 概要：メインスレッド
        ' 引数：( : )  = 
        ' 戻値：() 
        ' 備考：
        '----------------------------------------------------------------
        Private Sub MailThread()
            Dim EventAry(1) As Threading.WaitHandle     'イベント
            Dim iRetc As Integer
            Dim hEvent_Read As New AutoResetEvent(False)    '強制受信イベント
            Dim iRecvSize As Integer                    '受信データサイズ

            '''' イベントセット
            EventAry(0) = m_hEvent_End
            EventAry(1) = hEvent_Read

            While (True)
                '''' シグナル待ち
                iRetc = Threading.WaitHandle.WaitAny(EventAry, m_ReadInterval, True)
                '''' まず初めに、スレッド終了
                If iRetc = 0 Then Exit While

                '''' 強制読込み 定周期読込み
                ''受信
                iRecvSize = MyBase.Mail_Recv(m_RecvValue)
                If iRecvSize > 0 Then           '受信有り
                    '受信イベント発生
                    'RaiseEvent getMailRecv(m_RecvValue)
                    '''' 受信データをデリゲートに渡す
                    If m_Control Is Nothing Then
                        m_DeleGetMail.BeginInvoke(m_RecvValue, Nothing, Nothing)
                    Else
                        m_Control.BeginInvoke(m_DeleGetMail, New Object() {m_RecvValue})
                    End If

                    '受信した場合は、この発生したイベントのモジュールが抜けるまで待たされるので、直に強制受信させる
                    hEvent_Read.Set()

                ElseIf iRecvSize = 0 Or iRecvSize = -1 Then	   '受信データ無し

                Else							'関数エラー
                    Debug.Assert(False, "受信スレッド エラー発生")
                    Throw New System.Exception("受信スレッド エラー発生")
                    m_hEvent_End.Set()	'終了シグナル
                End If
            End While

            '''' 後始末
            hEvent_Read.Close()
        End Sub
    End Class
End Namespace