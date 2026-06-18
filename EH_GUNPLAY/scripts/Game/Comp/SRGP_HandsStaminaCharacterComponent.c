class SRGP_HandsStaminaCharacterComponentClass : ScriptComponentClass
{
}

class SRGP_HandsStaminaCharacterComponent : ScriptComponent
{
	protected IEntity m_Owner;
	
	protected float m_fHandsStamina;
	protected float m_fWeaponWeight;
	
	[Attribute("0.1", uiwidget: UIWidgets.Auto, desc: "Drain rate on fixed frame", params: "0 1")]
	protected float m_fHSDrain;
	
	[Attribute("0.1", uiwidget: UIWidgets.Auto, desc: "Regen rate on fixed frame", params: "0 1")]
	protected float m_fHSRegen;
	
	[Attribute("0 0 20 3", uiwidget: UIWidgets.CurveDialog, desc: "Relation of weapon weight to stamina drain", category: "Settings", params: "20 3 0 0")]
	protected ref Curve m_cWeaponWeightDrainCurve;
	
	[Attribute("0 2 20 0.2", uiwidget: UIWidgets.CurveDialog, desc: "Relation of weapon weight to stamina regen", category: "Settings", params: "20 2 0 0")]
	protected ref Curve m_cWeaponWeightRegenCurve;

	override void OnPostInit(IEntity owner)
	{
		m_Owner = owner;
		m_fHandsStamina = 100;
		SetEventMask(owner, EntityEvent.FIXEDFRAME);
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
    {
		if (SRGP_IsInADS(owner) && m_fHandsStamina > 0 && SRGP_GetStance(owner) != 2 && SRGP_IsWeaponDeployed(owner) == 0)
		{
			m_fHandsStamina = DrainTick(owner, m_fHandsStamina);
		}
		else if ((!SRGP_IsInADS(owner) && m_fHandsStamina < 100) || SRGP_GetStance(owner) == 2 || SRGP_IsWeaponDeployed(owner) >= 1)
			m_fHandsStamina = RegenTick(owner, m_fHandsStamina);
	}
	
	float GetStamina()
	{
		return m_fHandsStamina;
	}
	
	protected float RegenTick(IEntity owner, float stamina)
	{
		float weight = SRGP_GetWeaponWeight(owner);
		float weightFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		weight,
		m_cWeaponWeightRegenCurve)[1];
		Print(weight);
		Print(weightFactor);
		
		if (SRGP_GetStance(owner) == 0)
			stamina += m_fHSRegen * weightFactor;
		else if (SRGP_GetStance(owner) == 1)
			stamina += m_fHSRegen * 1.5 * weightFactor;
		else if (SRGP_GetStance(owner) == 2)
			stamina += m_fHSRegen * 2 * weightFactor;
		stamina = Math.Round(stamina * Math.Pow(10, 3)) / Math.Pow(10, 3);
		if (stamina > 100)
			stamina = 100;
		return stamina;
	}
	
	protected float DrainTick(IEntity owner, float stamina)
	{
		float weight = SRGP_GetWeaponWeight(owner);
		float weightFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		weight,
		m_cWeaponWeightDrainCurve)[1];
		Print(weight);
		Print(weightFactor);
		
		if (SRGP_GetStance(owner) == 0)
			stamina -= m_fHSDrain * weightFactor;
		else if (SRGP_GetStance(owner) == 1)
			stamina -= m_fHSDrain * 0.5  * weightFactor;
		stamina = Math.Round(stamina * Math.Pow(10, 3)) / Math.Pow(10, 3);
		if (stamina < 0)
		stamina = 0;
		return stamina;
	}
	
	bool SRGP_IsInADS(IEntity owner)
	{
	    CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return false;
   		return cc.IsWeaponADS();
	}
	
	// returns int state (0 - not deployed, 1 - on obstacle, 2 - bipod)
	int SRGP_IsWeaponDeployed(IEntity owner)
	{
	    CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return 0;
		if (cc.GetIsWeaponDeployedBipod())
			return 2;
   		else if (cc.GetIsWeaponDeployed())
			return 1;
		return 0;
	}
	
	int SRGP_GetStance(IEntity owner)
	{
		CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return 0;
		return cc.GetStance();
	}
	
	float SRGP_GetWeaponWeight(IEntity owner)
	{
		BaseWeaponManagerComponent wm = BaseWeaponManagerComponent.Cast(owner.FindComponent(BaseWeaponManagerComponent));
		if (!wm)
			return 0;
		BaseWeaponComponent wc = wm.GetCurrentWeapon();
		if (!wc)
			return 0;
		IEntity weaponEntity = wc.GetOwner();
		if (!weaponEntity)
			return 0;
		BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(weaponEntity.FindComponent(BaseInventoryStorageComponent));
	    if (storage)
	    {
        float weight = storage.GetTotalWeight();
        if (weight > 0)
            return weight;
    	}
		// in case if something went wrong, but weapon in hands :)
		return 3;
	}
	
}