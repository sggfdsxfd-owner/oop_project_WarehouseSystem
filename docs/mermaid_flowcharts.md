# 智慧物流系統 - Mermaid 流程圖集合

## 1. 主程式流程圖

```mermaid
flowchart TD
    Start([程式啟動]) --> Init["初始化<br/>- Database.load<br/>- Warehouse 建立<br/>- TUI 建立"]
    Init --> MainMenu["顯示主選單"]
    
    MainMenu --> Choice{使用者選擇}
    Choice -->|1. 登入| Login["handleLogin"]
    Choice -->|2. 新帳戶| Register["registerAccountFlow"]
    Choice -->|3. 結束| Exit["儲存數據<br/>- Database.saveCargo<br/>- Database.saveUsers<br/>- Logger.close"]
    
    Login --> LoginCheck{驗證成功?}
    LoginCheck -->|否| LoginFail["顯示失敗訊息<br/>Logger.log失敗"]
    LoginFail --> MainMenu
    LoginCheck -->|是| RoleCheck{檢查角色}
    
    RoleCheck -->|Admin| AdminMenu["showAdminMenu"]
    RoleCheck -->|Regular| OpMenu["showRegularMenu"]
    
    Register --> RegCheck{帳號已存在?}
    RegCheck -->|是| RegFail["提示失敗"]
    RegFail --> MainMenu
    RegCheck -->|否| RegSuccess["新增使用者<br/>Database.saveUsers<br/>Logger.log"]
    RegSuccess --> MainMenu
    
    AdminMenu --> AdminChoice{選擇操作}
    OpMenu --> OpChoice{選擇操作}
    
    AdminChoice -->|1. 查看庫存| ViewAll["listInventory<br/>showAll=true"]
    AdminChoice -->|2. 新增貨物| AddFlow["addCargoFlow"]
    AdminChoice -->|3. 下架貨物| RemoveFlow["removeCargoFlow"]
    AdminChoice -->|4. 重新上架| RelistFlow["relistCargoFlow"]
    AdminChoice -->|5. 查看統計| Stats["showStatistics"]
    AdminChoice -->|6. 登出| Logout["logout<br/>返回主選單"]
    
    OpChoice -->|1. 查看我的貨物| ViewOwn["listInventory<br/>showAll=false"]
    OpChoice -->|2. 新增貨物| AddFlow
    OpChoice -->|3. 下架貨物| RemoveFlow
    OpChoice -->|4. 重新上架| RelistFlow
    OpChoice -->|5. 登出| Logout
    
    ViewAll --> AdminMenu
    ViewOwn --> OpMenu
    Stats --> AdminMenu
    Logout --> MainMenu
    
    AddFlow --> MainMenu
    RemoveFlow --> MainMenu
    RelistFlow --> MainMenu
    
    Exit --> End([程式結束])
```

---

## 2. 登入與認證流程

```mermaid
flowchart TD
    Start([使用者選擇登入]) --> Input["輸入帳號"]
    Input --> PwdInput["輸入密碼<br/>readPassword<br/>遮蔽顯示"]
    PwdInput --> Search["warehouse.findUser<br/>std::find_if"]
    
    Search --> Found{使用者存在?}
    Found -->|否| Fail1["❌ 帳號不存在"]
    Found -->|是| Verify{密碼驗證}
    
    Verify -->|失敗| Fail2["❌ 密碼錯誤"]
    Verify -->|成功| Success["✅ 登入成功<br/>warehouse.login<br/>Logger.log"]
    
    Fail1 --> Retry{重試?}
    Fail2 --> Retry
    Retry -->|是| Input
    Retry -->|否| Cancel["返回主選單"]
    
    Success --> RoleCheck{檢查角色<br/>getRole}
    RoleCheck -->|Admin| AdminMenu["進入管理員選單"]
    RoleCheck -->|Regular| OpMenu["進入作業員選單"]
    
    Cancel --> End([結束認證])
    AdminMenu --> End
    OpMenu --> End
```

---

## 3. 新增貨物詳細流程

```mermaid
flowchart TD
    Start([新增貨物流程]) --> Input1["輸入貨物 ID<br/>readString"]
    Input1 --> Check{ID 已存在?}
    Check -->|是| Fail["❌ 此 ID 已存在"]
    Fail --> Retry{重試?}
    Retry -->|是| Input1
    Retry -->|否| Cancel["取消新增"]
    
    Check -->|否| Input2["輸入基本資訊<br/>- 名稱<br/>- 重量<br/>- 容積<br/>- 基本費率"]
    
    Input2 --> Category["分類選擇<br/>selectMenu"]
    Category --> CatChoice{選擇分類}
    CatChoice -->|預設選項| CatSelect["7個預設分類"]
    CatChoice -->|自訂| CustomCat["輸入自訂分類"]
    
    CatSelect --> TimeStamp["自動記錄 listingTime<br/>getCurrentTimestamp<br/>YYYY-MM-DD HH:MM:SS"]
    CustomCat --> TimeStamp
    
    TimeStamp --> TypeSelect["選擇貨物型別<br/>selectMenu"]
    TypeSelect --> TypeChoice{型別選擇}
    
    TypeChoice -->|1. 一般貨物| General["輸入資訊<br/>std::make_shared<br/>Cargo"]
    TypeChoice -->|2. 危險品| Dangerous["輸入資訊<br/>- 危險等級 1-9<br/>- UN 編號<br/>std::make_shared<br/>DangerousCargo"]
    TypeChoice -->|3. 易腐品| Perishable["輸入資訊<br/>- 到期日<br/>- 溫控溫度<br/>std::make_shared<br/>PerishableCargo"]
    TypeChoice -->|4. 易碎品| Fragile["輸入資訊<br/>- 包裝型態<br/>- 堆疊限制<br/>std::make_shared<br/>FragileCargo"]
    
    General --> Add["warehouse.addCargo<br/>vector.push_back"]
    Dangerous --> Add
    Perishable --> Add
    Fragile --> Add
    
    Add --> Save["Database.saveCargo<br/>寫入 data/cargo.txt"]
    Save --> Log["Logger.log<br/>記錄事件"]
    Log --> Success["✅ 新增成功"]
    
    Success --> End([結束])
    Cancel --> End
```

---

## 4. 下架與重新上架流程

```mermaid
flowchart TD
    Start1([下架貨物流程]) --> GetCargo["取得使用者的 Active 貨物<br/>std::find_if<br/>isListed() == true"]
    GetCargo --> HasCargo{有可下架的貨物?}
    HasCargo -->|否| NoData["❌ 無可下架貨物"]
    NoData --> End1([結束])
    
    HasCargo -->|是| Select["selectMenu 顯示清單"]
    Select --> Choose["使用者選擇貨物"]
    Choose --> Confirm["確認下架?"]
    Confirm -->|否| Cancel["取消操作"]
    Confirm -->|是| Unlist["warehouse.removeCargo<br/>設定 status='Unlisted'"]
    
    Unlist --> Save1["Database.saveCargo<br/>寫入 data/cargo.txt"]
    Save1 --> Log1["Logger.log<br/>記錄下架事件"]
    Log1 --> Success1["✅ 下架成功"]
    
    Success1 --> End1
    Cancel --> End1
    
    Start2([重新上架流程]) --> GetCargo2["取得使用者的 Unlisted 貨物<br/>std::find_if<br/>isListed() == false"]
    GetCargo2 --> HasCargo2{有可重新上架的貨物?}
    HasCargo2 -->|否| NoData2["❌ 無可重新上架貨物"]
    NoData2 --> End2([結束])
    
    HasCargo2 -->|是| Select2["selectMenu 顯示清單"]
    Select2 --> Choose2["使用者選擇貨物"]
    Choose2 --> Confirm2["確認重新上架?"]
    Confirm2 -->|否| Cancel2["取消操作"]
    Confirm2 -->|是| Relist["warehouse.relistCargo<br/>設定 status='Active'"]
    
    Relist --> Save2["Database.saveCargo<br/>寫入 data/cargo.txt"]
    Save2 --> Log2["Logger.log<br/>記錄重新上架事件"]
    Log2 --> Success2["✅ 重新上架成功"]
    
    Success2 --> End2
    Cancel2 --> End2
```

---

## 5. 多型計費流程

```mermaid
flowchart TD
    Start([計算倉庫總費用]) --> Loop["for 每個 cargo in warehouse.inventory"]
    Loop --> CheckType{檢查型別<br/>cargo->getType()}
    
    CheckType -->|General| General["baseFee = baseRate × volume"]
    CheckType -->|Dangerous| Dangerous["baseFee = baseRate × volume<br/>safeFee = baseFee × 0.2<br/>monitoring = 100<br/>total = baseFee + safeFee + monitoring"]
    CheckType -->|Perishable| Perishable["baseFee = baseRate × volume<br/>tempCheck{requiredTemp}"]
    CheckType -->|Fragile| Fragile["baseFee = baseRate × volume<br/>stackCheck{maxStackHeight}"]
    
    Perishable --> TempCond{溫度條件}
    TempCond -->|≤ 0°C| ColdFee1["coldFee = baseFee × 0.3<br/>total = baseFee + coldFee"]
    TempCond -->|≤ 10°C| ColdFee2["coldFee = baseFee × 0.15<br/>total = baseFee + coldFee"]
    TempCond -->|> 10°C| NoColdFee["total = baseFee"]
    
    Fragile --> StackCond{堆疊限制}
    StackCond -->|height ≤ 2| FloorFee["floorFee = 200<br/>total = baseFee + floorFee"]
    StackCond -->|height > 2| NoFloorFee["total = baseFee"]
    
    General --> Sum["Sum += total"]
    Dangerous --> Sum
    ColdFee1 --> Sum
    ColdFee2 --> Sum
    NoColdFee --> Sum
    FloorFee --> Sum
    NoFloorFee --> Sum
    
    Sum --> NextCargo{還有更多貨物?}
    NextCargo -->|是| Loop
    NextCargo -->|否| Result["totalRevenue = Sum<br/>返回結果"]
    Result --> End([結束])
```

---

## 6. 類別層次結構圖

```mermaid
graph TD
    Cargo["<b>Cargo 基底類別</b><br/>■ id: string<br/>■ name: string<br/>■ weight: double<br/>■ volume: double<br/>■ baseRate: double<br/>■ owner: string<br/>■ category: string ⭐<br/>■ listingTime: string ⭐<br/>■ status: string ⭐<br/>─────────────<br/>▶ calculateStorageFee()<br/>▶ printDetails()<br/>▶ setStatus()"]
    
    General["<b>General</b><br/>一般貨物<br/>─────────────<br/>calculateStorageFee():<br/>return baseRate × volume"]
    Dangerous["<b>DangerousCargo</b><br/>危險品<br/>─────────────<br/>■ hazardLevel: int<br/>■ unNumber: string<br/>─────────────<br/>calculateStorageFee():<br/>base + base×0.2 + 100"]
    Perishable["<b>PerishableCargo</b><br/>易腐品<br/>─────────────<br/>■ expiryDate: string<br/>■ requiredTemp: double<br/>─────────────<br/>calculateStorageFee():<br/>base + coldChainFee"]
    Fragile["<b>FragileCargo</b><br/>易碎品<br/>─────────────<br/>■ packagingType: string<br/>■ maxStackHeight: int<br/>─────────────<br/>calculateStorageFee():<br/>base + floorFee"]
    
    User["<b>User 基底類別</b><br/>■ username: string<br/>■ password: string<br/>■ role: string<br/>─────────────<br/>▶ canManageInventory()?<br/>▶ canManageUsers()?"]
    
    Admin["<b>AdminUser</b><br/>管理員<br/>─────────────<br/>canManageInventory(): true<br/>canManageUsers(): true"]
    Regular["<b>RegularUser</b><br/>一般作業員<br/>─────────────<br/>canManageInventory(): false<br/>canManageUsers(): false"]
    
    Cargo --> General
    Cargo --> Dangerous
    Cargo --> Perishable
    Cargo --> Fragile
    
    User --> Admin
    User --> Regular
    
    style Cargo fill:#e1f5ff
    style General fill:#f1f8e9
    style Dangerous fill:#fff3e0
    style Perishable fill:#fce4ec
    style Fragile fill:#f3e5f5
    style User fill:#e8f5e9
    style Admin fill:#c8e6c9
    style Regular fill:#dcedc8
```

---

## 7. CSV 資料持久化流程

```mermaid
flowchart TD
    Start([系統啟動]) --> Load1["Database.loadUsers"]
    Load1 --> Read1["讀取 data/users.txt"]
    Read1 --> Parse1["CSV 解析<br/>split by ','"]
    Parse1 --> Create1["for 每行:<br/>std::make_shared&lt;User&gt;<br/>warehouse.addUser"]
    
    Create1 --> Load2["Database.loadCargo"]
    Load2 --> Read2["讀取 data/cargo.txt"]
    Read2 --> Parse2["CSV 解析<br/>split by ','"]
    Parse2 --> TypeCheck["根據 Type 欄位<br/>std::dynamic_pointer_cast"]
    
    TypeCheck --> CreateCargo["for 每行:<br/>std::make_shared&lt;CargoType&gt;<br/>warehouse.addCargo"]
    CreateCargo --> Ready["✅ 資料載入完成"]
    
    Ready --> Running["系統運行<br/>使用者操作"]
    Running --> Changes{發生變化?}
    
    Changes -->|新增/修改貨物| Save1["Database.saveCargo<br/>遍歷 inventory<br/>輸出 CSV"]
    Changes -->|新增使用者| Save2["Database.saveUsers<br/>遍歷 users<br/>輸出 CSV"]
    Changes -->|操作完成| Log["Logger.log<br/>記錄到 system.log"]
    
    Save1 --> Write1["寫入 data/cargo.txt"]
    Save2 --> Write2["寫入 data/users.txt"]
    Log --> Changes
    
    Write1 --> Changes
    Write2 --> Changes
    
    Changes -->|程式結束| Exit["Database 儲存所有<br/>程式結束"]
    Exit --> End([結束])
```

---

## 8. 密碼遮蔽輸入流程

```mermaid
flowchart TD
    Start([readPassword]) --> Prompt["顯示提示訊息"]
    Prompt --> Input["等待使用者輸入"]
    
    Input --> Key{按鍵類型?}
    
    Key -->|普通字元| Append["密碼變數 += 字元<br/>顯示 '*'"]
    Key -->|Backspace| Delete["if 密碼非空<br/>密碼變數 pop_back<br/>刪除最後的 '*'"]
    Key -->|Enter| Return["返回完整密碼"]
    Key -->|其他| Ignore["忽略"]
    
    Append --> Input
    Delete --> Input
    Ignore --> Input
    
    Return --> End([結束])
    
    style Start fill:#90caf9
    style End fill:#90caf9
```

---

## 9. 統計報表流程

```mermaid
flowchart TD
    Start([showStatistics]) --> Count["計算統計數據"]
    
    Count --> Total["totalCount = inventory.size"]
    Count --> Volume["totalVolume = Σ cargo.getVolume"]
    Count --> Revenue["totalRevenue = calculateTotalRevenue<br/>展現多型計費"]
    
    Total --> Display["顯示統計結果"]
    Volume --> Display
    Revenue --> Display
    
    Display --> ByType["按型別統計<br/>std::count_if"]
    ByType --> CountGeneral["Count General"]
    ByType --> CountDangerous["Count Dangerous"]
    ByType --> CountPerishable["Count Perishable"]
    ByType --> CountFragile["Count Fragile"]
    
    CountGeneral --> BarChart["繪製 ASCII 柱狀圖"]
    CountDangerous --> BarChart
    CountPerishable --> BarChart
    CountFragile --> BarChart
    
    BarChart --> Warnings["檢查危險品警告"]
    Warnings --> CheckHazard{"是否有<br/>危險品?"}
    
    CheckHazard -->|是| Alert["⚠️ 危險品警告<br/>顯示詳細資訊"]
    CheckHazard -->|否| NoAlert["無警告"]
    
    Alert --> End([結束])
    NoAlert --> End
```

---

## 使用方式

本文件包含 9 個 Mermaid 流程圖，可用於：
- 📚 系統文檔
- 🎓 課程教學
- 💼 專案簡報
- 📝 技術規格書

### 支援的格式：
- ✅ Markdown（本文件）
- ✅ GitHub / GitLab Wiki
- ✅ Notion / Confluence
- ✅ HTML（comprehensive_report.html 中已嵌入）
- ✅ PlantUML 編輯器

### 快速查看：
1. VS Code 中使用 Markdown Preview 延伸功能
2. GitHub 上直接查看此 .md 文件
3. 複製代碼到 [mermaid.live](https://mermaid.live) 線上編輯器
