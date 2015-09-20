#ifndef H_GETTRIGCNT
#define H_GETTRIGCNT

int simple_GetTrigCnt(int packetLength)
{
	return (packetLength - 26) / 2;
}

int full_reduction_GetTrigCnt(int packetLength)
{
	return (packetLength - 28) * 8 / 18;
}

#endif
