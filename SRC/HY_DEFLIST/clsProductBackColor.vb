'*********************************************************************************
'�@���Y�ԍ��w�i�F�ݒ�N���X
'	[Ver]
'		Ver.01    2008/07/23  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports System.Collections
Imports tClassLibrary


Public Class clsProductBackColor

    Implements IDisposable                          ' �f�X�g���N�^��`


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DSP_KIZU_LST As Integer = 5                        '�\�������r���
    Private Const PAGE_MAX As Integer = 5                            '�ő�y�[�W�ԍ�
    Private Const MAX_PRODUCT_COLOR As Integer = 5                   '���Y�ԍ��w�i�F��
    Private Const M_C_TOP As Integer = 0                             '�\
    Private Const M_C_BOT As Integer = 1                             '��
    Private Const MC_PNOBG_MAX As Integer = 120                      '�w�i�F�w��ԍ����t�ő吔 ��������Â����t��񂩂�J��

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                    ' ���O�Ǘ��N���X
    Private disposedValue As Boolean = False                        ' �d������Ăяo�������o����ɂ�

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_ProNo_BackColor(4) As System.Drawing.Color            '���Y�ԍ��w�i�F
    Private m_dctProductNoBackColor As New Dictionary(Of String, Integer)     '���Y�ԍ��̔w�i�F�Z�b�g
    Private m_lstData As New List(Of String)                        '���t�����r���Ǘ�NO�J���p
    Private m_nPltPoint As Integer = 0                              '���Y�ԍ��w�i�F�̍ŐV�|�C���^
    Private m_NewRecNo As Integer = -1                              '�r���̍ŐV���R�[�h�m�n

    Private mPltNo As Integer


    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClass.tClass_LogManager)

        '���O�N���X
        mcls_Log = clsLog

        '�w�i�F���Œ�Ńe�[�u���Z�b�g
        m_ProNo_BackColor(0) = Color.Cyan
        m_ProNo_BackColor(1) = Color.DodgerBlue
        m_ProNo_BackColor(2) = Color.Yellow
        m_ProNo_BackColor(3) = Color.GreenYellow
        m_ProNo_BackColor(4) = Color.Lime

    End Sub
#Region " IDisposable Support "
    ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
            End If

            '' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
            m_dctProductNoBackColor.Clear()
            m_dctProductNoBackColor = Nothing

            m_lstData = Nothing

        End If
        Me.disposedValue = True
    End Sub

    ''' <summary>
    ''' ��ʋN�������Y�ԍ��w�i�F����U�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadPnoBGinit()

        Dim ii As Integer
        Dim jj As Integer
        Dim n_cnt_buf As Integer
        Dim nRecNo(1) As Integer
        Dim nCnt(1) As Integer
        Dim typDefInf As TBL_HY_DEFLIST                    '�r���ꗗ�e�[�u��

        Try

            '�r���ꗗ�e�[�u���ǂݍ��݁B
            typDefInf = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_DEFLIST_NAME, GetType(TBL_HY_DEFLIST)), TBL_HY_DEFLIST)

            '�ŐV���R�[�h
            nRecNo(M_C_TOP) = typDefInf.rec.head.nPos(M_C_TOP) - 1
            nRecNo(M_C_BOT) = typDefInf.rec.head.nPos(M_C_BOT) - 1

            '�ŌÃ��R�[�h
            nCnt(M_C_TOP) = nRecNo(M_C_TOP) + 1
            If nCnt(M_C_TOP) >= HY_DEFLIST_REC_MAX Then
                nCnt(M_C_TOP) = 0
            End If

            nCnt(M_C_BOT) = nRecNo(M_C_BOT) + 1
            If nCnt(M_C_BOT) >= HY_DEFLIST_REC_MAX Then
                nCnt(M_C_BOT) = 0
            End If

            '�ŌÂ���ԍ�����U��
            n_cnt_buf = mPltNo
            For jj = 0 To 1
                For ii = 0 To HY_DEFLIST_REC_MAX - 1

                    If typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo <> "" And
                       False = m_dctProductNoBackColor.ContainsKey(typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo) Then
                        '�r���Ǘ��m�n�ɑ΂��ĐF�z��̗v�f����o�^����()
                        m_dctProductNoBackColor.Add(typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo, n_cnt_buf)
                        '�F�z��̗v�f�������Z
                        n_cnt_buf += 1
                        '�F�͂T�F���T�C�N���b�N�Ƃ��Ďg�p���邽�߁A�v�f�����T�ȏ�ɂȂ�����O�ɖ߂�
                        If n_cnt_buf >= MAX_PRODUCT_COLOR Then n_cnt_buf = 0

                        '���t�������𒴂�����Â����̂���J�����Ă���
                        m_lstData.Add(typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo)
                        If MC_PNOBG_MAX < m_dctProductNoBackColor.Count Then
                            m_dctProductNoBackColor.Remove(m_lstData.Item(0))
                            m_lstData.RemoveAt(0)
                        End If

                    End If

                    nCnt(jj) += 1
                    If nCnt(jj) >= HY_DEFLIST_REC_MAX Then
                        nCnt(jj) = 0
                    End If

                Next ii
            Next jj
            mPltNo = n_cnt_buf

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���Y�ԍ��w�i�F�R�t���G���[ [" & ex.Message & "]")
        End Try

    End Sub

    ''' <summary>
    ''' �r���Ǘ�NO�Ɋ���U��ꂽ�w�i�F��Ԃ�
    ''' </summary>
    ''' <param name="strProductNo">�r���Ǘ�NO</param>
    ''' <remarks></remarks>
    Public Function SetPNoBackColor(ByVal strProductNo As String) As System.Drawing.Color

        Dim bg_color As System.Drawing.Color

        Try
            bg_color = m_ProNo_BackColor(m_dctProductNoBackColor(strProductNo))
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "���Y�ԍ��w�i�F�擾�G���[ [" & ex.Message & "]")
        End Try

        Return bg_color

    End Function

End Class
