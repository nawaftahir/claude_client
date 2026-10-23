// QuatMultiply body; pp_in1, pp_in2, pp_out params
	pp_out[0] = pp_in1[0] * pp_in2[3] + pp_in1[3] * pp_in2[0] + pp_in1[2] * pp_in2[1] - pp_in1[1] * pp_in2[2];
	pp_out[1] = pp_in1[1] * pp_in2[3] - pp_in1[2] * pp_in2[0] + pp_in1[3] * pp_in2[1] + pp_in1[0] * pp_in2[2];
	pp_out[2] = pp_in1[2] * pp_in2[3] + pp_in1[1] * pp_in2[0] - pp_in1[0] * pp_in2[1] + pp_in1[3] * pp_in2[2];
	pp_out[3] = pp_in1[3] * pp_in2[3] - pp_in1[0] * pp_in2[0] - pp_in1[1] * pp_in2[1] - pp_in1[2] * pp_in2[2];
