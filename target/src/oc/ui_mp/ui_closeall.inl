// UI_CloseAll body: Menus_CloseAll on uiInfo->uiDC
	displayContextDef_t *dc;
	int i;

	dc = &uiInfo->uiDC;
	for (i = 0; i < dc->menuCount; i++)
		Menus_Close(dc, dc->Menus[i]);
