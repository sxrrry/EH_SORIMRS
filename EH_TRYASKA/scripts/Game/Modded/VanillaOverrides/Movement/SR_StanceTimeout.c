sealed class SR_StanceTimeout : GameSystem
{
	int m_currentStance = 0;
	
	bool m_bCanChangePos = true;
	
	[Attribute("33", UIWidgets.Auto, "Coefficient which calculated by (charTotWeight * this (min/maxed to WEIGHT_MIN_TIMEOUT/WEIGHT_MAX_TIMEOUT))", params: "1 200")]
	int WEIGHT_COEFFICIENT;
	[Attribute("500", UIWidgets.Auto, "Minimal timeout", params: "1 inf")]
	int WEIGHT_MIN_TIMEOUT;
	[Attribute("2500", UIWidgets.Auto, "Maximal timeout", params: "1 inf")]
	int WEIGHT_MAX_TIMEOUT;
	
	static const int PRONE_STANCE = 2;
	
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo
			.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Client)
			.AddPoint(WorldSystemPoint.FixedFrame);
	}
	
	override protected void OnUpdate(WorldSystemPoint point)
	{
		super.OnUpdate(point);
		PreventExploiting();
	}

	private void PreventExploiting()
	{
		IEntity player = GetGame().GetPlayerController().GetControlledEntity();
		if (!player)
			return;
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		if (!characterController)
			return;
		SCR_CharacterInventoryStorageComponent charInvStrgComp = SCR_CharacterInventoryStorageComponent.Cast(player.FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!charInvStrgComp)
			return;
		
		if (m_bCanChangePos == false)
		{
			characterController.ForceStance(PRONE_STANCE);
			return;
		}
		
		if (characterController.IsChangingStance() && m_bCanChangePos && characterController.GetStance() == 2)
		{
			Off();
			GetGame().GetCallqueue().CallLater(On, TimeoutByWeight(charInvStrgComp));
		}
	}
	
	private int TimeoutByWeight(SCR_CharacterInventoryStorageComponent charInvStrgComp)
	{
		return Math.Min(Math.Max(charInvStrgComp.GetTotalWeight() * WEIGHT_COEFFICIENT, WEIGHT_MIN_TIMEOUT), WEIGHT_MAX_TIMEOUT);
	}
	
	private void On()
	{
		m_bCanChangePos = true;
	}
	
	private void Off()
	{
		m_bCanChangePos = false;
	}
}