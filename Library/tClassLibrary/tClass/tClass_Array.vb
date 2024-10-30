'*********************************************************************************
'同期を行った配列要素の基底クラス
'	[Ver]
'		Ver.01    2005/12/23  vs2005 対応
'			                  Generic 対応 (旧バージョンとして使いたい場合は、型にObjectを指定)
'		Ver.02    2006/01/28  バグ 対応
'		Ver.03    2006/01/28  vs2003 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass
    '///////////////////////////////////////////////////////////////////////////////
    '[Generic対応]
    '///////////////////////////////////////////////////////////////////////////////
#If VBC_VER >= 8.0 Then                         ' Generic に対応しているバージョン時
    ''' <summary>
    ''' 同期を行った配列要素の基底クラス [Generic対応]
    ''' </summary>
    ''' <typeparam name="T">[Generic] 要素の型</typeparam>
    ''' <typeparam name="KEY">[Generic] 検索キーの型</typeparam>
    ''' <remarks>[Generic] インスタンスを生成する場合に (Of String) 等指定する</remarks>
    <System.Serializable()> _
    Public Class tClass_Array(Of T, KEY)				' Genericに対応
        Implements IDisposable							' デストラクタ定義
        Implements IEnumerable							' For each が出来る定義

        '********************************************************************
        'ローカル変数定義
        '********************************************************************
        Private disposedValue As Boolean = False		' 重複する呼び出しを検出するには
        Protected Friend m_pAry As ArrayList			' 要素リスト
        Private m_pLockObject As Object					' SyncLock同期オブジェクト


        '********************************************************************
        'プロパティ定義
        '********************************************************************
        ''' <summary>
        ''' 指定したインデックスにある要素を取得または設定
        ''' </summary>
        ''' <param name="index">インデックス(0オリジン)</param>
        ''' <value>設定する要素</value>
        ''' <returns>取得した要素</returns>
        ''' <remarks></remarks>
        Default Public Property Item(ByVal index As Integer) As T
            Get
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                    Return CType(m_pAry.Item(index), T)
                End SyncLock
            End Get
            Set(ByVal Value As T)
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return
                    m_pAry.Item(index) = Value
                End SyncLock
            End Set
        End Property

        ''' <summary>
        ''' 要素の数
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property Count() As Integer
            Get
                SyncLock m_pLockObject
                    Return m_pAry.Count
                End SyncLock
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
            m_pAry = New ArrayList
            m_pLockObject = New Object
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                m_pAry.Clear()
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks>必ず キューに溜まっている要素は 開放しておく</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' 要素をリストの最初に挿入
        ''' </summary>
        ''' <param name="gnrT">[Generic] 追加する要素</param>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Sub Prepend(ByVal gnrT As T)
            SyncLock m_pLockObject
                m_pAry.Insert(0, gnrT)
            End SyncLock
        End Sub

        ''' <summary>
        ''' 要素をリストの最後に追加
        ''' </summary>
        ''' <param name="gnrT">[Generic] 追加する要素</param>
        ''' <returns>追加された要素の位置</returns>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Function Add(ByVal gnrT As T) As Integer
            SyncLock m_pLockObject
                Return m_pAry.Add(gnrT)
            End SyncLock
        End Function

        ''' <summary>
        ''' 要素を指定位置に追加
        ''' </summary>
        ''' <param name="index">位置 (0オリジン) [0～Countの間]</param>
        ''' <param name="gnrT">[Generic] 追加する要素</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Function Insert(ByVal index As Integer, ByVal gnrT As T) As Boolean
            SyncLock m_pLockObject
                If index > m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.Insert(index, gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' 最初の要素を取り出す
        ''' </summary>
        ''' <returns>取り出した要素 (異常時 Nothing)</returns>
        ''' <remarks>取得要素は、リストから削除される</remarks>
        Public Function GetHead() As T
            Return Me.GetItem(0)
        End Function

        ''' <summary>
        ''' 最後の要素を取り出す
        ''' </summary>
        ''' <returns>取り出した要素 (異常時 Nothing)</returns>
        ''' <remarks>取得要素は、リストから削除される</remarks>
        Public Function GetTail() As T
            Return Me.GetItem(m_pAry.Count - 1)
        End Function

        ''' <summary>
        ''' 指定位置の要素を取り出す
        ''' </summary>
        ''' <param name="index">位置 (0オリジン) [0～Count-1の間]</param>
        ''' <returns>取り出した要素 (異常時 Nothing)</returns>
        ''' <remarks>取得要素は、リストから削除される</remarks>
        Public Function GetItem(ByVal index As Integer) As T
            SyncLock m_pLockObject
                Dim bln As Boolean = True
                If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                Try
                    Return Me.Item(index)
                Catch ex As Exception
                    bln = False
                    Return Nothing
                Finally
                    If bln Then m_pAry.RemoveAt(index)
                End Try
            End SyncLock
        End Function

        ''' <summary>
        ''' 指定位置の要素を削除する
        ''' </summary>
        ''' <param name="gnrT">[Generic] 削除する要素</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function Remove(ByVal gnrT As T) As Boolean
            SyncLock m_pLockObject
                m_pAry.Remove(gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' 指定位置の要素を削除する
        ''' </summary>
        ''' <param name="index">削除する位置 (0オリジン) [0～Count-1の間]</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function RemoveAt(ByVal index As Integer) As Boolean
            SyncLock m_pLockObject
                If index >= m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.RemoveAt(index)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' 全要素をクリアする
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Clear()
            SyncLock m_pLockObject
                m_pAry.Clear()
            End SyncLock
        End Sub

        ''' <summary>
        ''' 反復子を取得するインターフェース
        ''' </summary>
        ''' <returns>反復子</returns>
        ''' <remarks>For eachをコールした場合自動的にこのコードから取得される</remarks>
        Private Function GetEnumerator() As System.Collections.IEnumerator Implements System.Collections.IEnumerable.GetEnumerator
            Return m_pAry.GetEnumerator
        End Function

        '********************************************************************
        'インターフェース 関数コール
        '********************************************************************
        ''' <summary>
        ''' 要素から指定キーで検索
        ''' </summary>
        ''' <param name="gnrKey">検索キー</param>
        ''' <param name="findMethod">検索処理を実装したメソッド(findメソッドの実装関数(通常 Implements した クラスから呼ぶ場合 Me でOK))</param>
        ''' <returns>検索結果 無し時:nothing 有り時：該当クラス</returns>
        ''' <remarks>Implements tIface_Find を定義する事</remarks>
        Public Overridable Overloads Function Find(ByVal gnrKey As KEY, ByVal findMethod As tIface.tIface_Find(Of KEY)) As T
            Dim index As Integer

            SyncLock m_pLockObject
                Try
                    '''' 実装メッソド コール
                    index = findMethod.Find(gnrKey, m_pAry)
                    If index > m_pAry.Count OrElse index < 0 Then Return Nothing

                    '''' 結果の配列位置からオブジェクト取り出し
                    Return Me.Item(index)
                Catch ex As Exception
                    Return Nothing
                End Try
            End SyncLock
        End Function

        '''' <summary>
        '''' Findのインターフェース
        '''' </summary>
        '''' <remarks>tClass_Array の呼び元クラスで Implements tIface_Find を定義する事</remarks>
        'Public Interface tIface_Find
        '    ''' <summary>
        '    ''' 要素リストから指定キーで検索を行い、検索結果の配列位置を戻す 処理を実装する
        '    ''' </summary>
        '    ''' <param name="gnrKey">検索キー</param>
        '    ''' <param name="aryList">要素リスト</param>
        '    ''' <returns>検索結果を返す 0オリジンの配列位置 -1:該当無し</returns>
        '    ''' <remarks></remarks>
        '    Function Find(ByVal gnrKey As KEY, ByVal aryList As ArrayList) As Integer

        'End Interface
    End Class
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic非対応]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 同期を行った配列要素の基底クラス
    ''' </summary>
    ''' <remarks></remarks>
    <System.Serializable()> _
    Public Class tClass_Array
        Implements IDisposable                          ' デストラクタ定義
        Implements IEnumerable                          ' For each が出来る定義

        '********************************************************************
        'ローカル変数定義
        '********************************************************************
        Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには
        Protected Friend m_pAry As ArrayList            ' 要素リスト
        Private m_pLockObject As Object                 ' SyncLock同期オブジェクト


        '********************************************************************
        'プロパティ定義
        '********************************************************************
        ''' <summary>
        ''' 指定したインデックスにある要素を取得または設定
        ''' </summary>
        ''' <param name="index">インデックス(0オリジン)</param>
        ''' <value>設定する要素</value>
        ''' <returns>取得した要素</returns>
        ''' <remarks></remarks>
        Default Public Property Item(ByVal index As Integer) As Object
            Get
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                    Return m_pAry.Item(index)
                End SyncLock
            End Get
            Set(ByVal Value As Object)
                SyncLock m_pLockObject
                    If index >= m_pAry.Count OrElse index < 0 Then Return
                    m_pAry.Item(index) = Value
                End SyncLock
            End Set
        End Property

        ''' <summary>
        ''' 要素の数
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property Count() As Integer
            Get
                SyncLock m_pLockObject
                    Return m_pAry.Count
                End SyncLock
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
            m_pAry = New ArrayList
            m_pLockObject = New Object
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                m_pAry.Clear()
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks>必ず キューに溜まっている要素は 開放しておく</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        '********************************************************************
        '[Generic対応]
        '********************************************************************
        ''' <summary>
        ''' 要素をリストの最初に挿入
        ''' </summary>
        ''' <param name="gnrT">[Object] 追加する要素</param>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Sub Prepend(ByVal gnrT As Object)
            SyncLock m_pLockObject
                m_pAry.Insert(0, gnrT)
            End SyncLock
        End Sub

        ''' <summary>
        ''' 要素をリストの最後に追加
        ''' </summary>
        ''' <param name="gnrT">[Object] 追加する要素</param>
        ''' <returns>追加された要素の位置</returns>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Function Add(ByVal gnrT As Object) As Integer
            SyncLock m_pLockObject
                Return m_pAry.Add(gnrT)
            End SyncLock
        End Function

        ''' <summary>
        ''' 要素を指定位置に追加
        ''' </summary>
        ''' <param name="index">位置 (0オリジン) [0～Countの間]</param>
        ''' <param name="gnrT">[Object] 追加する要素</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Function Insert(ByVal index As Integer, ByVal gnrT As Object) As Boolean
            SyncLock m_pLockObject
                If index > m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.Insert(index, gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' 最初の要素を取り出す
        ''' </summary>
        ''' <returns>取り出した要素 (異常時 Nothing)</returns>
        ''' <remarks>取得要素は、リストから削除される</remarks>
        Public Function GetHead() As Object
            Return Me.GetItem(0)
        End Function

        ''' <summary>
        ''' 最後の要素を取り出す
        ''' </summary>
        ''' <returns>取り出した要素 (異常時 Nothing)</returns>
        ''' <remarks>取得要素は、リストから削除される</remarks>
        Public Function GetTail() As Object
            Return Me.GetItem(m_pAry.Count - 1)
        End Function

        ''' <summary>
        ''' 指定位置の要素を取り出す
        ''' </summary>
        ''' <param name="index">位置 (0オリジン) [0～Count-1の間]</param>
        ''' <returns>取り出した要素 (異常時 Nothing)</returns>
        ''' <remarks>取得要素は、リストから削除される</remarks>
        Public Function GetItem(ByVal index As Integer) As Object
            SyncLock m_pLockObject
                Dim bln As Boolean = True
                If index >= m_pAry.Count OrElse index < 0 Then Return Nothing
                Try
                    Return Me.Item(index)
                Catch ex As Exception
                    bln = False
                    Return Nothing
                Finally
                    If bln Then m_pAry.RemoveAt(index)
                End Try
            End SyncLock
        End Function

        ''' <summary>
        ''' 指定位置の要素を削除する
        ''' </summary>
        ''' <param name="gnrT">[Object] 削除する要素</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function Remove(ByVal gnrT As Object) As Boolean
            SyncLock m_pLockObject
                m_pAry.Remove(gnrT)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' 指定位置の要素を削除する
        ''' </summary>
        ''' <param name="index">削除する位置 (0オリジン) [0～Count-1の間]</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function RemoveAt(ByVal index As Integer) As Boolean
            SyncLock m_pLockObject
                If index >= m_pAry.Count OrElse index < 0 Then Return False
                m_pAry.RemoveAt(index)
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' 全要素をクリアする
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Clear()
            SyncLock m_pLockObject
                m_pAry.Clear()
            End SyncLock
        End Sub

        ''' <summary>
        ''' 反復子を取得するインターフェース
        ''' </summary>
        ''' <returns>反復子</returns>
        ''' <remarks>For eachをコールした場合自動的にこのコードから取得される</remarks>
        Private Function GetEnumerator() As System.Collections.IEnumerator Implements System.Collections.IEnumerable.GetEnumerator
            Return m_pAry.GetEnumerator
        End Function

        '********************************************************************
        'インターフェース 関数コール
        '********************************************************************
        ''' <summary>
        ''' 要素から指定キーで検索
        ''' </summary>
        ''' <param name="gnrKey">検索キー</param>
        ''' <param name="findMethod">検索処理を実装したメソッド(findメソッドの実装関数(通常 Implements した クラスから呼ぶ場合 Me でOK))</param>
        ''' <returns>検索結果 無し時:nothing 有り時：該当クラス</returns>
        ''' <remarks>Implements tIface_Find を定義する事</remarks>
        Public Overridable Overloads Function Find(ByVal gnrKey As Object, ByVal findMethod As tIface.tIface_Find) As Object
            Dim index As Integer

            SyncLock m_pLockObject
                Try
                    '''' 実装メッソド コール
                    index = findMethod.Find(gnrKey, m_pAry)
                    If index > m_pAry.Count OrElse index < 0 Then Return Nothing

                    '''' 結果の配列位置からオブジェクト取り出し
                    Return Me.Item(index)
                Catch ex As Exception
                    Return Nothing
                End Try
            End SyncLock
        End Function
    End Class

End Namespace