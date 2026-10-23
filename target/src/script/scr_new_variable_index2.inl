// GetNewVariableIndexInternal2 body; i2_* are the params, PP_RETVAL(v) is the result
	VariableValueInternal *entry;
	VariableValueInternal *siblingEntry;
	unsigned short siblingId;
	VariableValueInternal *siblingValue;
	VariableValueInternal *parentValue;

	i2_index = GetNewVariableIndexInternal3(i2_parentId, i2_name, i2_index);
	parentValue = &scrVarGlob.variableList[i2_parentId];
	siblingId = parentValue->nextSibling;
	siblingValue = &scrVarGlob.variableList[siblingId];
	entry = &scrVarGlob.variableList[i2_index];
	siblingEntry = &scrVarGlob.variableList[entry->hash.id];
	siblingEntry->nextSibling = siblingId;
	siblingValue->hash.u.prevSibling = i2_index;
	entry->hash.u.prevSibling = parentValue->v.next;
	parentValue->nextSibling = i2_index;
	PP_RETVAL( i2_index );
