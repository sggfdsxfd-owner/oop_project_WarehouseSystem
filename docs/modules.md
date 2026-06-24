# 核心模組詳細說明

## 目錄
1. [Cargo - 貨物基底類別](#cargo-貨物基底類別)
2. [DangerousCargo / PerishableCargo / FragileCargo - 衍生類別](#衍生類別)
3. [User - 使用者權限管制](#user-使用者權限管制)
4. [Warehouse - 倉庫核心管理](#warehouse-倉庫核心管理)
5. [Database - 檔案 I/O 與序列化](#database-檔案-io-與序列化)
6. [TUI - 終端機使用者介面](#tui-終端機使用者介面)
7. [Logger - 系統日誌](#logger-系統日誌)

---

## Cargo - 貨物基底類別

### 設計目的
定義所有貨物的共通屬性與行為，採用**虛擬函式**支援多型計費與輸出。

### 類別定義
```cpp
class Cargo {
protected:
    std::string id;              // 貨物編號
    std::string name;            // 貨物名稱
    double weight;               // 重量 (kg)
    double volume;               // 容積 (m³)
    double baseRate;             // 基本費率 (元/月/m³)
    std::string owner;           // 擁有者帳號
    std::string category;        // 分類 (新增)
    std::string listingTime;     // 上架時間 (新增)
    std::string status;          // 狀態：Active / Unlisted (新增)

public:
    // 建構子
    Cargo(const std::string& id, const std::string& name, 
          double weight, double volume, double baseRate, 
          const std::string& owner, 
          const std::string& category, 
          const std::string& listingTime,
          const std::string& status = "Active");

    // 存取方法
    std::string getId() const;
    std::string getName() const;
    double getWeight() const;
    double getVolume() const;
    double getBaseRate() const;
    std::string getOwner() const;
    std::string getCategory() const;
    std::string getListingTime() const;
    std::string getStatus() const;
    std::string getType() const { return "General"; }
    
    // 狀態設定
    void setStatus(const std::string& newStatus);
    bool isListed() const { return status == "Active"; }

    // 虛擬方法（展現多型）
    virtual double calculateStorageFee() const;
    virtual void printDetails() const;
};
```

### 主要功能

#### 1. **calculateStorageFee()** - 計費邏輯
```cpp
double Cargo::calculateStorageFee() const {
    // 基本計費：基本費率 × 容積
    return baseRate * volume;
}
```
- **說明**：一般貨物以 `baseRate × volume` 計算月儲費。
- **多型應用**：衍生類別覆寫此方法加上特殊費用。

#### 2. **printDetails()** - 多行輸出
```cpp
void Cargo::printDetails() const {
    std::cout << std::left << std::setw(8) << id
              << std::setw(15) << name
              << std::setw(10) << category
              << std::right << std::setw(13) << weight
              << std::setw(13) << volume
              << std::setw(15) << calculateStorageFee() << std::endl;
}
```
- **說明**：格式化輸出貨物資訊，供清單顯示使用。
- **衍生類別擴充**：危險品加入警告標記、易腐品加入溫控標記等。

#### 3. **狀態管理**
- `isListed()`：檢查是否為上架狀態（Active）。
- `setStatus()`：更新狀態為 Active 或 Unlisted。

### 使用示例
```cpp
// 建立一般貨物
auto cargo = std::make_shared<Cargo>(
    "C001",           // ID
    "普通箱子",       // 名稱
    100,              // 重量
    2,                // 容積
    50,               // 基本費率
    "admin",          // 擁有者
    "未分類",         // 分類
    "2026-06-24 10:00:00"  // 上架時間
);

// 存取資訊
std::cout << "ID: " << cargo->getId() << std::endl;
std::cout << "月儲費: " << cargo->calculateStorageFee() << std::endl;

// 下架
cargo->setStatus("Unlisted");

// 檢查狀態
if (!cargo->isListed()) {
    std::cout << "此貨物已下架。" << std::endl;
}
```

---

## 衍生類別

### DangerousCargo - 危險品

#### 類別定義
```cpp
class DangerousCargo : public Cargo {
private:
    int hazardLevel;        // 危險等級 (1-9)
    std::string unNumber;   // UN 聯合國物資編號

public:
    DangerousCargo(const std::string& id, const std::string& name, 
                   double weight, double volume, double baseRate, 
                   const std::string& owner, 
                   const std::string& category, 
                   const std::string& listingTime,
                   const std::string& status,
                   int hazardLevel, const std::string& unNumber);

    int getHazardLevel() const;
    std::string getUnNumber() const;
    std::string getType() const override { return "Dangerous"; }
    double calculateStorageFee() const override;
    void printDetails() const override;
};
```

#### 計費邏輯
```cpp
double DangerousCargo::calculateStorageFee() const {
    double baseFee = Cargo::calculateStorageFee();
    // 加收安全保險費與監控費
    double safetyFee = baseFee * 0.2;      // 安全費 20%
    double monitoringFee = 100;             // 監控費 100 元/月
    return baseFee + safetyFee + monitoringFee;
}
```

- **說明**：危險品需額外支付安全保險與監控費用。

#### 輸出範例
```
[危險警告] ID: C002 | 名稱: 鋰電池組 | 危險等級: 5 | UN: UN3480
費用: (基本 + 20% 安全費 + 監控費)
```

---

### PerishableCargo - 易腐品

#### 類別定義
```cpp
class PerishableCargo : public Cargo {
private:
    std::string expiryDate;      // 到期日 (YYYY-MM-DD)
    double requiredTemp;         // 溫控需求 (°C)

public:
    PerishableCargo(const std::string& id, const std::string& name, 
                    double weight, double volume, double baseRate, 
                    const std::string& owner, 
                    const std::string& category, 
                    const std::string& listingTime,
                    const std::string& status,
                    const std::string& expiryDate, double requiredTemp);

    std::string getExpiryDate() const;
    double getRequiredTemp() const;
    std::string getType() const override { return "Perishable"; }
    double calculateStorageFee() const override;
    void printDetails() const override;
};
```

#### 計費邏輯
```cpp
double PerishableCargo::calculateStorageFee() const {
    double baseFee = Cargo::calculateStorageFee();
    
    // 根據溫控需求加收冷鏈費
    double coldChainFee = 0;
    if (requiredTemp <= 0) {
        coldChainFee = baseFee * 0.3;   // 冷凍 30%
    } else if (requiredTemp <= 10) {
        coldChainFee = baseFee * 0.15;  // 冷藏 15%
    }
    
    return baseFee + coldChainFee;
}
```

- **說明**：易腐品依據溫控需求動態加收冷鏈費用。

#### 輸出範例
```
ID: C003 | 名稱: 新鮮水蜜桃 | 到期日: 2026-07-01 | 溫控: 4°C
費用: (基本 + 冷藏費 15%)
```

---

### FragileCargo - 易碎品

#### 類別定義
```cpp
class FragileCargo : public Cargo {
private:
    std::string packagingType;   // 防護包裝 (如：木箱、氣泡紙)
    int maxStackHeight;          // 最大堆疊高度層數

public:
    FragileCargo(const std::string& id, const std::string& name, 
                 double weight, double volume, double baseRate, 
                 const std::string& owner, 
                 const std::string& category, 
                 const std::string& listingTime,
                 const std::string& status,
                 const std::string& packagingType, int maxStackHeight);

    std::string getPackagingType() const;
    int getMaxStackHeight() const;
    std::string getType() const override { return "Fragile"; }
    double calculateStorageFee() const override;
    void printDetails() const override;
};
```

#### 計費邏輯
```cpp
double FragileCargo::calculateStorageFee() const {
    double baseFee = Cargo::calculateStorageFee();
    
    // 若堆疊限制嚴格，加收地板佔用費
    double floorFee = 0;
    if (maxStackHeight <= 2) {
        floorFee = 200;  // 低堆疊限制加收地板費 200 元/月
    }
    
    return baseFee + floorFee;
}
```

- **說明**：易碎品若堆疊限制低，需加收地板佔用費。

#### 輸出範例
```
ID: C004 | 名稱: 高級水晶燈 | 包裝: 木箱 | 堆疊限制: 1 層
費用: (基本 + 地板費 200)
```

---

## User - 使用者權限管制

### 基底類別設計
```cpp
class User {
protected:
    std::string username;  // 帳號
    std::string password;  // 密碼
    std::string role;      // 角色：Admin 或 Regular

public:
    User(const std::string& username, const std::string& password, 
         const std::string& role);
    
    virtual ~User() = default;

    std::string getUsername() const { return username; }
    std::string getRole() const { return role; }
    bool verifyPassword(const std::string& pwd) const { return password == pwd; }
    std::string getPassword() const { return password; }

    // 多型權限檢查
    virtual bool canManageInventory() const = 0;
    virtual bool canManageUsers() const = 0;
};
```

### 衍生類別

#### AdminUser - 系統管理員
```cpp
class AdminUser : public User {
public:
    AdminUser(const std::string& username, const std::string& password);
    
    bool canManageInventory() const override { return true; }  // 可管理所有貨物
    bool canManageUsers() const override { return true; }      // 可新增使用者
};
```

**權限**：
- ✅ 新增 / 下架 / 重新上架任何貨物。
- ✅ 查看所有使用者的貨物。
- ✅ 新增系統使用者。
- ✅ 查看倉庫統計與報表。

#### RegularUser - 一般作業員
```cpp
class RegularUser : public User {
public:
    RegularUser(const std::string& username, const std::string& password);
    
    bool canManageInventory() const override { return false; }  // 受限管理
    bool canManageUsers() const override { return false; }      // 不可新增使用者
};
```

**權限**：
- ✅ 新增 / 下架 / 重新上架自己擁有的貨物。
- ❌ 操作他人貨物。
- ❌ 新增使用者。

### 使用示例
```cpp
// 建立管理員與作業員
auto admin = std::make_shared<AdminUser>("admin", "admin123");
auto operator_user = std::make_shared<RegularUser>("user", "user123");

// 權限檢查
if (admin->canManageUsers()) {
    std::cout << "管理員可新增使用者。" << std::endl;
}

if (!operator_user->canManageUsers()) {
    std::cout << "作業員無法新增使用者。" << std::endl;
}
```

---

## Warehouse - 倉庫核心管理

### 設計目的
統一管理庫存、使用者、權限與業務邏輯，提供高層 API 供 TUI 與 main 使用。

### 類別定義
```cpp
class Warehouse {
private:
    std::vector<std::shared_ptr<Cargo>> inventory;
    std::vector<std::shared_ptr<User>> users;
    std::shared_ptr<User> currentUser;

public:
    Warehouse();

    // 帳密登入與管制
    bool login(const std::string& username, const std::string& password);
    void logout();
    std::shared_ptr<User> getCurrentUser() const;
    bool isLoggedIn() const;

    // 貨物管理 API
    bool addCargo(std::shared_ptr<Cargo> cargo);
    bool removeCargo(const std::string& id);      // 下架
    bool relistCargo(const std::string& id);      // 重新上架
    std::shared_ptr<Cargo> findCargo(const std::string& id) const;
    const std::vector<std::shared_ptr<Cargo>>& getInventory() const;
    void clearInventory();

    // 使用者管理 API
    bool addUser(std::shared_ptr<User> user);
    std::shared_ptr<User> findUser(const std::string& username) const;
    const std::vector<std::shared_ptr<User>>& getUsers() const;
    void clearUsers();

    // 統計計算
    double calculateTotalVolume() const;
    double calculateTotalRevenue() const;
};
```

### 核心功能

#### 1. **login() 與 logout()**
```cpp
bool Warehouse::login(const std::string& username, const std::string& password) {
    auto user = findUser(username);
    if (user && user->verifyPassword(password)) {
        currentUser = user;
        return true;
    }
    return false;
}

void Warehouse::logout() {
    currentUser = nullptr;
}
```

#### 2. **addCargo() - 新增貨物**
```cpp
bool Warehouse::addCargo(std::shared_ptr<Cargo> cargo) {
    if (!cargo) return false;
    // 檢查 ID 是否重複
    if (findCargo(cargo->getId()) != nullptr) {
        return false;
    }
    inventory.push_back(cargo);
    return true;
}
```

- **說明**：檢查 ID 唯一性後加入庫存。

#### 3. **removeCargo() - 下架（非刪除）**
```cpp
bool Warehouse::removeCargo(const std::string& id) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&id](const std::shared_ptr<Cargo>& c) { return c->getId() == id; });
    
    if (it != inventory.end()) {
        auto cargo = *it;
        if (!cargo->isListed()) return false;  // 已下架則失敗
        cargo->setStatus("Unlisted");          // 下架（保留記錄）
        return true;
    }
    return false;
}
```

- **說明**：設定狀態為 Unlisted，保留完整記錄以供日後復原。

#### 4. **relistCargo() - 重新上架**
```cpp
bool Warehouse::relistCargo(const std::string& id) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&id](const std::shared_ptr<Cargo>& c) { return c->getId() == id; });
    
    if (it != inventory.end()) {
        auto cargo = *it;
        if (cargo->isListed()) return false;   // 已上架則失敗
        cargo->setStatus("Active");            // 恢復為上架
        return true;
    }
    return false;
}
```

#### 5. **calculateTotalRevenue() - 統計總費用（展現多型）**
```cpp
double Warehouse::calculateTotalRevenue() const {
    double total = 0.0;
    for (const auto& cargo : inventory) {
        // 多型呼叫各衍生類別的 calculateStorageFee()
        total += cargo->calculateStorageFee();
    }
    return total;
}
```

- **說明**：迭代所有貨物，透過虛擬函式自動選擇正確的計費方法。

### 使用示例
```cpp
Warehouse warehouse;

// 新增貨物
auto cargo1 = std::make_shared<Cargo>("C001", "箱子", ...);
warehouse.addCargo(cargo1);

// 新增使用者
auto user = std::make_shared<AdminUser>("admin", "pass");
warehouse.addUser(user);

// 登入
if (warehouse.login("admin", "pass")) {
    std::cout << "登入成功！" << std::endl;
}

// 查詢貨物
auto found = warehouse.findCargo("C001");

// 下架
warehouse.removeCargo("C001");

// 計算總費用
double revenue = warehouse.calculateTotalRevenue();

// 登出
warehouse.logout();
```

---

## Database - 檔案 I/O 與序列化

### 設計目的
負責 CSV 格式的讀寫、物件反序列化與持久化。

### 類別定義
```cpp
class Database {
private:
    std::string usersFilePath;
    std::string cargoFilePath;

    static std::vector<std::string> split(const std::string& s, char delimiter);

public:
    Database(const std::string& usersPath = "data/users.txt", 
             const std::string& cargoPath = "data/cargo.txt");

    bool loadUsers(Warehouse& warehouse);
    bool saveUsers(const Warehouse& warehouse);
    bool loadCargo(Warehouse& warehouse);
    bool saveCargo(const Warehouse& warehouse);
};
```

### CSV 格式說明

#### users.txt
```
Role,Username,Password
Admin,admin,admin123
Regular,user,user123
```

#### cargo.txt
```
Type,ID,Name,Weight,Volume,BaseRate,Owner,Category,ListingTime,Status,TypeSpecific1,TypeSpecific2
General,C001,普通箱子,100,2,50,admin,未分類,2026-06-24 10:00:00,Active
Dangerous,C002,鋰電池,50,0.5,120,admin,電子,2026-06-24 11:00:00,Active,5,UN3480
Perishable,C003,水蜜桃,200,1.2,80,user,水果,2026-06-24 12:00:00,Active,2026-07-01,4
Fragile,C004,水晶燈,10,0.8,100,user,家飾,2026-06-24 13:00:00,Unlisted,木箱,1
```

### 核心方法

#### loadUsers()
```cpp
bool Database::loadUsers(Warehouse& warehouse) {
    warehouse.clearUsers();
    std::ifstream file(usersFilePath);

    if (!file.is_open()) {
        // 若檔案不存在，寫入預設帳密
        std::ofstream outFile(usersFilePath);
        if (outFile.is_open()) {
            outFile << "Admin,admin,admin123" << std::endl;
            outFile << "Regular,user,user123" << std::endl;
            outFile.close();
        }
        file.open(usersFilePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ',');
        if (tokens.size() < 3) continue;

        if (tokens[0] == "Admin") {
            warehouse.addUser(std::make_shared<AdminUser>(tokens[1], tokens[2]));
        } else {
            warehouse.addUser(std::make_shared<RegularUser>(tokens[1], tokens[2]));
        }
    }
    file.close();
    return true;
}
```

#### loadCargo()
```cpp
bool Database::loadCargo(Warehouse& warehouse) {
    warehouse.clearInventory();
    std::ifstream file(cargoFilePath);
    if (!file.is_open()) {
        return true;  // 檔案不存在代表空倉庫
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ',');
        if (tokens.size() < 7) continue;

        std::string type = tokens[0];
        std::string id = tokens[1];
        // ...其他欄位...
        std::string category = tokens.size() > 7 ? tokens[7] : "";
        std::string listingTime = tokens.size() > 8 ? tokens[8] : "";
        std::string status = tokens.size() > 9 ? tokens[9] : "Active";

        std::shared_ptr<Cargo> cargo = nullptr;
        if (type == "General") {
            cargo = std::make_shared<Cargo>(..., category, listingTime, status);
        } else if (type == "Dangerous") {
            cargo = std::make_shared<DangerousCargo>(..., category, listingTime, status, ...);
        }
        // ...其他型別...

        if (cargo) warehouse.addCargo(cargo);
    }
    file.close();
    return true;
}
```

#### saveCargo()
```cpp
bool Database::saveCargo(const Warehouse& warehouse) {
    std::ofstream file(cargoFilePath);
    if (!file.is_open()) return false;

    for (const auto& cargo : warehouse.getInventory()) {
        file << cargo->getType() << ","
             << cargo->getId() << ","
             << cargo->getName() << ","
             << cargo->getWeight() << ","
             << cargo->getVolume() << ","
             << cargo->getBaseRate() << ","
             << cargo->getOwner() << ","
             << cargo->getCategory() << ","
             << cargo->getListingTime() << ","
             << cargo->getStatus();

        // 依類型附加特定欄位
        if (cargo->getType() == "Dangerous") {
            auto dc = std::dynamic_pointer_cast<DangerousCargo>(cargo);
            if (dc) {
                file << "," << dc->getHazardLevel() << "," << dc->getUnNumber();
            }
        }
        // ...其他型別...
        file << std::endl;
    }
    file.close();
    return true;
}
```

### 向後相容性
- 若舊檔案缺少新欄位（如 `Category`、`ListingTime`、`Status`），系統自動填入預設值。
- 新欄位追加至行尾，不影響既有欄位解析。

---

## TUI - 終端機使用者介面

### 設計目的
提供互動式的選單、輸入防呆、彩色提示與完整使用者流程。

### 類別定義
```cpp
class TUI {
private:
    Warehouse& warehouse;
    Database& database;

    // 彩色定義
    static const std::string COLOR_RESET, COLOR_BOLD, COLOR_RED, 
                             COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_CYAN;

    // UI 控制
    void clearScreen() const;
    void printHeader(const std::string& title, bool center = false) const;
    void printBanner() const;
    int selectMenu(const std::string& headerTitle, const std::vector<std::string>& options) const;

    // 輸入方法
    std::string readString(const std::string& prompt) const;
    std::string readPassword(const std::string& prompt) const;  // 遮蔽輸入
    int readInt(const std::string& prompt) const;
    double readDouble(const std::string& prompt) const;
    std::string getCurrentTimestamp() const;

    // 流程
    void handleLogin(bool requireAdmin);
    void registerAccountFlow();
    void showAdminMenu();
    void showRegularMenu();
    void addCargoFlow();
    void removeCargoFlow();
    void viewUnlistedCargoFlow(bool showAll);
    void relistCargoFlow();
    void listInventory(bool showAll) const;
    void showStatistics() const;
    void addUserFlow();

public:
    TUI(Warehouse& wh, Database& db);
    void run();
};
```

### 主要流程

#### 1. **run() - 主循環**
```cpp
void TUI::run() {
    database.loadUsers(warehouse);
    database.loadCargo(warehouse);

    std::vector<std::string> options = {
        "一般作業員登入 (Operator Login)",
        "系統管理員登入 (Admin Login)",
        "註冊新帳號 (Register New Account)",
        "結束系統 (Exit System)"
    };

    while (true) {
        int choice = selectMenu("主選單 (Main Menu)", options);
        if (choice == 0) {
            handleLogin(false);
        } else if (choice == 1) {
            handleLogin(true);
        } else if (choice == 2) {
            registerAccountFlow();
        } else if (choice == 3) {
            std::cout << "感謝使用！" << std::endl;
            break;
        }
    }
}
```

#### 2. **selectMenu() - 選單選擇**
- 使用方向鍵或 WASD 導航。
- 按 Enter 確認。
- 視覺反白選中項目。

#### 3. **readPassword() - 遮蔽密碼輸入**
```cpp
std::string TUI::readPassword(const std::string& prompt) const {
    std::cout << prompt;
    std::string password;
#ifdef _WIN32
    while (true) {
        int ch = _getch();
        if (ch == 13) {           // Enter
            break;
        } else if (ch == 8) {     // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(static_cast<char>(ch));
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    // Linux/Mac: 使用 termios
    // ...
#endif
    return password;
}
```

#### 4. **addCargoFlow() - 新增貨物**
1. 輸入貨物 ID、名稱、重量、容積、費率。
2. **選單式分類**：提供常見分類或自訂。
3. 自動記錄 `listingTime`。
4. 選擇擁有人（Admin 可指派他人，Regular 自動設為自己）。
5. 選擇貨物型別（一般/危險/易腐/易碎）。
6. 填寫型別特定欄位。
7. 寫入資料庫並記錄日誌。

#### 5. **removeCargoFlow() - 下架貨物**
1. 列出可下架的上架貨物（Regular 使用者僅見自己的）。
2. 使用選單選擇。
3. 顯示貨物詳情並要求確認。
4. 更新狀態為 Unlisted 並寫回資料庫。

#### 6. **showStatistics() - 倉庫報表**
- 總貨物數、總容積、總費用。
- 貨物種類分佈圖（視覺柱狀圖）。
- 安全警示（危險品等級 ≥ 7 顯示警告）。

### 彩色提示
```cpp
const std::string TUI::COLOR_RESET   = "\033[0m";
const std::string TUI::COLOR_RED     = "\033[31m";      // 錯誤/警告
const std::string TUI::COLOR_GREEN   = "\033[32m";     // 成功
const std::string TUI::COLOR_YELLOW  = "\033[33m";     // 提示
const std::string TUI::COLOR_BLUE    = "\033[34m";     // 資訊
const std::string TUI::COLOR_CYAN    = "\033[36m";     // 標題
```

---

## Logger - 系統日誌

### 設計目的
記錄所有重要系統事件供稽核與除錯。

### 類別定義
```cpp
class Logger {
private:
    std::string logFilePath;
    std::ofstream logFile;

    static std::string getCurrentTimestamp();

public:
    explicit Logger(const std::string& filePath = "data/system.log");
    ~Logger();

    bool log(const std::string& event);
};
```

### 使用示例
```cpp
Logger logger("data/system.log");

// 記錄登入事件
logger.log("User 'admin' logged in successfully");

// 記錄新增貨物
logger.log("Cargo C001 added by admin");

// 記錄下架
logger.log("Cargo C001 unlisted by admin");

// 記錄錯誤
logger.log("ERROR: Failed to add duplicate cargo C001");
```

### 日誌檔案格式
```
[2026-06-24 10:00:00] System started
[2026-06-24 10:01:23] User 'admin' logged in successfully
[2026-06-24 10:02:45] Cargo C001 (一般貨物) added by admin
[2026-06-24 10:05:30] Cargo C001 unlisted by admin
[2026-06-24 10:06:15] Cargo C001 relisted by admin
[2026-06-24 10:10:00] User 'user' logged in successfully
[2026-06-24 10:15:20] ERROR: Login failed for user 'unknown'
[2026-06-24 10:20:00] System shutting down
```

### 日誌記錄點
- **系統事件**：啟動、關閉。
- **使用者操作**：登入、登出、註冊、新增使用者。
- **貨物操作**：新增、下架、重新上架。
- **錯誤事件**：登入失敗、ID 重複、型別錯誤等。

---

## 模組間互動流程

```
main.cpp
  ├─ Database.loadUsers(warehouse) ─→ 從 users.txt 建立 User 物件加入 Warehouse
  ├─ Database.loadCargo(warehouse) ─→ 從 cargo.txt 建立 Cargo 及衍生物件加入 Warehouse
  ├─ Logger.log("System started")
  └─ TUI.run()
       ├─ selectMenu() ─→ 使用者選擇登入/註冊
       ├─ handleLogin()
       │   └─ Warehouse.login() ─→ User 驗證
       ├─ showAdminMenu() / showRegularMenu()
       ├─ addCargoFlow()
       │   ├─ readString/readInt/readDouble ─→ 輸入資訊
       │   ├─ selectMenu() ─→ 分類選擇
       │   ├─ Warehouse.addCargo() ─→ 新增至 inventory
       │   ├─ Database.saveCargo() ─→ 寫入 CSV
       │   └─ Logger.log() ─→ 記錄事件
       ├─ removeCargoFlow()
       │   ├─ selectMenu() ─→ 選擇貨物
       │   ├─ Warehouse.removeCargo() ─→ 設定狀態為 Unlisted
       │   ├─ Database.saveCargo()
       │   └─ Logger.log()
       └─ ...其他操作...
```

---

## 總結

| 模組 | 職責 | 關鍵特性 |
|------|------|---------|
| **Cargo** | 貨物基礎模型 | 虛擬方法、多型計費 |
| **衍生類別** | 貨物型別特化 | 覆寫計費、特定欄位 |
| **User** | 使用者權限 | 角色分離、虛擬權限檢查 |
| **Warehouse** | 業務邏輯中樞 | 庫存管理、統計計算、權限檢查 |
| **Database** | 持久化層 | CSV I/O、物件反序列化、向後相容 |
| **TUI** | 使用者介面 | 選單導航、遮蔽輸入、彩色提示 |
| **Logger** | 系統稽核 | 事件記錄、追蹤使用者操作 |

各模組透過 **shared_ptr**、**虛擬方法** 與 **STL 容器** 緊密協作，展現 C++17 OOP 最佳實踐。

