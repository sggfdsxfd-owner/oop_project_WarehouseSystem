#include "User.h"

User::User(const std::string& username, const std::string& password, const std::string& role)
    : username(username), password(password), role(role) {}

AdminUser::AdminUser(const std::string& username, const std::string& password)
    : User(username, password, "Admin") {}

RegularUser::RegularUser(const std::string& username, const std::string& password)
    : User(username, password, "Regular") {}
