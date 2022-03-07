#include "include/API/PlayerController.hpp"
#include "include/Utils/constants.hpp"
#include "include/Utils/WebUtils.hpp"
#include "include/main.hpp"

#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"

#include "System/Action_1.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "UnityEngine/Resources.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

using UnityEngine::Resources;
using namespace GlobalNamespace;

Player* PlayerController::currentPlayer = NULL;
Player* PlayerController::platformPlayer = NULL;
string PlayerController::lastErrorDescription = "";
function<void(Player*)> PlayerController::playerChanged = [](Player* player) {

};

string PlayerController::RefreshOnline() {
    string result = "";
    WebUtils::Get(API_URL + "user/id", result);
    if (result.length() > 0) {
        currentPlayer = new Player();
        currentPlayer->id = result;

        WebUtils::GetJSONAsync(API_URL + "player/" + result, [](long status, bool error, rapidjson::Document& result){
            if (status == 200) {
                currentPlayer = new Player(result);
                playerChanged(currentPlayer);
            }
        });
    }
    return result;
}

void PlayerController::RefreshPlatform() {
    IPlatformUserModel* userModel = NULL;
    ::ArrayW<PlatformLeaderboardsModel *> pmarray = Resources::FindObjectsOfTypeAll<PlatformLeaderboardsModel*>();
    for (size_t i = 0; i < pmarray.Length(); i++)
    {
        if (pmarray.get(i)->platformUserModel != NULL) {
            userModel = pmarray.get(i)->platformUserModel;
            break;
        }
    }

    if (userModel == NULL) { return; }

    auto userInfoTask = userModel->GetUserInfo();

    auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
            UserInfo *ui = userInfoTask->get_Result();
            if (ui != nullptr) {
                platformPlayer = new Player();
                platformPlayer->name = (string)ui->userName;
                platformPlayer->id = (string)ui->platformUserId;
            }
        }
    );
    reinterpret_cast<System::Threading::Tasks::Task*>(userInfoTask)->ContinueWith(action);
}

string PlayerController::Refresh() {
    if (platformPlayer == NULL) {
        RefreshPlatform();
    }
    
    return RefreshOnline();
}

string PlayerController::SignUp(string login, string password) {
    string result = "";
    lastErrorDescription = "";

    string error = "";
    long statusCode = WebUtils::Get(API_URL + "signinoculus?action=signup&login=" + login + "&password=" + password, error);
    if (statusCode == 200) {
        result = Refresh();
    } else {
        lastErrorDescription = error;
        getLogger().error("BeatLeader %s", ("signup error" + to_string(statusCode)).c_str());
    }

    return result;
}

string PlayerController::LogIn(string login, string password) {
    string result = "";
    lastErrorDescription = "";

    string error = "";
    long statusCode = WebUtils::Get(API_URL + "signinoculus?action=login&login=" + login + "&password=" + password, error);
    if (statusCode == 200) {
        result = Refresh();
    } else {
        lastErrorDescription = error;
        getLogger().error("BeatLeader %s", ("login error" + to_string(statusCode)).c_str());
    }

    return result;
}

bool PlayerController::LogOut() {
    string result = "";
    WebUtils::Get(API_URL + "signout", result);
    lastErrorDescription = result;
    WebUtils::Get(API_URL + "user/id", result);
    if (result.length() == 0) {
        currentPlayer = NULL;
        playerChanged(currentPlayer);
        return true;
    } else {
        return false;
    }
}