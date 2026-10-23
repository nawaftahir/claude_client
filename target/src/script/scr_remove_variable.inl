// RemoveVariable body; pp_parentId, pp_name are the params
	VariableValueInternal *entryValue;
	unsigned int index;
	unsigned int id;

	assert((scrVarGlob.variableList[pp_parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	index = FindVariableIndexHash(pp_parentId, pp_name);
	entryValue = &scrVarGlob.variableList[index];

	id = entryValue->hash.id;
	assert(id);

	MakeVariableExternal(entryValue, &scrVarGlob.variableList[pp_parentId]);
	FreeValue(id);
