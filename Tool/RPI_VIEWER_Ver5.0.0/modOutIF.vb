Imports System.Runtime.InteropServices


''' <summary>
''' �O���pI/F��`
''' </summary>
Public Module modOutIF

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �N������

    ''' <summary>
    ''' �N������
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure START_ARGS
        Dim sTaskName As String			' ���[���X���b�g�������B���̂Ƃ��ɂ�ini�t�@�C�������D�悳��� (TaskName:=)
        Dim nId As Integer				' RPI_SEQ�̉��Ԗ�(0�I���W��) (ID:=)

        Dim sArgs() As String			' ���������H
    End Structure

    Public Const START_CMD_TASKNAME As String = "TaskName:="
    Public Const START_CMD_ID As String = "ID:="



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[���X���b�g��M�p
    Public Const SIZE_MSL As Integer = 256						'// send_mail, recv_mail�T�C�Y
    Public Const SIZE_MSL_DATA As Integer = SIZE_MSL - 8		'// nEventNo�ȍ~�̃f�[�^��


    ''' <summary>
    ''' MIX�^�W���`���Ɠ���
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_MSL, Pack:=1)> _
     Public Structure COMMON_QUE_STRMIX
        Dim nEventNo As Integer
        Dim nLineNo As Integer

        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim idata() As Integer

        '// ��ByValTStr ���ꂾ�� �I�[�ɏ����NULL���t�^�����ׁA�I�[1�o�C�g���폜�����B
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata1 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata2 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata3 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata4 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata5 As String

        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=(SIZE_MSL_DATA - 4 * 20 - 32 * 5))> _
        Dim ydata As String

        Public Sub inisialize()
            ReDim idata(20 - 1)
        End Sub

    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ RPI_VIEWER �O���w���p

    '===================================================================
    ' ���ʗp

    '----------------------------------------------------
    ' ��ʏI���w��
    Public Const FACT_RPIV_END As Integer = 0		' ��ʏI���w��

    '----------------------------------------------------
    ' RPI�t�@�C���\��
    ' MIX_S[0] : RPI�t�H���_�p�X (�������Ƀt���p�X�����Ă�OK)
    ' MIX_S[1] : RPI�t�@�C����1
    ' MIX_S[2] : RPI�t�@�C����2	(��S[2]�Ƀf�[�^���Z�b�g����Ă���ꍇ�́A�����I�ɂ͒P����S[0]�`[2]��A�����邾���Ƃ���)

    ' MIX_I[0] : �����`�����l���� (0�I���W��)
    ' MIX_I[1] : Y���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
    ' MIX_I[2] : Y����[mm]
    ' MIX_I[3] : X���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
    ' MIX_I[4] : X����[mm] (-1:�ړ������Ȃ�)
    ' MIX_I[5] : X����[mm]���ǂ����̎w�肩��H(0:���[  1:�Z���^�[����)

    ' �ȉ��R���ڂ̈��� (0:���� -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 1:���摜�ɑ΂��Ĕ��] 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��])
    ' MIX_I[6] : ���E���]
    ' MIX_I[7] : �㉺���]
    ' MIX_I[8] : 90�x���]
    Public Const FACT_RPIV_DSPING As Integer = 1		' RPI�t�@�C���\��

    '----------------------------------------------------
    ' �t�H�[����I����Ԃɂ���
    '	�� �����A���̏ꍇ�A�w��ʒu�W�����v����A�N�e�B�u��Ԃ̂Ƃ������Ȃ��΍�
    Public Const FACT_RPIV_ACTIVE As Integer = 2		' RPI�r���[�A�[ �A�N�e�B�u

    '----------------------------------------------------
    ' RPI�t�@�C�������{�\��
    ' MIX_S[0] : �Ώۂ�INI�t�@�C���Z�N�V����
    ' MIX_S[1] : �Ǘ�No

    ' MIX_I[0] : ��(0,1)
    ' MIX_I[1] : �p�x�A�F (0�I���W��)
    Public Const FACT_RPIV_SEARCH_DSPING As Integer = 3		' RPI�t�@�C�������{�\��

    '----------------------------------------------------
    ' �w��ʒu
    ' MIX_I[0] : Y���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
    ' MIX_I[1] : Y����[mm]
    ' MIX_I[2] : X���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
    ' MIX_I[3] : X����[mm] (-1:�ړ������Ȃ�)
    ' MIX_I[4] : X����[mm]���ǂ����̎w�肩��H(0:���[  1:�Z���^�[����)
    Public Const FACT_RPIV_DSP_MOVE As Integer = 10		' RPI�t�@�C���\��


    '----------------------------------------------------
    ' ���]
    ' �ȉ��R���ڂ̈��� (0:���� -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 1:���摜�ɑ΂��Ĕ��] 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��])
    ' MIX_I[0] : ���E���]
    ' MIX_I[1] : �㉺���]
    ' MIX_I[2] : 90�x���]
    Public Const FACT_RPIV_DSP_TURN As Integer = 11		' �\�������ؑ�



    '===================================================================
    ' �A���p (100�`200)

    ' MIX_I[0] : �Ή��r�b�g (���ʂ��珇��bitON�ŁA�Ή����鍀�ڂ𔽉f�B0x01�ł���ΐL�ї�)
    ' MIX_I[1] : �L�ї� [10-6��]
    ' MIX_I[2] : �X�^�[�g�I�t�Z�b�gY [mm]
    ' MIX_I[3] : �X�^�[�g�I�t�Z�b�gX [mm]
    Public Const FACT_RPIS_SETTING As Integer = 100

    '----------------------------------------------------
    ' �\���ʒu�E�T�C�Y
    ' MIX_I[0] : ���j�^�[No
    ' MIX_I[1] : ���j�^�[���|�W�V����X [pixel]
    ' MIX_I[2] : ���j�^�[���|�W�V����Y [pixel]
    ' MIX_I[3] : �\���T�C�Y�w [pixel]
    ' MIX_I[4] : �\���T�C�Y�x [pixel]
    ' MIX_I[5] : �ő�ŏ��敪�@0:�ʏ� 1:�ŏ��� 2:�ő剻
    Public Const FACT_RPIS_DSP_POS As Integer = 101

    '----------------------------------------------------
    ' �A���J�n�^�A����~
    ' MIX_I[0] : 0:�A���J�n  1:�A����~
    Public Const FACT_RPIS_RENDO As Integer = 102

    '----------------------------------------------------
    ' �I�t�Z�b�g�l���f
    Public Const FACT_RPIS_OFFSET As Integer = 103

    '----------------------------------------------------
    ' �q�o�h�r���[�A�N���[�Y
    Public Const FACT_RPIS_CLOSE As Integer = 104

    '----------------------------------------------------
    ' �q�o�h�r���[�A�����\��
    Public Const FACT_RPIV_FIRST_DSPING As Integer = 105


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���L������ (�Ȉ՘A�g)
    Public Const TBL_RPI_SYNC_NAME As String = "TBL_RPI_SYNC"		' �A���e�[�u��

    ''' <summary>
    ''' ���ʃR�C������
    ''' </summary>
    Public Const SIZE_TYPE_SYNC As Integer = 8
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_SYNC, pack:=1)> _
    Public Structure TYPE_SYNC
        Dim nY As Integer						' Y���W[mm]
        Dim nX As Integer						' X���W[mm]

        '�C�j�V�����C�Y
        Public Sub initialize()
        End Sub
    End Structure




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���L������ (���H���ԘA�g)
    Public Const TBL_RPI_TRK_NAME As String = "TBL_RPI_TRK"		' �A���e�[�u��
    Public Const TBL_RPI_REC_MAX As Integer = 5					' �A���e�[�u����

    ''' <summary>
    ''' �w�b�_�[����
    ''' </summary>
    Public Const SIZE_TYPE_HEAD As Integer = 4
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_HEAD, pack:=1)> _
     Public Structure TYPE_TRK_HEAD
        Dim nActiveID As Integer						' ���쒆ID (0�I���W��)

        '�C�j�V�����C�Y
        Public Sub initialize()

        End Sub
    End Structure

    ''' <summary>
    ''' ���ʃR�C������
    ''' </summary>
    Public Const SIZE_TYPE_ITEM As Integer = 8
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_ITEM, pack:=1)> _
     Public Structure TYPE_TRK_ITEM
        Dim nY As Integer								' Y���W[mm]
        Dim nX As Integer								' X���W[mm]



        '�C�j�V�����C�Y
        Public Sub initialize()

        End Sub
    End Structure



    ''' <summary>
    ''' RPI_VIEWER�ԘA���e�[�u��
    ''' </summary>
    Public Const SIZE_TYPE_TRK As Integer = SIZE_TYPE_ITEM * TBL_RPI_REC_MAX + SIZE_TYPE_HEAD

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_TRK, pack:=1)> _
    Public Structure TYPE_TRK
        Dim head As TYPE_TRK_HEAD

        <VBFixedArray(TBL_RPI_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_RPI_REC_MAX)> _
        Dim rec() As TYPE_TRK_ITEM


        '�C�j�V�����C�Y
        Public Sub initialize()
            ReDim rec(TBL_RPI_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
            head.initialize()
        End Sub
    End Structure


End Module
