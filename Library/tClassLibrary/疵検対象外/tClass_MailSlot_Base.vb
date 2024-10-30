'*********************************************************************************
'MailSlotの送受信を行う基底クラス
'[Ver]
' Ver.01    2005/12/18  vs2005 対応
'
'[メモ]
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Threading
Imports System.Runtime.InteropServices

Namespace tClass

    ''' <summary>
    ''' MailSlotの送受信を行う基底クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_MailSlot_Base
        Implements IDisposable                          ' デストラクタ定義

        '********************************************************************
        ' API定数 定義
        '********************************************************************
#Region "API定数定義"
        '' アクセス権限
        Protected Const GENERIC_READ As Integer = &H80000000          ' オブジェクトへの読み取りアクセス
        Protected Const GENERIC_WRITE As Integer = &H40000000         ' オブジェクトへの書き込みアクセス
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
        '' ファイル生成
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

        '' ハンドル破棄
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

        '' ファイル読込み
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

        '' ファイル書込み
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

        '' I/O操作の取り消しを行う (FILE_FLAG_OVERLAPPED 時のみ有効)
        <DllImport("KERNEL32.dll", EntryPoint:="CancelIo", SetLastError:=True)> _
        Protected Shared Function CancelIo( _
                ByVal hFile As IntPtr) _
            As Boolean
        End Function

        '' メールスロット生成
        <DllImport("KERNEL32.DLL", EntryPoint:="CreateMailslotA", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
        Protected Shared Function CreateMailslot( _
          <MarshalAs(UnmanagedType.LPStr)> ByVal lpName As String, _
           ByVal nMaxMessageSize As Integer, _
           ByVal lReadTimeout As Integer, _
           ByRef lpSecurityAttributes As SECURITY_ATTRIBUTES) _
         As IntPtr
        End Function


        '' メールスロット情報取得
        <DllImport("KERNEL32.dll", EntryPoint:="GetMailslotInfo", SetLastError:=True)> _
        Protected Shared Function GetMailslotInfo( _
          ByVal hMailslot As IntPtr, _
          ByRef lpMaxMessageSize As Integer, _
          ByRef lpNextSize As Integer, _
          ByRef lpMessageCount As Integer, _
          ByRef lpReadTimeout As Integer) _
         As Boolean
        End Function

        '' 継承オプション
        <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, pack:=1)> _
        Protected Structure SECURITY_ATTRIBUTES
            Dim nLength As Integer
            Dim lpSecurityDescriptor As IntPtr
            Dim bInheritHandle As Integer
        End Structure
#End Region

        '********************************************************************
        'グローバル定数定義
        '********************************************************************
        Public Const gcMaxMailSize As Integer = 400		' メールスロット最大サイズ
        Public Const gcMyPC As Char = "."c				' 自PC
        Public Const gcUDP As Char = "*"c				' UDP送信

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private disposedValue As Boolean = False		' 重複する呼び出しを検出するには
        Private m_pLockObject As Object					' SyncLock同期オブジェクト (送信時のみ使用)

        'ポインタ
        Protected m_hRecvMailPtr As IntPtr				' 受信メールスロット ハンドル
        ' ''Protected m_hSendMailPtr As IntPtr              ' 送信メールスロット ハンドル

        Protected m_hRecvBufPtr As IntPtr				' 受信メモリ領域アドレス
        Protected m_hSendBufPtr As IntPtr				' 送信メモリ領域アドレス

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '' 送受信データ構造体のサイズ分 メモリ領域確保 (メールスロットの最大値)
            m_hRecvBufPtr = Marshal.AllocHGlobal(gcMaxMailSize)
            m_hSendBufPtr = Marshal.AllocHGlobal(gcMaxMailSize)
            m_pLockObject = New Object
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
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
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks>使用後、必ずコールする事</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' 受信メールスロット生成
        ''' </summary>
        ''' <param name="strMyTaskName">メールスロット名称</param>
        ''' <param name="recvBufLen">受信バッファサイズ  ※もう使わない</param>    
        ''' <param name="sendBufLen">送信バッファサイズ  ※もう使わない</param>            
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function Mail_Open(ByVal strMyTaskName As String, Optional ByVal recvBufLen As Integer = gcMaxMailSize, Optional ByVal sendBufLen As Integer = gcMaxMailSize) As Boolean
            Dim strSrotName As String				' メールスロット名称
            Dim tSA As SECURITY_ATTRIBUTES

            '' オープンチェック
            If Not m_hRecvMailPtr.Equals(IntPtr.Zero) Then Return False

            '' メールスロット用データ生成
            tSA.nLength = System.Runtime.InteropServices.Marshal.SizeOf(tSA)
            tSA.lpSecurityDescriptor = IntPtr.Zero
            tSA.bInheritHandle = 0 'False
            strSrotName = "\\" & gcMyPC & "\MailSlot\" & strMyTaskName


            '' メールスロットオープン 
            m_hRecvMailPtr = CreateMailslot(strSrotName, 0, 0, tSA)
            If m_hRecvMailPtr.ToInt32 <= 0 Then
                m_hRecvMailPtr = IntPtr.Zero
                System.Console.WriteLine(System.Runtime.InteropServices.Marshal.GetLastWin32Error())
                Return False
            End If

            '' 送受信データ構造体のサイズ分 メモリ領域確保 (メールスロットの最大値)
            'm_hRecvBufPtr = Marshal.AllocHGlobal(recvBufLen)
            'm_hSendBufPtr = Marshal.AllocHGlobal(sendBufLen)
            'm_pLockObject = New Object
            Return True
        End Function

        ''' <summary>
        ''' メールスロット終了
        ''' </summary>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function Mail_Close() As Boolean
            '' オープンチェック(受信メールスロットのハンドルは必ず出来ているはずなので)
            If m_hRecvMailPtr.Equals(IntPtr.Zero) Then Return False

            '' 受信ファイル開放
            CancelIo(m_hRecvMailPtr)	   '途中IO操作取り消し
            CloseHandle(m_hRecvMailPtr)
            m_hRecvMailPtr = IntPtr.Zero
            Return True

            ' '' '' 送信ファイル開放
            ' ''If Not m_hSendMailPtr.Equals(IntPtr.Zero) Then
            ' ''    CancelIo(m_hSendMailPtr)       '途中IO操作取り消し
            ' ''    CloseHandle(m_hSendMailPtr)
            ' ''    m_hSendMailPtr = IntPtr.Zero
            ' ''End If

            ' '' 送受信メモリ領域 開放
            'Marshal.FreeHGlobal(m_hRecvBufPtr)
            'Marshal.FreeHGlobal(m_hSendBufPtr)
            'm_hRecvBufPtr = IntPtr.Zero
            'm_hSendBufPtr = IntPtr.Zero
            'm_pLockObject = Nothing
        End Function

        ''' <summary>
        ''' 送信
        ''' </summary>
        ''' <param name="strSendPCName">送信先PC名</param>
        ''' <param name="strSendTaskName">送信先メールスロット名称</param>
        ''' <param name="byteArray">送信データ配列</param>
        ''' <returns>-1:関数エラー 1～:送信サイズ</returns>
        ''' <remarks>毎回ファイルのオープン・クローズを行う</remarks>
        Public Overloads Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal byteArray() As Byte) As Integer
            Dim handle As IntPtr					' 書込みファイルハンドル
            Dim strSrotName As String				' メールスロット名称
            Dim blnRetc As Boolean					' 復帰情報
            Dim getMailSize As Integer				' 実際の書込まれたサイズ

            '' メールスロット名称生成
            strSrotName = "\\" & strSendPCName & "\MailSlot\" & strSendTaskName

            '' ファイル生成
            handle = CreateFile(strSrotName, GENERIC_WRITE, FILE_SHARE_WRITE Or FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
            If handle.ToInt32 <= 0 Then Return -9

            Try
                '' ファイル書込み
                blnRetc = WriteFile(handle, byteArray, byteArray.Length, getMailSize, Nothing)
                If Not blnRetc Then Return -1
                Return getMailSize
            Catch ex As Exception
                Return -7
            Finally
                '' ファイル開放
                CloseHandle(handle)
            End Try
        End Function

        ''' <summary>
        ''' 送信
        ''' </summary>
        ''' <param name="strSendPCName">送信先PC名</param>
        ''' <param name="strSendTaskName">送信先メールスロット名称</param>
        ''' <param name="objVal">送信構造体データ</param>
        ''' <returns>-1:関数エラー 1～:送信サイズ</returns>
        ''' <remarks>毎回ファイルのオープン・クローズを行う</remarks>
        Public Overloads Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal objVal As ValueType) As Integer
            Dim handle As IntPtr					' 書込みファイルハンドル
            Dim strSrotName As String				' メールスロット名称
            Dim blnRetc As Boolean					' 復帰情報
            Dim getMailSize As Integer				' 実際の書込まれたサイズ

            SyncLock m_pLockObject
                '' メールスロット名称生成
                strSrotName = "\\" & strSendPCName & "\MailSlot\" & strSendTaskName

                '' ファイル生成
                handle = CreateFile(strSrotName, GENERIC_WRITE, FILE_SHARE_WRITE Or FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
                If handle.ToInt32 <= 0 Then Return -9

                Try
                    '' メモリに送信データセット (これが難しかった・・・)
                    Marshal.StructureToPtr(objVal, m_hSendBufPtr, True)

                    '' ファイル書込み
                    blnRetc = WriteFile(handle, m_hSendBufPtr, Marshal.SizeOf(objVal), getMailSize, Nothing)
                    If Not blnRetc Then Return -1
                    Return getMailSize
                Catch ex As Exception
                    Return -7
                Finally
                    '' ファイル開放
                    CloseHandle(handle)
                End Try
            End SyncLock
        End Function

        ''' <param name="strSendPCName">送信先PC名</param>
        ''' <param name="strSendTaskName">送信先メールスロット名称</param>
        ''' <param name="objVal">送信インスタンスデータ</param>
        Public Overloads Function Mail_Send(ByVal strSendPCName As String, ByVal strSendTaskName As String, ByVal objVal As Object) As Integer
            '' メモリに送信データセット (これが難しかった・・・)
            Dim memS As New IO.MemoryStream()
            '' バイナリフォーマッタ 作成
            Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
            '' シリアル化
            formatter.Serialize(memS, objVal)
            memS.Position = 0

            '' 送信
            Return Me.Mail_Send(strSendPCName, strSendTaskName, memS.GetBuffer())
        End Function


        '' ''----------------------------------------------------------------
        '' '' 概要：送信 (送信相手が同じ場合)
        '' '' 引数：(in :ValueType) objVal = 送信データ
        '' '' 戻値：(Integer) -1:関数エラー 1～:送信サイズ
        '' '' 備考：必ず blnMail_SendOpen を 呼んでおく事
        '' ''----------------------------------------------------------------
        ' ''Public Overloads Function Mail_Send(ByVal objVal As ValueType) As Integer
        ' ''    Dim blnRetc As Boolean                  '復帰情報
        ' ''    Dim getMailSize As Integer              '実際の書込まれたサイズ

        ' ''    '' オープンチェック
        ' ''    If m_hSendMailPtr.Equals(IntPtr.Zero) Then Return -9

        ' ''    '' メモリに送信データセット (これが難しかった・・・)
        ' ''    Marshal.StructureToPtr(objVal, m_hSendBufPtr, True)

        ' ''    '' ファイル書込み
        ' ''    blnRetc = WriteFile(m_hSendMailPtr, m_hSendBufPtr, Marshal.SizeOf(objVal), getMailSize, Nothing)
        ' ''    If Not blnRetc Then Return -1

        ' ''    Return getMailSize
        ' ''End Function

        '' ''----------------------------------------------------------------
        '' '' 概要：送信 相手と接続
        '' '' 引数：(in :String) strSendPCName = 送信PC名
        '' '' 引数：(in :String) strTaskName = 送信タスク名称
        '' '' 戻値：(Boolean) True:正常 False:異常
        '' '' 備考：
        '' ''----------------------------------------------------------------
        ' ''Public Function Mail_SendOpen(ByVal strSendPCName As String, ByVal strSendTaskName As String) As Boolean
        ' ''    Dim handle As IntPtr
        ' ''    Dim strSrotName As String               'メールスロット名称

        ' ''    ''' オープンチェック
        ' ''    If Not m_hSendMailPtr.Equals(IntPtr.Zero) Then Return False

        ' ''    ''' メールスロット名称生成
        ' ''    strSrotName = "\\" & strSendPCName & "\MailSlot\" & strSendTaskName

        ' ''    ''' ファイル生成
        ' ''    handle = CreateFile(strSrotName, GENERIC_WRITE, FILE_SHARE_WRITE Or FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
        ' ''    If handle.ToInt32 <= 0 Then
        ' ''        m_hSendMailPtr = IntPtr.Zero
        ' ''        Return False
        ' ''    End If
        ' ''    m_hSendMailPtr = handle
        ' ''    Return True
        ' ''End Function

        '' ''----------------------------------------------------------------
        '' '' 概要：送信 相手と切断
        '' '' 引数：( :)  = 
        '' '' 戻値：(Boolean) True:正常 False:異常
        '' '' 備考：
        '' ''----------------------------------------------------------------
        ' ''Public Function Mail_SendClose() As Boolean
        ' ''    ''' オープンチェック
        ' ''    If m_hSendMailPtr.Equals(IntPtr.Zero) Then Return False

        ' ''    ''' ファイル開放
        ' ''    CloseHandle(m_hSendMailPtr)
        ' ''    m_hSendMailPtr = IntPtr.Zero
        ' ''    Return True
        ' ''End Function

        ''' <summary>
        ''' 受信
        ''' </summary>
        ''' <param name="byteArray">受信データ</param>
        ''' <param name="recvSize">受信サイズ</param>
        ''' <returns>-1:関数エラー 0:受信無し 1～:受信サイズ</returns>
        ''' <remarks></remarks>
        Public Function Mail_Recv(ByRef byteArray() As Byte, Optional ByVal recvSize As Integer = gcMaxMailSize) As Integer
            Dim getMailSize As Integer		   '受信サイズ
            Dim blnRetc As Boolean
            Try
                '' ファイル読込み
                blnRetc = ReadFile(m_hRecvMailPtr, byteArray, recvSize, getMailSize, Nothing)
                If Not blnRetc Then Return -1
                If getMailSize = 0 Then Return 0 '受信無し

                '' 戻り値セット
                Return getMailSize
            Catch ex As Exception
                Return -9
            End Try
        End Function

        ''' <summary>
        ''' 受信
        ''' </summary>
        ''' <param name="recvPtr">受信メモリ領域アドレス</param>
        ''' <param name="recvSize">受信サイズ</param>
        ''' <returns>-1:関数エラー 0:受信無し 1～:受信サイズ</returns>
        ''' <remarks></remarks>
        Public Function Mail_Recv(ByRef recvPtr As IntPtr, Optional ByVal recvSize As Integer = gcMaxMailSize) As Integer
            Dim getMailSize As Integer		   '受信サイズ
            Dim blnRetc As Boolean
            Try
                '' ファイル読込み
                blnRetc = ReadFile(m_hRecvMailPtr, m_hRecvBufPtr, recvSize, getMailSize, Nothing)
                If Not blnRetc Then Return -1
                If getMailSize = 0 Then Return 0 '受信無し

                '' 戻り値セット
                recvPtr = m_hRecvBufPtr
                Return getMailSize
            Catch ex As Exception
                Return -9
            End Try
        End Function

        ''' <summary>
        ''' 受信
        ''' </summary>
        ''' <param name="objVal">受信構造体</param>
        ''' <returns>-1:関数エラー 0:受信無し 1～:受信サイズ</returns>
        ''' <remarks></remarks>
        Public Function Mail_Recv(ByRef objVal As ValueType) As Integer
            Dim recvPtr As IntPtr	'受信メモリ領域アドレス
            Dim iRetc As Integer

            '' 受信
            iRetc = Mail_Recv(recvPtr, Marshal.SizeOf(objVal))
            If iRetc > 0 Then		'受信データ有り
                '' 受信データを指定構造体にセット
                objVal = DirectCast(Marshal.PtrToStructure(recvPtr, objVal.GetType), ValueType)
            End If

            Return iRetc
        End Function

        Public Function Mail_Recv(ByRef objVal As Object, Optional ByVal recvSize As Integer = gcMaxMailSize) As Integer
            Dim iRetc As Integer
            Dim byteArray() As Byte = Nothing
            '' 受信
            iRetc = Mail_Recv(byteArray, recvSize)
            If iRetc > 0 Then		'受信データ有り
                '' メモリに送信データセット (これが難しかった・・・)
                Dim memS As New IO.MemoryStream(byteArray)
                '' バイナリフォーマッタ 作成
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                '' シリアル化
                objVal = formatter.Deserialize(memS)
            End If

            Return iRetc
        End Function


    End Class
End Namespace