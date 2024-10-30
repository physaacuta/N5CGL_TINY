
Imports tClassLibrary

Public Class clsRecvMail




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Event tEv_RecvMail(ByVal nEvtNo As Integer, ByVal que As COMMON_QUE_STRMIX)	' 散布図クリック (イベントNo, Que本体情報)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mail As tClassLibrary.tClass.tClass_MailSlot			   ' メールスロット受信クラス
    Private m_RecvQueBuf As COMMON_QUE_STRMIX							' メールスロット受信バッファ

    Private m_bRecving As Boolean = False								' 受信可能true

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New(ByVal cont As Control)
        Try
            mcls_Mail = New tClass.tClass_MailSlot(g_typStartArgs.sTaskName, 300)
            mcls_Mail.Mail_ReadStart(m_RecvQueBuf, AddressOf tDele_GetMail, cont)
            m_bRecving = True

        Catch ex As Exception
            'MsgBox("Init失敗")
            m_bRecving = False
            End
        End Try
    End Sub

    ''' <summary>
    ''' デストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Dispose()

        Try
            mcls_Mail.Mail_ReadStop()
            mcls_Mail.Dispose()
            mcls_Mail = Nothing
        Catch ex As Exception
        Finally
            m_bRecving = False
        End Try
    End Sub

    ''' <summary>
    ''' 受信
    ''' </summary>
    ''' <param name="recvVal"></param>
    ''' <remarks></remarks>
    Public Sub tDele_GetMail(ByVal recvVal As System.ValueType)
        Dim que As COMMON_QUE_STRMIX = CType(recvVal, COMMON_QUE_STRMIX)
        Dim nEvtNo As Integer = que.nEventNo
        RaiseEvent tEv_RecvMail(nEvtNo, que)
    End Sub


End Class
