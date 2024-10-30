'*********************************************************************************
'Threadの動作を行う基本クラス
'*********************************************************************************
Option Strict On                'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Threading


Namespace tClass
    Public Class tClass_WorkerThPool
        Implements IDisposable          'デストラクタ定義

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private Shared m_ary As ArrayList

        '********************************************************************
        'メソッド定義
        '********************************************************************
        '----------------------------------------------------------------
        ' 概要：コンストラクタ
        ' 引数：(in :)  = 
        '----------------------------------------------------------------
        Shared Sub New()
            m_ary = New ArrayList
        End Sub

        '----------------------------------------------------------------
        ' 概要：Disposeデストラクタ
        ' 引数：(in :)  = 
        ' 戻値：() 
        ' 備考：
        '----------------------------------------------------------------
        Public Sub Dispose() Implements System.IDisposable.Dispose
            Try
                '' 全件削除
                ResetEvent()
            Catch ex As Exception
            End Try
        End Sub

        '----------------------------------------------------------------
        ' 概要：ファイナライズ
        ' 引数：(in :)  = 
        '----------------------------------------------------------------
        Protected Overrides Sub Finalize()
            Dispose()
            MyBase.Finalize()
        End Sub


        '----------------------------------------------------------------
        ' 概要：イベントタイマーセット
        ' 引数：(in :Integer) key = 一意なキー
        ' 引数：(in :Object) returnSender = デリゲートの戻り値
        ' 引数：(in :Object) returnE = デリゲートの戻り値
        ' 引数：(in :Integer) timeOut = タイムアウト値 [ms]
        ' 引数：(in :Delegate) callBack = デリゲートインスタンス (New tClass.tClass_232C.tDele_232C_Event(AddressOf subDele_232C_Event))こんな感じ
        ' 戻値：なし
        ' 備考：
        '----------------------------------------------------------------
        Public Shared Sub SetEvent(ByVal key As Integer, ByVal returnSender As Object, ByVal returnE As Object, ByVal timeOut As Integer, ByVal callBack As [Delegate])
            Dim inf As New tClass.tClass_WorkerThPool.TaskInfo(key, callBack, returnSender, returnE)      '受け渡し用クラス生成
            inf._ev = New AutoResetEvent(False)                                                         'シグナル生成
            'スレッドプールに登録
            inf._handle = ThreadPool.RegisterWaitForSingleObject(inf._ev, _
                            New WaitOrTimerCallback(AddressOf subCallBack), _
                            inf, timeOut, True)
SyncLock m_ary
            m_ary.Add(inf)
End SyncLock
        End Sub

        '----------------------------------------------------------------
        ' 概要：イベントタイマーキャンセル
        ' 引数：(in :) 
        ' 戻値：なし
        ' 備考：全アイテム削除
        '----------------------------------------------------------------
        Public Overloads Shared Sub ResetEvent()
SyncLock m_ary
            Dim inf As TaskInfo
            '' 全アイテム削除
            For Each inf In m_ary
                If Not inf._ev Is Nothing Then
                    '初期化
                    inf._handle.Unregister(Nothing)
                    inf._handle = Nothing
                    inf._ev.Close()
                    inf._ev = Nothing
                    inf._callBack = Nothing
                    inf._key = Nothing
                    inf._sender = Nothing
                    inf._e = Nothing
                    inf = Nothing
                End If
            Next
            '' リスト全削除
            m_ary.Clear()
End SyncLock
        End Sub

        '----------------------------------------------------------------
        ' 概要：イベントタイマーキャンセル
        ' 引数：(in :Integer) key = 一意なキー
        ' 戻値：(Boolean) True:キャンセル成功 False:該当無し(実行済み？)
        ' 備考：
        '----------------------------------------------------------------
        Public Overloads Shared Function ResetEvent(ByVal key As Integer) As Boolean
SyncLock m_ary
            Dim inf As TaskInfo
            '' 指定キーのみ削除
            For Each inf In m_ary
                If inf._key = key Then
                    If inf._ev Is Nothing Then Return False
                    m_ary.Remove(inf)
                    '初期化
                    inf._handle.Unregister(Nothing)
                    inf._handle = Nothing
                    inf._ev.Close()
                    inf._ev = Nothing
                    inf._callBack = Nothing
                    inf._key = Nothing
                    inf._sender = Nothing
                    inf._e = Nothing
                    inf = Nothing
                    Return True
                End If
            Next inf
End SyncLock
            Return False
        End Function

        '￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
        ' 概要：コールバック
        '＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
        Private Shared Sub subCallBack(ByVal state As Object, ByVal timedOut As Boolean)
            Dim inf As TaskInfo
SyncLock m_ary
            inf = CType(state, TaskInfo)
            If inf._ev Is Nothing Then Exit Sub
            m_ary.Remove(inf)
End SyncLock
            '' 指定されたデリゲートへ通知
            inf._callBack.DynamicInvoke(New Object() {inf._sender, inf._e})
            '初期化
            inf._handle.Unregister(Nothing)
            inf._handle = Nothing
            inf._ev.Close()
            inf._ev = Nothing
            inf._callBack = Nothing
            inf._key = Nothing
            inf._sender = Nothing
            inf._e = Nothing
            inf = Nothing
        End Sub

        '********************************************************************
        ' 受け渡しクラス
        '********************************************************************
        Private Class TaskInfo
            Public _handle As RegisteredWaitHandle = Nothing    'スレッドハンドル
            Public _ev As AutoResetEvent
            Public _key As Integer              '一意なキー情報
            Public _callBack As [Delegate]      '呼び出し先

            ''' 受け渡しデータ
            Public _sender As Object
            Public _e As Object

            ''' コンストラクタ
            Public Sub New(ByVal key As Integer, ByVal callBack As [Delegate], ByVal state As Object, ByVal e As Object)
                _key = key
                _callBack = callBack
                _sender = state
                _e = e
            End Sub
        End Class

    End Class
End Namespace