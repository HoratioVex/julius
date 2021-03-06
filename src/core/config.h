#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

typedef enum {
    CONFIG_GP_FIX_IMMIGRATION_BUG,
    CONFIG_GP_FIX_100_YEAR_GHOSTS,
    CONFIG_UI_SIDEBAR_INFO,
    CONFIG_UI_SHOW_INTRO_VIDEO,
    CONFIG_UI_SMOOTH_SCROLLING,
    CONFIG_UI_WALKER_WAYPOINTS,
    CONFIG_GP_CH_GRANDFESTIVAL,
    CONFIG_GP_CH_JEALOUS_GODS,
    CONFIG_GP_CH_GLOBAL_LABOUR,
    CONFIG_GP_CH_SCHOOL_WALKERS,
    CONFIG_GP_CH_RETIRE_AT_60,
    CONFIG_MAX_ENTRIES
} config_key;

int config_get(config_key key);
void config_set(config_key key, int value);
void config_set_defaults(void);

void config_load(void);
void config_save(void);

#endif // CORE_CONFIG_H
