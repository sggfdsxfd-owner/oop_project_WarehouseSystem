# 核心模組與 UML 圖

## 核心模組總覽

| 模組 | 說明 |
|------|------|
| `Cargo` / `Cargo.cpp` | 核心資料模型，定義貨物通用屬性與多型計費。|
| `DangerousCargo` / `DangerousCargo.cpp` | 危險品衍生類別，加入危險等級、UN 編號與額外安全費用。|
| `PerishableCargo` / `PerishableCargo.cpp` | 易腐品衍生類別，加入到期日、溫控需求與冷鏈費用計算。|
| `FragileCargo` / `FragileCargo.cpp` | 易碎品衍生類別，加入包裝型態、堆疊限制與地板佔用費。|
| `User` / `User.cpp` | 使用者基底類別，定義帳號、密碼、角色與權限檢查。|
| `Warehouse` / `Warehouse.cpp` | 倉庫核心管理，管理庫存、使用者、登入、下架、重新上架、統計。|
| `Database` / `Database.cpp` | 檔案存取與序列化，負責 CSV 讀寫與物件重建。|
| `TUI` / `TUI.cpp` | 終端機操作介面，呈現選單、輸入防呆、遮蔽密碼、顯示結果。|
| `Logger` / `Logger.cpp` | 系統日誌，記錄操作事件與稽核軌跡。|

---

## 模組說明

### Cargo 模組
- **職責**：定義貨物基本欄位與通用計費方法。
- **功能**：多型計費、詳細輸出、狀態管理（Active / Unlisted）。

### DangerousCargo / PerishableCargo / FragileCargo
- **DangerousCargo**：危險等級、UN 編號、加收安全費用。
- **PerishableCargo**：到期日、溫控需求、動態冷鏈費。
- **FragileCargo**：包裝型態、堆疊高度、地板佔用費。

### User 模組
- **職責**：帳號管理與角色授權。
- **功能**：管理員、一般作業員權限判斷；驗證密碼；權限控制。 

### Warehouse 模組
- **職責**：統一業務邏輯與資料管理。
- **功能**：新增貨物、下架、重新上架、查詢、登入、統計、角色限制。

### Database 模組
- **職責**：持久化與資料重建。
- **功能**：CSV 讀寫、向後相容、動態建立正確貨物類別。

### TUI 模組
- **職責**：終端機操作流程與使用者互動。
- **功能**：選單式輸入、遮蔽密碼、彩色提示、流程導向。 

### Logger 模組
- **職責**：操作稽核與系統日誌記錄。
- **功能**：寫入 `data/system.log`、記錄登入、註冊、新增、下架、重新上架等事件。

---

## 類別圖 (Class Diagram)

```mermaid
classDiagram
    class Cargo {
        +string id
        +string name
        +double weight
        +double volume
        +double baseRate
        +string owner
        +string category
        +string listingTime
        +string status
        +double calculateStorageFee()
        +void printDetails()
        +void setStatus(string)
        +bool isListed()
    }

    class DangerousCargo {
        +int hazardLevel
        +string unNumber
        +double calculateStorageFee()
    }

    class PerishableCargo {
        +string expiryDate
        +double requiredTemp
        +double calculateStorageFee()
    }

    class FragileCargo {
        +string packagingType
        +int maxStackHeight
        +double calculateStorageFee()
    }

    class User {
        +string username
        +string password
        +string role
        +bool verifyPassword(string)
        +bool canManageInventory()
        +bool canManageUsers()
    }

    class AdminUser {
        +bool canManageInventory()
        +bool canManageUsers()
    }

    class RegularUser {
        +bool canManageInventory()
        +bool canManageUsers()
    }

    class Warehouse {
        -vector<shared_ptr<Cargo>> inventory
        -vector<shared_ptr<User>> users
        -shared_ptr<User> currentUser
        +bool login(string, string)
        +void logout()
        +bool addCargo(shared_ptr<Cargo>)
        +bool removeCargo(string)
        +bool relistCargo(string)
        +shared_ptr<Cargo> findCargo(string)
        +double calculateTotalRevenue()
    }

    class Database {
        -string usersFilePath
        -string cargoFilePath
        +bool loadUsers(Warehouse&)
        +bool saveUsers(const Warehouse&)
        +bool loadCargo(Warehouse&)
        +bool saveCargo(const Warehouse&)
    }

    class TUI {
        -Warehouse& warehouse
        -Database& database
        +void run()
        +void handleLogin()
        +void addCargoFlow()
        +void removeCargoFlow()
        +void relistCargoFlow()
    }

    class Logger {
        -ofstream logFile
        +bool log(string)
    }

    Cargo <|-- DangerousCargo
    Cargo <|-- PerishableCargo
    Cargo <|-- FragileCargo
    User <|-- AdminUser
    User <|-- RegularUser
    Warehouse --> Cargo
    Warehouse --> User
    Database --> Warehouse
    TUI --> Warehouse
    TUI --> Database
    TUI --> Logger
```

---

## 序列圖 (Sequence Diagram)

### 1. 登入流程

```mermaid
sequenceDiagram
    participant User as 使用者
    participant TUI as TUI
    participant Warehouse as Warehouse
    participant Logger as Logger

    User->>TUI: 輸入帳號/密碼
    TUI->>Warehouse: login(username, password)
    Warehouse->>Warehouse: findUser(username)
    Warehouse->>Warehouse: verifyPassword(password)
    Warehouse-->>TUI: 登入結果
    alt 成功
        TUI->>Logger: log("登入成功")
        TUI-->>User: 顯示登入成功
    else 失敗
        TUI->>Logger: log("登入失敗")
        TUI-->>User: 顯示錯誤訊息
    end
```

### 2. 新增貨物流程

```mermaid
sequenceDiagram
    participant User as 使用者
    participant TUI as TUI
    participant Warehouse as Warehouse
    participant Database as Database
    participant Logger as Logger

    User->>TUI: 選擇新增貨物
    TUI->>User: 輸入貨物資料
    User-->>TUI: 提交貨物資訊
    TUI->>Warehouse: addCargo(cargo)
    Warehouse-->>TUI: 新增成功
    TUI->>Database: saveCargo(warehouse)
    Database-->>TUI: 儲存完成
    TUI->>Logger: log("新增貨物")
    TUI-->>User: 顯示新增成功
```

---

## 使用說明

1. 開啟 `docs/core_module_diagrams.md` 於 VS Code
2. 使用 Mermaid Preview 或 Markdown Preview 擴充套件
3. 若要轉成 PDF，可直接在瀏覽器預覽後列印
