'*********************************************************************************
'Thread動作の管理を行う基底クラス
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Threading						' スレッド

Namespace tClass
    ''' <summary>
    ''' Thread動作の管理を行う基底クラス
    ''' </summary>
    ''' <remarks>ワーカースレッドクラスを生成する場合は、必ず本クラスを継承するかメンバーに加える事</remarks>
    Public Class tClass_WorkThread
        Implements IDisposable							' デストラクタ定義


        '********************************************************************
        'グローバル変数定義
        '********************************************************************
        Public g_evStop As AutoResetEvent				' スレッド終了シグナル

        '********************************************************************
        'ローカル変数定義
        '********************************************************************
        Private disposedValue As Boolean = False		' 重複する呼び出しを検出するには
        Protected Friend my_pThread As Thread			' メインスレッドインスタンス

        '********************************************************************
        'プロパティ
        '********************************************************************
        ''' <summary>
        ''' スレッドID (読み取り専用)
        ''' </summary>
        ''' <returns>スレッドID</returns>
        Public ReadOnly Property GetTid() As Integer
            Get
                If my_pThread Is Nothing Then
                    Return -1
                Else
                    Return my_pThread.ManagedThreadId()
                End If
            End Get
        End Property

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            ' シグナル生成 (非シグナル)
            g_evStop = New AutoResetEvent(False)
            my_pThread = Nothing
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                g_evStop.Close()
                g_evStop = Nothing
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' スレッド実行
        ''' </summary>
        ''' <param name="method">スレッドで実行されるメソッド (AddressOfで指定する事)</param>
        ''' <param name="priority">スレッドの優先度</param>
        ''' <param name="strTreadName">カレントスレッド名称</param>
        ''' <param name="background">スレッドの実行形式 (True:BackGround(thredが死ぬと一緒に死ぬ) False:通常)</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function ThreadStart(ByVal method As Threading.ThreadStart, Optional ByVal priority As Threading.ThreadPriority = ThreadPriority.Normal, Optional ByVal strTreadName As String = "", Optional ByVal background As Boolean = False) As Boolean
            '' スレッドチェック
            If Not my_pThread Is Nothing Then Return False

            '' 実行スレッド定義
            my_pThread = New System.Threading.Thread(method)
            If "" <> strTreadName Then my_pThread.Name = strTreadName
            my_pThread.IsBackground = background						   'BackGroundにすると生成スレッドが死ぬと当スレッドも死んでくれる為
            my_pThread.Priority = priority

            '' 実行
            my_pThread.Start()
            Return True
        End Function

#If VBC_VER >= 8.0 Then
        ''' <summary>
        ''' スレッド実行
        ''' </summary>
        ''' <param name="method">スレッドで実行されるメソッド (AddressOfで指定する事)</param>
        ''' <param name="objData">実行スレッドメソッドに渡される引数</param>
        ''' <param name="priority">スレッドの優先度</param>
        ''' <param name="strTreadName">カレントスレッド名称</param>
        ''' <param name="background">スレッドの実行形式 (True:BackGround(thredが死ぬと一緒に死ぬ) False:通常)</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function ThreadStart(ByVal method As Threading.ParameterizedThreadStart, ByVal objData As Object, Optional ByVal priority As Threading.ThreadPriority = ThreadPriority.Normal, Optional ByVal strTreadName As String = "", Optional ByVal background As Boolean = False) As Boolean
            '' スレッドチェック
            If Not my_pThread Is Nothing Then Return False

            '' 実行スレッド定義
            my_pThread = New System.Threading.Thread(method)
            If "" <> strTreadName Then my_pThread.Name = strTreadName
            my_pThread.IsBackground = background						   'BackGroundにすると生成スレッドが死ぬと当スレッドも死んでくれる為
            my_pThread.Priority = priority

            '' 実行
            my_pThread.Start(objData)
            Return True
        End Function
#End If

        ''' <summary>
        ''' スレッド終了
        ''' </summary>
        ''' <param name="iInterval">終了待ち時間 [ms]</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>例外処理として[ThreadAbortException]を実装する事</remarks>
        Public Function ThreadStop(Optional ByVal iInterval As Integer = 3000) As Boolean
            '' スレッドチェック
            If my_pThread Is Nothing Then Return False

            '' スレッド停止シグナル発行
            g_evStop.Set()

            '' スレッドが停止状態になるまでブロック
            Try
                If Not my_pThread.Join(iInterval) Then
                    'スレッド強制終了
                    my_pThread.Abort()
                    my_pThread = Nothing
                    'やっぱりダメだった･･･
                    Return False
                End If
            Catch ex As Exception
            End Try

            '' スレッド終了
            If my_pThread.IsAlive Then
                my_pThread.Abort()
            End If
            my_pThread = Nothing
            Return True
        End Function


    End Class
End Namespace
