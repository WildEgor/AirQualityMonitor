#pragma once
#include <AutoOTA.h>
#include <Looper.h>

#include "configs/config.h"

#define LOG_COMPONENT "OTA"
#include "services/logger.h"

/**
 * @name OTA
 * @details Implement logic to fetch firmware from remote source
 */
class OTA : public LoopTickerBase
{
public:
    OTA();

    /**
     * @brief Execute OTA checks
     */
    void exec() override;
    /**
     * @details Check if new updates available
     * @return bool
     */
    bool hasUpdate();
    /**
     * @details Init async/sync update
     * @return bool
     */
    bool update(bool async = true);

    /**
     * @details Return current version
     * @return String
     */
    String version();

private:
    AutoOTA _ota = AutoOTA(APP_VERSION, PROJECT_PATH);

    bool _is_initialized = false;
    String _ver = "";
    String _notes = "";
};