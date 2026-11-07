/* libnx lbl service: <switch/services/lbl.h> cmd 2 / 3 / 5 */
#include <stdio.h>
#include <switch.h>

static float clamp_unit(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    PadState pad;
    padInitializeDefault(&pad);

    Result init_rc = lblInitialize();
    bool   ready   = R_SUCCEEDED(init_rc);
    Result set_rc  = 0;

    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break;

        float  setting = 0.0f, applied = 0.0f;
        Result rc_get = 0, rc_applied = 0;

        if (ready) {
            rc_get = lblGetCurrentBrightnessSetting(&setting);

            if (R_SUCCEEDED(rc_get)) {
                float delta = 0.0f;
                if (kDown & HidNpadButton_Left)  delta -= 0.1f;
                if (kDown & HidNpadButton_Right) delta += 0.1f;
                if (kDown & HidNpadButton_Down)  delta -= 0.01f;
                if (kDown & HidNpadButton_Up)    delta += 0.01f;

                if (delta != 0.0f) {
                    float v = clamp_unit(setting + delta);
                    set_rc = lblSetCurrentBrightnessSetting(v);
                    if (R_SUCCEEDED(set_rc))
                        setting = v;
                }
            }

            rc_applied = lblGetBrightnessSettingAppliedToBacklight(&applied);
        }

        consoleClear();

        if (!ready) {
            printf("lblInitialize 0x%08X\n", (unsigned)init_rc);
        } else {
            printf("%-36s%.6f\n", "CurrentBrightnessSetting", setting);
            printf("%-36s%.6f\n", "BrightnessSettingAppliedToBacklight", applied);

            if (R_FAILED(rc_get))
                printf("lblGetCurrentBrightnessSetting 0x%08X\n", (unsigned)rc_get);
            if (R_FAILED(rc_applied))
                printf("lblGetBrightnessSettingAppliedToBacklight 0x%08X\n", (unsigned)rc_applied);
            if (R_FAILED(set_rc))
                printf("lblSetCurrentBrightnessSetting 0x%08X\n", (unsigned)set_rc);
        }

        consoleUpdate(NULL);
    }

    if (ready)
        lblExit();
    consoleExit(NULL);
    return 0;
}
