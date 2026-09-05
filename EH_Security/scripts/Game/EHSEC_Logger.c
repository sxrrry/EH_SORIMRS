class EHSEC_Logger
{
	static void EHSEC_WriteLog(string log, string adminName)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("[EHSEC] - ADMIN %1 | %2", adminName, log);
	}
	
	static void EHSEC_WriteLog(string log, string adminName, string userName)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("[EHSEC] - ADMIN %1 | %2 %3", adminName, log, userName);
	}
}