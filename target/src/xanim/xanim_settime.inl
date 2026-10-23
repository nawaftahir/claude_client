// XAnimSetTime body; params st_tree, st_index, st_time; names st_state/st_cycle/st_oldcycle per header set
{
	unsigned short childIndex;
	XAnimInfo *info;

	childIndex = (st_tree)->infoArray[st_index];

	if ( !childIndex )
	{
	}
	else
	{
		info = &g_xAnimInfo[childIndex];
		info->st_state.time = (st_time);
		info->st_state.st_cycle = 0;
		info->st_state.oldTime = (st_time);
		info->st_state.st_oldcycle = 0;
		info->notifyIndex = -1;
	}
}
