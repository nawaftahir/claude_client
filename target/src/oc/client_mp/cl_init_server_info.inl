// CL_InitServerInfo body; pp_server, pp_adr are the arguments
    pp_server->adr = pp_adr;
    pp_server->clients = 0;
    pp_server->hostName[0] = '\0';
    pp_server->mapName[0] = '\0';
    pp_server->maxClients = 0;
    pp_server->maxPing = 0;
    pp_server->minPing = 0;
    pp_server->ping = -1;
    pp_server->game[0] = '\0';
    pp_server->gameType[0] = '\0';
    pp_server->netType = 0;
    pp_server->allowAnonymous = 0;
    pp_server->dirty = 1;
    pp_server->requestCount = 0;
