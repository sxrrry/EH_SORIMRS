class SRGP_Utils
{
	static bool SRGP_IsInADS(IEntity owner)
	{
	    CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return false;
   		return cc.IsWeaponADS();
		
	}
	
	static bool SRGP_IsFreeloking(IEntity owner)
	{
	    CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return false;
   		return cc.IsFreeLookEnabled();
	}
	
	static float SRGP_NormalizeAngle(float angle)
	{
		if (angle > 180) 
		{
			angle -= 360;
			return angle;
		}
		if (angle < -180) 
		{
			angle += 360;
			return angle;
		}
		return angle;
	}
	
	// returns int state (0 - not deployed, 1 - on obstacle, 2 - bipod)
	static int SRGP_IsWeaponDeployed(IEntity owner)
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
	
	static int SRGP_GetStance(IEntity owner)
	{
		CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return 0;
		return cc.GetStance();
	}
	
	static float SRGP_GetWeaponWeight(IEntity owner)
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