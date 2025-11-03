#include "GetLocalization.h"
#include "../Core/LocalizationManager.h"
#include <HookCrashers.h>

namespace CustomLocalizations {

    static void GetLocalizationHandler(int paramCount, HC_SWFArgument** swfArgs, HC_SWFReturn* swfReturn) {
        HookCrashers::SWF::ArgsReader args(paramCount, swfArgs);
        HookCrashers::SWF::ReturnValue ret(swfReturn);

        if (paramCount < 1) {
            ret.SetFailure();
            return;
        }

        std::string logicalId = args.GetString(0);
        int returnValue = -1;
        if (logicalId.empty()) {
            ret.SetInt(returnValue);
            return;
        }

        returnValue = LocalizationManager::getInstance().getNumericId(logicalId);
        ret.SetInt(returnValue);
    }

    void RegisterGetLocalizationFunction() {
        HookCrashers::RegisterCustomSWF(50200, "GetLocalization", GetLocalizationHandler);
        HookCrashers::LogInfo("[CustomLocalizations] 'GetLocalization' SWF function registered.");
    }
}