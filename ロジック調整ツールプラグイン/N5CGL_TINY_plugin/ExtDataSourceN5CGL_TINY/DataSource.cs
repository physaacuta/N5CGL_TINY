using System;
using System.Collections.Generic;
using System.Text;

namespace ExtDataSourceN5CGL_TINY
{
    public class DataSource : SwdOnlineIF.IDataSource, SwdOnlineIF.IDataSourceDefectSearch
    {
        public DataSource()
        {
        }

        private DBAccessN5CGL_TINY.DBAccess m_db = new DBAccessN5CGL_TINY.DBAccess();

        private SwdData.ITermListConst m_termlist = null;

        private int[] m_defect_types = null;

        #region IDataSource メンバ

        public string Title
        {
            get { return "N5CGL_TINY(コイル一覧)"; }
        }

        public string Description
        {
            get { return "名古屋5CGL極微小検査装置のSQLサーバ内にある疵データ"; }
        }

        public string Path
        {
            get { return System.Reflection.Assembly.GetExecutingAssembly().Location; }
        }

        public string Version
        {
            get { return System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(); }
        }

        public void Init(System.Data.IDbConnection connection, int command_timeout)
        {
            m_db.CommandTimeout = command_timeout;

            // 初期化
            m_db.Init(connection);

            // 項目リスト
            m_termlist = m_db.GetTermlist(connection);

            // 疵種のIDを配列にする
            System.Collections.Generic.ICollection<SwdData.TermItem> types = m_termlist.GetSortedTermItems(SwdData.Terms.DefectType);
            m_defect_types = new int[types.Count];
            int i = 0;
            foreach (SwdData.TermItem type in types)
            {
                m_defect_types[i++] = type.Id;
            }
        }

        public void Dispose()
        {
            m_db.Dispose();
            m_termlist = null;
        }

        public SwdData.ITermListConst GetTermlist()
        {
            return m_termlist;
        }

        /// <summary>
        /// コイル一覧に表示する列名称
        /// </summary>
        public string[] ListColumnTitle
        {
            get
            {
                System.Collections.Generic.List<string> titles = new List<string>();

                titles.Add("登録日付");
                titles.Add("コイルNo");
                titles.Add("検査基準_表");
                titles.Add("検査基準_裏");
                titles.Add("主材質");
                titles.Add("表面処理1桁目");
                titles.Add("計画次工程コード");
                titles.Add("表疵個数");
                titles.Add("裏疵個数");
                foreach (int type in m_defect_types)
                {
                    titles.Add(m_termlist.GetName(SwdData.Terms.DefectType, type) + "(表)");
                    titles.Add(m_termlist.GetName(SwdData.Terms.DefectType, type) + "(裏)");
                }
                titles.Add("材料板幅[mm]");
                titles.Add("長[m]");
                titles.Add("注文板厚[mm]");
                titles.Add("表面状態(表)");
                titles.Add("表面状態(裏)");

                return titles.ToArray();
            }
        }

        /// <summary>
        /// コイル一覧に表示する列のデータ型
        /// </summary>
        public SwdData.ListViewSorter ListViewSorter
        {
            get
            {
                System.Collections.Generic.List<SwdData.ListViewSorter.ColumnTypes> types = new List<SwdData.ListViewSorter.ColumnTypes>();

                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Integer);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Integer);
                for (int i = 0; i < m_defect_types.Length; i++)
                {
                    types.Add(SwdData.ListViewSorter.ColumnTypes.Integer); // 表
                    types.Add(SwdData.ListViewSorter.ColumnTypes.Integer); // 裏
                }
                types.Add(SwdData.ListViewSorter.ColumnTypes.Float);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Float);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Float);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);

                return new SwdData.ListViewSorter(types.ToArray());
            }
        }

        /// <summary>
        /// コイル一覧の更新ボタン押下時
        /// </summary>
        public SwdOnlineIF.IProduct[] GetProducts(System.Data.IDbConnection connection, 
            DateTime from, DateTime to, bool count_ndefects, SwdData.IFormProgress progress)
        {
            DBAccessN5CGL_TINY.CoilListItem[] coils = m_db.GetCoils(connection, from, to, count_ndefects, false, progress);
            if (null == coils || 0 == coils.Length) return null;

            SwdOnlineIF.IProduct[] products = new SwdOnlineIF.IProduct[coils.Length];
            int i = 0;
            foreach (DBAccessN5CGL_TINY.CoilListItem coil in coils)
            {
                products[i++] = new Coil(coil, m_defect_types);
            }

            return products;
        }

        /// <summary>
        /// コイル一覧の検索ボタン押下時
        /// </summary>
        public SwdOnlineIF.IProduct[] GetProducts(System.Data.IDbConnection connection, string product_num, bool count_ndefects)
        {
            DBAccessN5CGL_TINY.CoilListItem[] coils = m_db.GetCoil(connection, product_num, count_ndefects, false);
            if (null == coils || 0 == coils.Length) return null;

            SwdOnlineIF.IProduct[] products = new SwdOnlineIF.IProduct[coils.Length];
            int i = 0;
            foreach (DBAccessN5CGL_TINY.CoilListItem coil in coils)
            {
                products[i++] = new Coil(coil, m_defect_types);
            }

            return products;
        }

        /// <summary>
        /// コイル一覧のコイル選択して疵情報表示時
        /// </summary>
        public int GetDefects(System.Data.IDbConnection connection,
            SwdOnlineIF.IProduct product, int[] types, int[] sevirities,
            float postop_from, float postop_to, float postail_from, float postail_to,
            SwdData.InspSurface insp_surface,
            SwdData.IDefectItemCollection defect_items, SwdData.IFormProgress progress)
        {
            Coil coil = product as Coil;
            if (null == coil) return 0;

            List<DBAccessN5CGL_TINY.DefectHandle> handles = new List<DBAccessN5CGL_TINY.DefectHandle>();
            if (float.IsNaN(postop_from) && float.IsNaN(postop_to) && float.IsNaN(postail_from) && float.IsNaN(postail_to))
            {
                // コイル全長が対象
                m_db.GetDefectHandle(connection, coil.CoilBase, types, sevirities, float.NaN, float.NaN, insp_surface, handles);
            }
            else
            {
                if (!float.IsNaN(postop_from) || !float.IsNaN(postop_to))
                {
                    m_db.GetDefectHandle(connection, coil.CoilBase, types, sevirities, postop_from, postop_to, insp_surface, handles);
                }
                if (!float.IsNaN(postail_from) || !float.IsNaN(postail_to))
                {
                    float from = float.IsNaN(postail_to) ? float.NaN : coil.ProductLength - postail_to;
                    float to = float.IsNaN(postail_from) ? float.NaN : coil.ProductLength - postail_from;
                    m_db.GetDefectHandle(connection, coil.CoilBase, types, sevirities, from, to, insp_surface, handles);
                }
            }
            if (0 == handles.Count) return 0;

            if (null != progress)
            {
                progress.Val = 0;
                progress.Max = handles.Count;
            }
            return m_db.GetDefectByHandle(connection, handles, defect_items, progress);
        }

        public bool IsPosTopRangeSupported
        {
            get { return true; }
        }

        public bool IsPosTailRangeSupported
        {
            get { return true; }
        }

        #endregion

        #region IDataSourceDefectSearch メンバ

        string SwdOnlineIF.IDataSourceDefectSearch.Title
        {
            get { return "N5CGL_TINY(疵検索)"; }
        }

        string SwdOnlineIF.IDataSourceDefectSearch.Description
        {
            get { return "名古屋5CGL極微小検査装置のSQLサーバ内にある疵データを検索する"; }
        }                

        public object[] SearchDefects(System.Data.IDbConnection connection, SwdOnlineIF.SearchCondition condition, SwdData.IFormProgress progress)
        {
            DBAccessN5CGL_TINY.NumFeatSearchCond[] numfeat_cond = null;
            if (null != condition.NumFeatureCond)
            {
                numfeat_cond = new DBAccessN5CGL_TINY.NumFeatSearchCond[condition.NumFeatureCond.Length];
                for (int i = 0; i < numfeat_cond.Length; i++)
                {
                    numfeat_cond[i] = new DBAccessN5CGL_TINY.NumFeatSearchCond(
                        condition.NumFeatureCond[i].Id,
                        condition.NumFeatureCond[i].Lower,
                        condition.NumFeatureCond[i].Upper);
                }
            }

            // 疵検索用 検索キーの入力状態取得
            string insp_level_up = null;
            string insp_level_dn = null;
            string prim_cntxt = null;
            string surf_proc_dig1 = null;
            string next_proc_sched = null;
            if (null != condition.ExtraSearchCond)
            {
                foreach (SwdOnlineIF.ExtraSearchCond cond in condition.ExtraSearchCond)
                {
                    switch (cond.Name)
                    {
                        case "検査基準_表":
                            insp_level_up = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "検査基準_裏":
                            insp_level_dn = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "主材料":
                            prim_cntxt = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "表面処理1桁目":
                            surf_proc_dig1 = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "計画次工程コード":
                            next_proc_sched = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;
                    }
                }
            }

            return m_db.SearchDefects(connection,
                condition.UseDateTime, condition.DateTimeFrom, condition.DateTimeTo,
                condition.ProductNum,
                condition.UseSurface, condition.Surface,
                condition.Types, condition.Sevirities,
                condition.UseWidth, condition.WidthLower, condition.WidthUpper,
                condition.UseLength, condition.LengthLower, condition.LengthUpper,
                condition.UsePosLeft, condition.PosLeftLower, condition.PosLeftUpper,
                condition.UsePosRight, condition.PosRightLower, condition.PosRightUpper,
                condition.UsePosTop, condition.PosTopLower, condition.PosTopUpper,
                numfeat_cond,
                insp_level_up,
                insp_level_dn,
                prim_cntxt,
                surf_proc_dig1,
                next_proc_sched,
                progress);
        }

        public int GetDefects(System.Data.IDbConnection connection, object[] handles, SwdData.IDefectItemCollection defect_items, SwdData.IFormProgress progress)
        {
            progress.Max = handles.Length;
            progress.Val = 0;
            int nerror = 0;
            foreach (DBAccessN5CGL_TINY.DefectHandle handle in handles)
            {
                SwdData.IDefectItem defect_item = m_db.GetDefectByHandle(connection, handle);
                if (null == defect_item)
                {
                    nerror++;
                }
                else
                {
                    defect_items.Add(defect_item);
                }
                progress.Val++;
                if (progress.IsCanceled) break;
            }
            return nerror;
        }

        public SwdOnlineIF.ExtraSearchCondConfig[] GetExtraSearchCondConfigs()
        {
            return new SwdOnlineIF.ExtraSearchCondConfig[]
            {
                // 疵検索 検索条件追加キーを定義
                new SwdOnlineIF.ExtraSearchCondConfig("検査基準_表",        typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("検査基準_裏",        typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("主材料",             typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("表面処理1桁目",      typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("計画次工程コード",   typeof(string))
            };
        }

        #endregion
    }
}
