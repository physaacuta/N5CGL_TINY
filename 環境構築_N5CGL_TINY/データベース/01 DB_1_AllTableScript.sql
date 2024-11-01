USE [master]
GO
/****** Object:  Database [DATABASE_N5CGL_TINY]    Script Date: 2024/08/19 19:42:11 ******/
CREATE DATABASE [DATABASE_N5CGL_TINY]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'DATABASE_N5CGL_TINY', FILENAME = N'f:\db\DATABASE_N5CGL_TINY.MDF' , SIZE = 12000GB , MAXSIZE = UNLIMITED, FILEGROWTH = 0)
 LOG ON 
( NAME = N'DATABASE_N5CGL_TINY_log', FILENAME = N'f:\db\DATABASE_N5CGL_TINY_log.LDF' , SIZE = 102400KB , MAXSIZE = 2048GB , FILEGROWTH = 10240KB )
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET COMPATIBILITY_LEVEL = 120
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [DATABASE_N5CGL_TINY].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET ARITHABORT OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET  ENABLE_BROKER 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET RECOVERY SIMPLE 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET  MULTI_USER 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET DB_CHAINING OFF 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET TARGET_RECOVERY_TIME = 0 SECONDS 
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET DELAYED_DURABILITY = DISABLED 
GO
USE [DATABASE_N5CGL_TINY]
GO
/****** Object:  Table [dbo].[ATTR_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[ATTR_NAME](
	[Angle] [int] NOT NULL,
	[Enum] [int] NOT NULL,
	[EdasysID] [int] NOT NULL,
	[Name] [varchar](128) NULL,
 CONSTRAINT [PK_ATTR_NAME] PRIMARY KEY CLUSTERED 
(
	[Angle] ASC,
	[Enum] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BUF_FF_R_MAEINF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BUF_FF_R_MAEINF](
	[ID] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[生伝文1] [varbinary](max) NOT NULL,
	[生伝文2] [varbinary](max) NOT NULL,
 CONSTRAINT [PK_BUF_FF_R_MAEINF] PRIMARY KEY CLUSTERED 
(
	[ID] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BUF_FF_S_ZISEKI]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BUF_FF_S_ZISEKI](
	[ID] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[伝文区分] [int] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[生伝文1] [varbinary](max) NOT NULL,
	[生伝文2] [varbinary](max) NOT NULL,
	[生伝文3] [varbinary](max) NOT NULL,
	[生伝文4] [varbinary](max) NOT NULL,
	[管理No] [varchar](20) NOT NULL,
	[送信日付] [datetime] NULL,
 CONSTRAINT [PK_BUF_FF_S_ZISEKI] PRIMARY KEY CLUSTERED 
(
	[ID] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BUF_PRO_R_COILINF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BUF_PRO_R_COILINF](
	[ID] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[送信位置] [int] NOT NULL,
	[生伝文] [varbinary](max) NOT NULL,
 CONSTRAINT [PK_BUF_PRO_R_COILINF] PRIMARY KEY CLUSTERED 
(
	[ID] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BUF_PRO_R_CUT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BUF_PRO_R_CUT](
	[ID] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[開始位置] [int] NOT NULL,
	[終了位置] [int] NOT NULL,
	[生伝文] [varbinary](max) NOT NULL,
	[管理No] [varchar](20) NULL,
	[編集日付] [datetime] NULL,
 CONSTRAINT [PK_BUF_PRO_R_CUT] PRIMARY KEY CLUSTERED 
(
	[ID] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BUF_PRO_R_SAISUN]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BUF_PRO_R_SAISUN](
	[ID] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[生伝文] [varbinary](max) NOT NULL,
	[管理No] [varchar](20) NULL,
	[編集日付] [datetime] NULL,
 CONSTRAINT [PK_BUF_PRO_R_SAISUN] PRIMARY KEY CLUSTERED 
(
	[ID] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[BUF_PRO_S_ZISEKI]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[BUF_PRO_S_ZISEKI](
	[ID] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[伝文区分] [int] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[分割No] [int] NOT NULL,
	[生伝文] [varbinary](max) NOT NULL,
	[管理No] [varchar](20) NULL,
	[送信日付] [datetime] NULL,
 CONSTRAINT [PK_BUF_PRO_S_ZISEKI] PRIMARY KEY CLUSTERED 
(
	[ID] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[GRADE_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[GRADE_NAME](
	[EdasysID] [int] NOT NULL,
	[ID] [int] NOT NULL,
	[Name] [varchar](64) NULL,
	[優先度] [int] NOT NULL,
	[マップ表示色] [int] NOT NULL,
	[欠陥グレード] [int] NOT NULL,
	[FF伝送名称] [int] NOT NULL,
	[プロコングループ] [int] NOT NULL,
	[疵マップシンボル表示色] [int] NOT NULL,
	[Score] [int] NOT NULL,
	[NumLimit] [int] NOT NULL,
 CONSTRAINT [PK_GRADE_NAME] PRIMARY KEY CLUSTERED 
(
	[EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[Group_Name]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[Group_Name](
	[EdasysID] [int] NOT NULL,
	[Name] [varchar](32) NOT NULL,
 CONSTRAINT [PK_Group_Name] PRIMARY KEY CLUSTERED 
(
	[EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_BASE_ACTION_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_BASE_ACTION_MASTER](
	[表示順番] [int] NOT NULL,
	[アラームNo] [int] NOT NULL,
	[未実行異常時間] [int] NULL,
	[表示名称] [varchar](256) NULL,
	[内容追加条件] [varchar](256) NULL,
 CONSTRAINT [PK_M_BASE_ACTION_MASTER] PRIMARY KEY CLUSTERED 
(
	[表示順番] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_CAMERA_LINKSWITCH]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_CAMERA_LINKSWITCH](
	[ID] [int] NOT NULL,
	[ボードID] [int] NOT NULL,
	[ステータス] [bit] NOT NULL,
	[ボード1_カメラ選択] [int] NOT NULL,
	[ボード2_カメラ選択] [int] NOT NULL,
	[ボード3_カメラ選択] [int] NOT NULL,
	[ボード4_カメラ選択] [int] NOT NULL,
	[ボード5_カメラ選択] [int] NOT NULL,
	[ボード6_カメラ選択] [int] NOT NULL,
	[ボード7_カメラ選択] [int] NOT NULL,
	[ボード8_カメラ選択] [int] NOT NULL,
	[ボード9_カメラ選択] [int] NOT NULL,
	[ボード10_カメラ選択] [int] NOT NULL,
	[ボード11_カメラ選択] [int] NOT NULL,
	[ボード12_カメラ選択] [int] NOT NULL,
	[ボード13_カメラ選択] [int] NOT NULL,
	[ボード14_カメラ選択] [int] NOT NULL,
	[ボード15_カメラ選択] [int] NOT NULL,
 CONSTRAINT [PK_M_CAMERA_LINKSWITCH] PRIMARY KEY CLUSTERED 
(
	[ID] ASC,
	[ボードID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_COMMON_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_COMMON_MASTER](
	[LINEID] [int] NOT NULL,
	[RAS正常色] [int] NOT NULL,
	[RAS異常色] [int] NOT NULL,
	[RAS注意色] [int] NOT NULL,
	[LOG表示件数] [int] NOT NULL,
	[LOGイベント色] [int] NOT NULL,
	[LOG重故障色] [int] NOT NULL,
	[LOG軽故障色] [int] NOT NULL,
	[探傷画面表示区間小] [int] NOT NULL,
	[探傷画面表示区間大] [int] NOT NULL,
	[ウォーマ材指定位置] [int] NOT NULL,
	[Jpeg切り出し画像品質] [int] NOT NULL,
	[カメラ温度閾値] [int] NOT NULL,
	[エッジ整合性_実施種別] [int] NOT NULL,
	[エッジ整合性_検出範囲] [real] NOT NULL,
	[エッジ整合性_リカバリー検出範囲] [real] NOT NULL,
	[エッジ整合性_板幅許容誤差] [real] NOT NULL,
	[エッジ整合性_前フレームとの許容誤差] [real] NOT NULL,
	[エッジ整合性_溶接後不感帯] [int] NOT NULL,
	[エッジ整合性_エッジ検出異常FF送信] [int] NOT NULL,
	[エッジ検出_実施種別] [int] NOT NULL,
	[エッジ検出_検出範囲] [real] NOT NULL,
	[重複疵マップ背景色_軽] [int] NOT NULL,
	[重複疵マップ背景色_中] [int] NOT NULL,
	[重複疵マップ背景色_重] [int] NOT NULL,
	[重複疵マップ疵個数閾値_軽] [int] NOT NULL,
	[重複疵マップ疵個数閾値_中] [int] NOT NULL,
	[重複疵マップ疵個数閾値_重] [int] NOT NULL,
	[設備長_検査基点_上流カメラ] [int] NOT NULL,
	[設備長_検査基点_SPM] [int] NOT NULL,
	[設備長_表示基点_裏検査台] [int] NOT NULL,
	[設備長_表示基点_表検査台] [int] NOT NULL,
	[設備長_表示基点_シャー] [int] NOT NULL,
	[設備長_検査基点ゲート信号ON_検査基点] [int] NOT NULL,
	[設備長_表示基点ゲート信号ON_表示基点] [int] NOT NULL,
	[疵接近ランプ_不感帯_先端] [int] NOT NULL,
	[疵接近ランプ_不感帯_尾端] [int] NOT NULL,
	[疵接近ランプ_出力区間_前] [int] NOT NULL,
	[疵接近ランプ_出力区間_後] [int] NOT NULL,
	[疵接近アラーム_不感帯_先端] [int] NOT NULL,
	[疵接近アラーム_不感帯_尾端] [int] NOT NULL,
	[疵接近アラーム_出力区間_前] [int] NOT NULL,
	[疵接近アラーム_出力区間_後] [int] NOT NULL,
	[ボイス_出力位置_前] [int] NOT NULL,
	[全長画像_保存_R] [int] NOT NULL,
	[全長画像_保存_G] [int] NOT NULL,
	[全長画像_保存_B] [int] NOT NULL,
	[全長画像_保存_差] [int] NOT NULL,
	[PLG距離比較異常上限値] [real] NOT NULL,
	[PLG距離表示異常上限値] [real] NOT NULL,
	[プロコンコイル長比較異常上限値] [real] NOT NULL,
	[ロジックパララン機能01_入力カメラ01] [int] NOT NULL,
	[ロジックパララン機能01_入力カメラ02] [int] NOT NULL,
	[ロジックパララン機能02_入力カメラ01] [int] NOT NULL,
	[ロジックパララン機能02_入力カメラ02] [int] NOT NULL,
	[LongPd_GuidanceAvailable] [int] NOT NULL,
	[LongPd_GuidanceCheckInterval] [int] NOT NULL,
	[LongPd_NearFurnaceRangeFrom] [int] NOT NULL,
	[LongPd_NearFurnaceRangeTo] [int] NOT NULL,
	[LongPd_PeriodicPitchTolerance] [int] NOT NULL,
	[LongPd_GuidanceForeColor] [int] NOT NULL,
	[LongPd_GuidanceBackColor] [int] NOT NULL,
	[LongPd_GuidanceMessage] [varchar](64) NOT NULL,
 CONSTRAINT [PK_M_COMMON_MASTER] PRIMARY KEY CLUSTERED 
(
	[LINEID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_LOG_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_LOG_MASTER](
	[アラームNo] [int] NOT NULL,
	[種別] [int] NOT NULL,
	[区分] [int] NOT NULL,
	[オペガイ有無] [int] NULL,
	[オペガイ時間] [int] NULL,
	[内容] [varchar](128) NULL,
	[詳細] [varchar](256) NULL,
 CONSTRAINT [PK_M_LOG_MASTER] PRIMARY KEY CLUSTERED 
(
	[アラームNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_LONGPD_COMMON]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_LONGPD_COMMON](
	[ID] [int] NOT NULL,
	[ExecInterval] [int] NOT NULL,
	[InitialExec_OldLimit] [int] NOT NULL,
	[SQLQuery_UpdateAttr_Top] [varchar](max) NULL,
	[SQLQuery_UpdateAttr_Bot] [varchar](max) NULL,
	[MaxDefectLen] [int] NOT NULL,
	[FFTFreq] [int] NOT NULL,
	[FFTPeakThr] [float] NOT NULL,
	[HoughConvertAccumulatorRasio] [float] NOT NULL,
	[HoughConvertMinLineLengthRasio] [float] NOT NULL,
	[HoughConvertMaxLineGap] [int] NOT NULL,
	[HoughConvertOnlineThr] [float] NULL,
	[HoughConvertTangentThr] [float] NULL,
	[ExpandImage] [int] NOT NULL,
	[CsvDir] [varchar](max) NOT NULL,
	[CsvSaveDate] [int] NOT NULL,
	[PeriodicExecInterval] [int] NULL,
 CONSTRAINT [PK_M_LONGPD_COMMON] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_LONGPD_PCODE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_LONGPD_PCODE](
	[Pcode] [int] NOT NULL,
	[Enable] [int] NOT NULL,
 CONSTRAINT [PK_M_LONGPD_PCODE] PRIMARY KEY CLUSTERED 
(
	[Pcode] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_LONGPD_PCODE_FILTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_LONGPD_PCODE_FILTER](
	[Pcode] [int] NOT NULL,
	[Filter] [int] NOT NULL,
	[Name] [varchar](64) NULL,
	[Enable] [int] NOT NULL,
	[MaskFront] [int] NOT NULL,
	[MaskTail] [int] NOT NULL,
	[MaskLeft] [int] NOT NULL,
	[MaskRight] [int] NOT NULL,
	[DetectPdMin] [int] NOT NULL,
	[DetectPdMax] [int] NOT NULL,
	[DefectNumMin] [int] NOT NULL,
	[DefectNumMax] [int] NOT NULL,
	[WidthDiv] [int] NOT NULL,
	[WidthShiftPitch] [int] NULL,
	[FFTEx] [int] NULL,
	[CommonID] [int] NULL,
 CONSTRAINT [PK_M_LONGPD_PCODE_FILTER] PRIMARY KEY CLUSTERED 
(
	[Pcode] ASC,
	[Filter] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_LONGPD_PCODE_FILTER_OUTPUT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_LONGPD_PCODE_FILTER_OUTPUT](
	[pcode] [int] NOT NULL,
	[filter] [int] NOT NULL,
	[TypeEdasysID] [int] NOT NULL,
	[GrEdasysID] [int] NOT NULL,
 CONSTRAINT [PK_M_LONGPD_PCODE_FILTER_OUTPUT] PRIMARY KEY CLUSTERED 
(
	[pcode] ASC,
	[filter] ASC,
	[TypeEdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_MAE_GRADE_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_MAE_GRADE_NAME](
	[前工程コード] [varchar](2) NOT NULL,
	[FFName] [varchar](2) NOT NULL,
	[グレードID] [int] NOT NULL,
	[Name] [varchar](32) NOT NULL,
	[優先度] [int] NOT NULL,
	[マップ表示色] [int] NOT NULL,
 CONSTRAINT [PK_M_MAE_GRADE_NAME] PRIMARY KEY CLUSTERED 
(
	[前工程コード] ASC,
	[FFName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_MAE_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_MAE_MASTER](
	[前工程コード] [varchar](2) NOT NULL,
	[有効判定] [int] NOT NULL,
	[名称] [varchar](32) NULL,
	[前工程表示モード] [int] NOT NULL,
	[画像形式] [int] NOT NULL,
	[疵画像名称1] [varchar](32) NULL,
	[疵画像名称2] [varchar](32) NULL,
	[疵画像名称3] [varchar](32) NULL,
	[疵画像名称4] [varchar](32) NULL,
	[他所材判定] [int] NOT NULL,
 CONSTRAINT [PK_M_MAE_MASTER] PRIMARY KEY CLUSTERED 
(
	[前工程コード] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_MAE_TYPE_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_MAE_TYPE_NAME](
	[前工程コード] [varchar](2) NOT NULL,
	[FFName] [varchar](2) NOT NULL,
	[ID] [int] NOT NULL,
	[Name] [varchar](32) NOT NULL,
	[優先度] [int] NOT NULL,
	[マップ表示名称] [varchar](4) NOT NULL,
	[表示有無] [int] NOT NULL,
 CONSTRAINT [PK_M_MAE_TYPE_NAME] PRIMARY KEY CLUSTERED 
(
	[前工程コード] ASC,
	[FFName] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PALL_OUTPUT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_PALL_OUTPUT](
	[疵種EdasysID] [int] NOT NULL,
	[重欠陥一覧画面表示_詳細条件] [int] NOT NULL,
	[重欠陥一覧画面表示_簡易条件] [int] NOT NULL,
	[重欠陥一覧画面表示_簡易項目] [int] NOT NULL,
 CONSTRAINT [PK_M_PALL_OUTPUT] PRIMARY KEY CLUSTERED 
(
	[疵種EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_PAR_GRID_SETTING]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_PAR_GRID_SETTING](
	[画面ID] [varchar](20) NOT NULL,
	[グリッドID] [int] NOT NULL,
	[項目No] [int] NOT NULL,
	[項目ヘッダー1] [varchar](256) NULL,
	[項目ヘッダー2] [varchar](256) NULL,
	[備考] [varchar](256) NULL,
	[項目セルタイプ] [int] NOT NULL,
	[lov_id] [int] NULL,
	[表示_項目表示有無] [bit] NOT NULL,
	[表示_読み取り専用] [bit] NOT NULL,
	[表示_項目幅] [int] NULL,
	[表示_項目配置] [int] NOT NULL,
	[表示_フォーマット] [varchar](32) NULL,
	[表示_列固定] [bit] NOT NULL,
	[DB_テーブル名] [varchar](256) NULL,
	[DB_フィールド名] [varchar](256) NULL,
	[DB_主キー] [bit] NOT NULL,
	[DB_フィールドタイプ] [int] NOT NULL,
	[入力_入力可能最小値] [float] NULL,
	[入力_入力可能最大値] [float] NULL,
	[入力_NULL入力可否] [bit] NOT NULL,
	[入力_入力可能桁数] [int] NULL,
	[入力_小数点以下桁数] [int] NULL,
	[入力_表示ダイアログ] [int] NULL,
 CONSTRAINT [PK_M_PAR_GRID_SETTING] PRIMARY KEY CLUSTERED 
(
	[画面ID] ASC,
	[グリッドID] ASC,
	[項目No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PAR_LOV_ITEM]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_PAR_LOV_ITEM](
	[lov_id] [int] NOT NULL,
	[seq] [int] NOT NULL,
	[disp_member] [varchar](64) NOT NULL,
	[val_member_i] [int] NULL,
	[val_member_c] [varchar](32) NULL,
	[val_member_b] [bit] NULL,
 CONSTRAINT [PK_M_PAR_LOV_ITEM] PRIMARY KEY CLUSTERED 
(
	[lov_id] ASC,
	[seq] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PC_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_PC_INF](
	[PCKIND] [int] NOT NULL,
	[PC種別] [varchar](32) NULL,
	[PC区分] [int] NOT NULL,
	[GroupNo] [int] NOT NULL,
	[PCID] [int] NOT NULL,
 CONSTRAINT [PK_M_PC_INF] PRIMARY KEY CLUSTERED 
(
	[PCKIND] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PC_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_PC_MASTER](
	[PCID] [int] NOT NULL,
	[PC名] [varchar](16) NOT NULL,
	[GroupNo] [int] NOT NULL,
	[パソコン名称] [varchar](32) NULL,
 CONSTRAINT [PK_M_PC_MASTER] PRIMARY KEY CLUSTERED 
(
	[PCID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PCODE_JUDGE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_PCODE_JUDGE](
	[pcode] [int] NOT NULL,
	[条件No] [int] NOT NULL,
	[有効判定] [int] NOT NULL,
	[命令No] [varchar](32) NULL,
	[生産No] [varchar](32) NULL,
	[注文板厚下限] [int] NULL,
	[注文板厚上限] [int] NULL,
	[検査基準] [varchar](32) NULL,
	[向先] [varchar](32) NULL,
	[用途] [varchar](32) NULL,
	[主材質] [varchar](32) NULL,
	[表面処理1桁目] [varchar](32) NULL,
	[品種] [varchar](32) NULL,
 CONSTRAINT [PK_M_PCODE_JUDGE] PRIMARY KEY CLUSTERED 
(
	[pcode] ASC,
	[条件No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PCODE_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_PCODE_MASTER](
	[pcode] [int] NOT NULL,
	[有効判定] [int] NOT NULL,
	[検査パターン名称] [varchar](32) NULL,
	[優先度] [int] NULL,
 CONSTRAINT [PK_M_PCODE_MASTER] PRIMARY KEY CLUSTERED 
(
	[pcode] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_PCODE_OUTPUT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_PCODE_OUTPUT](
	[pcode] [int] NOT NULL,
	[疵種EdasysID] [int] NOT NULL,
	[探傷画面表示_表] [int] NOT NULL,
	[探傷画面表示_裏] [int] NOT NULL,
	[入側単位疵点マップ画面表示] [int] NOT NULL,
	[FF送信有無] [int] NOT NULL,
	[疵接近軽] [int] NOT NULL,
	[疵接近重] [int] NOT NULL,
	[パトライト出力] [int] NOT NULL,
	[ボイス連続疵] [int] NOT NULL,
	[ボイス重大疵] [int] NOT NULL,
	[ボイスヘゲ] [int] NOT NULL,
	[ボイススリバ] [int] NOT NULL,
	[ボイス表裏同一疵] [int] NOT NULL,
	[ボイス周期疵] [int] NOT NULL,
	[ボイスその他疵] [int] NOT NULL,
	[DB保存_一般_表] [int] NULL,
	[DB保存_一般_裏] [int] NULL,
	[DB保存_優先_表] [int] NULL,
	[DB保存_優先_裏] [int] NULL,
 CONSTRAINT [PK_M_PCODE_OUTPUT] PRIMARY KEY CLUSTERED 
(
	[pcode] ASC,
	[疵種EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_SCODE_JUDGE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_SCODE_JUDGE](
	[scode] [int] NOT NULL,
	[条件No] [int] NOT NULL,
	[有効判定] [int] NOT NULL,
	[SPM圧下有無] [int] NULL,
	[命令No] [varchar](32) NULL,
	[生産No] [varchar](32) NULL,
	[注文板厚下限] [int] NULL,
	[注文板厚上限] [int] NULL,
	[検査基準] [varchar](32) NULL,
	[向先] [varchar](32) NULL,
	[用途] [varchar](32) NULL,
	[主材質] [varchar](32) NULL,
	[表面処理1桁目] [varchar](32) NULL,
	[品種] [varchar](32) NULL,
 CONSTRAINT [PK_M_SCODE_JUDGE] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[条件No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_SCODE_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_SCODE_MASTER](
	[scode] [int] NOT NULL,
	[有効判定] [int] NOT NULL,
	[検査表面状態名称] [varchar](32) NULL,
	[優先度] [int] NULL,
 CONSTRAINT [PK_M_SCODE_MASTER] PRIMARY KEY CLUSTERED 
(
	[scode] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_SCODE_OUTPUT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_SCODE_OUTPUT](
	[scode] [int] NOT NULL,
	[疵種EdasysID] [int] NOT NULL,
	[ダミー出力] [int] NOT NULL,
 CONSTRAINT [PK_M_SCODE_OUTPUT] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[疵種EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_ST_HISTORY_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_ST_HISTORY_MASTER](
	[区分] [int] NOT NULL,
	[収集周期] [int] NOT NULL,
	[名称1] [varchar](64) NULL,
	[名称2] [varchar](64) NULL,
	[名称3] [varchar](64) NULL,
	[名称4] [varchar](64) NULL,
	[名称5] [varchar](64) NULL,
	[名称6] [varchar](64) NULL,
	[名称7] [varchar](64) NULL,
	[名称8] [varchar](64) NULL,
	[名称9] [varchar](64) NULL,
	[名称10] [varchar](64) NULL,
	[名称11] [varchar](64) NULL,
	[名称12] [varchar](64) NULL,
	[名称13] [varchar](64) NULL,
	[名称14] [varchar](64) NULL,
	[名称15] [varchar](64) NULL,
	[名称16] [varchar](64) NULL,
	[名称17] [varchar](64) NULL,
	[名称18] [varchar](64) NULL,
	[名称19] [varchar](64) NULL,
	[名称20] [varchar](64) NULL,
	[機器ID_1] [int] NULL,
	[機器ID_2] [int] NULL,
	[機器ID_3] [int] NULL,
	[機器ID_4] [int] NULL,
	[機器ID_5] [int] NULL,
	[機器ID_6] [int] NULL,
	[機器ID_7] [int] NULL,
	[機器ID_8] [int] NULL,
	[機器ID_9] [int] NULL,
	[機器ID_10] [int] NULL,
	[機器ID_11] [int] NULL,
	[機器ID_12] [int] NULL,
	[機器ID_13] [int] NULL,
	[機器ID_14] [int] NULL,
	[機器ID_15] [int] NULL,
	[機器ID_16] [int] NULL,
	[機器ID_17] [int] NULL,
	[機器ID_18] [int] NULL,
	[機器ID_19] [int] NULL,
	[機器ID_20] [int] NULL,
 CONSTRAINT [PK_M_ST_HISTORY_MASTER] PRIMARY KEY CLUSTERED 
(
	[区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[M_STATUS_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[M_STATUS_INF](
	[機器ID] [int] NOT NULL,
	[機器状態] [int] NOT NULL,
 CONSTRAINT [PK_M_STATUS_INF] PRIMARY KEY CLUSTERED 
(
	[機器ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[M_STATUS_MASTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[M_STATUS_MASTER](
	[機器ID] [int] NOT NULL,
	[機器名] [varchar](64) NOT NULL,
	[PCID] [int] NOT NULL,
	[回線確認有無] [int] NOT NULL,
	[初期値] [int] NULL,
	[システム範囲] [int] NULL,
	[異常範囲] [int] NOT NULL,
	[検査異常度] [int] NOT NULL,
	[機器異常度] [int] NOT NULL,
	[優先度] [int] NULL,
	[異常MSG] [varchar](256) NULL,
 CONSTRAINT [PK_M_STATUS_MASTER] PRIMARY KEY CLUSTERED 
(
	[機器ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_CAMANGLE_SCODE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PARAM_CAMANGLE_SCODE](
	[scode] [int] NOT NULL,
	[camangle] [int] NOT NULL,
	[AGCupper] [int] NOT NULL,
	[AGClower] [int] NOT NULL,
	[ShdTarget] [int] NOT NULL,
	[ShdGainLimit] [float] NOT NULL,
	[ShdXsize] [int] NOT NULL,
	[ShdYsize] [int] NOT NULL,
	[ShdEdgeXSize] [int] NOT NULL,
	[ShdEdgeYSize] [int] NOT NULL,
	[SmoothOn] [bit] NOT NULL,
	[MainLimitLower] [int] NOT NULL,
	[MainLimitUpper] [int] NOT NULL,
	[SimpleBinThrLower] [int] NOT NULL,
	[SimpleBinThrUpper] [int] NOT NULL,
	[NopenSimpleBin] [int] NOT NULL,
	[slice_l0] [int] NOT NULL,
	[slice_l1] [int] NOT NULL,
	[slice_l2] [int] NOT NULL,
	[slice_l3] [int] NOT NULL,
	[slice_u0] [int] NOT NULL,
	[slice_u1] [int] NOT NULL,
	[slice_u2] [int] NOT NULL,
	[slice_u3] [int] NOT NULL,
	[WindMinCoef] [float] NOT NULL,
	[WindMaxCoef] [float] NOT NULL,
	[WindThrLower] [int] NOT NULL,
	[WindThrUpper] [int] NOT NULL,
	[PdLowerLimit] [int] NOT NULL,
	[PdUpperLimit] [int] NOT NULL,
	[PdContrExpFactor] [float] NOT NULL,
	[PdBinThrLower] [int] NOT NULL,
	[PdBinThrUpper] [int] NOT NULL,
	[PdNopenSimpleBin] [int] NOT NULL,
	[PdSliceL0] [int] NOT NULL,
	[PdSliceL1] [int] NOT NULL,
	[PdSliceL2] [int] NOT NULL,
	[PdSliceL3] [int] NOT NULL,
	[PdSliceU0] [int] NOT NULL,
	[PdSliceU1] [int] NOT NULL,
	[PdSliceU2] [int] NOT NULL,
	[PdSliceU3] [int] NOT NULL,
	[NearEdgeNopenThr1] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_CAMANGLE_SCODE] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[camangle] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[PARAM_CAMSET]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_CAMSET](
	[camset] [int] NOT NULL,
	[CamPair] [int] NOT NULL,
	[CamPairCycle] [int] NULL,
	[Hres] [float] NOT NULL,
	[Xoffset] [float] NOT NULL,
	[K11] [int] NOT NULL,
	[K12] [int] NOT NULL,
	[K13] [int] NOT NULL,
	[K21] [int] NOT NULL,
	[K22] [int] NOT NULL,
	[K23] [int] NOT NULL,
	[K31] [int] NOT NULL,
	[K32] [int] NOT NULL,
	[K33] [int] NOT NULL,
	[RefGain_R] [float] NOT NULL,
	[RefGain_G] [float] NOT NULL,
	[RefGain_B] [float] NOT NULL,
	[RefExpTime_R] [int] NOT NULL,
	[RefExpTime_G] [int] NOT NULL,
	[RefExpTime_B] [int] NOT NULL,
	[EdgeDetectStart_L] [int] NOT NULL,
	[EdgeDetectStart_R] [int] NOT NULL,
	[EdgeDetectType] [int] NOT NULL,
	[EdgeLineFollow] [int] NOT NULL,
	[DdsSavePath] [varchar](256) NOT NULL,
	[SendToDownstream] [bit] NOT NULL,
 CONSTRAINT [PK_PARAM_CAMSET] PRIMARY KEY CLUSTERED 
(
	[camset] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_CAMSET_ANGLE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PARAM_CAMSET_ANGLE](
	[camangle] [int] NOT NULL,
	[camset] [int] NOT NULL,
	[CamOffsetTap1] [int] NOT NULL,
	[CamOffsetTap2] [int] NOT NULL,
	[CamOffsetTap3] [int] NOT NULL,
	[CamOffsetTap4] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_CAMSET_ANGLE] PRIMARY KEY CLUSTERED 
(
	[camangle] ASC,
	[camset] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[PARAM_CAMSET_ANGLE_SCODE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PARAM_CAMSET_ANGLE_SCODE](
	[scode] [int] NOT NULL,
	[camangle] [int] NOT NULL,
	[camset] [int] NOT NULL,
	[InitialExpTime] [int] NOT NULL,
	[CameraGain] [float] NOT NULL,
	[EdgeMAsize] [int] NOT NULL,
	[EdgeLowLimit] [int] NOT NULL,
	[EdgeFindMethod] [int] NOT NULL,
	[EdgeFollowMethod] [int] NOT NULL,
	[NearEdgeThrLower1] [int] NOT NULL,
	[NearEdgeThrLower2] [int] NOT NULL,
	[NearEdgeThrUpper1] [int] NOT NULL,
	[NearEdgeThrUpper2] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_CAMSET_ANGLE_SCODE] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[camangle] ASC,
	[camset] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[PARAM_CAMSET_SCODE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PARAM_CAMSET_SCODE](
	[scode] [int] NOT NULL,
	[camset] [int] NOT NULL,
	[EdgeDetectCam] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_CAMSET_SCODE] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[camset] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[PARAM_COMMON]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_COMMON](
	[Key] [int] NOT NULL,
	[PLGパルスレート] [float] NOT NULL,
	[PLGパルスレート比較] [float] NOT NULL,
	[PLGパルスレート表示] [float] NOT NULL,
	[カメラPLG逓倍率] [int] NOT NULL,
	[カメラPLG分周率] [int] NOT NULL,
	[カメラPLG信号特性] [int] NOT NULL,
	[カメラPLG位相] [int] NOT NULL,
	[カメラPLG位相差] [int] NOT NULL,
	[カメラPLG逓倍率_O] [int] NOT NULL,
	[カメラPLG分周率_O] [int] NOT NULL,
	[カメラPLG信号特性_O] [int] NOT NULL,
	[カメラPLG位相_O] [int] NOT NULL,
	[カメラPLG位相差_O] [int] NOT NULL,
	[カメラPLG逓倍率_U] [int] NOT NULL,
	[カメラPLG分周率_U] [int] NOT NULL,
	[カメラPLG信号特性_U] [int] NOT NULL,
	[カメラPLG位相_U] [int] NOT NULL,
	[カメラPLG位相差_U] [int] NOT NULL,
	[カメラPLG逓倍率_T] [int] NOT NULL,
	[カメラPLG分周率_T] [int] NOT NULL,
	[カメラPLG信号特性_T] [int] NOT NULL,
	[カメラPLG位相_T] [int] NOT NULL,
	[カメラPLG位相差_T] [int] NOT NULL,
	[カメラタップ数] [int] NOT NULL,
	[表裏間基準面] [int] NOT NULL,
	[表裏間距離] [float] NOT NULL,
	[外接長方形の重なり] [float] NOT NULL,
	[Vres] [float] NOT NULL,
	[ExSyncIntval] [int] NOT NULL,
	[ExSyncMinH] [int] NOT NULL,
	[ExSyncMinL] [int] NOT NULL,
	[DummyIntval] [int] NOT NULL,
	[ShdOutput] [bit] NOT NULL,
	[CamsetNumSide] [int] NOT NULL,
	[AVALIniFile] [varchar](256) NULL,
	[FixOutputImageSize] [bit] NOT NULL,
	[FixSizeX] [int] NOT NULL,
	[FixSizeY] [int] NOT NULL,
	[MinImageSizeX] [int] NOT NULL,
	[MinImageSizeY] [int] NOT NULL,
	[MaxImageSizeX] [int] NOT NULL,
	[MaxImageSizeY] [int] NOT NULL,
	[CropStepX] [int] NOT NULL,
	[CropStepY] [int] NOT NULL,
	[CropMaxContrast] [bit] NOT NULL,
	[ColorDiffRange] [int] NOT NULL,
	[SendInfoOnlyNum] [int] NOT NULL,
	[SendAreaPriorNum] [int] NOT NULL,
	[MaxFeatcalcNum] [int] NOT NULL,
	[SendDensityIPAddr] [varchar](256) NOT NULL,
	[SendDensityPort] [int] NOT NULL,
	[EdgeRpcPort] [int] NOT NULL,
	[EdgeRestrictPort] [int] NOT NULL,
	[ShFrameShrinkX] [int] NOT NULL,
	[ShFrameShrinkY] [int] NOT NULL,
	[ResetExpTimeOnSameScode] [bit] NOT NULL,
	[AGCstep] [int] NOT NULL,
	[AGCIntval] [int] NOT NULL,
	[AGCValidWidth] [int] NOT NULL,
	[LocalDensityGridX] [int] NOT NULL,
	[LocalDensityGridY] [int] NOT NULL,
	[LocalDensityMaxArea] [int] NOT NULL,
	[DdsFreeSpaceGB] [int] NOT NULL,
	[DdsName_EdasysID] [int] NOT NULL,
	[DdsExportNumLimit] [int] NOT NULL,
	[DdsExportTypeLimit] [int] NOT NULL,
	[DownstreamIPAddr] [varchar](32) NOT NULL,
	[DownstreamPort] [int] NOT NULL,
	[TopBotSearchX] [float] NOT NULL,
	[TopBotSearchY] [float] NOT NULL,
	[PeriodHistThr1] [int] NOT NULL,
	[PeriodHistThr2] [int] NOT NULL,
	[PeriodHistThr3] [int] NOT NULL,
	[PeriodHistThr4] [int] NOT NULL,
	[PeriodHistThr5] [int] NOT NULL,
	[PeriodHistThr6] [int] NOT NULL,
	[PeriodHistThr7] [int] NOT NULL,
	[PeriodHistThr8] [int] NOT NULL,
	[PeriodLpitch1] [float] NOT NULL,
	[PeriodLpitch2] [float] NOT NULL,
	[PeriodLpitch3] [float] NOT NULL,
	[PeriodLpitch4] [float] NOT NULL,
	[PeriodLpitch5] [float] NOT NULL,
	[PeriodLpitch6] [float] NOT NULL,
	[PeriodLpitch7] [float] NOT NULL,
	[PeriodLpitch8] [float] NOT NULL,
	[PeriodNthr1] [int] NOT NULL,
	[PeriodNthr2] [int] NOT NULL,
	[PeriodNthr3] [int] NOT NULL,
	[PeriodNthr4] [int] NOT NULL,
	[PeriodNthr5] [int] NOT NULL,
	[PeriodNthr6] [int] NOT NULL,
	[PeriodNthr7] [int] NOT NULL,
	[PeriodNthr8] [int] NOT NULL,
	[PeriodWpitch1] [int] NOT NULL,
	[PeriodWpitch2] [int] NOT NULL,
	[PeriodWpitch3] [int] NOT NULL,
	[PeriodWpitch4] [int] NOT NULL,
	[PeriodWpitch5] [int] NOT NULL,
	[PeriodWpitch6] [int] NOT NULL,
	[PeriodWpitch7] [int] NOT NULL,
	[PeriodWpitch8] [int] NOT NULL,
	[PeriodMin1] [float] NOT NULL,
	[PeriodMin2] [float] NOT NULL,
	[PeriodMin3] [float] NOT NULL,
	[PeriodMin4] [float] NOT NULL,
	[PeriodMin5] [float] NOT NULL,
	[PeriodMin6] [float] NOT NULL,
	[PeriodMin7] [float] NOT NULL,
	[PeriodMin8] [float] NOT NULL,
	[PeriodMax1] [float] NOT NULL,
	[PeriodMax2] [float] NOT NULL,
	[PeriodMax3] [float] NOT NULL,
	[PeriodMax4] [float] NOT NULL,
	[PeriodMax5] [float] NOT NULL,
	[PeriodMax6] [float] NOT NULL,
	[PeriodMax7] [float] NOT NULL,
	[PeriodMax8] [float] NOT NULL,
	[PeriodKeepLen1] [int] NOT NULL,
	[PeriodKeepLen2] [int] NOT NULL,
	[PeriodKeepLen3] [int] NOT NULL,
	[PeriodKeepLen4] [int] NOT NULL,
	[PeriodKeepLen5] [int] NOT NULL,
	[PeriodKeepLen6] [int] NOT NULL,
	[PeriodKeepLen7] [int] NOT NULL,
	[PeriodKeepLen8] [int] NOT NULL,
	[PeriodWeight1] [float] NOT NULL,
	[PeriodWeight2] [float] NOT NULL,
	[PeriodWeight3] [float] NOT NULL,
	[PeriodWeight4] [float] NOT NULL,
	[PeriodWeight5] [float] NOT NULL,
	[PeriodWeight6] [float] NOT NULL,
	[PeriodWeight7] [float] NOT NULL,
	[PeriodWeight8] [float] NOT NULL,
	[ConnectMaxNFrames] [int] NOT NULL,
	[ConnectMinLength] [int] NOT NULL,
	[ConnectMaxWidth] [int] NOT NULL,
	[ConnectMaxGap] [int] NOT NULL,
	[DLBatchSize] [int] NOT NULL,
	[DLModelName] [varchar](256) NOT NULL,
	[DLDCDName] [varchar](256) NOT NULL,
 CONSTRAINT [PK_PARAM_COMMON] PRIMARY KEY CLUSTERED 
(
	[Key] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_CROP_SHRINK]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PARAM_CROP_SHRINK](
	[Order] [int] NOT NULL,
	[Width] [int] NOT NULL,
	[Height] [int] NOT NULL,
	[ShrinkRatio] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_CROP_SHRINK] PRIMARY KEY CLUSTERED 
(
	[Order] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[PARAM_CYCLE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[PARAM_CYCLE](
	[id] [int] NOT NULL,
	[PeriodHistThr] [int] NOT NULL,
	[PeriodLpitch] [float] NOT NULL,
	[PeriodNthr] [int] NOT NULL,
	[PeriodWpitch] [int] NOT NULL,
	[PeriodMin] [float] NOT NULL,
	[PeriodMax] [float] NOT NULL,
	[PeriodKeepLen] [int] NOT NULL,
	[PeriodWeight] [float] NOT NULL,
	[PeriodFrontEnableLen] [int] NOT NULL,
	[PeriodTailEnableLen] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_CYCLE] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[PARAM_FILTER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_FILTER](
	[id] [int] NOT NULL,
	[name] [varchar](256) NOT NULL,
	[SizeX] [int] NOT NULL,
	[SizeY] [int] NOT NULL,
	[Offset] [real] NOT NULL,
	[NormalizeCoef] [real] NOT NULL,
	[Coef11] [real] NOT NULL,
	[Coef12] [real] NOT NULL,
	[Coef13] [real] NOT NULL,
	[Coef14] [real] NOT NULL,
	[Coef15] [real] NOT NULL,
	[Coef16] [real] NOT NULL,
	[Coef17] [real] NOT NULL,
	[Coef21] [real] NOT NULL,
	[Coef22] [real] NOT NULL,
	[Coef23] [real] NOT NULL,
	[Coef24] [real] NOT NULL,
	[Coef25] [real] NOT NULL,
	[Coef26] [real] NOT NULL,
	[Coef27] [real] NOT NULL,
	[Coef31] [real] NOT NULL,
	[Coef32] [real] NOT NULL,
	[Coef33] [real] NOT NULL,
	[Coef34] [real] NOT NULL,
	[Coef35] [real] NOT NULL,
	[Coef36] [real] NOT NULL,
	[Coef37] [real] NOT NULL,
	[Coef41] [real] NOT NULL,
	[Coef42] [real] NOT NULL,
	[Coef43] [real] NOT NULL,
	[Coef44] [real] NOT NULL,
	[Coef45] [real] NOT NULL,
	[Coef46] [real] NOT NULL,
	[Coef47] [real] NOT NULL,
	[Coef51] [real] NOT NULL,
	[Coef52] [real] NOT NULL,
	[Coef53] [real] NOT NULL,
	[Coef54] [real] NOT NULL,
	[Coef55] [real] NOT NULL,
	[Coef56] [real] NOT NULL,
	[Coef57] [real] NOT NULL,
	[Coef61] [real] NOT NULL,
	[Coef62] [real] NOT NULL,
	[Coef63] [real] NOT NULL,
	[Coef64] [real] NOT NULL,
	[Coef65] [real] NOT NULL,
	[Coef66] [real] NOT NULL,
	[Coef67] [real] NOT NULL,
	[Coef71] [real] NOT NULL,
	[Coef72] [real] NOT NULL,
	[Coef73] [real] NOT NULL,
	[Coef74] [real] NOT NULL,
	[Coef75] [real] NOT NULL,
	[Coef76] [real] NOT NULL,
	[Coef77] [real] NOT NULL,
 CONSTRAINT [PK_PARAM_FILTER] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_FILTERBIN]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_FILTERBIN](
	[scode] [int] NOT NULL,
	[id_filterbin] [int] NOT NULL,
	[camangle] [int] NOT NULL,
	[ReductionId] [int] NOT NULL,
	[FilterId] [int] NOT NULL,
	[UpperThreshold] [real] NOT NULL,
	[LowerThreshold] [real] NOT NULL,
	[UpperLimit] [real] NOT NULL,
	[LowerLimit] [real] NOT NULL,
	[EdgeDeadband] [int] NOT NULL,
	[name] [varchar](256) NOT NULL,
	[NearEdgeApplyWidth] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_FILTERBIN] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[id_filterbin] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_LABELIMAGE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_LABELIMAGE](
	[scode] [int] NOT NULL,
	[id] [int] NOT NULL,
	[SimpleBin1] [varchar](1) NOT NULL,
	[SimpleBin2] [varchar](1) NOT NULL,
	[SimpleBin3] [varchar](1) NOT NULL,
	[SimpleBin4] [varchar](1) NOT NULL,
	[SimpleBin5] [varchar](1) NOT NULL,
	[SimpleBin6] [varchar](1) NOT NULL,
	[SimpleBin7] [varchar](1) NOT NULL,
	[SimpleBin8] [varchar](1) NOT NULL,
	[SimpleBin9] [varchar](1) NOT NULL,
	[SimpleBin10] [varchar](1) NOT NULL,
	[SimpleBin11] [varchar](1) NOT NULL,
	[SimpleBin12] [varchar](1) NOT NULL,
	[SimpleBin13] [varchar](1) NOT NULL,
	[SimpleBin14] [varchar](1) NOT NULL,
	[SimpleBin15] [varchar](1) NOT NULL,
	[SimpleBin16] [varchar](1) NOT NULL,
	[FilterBin1] [varchar](1) NOT NULL,
	[FilterBin2] [varchar](1) NOT NULL,
	[FilterBin3] [varchar](1) NOT NULL,
	[FilterBin4] [varchar](1) NOT NULL,
	[FilterBin5] [varchar](1) NOT NULL,
	[FilterBin6] [varchar](1) NOT NULL,
	[FilterBin7] [varchar](1) NOT NULL,
	[FilterBin8] [varchar](1) NOT NULL,
	[FilterBin9] [varchar](1) NOT NULL,
	[FilterBin10] [varchar](1) NOT NULL,
	[FilterBin11] [varchar](1) NOT NULL,
	[FilterBin12] [varchar](1) NOT NULL,
	[FilterBin13] [varchar](1) NOT NULL,
	[FilterBin14] [varchar](1) NOT NULL,
	[FilterBin15] [varchar](1) NOT NULL,
	[FilterBin16] [varchar](1) NOT NULL,
	[Nopen] [int] NOT NULL,
	[Nclose] [int] NOT NULL,
	[bNearEdge] [bit] NOT NULL,
	[NearEdgeDeadBand] [int] NOT NULL,
	[NearEdgeApplyWidth] [int] NOT NULL,
	[name] [varchar](256) NOT NULL,
 CONSTRAINT [PK_PARAM_LABELIMAGE] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_REDUCTION]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_REDUCTION](
	[id] [int] NOT NULL,
	[name] [varchar](256) NOT NULL,
	[MeanX] [int] NOT NULL,
	[MeanY] [int] NOT NULL,
	[RatioX] [int] NOT NULL,
	[RatioY] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_REDUCTION] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_SCODE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_SCODE](
	[scode] [int] NOT NULL,
	[EdgeDeadband] [int] NOT NULL,
	[ShdEdgeOffset_L] [int] NOT NULL,
	[ShdEdgeWidth_L] [int] NOT NULL,
	[ShdEdgeOffset_R] [int] NOT NULL,
	[ShdEdgeWidth_R] [int] NOT NULL,
	[ShdEdgeEmbedMedianSize] [int] NOT NULL,
	[Nopen] [int] NOT NULL,
	[Nclose] [int] NOT NULL,
	[DCDname] [varchar](256) NOT NULL,
	[DCDname2nd] [varchar](256) NOT NULL,
	[AreaFilterThr] [int] NOT NULL,
	[EllipticConnectAreaThr] [int] NOT NULL,
	[EllipticConnectExpand1] [float] NOT NULL,
	[EllipticConnectExpand2] [float] NOT NULL,
	[BinProjConnectAreaThr] [int] NOT NULL,
	[BinProjConnectProjThrX] [int] NOT NULL,
	[BinProjConnectExpandX] [int] NOT NULL,
	[BinProjConnectProjThrY] [int] NOT NULL,
	[BinProjConnectExpandY] [int] NOT NULL,
	[DensNBin] [int] NOT NULL,
	[DensNFrame] [int] NOT NULL,
	[照明照度_表] [int] NOT NULL,
	[照明照度2_表] [int] NOT NULL,
	[照明照度3_表] [int] NOT NULL,
	[照明照度4_表] [int] NOT NULL,
	[照明照度5_表] [int] NOT NULL,
	[照明照度_裏] [int] NOT NULL,
	[照明照度2_裏] [int] NOT NULL,
	[照明照度3_裏] [int] NOT NULL,
	[照明照度4_裏] [int] NOT NULL,
	[照明照度5_裏] [int] NOT NULL,
	[照明照度補正値_表] [varbinary](48) NOT NULL,
	[照明照度補正値2_表] [varbinary](48) NOT NULL,
	[照明照度補正値3_表] [varbinary](48) NOT NULL,
	[照明照度補正値4_表] [varbinary](48) NOT NULL,
	[照明照度補正値5_表] [varbinary](48) NOT NULL,
	[照明照度補正値_裏] [varbinary](48) NOT NULL,
	[照明照度補正値2_裏] [varbinary](48) NOT NULL,
	[照明照度補正値3_裏] [varbinary](48) NOT NULL,
	[照明照度補正値4_裏] [varbinary](48) NOT NULL,
	[照明照度補正値5_裏] [varbinary](48) NOT NULL,
	[PdNopen] [int] NOT NULL,
	[PdNclose] [int] NOT NULL,
	[PdAreaFilterThr] [int] NOT NULL,
	[PdDCDName] [varchar](256) NOT NULL,
	[PdEllipticConnectAreaThr] [int] NOT NULL,
	[PdEllipticConnectExpand1] [float] NOT NULL,
	[PdEllipticConnectExpand2] [float] NOT NULL,
	[DdsDoExport] [int] NOT NULL,
	[NearEdgeDeadband] [int] NOT NULL,
	[NearEdgeApplyWidth] [int] NOT NULL,
	[NearEdgeNopen] [int] NOT NULL,
	[NearEdgeNclose] [int] NOT NULL,
	[NearEdgeNAreaFilterThr] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_SCODE] PRIMARY KEY CLUSTERED 
(
	[scode] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_SIMPLEBIN]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_SIMPLEBIN](
	[scode] [int] NOT NULL,
	[id_simplebin] [int] NOT NULL,
	[camangle] [int] NOT NULL,
	[UpperThreshold] [int] NOT NULL,
	[LowerThreshold] [int] NOT NULL,
	[Nopen] [int] NOT NULL,
	[Nclose] [int] NOT NULL,
	[name] [varchar](256) NOT NULL,
 CONSTRAINT [PK_PARAM_SIMPLEBIN] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[id_simplebin] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PARAM_THRESHOLD]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PARAM_THRESHOLD](
	[scode] [int] NOT NULL,
	[camangle] [int] NOT NULL,
	[id] [int] NOT NULL,
	[ReductionId] [int] NOT NULL,
	[FilterId] [int] NOT NULL,
	[UpperThreshold] [real] NOT NULL,
	[LowerThreshold] [real] NOT NULL,
	[UpperLimit] [real] NOT NULL,
	[LowerLimit] [real] NOT NULL,
	[name] [varchar](256) NOT NULL,
	[EdgeDeadband] [int] NOT NULL,
 CONSTRAINT [PK_PARAM_THRESHOLD] PRIMARY KEY CLUSTERED 
(
	[scode] ASC,
	[camangle] ASC,
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[PCINFO_DESC]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[PCINFO_DESC](
	[Order] [int] NOT NULL,
	[Name] [varchar](64) NOT NULL,
	[Size] [int] NOT NULL,
	[EdasysID] [int] NOT NULL,
	[IsAscii] [bit] NOT NULL,
	[IsNum] [bit] NOT NULL,
	[ConvCoef] [float] NOT NULL,
	[CoilInfColName] [varchar](64) NULL,
 CONSTRAINT [PK_PCINFO_DESC] PRIMARY KEY CLUSTERED 
(
	[Order] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[ROLL_INFO]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[ROLL_INFO](
	[Num] [int] NOT NULL,
	[Side] [int] NOT NULL,
	[PdEvalCamAngle] [int] NOT NULL,
	[PdEvalMethod] [int] NOT NULL,
	[Diam_EdasysID] [int] NOT NULL,
	[ExtRatio_EdasysID] [int] NOT NULL,
	[RndLen_EdasysID] [int] NOT NULL,
	[Diameter] [float] NOT NULL,
	[MaxDiameter] [float] NOT NULL,
	[Offset1] [float] NOT NULL,
	[Coef] [float] NOT NULL,
	[NPatterns] [int] NOT NULL,
	[PitchDelta] [int] NOT NULL,
	[NAdd] [int] NOT NULL,
	[NUnits] [int] NOT NULL,
 CONSTRAINT [PK_ROLL_INFO] PRIMARY KEY CLUSTERED 
(
	[Num] ASC,
	[Side] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[T_ACCESS_NEXT_COIL_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_ACCESS_NEXT_COIL_INF](
	[管理No] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[事前編集済みフラグ] [int] NOT NULL,
	[次工程取得済みフラグ] [int] NOT NULL,
	[コイル長] [int] NOT NULL,
	[データ設定数_表] [int] NOT NULL,
	[データ設定数_裏] [int] NOT NULL,
	[疵情報数_表] [int] NOT NULL,
	[疵情報数_裏] [int] NOT NULL,
	[事前編集_PC名称] [varchar](64) NULL,
	[事前編集_ユーザー名] [varchar](64) NULL,
	[事前編集_IPアドレス] [varchar](64) NULL,
	[事前編集_RCL疵検管理No] [varchar](20) NULL,
	[事前編集_RCLラインID] [int] NULL,
	[事前編集_コイルNo] [varchar](12) NULL,
	[事前編集_登録日付] [datetime] NULL,
 CONSTRAINT [PK_T_ACCESS_NEXT_COIL_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_ACCESS_NEXT_DEFECT_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_ACCESS_NEXT_DEFECT_INF](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[事前編集フラグ] [int] NOT NULL,
	[補正後長手位置] [int] NOT NULL,
	[判定疵種] [int] NULL,
	[判定グレード] [int] NULL,
 CONSTRAINT [PK_T_ACCESS_NEXT_DEFECT_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_CAMERA]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_CAMERA](
	[管理No] [varchar](20) NOT NULL,
	[カメラ1_輝度R_C] [tinyint] NOT NULL,
	[カメラ1_輝度G_C] [tinyint] NOT NULL,
	[カメラ1_輝度B_C] [tinyint] NOT NULL,
	[カメラ2_輝度R_C] [tinyint] NOT NULL,
	[カメラ2_輝度G_C] [tinyint] NOT NULL,
	[カメラ2_輝度B_C] [tinyint] NOT NULL,
	[カメラ3_輝度R_C] [tinyint] NOT NULL,
	[カメラ3_輝度G_C] [tinyint] NOT NULL,
	[カメラ3_輝度B_C] [tinyint] NOT NULL,
	[カメラ4_輝度R_C] [tinyint] NOT NULL,
	[カメラ4_輝度G_C] [tinyint] NOT NULL,
	[カメラ4_輝度B_C] [tinyint] NOT NULL,
	[カメラ5_輝度R_C] [tinyint] NOT NULL,
	[カメラ5_輝度G_C] [tinyint] NOT NULL,
	[カメラ5_輝度B_C] [tinyint] NOT NULL,
	[カメラ6_輝度R_C] [tinyint] NOT NULL,
	[カメラ6_輝度G_C] [tinyint] NOT NULL,
	[カメラ6_輝度B_C] [tinyint] NOT NULL,
	[カメラ7_輝度R_C] [tinyint] NOT NULL,
	[カメラ7_輝度G_C] [tinyint] NOT NULL,
	[カメラ7_輝度B_C] [tinyint] NOT NULL,
	[カメラ8_輝度R_C] [tinyint] NOT NULL,
	[カメラ8_輝度G_C] [tinyint] NOT NULL,
	[カメラ8_輝度B_C] [tinyint] NOT NULL,
	[カメラ9_輝度R_C] [tinyint] NOT NULL,
	[カメラ9_輝度G_C] [tinyint] NOT NULL,
	[カメラ9_輝度B_C] [tinyint] NOT NULL,
	[カメラ10_輝度R_C] [tinyint] NOT NULL,
	[カメラ10_輝度G_C] [tinyint] NOT NULL,
	[カメラ10_輝度B_C] [tinyint] NOT NULL,
	[カメラ11_輝度R_C] [tinyint] NOT NULL,
	[カメラ11_輝度G_C] [tinyint] NOT NULL,
	[カメラ11_輝度B_C] [tinyint] NOT NULL,
	[カメラ12_輝度R_C] [tinyint] NOT NULL,
	[カメラ12_輝度G_C] [tinyint] NOT NULL,
	[カメラ12_輝度B_C] [tinyint] NOT NULL,
	[カメラ13_輝度R_C] [tinyint] NOT NULL,
	[カメラ13_輝度G_C] [tinyint] NOT NULL,
	[カメラ13_輝度B_C] [tinyint] NOT NULL,
	[カメラ14_輝度R_C] [tinyint] NOT NULL,
	[カメラ14_輝度G_C] [tinyint] NOT NULL,
	[カメラ14_輝度B_C] [tinyint] NOT NULL,
	[カメラ15_輝度R_C] [tinyint] NOT NULL,
	[カメラ15_輝度G_C] [tinyint] NOT NULL,
	[カメラ15_輝度B_C] [tinyint] NOT NULL,
	[カメラ16_輝度R_C] [tinyint] NOT NULL,
	[カメラ16_輝度G_C] [tinyint] NOT NULL,
	[カメラ16_輝度B_C] [tinyint] NOT NULL,
	[カメラ17_輝度R_C] [tinyint] NOT NULL,
	[カメラ17_輝度G_C] [tinyint] NOT NULL,
	[カメラ17_輝度B_C] [tinyint] NOT NULL,
	[カメラ18_輝度R_C] [tinyint] NOT NULL,
	[カメラ18_輝度G_C] [tinyint] NOT NULL,
	[カメラ18_輝度B_C] [tinyint] NOT NULL,
	[カメラ19_輝度R_C] [tinyint] NOT NULL,
	[カメラ19_輝度G_C] [tinyint] NOT NULL,
	[カメラ19_輝度B_C] [tinyint] NOT NULL,
	[カメラ20_輝度R_C] [tinyint] NOT NULL,
	[カメラ20_輝度G_C] [tinyint] NOT NULL,
	[カメラ20_輝度B_C] [tinyint] NOT NULL,
	[露光時間R_1] [int] NOT NULL,
	[露光時間G_1] [int] NOT NULL,
	[露光時間B_1] [int] NOT NULL,
	[露光時間R_2] [int] NOT NULL,
	[露光時間G_2] [int] NOT NULL,
	[露光時間B_2] [int] NOT NULL,
	[露光時間R_3] [int] NOT NULL,
	[露光時間G_3] [int] NOT NULL,
	[露光時間B_3] [int] NOT NULL,
	[露光時間R_4] [int] NOT NULL,
	[露光時間G_4] [int] NOT NULL,
	[露光時間B_4] [int] NOT NULL,
	[露光時間R_5] [int] NOT NULL,
	[露光時間G_5] [int] NOT NULL,
	[露光時間B_5] [int] NOT NULL,
	[露光時間R_6] [int] NOT NULL,
	[露光時間G_6] [int] NOT NULL,
	[露光時間B_6] [int] NOT NULL,
	[露光時間R_7] [int] NOT NULL,
	[露光時間G_7] [int] NOT NULL,
	[露光時間B_7] [int] NOT NULL,
	[露光時間R_8] [int] NOT NULL,
	[露光時間G_8] [int] NOT NULL,
	[露光時間B_8] [int] NOT NULL,
	[露光時間R_9] [int] NOT NULL,
	[露光時間G_9] [int] NOT NULL,
	[露光時間B_9] [int] NOT NULL,
	[露光時間R_10] [int] NOT NULL,
	[露光時間G_10] [int] NOT NULL,
	[露光時間B_10] [int] NOT NULL,
	[露光時間R_11] [int] NOT NULL,
	[露光時間G_11] [int] NOT NULL,
	[露光時間B_11] [int] NOT NULL,
	[露光時間R_12] [int] NOT NULL,
	[露光時間G_12] [int] NOT NULL,
	[露光時間B_12] [int] NOT NULL,
	[露光時間R_13] [int] NOT NULL,
	[露光時間G_13] [int] NOT NULL,
	[露光時間B_13] [int] NOT NULL,
	[露光時間R_14] [int] NOT NULL,
	[露光時間G_14] [int] NOT NULL,
	[露光時間B_14] [int] NOT NULL,
	[露光時間R_15] [int] NOT NULL,
	[露光時間G_15] [int] NOT NULL,
	[露光時間B_15] [int] NOT NULL,
	[露光時間R_16] [int] NOT NULL,
	[露光時間G_16] [int] NOT NULL,
	[露光時間B_16] [int] NOT NULL,
	[露光時間R_17] [int] NOT NULL,
	[露光時間G_17] [int] NOT NULL,
	[露光時間B_17] [int] NOT NULL,
	[露光時間R_18] [int] NOT NULL,
	[露光時間G_18] [int] NOT NULL,
	[露光時間B_18] [int] NOT NULL,
	[露光時間R_19] [int] NOT NULL,
	[露光時間G_19] [int] NOT NULL,
	[露光時間B_19] [int] NOT NULL,
	[露光時間R_20] [int] NOT NULL,
	[露光時間G_20] [int] NOT NULL,
	[露光時間B_20] [int] NOT NULL,
	[カメラ1_ゲインR_C] [float] NOT NULL,
	[カメラ1_ゲインG_C] [float] NOT NULL,
	[カメラ1_ゲインB_C] [float] NOT NULL,
	[カメラ2_ゲインR_C] [float] NOT NULL,
	[カメラ2_ゲインG_C] [float] NOT NULL,
	[カメラ2_ゲインB_C] [float] NOT NULL,
	[カメラ3_ゲインR_C] [float] NOT NULL,
	[カメラ3_ゲインG_C] [float] NOT NULL,
	[カメラ3_ゲインB_C] [float] NOT NULL,
	[カメラ4_ゲインR_C] [float] NOT NULL,
	[カメラ4_ゲインG_C] [float] NOT NULL,
	[カメラ4_ゲインB_C] [float] NOT NULL,
	[カメラ5_ゲインR_C] [float] NOT NULL,
	[カメラ5_ゲインG_C] [float] NOT NULL,
	[カメラ5_ゲインB_C] [float] NOT NULL,
	[カメラ6_ゲインR_C] [float] NOT NULL,
	[カメラ6_ゲインG_C] [float] NOT NULL,
	[カメラ6_ゲインB_C] [float] NOT NULL,
	[カメラ7_ゲインR_C] [float] NOT NULL,
	[カメラ7_ゲインG_C] [float] NOT NULL,
	[カメラ7_ゲインB_C] [float] NOT NULL,
	[カメラ8_ゲインR_C] [float] NOT NULL,
	[カメラ8_ゲインG_C] [float] NOT NULL,
	[カメラ8_ゲインB_C] [float] NOT NULL,
	[カメラ9_ゲインR_C] [float] NOT NULL,
	[カメラ9_ゲインG_C] [float] NOT NULL,
	[カメラ9_ゲインB_C] [float] NOT NULL,
	[カメラ10_ゲインR_C] [float] NOT NULL,
	[カメラ10_ゲインG_C] [float] NOT NULL,
	[カメラ10_ゲインB_C] [float] NOT NULL,
	[カメラ11_ゲインR_C] [float] NOT NULL,
	[カメラ11_ゲインG_C] [float] NOT NULL,
	[カメラ11_ゲインB_C] [float] NOT NULL,
	[カメラ12_ゲインR_C] [float] NOT NULL,
	[カメラ12_ゲインG_C] [float] NOT NULL,
	[カメラ12_ゲインB_C] [float] NOT NULL,
	[カメラ13_ゲインR_C] [float] NOT NULL,
	[カメラ13_ゲインG_C] [float] NOT NULL,
	[カメラ13_ゲインB_C] [float] NOT NULL,
	[カメラ14_ゲインR_C] [float] NOT NULL,
	[カメラ14_ゲインG_C] [float] NOT NULL,
	[カメラ14_ゲインB_C] [float] NOT NULL,
	[カメラ15_ゲインR_C] [float] NOT NULL,
	[カメラ15_ゲインG_C] [float] NOT NULL,
	[カメラ15_ゲインB_C] [float] NOT NULL,
	[カメラ16_ゲインR_C] [float] NOT NULL,
	[カメラ16_ゲインG_C] [float] NOT NULL,
	[カメラ16_ゲインB_C] [float] NOT NULL,
	[カメラ17_ゲインR_C] [float] NOT NULL,
	[カメラ17_ゲインG_C] [float] NOT NULL,
	[カメラ17_ゲインB_C] [float] NOT NULL,
	[カメラ18_ゲインR_C] [float] NOT NULL,
	[カメラ18_ゲインG_C] [float] NOT NULL,
	[カメラ18_ゲインB_C] [float] NOT NULL,
	[カメラ19_ゲインR_C] [float] NOT NULL,
	[カメラ19_ゲインG_C] [float] NOT NULL,
	[カメラ19_ゲインB_C] [float] NOT NULL,
	[カメラ20_ゲインR_C] [float] NOT NULL,
	[カメラ20_ゲインG_C] [float] NOT NULL,
	[カメラ20_ゲインB_C] [float] NOT NULL,
 CONSTRAINT [PK_T_COIL_CAMERA] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_CUT_POS]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_CUT_POS](
	[管理No] [varchar](20) NOT NULL,
	[入側子番] [int] NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[ID] [varchar](20) NOT NULL,
	[No] [int] NOT NULL,
	[区分] [int] NOT NULL,
	[先頭位置] [int] NOT NULL,
	[保有長さ] [int] NOT NULL,
	[出側距離位置] [int] NOT NULL,
 CONSTRAINT [PK_T_COIL_CUT_POS] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[入側子番] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_CUT_RESULT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_CUT_RESULT](
	[管理No] [varchar](20) NOT NULL,
	[カットNo] [int] NOT NULL,
	[ID] [varchar](20) NOT NULL,
	[命令No] [varchar](4) NOT NULL,
	[生産No] [varchar](12) NOT NULL,
	[カット位置] [int] NOT NULL,
 CONSTRAINT [PK_T_COIL_CUT_RESULT] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[カットNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_INF](
	[管理No] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[キー情報] [varchar](20) NULL,
	[命令No] [varchar](4) NULL,
	[生産No] [varchar](12) NULL,
	[送信位置] [int] NULL,
	[コイル長] [int] NULL,
	[材料板厚] [int] NULL,
	[材料板幅] [int] NULL,
	[注文板厚] [int] NULL,
	[注文板幅] [int] NULL,
	[現工程トリム幅] [int] NULL,
	[SPM伸び率] [int] NULL,
	[TLV伸び率] [int] NULL,
	[入側フロント端板長さ] [int] NULL,
	[入側テール端板長さ] [int] NULL,
	[SPMワークロール径上] [int] NULL,
	[SPMワークロール径下] [int] NULL,
	[予備_Int_10] [int] NULL,
	[予備_Int_9] [int] NULL,
	[予備_Int_8] [int] NULL,
	[予備_Int_7] [int] NULL,
	[予備_Int_6] [int] NULL,
	[予備_Int_5] [int] NULL,
	[予備_Int_4] [int] NULL,
	[予備_Int_3] [int] NULL,
	[予備_Int_2] [int] NULL,
	[予備_Int_1] [int] NULL,
	[検査基準_表] [varchar](4) NULL,
	[検査基準_裏] [varchar](4) NULL,
	[クロメート命令_種類] [varchar](2) NULL,
	[クロメート命令_有無] [varchar](2) NULL,
	[品種] [varchar](1) NULL,
	[メッキ種類] [varchar](1) NULL,
	[主材質] [varchar](2) NULL,
	[ディファレンシャルマーク] [varchar](1) NULL,
	[原板区分] [varchar](1) NULL,
	[メッキサイクル] [varchar](1) NULL,
	[レベラー_インラインSPM] [varchar](1) NULL,
	[化学処理塗油] [varchar](1) NULL,
	[オフラインSPM圧下率] [varchar](1) NULL,
	[メッキ区分] [varchar](1) NULL,
	[表面処理1桁目] [varchar](1) NULL,
	[表面処理2桁目] [varchar](1) NULL,
	[塗油] [varchar](1) NULL,
	[工程記号予備] [varchar](1) NULL,
	[予備コイル情報] [varchar](1) NULL,
	[CAPL主材質] [varchar](10) NULL,
	[目付規格] [varchar](8) NULL,
	[Ni付着量] [varchar](2) NULL,
	[TCMロール粗度区分] [varchar](2) NULL,
	[SPM圧下有無] [varchar](2) NULL,
	[計画次工程コード] [varchar](2) NULL,
	[向先] [varchar](32) NULL,
	[用途] [varchar](4) NULL,
	[CAPL表面仕上げ] [varchar](4) NULL,
	[CAPL酸洗有無] [varchar](2) NULL,
	[品種区分] [varchar](1) NULL,
	[GA_CR区分] [varchar](1) NULL,
	[SPM圧下実績有無] [varchar](2) NULL,
	[工程コード] [varchar](2) NULL,
	[予備_Char_10] [varchar](16) NULL,
	[予備_Char_9] [varchar](16) NULL,
	[予備_Char_8] [varchar](16) NULL,
	[予備_Char_7] [varchar](16) NULL,
	[予備_Char_6] [varchar](16) NULL,
	[予備_Char_5] [varchar](16) NULL,
	[予備_Char_4] [varchar](16) NULL,
	[予備_Char_3] [varchar](16) NULL,
	[予備_Char_2] [varchar](16) NULL,
	[予備_Char_1] [varchar](16) NULL,
 CONSTRAINT [PK_T_COIL_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_ORDER]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_ORDER](
	[管理No] [varchar](20) NOT NULL,
	[通順] [int] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[入側フロント端板長さ] [int] NOT NULL,
	[前工程コード] [varchar](2) NOT NULL,
 CONSTRAINT [PK_T_COIL_ORDER] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[通順] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_POSITION]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_POSITION](
	[管理No] [varchar](20) NOT NULL,
	[ID] [int] NOT NULL,
	[検査状態_表] [smallint] NOT NULL,
	[検査状態_裏] [smallint] NOT NULL,
	[機器状態_表] [smallint] NOT NULL,
	[機器状態_裏] [smallint] NOT NULL,
	[エッジ位置生左] [real] NOT NULL,
	[エッジ位置生右] [real] NOT NULL,
	[ウォーク量] [real] NOT NULL,
 CONSTRAINT [PK_T_COIL_POSITION] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_RESULT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_RESULT](
	[管理No] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[実績格納区分] [int] NOT NULL,
	[判定状態_表] [int] NOT NULL,
	[判定状態_裏] [int] NOT NULL,
	[照明装置状態_表] [int] NOT NULL,
	[照明装置状態_裏] [int] NOT NULL,
	[カメラコントロール回路状態] [int] NOT NULL,
	[カメラリンクスイッチ回路状態] [int] NOT NULL,
	[プロコン状態] [int] NOT NULL,
	[FF状態] [int] NOT NULL,
	[シーケンサ状態] [int] NOT NULL,
	[カメラ台車状態_表] [int] NOT NULL,
	[カメラ台車状態_裏] [int] NOT NULL,
	[未保存_表] [int] NOT NULL,
	[未保存_裏] [int] NOT NULL,
	[未検査_表] [int] NOT NULL,
	[未検査_裏] [int] NOT NULL,
	[使用判定PC_表] [varchar](20) NOT NULL,
	[使用判定PC_裏] [varchar](20) NOT NULL,
	[使用全長処理PC_表] [varchar](4) NOT NULL,
	[使用全長処理PC_裏] [varchar](4) NOT NULL,
	[検査基点信号検出可否] [int] NOT NULL,
	[無害Gr削除区分] [int] NOT NULL,
	[統合サーバー取得区分] [int] NOT NULL,
	[ウォーマー材区分] [int] NOT NULL,
	[検査本数] [int] NOT NULL,
	[機器状態予備_10] [int] NULL,
	[機器状態予備_9] [int] NULL,
	[機器状態予備_8] [int] NULL,
	[機器状態予備_7] [int] NULL,
	[機器状態予備_6] [int] NULL,
	[機器状態予備_5] [int] NULL,
	[機器状態予備_4] [int] NULL,
	[機器状態予備_3] [int] NULL,
	[機器状態予備_2] [int] NULL,
	[機器状態予備_1] [int] NULL,
	[検査表面状態No_Def区分_表] [int] NOT NULL,
	[検査表面状態No_Def区分_裏] [int] NOT NULL,
	[検査パターンコード_Def区分_表] [int] NOT NULL,
	[検査パターンコード_Def区分_裏] [int] NOT NULL,
	[検査表面状態No_表] [int] NOT NULL,
	[検査表面状態No_裏] [int] NOT NULL,
	[検査パターンコード_表] [int] NOT NULL,
	[検査パターンコード_裏] [int] NOT NULL,
	[SPM圧下有無] [int] NOT NULL,
	[検査実測長] [int] NOT NULL,
	[検査比較実測長] [int] NULL,
	[表示実測長] [int] NOT NULL,
	[出側単位先端位置] [int] NOT NULL,
	[表保存対象疵個数] [int] NOT NULL,
	[裏保存対象疵個数] [int] NOT NULL,
	[表削除有害疵個数] [int] NOT NULL,
	[裏削除有害疵個数] [int] NOT NULL,
	[表削除無害疵個数] [int] NOT NULL,
	[裏削除無害疵個数] [int] NOT NULL,
	[表画像削除有害疵個数] [int] NOT NULL,
	[裏画像削除有害疵個数] [int] NOT NULL,
	[表画像削除無害疵個数] [int] NOT NULL,
	[裏画像削除無害疵個数] [int] NOT NULL,
	[表保存疵個数] [int] NOT NULL,
	[裏保存疵個数] [int] NOT NULL,
	[表画像保存疵個数] [int] NOT NULL,
	[裏画像保存疵個数] [int] NOT NULL,
	[運転状態] [int] NOT NULL,
	[検査状態_表] [int] NOT NULL,
	[検査状態_裏] [int] NOT NULL,
	[機器状態_表] [int] NOT NULL,
	[機器状態_裏] [int] NOT NULL,
	[PLG選択区分] [int] NOT NULL,
	[外部実績送信許可] [int] NOT NULL,
	[補正率] [float] NOT NULL,
	[プロコン実績反映] [int] NOT NULL,
	[プロコン実測長] [int] NOT NULL,
	[成品数] [int] NOT NULL,
	[塗油スプレー装置状態] [int] NOT NULL,
 CONSTRAINT [PK_T_COIL_RESULT] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_SAISUN_RESULT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_SAISUN_RESULT](
	[管理No] [varchar](20) NOT NULL,
	[採寸No] [int] NOT NULL,
	[ID] [varchar](20) NOT NULL,
	[命令No] [varchar](4) NOT NULL,
	[生産No] [varchar](12) NOT NULL,
	[採寸区分] [int] NOT NULL,
	[採寸開始位置] [int] NOT NULL,
	[採寸終了位置] [int] NOT NULL,
 CONSTRAINT [PK_T_COIL_SAISUN_RESULT] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[採寸No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_COIL_TRAND]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_COIL_TRAND](
	[管理No] [varchar](20) NOT NULL,
	[表裏区分] [tinyint] NOT NULL,
	[取得位置] [int] NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[ライン速度] [int] NOT NULL,
	[板幅Min] [real] NOT NULL,
	[板幅Max] [real] NOT NULL,
	[ウォーク量] [real] NOT NULL,
	[照明装置_調光値_1] [smallint] NOT NULL,
	[照明装置_調光値_2] [smallint] NOT NULL,
	[照明装置_調光値_3] [smallint] NOT NULL,
	[照明装置_調光値_4] [smallint] NOT NULL,
	[照明装置_調光値_5] [smallint] NOT NULL,
	[カメラ1_輝度R_C] [tinyint] NOT NULL,
	[カメラ1_輝度G_C] [tinyint] NOT NULL,
	[カメラ1_輝度B_C] [tinyint] NOT NULL,
	[カメラ2_輝度R_C] [tinyint] NOT NULL,
	[カメラ2_輝度G_C] [tinyint] NOT NULL,
	[カメラ2_輝度B_C] [tinyint] NOT NULL,
	[カメラ3_輝度R_C] [tinyint] NOT NULL,
	[カメラ3_輝度G_C] [tinyint] NOT NULL,
	[カメラ3_輝度B_C] [tinyint] NOT NULL,
	[カメラ4_輝度R_C] [tinyint] NOT NULL,
	[カメラ4_輝度G_C] [tinyint] NOT NULL,
	[カメラ4_輝度B_C] [tinyint] NOT NULL,
	[カメラ5_輝度R_C] [tinyint] NOT NULL,
	[カメラ5_輝度G_C] [tinyint] NOT NULL,
	[カメラ5_輝度B_C] [tinyint] NOT NULL,
	[カメラ6_輝度R_C] [tinyint] NOT NULL,
	[カメラ6_輝度G_C] [tinyint] NOT NULL,
	[カメラ6_輝度B_C] [tinyint] NOT NULL,
	[カメラ7_輝度R_C] [tinyint] NOT NULL,
	[カメラ7_輝度G_C] [tinyint] NOT NULL,
	[カメラ7_輝度B_C] [tinyint] NOT NULL,
	[カメラ8_輝度R_C] [tinyint] NOT NULL,
	[カメラ8_輝度G_C] [tinyint] NOT NULL,
	[カメラ8_輝度B_C] [tinyint] NOT NULL,
	[カメラ9_輝度R_C] [tinyint] NOT NULL,
	[カメラ9_輝度G_C] [tinyint] NOT NULL,
	[カメラ9_輝度B_C] [tinyint] NOT NULL,
	[カメラ10_輝度R_C] [tinyint] NOT NULL,
	[カメラ10_輝度G_C] [tinyint] NOT NULL,
	[カメラ10_輝度B_C] [tinyint] NOT NULL,
	[カメラ1_露光時間R] [smallint] NOT NULL,
	[カメラ1_露光時間G] [smallint] NOT NULL,
	[カメラ1_露光時間B] [smallint] NOT NULL,
	[カメラ2_露光時間R] [smallint] NOT NULL,
	[カメラ2_露光時間G] [smallint] NOT NULL,
	[カメラ2_露光時間B] [smallint] NOT NULL,
	[カメラ3_露光時間R] [smallint] NOT NULL,
	[カメラ3_露光時間G] [smallint] NOT NULL,
	[カメラ3_露光時間B] [smallint] NOT NULL,
	[カメラ4_露光時間R] [smallint] NOT NULL,
	[カメラ4_露光時間G] [smallint] NOT NULL,
	[カメラ4_露光時間B] [smallint] NOT NULL,
	[カメラ5_露光時間R] [smallint] NOT NULL,
	[カメラ5_露光時間G] [smallint] NOT NULL,
	[カメラ5_露光時間B] [smallint] NOT NULL,
	[カメラ6_露光時間R] [smallint] NOT NULL,
	[カメラ6_露光時間G] [smallint] NOT NULL,
	[カメラ6_露光時間B] [smallint] NOT NULL,
	[カメラ7_露光時間R] [smallint] NOT NULL,
	[カメラ7_露光時間G] [smallint] NOT NULL,
	[カメラ7_露光時間B] [smallint] NOT NULL,
	[カメラ8_露光時間R] [smallint] NOT NULL,
	[カメラ8_露光時間G] [smallint] NOT NULL,
	[カメラ8_露光時間B] [smallint] NOT NULL,
	[カメラ9_露光時間R] [smallint] NOT NULL,
	[カメラ9_露光時間G] [smallint] NOT NULL,
	[カメラ9_露光時間B] [smallint] NOT NULL,
	[カメラ10_露光時間R] [smallint] NOT NULL,
	[カメラ10_露光時間G] [smallint] NOT NULL,
	[カメラ10_露光時間B] [smallint] NOT NULL,
	[カメラ1_ゲイン_R] [float] NOT NULL,
	[カメラ1_ゲイン_G] [float] NOT NULL,
	[カメラ1_ゲイン_B] [float] NOT NULL,
	[カメラ2_ゲイン_R] [float] NOT NULL,
	[カメラ2_ゲイン_G] [float] NOT NULL,
	[カメラ2_ゲイン_B] [float] NOT NULL,
	[カメラ3_ゲイン_R] [float] NOT NULL,
	[カメラ3_ゲイン_G] [float] NOT NULL,
	[カメラ3_ゲイン_B] [float] NOT NULL,
	[カメラ4_ゲイン_R] [float] NOT NULL,
	[カメラ4_ゲイン_G] [float] NOT NULL,
	[カメラ4_ゲイン_B] [float] NOT NULL,
	[カメラ5_ゲイン_R] [float] NOT NULL,
	[カメラ5_ゲイン_G] [float] NOT NULL,
	[カメラ5_ゲイン_B] [float] NOT NULL,
	[カメラ6_ゲイン_R] [float] NOT NULL,
	[カメラ6_ゲイン_G] [float] NOT NULL,
	[カメラ6_ゲイン_B] [float] NOT NULL,
	[カメラ7_ゲイン_R] [float] NOT NULL,
	[カメラ7_ゲイン_G] [float] NOT NULL,
	[カメラ7_ゲイン_B] [float] NOT NULL,
	[カメラ8_ゲイン_R] [float] NOT NULL,
	[カメラ8_ゲイン_G] [float] NOT NULL,
	[カメラ8_ゲイン_B] [float] NOT NULL,
	[カメラ9_ゲイン_R] [float] NOT NULL,
	[カメラ9_ゲイン_G] [float] NOT NULL,
	[カメラ9_ゲイン_B] [float] NOT NULL,
	[カメラ10_ゲイン_R] [float] NOT NULL,
	[カメラ10_ゲイン_G] [float] NOT NULL,
	[カメラ10_ゲイン_B] [float] NOT NULL,
 CONSTRAINT [PK_T_COIL_TRAND] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[表裏区分] ASC,
	[取得位置] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR0_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR0_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR0_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR0_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR0_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR0_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR1_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR1_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR1_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR1_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR1_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR1_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR2_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR2_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR2_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR2_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR2_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR2_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR3_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR3_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR3_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR3_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR3_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR3_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR4_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR4_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR4_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_ATTR4_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_ATTR4_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[特徴量0] [float] NULL,
	[特徴量1] [float] NULL,
	[特徴量2] [float] NULL,
	[特徴量3] [float] NULL,
	[特徴量4] [float] NULL,
	[特徴量5] [float] NULL,
	[特徴量6] [float] NULL,
	[特徴量7] [float] NULL,
	[特徴量8] [float] NULL,
	[特徴量9] [float] NULL,
	[特徴量10] [float] NULL,
	[特徴量11] [float] NULL,
	[特徴量12] [float] NULL,
	[特徴量13] [float] NULL,
	[特徴量14] [float] NULL,
	[特徴量15] [float] NULL,
	[特徴量16] [float] NULL,
	[特徴量17] [float] NULL,
	[特徴量18] [float] NULL,
	[特徴量19] [float] NULL,
	[特徴量20] [float] NULL,
	[特徴量21] [float] NULL,
	[特徴量22] [float] NULL,
	[特徴量23] [float] NULL,
	[特徴量24] [float] NULL,
	[特徴量25] [float] NULL,
	[特徴量26] [float] NULL,
	[特徴量27] [float] NULL,
	[特徴量28] [float] NULL,
	[特徴量29] [float] NULL,
	[特徴量30] [float] NULL,
	[特徴量31] [float] NULL,
	[特徴量32] [float] NULL,
	[特徴量33] [float] NULL,
	[特徴量34] [float] NULL,
	[特徴量35] [float] NULL,
	[特徴量36] [float] NULL,
	[特徴量37] [float] NULL,
	[特徴量38] [float] NULL,
	[特徴量39] [float] NULL,
	[特徴量40] [float] NULL,
	[特徴量41] [float] NULL,
	[特徴量42] [float] NULL,
	[特徴量43] [float] NULL,
	[特徴量44] [float] NULL,
	[特徴量45] [float] NULL,
	[特徴量46] [float] NULL,
	[特徴量47] [float] NULL,
	[特徴量48] [float] NULL,
	[特徴量49] [float] NULL,
	[特徴量50] [float] NULL,
	[特徴量51] [float] NULL,
	[特徴量52] [float] NULL,
	[特徴量53] [float] NULL,
	[特徴量54] [float] NULL,
	[特徴量55] [float] NULL,
	[特徴量56] [float] NULL,
	[特徴量57] [float] NULL,
	[特徴量58] [float] NULL,
	[特徴量59] [float] NULL,
	[特徴量60] [float] NULL,
	[特徴量61] [float] NULL,
	[特徴量62] [float] NULL,
	[特徴量63] [float] NULL,
	[特徴量64] [float] NULL,
	[特徴量65] [float] NULL,
	[特徴量66] [float] NULL,
	[特徴量67] [float] NULL,
	[特徴量68] [float] NULL,
	[特徴量69] [float] NULL,
	[特徴量70] [float] NULL,
	[特徴量71] [float] NULL,
	[特徴量72] [float] NULL,
	[特徴量73] [float] NULL,
	[特徴量74] [float] NULL,
	[特徴量75] [float] NULL,
	[特徴量76] [float] NULL,
	[特徴量77] [float] NULL,
	[特徴量78] [float] NULL,
	[特徴量79] [float] NULL,
	[特徴量80] [float] NULL,
	[特徴量81] [float] NULL,
	[特徴量82] [float] NULL,
	[特徴量83] [float] NULL,
	[特徴量84] [float] NULL,
	[特徴量85] [float] NULL,
	[特徴量86] [float] NULL,
	[特徴量87] [float] NULL,
	[特徴量88] [float] NULL,
	[特徴量89] [float] NULL,
	[特徴量90] [float] NULL,
	[特徴量91] [float] NULL,
	[特徴量92] [float] NULL,
	[特徴量93] [float] NULL,
	[特徴量94] [float] NULL,
	[特徴量95] [float] NULL,
	[特徴量96] [float] NULL,
	[特徴量97] [float] NULL,
	[特徴量98] [float] NULL,
	[特徴量99] [float] NULL,
	[特徴量100] [float] NULL,
	[特徴量101] [float] NULL,
	[特徴量102] [float] NULL,
	[特徴量103] [float] NULL,
	[特徴量104] [float] NULL,
	[特徴量105] [float] NULL,
	[特徴量106] [float] NULL,
	[特徴量107] [float] NULL,
	[特徴量108] [float] NULL,
	[特徴量109] [float] NULL,
	[特徴量110] [float] NULL,
	[特徴量111] [float] NULL,
	[特徴量112] [float] NULL,
	[特徴量113] [float] NULL,
	[特徴量114] [float] NULL,
	[特徴量115] [float] NULL,
	[特徴量116] [float] NULL,
	[特徴量117] [float] NULL,
	[特徴量118] [float] NULL,
	[特徴量119] [float] NULL,
	[特徴量120] [float] NULL,
	[特徴量121] [float] NULL,
	[特徴量122] [float] NULL,
	[特徴量123] [float] NULL,
	[特徴量124] [float] NULL,
	[特徴量125] [float] NULL,
	[特徴量126] [float] NULL,
	[特徴量127] [float] NULL,
	[特徴量128] [float] NULL,
	[特徴量129] [float] NULL,
	[特徴量130] [float] NULL,
	[特徴量131] [float] NULL,
	[特徴量132] [float] NULL,
	[特徴量133] [float] NULL,
	[特徴量134] [float] NULL,
	[特徴量135] [float] NULL,
	[特徴量136] [float] NULL,
	[特徴量137] [float] NULL,
	[特徴量138] [float] NULL,
	[特徴量139] [float] NULL,
	[特徴量140] [float] NULL,
	[特徴量141] [float] NULL,
	[特徴量142] [float] NULL,
	[特徴量143] [float] NULL,
	[特徴量144] [float] NULL,
	[特徴量145] [float] NULL,
	[特徴量146] [float] NULL,
	[特徴量147] [float] NULL,
	[特徴量148] [float] NULL,
	[特徴量149] [float] NULL,
	[特徴量150] [float] NULL,
	[特徴量151] [float] NULL,
	[特徴量152] [float] NULL,
	[特徴量153] [float] NULL,
	[特徴量154] [float] NULL,
	[特徴量155] [float] NULL,
	[特徴量156] [float] NULL,
	[特徴量157] [float] NULL,
	[特徴量158] [float] NULL,
	[特徴量159] [float] NULL,
	[特徴量160] [float] NULL,
	[特徴量161] [float] NULL,
	[特徴量162] [float] NULL,
	[特徴量163] [float] NULL,
	[特徴量164] [float] NULL,
	[特徴量165] [float] NULL,
	[特徴量166] [float] NULL,
	[特徴量167] [float] NULL,
	[特徴量168] [float] NULL,
	[特徴量169] [float] NULL,
	[特徴量170] [float] NULL,
	[特徴量171] [float] NULL,
	[特徴量172] [float] NULL,
	[特徴量173] [float] NULL,
	[特徴量174] [float] NULL,
	[特徴量175] [float] NULL,
	[特徴量176] [float] NULL,
	[特徴量177] [float] NULL,
	[特徴量178] [float] NULL,
	[特徴量179] [float] NULL,
	[特徴量180] [float] NULL,
	[特徴量181] [float] NULL,
	[特徴量182] [float] NULL,
	[特徴量183] [float] NULL,
	[特徴量184] [float] NULL,
	[特徴量185] [float] NULL,
	[特徴量186] [float] NULL,
	[特徴量187] [float] NULL,
	[特徴量188] [float] NULL,
	[特徴量189] [float] NULL,
	[特徴量190] [float] NULL,
	[特徴量191] [float] NULL,
	[特徴量192] [float] NULL,
	[特徴量193] [float] NULL,
	[特徴量194] [float] NULL,
	[特徴量195] [float] NULL,
	[特徴量196] [float] NULL,
	[特徴量197] [float] NULL,
	[特徴量198] [float] NULL,
	[特徴量199] [float] NULL,
	[特徴量200] [float] NULL,
	[特徴量201] [float] NULL,
	[特徴量202] [float] NULL,
	[特徴量203] [float] NULL,
	[特徴量204] [float] NULL,
	[特徴量205] [float] NULL,
	[特徴量206] [float] NULL,
	[特徴量207] [float] NULL,
	[特徴量208] [float] NULL,
	[特徴量209] [float] NULL,
	[特徴量210] [float] NULL,
	[特徴量211] [float] NULL,
	[特徴量212] [float] NULL,
	[特徴量213] [float] NULL,
	[特徴量214] [float] NULL,
	[特徴量215] [float] NULL,
	[特徴量216] [float] NULL,
	[特徴量217] [float] NULL,
	[特徴量218] [float] NULL,
	[特徴量219] [float] NULL,
	[特徴量220] [float] NULL,
	[特徴量221] [float] NULL,
	[特徴量222] [float] NULL,
	[特徴量223] [float] NULL,
	[特徴量224] [float] NULL,
	[特徴量225] [float] NULL,
	[特徴量226] [float] NULL,
	[特徴量227] [float] NULL,
	[特徴量228] [float] NULL,
	[特徴量229] [float] NULL,
	[特徴量230] [float] NULL,
	[特徴量231] [float] NULL,
	[特徴量232] [float] NULL,
	[特徴量233] [float] NULL,
	[特徴量234] [float] NULL,
	[特徴量235] [float] NULL,
	[特徴量236] [float] NULL,
	[特徴量237] [float] NULL,
	[特徴量238] [float] NULL,
	[特徴量239] [float] NULL,
	[特徴量240] [float] NULL,
	[特徴量241] [float] NULL,
	[特徴量242] [float] NULL,
	[特徴量243] [float] NULL,
	[特徴量244] [float] NULL,
	[特徴量245] [float] NULL,
	[特徴量246] [float] NULL,
	[特徴量247] [float] NULL,
	[特徴量248] [float] NULL,
	[特徴量249] [float] NULL,
	[特徴量250] [float] NULL,
	[特徴量251] [float] NULL,
	[特徴量252] [float] NULL,
	[特徴量253] [float] NULL,
	[特徴量254] [float] NULL,
	[特徴量255] [float] NULL,
 CONSTRAINT [PK_T_DEFECT_ATTR4_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_IMG_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_IMG_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[正画像] [varbinary](max) NULL,
	[乱画像] [varbinary](max) NULL,
	[乱画像2] [varbinary](max) NULL,
	[乱画像3] [varbinary](max) NULL,
 CONSTRAINT [PK_T_DEFECT_IMG_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_IMG_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_IMG_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[正画像] [varbinary](max) NULL,
	[乱画像] [varbinary](max) NULL,
	[乱画像2] [varbinary](max) NULL,
	[乱画像3] [varbinary](max) NULL,
 CONSTRAINT [PK_T_DEFECT_IMG_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_INF_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_INF_B](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[重複結果] [int] NOT NULL,
	[判定疵種] [int] NOT NULL,
	[判定グレード] [int] NOT NULL,
	[周期フラグ] [int] NOT NULL,
	[DSWS区分] [int] NOT NULL,
	[カメラセット] [int] NOT NULL,
	[長手位置] [int] NOT NULL,
	[DS位置] [real] NOT NULL,
	[WS位置] [real] NOT NULL,
	[ライン速度] [int] NOT NULL,
	[ピッチ] [int] NOT NULL,
	[径] [real] NOT NULL,
	[疵面積] [real] NOT NULL,
	[疵幅] [int] NOT NULL,
	[疵長さ] [int] NOT NULL,
	[無害Gr] [int] NOT NULL,
	[幅分割区分] [int] NOT NULL,
	[幅方向絶対位置] [int] NOT NULL,
	[表示対象区分_探傷画面] [int] NOT NULL,
	[予備_10] [int] NULL,
	[予備_9] [int] NULL,
	[予備_8] [int] NULL,
	[予備_7] [int] NULL,
	[予備_6] [int] NULL,
	[予備_5] [int] NULL,
	[予備_4] [int] NULL,
	[予備_3] [int] NULL,
	[予備_2] [int] NULL,
	[予備_1] [int] NULL,
	[疵連番] [int] NOT NULL,
	[縦分解能] [real] NOT NULL,
	[横分解能] [real] NOT NULL,
	[フレームNo] [int] NOT NULL,
	[フレーム外接Xmin] [int] NOT NULL,
	[フレーム外接Xmax] [int] NOT NULL,
	[フレーム外接Ymin] [int] NOT NULL,
	[フレーム外接Ymax] [int] NOT NULL,
	[間引きフレーム外接Xmin] [int] NOT NULL,
	[間引きフレーム外接Xmax] [int] NOT NULL,
	[間引きフレーム外接Ymin] [int] NOT NULL,
	[間引きフレーム外接Ymax] [int] NOT NULL,
	[外接Xmin] [int] NOT NULL,
	[外接Xmax] [int] NOT NULL,
	[外接Ymin] [int] NOT NULL,
	[外接Ymax] [int] NOT NULL,
	[疵中心位置X] [int] NOT NULL,
	[疵中心位置Y] [int] NOT NULL,
	[縦画素数] [int] NOT NULL,
	[横画素数] [int] NOT NULL,
	[画像サイズ正] [int] NOT NULL,
	[画像サイズ乱] [int] NOT NULL,
	[画像サイズ乱2] [int] NOT NULL,
	[画像サイズ乱3] [int] NOT NULL,
	[元画像外接Xmin] [int] NOT NULL,
	[元画像外接Ymin] [int] NOT NULL,
	[元画像外接Xmax] [int] NOT NULL,
	[元画像外接Ymax] [int] NOT NULL,
	[元画像疵中心位置X] [int] NOT NULL,
	[元画像疵中心位置Y] [int] NOT NULL,
	[元画像縦画素数] [int] NOT NULL,
	[元画像横画素数] [int] NOT NULL,
	[元縦分解能] [real] NOT NULL,
	[元横分解能] [real] NOT NULL,
	[縮小率] [int] NOT NULL,
 CONSTRAINT [PK_T_DEFECT_INF_B] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_DEFECT_INF_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_DEFECT_INF_T](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[重複結果] [int] NOT NULL,
	[判定疵種] [int] NOT NULL,
	[判定グレード] [int] NOT NULL,
	[周期フラグ] [int] NOT NULL,
	[DSWS区分] [int] NOT NULL,
	[カメラセット] [int] NOT NULL,
	[長手位置] [int] NOT NULL,
	[DS位置] [real] NOT NULL,
	[WS位置] [real] NOT NULL,
	[ライン速度] [int] NOT NULL,
	[ピッチ] [int] NOT NULL,
	[径] [real] NOT NULL,
	[疵面積] [real] NOT NULL,
	[疵幅] [int] NOT NULL,
	[疵長さ] [int] NOT NULL,
	[無害Gr] [int] NOT NULL,
	[幅分割区分] [int] NOT NULL,
	[幅方向絶対位置] [int] NOT NULL,
	[表示対象区分_探傷画面] [int] NOT NULL,
	[予備_10] [int] NULL,
	[予備_9] [int] NULL,
	[予備_8] [int] NULL,
	[予備_7] [int] NULL,
	[予備_6] [int] NULL,
	[予備_5] [int] NULL,
	[予備_4] [int] NULL,
	[予備_3] [int] NULL,
	[予備_2] [int] NULL,
	[予備_1] [int] NULL,
	[疵連番] [int] NOT NULL,
	[縦分解能] [real] NOT NULL,
	[横分解能] [real] NOT NULL,
	[フレームNo] [int] NOT NULL,
	[フレーム外接Xmin] [int] NOT NULL,
	[フレーム外接Xmax] [int] NOT NULL,
	[フレーム外接Ymin] [int] NOT NULL,
	[フレーム外接Ymax] [int] NOT NULL,
	[間引きフレーム外接Xmin] [int] NOT NULL,
	[間引きフレーム外接Xmax] [int] NOT NULL,
	[間引きフレーム外接Ymin] [int] NOT NULL,
	[間引きフレーム外接Ymax] [int] NOT NULL,
	[外接Xmin] [int] NOT NULL,
	[外接Xmax] [int] NOT NULL,
	[外接Ymin] [int] NOT NULL,
	[外接Ymax] [int] NOT NULL,
	[疵中心位置X] [int] NOT NULL,
	[疵中心位置Y] [int] NOT NULL,
	[縦画素数] [int] NOT NULL,
	[横画素数] [int] NOT NULL,
	[画像サイズ正] [int] NOT NULL,
	[画像サイズ乱] [int] NOT NULL,
	[画像サイズ乱2] [int] NOT NULL,
	[画像サイズ乱3] [int] NOT NULL,
	[元画像外接Xmin] [int] NOT NULL,
	[元画像外接Ymin] [int] NOT NULL,
	[元画像外接Xmax] [int] NOT NULL,
	[元画像外接Ymax] [int] NOT NULL,
	[元画像疵中心位置X] [int] NOT NULL,
	[元画像疵中心位置Y] [int] NOT NULL,
	[元画像縦画素数] [int] NOT NULL,
	[元画像横画素数] [int] NOT NULL,
	[元縦分解能] [real] NOT NULL,
	[元横分解能] [real] NOT NULL,
	[縮小率] [int] NOT NULL,
 CONSTRAINT [PK_T_DEFECT_INF_T] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_EDGE_CHANGE_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_EDGE_CHANGE_INF](
	[管理No] [varchar](20) NOT NULL,
	[長手位置] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[フレームNo] [int] NOT NULL,
	[検出区分] [int] NOT NULL,
	[エッジ位置生左] [real] NOT NULL,
	[エッジ位置生右] [real] NOT NULL,
	[エッジ位置最終左] [real] NOT NULL,
	[エッジ位置最終右] [real] NOT NULL,
	[エッジ不正理由] [int] NULL,
 CONSTRAINT [PK_T_EDGE_CHANGE_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[長手位置] ASC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_EDGE_COIL_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_EDGE_COIL_INF](
	[管理No] [varchar](20) NOT NULL,
	[表裏区分] [int] NOT NULL,
	[板幅最小] [real] NOT NULL,
	[板幅最大] [real] NOT NULL,
	[板幅許容範囲外数] [int] NOT NULL,
	[エッジ検出失敗個数_左] [int] NOT NULL,
	[エッジ検出失敗個数_右] [int] NOT NULL,
	[コイル単位フレーム数] [int] NOT NULL,
	[カメラセット1_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット1_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット1_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット1_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット1_未検査_QFULL] [int] NOT NULL,
	[カメラセット2_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット2_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット2_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット2_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット2_未検査_QFULL] [int] NOT NULL,
	[カメラセット3_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット3_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット3_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット3_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット3_未検査_QFULL] [int] NOT NULL,
	[カメラセット4_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット4_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット4_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット4_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット4_未検査_QFULL] [int] NOT NULL,
	[カメラセット5_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット5_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット5_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット5_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット5_未検査_QFULL] [int] NOT NULL,
	[カメラセット6_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット6_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット6_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット6_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット6_未検査_QFULL] [int] NOT NULL,
	[カメラセット7_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット7_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット7_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット7_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット7_未検査_QFULL] [int] NOT NULL,
	[カメラセット8_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット8_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット8_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット8_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット8_未検査_QFULL] [int] NOT NULL,
	[カメラセット9_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット9_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット9_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット9_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット9_未検査_QFULL] [int] NOT NULL,
	[カメラセット10_未検査_EDGELOST] [int] NOT NULL,
	[カメラセット10_未検査_PROCSKIP] [int] NOT NULL,
	[カメラセット10_未検査_SMEMFULL] [int] NOT NULL,
	[カメラセット10_未検査_TOOMANY] [int] NOT NULL,
	[カメラセット10_未検査_QFULL] [int] NOT NULL,
 CONSTRAINT [PK_T_EDGE_COIL_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_LOG_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_LOG_INF](
	[発生日付時刻] [datetime] NULL,
	[発生元] [varchar](16) NULL,
	[PC名] [varchar](16) NULL,
	[PCKIND] [int] NULL,
	[アラームNo] [int] NOT NULL,
	[内容] [varchar](256) NULL,
	[登録日付] [datetime] NOT NULL,
	[AutoNo] [int] NOT NULL,
 CONSTRAINT [PK_T_LOG_INF] PRIMARY KEY CLUSTERED 
(
	[登録日付] DESC,
	[AutoNo] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_LONGPD_EXEC_RESULT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_LONGPD_EXEC_RESULT](
	[ProcNo] [varchar](20) NOT NULL,
	[SaveDateTime] [datetime] NOT NULL,
	[TopManagementNo] [varchar](20) NOT NULL,
	[CalcStartPos] [int] NOT NULL,
	[CalcRange] [int] NOT NULL,
	[ProcTarget_Top] [int] NOT NULL,
	[ProcTarget_Bot] [int] NOT NULL,
	[FilterCount_Top] [int] NOT NULL,
	[FilterCount_Bot] [int] NOT NULL,
 CONSTRAINT [PK_T_LONGPD_COIL_RESULT_PERIODIC] PRIMARY KEY CLUSTERED 
(
	[ProcNo] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_LONGPD_EXEC_RESULT_DEFECT_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_LONGPD_EXEC_RESULT_DEFECT_B](
	[ManagementNo] [varchar](20) NOT NULL,
	[DefectNo] [int] NOT NULL,
	[TorB] [int] NOT NULL,
	[WidDivPosNo] [int] NOT NULL,
	[Cycle_Filter1] [float] NOT NULL,
	[Cycle_Filter2] [float] NOT NULL,
	[Cycle_Filter3] [float] NOT NULL,
	[Cycle_Filter4] [float] NOT NULL,
	[Cycle_Filter5] [float] NOT NULL,
	[Cycle_Filter6] [float] NOT NULL,
	[Cycle_Filter7] [float] NOT NULL,
	[Cycle_Filter8] [float] NOT NULL,
	[Cycle_Filter9] [float] NOT NULL,
	[Cycle_Filter10] [float] NOT NULL,
	[Cycle_Filter11] [float] NOT NULL,
	[Cycle_Filter12] [float] NOT NULL,
	[Cycle_Filter13] [float] NOT NULL,
	[Cycle_Filter14] [float] NOT NULL,
	[Cycle_Filter15] [float] NOT NULL,
	[Cycle_Filter16] [float] NOT NULL,
 CONSTRAINT [PK_T_LONGPD_DEFECT_B] PRIMARY KEY CLUSTERED 
(
	[ManagementNo] DESC,
	[DefectNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_LONGPD_EXEC_RESULT_DEFECT_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_LONGPD_EXEC_RESULT_DEFECT_T](
	[ManagementNo] [varchar](20) NOT NULL,
	[DefectNo] [int] NOT NULL,
	[TorB] [int] NOT NULL,
	[WidDivPosNo] [int] NOT NULL,
	[Cycle_Filter1] [float] NOT NULL,
	[Cycle_Filter2] [float] NOT NULL,
	[Cycle_Filter3] [float] NOT NULL,
	[Cycle_Filter4] [float] NOT NULL,
	[Cycle_Filter5] [float] NOT NULL,
	[Cycle_Filter6] [float] NOT NULL,
	[Cycle_Filter7] [float] NOT NULL,
	[Cycle_Filter8] [float] NOT NULL,
	[Cycle_Filter9] [float] NOT NULL,
	[Cycle_Filter10] [float] NOT NULL,
	[Cycle_Filter11] [float] NOT NULL,
	[Cycle_Filter12] [float] NOT NULL,
	[Cycle_Filter13] [float] NOT NULL,
	[Cycle_Filter14] [float] NOT NULL,
	[Cycle_Filter15] [float] NOT NULL,
	[Cycle_Filter16] [float] NOT NULL,
 CONSTRAINT [PK_T_LONGPD_DEFECT_T] PRIMARY KEY CLUSTERED 
(
	[ManagementNo] DESC,
	[DefectNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_LONGPD_EXEC_RESULT_LINE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_LONGPD_EXEC_RESULT_LINE](
	[ProcNo] [varchar](20) NOT NULL,
	[SaveDateTime] [datetime] NOT NULL,
	[ManagementNo] [varchar](20) NOT NULL,
	[WidthFrom] [int] NOT NULL,
	[WidthTo] [int] NOT NULL,
	[LineSegNo] [int] NOT NULL,
	[LineSegStartX] [int] NOT NULL,
	[LineSegStartY] [int] NOT NULL,
	[LineSegEndX] [int] NOT NULL,
	[LineSegEndY] [int] NOT NULL,
	[RoughCycle] [float] NOT NULL,
	[Cycle] [float] NOT NULL,
	[DefectCount] [int] NOT NULL,
 CONSTRAINT [PK_T_LONGPD_EXEC_RESULT_LINE] PRIMARY KEY CLUSTERED 
(
	[ProcNo] ASC,
	[SaveDateTime] ASC,
	[ManagementNo] ASC,
	[WidthFrom] ASC,
	[WidthTo] ASC,
	[LineSegNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_LONGPD_EXEC_RESULT_WIDTH]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_LONGPD_EXEC_RESULT_WIDTH](
	[ProcNo] [varchar](20) NOT NULL,
	[ManagementNo] [varchar](20) NULL,
	[TorB] [int] NOT NULL,
	[FilterNo] [int] NOT NULL,
	[WidDivPosNo] [int] NOT NULL,
	[FilterName] [varchar](64) NOT NULL,
	[WidDivPos_from] [int] NOT NULL,
	[WidDivPos_to] [int] NOT NULL,
	[CycleValue] [float] NOT NULL,
 CONSTRAINT [PK_T_LONGPD_EXEC_RESULT_WIDTH] PRIMARY KEY CLUSTERED 
(
	[ProcNo] ASC,
	[TorB] ASC,
	[FilterNo] ASC,
	[WidDivPosNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_MAE_COIL_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_MAE_COIL_INF](
	[管理No] [varchar](20) NOT NULL,
	[登録日付] [datetime] NOT NULL,
	[キー情報] [varchar](20) NOT NULL,
	[ID] [varchar](20) NULL,
	[仮管理Noフラグ] [int] NOT NULL,
	[FF前工程疵情報受信フラグ] [int] NOT NULL,
	[工程コード] [varchar](2) NOT NULL,
	[命令No] [varchar](4) NOT NULL,
	[コイルNo] [varchar](12) NOT NULL,
	[入側フロント端板長さ] [int] NOT NULL,
	[冷延コイル番号] [varchar](10) NULL,
	[冷延コイル重量] [int] NULL,
	[冷延コイル長さ] [int] NULL,
	[冷延厚] [int] NULL,
	[板厚公差MIN] [int] NULL,
	[板厚公差MAX] [int] NULL,
	[板巾_向先1] [int] NULL,
	[検査基準] [varchar](4) NULL,
	[捲き方向] [int] NULL,
	[材料巾] [int] NULL,
	[板幅公差MIN_向先1] [int] NULL,
	[板幅公差MAX_向先1] [int] NULL,
	[長さ_ロット枚数_向先1] [int] NULL,
	[内径_向先1] [int] NULL,
	[板厚] [int] NULL,
	[入側内径] [int] NULL,
	[剪断長_向先1] [int] NULL,
	[前工程コード] [varchar](2) NULL,
	[仮想RCL管理No] [varchar](20) NULL,
	[母体となる冷延コイルNo] [varchar](10) NULL,
	[採取開始位置長さ] [int] NULL,
	[採取終了位置長さ] [int] NULL,
	[採取子番] [int] NULL,
 CONSTRAINT [PK_T_MAE_COIL_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_MAE_COIL_LINE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_MAE_COIL_LINE](
	[管理No] [varchar](20) NOT NULL,
	[工程種別] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[処理ステータス] [int] NOT NULL,
	[疵検出工程] [varchar](2) NOT NULL,
	[前工程管理No] [varchar](20) NOT NULL,
	[検査装置処理モード] [int] NOT NULL,
	[疵設定個数] [int] NOT NULL,
	[疵個数OV] [int] NOT NULL,
 CONSTRAINT [PK_T_MAE_COIL_LINE] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[工程種別] ASC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_MAE_DEFECT_IMG]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_MAE_DEFECT_IMG](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[画像1] [varbinary](max) NULL,
	[画像2] [varbinary](max) NULL,
	[画像3] [varbinary](max) NULL,
	[画像4] [varbinary](max) NULL,
 CONSTRAINT [PK_T_MAE_DEFECT_IMG] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_MAE_DEFECT_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_MAE_DEFECT_INF](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[工程種別] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[周期フラグ] [int] NULL,
	[長手位置] [int] NULL,
	[DSWS区分] [int] NULL,
	[DS位置] [real] NULL,
	[WS位置] [real] NULL,
	[ライン速度] [int] NULL,
	[ピッチ] [int] NULL,
	[径] [real] NULL,
	[疵幅] [int] NULL,
	[疵長さ] [int] NULL,
	[縦分解能] [real] NULL,
	[横分解能] [real] NULL,
	[外接Xmin] [int] NULL,
	[外接Xmax] [int] NULL,
	[外接Ymin] [int] NULL,
	[外接Ymax] [int] NULL,
	[疵中心位置X] [int] NULL,
	[疵中心位置Y] [int] NULL,
	[縦画素数] [int] NULL,
	[横画素数] [int] NULL,
	[画像サイズ1] [int] NULL,
	[画像サイズ2] [int] NULL,
	[画像サイズ3] [int] NULL,
	[画像サイズ4] [int] NULL,
 CONSTRAINT [PK_T_MAE_DEFECT_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_MAE_FF_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_MAE_FF_INF](
	[管理No] [varchar](20) NOT NULL,
	[疵No] [int] NOT NULL,
	[工程種別] [int] NOT NULL,
	[表裏区分] [int] NOT NULL,
	[事前編集フラグ] [int] NOT NULL,
	[前工程管理No] [varchar](20) NOT NULL,
	[前工程疵No] [int] NOT NULL,
	[FF疵種] [varchar](2) NOT NULL,
	[FFグレード] [varchar](2) NOT NULL,
	[FF疵検出工程] [varchar](2) NOT NULL,
	[FF疵区分] [int] NOT NULL,
	[FF疵長さ方向位置] [int] NOT NULL,
	[FF疵巾方向位置] [int] NOT NULL,
	[FF疵長さ] [int] NOT NULL,
	[判定疵種] [int] NOT NULL,
	[判定グレード] [int] NOT NULL,
	[最終長手位置] [int] NOT NULL,
 CONSTRAINT [PK_T_MAE_FF_INF] PRIMARY KEY CLUSTERED 
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_ST_HISTORY_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_ST_HISTORY_INF](
	[登録日付] [datetime] NOT NULL,
	[区分] [int] NOT NULL,
	[機器状態_1] [int] NULL,
	[機器状態_2] [int] NULL,
	[機器状態_3] [int] NULL,
	[機器状態_4] [int] NULL,
	[機器状態_5] [int] NULL,
	[機器状態_6] [int] NULL,
	[機器状態_7] [int] NULL,
	[機器状態_8] [int] NULL,
	[機器状態_9] [int] NULL,
	[機器状態_10] [int] NULL,
	[機器状態_11] [int] NULL,
	[機器状態_12] [int] NULL,
	[機器状態_13] [int] NULL,
	[機器状態_14] [int] NULL,
	[機器状態_15] [int] NULL,
	[機器状態_16] [int] NULL,
	[機器状態_17] [int] NULL,
	[機器状態_18] [int] NULL,
	[機器状態_19] [int] NULL,
	[機器状態_20] [int] NULL,
	[付加情報_1] [varchar](32) NULL,
 CONSTRAINT [PK_T_ST_HISTORY_INF] PRIMARY KEY CLUSTERED 
(
	[登録日付] DESC,
	[区分] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[T_STATUS_LOG]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[T_STATUS_LOG](
	[登録日付] [datetime] NOT NULL,
	[AutoNo] [int] NOT NULL,
	[稼働状態] [int] NOT NULL,
	[検査状態_表] [int] NOT NULL,
	[検査状態_裏] [int] NOT NULL,
	[機器状態_表] [int] NOT NULL,
	[機器状態_裏] [int] NOT NULL,
	[ガイダンス] [varchar](256) NULL,
	[管理No] [varchar](20) NULL,
	[コイルNo] [varchar](32) NULL,
 CONSTRAINT [PK_T_STATUS_LOG] PRIMARY KEY CLUSTERED 
(
	[登録日付] DESC,
	[AutoNo] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[TYPE_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[TYPE_NAME](
	[EdasysID] [int] NOT NULL,
	[ID] [int] NOT NULL,
	[Name] [varchar](64) NULL,
	[優先度] [int] NOT NULL,
	[マップ表示名称] [varchar](6) NULL,
	[FF伝送名称] [varchar](2) NULL,
	[プロコングループ] [int] NOT NULL,
	[Score] [int] NOT NULL,
	[PeriodProcMask] [int] NULL,
	[疵マップシンボル表示色] [int] NOT NULL,
	[疵マップシンボル] [int] NULL,
	[切出画像種類1] [int] NULL,
	[切出画像種類2] [int] NULL,
 CONSTRAINT [PK_TYPE_NAME] PRIMARY KEY CLUSTERED 
(
	[EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  View [dbo].[V_DEFECT_ADD_COIL_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_DEFECT_ADD_COIL_INF]
AS
SELECT                      dbo.T_COIL_INF.管理No AS kizuken_no, dbo.T_COIL_RESULT.検査実測長 AS Wid, 
                                      dbo.T_COIL_INF.材料板幅 AS Len
FROM                         dbo.T_COIL_INF INNER JOIN
                                      dbo.T_COIL_RESULT ON dbo.T_COIL_INF.管理No = dbo.T_COIL_RESULT.管理No


GO
/****** Object:  View [dbo].[V_LONGPD_COIL_INF]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_COIL_INF]
AS
SELECT                      inf.管理No AS ManagementNo, inf.生産No AS CoilNo, ISNULL(res.検査パターンコード_表, 64) AS PcodeTop, 
                                      ISNULL(res.検査パターンコード_裏, 64) AS PcodeBot, ISNULL(res.検査実測長, - 1) AS CoilLength, 
                                      inf.材料板幅 AS CoilWidth, ISNULL(res.登録日付, NULL) AS SaveDateTime
FROM                         dbo.T_COIL_INF AS inf LEFT OUTER JOIN
                                      dbo.T_COIL_RESULT AS res ON res.管理No = inf.管理No

GO
/****** Object:  View [dbo].[V_LONGPD_DEFECT_INF_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_DEFECT_INF_B]
AS
SELECT                      TOP (100) PERCENT 管理No AS ManagementNo, 疵No AS DefectNo, 判定疵種 AS Type, 判定グレード AS Grade, 
                                      長手位置 AS Pos, DS位置 AS WidPos, 疵長さ AS Length
FROM                         dbo.T_DEFECT_INF_B AS inf

GO
/****** Object:  View [dbo].[V_LONGPD_DEFECT_INF_B_WS]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_DEFECT_INF_B_WS]
AS
SELECT                      TOP (100) PERCENT 管理No AS ManagementNo, 疵No AS DefectNo, 判定疵種 AS Type, 判定グレード AS Grade, 
                                      長手位置 AS Pos, WS位置 AS WidPos, 疵長さ AS Length
FROM                         dbo.T_DEFECT_INF_B AS inf

GO
/****** Object:  View [dbo].[V_LONGPD_DEFECT_INF_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_DEFECT_INF_T]
AS
SELECT                      TOP (100) PERCENT 管理No AS ManagementNo, 疵No AS DefectNo, 判定疵種 AS Type, 判定グレード AS Grade, 
                                      長手位置 AS Pos, DS位置 AS WidPos, 疵長さ AS Length
FROM                         dbo.T_DEFECT_INF_T AS inf

GO
/****** Object:  View [dbo].[V_LONGPD_EXEC_RESULT]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_EXEC_RESULT]
AS
SELECT                      ProcNo, SaveDateTime, TopManagementNo AS 管理No, CalcStartPos, CalcRange, ProcTarget_Top, ProcTarget_Bot, 
                                      FilterCount_Top, FilterCount_Bot
FROM                         dbo.T_LONGPD_EXEC_RESULT

GO
/****** Object:  View [dbo].[V_LONGPD_EXEC_RESULT_DEFECT_B]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE VIEW [dbo].[V_LONGPD_EXEC_RESULT_DEFECT_B]
AS
SELECT 
   [ManagementNo] As 管理No
      ,[DefectNo] As 疵No
      ,[TorB]
      ,[WidDivPosNo]
      ,[Cycle_Filter1]
      ,[Cycle_Filter2]
      ,[Cycle_Filter3]
      ,[Cycle_Filter4]
      ,[Cycle_Filter5]
      ,[Cycle_Filter6]
      ,[Cycle_Filter7]
      ,[Cycle_Filter8]
      ,[Cycle_Filter9]
      ,[Cycle_Filter10]
      ,[Cycle_Filter11]
      ,[Cycle_Filter12]
      ,[Cycle_Filter13]
      ,[Cycle_Filter14]
      ,[Cycle_Filter15]
      ,[Cycle_Filter16]
  FROM [dbo].[T_LONGPD_EXEC_RESULT_DEFECT_B]

GO
/****** Object:  View [dbo].[V_LONGPD_EXEC_RESULT_DEFECT_T]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE VIEW [dbo].[V_LONGPD_EXEC_RESULT_DEFECT_T]
AS
SELECT 
   [ManagementNo] As 管理No
      ,[DefectNo] As 疵No
      ,[TorB]
      ,[WidDivPosNo]
      ,[Cycle_Filter1]
      ,[Cycle_Filter2]
      ,[Cycle_Filter3]
      ,[Cycle_Filter4]
      ,[Cycle_Filter5]
      ,[Cycle_Filter6]
      ,[Cycle_Filter7]
      ,[Cycle_Filter8]
      ,[Cycle_Filter9]
      ,[Cycle_Filter10]
      ,[Cycle_Filter11]
      ,[Cycle_Filter12]
      ,[Cycle_Filter13]
      ,[Cycle_Filter14]
      ,[Cycle_Filter15]
      ,[Cycle_Filter16]
  FROM [dbo].[T_LONGPD_EXEC_RESULT_DEFECT_T]

GO
/****** Object:  View [dbo].[V_LONGPD_EXEC_RESULT_LINE]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_EXEC_RESULT_LINE]
AS
SELECT                      ProcNo, SaveDateTime, ManagementNo AS 管理No, WidthFrom, WidthTo, LineSegNo, LineSegStartX, LineSegStartY, 
                                      LineSegEndX, LineSegEndY, RoughCycle, Cycle, DefectCount
FROM                         dbo.T_LONGPD_EXEC_RESULT_LINE

GO
/****** Object:  View [dbo].[V_LONGPD_EXEC_RESULT_WIDTH]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_EXEC_RESULT_WIDTH]
AS
SELECT                      ProcNo, ManagementNo AS 管理No, TorB, FilterNo, WidDivPosNo, FilterName, WidDivPos_from, WidDivPos_to, 
                                      CycleValue
FROM                         dbo.T_LONGPD_EXEC_RESULT_WIDTH

GO
/****** Object:  View [dbo].[V_LONGPD_GRADE_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_GRADE_NAME]
AS
SELECT                      ID, Name AS GradeName, EdasysID AS GradeEdasysID, 優先度 AS Priority
FROM                         dbo.GRADE_NAME

GO
/****** Object:  View [dbo].[V_LONGPD_TYPE_NAME]    Script Date: 2024/08/19 19:42:12 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE VIEW [dbo].[V_LONGPD_TYPE_NAME]
AS
SELECT                      TOP (100) PERCENT ID, Name AS TypeName, EdasysID AS TypeEdasysID, 優先度 AS Priority
FROM                         dbo.TYPE_NAME
ORDER BY               優先度

GO
/****** Object:  Index [IX_ATTR_NAME_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_ATTR_NAME_1] ON [dbo].[ATTR_NAME]
(
	[Angle] ASC,
	[Enum] ASC,
	[EdasysID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_BUF_FF_R_MAEINF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_BUF_FF_R_MAEINF_1] ON [dbo].[BUF_FF_R_MAEINF]
(
	[ID] DESC,
	[キー情報] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_BUF_FF_S_ZISEKI_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_BUF_FF_S_ZISEKI_1] ON [dbo].[BUF_FF_S_ZISEKI]
(
	[ID] DESC,
	[伝文区分] DESC,
	[キー情報] ASC,
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_BUF_PRO_R_COILINF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_BUF_PRO_R_COILINF_1] ON [dbo].[BUF_PRO_R_COILINF]
(
	[ID] DESC,
	[キー情報] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_BUF_PRO_R_CUT_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_BUF_PRO_R_CUT_1] ON [dbo].[BUF_PRO_R_CUT]
(
	[ID] DESC,
	[キー情報] ASC,
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_BUF_PRO_R_SAISUN_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_BUF_PRO_R_SAISUN_1] ON [dbo].[BUF_PRO_R_SAISUN]
(
	[ID] DESC,
	[キー情報] ASC,
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_BUF_PRO_S_ZISEKI_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_BUF_PRO_S_ZISEKI_1] ON [dbo].[BUF_PRO_S_ZISEKI]
(
	[ID] DESC,
	[伝文区分] DESC,
	[キー情報] ASC,
	[分割No] ASC,
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_GRADE_NAME_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_GRADE_NAME_1] ON [dbo].[GRADE_NAME]
(
	[EdasysID] ASC,
	[ID] ASC,
	[優先度] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_BASE_ACTION_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_BASE_ACTION_MASTER_1] ON [dbo].[M_BASE_ACTION_MASTER]
(
	[表示順番] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_CAMERA_LINKSWITCH_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_CAMERA_LINKSWITCH_1] ON [dbo].[M_CAMERA_LINKSWITCH]
(
	[ID] ASC,
	[ボードID] ASC,
	[ステータス] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_COMMON_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_COMMON_MASTER_1] ON [dbo].[M_COMMON_MASTER]
(
	[LINEID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_LOG_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_LOG_MASTER_1] ON [dbo].[M_LOG_MASTER]
(
	[アラームNo] ASC,
	[種別] ASC,
	[区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_M_MAE_GRADE_NAME_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_MAE_GRADE_NAME_1] ON [dbo].[M_MAE_GRADE_NAME]
(
	[前工程コード] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_M_MAE_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_MAE_MASTER_1] ON [dbo].[M_MAE_MASTER]
(
	[前工程コード] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_M_MAE_TYPE_NAME_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_MAE_TYPE_NAME_1] ON [dbo].[M_MAE_TYPE_NAME]
(
	[前工程コード] ASC,
	[FFName] ASC,
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_M_PAR_GRID_SETTING_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_PAR_GRID_SETTING_1] ON [dbo].[M_PAR_GRID_SETTING]
(
	[画面ID] ASC,
	[グリッドID] ASC,
	[項目No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_PAR_LOV_ITEM_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_PAR_LOV_ITEM_1] ON [dbo].[M_PAR_LOV_ITEM]
(
	[lov_id] ASC,
	[seq] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_PC_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_PC_INF_1] ON [dbo].[M_PC_INF]
(
	[PCKIND] ASC,
	[PC区分] ASC,
	[PCID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_M_PC_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_PC_MASTER_1] ON [dbo].[M_PC_MASTER]
(
	[PCID] ASC,
	[PC名] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_PCODE_JUDGE_2]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_PCODE_JUDGE_2] ON [dbo].[M_PCODE_JUDGE]
(
	[pcode] ASC,
	[条件No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_PCODE_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_PCODE_MASTER_1] ON [dbo].[M_PCODE_MASTER]
(
	[優先度] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_SCODE_JUDGE_2]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_SCODE_JUDGE_2] ON [dbo].[M_SCODE_JUDGE]
(
	[scode] ASC,
	[条件No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_SCODE_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_SCODE_MASTER_1] ON [dbo].[M_SCODE_MASTER]
(
	[優先度] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_ST_HISTORY_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_ST_HISTORY_MASTER_1] ON [dbo].[M_ST_HISTORY_MASTER]
(
	[区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_STATUS_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_STATUS_INF_1] ON [dbo].[M_STATUS_INF]
(
	[機器ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_M_STATUS_MASTER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_M_STATUS_MASTER_1] ON [dbo].[M_STATUS_MASTER]
(
	[機器ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_ACCESS_NEXT_COIL_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_ACCESS_NEXT_COIL_INF_1] ON [dbo].[T_ACCESS_NEXT_COIL_INF]
(
	[管理No] DESC,
	[事前編集済みフラグ] ASC,
	[次工程取得済みフラグ] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_T_ACCESS_NEXT_COIL_INF_2]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_ACCESS_NEXT_COIL_INF_2] ON [dbo].[T_ACCESS_NEXT_COIL_INF]
(
	[登録日付] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_ACCESS_NEXT_DEFECT_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_ACCESS_NEXT_DEFECT_INF_1] ON [dbo].[T_ACCESS_NEXT_DEFECT_INF]
(
	[管理No] DESC,
	[疵No] ASC,
	[表裏区分] ASC,
	[事前編集フラグ] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_CAMERA_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_CAMERA_1] ON [dbo].[T_COIL_CAMERA]
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_CAMERA_2]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_CAMERA_2] ON [dbo].[T_COIL_CAMERA]
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_CUT_POS_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_CUT_POS_1] ON [dbo].[T_COIL_CUT_POS]
(
	[管理No] DESC,
	[入側子番] ASC,
	[登録日付] DESC,
	[ID] DESC,
	[No] ASC,
	[先頭位置] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_CUT_RESULT_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_CUT_RESULT_1] ON [dbo].[T_COIL_CUT_RESULT]
(
	[管理No] DESC,
	[カットNo] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_INF_1] ON [dbo].[T_COIL_INF]
(
	[管理No] DESC,
	[登録日付] DESC,
	[キー情報] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_INF_2]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_INF_2] ON [dbo].[T_COIL_INF]
(
	[管理No] DESC,
	[生産No] ASC,
	[検査基準_表] ASC,
	[検査基準_裏] ASC,
	[品種] ASC,
	[主材質] ASC,
	[CAPL主材質] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_ORDER_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_ORDER_1] ON [dbo].[T_COIL_ORDER]
(
	[管理No] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_POSITION_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_POSITION_1] ON [dbo].[T_COIL_POSITION]
(
	[管理No] DESC,
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_RESULT_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_RESULT_1] ON [dbo].[T_COIL_RESULT]
(
	[管理No] DESC,
	[登録日付] DESC,
	[実績格納区分] ASC,
	[検査パターンコード_表] ASC,
	[検査パターンコード_裏] ASC,
	[運転状態] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_SAISUN_RESULT_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_SAISUN_RESULT_1] ON [dbo].[T_COIL_SAISUN_RESULT]
(
	[管理No] DESC,
	[採寸No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_COIL_TRAND_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_COIL_TRAND_1] ON [dbo].[T_COIL_TRAND]
(
	[管理No] DESC,
	[表裏区分] ASC,
	[取得位置] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR0_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR0_B_1] ON [dbo].[T_DEFECT_ATTR0_B]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR0_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR0_T_1] ON [dbo].[T_DEFECT_ATTR0_T]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR1_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR1_B_1] ON [dbo].[T_DEFECT_ATTR1_B]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR1_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR1_T_1] ON [dbo].[T_DEFECT_ATTR1_T]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR2_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR2_B_1] ON [dbo].[T_DEFECT_ATTR2_B]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR2_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR2_T_1] ON [dbo].[T_DEFECT_ATTR2_T]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR3_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR3_B_1] ON [dbo].[T_DEFECT_ATTR3_B]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR3_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR3_T_1] ON [dbo].[T_DEFECT_ATTR3_T]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR4_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR4_B_1] ON [dbo].[T_DEFECT_ATTR4_B]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_ATTR4_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_ATTR4_T_1] ON [dbo].[T_DEFECT_ATTR4_T]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_IMG_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_IMG_B_1] ON [dbo].[T_DEFECT_IMG_B]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_IMG_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_IMG_T_1] ON [dbo].[T_DEFECT_IMG_T]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_INF_B_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_INF_B_1] ON [dbo].[T_DEFECT_INF_B]
(
	[管理No] DESC,
	[疵No] ASC,
	[表裏区分] ASC,
	[重複結果] ASC,
	[判定疵種] ASC,
	[判定グレード] ASC,
	[カメラセット] ASC,
	[長手位置] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_DEFECT_INF_T_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_DEFECT_INF_T_1] ON [dbo].[T_DEFECT_INF_T]
(
	[管理No] DESC,
	[疵No] ASC,
	[表裏区分] ASC,
	[重複結果] ASC,
	[判定疵種] ASC,
	[判定グレード] ASC,
	[カメラセット] ASC,
	[長手位置] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_EDGE_CHANGE_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_EDGE_CHANGE_INF_1] ON [dbo].[T_EDGE_CHANGE_INF]
(
	[管理No] DESC,
	[長手位置] ASC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_EDGE_COIL_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_EDGE_COIL_INF_1] ON [dbo].[T_EDGE_COIL_INF]
(
	[管理No] DESC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_T_LOG_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_LOG_INF_1] ON [dbo].[T_LOG_INF]
(
	[アラームNo] ASC,
	[発生日付時刻] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_MAE_COIL_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_MAE_COIL_INF_1] ON [dbo].[T_MAE_COIL_INF]
(
	[管理No] DESC,
	[登録日付] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_MAE_COIL_INF_2]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_MAE_COIL_INF_2] ON [dbo].[T_MAE_COIL_INF]
(
	[キー情報] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_MAE_COIL_LINE_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_MAE_COIL_LINE_1] ON [dbo].[T_MAE_COIL_LINE]
(
	[管理No] DESC,
	[工程種別] ASC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_MAE_DEFECT_IMG_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_MAE_DEFECT_IMG_1] ON [dbo].[T_MAE_DEFECT_IMG]
(
	[管理No] DESC,
	[疵No] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_MAE_DEFECT_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_MAE_DEFECT_INF_1] ON [dbo].[T_MAE_DEFECT_INF]
(
	[管理No] DESC,
	[疵No] ASC,
	[工程種別] ASC,
	[表裏区分] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
SET ANSI_PADDING ON

GO
/****** Object:  Index [IX_T_MAE_FF_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_MAE_FF_INF_1] ON [dbo].[T_MAE_FF_INF]
(
	[管理No] DESC,
	[疵No] ASC,
	[工程種別] ASC,
	[表裏区分] ASC,
	[事前編集フラグ] ASC,
	[FF疵長さ] ASC,
	[判定疵種] ASC,
	[判定グレード] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_T_ST_HISTORY_INF_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_ST_HISTORY_INF_1] ON [dbo].[T_ST_HISTORY_INF]
(
	[登録日付] DESC,
	[区分] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_T_STATUS_LOG_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_T_STATUS_LOG_1] ON [dbo].[T_STATUS_LOG]
(
	[登録日付] DESC,
	[AutoNo] DESC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
/****** Object:  Index [IX_TYPE_NAME_1]    Script Date: 2024/08/19 19:42:12 ******/
CREATE NONCLUSTERED INDEX [IX_TYPE_NAME_1] ON [dbo].[TYPE_NAME]
(
	[EdasysID] ASC,
	[ID] ASC,
	[優先度] ASC,
	[Score] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_GuidanceAvailable]  DEFAULT ((0)) FOR [LongPd_GuidanceAvailable]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_GuidanceCheckInterval]  DEFAULT ((30)) FOR [LongPd_GuidanceCheckInterval]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_NearFurnaceRangeFrom]  DEFAULT ((2470)) FOR [LongPd_NearFurnaceRangeFrom]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_NearFurnaceRangeTo]  DEFAULT ((2600)) FOR [LongPd_NearFurnaceRangeTo]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_PeriodicPitchTolerance]  DEFAULT ((0)) FOR [LongPd_PeriodicPitchTolerance]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_GuidanceForeColor]  DEFAULT ((-65536)) FOR [LongPd_GuidanceForeColor]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_GuidanceBackColor]  DEFAULT ((-128)) FOR [LongPd_GuidanceBackColor]
GO
ALTER TABLE [dbo].[M_COMMON_MASTER] ADD  CONSTRAINT [DF_M_COMMON_MASTER_LongPd_GuidanceMessage]  DEFAULT ('長周期発生！確認してください。') FOR [LongPd_GuidanceMessage]
GO
ALTER TABLE [dbo].[PARAM_CAMANGLE_SCODE] ADD  CONSTRAINT [DF_PARAM_CAMANGLE_SCODE_NearEdgeNopenThr1]  DEFAULT ((0)) FOR [NearEdgeNopenThr1]
GO
ALTER TABLE [dbo].[PARAM_CAMSET_ANGLE_SCODE] ADD  CONSTRAINT [DF_PARAM_CAMSET_ANGLE_SCODE_NearEdgeThrLower1]  DEFAULT ((0)) FOR [NearEdgeThrLower1]
GO
ALTER TABLE [dbo].[PARAM_CAMSET_ANGLE_SCODE] ADD  CONSTRAINT [DF_PARAM_CAMSET_ANGLE_SCODE_NearEdgeThrLower2]  DEFAULT ((0)) FOR [NearEdgeThrLower2]
GO
ALTER TABLE [dbo].[PARAM_CAMSET_ANGLE_SCODE] ADD  CONSTRAINT [DF_PARAM_CAMSET_ANGLE_SCODE_NearEdgeThrUpper1]  DEFAULT ((255)) FOR [NearEdgeThrUpper1]
GO
ALTER TABLE [dbo].[PARAM_CAMSET_ANGLE_SCODE] ADD  CONSTRAINT [DF_PARAM_CAMSET_ANGLE_SCODE_NearEdgeThrUpper2]  DEFAULT ((255)) FOR [NearEdgeThrUpper2]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_ConnectMaxNFrames]  DEFAULT ((0)) FOR [ConnectMaxNFrames]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_ConnectMinLength]  DEFAULT ((0)) FOR [ConnectMinLength]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_ConnectMaxWidth]  DEFAULT ((0)) FOR [ConnectMaxWidth]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_ConnectMaxGap]  DEFAULT ((0)) FOR [ConnectMaxGap]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_DLBatchSize]  DEFAULT ((0)) FOR [DLBatchSize]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_DLModelName]  DEFAULT ('') FOR [DLModelName]
GO
ALTER TABLE [dbo].[PARAM_COMMON] ADD  CONSTRAINT [DF_PARAM_COMMON_DLDCDName]  DEFAULT ('') FOR [DLDCDName]
GO
ALTER TABLE [dbo].[PARAM_FILTERBIN] ADD  CONSTRAINT [DF_PARAM_FILTERBIN_NearEdgeApplyWidth]  DEFAULT ((0)) FOR [NearEdgeApplyWidth]
GO
ALTER TABLE [dbo].[PARAM_LABELIMAGE] ADD  CONSTRAINT [DF_PARAM_LABELIMAGE_bNearEdge]  DEFAULT ((0)) FOR [bNearEdge]
GO
ALTER TABLE [dbo].[PARAM_LABELIMAGE] ADD  CONSTRAINT [DF_PARAM_LABELIMAGE_NearEdgeDeadBand]  DEFAULT ((0)) FOR [NearEdgeDeadBand]
GO
ALTER TABLE [dbo].[PARAM_LABELIMAGE] ADD  CONSTRAINT [DF_PARAM_LABELIMAGE_NearEdgeApplyWidth]  DEFAULT ((0)) FOR [NearEdgeApplyWidth]
GO
ALTER TABLE [dbo].[PARAM_LABELIMAGE] ADD  CONSTRAINT [DF_PARAM_LABELIMAGE_name]  DEFAULT ('') FOR [name]
GO
ALTER TABLE [dbo].[PARAM_SCODE] ADD  CONSTRAINT [DF_PARAM_SCODE_NearEdgeDeadband]  DEFAULT ((0)) FOR [NearEdgeDeadband]
GO
ALTER TABLE [dbo].[PARAM_SCODE] ADD  CONSTRAINT [DF_PARAM_SCODE_NearEdgeApplyWidth]  DEFAULT ((0)) FOR [NearEdgeApplyWidth]
GO
ALTER TABLE [dbo].[PARAM_SCODE] ADD  CONSTRAINT [DF_PARAM_SCODE_NearEdgeNopen]  DEFAULT ((0)) FOR [NearEdgeNopen]
GO
ALTER TABLE [dbo].[PARAM_SCODE] ADD  CONSTRAINT [DF_PARAM_SCODE_NearEdgeNclose]  DEFAULT ((0)) FOR [NearEdgeNclose]
GO
ALTER TABLE [dbo].[PARAM_SCODE] ADD  CONSTRAINT [DF_PARAM_SCODE_NearEdgeNAreaFilterThr]  DEFAULT ((10)) FOR [NearEdgeNAreaFilterThr]
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "T_COIL_INF"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 235
            End
            DisplayFlags = 280
            TopColumn = 56
         End
         Begin Table = "T_COIL_RESULT"
            Begin Extent = 
               Top = 6
               Left = 273
               Bottom = 136
               Right = 514
            End
            DisplayFlags = 280
            TopColumn = 47
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 900
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_DEFECT_ADD_COIL_INF'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_DEFECT_ADD_COIL_INF'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "res"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 279
            End
            DisplayFlags = 280
            TopColumn = 0
         End
         Begin Table = "inf"
            Begin Extent = 
               Top = 6
               Left = 317
               Bottom = 136
               Right = 514
            End
            DisplayFlags = 280
            TopColumn = 0
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 2970
         Alias = 2640
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_COIL_INF'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_COIL_INF'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "inf"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 255
               Right = 253
            End
            DisplayFlags = 280
            TopColumn = 0
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1935
         Alias = 3480
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_DEFECT_INF_B'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_DEFECT_INF_B'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "inf"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 252
            End
            DisplayFlags = 280
            TopColumn = 5
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 2370
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_DEFECT_INF_B_WS'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_DEFECT_INF_B_WS'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "inf"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 252
            End
            DisplayFlags = 280
            TopColumn = 0
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 3600
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_DEFECT_INF_T'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_DEFECT_INF_T'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "T_LONGPD_EXEC_RESULT"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 233
            End
            DisplayFlags = 280
            TopColumn = 5
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 2130
         Alias = 900
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_EXEC_RESULT'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_EXEC_RESULT'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "T_LONGPD_EXEC_RESULT_LINE"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 213
            End
            DisplayFlags = 280
            TopColumn = 9
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 900
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_EXEC_RESULT_LINE'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_EXEC_RESULT_LINE'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "T_LONGPD_EXEC_RESULT_WIDTH"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 136
               Right = 214
            End
            DisplayFlags = 280
            TopColumn = 5
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 900
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_EXEC_RESULT_WIDTH'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_EXEC_RESULT_WIDTH'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "GRADE_NAME"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 190
               Right = 232
            End
            DisplayFlags = 280
            TopColumn = 4
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 1875
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_GRADE_NAME'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_GRADE_NAME'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPane1', @value=N'[0E232FF0-B466-11cf-A24F-00AA00A3EFFF, 1.00]
Begin DesignProperties = 
   Begin PaneConfigurations = 
      Begin PaneConfiguration = 0
         NumPanes = 4
         Configuration = "(H (1[40] 4[20] 2[20] 3) )"
      End
      Begin PaneConfiguration = 1
         NumPanes = 3
         Configuration = "(H (1 [50] 4 [25] 3))"
      End
      Begin PaneConfiguration = 2
         NumPanes = 3
         Configuration = "(H (1 [50] 2 [25] 3))"
      End
      Begin PaneConfiguration = 3
         NumPanes = 3
         Configuration = "(H (4 [30] 2 [40] 3))"
      End
      Begin PaneConfiguration = 4
         NumPanes = 2
         Configuration = "(H (1 [56] 3))"
      End
      Begin PaneConfiguration = 5
         NumPanes = 2
         Configuration = "(H (2 [66] 3))"
      End
      Begin PaneConfiguration = 6
         NumPanes = 2
         Configuration = "(H (4 [50] 3))"
      End
      Begin PaneConfiguration = 7
         NumPanes = 1
         Configuration = "(V (3))"
      End
      Begin PaneConfiguration = 8
         NumPanes = 3
         Configuration = "(H (1[56] 4[18] 2) )"
      End
      Begin PaneConfiguration = 9
         NumPanes = 2
         Configuration = "(H (1 [75] 4))"
      End
      Begin PaneConfiguration = 10
         NumPanes = 2
         Configuration = "(H (1[66] 2) )"
      End
      Begin PaneConfiguration = 11
         NumPanes = 2
         Configuration = "(H (4 [60] 2))"
      End
      Begin PaneConfiguration = 12
         NumPanes = 1
         Configuration = "(H (1) )"
      End
      Begin PaneConfiguration = 13
         NumPanes = 1
         Configuration = "(V (4))"
      End
      Begin PaneConfiguration = 14
         NumPanes = 1
         Configuration = "(V (2))"
      End
      ActivePaneConfig = 0
   End
   Begin DiagramPane = 
      Begin Origin = 
         Top = 0
         Left = 0
      End
      Begin Tables = 
         Begin Table = "TYPE_NAME"
            Begin Extent = 
               Top = 6
               Left = 38
               Bottom = 173
               Right = 240
            End
            DisplayFlags = 280
            TopColumn = 0
         End
      End
   End
   Begin SQLPane = 
   End
   Begin DataPane = 
      Begin ParameterDefaults = ""
      End
   End
   Begin CriteriaPane = 
      Begin ColumnWidths = 11
         Column = 1440
         Alias = 1740
         Table = 1170
         Output = 720
         Append = 1400
         NewValue = 1170
         SortType = 1350
         SortOrder = 1410
         GroupBy = 1350
         Filter = 1350
         Or = 1350
         Or = 1350
         Or = 1350
      End
   End
End
' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_TYPE_NAME'
GO
EXEC sys.sp_addextendedproperty @name=N'MS_DiagramPaneCount', @value=1 , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'VIEW',@level1name=N'V_LONGPD_TYPE_NAME'
GO
USE [master]
GO
ALTER DATABASE [DATABASE_N5CGL_TINY] SET  READ_WRITE 
GO
