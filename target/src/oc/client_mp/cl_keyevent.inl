// CL_KeyEvent body; KE_RETURN is return in CL_KeyEvent and a jump past the expansion in Key_ClearStates
    char *kb;
    char cmd[1024];
    qboolean bypassMenu;

    bypassMenu = 0;

    // update auto-repeat status and BUTTON_ANY status
    keys[key].down = down;
    if (down) {
        keys[key].repeats++;
        if (keys[key].repeats == 1)
            anykeydown++;
    } else {
        keys[key].repeats = 0;
        anykeydown--;
    }

    if (key != '`' && key != '~' && !(clients->keyCatchers & 1) && keys[key].binding
        && !stricmp(keys[key].binding, "devgui")) {
        if (down && keys[key].repeats == 1) {
            Cbuf_AddText(keys[key].binding);
            KE_RETURN;
        }
    }

    // only a few keys repeat
    if (down && keys[key].repeats > 1) {
        if (!(clients->keyCatchers & (1 | 0x10))) {
            if (!(clients->keyCatchers & 8))
                KE_RETURN;
            switch (key) {
            case 0x9a:
            case 0x9b:
            case 0xa3:
            case 0xa4:
                break;
            default:
                KE_RETURN;
            }
        }
        if (key == '`' || key == '~' || key == 0x1b)
            KE_RETURN;
    }

    // console key is hardcoded, so the user can never unbind it
    if (key == '`' || key == '~') {
        if (!down)
            KE_RETURN;
        if (!(clients->keyCatchers & 1) && !com_sv_running->current.enabled
            && sv_disableClientConsole->current.enabled)
            KE_RETURN;
        if (keys[0xa0].down) {
            if (!Con_IsActive())
                Con_ToggleConsole_f();
            Con_ToggleConsoleOutput();
        } else {
            Con_ToggleConsole_f();
        }
        KE_RETURN;
    }

    // any key during a demo or cinematic acts like escape
    if (down && (key < 0x80 || key == 0xc8) && (clc.demoplaying || clc.state == CA_CINEMATIC || clc.state == CA_LOGO)
        && !clients->keyCatchers) {
        Dvar_SetString((dvar_t *)nextdemo, "");
        key = 0x1b;
    }

    // escape is always handled special
    if (key == 0x1b && down) {
        if (clients->keyCatchers & 0x10) {
            Message_Key(key);
            KE_RETURN;
        }
        if (!(clients->keyCatchers & 8)) {
            switch (clc.state) {
            case CA_ACTIVE:
                if (!clc.demoplaying && !legacyHacks->cl_serverloadwaiting) {
                    setActiveMenu(2);
                    KE_RETURN;
                }
                break;
            case CA_CINEMATIC:
            case CA_LOGO:
                CL_StopLogoOrCinematic();
                KE_RETURN;
            case CA_CONNECTING:
            case CA_CHALLENGING:
            case CA_CONNECTED:
                CL_Disconnect();
                if (com_sv_running->current.enabled)
                    legacyHacks->sv_killserver = 1;
                KE_RETURN;
            default:
                if (!cls.uiStarted)
                    KE_RETURN;
                break;
            }
            setActiveMenu(1);
            KE_RETURN;
        }
        UI_KeyEvent(key, down);
        KE_RETURN;
    }

    if (clients->cgameInitialized && CG_KeyInterceptEvent(key, down))
        KE_RETURN;

    // key up events only perform actions if the game key binding is
    // a button command (leading + sign)
    if (!down) {
        kb = keys[key].binding;
        if (kb && kb[0] == '+') {
            Com_sprintf(cmd, sizeof(cmd), "-%s %i %i\n", kb + 1, key, time);
            Cbuf_AddText(cmd);
        }
        if ((clients->keyCatchers & 8) && cls.uiStarted)
            UI_KeyEvent(key, down);
        KE_RETURN;
    }

    if (cl_bypassMouseInput && cl_bypassMouseInput->current.enabled) {
        if (key == 0xc8 || key == 0xc9 || key == 0xca || !UI_checkKeyExec(key))
            bypassMenu = 1;
    }

    // distribute the key down event to the apropriate handler
    if (clients->keyCatchers & 1) {
        Console_Key(key);
        KE_RETURN;
    }
    if ((clients->keyCatchers & 8) && !bypassMenu) {
        UI_KeyEvent(key, down);
        KE_RETURN;
    }
    if (clients->keyCatchers & 0x10) {
        Message_Key(key);
        KE_RETURN;
    }
    if (clc.state == CA_DISCONNECTED) {
        Console_Key(key);
        KE_RETURN;
    }

    // send the bound action
    kb = keys[key].binding;
    if (!kb) {
        if (key >= 0xc8)
            Com_Printf("%s is unbound, use controls menu to set.\n", Key_KeynumToString(key, 0));
    } else if (kb[0] == '+') {
        // button commands add keynum and time as parms
        Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", kb, key, time);
        Cbuf_AddText(cmd);
    } else {
        Cbuf_AddText(kb);
        Cbuf_AddText("\n");
    }
