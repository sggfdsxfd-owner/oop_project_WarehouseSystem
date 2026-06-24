#include "TUI.h"
#include "DangerousCargo.h"
#include "PerishableCargo.h"
#include "FragileCargo.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <ctime>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

const std::string TUI::COLOR_RESET   = "\033[0m";
const std::string TUI::COLOR_BOLD    = "\033[1m";
const std::string TUI::COLOR_RED     = "\033[31m";
const std::string TUI::COLOR_GREEN   = "\033[32m";
const std::string TUI::COLOR_YELLOW  = "\033[33m";
const std::string TUI::COLOR_BLUE    = "\033[34m";
const std::string TUI::COLOR_CYAN    = "\033[36m";

TUI::TUI(Warehouse& wh, Database& db) : warehouse(wh), database(db) {}

void TUI::clearScreen() const {
#ifdef _WIN32
    // 在 Windows 下可以呼叫 cls，或是支援 ANSI 的話直接用 ANSI 逸出碼
    // 這裡我們雙重保險
    std::system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void TUI::printHeader(const std::string& title, bool center) const {
    std::cout << COLOR_BOLD << COLOR_CYAN;
    std::cout << "==================================================" << std::endl;
    if (center) {
        int visualLength = 0;
        for (size_t j = 0; j < title.length(); ) {
            unsigned char c = title[j];
            if (c >= 0x80) { // UTF-8 中文字元占 3 bytes
                visualLength += 2;
                j += 3;
            } else {
                visualLength += 1;
                j += 1;
            }
        }
        int padding = (50 - visualLength) / 2;
        if (padding < 0) padding = 0;
        std::cout << std::string(padding, ' ') << title << std::endl;
    } else {
        std::cout << title << std::endl;
    }
    std::cout << "==================================================" << COLOR_RESET << std::endl;
}

void TUI::printBanner() const {
    std::cout << COLOR_BOLD << COLOR_GREEN;
    std::cout << "┌──────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│              智慧物流與倉庫管理系統              │" << std::endl;
    std::cout << "│   Smart Warehouse & Logistics Management System  │" << std::endl;
    std::cout << "└──────────────────────────────────────────────────┘" << std::endl;
    std::cout << COLOR_RESET << std::endl;
}

int TUI::selectMenu(const std::string& headerTitle, const std::vector<std::string>& options) const {
    int selected = 0;
    int numOptions = options.size();

    while (true) {
        clearScreen();
        printBanner();
        printHeader(headerTitle, false); // 靠左對齊

        for (int i = 0; i < numOptions; ++i) {
            if (i == selected) {
                // 選中項目以 ANSI 反白顯示
                std::cout << COLOR_GREEN << " > " << "\033[7m" << options[i] << COLOR_RESET << std::endl;
            } else {
                std::cout << "   " << options[i] << std::endl;
            }
        }
        std::cout << COLOR_CYAN << "==================================================" << COLOR_RESET << std::endl;
        std::cout << "↑/↓ 鍵選擇，Enter 確認" << std::endl;

#ifdef _WIN32
        int key = _getch();
        if (key == 0 || key == 224) {
            key = _getch();
            if (key == 72) { // Up arrow
                selected = (selected - 1 + numOptions) % numOptions;
            } else if (key == 80) { // Down arrow
                selected = (selected + 1) % numOptions;
            }
        } else if (key == 'w' || key == 'W') {
            selected = (selected - 1 + numOptions) % numOptions;
        } else if (key == 's' || key == 'S') {
            selected = (selected + 1) % numOptions;
        } else if (key == 13) { // Enter key
            return selected;
        }
#else
        // 非 Windows 環境備份機制
        int choice;
        std::cout << "請輸入選項索引 (1-" << numOptions << "): ";
        if (std::cin >> choice) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (choice >= 1 && choice <= numOptions) {
                return choice - 1;
            }
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
#endif
    }
}

std::string TUI::readString(const std::string& prompt) const {
    std::cout << prompt;
    std::string val;
    std::getline(std::cin, val);
    return val;
}

std::string TUI::readPassword(const std::string& prompt) const {
    std::cout << prompt;
    std::string password;
#ifdef _WIN32
    while (true) {
        int ch = _getch();
        if (ch == 13) { // Enter
            break;
        } else if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else if (ch == 0 || ch == 224) {
            _getch();
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(static_cast<char>(ch));
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << std::endl;
#endif
    return password;
}

std::string TUI::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

int TUI::readInt(const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        int val;
        if (std::cin >> val) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << COLOR_RED << "輸入錯誤！請輸入整數。" << COLOR_RESET << std::endl;
        }
    }
}

double TUI::readDouble(const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        double val;
        if (std::cin >> val) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << COLOR_RED << "輸入錯誤！請輸入數值。" << COLOR_RESET << std::endl;
        }
    }
}

void TUI::run() {
    // 啟動載入
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
            std::cout << COLOR_GREEN << "\n感謝使用本系統，資料已自動存檔，再見！" << COLOR_RESET << std::endl;
            break;
        }
    }
}

void TUI::handleLogin(bool requireAdmin) {
    clearScreen();
    printHeader(requireAdmin ? "系統管理員登入 (Admin Login)" : "一般作業員登入 (Operator Login)");
    
    std::string username = readString("請輸入帳號: ");
    std::string password = readPassword("請輸入密碼: ");

    if (warehouse.login(username, password)) {
        auto user = warehouse.getCurrentUser();
        
        // 角色管制檢查
        if (requireAdmin && user->getRole() != "Admin") {
            std::cout << COLOR_RED << "\n登入失敗！此管道僅限系統管理員登入。" << COLOR_RESET << std::endl;
            warehouse.logout();
            std::cout << "按任意鍵返回主選單..." << std::endl;
            std::cin.get();
            return;
        }
        if (!requireAdmin && user->getRole() == "Admin") {
            std::cout << COLOR_YELLOW << "\n提示：偵測到您使用管理員帳號登入。\n系統將自動為您引導至管理員控制台。" << COLOR_RESET << std::endl;
        }

        std::cout << COLOR_GREEN << "\n登入成功！歡迎 " << user->getUsername() 
                  << " (" << (user->getRole() == "Admin" ? "管理員" : "一般作業員") << ")" 
                  << COLOR_RESET << std::endl;
        std::cout << "按任意鍵進入系統..." << std::endl;
        std::cin.get();

        if (user->getRole() == "Admin") {
            showAdminMenu();
        } else {
            showRegularMenu();
        }
    } else {
        std::cout << COLOR_RED << "\n登入失敗！帳號或密碼錯誤。" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回主選單..." << std::endl;
        std::cin.get();
    }
}

void TUI::registerAccountFlow() {
    clearScreen();
    printHeader("註冊新帳號 (Register New Account)");

    std::string username = readString("請輸入新帳號: ");
    if (warehouse.findUser(username) != nullptr) {
        std::cout << COLOR_RED << "錯誤：該帳號已存在！" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回主選單..." << std::endl;
        std::cin.get();
        return;
    }

    std::string password = readPassword("請輸入新密碼: ");

    std::vector<std::string> roleOptions = {
        "一般作業員 (Regular Operator)",
        "系統管理員 (Admin Manager)"
    };

    int roleChoice = selectMenu("請選擇使用者角色 (Select User Role)", roleOptions);
    std::shared_ptr<User> newUser;
    if (roleChoice == 1) {
        newUser = std::make_shared<AdminUser>(username, password);
    } else {
        newUser = std::make_shared<RegularUser>(username, password);
    }

    if (warehouse.addUser(newUser)) {
        database.saveUsers(warehouse);
        std::cout << COLOR_GREEN << "\n註冊成功！帳號已儲存。" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "\n註冊失敗！請稍後再試。" << COLOR_RESET << std::endl;
    }

    std::cout << "按任意鍵返回主選單..." << std::endl;
    std::cin.get();
}

void TUI::showAdminMenu() {
    std::vector<std::string> options = {
        "查看庫存貨物 (View Active Cargo)",
        "查看下架貨物 (View Unlisted Cargo)",
        "新增貨物入庫 (Add Cargo)",
        "下架貨物 (Unlist Cargo)",
        "重新上架貨物 (Relist Cargo)",
        "倉庫營運統計與報表 (Operations Report)",
        "新增系統使用者帳號 (Add User)",
        "安全登出 (Logout)"
    };

    while (true) {
        int choice = selectMenu("管理員控制台 (Admin Dashboard) - " + warehouse.getCurrentUser()->getUsername(), options);
        if (choice == 0) {
            listInventory(true);
        } else if (choice == 1) {
            viewUnlistedCargoFlow(true);
        } else if (choice == 2) {
            addCargoFlow();
        } else if (choice == 3) {
            removeCargoFlow();
        } else if (choice == 4) {
            relistCargoFlow();
        } else if (choice == 5) {
            showStatistics();
        } else if (choice == 6) {
            addUserFlow();
        } else if (choice == 7) {
            warehouse.logout();
            std::cout << COLOR_GREEN << "\n已成功登出。" << COLOR_RESET << std::endl;
            std::cout << "按任意鍵返回登入頁面..." << std::endl;
            std::cin.get();
            break;
        }
    }
}

void TUI::showRegularMenu() {
    std::vector<std::string> options = {
        "查看我管理的貨物 (View My Cargo)",
        "查看我的下架貨物 (View My Unlisted Cargo)",
        "申報新貨物入庫 (Declare New Cargo)",
        "下架貨物 (Unlist Cargo)",
        "重新上架貨物 (Relist Cargo)",
        "安全登出 (Logout)"
    };

    while (true) {
        int choice = selectMenu("作業員控制台 (Operator Panel) - " + warehouse.getCurrentUser()->getUsername(), options);
        if (choice == 0) {
            listInventory(false);
        } else if (choice == 1) {
            viewUnlistedCargoFlow(false);
        } else if (choice == 2) {
            addCargoFlow();
        } else if (choice == 3) {
            removeCargoFlow();
        } else if (choice == 4) {
            relistCargoFlow();
        } else if (choice == 5) {
            warehouse.logout();
            std::cout << COLOR_GREEN << "\n已成功登出。" << COLOR_RESET << std::endl;
            std::cout << "按任意鍵返回登入頁面..." << std::endl;
            std::cin.get();
            break;
        }
    }
}

void TUI::listInventory(bool showAll) const {
    clearScreen();
    printHeader(showAll ? "所有庫存貨物清單" : "您擁有的庫存貨物清單");

    auto inventory = warehouse.getInventory();
    auto curUser = warehouse.getCurrentUser();

    // 進行過濾
    std::vector<std::shared_ptr<Cargo>> displayList;
    for (const auto& c : inventory) {
        if (c->isListed() && (showAll || c->getOwner() == curUser->getUsername())) {
            displayList.push_back(c);
        }
    }

    if (displayList.empty()) {
        std::cout << COLOR_YELLOW << "目前沒有任何庫存貨物。" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_BOLD;
        std::cout << std::left << std::setw(8) << "ID"
                  << std::setw(15) << "名稱"
                  << std::setw(10) << "類別"
                  << std::right << std::setw(13) << "重量"
                  << std::setw(13) << "容積"
                  << std::setw(15) << "預估費用" << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << COLOR_RESET << std::endl;
        
        for (const auto& cargo : displayList) {
            cargo->printDetails();
        }
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
        std::cout << "總計貨物數: " << displayList.size() << " 件" << std::endl;
    }
    std::cout << "\n按任意鍵返回選單..." << std::endl;
    std::cin.get();
}

void TUI::addCargoFlow() {
    clearScreen();
    printHeader("申報新貨物入庫");

    std::string id = readString("請輸入貨物 ID (例如 C001): ");
    if (warehouse.findCargo(id) != nullptr) {
        std::cout << COLOR_RED << "錯誤：該貨物 ID 已存在於倉庫中！" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回..." << std::endl;
        std::cin.get();
        return;
    }

    std::string name = readString("請輸入貨物名稱: ");
    double weight = readDouble("請輸入貨物重量 (kg): ");
    double volume = readDouble("請輸入貨物容積 (m³): ");
    double baseRate = readDouble("請輸入基本費率 (每月每 m³ 元): ");

    // 分類採選單式選擇，提供常見選項並允許自訂
    std::vector<std::string> categoryOptions = {"未分類", "電子", "食品", "水果", "家飾", "服飾", "其他"};
    int catChoice = selectMenu("請選擇貨物分類 (Select Category)", categoryOptions);
    std::string category;
    if (catChoice == static_cast<int>(categoryOptions.size()) - 1) {
        category = readString("請輸入自訂分類: ");
    } else {
        category = categoryOptions[catChoice];
    }

    std::string listingTime = getCurrentTimestamp();

    std::string owner;
    if (warehouse.getCurrentUser()->getRole() == "Admin") {
        owner = readString("請指派貨物擁有人 (使用者帳號): ");
        if (warehouse.findUser(owner) == nullptr) {
            std::cout << COLOR_YELLOW << "警告：該使用者尚未註冊，但仍指派為擁有者。" << COLOR_RESET << std::endl;
        }
    } else {
        owner = warehouse.getCurrentUser()->getUsername();
    }

    std::vector<std::string> typeOptions = {
        "普通貨物 (General)",
        "危險品 (Dangerous)",
        "易腐品 (Perishable)",
        "易碎品 (Fragile)"
    };

    int typeChoice = selectMenu("請選擇貨物屬性類型 (Select Cargo Type)", typeOptions);

    std::shared_ptr<Cargo> newCargo = nullptr;

    if (typeChoice == 1) { // 危險品
        int hazardLevel = readInt("請輸入危險等級 (1-9): ");
        std::string unNumber = readString("請輸入 UN 聯合國物資編號 (例如 UN3480): ");
        newCargo = std::make_shared<DangerousCargo>(id, name, weight, volume, baseRate, owner, category, listingTime, "Active", hazardLevel, unNumber);
    } else if (typeChoice == 2) { // 易腐品
        std::string expiryDate = readString("請輸入到期日 (格式 YYYY-MM-DD): ");
        double requiredTemp = readDouble("請輸入溫控需求 (°C): ");
        newCargo = std::make_shared<PerishableCargo>(id, name, weight, volume, baseRate, owner, category, listingTime, "Active", expiryDate, requiredTemp);
    } else if (typeChoice == 3) { // 易碎品
        std::string packagingType = readString("請輸入防護包裝方式 (如 木箱 / 氣泡紙): ");
        int maxStackHeight = readInt("請輸入最大堆疊高度層數: ");
        newCargo = std::make_shared<FragileCargo>(id, name, weight, volume, baseRate, owner, category, listingTime, "Active", packagingType, maxStackHeight);
    } else { // 普通貨物
        newCargo = std::make_shared<Cargo>(id, name, weight, volume, baseRate, owner, category, listingTime, "Active");
    }

    if (warehouse.addCargo(newCargo)) {
        database.saveCargo(warehouse);
        std::cout << COLOR_GREEN << "\n貨物入庫申報成功！資料已寫入資料庫。" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "\n入庫申報失敗！" << COLOR_RESET << std::endl;
    }

    std::cout << "按任意鍵返回..." << std::endl;
    std::cin.get();
}

void TUI::removeCargoFlow() {
    clearScreen();
    printHeader("辦理貨物出庫");

    // 以選單方式列出可下架（Active）貨物，使用者可直接選取
    auto inventory = warehouse.getInventory();
    auto curUser = warehouse.getCurrentUser();
    std::vector<std::shared_ptr<Cargo>> selectable;
    std::vector<std::string> options;
    for (const auto& c : inventory) {
        if (!c->isListed()) continue; // 只列出上架貨物
        if (curUser->getRole() != "Admin" && c->getOwner() != curUser->getUsername()) continue;
        selectable.push_back(c);
        std::ostringstream oss;
        oss << c->getId() << " - " << c->getName() << " (" << (c->getCategory().empty() ? "未分類" : c->getCategory()) << ")";
        options.push_back(oss.str());
    }

    if (selectable.empty()) {
        std::cout << COLOR_YELLOW << "目前沒有可供下架的貨物。" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回..." << std::endl;
        std::cin.get();
        return;
    }

    int pick = selectMenu("請選擇要下架的貨物", options);
    auto cargo = selectable[pick];
    std::string id = cargo->getId();

    std::cout << COLOR_YELLOW << "\n確定要出庫以下貨物嗎？" << COLOR_RESET << std::endl;
    cargo->printDetails();
    std::string confirm = readString("\n請輸入 Y 確認出庫，輸入其他鍵取消: ");

    if (confirm == "Y" || confirm == "y") {
        if (warehouse.removeCargo(id)) {
            database.saveCargo(warehouse);
            std::cout << COLOR_GREEN << "貨物已下架，資料庫已更新。" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "下架失敗！可能該貨物已經是下架狀態。" << COLOR_RESET << std::endl;
        }
    } else {
        std::cout << "取消出庫。" << std::endl;
    }

    std::cout << "按任意鍵返回..." << std::endl;
    std::cin.get();
}

void TUI::viewUnlistedCargoFlow(bool showAll) {
    clearScreen();
    printHeader(showAll ? "所有下架貨物清單" : "您的下架貨物清單");

    auto inventory = warehouse.getInventory();
    auto curUser = warehouse.getCurrentUser();
    std::vector<std::shared_ptr<Cargo>> displayList;
    for (const auto& c : inventory) {
        if (!c->isListed() && (showAll || c->getOwner() == curUser->getUsername())) {
            displayList.push_back(c);
        }
    }

    if (displayList.empty()) {
        std::cout << COLOR_YELLOW << "目前沒有任何下架貨物。" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_BOLD;
        std::cout << std::left << std::setw(8) << "ID"
                  << std::setw(15) << "名稱"
                  << std::setw(10) << "類別"
                  << std::right << std::setw(13) << "重量"
                  << std::setw(13) << "容積"
                  << std::setw(15) << "預估費用" << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << COLOR_RESET << std::endl;
        for (const auto& cargo : displayList) {
            cargo->printDetails();
        }
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
        std::cout << "總計下架貨物數: " << displayList.size() << " 件" << std::endl;
    }
    std::cout << "\n按任意鍵返回選單..." << std::endl;
    std::cin.get();
}

void TUI::relistCargoFlow() {
    clearScreen();
    printHeader("重新上架貨物");

    std::string id = readString("請輸入要重新上架的貨物 ID: ");
    auto cargo = warehouse.findCargo(id);
    if (!cargo) {
        std::cout << COLOR_RED << "找不到該貨物 ID！" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回..." << std::endl;
        std::cin.get();
        return;
    }

    auto curUser = warehouse.getCurrentUser();
    if (curUser->getRole() != "Admin" && cargo->getOwner() != curUser->getUsername()) {
        std::cout << COLOR_RED << "權限不足：您無法重新上架他人擁有的貨物！" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回..." << std::endl;
        std::cin.get();
        return;
    }

    if (cargo->isListed()) {
        std::cout << COLOR_YELLOW << "該貨物已經是上架狀態。" << COLOR_RESET << std::endl;
    } else {
        if (warehouse.relistCargo(id)) {
            database.saveCargo(warehouse);
            std::cout << COLOR_GREEN << "貨物已重新上架。" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "重新上架失敗！" << COLOR_RESET << std::endl;
        }
    }

    std::cout << "按任意鍵返回..." << std::endl;
    std::cin.get();
}

void TUI::showStatistics() const {
    clearScreen();
    printHeader("倉庫營運統計與報表");

    auto inventory = warehouse.getInventory();
    
    int totalCount = inventory.size();
    double totalVolume = warehouse.calculateTotalVolume();
    double totalRevenue = warehouse.calculateTotalRevenue();

    int dangerousCount = 0;
    int perishableCount = 0;
    int fragileCount = 0;
    int generalCount = 0;

    for (const auto& c : inventory) {
        if (c->getType() == "Dangerous") dangerousCount++;
        else if (c->getType() == "Perishable") perishableCount++;
        else if (c->getType() == "Fragile") fragileCount++;
        else generalCount++;
    }

    std::cout << COLOR_BOLD << "📋 倉庫整體容量狀態：" << COLOR_RESET << std::endl;
    std::cout << "  - 庫存總貨物數: " << totalCount << " 件" << std::endl;
    std::cout << "  - 總佔用空間容積: " << totalVolume << " m³" << std::endl;
    std::cout << "  - 本月預估總倉儲費收益: " << COLOR_GREEN << totalRevenue << COLOR_RESET << " 元" << std::endl;
    std::cout << std::endl;

    std::cout << COLOR_BOLD << "📊 貨物種類分佈圖：" << COLOR_RESET << std::endl;
    std::cout << "  - 一般貨物 (General):    " << std::setw(3) << generalCount << " 件 " << std::string(generalCount, '#') << std::endl;
    std::cout << "  - 危險品 (Dangerous):    " << std::setw(3) << dangerousCount << " 件 " << COLOR_RED << std::string(dangerousCount, '#') << COLOR_RESET << std::endl;
    std::cout << "  - 易腐品 (Perishable):   " << std::setw(3) << perishableCount << " 件 " << COLOR_BLUE << std::string(perishableCount, '#') << COLOR_RESET << std::endl;
    std::cout << "  - 易碎品 (Fragile):      " << std::setw(3) << fragileCount << " 件 " << COLOR_YELLOW << std::string(fragileCount, '#') << COLOR_RESET << std::endl;
    std::cout << std::endl;

    // 安全警告通報 (展現多型的警示)
    std::cout << COLOR_BOLD << COLOR_RED << "⚠️ 安全警示通報 (Safety Alerts)：" << COLOR_RESET << std::endl;
    bool hasAlert = false;
    for (const auto& c : inventory) {
        if (c->getType() == "Dangerous") {
            auto dc = std::dynamic_pointer_cast<DangerousCargo>(c);
            if (dc && dc->getHazardLevel() >= 7) {
                std::cout << "  - [" << COLOR_RED << "危險警告" << COLOR_RESET << "] 貨物 " << dc->getId() 
                          << " (" << dc->getName() << ") 的危險等級極高 (等級 " << dc->getHazardLevel() 
                          << ")，UN編號: " << dc->getUnNumber() << "，請指派專人監控！" << std::endl;
                hasAlert = true;
            }
        }
    }
    if (!hasAlert) {
        std::cout << COLOR_GREEN << "  - 無重大安全警示，所有危險品運作狀態正常。" << COLOR_RESET << std::endl;
    }

    std::cout << "\n按任意鍵返回..." << std::endl;
    std::cin.get();
}

void TUI::addUserFlow() {
    clearScreen();
    printHeader("新增系統使用者帳號");

    std::string username = readString("請輸入新使用者帳號: ");
    if (warehouse.findUser(username) != nullptr) {
        std::cout << COLOR_RED << "錯誤：該使用者帳號已存在！" << COLOR_RESET << std::endl;
        std::cout << "按任意鍵返回..." << std::endl;
        std::cin.get();
        return;
    }

    std::string password = readPassword("請輸入新密碼: ");
    
    std::vector<std::string> roleOptions = {
        "一般作業員 (Regular Operator)",
        "系統管理員 (Admin Manager)"
    };

    int roleChoice = selectMenu("請選擇使用者角色 (Select User Role)", roleOptions);

    std::shared_ptr<User> newUser = nullptr;
    if (roleChoice == 1) { // 系統管理員
        newUser = std::make_shared<AdminUser>(username, password);
    } else { // 一般作業員
        newUser = std::make_shared<RegularUser>(username, password);
    }

    if (warehouse.addUser(newUser)) {
        database.saveUsers(warehouse);
        std::cout << COLOR_GREEN << "\n使用者註冊成功！帳號已存檔。" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "\n註冊失敗！" << COLOR_RESET << std::endl;
    }

    std::cout << "按任意鍵返回..." << std::endl;
    std::cin.get();
}
