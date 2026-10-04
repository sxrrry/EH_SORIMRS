modded class SCR_EditorModeEntity
{
	int GetPlayerId()
	{
		if (m_EditorManager)
	    	return m_EditorManager.GetPlayerID();
		return 0;
	}
	
	override void DeactivateModeServer()
	{
		if (Replication.IsServer())
		{
			int playerId = m_EditorManager.GetPlayerID();

			PlayerManager playerManager =  PlayerManager.Cast(GetGame().GetPlayerManager());
			
			if (playerManager)
			{
			    string admName = playerManager.GetPlayerName(playerId);
				EHSEC_Logger.EHSEC_WriteLog("Closed gamemaster mode", admName);
			}
		}
		Event_OnDeactivateServer.Invoke();
	}
}

modded class SCR_DoorUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{		
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		if (!Replication.IsServer())
			return;
		
		SCR_EditorModeEntity editor = SCR_EditorModeEntity.Cast(pUserEntity);
		PlayerManager playerManager =  PlayerManager.Cast(GetGame().GetPlayerManager());
		
		if (editor && playerManager)
		{
		    string admName = playerManager.GetPlayerName(editor.GetPlayerId());
			EHSEC_Logger.EHSEC_WriteLog("Opened/closed door with admin action", admName);
		}
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
modded class SCR_HealEntitiesContextAction
{
	override void Perform(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition)
	{
		super.Perform(selectedEntity, cursorWorldPosition);
		
		//SCR_EditorModeEntity editor = SCR_EditorModeEntity.GetInstance();
		PlayerManager playerManager =  PlayerManager.Cast(GetGame().GetPlayerManager());
		
		if (playerManager)
		{
		    //string admName = playerManager.GetPlayerName(editor.GetPlayerId());
			string admName = "unknown"; // need to find out way to get editor entity properly
		    string userName = playerManager.GetPlayerName(selectedEntity.GetPlayerID());
			EHSEC_Logger.EHSEC_WriteLog("Healed player", admName, userName);
		}
	}
}

#ifdef ECHO_LOBBY_LOADED
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
modded class EH_RespawnCharacterContextAction
{
	override void Perform(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition)
	{
		super.Perform(selectedEntity, cursorWorldPosition);
		
		//SCR_EditorModeEntity editor = SCR_EditorModeEntity.GetInstance();
		PlayerManager playerManager =  PlayerManager.Cast(GetGame().GetPlayerManager());
		
		if (playerManager)
		{
		    //string admName = playerManager.GetPlayerName(editor.GetPlayerId());
			string admName = "unknown"; // need to find out way to get editor entity properly
		    string userName = playerManager.GetPlayerName(selectedEntity.GetPlayerID());
			EHSEC_Logger.EHSEC_WriteLog("Respawned player", admName, userName);
		}
	}

}
#endif