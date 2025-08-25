#include "GetLocalization.h"
#include "../Core/LocalizationManager.h"
#include <HookCrashersAPI.h>

using HookCrashers::API::Client;
using HookCrashers::API::ReturnHelper;
using HookCrashers::API::SWFArgumentReader;
using HC_SWFArgument = HookCrashers::SWF::Data::SWFArgument;
using HC_SWFReturn = HookCrashers::SWF::Data::SWFReturn;

namespace CustomLocalizations {

    static void GetLocalizationHandler(int paramCount, HC_SWFArgument** swfArgs, HC_SWFReturn* swfReturn) {
        if (paramCount < 1) {
            ReturnHelper::SetInt(swfReturn, -1);
            return;
        }

        std::string logicalId = SWFArgumentReader::GetString(swfArgs[0]);
        if (logicalId.empty()) {
            ReturnHelper::SetInt(swfReturn, -1);
            return;
        }

        int numericId = LocalizationManager::getInstance().getNumericId(logicalId);
        ReturnHelper::SetInt(swfReturn, numericId);
    }

    void RegisterGetLocalizationFunction() {
        Client::RegisterCustomSWF(50200, "GetLocalization", GetLocalizationHandler);
        Client::LogInfo("[CustomLocalizations] 'GetLocalization' SWF function registered.");
    }
}