'*********************************************************************************
' ���x�摜���k�N���X
'	[Ver]
'		Ver.01    2011/02/20  vs2005 �Ή�
'
'	[����]
'		EXE�Ɠ���t�H���_�� LacDLL.dll �����Ă�������
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Option Explicit On

Imports System.Runtime.InteropServices				'MarshalAs�N���X���X

Public Class LacManager

    ''' <summary>
    ''' LAC DLL (x86/x64)x(MERGE/SPLIT) ����؂蕪���邽�߂̃C���^�[�t�F�[�X�N���X
    ''' </summary>
    Interface ILacDll
        Function Encode(ByRef src As LacData, ByRef dst As LacData, ByVal nRate As Single) As LacStatus
        Function Decode(ByRef src As LacData, ByRef dst As LacData) As LacStatus
        'Function EncodeGpu(ByVal pInImg As IntPtr, ByVal nInSize As Integer, ByVal pOutImg As IntPtr, ByVal nOutSize As Integer) As Integer    ' GPU��LAC�́A�{���C�u�����ł͖��T�|�[�g
        Function DecodeGpu(ByVal pInImg As IntPtr, ByVal nInSize As Integer, ByVal pOutImg As IntPtr) As Integer
    End Interface


    ''' <summary>
    ''' ���A���
    ''' </summary>
    Public Enum LacStatus
        LacStatus_Ok = 0						'// 0:����
        LacStatus_OutOfMemory					'// 1:�������s�� �����Ԃ�PC�̃������̈���ۂ�
        LacStatus_ParamError					'// 2:�p�����[�^�w�肪�ُ�
        LacStatus_IoFileOpenFail				'// 3:�t�@�C���I�[�v�����s
        LacStatus_IoFileInvalidFormat			'// 4:�I�[�v���͐����������A�t�@�C�����̂����Ă�(=�w�b�_�[���ρH)
        LacStatus_IoGeneralError				'// 5:���̑�I/O�֌W�ُ̈�
        LacStatus_InternalError					'// 6:LAC�R�[�f�B�b�N���̂̓����ُ�
        LacStatus_LicenseError                  '// 7:���C�Z���X�G���[

        LacStatus_OtherErr = -1                 '// �e�b�N�X�G���W���łُ̈�i���������ADLL�������j
        DllNotFound = -2                         '// LAC��DLL������
    End Enum

    ''' <summary>
    ''' ���o�͌`��
    ''' </summary>
    Public Enum LacDataType
        FileName = 0                '// �t�@�C���󂯓n����
        MemBlock                    '// �������󂯓n����
    End Enum

    ''' <summary>
    ''' LAC DLL  ���o�͏��
    ''' </summary>
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure LacData
        Dim type As LacDataType                 '// ���o�͌`��
        Dim ptr As IntPtr                       '// �p�����[�^ (�t�@�C���n�����F�t�@�C���p�X�B   �������n�����F�������|�C���^�[(�̈�́A�Ăь��Ŋm��)) 
        Dim nbytes As Integer                   '// �p�����[�^���������n�����̂ݗL���B �������ɃZ�b�g�����l
        '											// ���������͎�(in�̂�) : in:���̓f�[�^�̃T�C�Y
        '											// �������o�͎�(in/out) : in:�o�͗p�������̗̈�m�ۂ����T�C�Y  out:�ϊ����LAC���Z�b�g�����T�C�Y 
    End Structure

    ''' <summary>
    ''' �T�|�[�g���k�`��
    ''' </summary>
    ''' <remark> nCompType�Ŏw�肷��l</remark>
    Public Enum EM_COMP_DLLTYPE
        COMP_DLLTYPE_UNKNOWN = -1                                              '�s��
        COMP_DLLTYPE_LAC = 0                                                   ' LAC(split)
        COMP_DLLTYPE_LAC_OLD = 1                                               ' LAC(old)
        COMP_DLLTYPE_NUC = 2                                                   ' NUC 
        COMP_DLLTYPE_LAC3 = 3                                                  ' LAC 3
        COMP_DLLTYPE_LAC4 = 4                                                  ' LAC 4
        COMP_DLLTYPE_LAC5 = 5                                                  ' LAC 5
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 32bit�� 64bit�� ���sPC�A�N�e�B�u�Ή�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �|�C���^�T�C�Y�łǂ���̎��s��������
    Private Shared ReadOnly Is64Bit As Boolean = IntPtr.Size = 8

    Private m_nCompType As Integer = 0                      ' DLL�^�C�v (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)

    Private m_LacDll As ILacDll                             'LAC DLL

    Public Sub New()
        m_nCompType = -1
    End Sub

    ''' <summary>
    ''' �K�؂�DLL�N���X���쐬����
    ''' </summary>
    ''' <remarks></remarks>
    Public Function LoadDll(nCompType As Integer) As ILacDll
        m_LacDll = New LacDynamicDll(nCompType)
        Return m_LacDll
    End Function

    ''' <summary>
    ''' NUC/LAC��DLL��؂�ւ��ĕԂ��B
    ''' </summary>
    ''' <param name="nCompType">���k�^�C�v  (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</param>
    Private Function SelectDll(Optional ByVal nCompType As Integer = -1) As ILacDll
        If nCompType = EM_COMP_DLLTYPE.COMP_DLLTYPE_UNKNOWN Then nCompType = m_nCompType

        If nCompType <> m_nCompType Then
            m_LacDll = New LacDynamicDll(nCompType)
            m_nCompType = nCompType
        End If

        Return m_LacDll
    End Function

    ''' <summary>
    ''' ���k��DLL�^�C�v
    ''' </summary>
    ''' <value>DLL�^�C�v  (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</value>
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
    '/ ���k
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
    ''' BMP��LAC (mem��mem)
    ''' </summary>
    ''' <param name="nRate">���k��:1[���掿]�`   �i��:-1�`-100[���掿]</param>
    ''' <param name="nInSize">���̓f�[�^�T�C�Y</param>
    ''' <param name="pInImg">�摜�f�[�^</param>
    ''' <param name="nMemSize">�Ăяo�����Ŋm�ۂ����̈�T�C�Y</param>
    ''' <param name="pOutImg">LAC�f�[�^�̈� (�Ăяo�����ŗ̈�m��)</param>
    ''' <param name="nOutSize">���k��̃T�C�Y</param>
    ''' <returns></returns>
    Public Function LacEncode(ByVal nRate As Integer, ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer) As LacStatus
        Return doLacEncode(SelectDll, LacDataType.MemBlock, pInImg, LacDataType.MemBlock, pOutImg, nOutSize, nRate)
    End Function

    ''' <summary>
    ''' BMP��LAC (mem��file) 
    ''' </summary>
    Public Function LacEncode(ByVal nRate As Integer, ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal sOutPath As String) As LacStatus
        Dim nOutSize As Integer
        Return doLacEncode(SelectDll, LacDataType.MemBlock, pInImg, LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize, nRate)
    End Function

    ''' <summary>
    ''' BMP��LAC (file��mem)
    ''' </summary>
    Public Function LacEncode(ByVal nRate As Integer, ByVal sInPath As String, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer) As LacStatus
        Return doLacEncode(SelectDll, LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.MemBlock, pOutImg, nOutSize, nRate)
    End Function

    ''' <summary>
    ''' BMP��LAC (file��file)
    ''' </summary>
    Public Function LacEncode(ByVal nRate As Integer, ByVal sInPath As String, ByVal sOutPath As String) As LacStatus
        Dim nOutSize As Integer
        Return doLacEncode(SelectDll, LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize, nRate)
    End Function





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �W�J
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
    ''' GPU��LAC(LAC5) �f�R�[�h
    ''' </summary>
    ''' <param name="dll">LAC DLL I/F</param>
    ''' <param name="stype">���͌`��</param>
    ''' <param name="sdata">���͉摜�f�[�^</param>
    ''' <param name="dtype">�o�͌`��</param>
    ''' <param name="ddata">�o�͉摜�f�[�^</param>
    ''' <param name="out_length">�o�͉摜�f�[�^�T�C�Y</param>
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
    ''' LAC��BMP (mem��mem)
    ''' </summary>
    ''' <param name="nInSize">���̓f�[�^�T�C�Y</param>
    ''' <param name="pInImg">�摜�f�[�^</param>
    ''' <param name="nMemSize">�Ăяo�����Ŋm�ۂ����̈�T�C�Y</param>
    ''' <param name="pOutImg">BMP�f�[�^�̈� (�Ăяo�����ŗ̈�m��)</param>
    ''' <param name="nOutSize">�𓀌�̃T�C�Y (in/out)</param>
    ''' <param name="nCompType">���f�[�^�̈��k�^�C�v (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</param>
    ''' <returns></returns>
    Public Function LacDecode(ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Return doLacDecode(SelectDll(nCompType), LacDataType.MemBlock, pInImg, LacDataType.MemBlock, pOutImg, nOutSize)
    End Function




    ''' <summary>
    ''' LAC��BMP (mem��file)
    ''' </summary>
    Public Function LacDecode(ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal sOutPath As String, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Dim nOutSize As Integer
        Return doLacDecode(SelectDll(nCompType), LacDataType.MemBlock, pInImg, LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize)
    End Function



    ''' <summary>
    ''' LAC��BMP (file��mem)
    ''' </summary>
    Public Function LacDecode(ByVal sInPath As String, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Return doLacDecode(SelectDll(nCompType), LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.MemBlock, pOutImg, nOutSize)
    End Function



    ''' <summary>
    ''' LAC��BMP (file��file)
    ''' </summary>
    Public Function LacDecode(ByVal sInPath As String, ByVal sOutPath As String, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC) As LacStatus
        Dim nOutSize As Integer
        Return doLacDecode(SelectDll(nCompType), LacDataType.FileName, FileNameToByteArray(sInPath), LacDataType.FileName, FileNameToByteArray(sOutPath), nOutSize)
    End Function


    ''' <summary>
    ''' LAC��BMP (mem��mem) ��GPU��
    ''' </summary>
    ''' <param name="nInSize">���̓f�[�^�T�C�Y</param>
    ''' <param name="pInImg">�摜�f�[�^</param>
    ''' <param name="nMemSize">�Ăяo�����Ŋm�ۂ����̈�T�C�Y</param>
    ''' <param name="pOutImg">BMP�f�[�^�̈� (�Ăяo�����ŗ̈�m��)</param>
    ''' <param name="nOutSize">�𓀌�̃T�C�Y (in/out)</param>
    ''' <param name="nCompType">���f�[�^�̈��k�^�C�v (0:lac(split) 1:Lac(old) 2:NUC 3:Lac3 4:lac4 5:Lac5)</param>
    ''' <returns></returns>
    Public Function LacDecodeGpu(ByVal nInSize As Integer, ByVal pInImg() As Byte, ByVal nMemSize As Integer, ByVal pOutImg() As Byte, ByRef nOutSize As Integer, Optional ByVal nCompType As Integer = EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC5) As Integer
        Return doLacDecodeGpu(SelectDll(nCompType), LacDataType.MemBlock, pInImg, LacDataType.MemBlock, pOutImg, nOutSize)
    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DLL�N���X�̒�`
    '/ /////////////////////////////////////////////////////////////////////////////////

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ LAC DLL�C���|�[�g

    ''' <summary>
    ''' LoadLibrary, GetProcAddress�ɂ��A�K�؂�LacDLL���Ăяo���N���X
    ''' </summary>
    ''' <remarks></remarks>
    Class LacDynamicDll
        Implements ILacDll

        ''DLL����Delegate���֐��|�C���^�u�^�v�̒�`
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
            ''' <summary>DLL��</summary>
            Public DllName As String
            ''' <summary>�W�J�֐���</summary>
            Dim sDecodeFunction As String
            ''' <summary>���k�֐���</summary>
            Dim sEncodeFunction As String
            ''' <summary>���k����float�H</summary>
            Dim bCompressAsSingle As Boolean
            ''' <summary>DLL�͈��k�ƓW�J�ŕʁH</summary>
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
        ''' LAC DLL �e��o�[�W�����̓������L���Ă���
        ''' </summary>
        Public Shared ReadOnly LacDllTypes() As LacDllType
        Shared Sub New()
            Dim dlls As New List(Of LacDllType)

            '�VLac DLL(�����^�B���k����float), DLL���́ALacDECx86.dll/LacEncx64.dll�Ƃ��B
            dlls.Add(New LacDllType("LAC", "LacDecode", "LacEncode", True, True))
            '��Lac DLL(���̌^�B���k����int). LacDLLx86.dll
            dlls.Add(New LacDllType("LAC", "LacDecode", "LacEncode", False, False))
            'NUC (LAC2)
            dlls.Add(New LacDllType("LAC2", "Lac2Decode", "Lac2Encode", False, False))
            'LAC3 
            dlls.Add(New LacDllType("LAC3", "Lac3Decode", "Lac3Encode", False, False))
            'LAC4 (�����^)
            dlls.Add(New LacDllType("LAC4", "Lac4Decode", "Lac4Encode", True, True))
            'LAC5 (�����^)
            dlls.Add(New LacDllType("LAC5", "cpulac5_decoder_decode", "gpulac5_encoder_encode", False, True))

            LacDllTypes = dlls.ToArray
        End Sub



        ''DLL�n���h��
        Private m_hDllEncode As New NativeDLL
        Private m_hDllDecode As New NativeDLL

        ''Delegate���֐��|�C���^
        Private mf_LacEncodeI As typLacEncodeI
        Private mf_LacEncodeF As typLacEncodeF
        Private mf_LacDecode As typLacDecode
        Private mf_LacDecodeGpu As typLacDecodeGpu

        ''' <summary>
        ''' BMP��LAC (���k��{)
        ''' </summary>
        ''' <param name="src">���͏��</param>
        ''' <param name="dst">�o�͏��</param>
        ''' <param name="nRate">���k��:1[���掿]�`   �i��:-1�`-100[���掿]</param>
        Function LacEncode(ByRef src As LacData, ByRef dst As LacData, ByVal nRate As Single) As LacStatus Implements ILacDll.Encode
            'float/int�ł�����̂ŁA�L���Ȃق����ĂԁB
            If mf_LacEncodeF IsNot Nothing Then
                Return mf_LacEncodeF(src, dst, nRate)

            ElseIf mf_LacEncodeI IsNot Nothing Then
                Return mf_LacEncodeI(src, dst, CInt(nRate))

            Else
                Return LacStatus.DllNotFound
            End If
        End Function

        ''' <summary>
        ''' LAC��BMP (�𓀊�{)
        ''' </summary>
        ''' <param name="src">���͏��</param>
        ''' <param name="dst">�o�͏��</param>
        Function LacDecode(ByRef src As LacData, ByRef dst As LacData) As LacStatus Implements ILacDll.Decode
            '�C���X�^���X���ʂȂ����ŁA�܂铊��
            If mf_LacDecode IsNot Nothing Then
                Return mf_LacDecode(src, dst)
            Else
                Return LacStatus.DllNotFound
            End If
        End Function

        ''' <summary>
        ''' LAC��BMP (�𓀊�{)
        ''' </summary>
        ''' <param name="src">���͏��</param>
        ''' <param name="nSize">���k�摜�T�C�Y</param>
        ''' <param name="dst">�o�͏��</param>
        Function LacDecodeGpu(ByVal src As IntPtr, ByVal nSize As Integer, ByVal dst As IntPtr) As Integer Implements ILacDll.DecodeGpu
            '�C���X�^���X���ʂȂ����ŁA�܂铊��
            If mf_LacDecodeGpu IsNot Nothing Then
                Return mf_LacDecodeGpu(src, nSize, dst)
            Else
                Return LacStatus.DllNotFound
            End If
        End Function



        Public Sub New(ByVal nCompType As Integer)
            '0/1��LACv1�̐V���Ȃ̂ŁA�V��D�悵�ēǂށB
            If nCompType = 0 Or nCompType = 1 Then
                If Not load(0) Then
                    load(1)
                End If
            Else
                load(nCompType)
            End If
        End Sub

        ''' <summary>
        ''' LAC DLL��ǂݍ���
        ''' </summary>
        Function load(ByVal nCompType As Integer) As Boolean
            '�}�V���A�[�L�e�N�`���B
            Dim sArch As String
            If LacManager.Is64Bit Then
                sArch = "x64"
            Else
                sArch = "x86"
            End If

            If nCompType < 0 Or nCompType >= LacDllTypes.Length Then Return False

            Dim load_type As LacDllType = LacDllTypes(nCompType)
            If load_type.SeparateDll Then
                '�����^�Ȃ̂ŕʁX��DLL��ǂ�
                m_hDllEncode.Load(load_type.DllName & "ENC" & sArch)
                m_hDllDecode.Load(load_type.DllName & "DEC" & sArch)
            Else
                '����^�Ȃ̂ŁA�ǂ��������DLL�B
                m_hDllEncode.Load(load_type.DllName & "DLL" & sArch)
                m_hDllDecode.Load(load_type.DllName & "DLL" & sArch)
            End If

            '���k�E�W�J�֐��̃A�h���X�����
            If m_hDllEncode.IsLoad Or m_hDllDecode.IsLoad Then

                If EM_COMP_DLLTYPE.COMP_DLLTYPE_LAC5 = nCompType Then
                    mf_LacDecodeGpu = m_hDllDecode.GetDelegateAddress(Of typLacDecodeGpu)(load_type.sDecodeFunction)
                Else
                    mf_LacDecode = m_hDllDecode.GetDelegateAddress(Of typLacDecode)(load_type.sDecodeFunction)
                End If

                '���k�́A���k����float/int�̋�ʂ�����
                If load_type.bCompressAsSingle Then
                    mf_LacEncodeF = m_hDllEncode.GetDelegateAddress(Of typLacEncodeF)(load_type.sEncodeFunction)
                Else
                    mf_LacEncodeI = m_hDllEncode.GetDelegateAddress(Of typLacEncodeI)(load_type.sEncodeFunction)
                End If

                '�Ƃ肠�����A�ǂ��炩���ǂ߂Ă����OK�Ƃ���ق������B
                Return mf_LacDecode IsNot Nothing Or mf_LacDecodeGpu IsNot Nothing Or mf_LacEncodeF IsNot Nothing Or mf_LacEncodeI IsNot Nothing
            End If

            Return False
        End Function

    End Class
End Class
