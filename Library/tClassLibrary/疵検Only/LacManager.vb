'*********************************************************************************
' 高度画像圧縮クラス
'	[Ver]
'		Ver.01    2011/02/20  vs2005 対応
'
'	[メモ]
'		EXEと同一フォルダに LacDLL.dll を入れておくこと
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Explicit On

Imports System.Runtime.InteropServices				'MarshalAsクラス等々

Public Class LacManager

    ''' <summary>
    ''' LAC DLL (x86/x64)x(MERGE/SPLIT) 問題を切り分けるためのインターフェースクラス
    ''' </summary>
    Interface ILacDll
        Function Encode(ByRef src As LacData, ByRef dst As LacData, ByVal nRate As Single) As LacStatus
        Function Decode(ByRef src As LacData, ByRef dst As LacData) As LacStatus
        'Function EncodeGpu(ByVal pInImg As IntPtr, ByVal nInSize As Integer, ByVal pOutImg As IntPtr, ByVal nOutSize As Integer) As Integer    ' GPU版LACは、本ライブラリでは未サポート
        Function DecodeGpu(ByVal pInImg As IntPtr, ByVal nInSize As Integer, ByVal pOutImg As IntPtr) As Integer
    End Interface


    ''' <summary>
    ''' 復帰情報
    ''' </summary>
    Public Enum LacStatus
        LacStatus_Ok = 0						'// 0:正常
        LacStatus_OutOfMemory					'// 1:メモリ不足 ←たぶんPCのメモリ領域っぽい
        LacStatus_ParamError					'// 2:パラメータ指定が異常
        LacStatus_IoFileOpenFail				'// 3:ファイルオープン失敗
        LacStatus_IoFileInvalidFormat			'// 4:オープンは成功したが、ファイル自体が壊れてる(=ヘッダーが変？)
        LacStatus_IoGeneralError				'// 5:その他I/O関係の異常
        LacStatus_InternalError					'// 6:LACコーディック自体の内部異常
        LacStatus_LicenseError                  '// 7:ライセンスエラー

        LacStatus_OtherErr = -1                 '// テックスエンジ側での異常（だいたい、DLLが無い）
        DllNotFound = -2                         '// LACのDLLが無い
    End Enum

    ''' <summary>
    ''' 入出力形式
    ''' </summary>
    Public Enum LacDataType
        FileName = 0                '// ファイル受け渡し時
        MemBlock                    '// メモリ受け渡し時
    End Enum

    ''' <summary>
    ''' LAC DLL  入出力情報
    ''' </summary>
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure LacData
        Dim type As LacDataType                 '// 入出力形式
        Dim ptr As IntPtr                       '// パラメータ (ファイル渡し時：ファイルパス。   メモリ渡し時：メモリポインター(領域は、呼び元で確保)) 
        Dim nbytes As Integer                   '// パラメータがメモリ渡し時のみ有効。 メモリにセットした値
        '											// メモリ入力時(inのみ) : in:入力データのサイズ
        '											// メモリ出力時(in/out) : in:出力用メモリの領域確保したサイズ  out:変換後にLACがセットしたサイズ 
    End Structure

    ''' <summary>
    ''' サポート圧縮形式
    ''' </summary>
    ''' <remark> nCompTypeで指定する値</remark>
    Public Enum EM_COMP_DLLTYPE
        COMP_DLLTYPE_UNKNOWN = -1                                              '不明
        COMP_DLLTYPE_LAC = 0                                                   ' LAC(split)
        COMP_DLLTYPE_LAC_OLD = 1                                               ' LAC(old)
        COMP_DLLTYPE_NUC = 2                                                   ' NUC 
        COMP_DLLTYPE_LAC3 = 3                                                  ' LAC 3
        COMP_DLLTYPE_LAC4 = 4                                                  ' LAC 4
        COMP_DLLTYPE_LAC5 = 5                                                  ' LAC 5
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 32bit環境 64bit環境 実行PCアクティブ対応
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ポインタサイズでどちらの実行環境化判定
    Private Shared ReadOnly Is64Bit As Boolean = IntPtr.Size = 8

    Private m_nCompType As Integer = 0                      ' DLLタイプ (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)

    Private m_LacDll As ILacDll                             'LAC DLL

    Public Sub New()
        m_nCompType = -1
    End Sub

    ''' <summary>
    ''' 適切なDLLクラスを作成する
    ''' </summary>
    ''' <remarks></remarks>
    Public Function LoadDll(nCompType As Integer) As ILacDll
        m_LacDll = New LacDynamicDll(nCompType)
        Return m_LacDll
    End Function

    ''' <summary>
    ''' NUC/LACのDLLを切り替えて返す。
    ''' </summary>
    ''' <param name="nCompType">圧縮タイプ  (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</param>
    Private Function SelectDll(Optional ByVal nCompType As Integer = -1) As ILacDll
        If nCompType = EM_COMP_DLLTYPE.COMP_DLLTYPE_UNKNOWN Then nCompType = m_nCompType

        If nCompType <> m_nCompType Then
            m_LacDll = New LacDynamicDll(nCompType)
            m_nCompType = nCompType
        End If

        Return m_LacDll
    End Function

    ''' <summary>
    ''' 圧縮のDLLタイプ
    ''' </summary>
    ''' <value>DLLタイプ  (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</value>
    Public Property CompType() As Integer
        Get
            Return m_nCompType
        End Get
        Set(ByVal value As Integer)
            SelectDll(value)
        End Set
    End Property

    Private Function FileNameToByteArray(ByVal s As String) As Byte()
        Return System.Text.Encoding.Default.GetBytes(s)
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 圧縮
    '/ /////////////////////////////////////////////////////////////////////////////////
    Shared Function doLacEncode(ByVal dll As ILacDll, ByVal stype As LacDataType, ByVal sdata() As Byte, ByVal dtype As LacDataType, ByVal ddata() As Byte, ByRef out_length As Integer, ByVal nRate As Single) As LacStatus
        If dll Is Nothing Then Return LacStatus.DllNotFound
        Dim src As New LacData
        Dim dst As New LacData

        src.type = stype
        src.nbytes = IIf(stype = LacDataType.FileName, 0, sdata.Length)

        dst.type = dtype
        dst.nbytes = IIf(dtype = LacDataType.FileName, 0, ddata.Length)

        Dim r As LacStatus = LacStatus.LacStatus_InternalError

        Using src_pin As New PinnedPtr(sdata)
            Using dst_ping As New PinnedPtr(ddata)
                src.ptr = src_pin.Ptr
                dst.ptr = dst_ping.Ptr
                r = dll.Encode(src, dst, nRate)
                out_length = dst.nbytes
                Return r
            End Using
        End Using
    End Function

    ''' <summary>
    ''' BMP→LAC (mem→mem)
    ''' </summary>
    ''' <param name="nRate">圧縮率:1[高画質]～   品質:-1～-100[高画質]</param>
    ''' <param name="nInSize">入力データサイズ</param>
    ''' <param name="pInImg">画像データ</param>
    ''' <param name="nMemSize">呼び出し元で確保した領域サイズ</param>
    ''' <param name="pOutImg">LACデータ領域 (呼び出し元で領域確保)</param>
    ''' <param name="nOutSize">圧縮後のサイズ</param>
    ''' <returns></returns>
    Public Function LacEncode(ByVal nRate As Integer, ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer) As LacStatus
        Return doLacEncode(SelectDll, LacDataType.MemBlock, pInImg, LacDataType.MemBlock, pOutImg, nOutSize, nRate)
    End Function

    ''' <summary>
    ''' BMP→LAC (mem→file) 
    ''' </summary>
    Public Function LacEncode(ByVal nRate As Integer, ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal sOutPath As String) As LacStatus
        Dim nOutSize As Integer
        Return doLacEncode(SelectDll, LacDataType.MemBlock, pInImg, LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize, nRate)
    End Function

    ''' <summary>
    ''' BMP→LAC (file→mem)
    ''' </summary>
    Public Function LacEncode(ByVal nRate As Integer, ByVal sInPath As String, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer) As LacStatus
        Return doLacEncode(SelectDll, LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.MemBlock, pOutImg, nOutSize, nRate)
    End Function

    ''' <summary>
    ''' BMP→LAC (file→file)
    ''' </summary>
    Public Function LacEncode(ByVal nRate As Integer, ByVal sInPath As String, ByVal sOutPath As String) As LacStatus
        Dim nOutSize As Integer
        Return doLacEncode(SelectDll, LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize, nRate)
    End Function





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 展開
    '/ /////////////////////////////////////////////////////////////////////////////////

    Shared Function doLacDecode(ByVal dll As ILacDll, ByVal stype As LacDataType, ByVal sdata() As Byte, ByVal dtype As LacDataType, ByVal ddata() As Byte, ByRef out_length As Integer) As LacStatus
        If dll Is Nothing Then Return LacStatus.DllNotFound
        Dim src As New LacData
        Dim dst As New LacData

        src.type = stype
        src.nbytes = IIf(stype = LacDataType.FileName, 0, sdata.Length)

        dst.type = dtype
        dst.nbytes = IIf(dtype = LacDataType.FileName, 0, ddata.Length)

        Dim r As LacStatus = LacStatus.LacStatus_InternalError

        Using src_pin As New PinnedPtr(sdata)
            Using dst_ping As New PinnedPtr(ddata)
                src.ptr = src_pin.Ptr
                dst.ptr = dst_ping.Ptr
                r = dll.Decode(src, dst)
                out_length = dst.nbytes
                Return r
            End Using
        End Using
    End Function

    ''' <summary>
    ''' GPU版LAC(LAC5) デコード
    ''' </summary>
    ''' <param name="dll">LAC DLL I/F</param>
    ''' <param name="stype">入力形式</param>
    ''' <param name="sdata">入力画像データ</param>
    ''' <param name="dtype">出力形式</param>
    ''' <param name="ddata">出力画像データ</param>
    ''' <param name="out_length">出力画像データサイズ</param>
    ''' <returns></returns>
    Shared Function doLacDecodeGpu(ByVal dll As ILacDll, ByVal stype As LacDataType, ByVal sdata() As Byte, ByVal dtype As LacDataType, ByVal ddata() As Byte, ByRef out_length As Integer) As Integer
        If dll Is Nothing Then Return LacStatus.DllNotFound
        Dim pInImg As IntPtr
        Dim nInSize As Integer
        Dim pOutImg As IntPtr

        nInSize = sdata.Length

        Dim r As Integer = LacStatus.LacStatus_InternalError

        Using src_pin As New PinnedPtr(sdata)
            Using dst_ping As New PinnedPtr(ddata)
                pInImg = src_pin.Ptr
                pOutImg = dst_ping.Ptr
                r = dll.DecodeGpu(pInImg, nInSize, pOutImg)
                out_length = ddata.Length
                Return r
            End Using
        End Using
    End Function

    ''' <summary>
    ''' LAC→BMP (mem→mem)
    ''' </summary>
    ''' <param name="nInSize">入力データサイズ</param>
    ''' <param name="pInImg">画像データ</param>
    ''' <param name="nMemSize">呼び出し元で確保した領域サイズ</param>
    ''' <param name="pOutImg">BMPデータ領域 (呼び出し元で領域確保)</param>
    ''' <param name="nOutSize">解凍後のサイズ (in/out)</param>
    ''' <param name="nCompType">元データの圧縮タイプ (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</param>
    ''' <returns></returns>
    Public Function LacDecode(ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Return doLacDecode(SelectDll(nCompType), LacDataType.MemBlock, pInImg, LacDataType.MemBlock, pOutImg, nOutSize)
    End Function




    ''' <summary>
    ''' LAC→BMP (mem→file)
    ''' </summary>
    Public Function LacDecode(ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal sOutPath As String, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Dim nOutSize As Integer
        Return doLacDecode(SelectDll(nCompType), LacDataType.MemBlock, pInImg, LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize)
    End Function



    ''' <summary>
    ''' LAC→BMP (file→mem)
    ''' </summary>
    Public Function LacDecode(ByVal sInPath As String, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Return doLacDecode(SelectDll(nCompType), LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.MemBlock, pOutImg, nOutSize)
    End Function



    ''' <summary>
    ''' LAC→BMP (file→file)
    ''' </summary>
    Public Function LacDecode(ByVal sInPath As String, ByVal sOutPath As String, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Dim nOutSize As Integer
        Return doLacDecode(SelectDll(nCompType), LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize)
    End Function


    ''' <summary>
    ''' LAC→BMP (mem→mem) ※GPU版
    ''' </summary>
    ''' <param name="nInSize">入力データサイズ</param>
    ''' <param name="pInImg">画像データ</param>
    ''' <param name="nMemSize">呼び出し元で確保した領域サイズ</param>
    ''' <param name="pOutImg">BMPデータ領域 (呼び出し元で領域確保)</param>
    ''' <param name="nOutSize">解凍後のサイズ (in/out)</param>
    ''' <param name="nCompType">元データの圧縮タイプ (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</param>
    ''' <returns></returns>
    Public Function LacDecodeGpu(ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC5) As Integer
        Return doLacDecodeGpu(SelectDll(nCompType), LacDataType.MemBlock, pInImg, LacDataType.MemBlock, pOutImg, nOutSize)
    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DLLクラスの定義
    '/ /////////////////////////////////////////////////////////////////////////////////

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ LAC DLLインポート

    ''' <summary>
    ''' LoadLibrary, GetProcAddressにより、適切なLacDLLを呼び出すクラス
    ''' </summary>
    ''' <remarks></remarks>
    Class LacDynamicDll
        Implements ILacDll

        ''DLL側のDelegate＝関数ポインタ「型」の定義
        <UnmanagedFunctionPointer(CallingConvention.Cdecl)> _
        Delegate Function typLacEncodeI( _
         <MarshalAs(UnmanagedType.Struct)> ByRef src As LacData, _
         <MarshalAs(UnmanagedType.Struct)> ByRef dst As LacData, _
         ByVal nRate As Integer) _
          As LacStatus

        <UnmanagedFunctionPointer(CallingConvention.Cdecl)> _
        Delegate Function typLacEncodeF( _
         <MarshalAs(UnmanagedType.Struct)> ByRef src As LacData, _
         <MarshalAs(UnmanagedType.Struct)> ByRef dst As LacData, _
         ByVal nRate As Single) _
          As LacStatus

        <UnmanagedFunctionPointer(CallingConvention.Cdecl)> _
        Delegate Function typLacDecode( _
         <MarshalAs(UnmanagedType.Struct)> ByRef src As LacData, _
         <MarshalAs(UnmanagedType.Struct)> ByRef dst As LacData) _
          As LacStatus

        <UnmanagedFunctionPointer(CallingConvention.Cdecl)> _
        Delegate Function typLacDecodeGpu( _
         ByVal src As IntPtr, _
         ByVal size As Integer, _
         ByVal dst As IntPtr) _
          As Integer

        Structure LacDllType
            ''' <summary>DLL名</summary>
            Public DllName As String
            ''' <summary>展開関数名</summary>
            Dim sDecodeFunction As String
            ''' <summary>圧縮関数名</summary>
            Dim sEncodeFunction As String
            ''' <summary>圧縮率がfloat？</summary>
            Dim bCompressAsSingle As Boolean
            ''' <summary>DLLは圧縮と展開で別？</summary>
            Dim SeparateDll As Boolean

            Public Sub New(ByVal s As String, ByVal dec As String, ByVal enc As String, ByVal bsingle As Boolean, ByVal sep As Boolean)
                DllName = s
                sDecodeFunction = dec
                sEncodeFunction = enc
                bCompressAsSingle = bsingle
                SeparateDll = sep
            End Sub
        End Structure

        ''' <summary>
        ''' LAC DLL 各種バージョンの特性が記してある
        ''' </summary>
        Public Shared ReadOnly LacDllTypes() As LacDllType
        Shared Sub New()
            Dim dlls As New List(Of LacDllType)

            '新Lac DLL(分離型。圧縮率はfloat), DLL名は、LacDECx86.dll/LacEncx64.dllとか。
            dlls.Add(New LacDllType("LAC", "LacDecode", "LacEncode", True, True))
            '旧Lac DLL(合体型。圧縮率はint). LacDLLx86.dll
            dlls.Add(New LacDllType("LAC", "LacDecode", "LacEncode", False, False))
            'NUC (LAC2)
            dlls.Add(New LacDllType("LAC2", "Lac2Decode", "Lac2Encode", False, False))
            'LAC3 
            dlls.Add(New LacDllType("LAC3", "Lac3Decode", "Lac3Encode", False, False))
            'LAC4 (分離型)
            dlls.Add(New LacDllType("LAC4", "Lac4Decode", "Lac4Encode", True, True))
            'LAC5 (分離型)
            dlls.Add(New LacDllType("LAC5", "cpulac5_decoder_decode", "gpulac5_encoder_encode", False, True))

            LacDllTypes = dlls.ToArray
        End Sub



        ''DLLハンドル
        Private m_hDllEncode As New NativeDLL
        Private m_hDllDecode As New NativeDLL

        ''Delegate＝関数ポインタ
        Private mf_LacEncodeI As typLacEncodeI
        Private mf_LacEncodeF As typLacEncodeF
        Private mf_LacDecode As typLacDecode
        Private mf_LacDecodeGpu As typLacDecodeGpu

        ''' <summary>
        ''' BMP→LAC (圧縮基本)
        ''' </summary>
        ''' <param name="src">入力情報</param>
        ''' <param name="dst">出力情報</param>
        ''' <param name="nRate">圧縮率:1[高画質]～   品質:-1～-100[高画質]</param>
        Function LacEncode(ByRef src As LacData, ByRef dst As LacData, ByVal nRate As Single) As LacStatus Implements ILacDll.Encode
            'float/int版があるので、有効なほうを呼ぶ。
            If mf_LacEncodeF IsNot Nothing Then
                Return mf_LacEncodeF(src, dst, nRate)

            ElseIf mf_LacEncodeI IsNot Nothing Then
                Return mf_LacEncodeI(src, dst, CInt(nRate))

            Else
                Return LacStatus.DllNotFound
            End If
        End Function

        ''' <summary>
        ''' LAC→BMP (解凍基本)
        ''' </summary>
        ''' <param name="src">入力情報</param>
        ''' <param name="dst">出力情報</param>
        Function LacDecode(ByRef src As LacData, ByRef dst As LacData) As LacStatus Implements ILacDll.Decode
            'インスタンスが別なだけで、まる投げ
            If mf_LacDecode IsNot Nothing Then
                Return mf_LacDecode(src, dst)
            Else
                Return LacStatus.DllNotFound
            End If
        End Function

        ''' <summary>
        ''' LAC→BMP (解凍基本)
        ''' </summary>
        ''' <param name="src">入力情報</param>
        ''' <param name="nSize">圧縮画像サイズ</param>
        ''' <param name="dst">出力情報</param>
        Function LacDecodeGpu(ByVal src As IntPtr, ByVal nSize As Integer, ByVal dst As IntPtr) As Integer Implements ILacDll.DecodeGpu
            'インスタンスが別なだけで、まる投げ
            If mf_LacDecodeGpu IsNot Nothing Then
                Return mf_LacDecodeGpu(src, nSize, dst)
            Else
                Return LacStatus.DllNotFound
            End If
        End Function



        Public Sub New(ByVal nCompType As Integer)
            '0/1はLACv1の新旧なので、新を優先して読む。
            If nCompType = 0 Or nCompType = 1 Then
                If Not load(0) Then
                    load(1)
                End If
            Else
                load(nCompType)
            End If
        End Sub

        ''' <summary>
        ''' LAC DLLを読み込む
        ''' </summary>
        Function load(ByVal nCompType As Integer) As Boolean
            'マシンアーキテクチャ。
            Dim sArch As String
            If LacManager.Is64Bit Then
                sArch = "x64"
            Else
                sArch = "x86"
            End If

            If nCompType < 0 Or nCompType >= LacDllTypes.Length Then Return False

            Dim load_type As LacDllType = LacDllTypes(nCompType)
            If load_type.SeparateDll Then
                '分離型なので別々のDLLを読む
                m_hDllEncode.Load(load_type.DllName & "ENC" & sArch)
                m_hDllDecode.Load(load_type.DllName & "DEC" & sArch)
            Else
                '同一型なので、どちらも同じDLL。
                m_hDllEncode.Load(load_type.DllName & "DLL" & sArch)
                m_hDllDecode.Load(load_type.DllName & "DLL" & sArch)
            End If

            '圧縮・展開関数のアドレスを取る
            If m_hDllEncode.IsLoad Or m_hDllDecode.IsLoad Then

                If EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC5 = nCompType Then
                    mf_LacDecodeGpu = m_hDllDecode.GetDelegateAddress(Of typLacDecodeGpu)(load_type.sDecodeFunction)
                Else
                    mf_LacDecode = m_hDllDecode.GetDelegateAddress(Of typLacDecode)(load_type.sDecodeFunction)
                End If

                '圧縮は、圧縮率がfloat/intの区別がある
                If load_type.bCompressAsSingle Then
                    mf_LacEncodeF = m_hDllEncode.GetDelegateAddress(Of typLacEncodeF)(load_type.sEncodeFunction)
                Else
                    mf_LacEncodeI = m_hDllEncode.GetDelegateAddress(Of typLacEncodeI)(load_type.sEncodeFunction)
                End If

                'とりあえず、どちらかが読めていればOKとするほか無い。
                Return mf_LacDecode IsNot Nothing Or mf_LacDecodeGpu IsNot Nothing Or mf_LacEncodeF IsNot Nothing Or mf_LacEncodeI IsNot Nothing
            End If

            Return False
        End Function

    End Class
End Class
