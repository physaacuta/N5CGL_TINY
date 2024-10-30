Option Explicit On
Option Strict On

Imports System.Drawing
Imports System.Runtime.InteropServices
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin



''' <summary>
''' 生画像デコーダ（ヘッダなし）
''' </summary>
Public Class tClass_RpiDecoder_Raw
    Implements IRPIDecoder

    Shared ReadOnly sCompType As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_RAW)
    Public Function IsCompressionSupported(ByVal compresstype As String) As Boolean Implements IRPIDecoder.IsCompressionSupported
        Return compresstype = sCompType
    End Function

    Public Function Decompress(<Out()> ByRef bmp As Bitmap, ByVal compressData() As Byte, ByVal header As tClass_ReadRPI) As Boolean Implements IRPIDecoder.Decompress
        bmp = tClass_BmpJpeg.DataToBmp(header.GetRpiHeader.width, header.GetRpiHeader.height, compressData)
        Return True
    End Function
End Class

''' <summary>
''' 生BMPデコーダ（BMPヘッダあり。圧縮なし）
''' </summary>
Public Class tClass_RpiDecoder_BMP
    Implements IRPIDecoder

    Shared ReadOnly sCompType As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_BMP)
    Public Function IsCompressionSupported(ByVal compresstype As String) As Boolean Implements IRPIDecoder.IsCompressionSupported
        Return compresstype = sCompType
    End Function

    Public Function Decompress(<Out()> ByRef bmp As Bitmap, ByVal compressData() As Byte, ByVal header As tClass_ReadRPI) As Boolean Implements IRPIDecoder.Decompress
        bmp = tClass_BmpJpeg.BMPDataToBitmap(compressData)
        Return True
    End Function
End Class

''' <summary>
''' JPEG画像デコーダ
''' </summary>
Public Class tClass_RpiDecoder_Jpeg
    Implements IRPIDecoder

    Shared ReadOnly sCompType As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_JPEG)
    Public Function IsCompressionSupported(ByVal compresstype As String) As Boolean Implements IRPIDecoder.IsCompressionSupported
        Return compresstype = sCompType
    End Function

    Public Function Decompress(<Out()> ByRef bmp As Bitmap, ByVal compressData() As Byte, ByVal header As tClass_ReadRPI) As Boolean Implements IRPIDecoder.Decompress
        bmp = New Bitmap(New IO.MemoryStream(compressData, 0, compressData.Length))
        Return True
    End Function

End Class


''' <summary>
''' LAC類デコーダ(LAC, NUC, LAC3, LAC4, LAC5)
''' </summary>
Public Class tClass_RpiDecoder_LAC
    Implements IRPIDecoder, IDisposable

    Private mcls_LacMgr As New LacManager                                   ' LACコーディック操作クラス (圧縮・解凍クラス)

    Shared ReadOnly sCompType1 As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC)
    Shared ReadOnly sCompType2 As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_NUC)
    Shared ReadOnly sCompType3 As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC3)
    Shared ReadOnly sCompType4 As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC4)
    Shared ReadOnly sCompType5 As String = tClass_ReadRPI.GetString(tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC5)

    Public Function IsCompressionSupported(ByVal compresstype As String) As Boolean Implements IRPIDecoder.IsCompressionSupported
        Return sCompType1 = compresstype Or sCompType2 = compresstype Or sCompType3 = compresstype Or sCompType4 = compresstype Or sCompType5 = compresstype
    End Function


    Function GetLacCodecMode(ByVal emComp As tClass_ReadRPI.EM_COMP_TYPE) As Integer
        Select Case emComp
            Case tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC
                Return 0
            Case tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_NUC
                Return 2
            Case tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC3
                Return 3
            Case tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC4
                Return 4
            Case tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC5
                Return 5
            Case Else
                Return -1
        End Select
    End Function


    Shared s_nParallelThread As Integer = 0
    Shared s_nRequiredMember As Integer
    Shared s_memory As VirtualAllocMemory
    Shared s_waiter As New Threading.AutoResetEvent(False)
    Shared s_lock As New Threading.Mutex()
    Const nRetryAllocate As Integer = 10

    Function TakeFromShared(nSize As Integer) As VirtualAllocMemory
        If s_memory Is Nothing Then Return Nothing

        If nSize = s_memory.Size Then
            Dim vm As VirtualAllocMemory = s_memory
            s_memory = Nothing
            Return vm
        Else
            s_memory.Dispose()
        End If

        s_memory = Nothing
        Return Nothing

    End Function


    Function EnsureAllocate(nSize As Integer) As VirtualAllocMemory
        'まず確保してみる
        Dim vm As New VirtualAllocMemory(nSize)
        If vm.Valid Then Return vm

        '待ったら貰えるのか？
        s_lock.WaitOne()
        If s_nParallelThread > 1 Then
            s_nRequiredMember += 1

            Do Until vm.Valid
                While s_memory Is Nothing

                    Threading.WaitHandle.SignalAndWait(s_lock, s_waiter)
                    s_lock.WaitOne()
                End While
                vm = TakeFromShared(nSize)
            Loop

            s_nRequiredMember -= 1
        End If
        s_lock.ReleaseMutex()

        If vm.Valid Then Return vm


        'だめだった。ほかのスレッドがいるようなら、そのスレッドが終わると、
        'その分のメモリが空くはず。
        'For ii As Integer = 0 To nRetryAllocate
        '    System.Threading.Thread.Sleep(100)

        '    Dim nthread As Integer = s_nParallelThread
        '    vm = New VirtualAllocMemory(nSize)
        '    If vm.Valid Then
        '        Return vm
        '    End If

        '    If nthread = 1 Then Exit For

        '    For jj As Integer = 0 To 100
        '        If s_nParallelThread < nthread Then
        '            System.Threading.Thread.Sleep(10)
        '        End If
        '    Next
        'Next

        Dim msg As String = String.Format("展開用のメモリ不足です: {0}, {1}", Marshal.GetLastWin32Error(), s_nParallelThread)
        Throw New System.OutOfMemoryException(msg)
    End Function


    Public Function Decompress(<Out()> ByRef bmp As Bitmap, ByVal compressData() As Byte, ByVal header As tClass_ReadRPI) As Boolean Implements IRPIDecoder.Decompress

        Dim h As tClass_ReadRPI.RPI_HEADER = header.GetRpiHeader
        Dim nLacCodecMode As Integer = GetLacCodecMode(h.emCompType)
        If nLacCodecMode = -1 Then Return False

        Dim nSize As Integer = 4096 + h.width * h.height * h.nchannel * h.pixel_nbit \ 8
        '        ReDim m_bLacRawWk(nSize - 1)

        Dim lacdll As LacManager.ILacDll = mcls_LacMgr.LoadDll(nLacCodecMode)

        System.Threading.Interlocked.Increment(s_nParallelThread)
        Try
            Using memLacDecode As VirtualAllocMemory = EnsureAllocate(nSize)
                Dim emRetc As LacManager.LacStatus
                Dim nRetc As Integer
                Using src_pin As New PinnedPtr(compressData)
                    Dim src As New LacManager.LacData
                    Dim dst As New LacManager.LacData

                    src.type = LacManager.LacDataType.MemBlock
                    src.ptr = src_pin.Ptr
                    src.nbytes = compressData.Length

                    dst.type = LacManager.LacDataType.MemBlock
                    dst.ptr = memLacDecode.Ptr
                    dst.nbytes = CInt(memLacDecode.Size)

                    If tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC5 = h.emCompType Then
                        ' LAC5
                        nRetc = lacdll.DecodeGpu(src.ptr, src.nbytes, dst.ptr)
                        emRetc = CType(nRetc, LacManager.LacStatus)
                    Else
                        ' LAC5以外
                        emRetc = lacdll.Decode(src, dst)
                    End If
                End Using

                If emRetc <> LacManager.LacStatus.LacStatus_Ok Then
                    Throw New InvalidOperationException("LAC展開失敗:" & emRetc.ToString)
                End If
                Try
                    If tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC5 = h.emCompType Then
                        ' LAC5
                        bmp = tClass_BmpJpeg.DataToBmp(h.width, h.height, memLacDecode.Ptr)
                    Else
                        ' LAC5以外
                        bmp = tClass_BmpJpeg.BMPDataToBitmap(memLacDecode.Ptr)
                    End If
                Catch e As ArgumentException
                    Throw New OutOfMemoryException("Bitmap用のメモリ不足です。")
                Finally
                    s_lock.WaitOne()
                    If s_nRequiredMember > 0 And s_memory Is Nothing Then
                        s_memory = memLacDecode.MoveTo
                        s_waiter.Set()
                    End If
                    s_lock.ReleaseMutex()
                End Try
            End Using
        Finally
            System.Threading.Interlocked.Decrement(s_nParallelThread)
        End Try

        Return True
    End Function

    Sub Dispose() Implements IDisposable.Dispose
        mcls_LacMgr = Nothing
    End Sub

End Class
