Option Strict On
Imports System.Collections
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class clsParamManager

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数 構造体定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵種 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure PARAM_TYPE
        ''' <summary>EdasysID</summary>
        Dim nEdasysID As Integer
        ''' <summary>優先度</summary>
        Dim nYuusen As Integer
        ''' <summary>正式名称</summary>
        Dim sName As String
        ''' <summary>略称</summary>
        Dim sSName As String
    End Structure

    ''' <summary>
    ''' グレード 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure PARAM_GRADE
        ''' <summary>EdasysID</summary>
        Dim nEdasysID As Integer
        ''' <summary>優先度</summary>
        Dim nYuusen As Integer
        ''' <summary>正式名称</summary>
        Dim sName As String
        ''' <summary>グレード色 (=オーバーレイ色も兼用)</summary>
        Dim clrDspColor As Color
    End Structure
    ''' <summary>
    ''' 検査表面状態 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure PARAM_SCODE
        ''' <summary>ID</summary>
        Dim nID As Integer
        ''' <summary>正式名称</summary>
        Dim sName As String
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' アクセスセッション
    Private m_Session As String													' iniファイルセッション

    ' DB情報
    Private m_Param_Type As New Dictionary(Of Integer, PARAM_TYPE)				' 疵種
    Private m_Param_Grade As New Dictionary(Of Integer, PARAM_GRADE)			' グレード
    Private m_Param_Scode As New Dictionary(Of Integer, PARAM_SCODE)		   ' 検査表面状態



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバーアクセス
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵種 指定EdasysID 取得
    ''' </summary>
    ''' <param name="nEdasysID">EdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetParamType(ByVal nEdasysID As Integer) As PARAM_TYPE
        If m_Param_Type.ContainsKey(nEdasysID) Then
            Return m_Param_Type(nEdasysID)
        Else
            Return Nothing
        End If
    End Function
    Public Function GetParamType() As Dictionary(Of Integer, PARAM_TYPE).ValueCollection
        Return m_Param_Type.Values
    End Function


    ''' <summary>
    ''' グレード 指定EdasysID 取得
    ''' </summary>
    ''' <param name="nEdasysID">EdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetParamGrade(ByVal nEdasysID As Integer) As PARAM_GRADE
        If m_Param_Grade.ContainsKey(nEdasysID) Then
            Return m_Param_Grade(nEdasysID)
        Else
            Return Nothing
        End If
    End Function
    Public Function GetParamGrade() As Dictionary(Of Integer, PARAM_GRADE).ValueCollection
        Return m_Param_Grade.Values
    End Function

    ''' <summary>
    ''' Scode 指定ID 取得
    ''' </summary>
    ''' <param name="nID">ID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetParamScode(ByVal nID As Integer) As PARAM_SCODE
        If m_Param_Scode.ContainsKey(nID) Then
            Return m_Param_Scode(nID)
        Else
            Return Nothing
        End If
    End Function
    Public Function GetParamScode() As Dictionary(Of Integer, PARAM_SCODE).ValueCollection
        Return m_Param_Scode.Values
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本処理
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' INIファイルセクション
    ''' </summary>
    ''' <param name="sSession">iniファイルセッション</param>
    ''' <remarks></remarks>
    Public Sub Set_DbSession(ByVal sSession As String)
        m_Session = sSession
    End Sub

    ''' <summary>
    ''' 全て読み込み
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG</returns>
    ''' <remarks></remarks>
    Public Function ReadAll() As Boolean
        If Not ReadParam_Type() Then Return False
        If Not ReadParam_Grade() Then Return False
        If Not ReadParam_Scode() Then Return False
        Return True
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DBアクセス (マスター関係)
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵種マスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_Type() As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        Dim wk As PARAM_TYPE

        'SQL文作成
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_TYPE", TASKINI_NAME)

        Try
            ' DB接続
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続異常 (ReadParam_Type)"))
                End If
                Return False
            End If

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行異常 (ReadParam_Type)({0})", sSQL))
                End If
                Return False
            End If


            ' 内部バッファ初期化
            m_Param_Type.Clear()

            ' データ取得
            While sqlRead.Read()

                wk.nEdasysID = sqlRead.GetInt32(0)
                wk.nYuusen = sqlRead.GetInt32(1)
                wk.sName = sqlRead.GetString(2)
                wk.sSName = sqlRead.GetString(3)

                ' 登録
                m_Param_Type.Add(wk.nEdasysID, wk)
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function


    ''' <summary>
    ''' グレードマスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_Grade() As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        Dim wk As PARAM_GRADE

        'SQL文作成
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_GRADE", TASKINI_NAME)

        Try
            ' DB接続
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続異常 (ReadParam_Grade)"))
                End If
                Return False
            End If

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行異常 (ReadParam_Grade)({0})", sSQL))
                End If
                Return False
            End If

            ' 内部バッファ初期化
            m_Param_Grade.Clear()

            ' データ取得
            While sqlRead.Read()

                wk.nEdasysID = sqlRead.GetInt32(0)
                wk.nYuusen = sqlRead.GetInt32(1)
                wk.sName = sqlRead.GetString(2)
                wk.clrDspColor = tMod.GetIntToColor(sqlRead.GetInt32(3))

                ' 登録
                m_Param_Grade.Add(wk.nEdasysID, wk)
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    ''' <summary>
    ''' 検査表面状態マスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_Scode() As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sSQL As String

        Dim wk As PARAM_SCODE

        'SQL文作成
        sSQL = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_SCODE", TASKINI_NAME)
        If sSQL = "" Then
            m_Param_Scode.Clear()
            Return True
        End If

        Try
            ' DB接続
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続異常 (ReadParam_Scode)"))
                End If
                Return False
            End If
            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行異常 (ReadParam_Scode)({0})", sSQL))
                End If
                Return False
            End If

            ' 内部バッファ初期化
            m_Param_Scode.Clear()

            ' データ取得
            While sqlRead.Read()

                wk.nID = sqlRead.GetInt32(0)
                wk.sName = sqlRead.GetString(1)

                ' 登録
                m_Param_Scode.Add(wk.nID, wk)
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DBアクセス (実績関係)
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コイル情報
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Friend Function ReadZi_CoilInf(ByVal clsRpi As tClass_ViewRpiThread, ByRef typ As DATA_COIL) As Boolean
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim wk As DATA_COIL
        wk.sKizukenNo = clsRpi.GetRpi_THeader.cKizukenNo
        '' SQL生成
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, "SQL_COIL_DATA", TASKINI_NAME)
        sSQL = String.Format(sSQL, wk.sKizukenNo)

        Try
            ' DB接続
            If Not tcls_DB.Open() Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続異常 (ReadZi_CoilInf)"))
                End If
                Return False
            End If

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行異常 (ReadZi_CoilInf)({0})", sSQL))
                End If
                Return False
            End If

            ' データ取得
            While sqlRead.Read()
                wk.sCoilNo = sqlRead.GetString(0)
                wk.sCoilNo = Trim(wk.sCoilNo)
                wk.nLen = sqlRead.GetInt32(1)
                wk.nWid = sqlRead.GetInt32(2)

                ' 登録
                typ = wk
            End While
        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function


    ''' <summary>
    ''' 疵情報 (元疵情報と疵情報の差分だけね)
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Friend Function ReadZi_DefectInf(ByVal clsRpi As tClass_ViewRpiThread, ByRef lst As List(Of DATA_DETECT_INF)) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sWk As String
        Dim wk As DATA_DETECT_INF

        '' SQL生成
        sWk = "SELECT_INF_" & clsRpi.GetRpi_THeader.nMen.ToString
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, sWk, TASKINI_NAME)
        sSQL = String.Format(sSQL, clsRpi.GetRpi_THeader.cKizukenNo)



        '======================================================
        ' データ取得
        Try
            '' DB接続
            If Not tcls_DB.Open() Then
                MsgBox("DBと接続失敗。疵オーバーレイの表示はできません。")
                Return False
            End If

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then
                If Not gcls_Log Is Nothing Then
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行異常 (ReadZi_DefectInf)({0})", sSQL))
                End If
                Return False
            End If

            ' データ取得
            While sqlRead.Read()
                wk.bDsp = True	' 最初は全選択

                wk.nNo = sqlRead.GetInt32(0)
                wk.tId = sqlRead.GetInt32(1)
                wk.gId = sqlRead.GetInt32(2)
                wk.nCamPos = sqlRead.GetInt32(3) - clsRpi.GetRpi_THeader.nStartCamset

                If sqlRead.Item(4).GetType().Name = "Int32" Then
                    wk.nFrameNum = sqlRead.GetInt32(4)
                    wk.nFrameNo = wk.nFrameNum - clsRpi.GetRpi_THeader.nFrameNumInit + 1
                    wk.nFrameXmin = sqlRead.GetInt32(5)
                    wk.nFrameXmax = sqlRead.GetInt32(6)
                    wk.nFrameYmin = sqlRead.GetInt32(7)
                    wk.nFrameYmax = sqlRead.GetInt32(8)
                Else
                    wk.nFrameNum = CInt(sqlRead.GetDouble(4))	' 四捨五入しないとダメ。マネージメントスタジオでは、整数に見えてもホントは0.999998とかになっている
                    wk.nFrameNo = wk.nFrameNum - clsRpi.GetRpi_THeader.nFrameNumInit + 1
                    wk.nFrameXmin = CInt(sqlRead.GetDouble(5))
                    wk.nFrameXmax = CInt(sqlRead.GetDouble(6))
                    wk.nFrameYmin = CInt(sqlRead.GetDouble(7))
                    wk.nFrameYmax = CInt(sqlRead.GetDouble(8))
                End If
                wk.nFrameNoBase = wk.nFrameNo

                wk.nKiriXmin = sqlRead.GetInt32(9)
                wk.nKiriXmax = sqlRead.GetInt32(10)
                wk.nKiriYmin = sqlRead.GetInt32(11)
                wk.nKiriYmax = sqlRead.GetInt32(12)

                wk.nY = sqlRead.GetInt32(13)
                wk.nX = sqlRead.GetInt32(14)
                wk.nXws = sqlRead.GetInt32(15)

                '' C基準に変換
                Dim nCenter As Integer = CInt((wk.nX + wk.nXws) / 2)		' センターからの距離に変換
                If wk.nX < wk.nXws Then		' DS (-)
                    wk.nXc = (wk.nX - nCenter)
                Else						' WS (+)
                    wk.nXc = (nCenter - wk.nXws)
                End If

                nCenter = CInt(clsRpi.GetRpi_Header().width / 2)					' 板センター
                Dim nWk As Integer = CInt(clsRpi.GetRpi_THeader().caminfo(wk.nCamPos).nBaseLeft + (wk.nFrameXmin + wk.nFrameXmax) / 2) - nCenter   ' センターからの距離に変換 [pix]
                wk.nFrameXc = CInt(nWk * clsRpi.GetRpi_Header().res_x)
                wk.nFrameBaseXc = wk.nFrameXc

                ' 反転前
                wk.nBaseY = wk.nY
                wk.nBaseX = wk.nX
                wk.nBaseXws = wk.nXws
                wk.nBaseXc = wk.nXc


                '' 優先度取得
                Try
                    wk.tYu = GetParamType(wk.tId).nYuusen
                    wk.gYu = GetParamGrade(wk.gId).nYuusen
                Catch ex As Exception
                End Try

                lst.Add(wk)
            End While

        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    ''' <summary>
    ''' カット情報
    ''' </summary>
    Friend Function ReadZi_CutInf(ByVal clsRpi As tClass_ViewRpiThread, ByRef lst As List(Of DATA_CUT_INF)) As Boolean
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim sWk As String
        Dim wk As DATA_CUT_INF

        '' SQL生成
        sWk = "SELECT_CUT"
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, sWk, TASKINI_NAME)
        If 0 = sSQL.Length Then Return True
        sSQL = String.Format(sSQL, clsRpi.GetRpi_THeader.cKizukenNo)


        '======================================================
        ' データ取得
        Try
            '' DB接続
            If Not tcls_DB.Open() Then
                MsgBox("DBと接続失敗。疵オーバーレイの表示はできません。")
                Return False
            End If

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return False

            ' データ取得
            While sqlRead.Read()
                wk.nY = sqlRead.GetInt32(0)
                ' 反転前
                wk.nBaseY = wk.nY

                lst.Add(wk)
            End While

        Catch ex As Exception
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True

    End Function

    ''' <summary>
    ''' ジャンプデピッチ値取得
    ''' </summary>
    ''' <returns>デフォルトピッチ値[mm]</returns>
    ''' <remarks></remarks>
    Friend Function ReadZi_DefPitch(ByVal clsRpi As tClass_ViewRpiThread) As Integer
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, m_Session)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim nWk As Integer = 0

        Dim sKizukenNo As String = clsRpi.GetRpi_THeader.cKizukenNo
        '' SQL生成
        Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(m_Session, "SELECT_JUMP", TASKINI_NAME)
        If 0 = sSQL.Length Then Return 0
        sSQL = String.Format(sSQL, sKizukenNo)

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Return 0

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return 0

            ' データ取得
            While sqlRead.Read()
                nWk = sqlRead.GetInt32(0)
                Exit While
            End While
        Catch ex As Exception
            Return 0
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return nWk
    End Function


    ''' <summary>
    ''' 反転
    ''' </summary>
    Friend Sub ReadZi_DefectInfReverce(ByVal clsRpi As tClass_ViewRpiThread, ByRef lst As List(Of DATA_DETECT_INF), ByRef lstCut As List(Of DATA_CUT_INF))

        Dim nCoilLen As Integer = CInt(Int((clsRpi.GetRpi_Header.nframe + 1) * clsRpi.GetRpi_Header.height * clsRpi.GetRpi_Header.res_y))
        Dim bTB As Boolean = clsRpi.GetCls_ImgDsp.tProp_ImgKind_IsTurnTB
        Dim bDsWs As Boolean = clsRpi.GetCls_ImgDsp.tProp_ImgKind_IsTurnLR


        For ii As Integer = 0 To lst.Count - 1
            Dim wk As DATA_DETECT_INF = lst(ii)

            ' 反転
            If bTB Then
                wk.nY = nCoilLen - wk.nBaseY
            Else
                wk.nY = wk.nBaseY
            End If

            ' 反転
            If bDsWs Then
                wk.nX = wk.nBaseXws
                wk.nXws = wk.nBaseX
                wk.nXc = wk.nBaseXc * -1
                wk.nFrameXc = wk.nFrameBaseXc * -1
            Else
                wk.nX = wk.nBaseX
                wk.nXws = wk.nBaseXws
                wk.nXc = wk.nBaseXc
                wk.nFrameXc = wk.nFrameBaseXc
            End If


            lst(ii) = wk
        Next


        For ii As Integer = 0 To lstCut.Count - 1
            Dim wk As DATA_CUT_INF = lstCut(ii)

            ' 反転
            If bTB Then
                wk.nY = nCoilLen - wk.nBaseY
            Else
                wk.nY = wk.nBaseY
            End If
            lstCut(ii) = wk
        Next

    End Sub

End Class
