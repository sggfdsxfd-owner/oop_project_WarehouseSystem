# 類別架構與方法說明文件 (Class Architecture & API Documentation)

本文件詳細列出「智慧物流與倉庫管理系統」的類別架構圖、繼承關係，以及各個類別之成員變數、建構子與方法的詳細功能說明。

---

## 1. 類別架構關係圖 (Class Diagram)

我們使用 **Mermaid** 語法繪製系統類別圖，呈現貨物、使用者與管理核心之間的關係：

```mermaid
classDiagram
    %% 貨物繼承關係
    Cargo <|-- DangerousCargo
    Cargo <|-- PerishableCargo
    Cargo <|-- FragileCargo

    %% 使用者繼承關係
    User <|-- AdminUser
    User <|-- RegularUser

    %% 關聯關係
    Warehouse "1" *-- "many" Cargo : 以 std::vector 存放
    Warehouse "1" *-- "many" User : 以 std::vector 存放
    Warehouse "1" o-- "1" User : 當前登入者 (currentUser)

    TUI ..> Warehouse : 呼叫核心業務邏輯
    TUI ..> Database : 觸發資料存檔與讀取
    Database ..> Warehouse : 序列化/反序列化資料

    class Cargo {
        #string id
        #string name
        #double weight
        #double volume
        #double baseRate
        #string owner
        +calculateStorageFee() double*
        +printDetails() void*
        +getType() string*
    }

    class DangerousCargo {
        -int hazardLevel
        -string unNumber
        +calculateStorageFee() double override
        +printDetails() void override
    }

    class PerishableCargo {
        -string expiryDate
        -double requiredTemp
        +calculateStorageFee() double override
        +printDetails() void override
    }

    class FragileCargo {
        -string packagingType
        -int maxStackHeight
        +calculateStorageFee() double override
        +printDetails() void override
    }

    class User {
        #string username
        #string password
        #string role
        +verifyPassword(string) bool
        +canManageInventory() bool*
        +canManageUsers() bool*
    }

    class AdminUser {
        +canManageInventory() bool override
        +canManageUsers() bool override
    }

    class RegularUser {
        +canManageInventory() bool override
        +canManageUsers() bool override
    }

    class Warehouse {
        -vector~shared_ptr~Cargo~~ inventory
        -vector~shared_ptr~User~~ users
        -shared_ptr~User~ currentUser
        +login(string, string) bool
        +logout() void
        +addCargo(shared_ptr~Cargo~) bool
        +removeCargo(string) bool
        +findCargo(string) shared_ptr~Cargo~
        +calculateTotalVolume() double
        +calculateTotalRevenue() double
    }
```

---

## 2. 貨物類別群說明 (Cargo Classes)

### 2.1 Cargo (基底類別)
代表一般通用貨物，也是所有特定貨物類別的父類別。

*   **成員變數**：
    *   `std::string id` (受保護的)：貨物唯一識別碼。
    *   `std::string name` (受保護的)：貨物名稱。
    *   `double weight` (受保護的)：貨物總重量 (kg)。
    *   `double volume` (受保護的)：貨物占用容積 (m³)。
    *   `double baseRate` (受保護的)：每立方公尺月租費基本費率。
    *   `std::string owner` (受保護的)：該貨物的所有權人帳號名稱。
*   **建構子**：
    *   `Cargo(id, name, weight, volume, baseRate, owner)`：初始化所有成員變數。
*   **成員方法**：
    *   `virtual double calculateStorageFee() const`：計算該貨物的倉儲費，預設公式為 `volume * baseRate`。
    *   `virtual void printDetails() const`：輸出格式化後的通用貨物資訊行。
    *   `virtual std::string getType() const`：回傳該貨物的類別名稱標籤，預設回傳 `"General"`。
    *   `Getters / Setters`：存取內部屬性。

### 2.2 DangerousCargo (危險品類別)
繼承自 `Cargo`，用於存放具化學性、放射性或易燃的危險材料。

*   **成員變數**：
    *   `int hazardLevel` (私有的)：危險等級，數值為 1-9。
    *   `std::string unNumber` (私有的)：聯合國危險物編號。
*   **建構子**：
    *   `DangerousCargo(..., hazardLevel, unNumber)`：調用父類別建構子，並初始化危險參數。
*   **重寫方法 (Overrides)**：
    *   `double calculateStorageFee() const override`：重寫費率公式，依危險等級做倍數加成，並另加 $500$ 元之安全保險基本費。
    *   `void printDetails() const override`：先呼叫父類別 `printDetails`，隨後換行輸出危險等級與 UN 編號等特殊資訊。
    *   `std::string getType() const override`：回傳 `"Dangerous"`。

### 2.3 PerishableCargo (易腐品類別)
繼承自 `Cargo`，專用於需要控溫存放的生鮮食品或醫藥原料。

*   **成員變數**：
    *   `std::string expiryDate` (私有的)：格式為 `YYYY-MM-DD` 的有效期限。
    *   `double requiredTemp` (私有的)：儲存溫度限制 (°C)。
*   **建構子**：
    *   `PerishableCargo(..., expiryDate, requiredTemp)`：調用父類別建構子，並初始化溫控與過期參數。
*   **重寫方法 (Overrides)**：
    *   `double calculateStorageFee() const override`：重寫費率。小於 0 °C 追加冷凍級電費（$300 \times \text{體積}$）；小於等於 8 °C 追加冷藏級電費（$150 \times \text{體積}$）。
    *   `void printDetails() const override`：輸出基本資訊後，換行輸出保存期限與溫控需求。
    *   `std::string getType() const override`：回傳 `"Perishable"`。

### 2.4 FragileCargo (易碎品類別)
繼承自 `Cargo`，專用於精細儀器、玻璃陶瓷等易破碎、有堆疊限制的物品。

*   **成員變數**：
    *   `std::string packagingType` (私有的)：防護包裝方式（木箱/氣泡紙）。
    *   `int maxStackHeight` (私有的)：最大容許堆疊層數。
*   **建構子**：
    *   `FragileCargo(..., packagingType, maxStackHeight)`：調用父類別建構子，並初始化易碎專用欄位。
*   **重寫方法 (Overrides)**：
    *   `double calculateStorageFee() const override`：重寫費率。若最大堆疊層數 $\le 2$，則加收地板補償費；並根據包裝的防護複雜度（如木箱加收 $100$ 元）計算手續費。
    *   `void printDetails() const override`：輸出後，額外換行提示包裝防護類型及堆疊限制層數。
    *   `std::string getType() const override`：回傳 `"Fragile"`。

---

## 3. 使用者管理群說明 (User Classes)

### 3.1 User (基底類別)
用以實作使用者身分與基本權限管理。

*   **成員變數**：
    *   `std::string username` (受保護的)：登入帳號。
    *   `std::string password` (受保護的)：登入密碼（明文或雜湊）。
    *   `std::string role` (受保護的)：使用者角色標記（`"Admin"` 或 `"Regular"`）。
*   **成員方法**：
    *   `bool verifyPassword(const std::string& pwd) const`：驗證輸入密碼是否相符。
    *   `virtual bool canManageInventory() const`：多型權限判定，預設回傳 `false`。
    *   `virtual bool canManageUsers() const`：多型權限判定，預設回傳 `false`。

### 3.2 AdminUser (管理員類別)
繼承自 `User`。代表系統管理者，具備核心異動與財務審查權限。

*   **重寫方法 (Overrides)**：
    *   `bool canManageInventory() const override`：回傳 `true`（允許檢視與刪除他人貨物）。
    *   `bool canManageUsers() const override`：回傳 `true`（允許新增系統帳號）。

### 3.3 RegularUser (一般作業員類別)
繼承自 `User`。代表一般客戶或基層倉儲人員。

*   **重寫方法 (Overrides)**：
    *   `bool canManageInventory() const override`：回傳 `false`（僅能新增與異動自身申報的貨物）。
    *   `bool canManageUsers() const override`：回傳 `false`（拒絕帳號管理）。

---

## 4. 倉儲核心與檔案管理類別 (Warehouse & Database)

### 4.1 Warehouse (倉儲管理核心)
系統運算的核心，以記憶體結構管理物件容器。

*   **成員變數**：
    *   `std::vector<std::shared_ptr<Cargo>> inventory`：儲存多型貨物指標的容器。
    *   `std::vector<std::shared_ptr<User>> users`：儲存多型使用者指標的容器。
    *   `std::shared_ptr<User> currentUser`：目前系統已登入的使用者會話 (Session)。
*   **核心業務方法**：
    *   `bool login(username, password)`：遍歷 `users`，找到帳號且密碼相符後，將該指標賦予 `currentUser` 並回傳 `true`。
    *   `void logout()`：將 `currentUser` 設為 `nullptr`。
    *   `bool addCargo(shared_ptr<Cargo>)`：加入新貨物，重複 ID 檢查。
    *   `bool removeCargo(id)`：透過 `std::find_if` 搜尋對應 ID 並將其自容器中移除。
    *   `double calculateTotalVolume() const`：計算所有庫存總容積需求。
    *   `double calculateTotalRevenue() const`：計算所有庫存之多型倉儲收益加總。

### 4.2 Database (持久化模組)
負責記憶體狀態與 CSV 檔案之間的轉換序列化工作。

*   **成員方法**：
    *   `bool loadUsers(Warehouse& warehouse)`：自檔案逐行解析角色、帳號、密碼，利用 Downcasting 還原為 `AdminUser` 或 `RegularUser` 並塞入 `Warehouse`。
    *   `bool saveUsers(const Warehouse& warehouse)`：將所有使用者狀態輸出至檔案。
    *   `bool loadCargo(Warehouse& warehouse)`：自 CSV 解析貨物並透過型態欄位，動態將對應欄位塞入 `Dangerous`/`Perishable`/`Fragile` 之智慧指標中。
    *   `bool saveCargo(const Warehouse& warehouse)`：依據各貨物多型判定，將特定專屬欄位追加寫入檔案。
